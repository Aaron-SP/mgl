/* Copyright [2013-2018] [Aaron Springstroh, Minimal Graphics Library]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#ifndef __GRID__
#define __GRID__

#include <cmath>
#include <min/bit_flag.h>
#include <min/intersect.h>
#include <min/ray.h>
#include <min/sort.h>
#include <min/utility.h>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

// The shape class must fulfill the following interface to be inserted into the spatial structure
// shape.get_center()
// shape.get_min()
// shape.get_max()
// shape.square_size()
// intersect(shape, shape)

// Forward declaration for grid_node
namespace min
{
template <typename T, typename K, typename L, template <typename> class vec, template <typename, template <typename> class> class cell, template <typename, template <typename> class> class shape>
class grid;
}

namespace min
{

template <typename T, typename K, typename L, template <typename> class vec, template <typename, template <typename> class> class cell, template <typename, template <typename> class> class shape>
class grid_node
{
    friend class grid<T, K, L, vec, cell, shape>;

  private:
    std::vector<K> _keys;
    cell<T, vec> _cell;
    inline void add_key(K key)
    {
        _keys.push_back(key);
    }
    inline void clear()
    {
        // Clear out the cell data
        _keys.clear();
    }

  public:
    grid_node(const cell<T, vec> &c) : _cell(c) {}
    inline const std::vector<K> &get_keys() const
    {
        return _keys;
    }
    inline const cell<T, vec> &get_cell() const
    {
        return _cell;
    }
    inline bool point_inside(const vec<T> &point) const
    {
        // The grid_key is assumed to be a box, so can't use _root.get_cell().point_inside()!
        return point.inside(_cell.get_min(), _cell.get_max());
    }
    inline K size() const
    {
        return _keys.size();
    }
};

template <typename T, typename K, typename L, template <typename> class vec, template <typename, template <typename> class> class cell, template <typename, template <typename> class> class shape>
class grid
{
  private:
    std::vector<shape<T, vec>> _shapes;
    std::vector<grid_node<T, K, L, vec, cell, shape>> _cells;
    std::vector<K> _index_map;
    std::vector<size_t> _key_cache;
    std::vector<K> _sort_copy;
    mutable bit_flag<K, L> _flags;
    mutable std::vector<std::pair<K, K>> _hits;
    mutable std::vector<std::pair<K, vec<T>>> _ray_hits;
    cell<T, vec> _root;
    vec<T> _lower_bound;
    vec<T> _upper_bound;
    K _scale;
    vec<T> _cell_extent;
    size_t _flag_size;

    inline void build()
    {
        // Clears out the entire grid cell buffer
        _cells.clear();

        // Calculate the grid cells from the root cell
        const auto cells = _root.grid(_scale);
        _cells.reserve(cells.size());
        for (const auto &c : cells)
        {
            _cells.emplace_back(cell<T, vec>(c.first, c.second));
        }

        // Calculate intersections of sub cell with list of shapes
        const auto size = _shapes.size();
        for (K i = 0; i < size; i++)
        {
            // Get the surrounding overlapping neighbor cells
            const auto &b = _shapes[i];
            const auto overlap = vec<T>::grid_overlap(_root.get_min(), _cell_extent, _scale, b.get_min(), b.get_max());

            // All surrounding neighbors overlap
            for (auto &n : overlap)
            {
                // Assign keys to cell
                _cells[n].add_key(i);
            }
        }

        // Reset the flag size if size changes
        if (size > _flag_size)
        {
            // cache the flag size
            _flag_size = size;

            // Create flag buffer
            _flags = bit_flag<K, L>(size, size);
        }
        else
        {
            // clear the flag buffer
            _flags.clear();
        }
    }
    inline size_t get_key(const vec<T> &point) const
    {
        // This must be guaranteed to be safe by callers
        return vec<T>::grid_key(_root.get_min(), _cell_extent, _scale, point);
    }
    inline void get_overlap(const size_t key) const
    {
        // Get the cell from the next key
        const grid_node<T, K, L, vec, cell, shape> &node = _cells[key];

        // Get all keys in this cell
        const std::vector<K> &keys = node.get_keys();
        const K size = keys.size();
        for (K i = 0; i < size; i++)
        {
            if (!_flags.get_set_on(keys[i], 0))
            {
                _hits.emplace_back(keys[i], 0);
            }
        }
    }
    inline void get_pairs(const grid_node<T, K, L, vec, cell, shape> &node) const
    {
        // Perform an N^2-N intersection test for all shapes in this cell
        const std::vector<K> &keys = node.get_keys();
        const K size = keys.size();
        for (K i = 0; i < size; i++)
        {
            for (K j = i + 1; j < size; j++)
            {
                // See if this collision has already been found; prefer a < b
                K a = keys[i];
                K b = keys[j];
                if (a > b)
                {
                    a = keys[j];
                    b = keys[i];
                }

                // Add the test to flags to avoid retesting
                if (!_flags.get_set_on(a, b))
                {
                    // Get the two cells
                    const shape<T, vec> &a_shape = _shapes[a];
                    const shape<T, vec> &b_shape = _shapes[b];
                    if (intersect(a_shape, b_shape))
                    {
                        _hits.emplace_back(a, b);
                    }
                }
            }
        }
    }
    inline void get_ray_intersect(const grid_node<T, K, L, vec, cell, shape> &node, const ray<T, vec> &r) const
    {
        // Perform an N intersection test for all shapes in this cell against the ray
        const std::vector<K> &keys = node.get_keys();
        const K size = keys.size();
        vec<T> point;
        for (K i = 0; i < size; i++)
        {
            const K key = keys[i];
            const shape<T, vec> &s = _shapes[key];
            if (intersect(s, r, point))
            {
                _ray_hits.emplace_back(key, point);
            }
        }
    }
    inline void set_scale(const std::vector<shape<T, vec>> &shapes)
    {
        // Find the largest object in the collection
        const auto size = shapes.size();
        if (size > 0)
        {
            // square distance across the extent
            T max = shapes[0].square_size();

            // Calculate the maximum square distance across each extent
            for (K i = 1; i < size; i++)
            {
                // Update the maximum
                const T d2 = shapes[i].square_size();
                if (d2 > max)
                {
                    max = d2;
                }
            }

            // Calculate the world cell extent
            const T d2 = std::sqrt(_root.square_size());
            max = std::sqrt(max);

            // Calculate the scale
            // to be the world cell extent / max object extent
            // Calculate 2^n, (28.284/8.48) == 4, 2^4 = 16
            _scale = 0x1 << static_cast<K>(std::ceil(std::log2(d2 / max)));

            // Optimize the grid scale if there are two many items
            _scale = std::min(_scale, static_cast<K>(std::ceil(std::cbrt(size))));

            // Set the grid cell extent
            _cell_extent = _root.get_extent() / _scale;
        }
    }
    inline void sort(const std::vector<shape<T, vec>> &shapes)
    {
        // Create index vector to sort 0 to N
        const auto size = shapes.size();
        _index_map.resize(size);
        std::iota(_index_map.begin(), _index_map.end(), 0);

        // Cache key calculation for sorting speed up
        _key_cache.resize(size);
        for (size_t i = 0; i < size; i++)
        {
            _key_cache[i] = this->get_key(shapes[i].get_center());
        }

        // use uint radix sort for sorting keys
        // lambda function to create sorted array indices based on grid key
        uint_sort<K>(_index_map, _sort_copy, [this](const K a) {
            return this->_key_cache[a];
        });

        // Iterate over sorted indices and store sorted shapes
        _shapes.clear();
        _shapes.reserve(size);
        for (auto &i : _index_map)
        {
            _shapes.emplace_back(shapes[i]);
        }
    }

  public:
    grid(const cell<T, vec> &c)
        : _root(c),
          _lower_bound(_root.get_min() + var<T>::TOL_PHYS_EDGE),
          _upper_bound(_root.get_max() - var<T>::TOL_PHYS_EDGE),
          _scale(0), _flag_size(0) {}

    inline vec<T> clamp_bounds(const vec<T> &point) const
    {
        return vec<T>(point).clamp(_lower_bound, _upper_bound);
    }
    inline const vec<T> &get_lower_bound() const
    {
        return _lower_bound;
    }
    inline const vec<T> &get_upper_bound() const
    {
        return _upper_bound;
    }
    inline const grid_node<T, K, L, vec, cell, shape> &get_node(const vec<T> &point) const
    {
        // This function computes the grid location code
        const size_t key = get_key(point);

        // Return the cell node
        return _cells[key];
    }
    inline void resize(const cell<T, vec> &c)
    {
        _root = c;
        _lower_bound = _root.get_min() + var<T>::TOL_PHYS_EDGE;
        _upper_bound = _root.get_max() - var<T>::TOL_PHYS_EDGE;
    }
    inline void check_size(const std::vector<shape<T, vec>> &shapes) const
    {
        // Check size of the number of objects to insert into grid
        if (shapes.size() > std::numeric_limits<K>::max() - 1)
        {
            throw std::runtime_error("grid: too many objects to insert, max supported is " + std::to_string(std::numeric_limits<K>::max()));
        }
    }
    inline const std::vector<std::pair<K, K>> &get_collisions() const
    {
        // Clear out the old collision sets and vectors
        _flags.clear();

        // Output vector
        _hits.clear();
        _hits.reserve(_shapes.size());

        // Calculate the intersection pairs for every cell
        for (const auto &node : _cells)
        {
            get_pairs(node);
        }

        // Return the collision list
        return _hits;
    }
    inline const std::vector<std::pair<K, K>> &get_collisions(const vec<T> &point) const
    {
        // Clear out the old collision sets and vectors
        _flags.clear();

        // Clamp point into world bounds
        const vec<T> clamped = clamp_bounds(point);

        // Output vector
        _hits.clear();
        _hits.reserve(_shapes.size());

        // get the cell from the point
        const grid_node<T, K, L, vec, cell, shape> &node = get_node(clamped);

        // Get the intersecting pairs in this cell
        get_pairs(node);

        // Return the collision list
        return _hits;
    }
    inline const std::vector<std::pair<K, vec<T>>> &get_collisions(const ray<T, vec> &r) const
    {
        // Output vector
        _ray_hits.clear();
        _ray_hits.reserve(_shapes.size());

        // Check if grid is not built yet
        if (_cells.size() == 0)
        {
            return _ray_hits;
        }

        // Get the cell from the ray origin
        // this will check if ray originates within the grid
        const grid_node<T, K, L, vec, cell, shape> &node = get_node(r.get_origin());

        // Get the intersecting pairs in this cell
        get_ray_intersect(node, r);

        // If we found shapes return early
        if (_ray_hits.size() > 0)
        {
            return _ray_hits;
        }

        // This function computes the ray lengths along the grid cell
        auto grid_ray = vec<T>::grid_ray(_root.get_min(), _cell_extent, r.get_origin(), r.get_direction(), r.get_inverse());

        // Get the grid cell of ray origin
        auto grid_index = vec<T>::grid_index(_root.get_min(), _cell_extent, r.get_origin());

        // While we didn't hit anything in the grid
        bool bad_flag = false;
        while (_ray_hits.size() == 0 && !bad_flag)
        {
            // Find the next cell along the ray to test, bad flag signals that we have hit the last valid cell
            const size_t next = vec<T>::grid_ray_next(grid_index, grid_ray, bad_flag, _scale);

            // check to see if we are still inside the grid
            if (bad_flag || next >= _cells.size())
            {
                return _ray_hits;
            }

            // Get the cell from the next key
            const grid_node<T, K, L, vec, cell, shape> &node = _cells[next];

            // Get the intersecting pairs in this cell
            get_ray_intersect(node, r);
        }

        // Return the collision list
        return _ray_hits;
    }
    inline const std::vector<K> &get_index_map() const
    {
        return _index_map;
    }
    inline K get_scale() const
    {
        return _scale;
    }
    inline const std::vector<std::pair<K, K>> &get_overlap(const shape<T, vec> &overlap) const
    {
        // Check if grid is not built yet
        if (_cells.size() == 0)
        {
            return _hits;
        }

        // Clear out the old collision sets and vectors
        _flags.clear();

        // Output vector
        _hits.clear();
        _hits.reserve(_shapes.size());

        // Callback function
        const auto f = [this](const size_t key) {
            // Get the intersecting pairs in this cell
            this->get_overlap(key);
        };

        // Clamp overlap min and max to world edges
        const vec<T> min = clamp_bounds(overlap.get_min());
        const vec<T> max = clamp_bounds(overlap.get_max());

        // Do callback on range of cells in overlapping region
        vec<T>::grid_range(_root.get_min(), _cell_extent, _scale, min, max, f);

        // Return the overlap list
        return _hits;
    }
    inline const std::vector<shape<T, vec>> &get_shapes()
    {
        return _shapes;
    }
    inline bool inside(const vec<T> &point) const
    {
        return _root.point_inside(point);
    }
    inline void insert(const std::vector<shape<T, vec>> &shapes)
    {
        // Set the grid scale
        set_scale(shapes);

        // Sort the shape array and store copy
        sort(shapes);

        // Rebuild the grid after changing the contents
        build();
    }
    inline void insert(const std::vector<shape<T, vec>> &shapes, const K scale)
    {
        // Set the grid scale
        _scale = scale;

        // Set the grid cell extent
        _cell_extent = _root.get_extent() / _scale;

        // Sort the shape array and store copy
        sort(shapes);

        // Rebuild the grid after changing the contents
        build();
    }
    inline void insert_no_sort(const std::vector<shape<T, vec>> &shapes)
    {
        // Set the grid scale
        set_scale(shapes);

        // Insert shapes without sorting
        _shapes.clear();
        _shapes.insert(_shapes.end(), shapes.begin(), shapes.end());

        // Rebuild the grid after changing the contents
        build();
    }
    inline const std::vector<K> &point_inside(const vec<T> &point) const
    {
        // Clamp point into world bounds
        const vec<T> clamped = clamp_bounds(point);

        // Get the keys on the cell node
        return get_node(clamped).get_keys();
    }
};
}

#endif
