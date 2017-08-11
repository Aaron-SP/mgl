/* Copyright [2013-2016] [Aaron Springstroh, Minimal Graphics Library]

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
#include <min/intersect.h>
#include <min/ray.h>
#include <min/utility.h>
#include <numeric>
#include <stdexcept>
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
    mutable bit_flag<K, L> _flags;
    mutable std::vector<std::pair<K, K>> _hits;
    mutable std::vector<std::pair<K, vec<T>>> _ray_hits;
    cell<T, vec> _root;
    K _scale;
    vec<T> _cell_extent;

    inline void build()
    {
        // Clears out the entire grid cell buffer
        _cells.clear();

        // Calculate the grid cells from the root cell
        auto cells = _root.grid(_scale);
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
            const auto neighbors = vec<T>::grid_overlap(_root.get_min(), _cell_extent, _scale, b.get_min(), b.get_max());

            // All surrounding neighbors overlap
            for (auto &n : neighbors)
            {
                // Assign keys to cell
                _cells[n].add_key(i);
            }
        }

        // Preallocate cache dimensions
        _flags = bit_flag<K, L>(size, size);
    }
    inline size_t get_key(const vec<T> &point) const
    {
        // !This will crash if point is not inside the world cell!
        if (!_root.point_inside(point))
        {
            throw std::runtime_error("grid.get_key(): point is not inside the world cell");
        }

        return vec<T>::grid_key(_root.get_min(), _cell_extent, _scale, point);
    }
    inline void get_pairs(const grid_node<T, K, L, vec, cell, shape> &node, std::vector<std::pair<K, K>> &hits) const
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
                        hits.emplace_back(a, b);
                    }
                }
            }
        }
    }
    inline void get_ray_intersect(const grid_node<T, K, L, vec, cell, shape> &node, std::vector<std::pair<K, vec<T>>> &hits, const ray<T, vec> &r) const
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
                hits.emplace_back(key, point);
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
            _scale = 0x1 << (K)std::ceil(std::log2(d2 / max));

            // Optimize the grid scale if there are two many items
            _scale = std::min(_scale, (K)std::ceil(std::cbrt(size)));

            // Set the grid cell extent
            _cell_extent = _root.get_extent() / _scale;
        }
    }
    inline std::vector<size_t> sort(const std::vector<shape<T, vec>> &shapes)
    {
        // Create index vector to sort 0 to N
        const auto size = shapes.size();
        std::vector<size_t> index(size);
        std::iota(index.begin(), index.end(), 0);

        // Cache key calculation for sorting speed up
        std::vector<size_t> key_cache(size);
        for (size_t i = 0; i < size; i++)
        {
            key_cache[i] = this->get_key(shapes[i].get_center());
        }

        // use uint radix sort for sorting keys
        // lambda function to create sorted array indices based on grid key
        uint_sort<size_t>(index, [this, &key_cache](const size_t a) {
            return key_cache[a];
        });

        // Iterate over sorted indices and store sorted shapes
        _shapes.clear();
        _shapes.reserve(size);
        for (auto &i : index)
        {
            _shapes.emplace_back(shapes[i]);
        }

        // Return the index list so we can interpret the collision indices
        return index;
    }

  public:
    grid(const cell<T, vec> &c) : _root(c), _scale(0)
    {
        // Check that the grid max >= min
        const vec<T> &min = _root.get_min();
        const vec<T> &max = _root.get_max();
        if (min >= max)
        {
            throw std::runtime_error("grid(): invalid grid dimensions");
        }
    }
    inline const grid_node<T, K, L, vec, cell, shape> &get_node(const vec<T> &point) const
    {
        // This function computes the grid location code
        const size_t key = this->get_key(point);

        // Return the cell node
        return _cells[key];
    }
    inline const std::vector<shape<T, vec>> &get_shapes()
    {
        return _shapes;
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
            get_pairs(node, _hits);
        }

        // Return the collision list
        return _hits;
    }
    inline const std::vector<std::pair<K, K>> &get_collisions(const vec<T> &point) const
    {
        // Clear out the old collision sets and vectors
        _flags.clear();

        // Output vector
        _hits.clear();
        _hits.reserve(_shapes.size());

        // get the cell from the point
        const grid_node<T, K, L, vec, cell, shape> &node = get_node(point);

        // Get the intersecting pairs in this cell
        get_pairs(node, _hits);

        // Return the collision list
        return _hits;
    }
    inline const std::vector<std::pair<K, vec<T>>> &get_collisions(const ray<T, vec> &r) const
    {
        // Output vector
        _ray_hits.clear();

        // get the cell from the ray origin
        // this will check if ray originates within the grid
        const grid_node<T, K, L, vec, cell, shape> &node = get_node(r.get_origin());

        // Get the intersecting pairs in this cell
        get_ray_intersect(node, _ray_hits, r);

        // If we found shapes return early
        if (_ray_hits.size() > 0)
        {
            return _ray_hits;
        }

        // This function computes the ray lengths along the grid cell
        auto grid_ray = vec<T>::grid_ray(_cell_extent, r.get_origin(), r.get_direction(), r.get_inverse());

        // Get the grid cell of ray origin
        auto grid_index = vec<T>::grid_index(_root.get_min(), _cell_extent, r.get_origin());

        bool bad_flag = false;

        // while we didn't hit anything in the grid
        while (_ray_hits.size() == 0 && !bad_flag)
        {
            // Find the next cell along the ray to test, bad flag signals that we have hit the last valid cell
            const size_t next = vec<T>::grid_ray_next(grid_index, grid_ray, bad_flag, _scale);

            // check to see if we are still inside the grid
            if (next >= _cells.size())
            {
                return _ray_hits;
            }

            // get the cell from the next key
            const grid_node<T, K, L, vec, cell, shape> &node = _cells[next];

            // Get the intersecting pairs in this cell
            get_ray_intersect(node, _ray_hits, r);
        }

        // Return the collision list
        return _ray_hits;
    }
    inline K get_scale() const
    {
        return _scale;
    }
    inline std::vector<size_t> insert(const std::vector<shape<T, vec>> &shapes)
    {
        // Check size of the number of objects to insert into grid
        if (shapes.size() > std::numeric_limits<K>::max() - 1)
        {
            throw std::runtime_error("grid.insert(): too many objects to insert, max supported is " + std::to_string(std::numeric_limits<K>::max()));
        }

        // Set the grid scale
        set_scale(shapes);

        // Sort the shape array and store copy
        const std::vector<size_t> map = sort(shapes);

        // Rebuild the grid after changing the contents
        build();

        // Return this map so we can interpret the collision indices
        return map;
    }
    inline void insert_no_sort(const std::vector<shape<T, vec>> &shapes)
    {
        // Check size of the number of objects to insert into grid
        if (shapes.size() > std::numeric_limits<K>::max() - 1)
        {
            throw std::runtime_error("grid.insert(): too many objects to insert, max supported is " + std::to_string(std::numeric_limits<K>::max()));
        }

        // Set the grid scale
        set_scale(shapes);

        // insert shapes without sorting
        _shapes = shapes;

        // Rebuild the grid after changing the contents
        build();
    }
    inline const std::vector<K> &point_inside(const vec<T> &point) const
    {
        // Get the keys on the cell node
        return this->get_node(point).get_keys();
    }
};
}

#endif
