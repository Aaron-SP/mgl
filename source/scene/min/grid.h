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
#include <min/utility.h>
#include <numeric>
#include <stdexcept>
#include <vector>

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
        auto size = _shapes.size();
        for (K i = 0; i < size; i++)
        {
            // Get the surrounding overlapping neighbor cells
            const auto &b = _shapes[i];
            auto neighbors = vec<T>::grid_overlap(_root.get_min(), _cell_extent, _scale, b.get_min(), b.get_max());

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
                if (!_flags.get(a, b))
                {
                    // Get the two cells
                    const shape<T, vec> &a_shape = _shapes[a];
                    const shape<T, vec> &b_shape = _shapes[b];
                    if (intersect(a_shape, b_shape))
                    {
                        // Add the test to flags to avoid retesting
                        _flags.set_on(a, b);
                        hits.emplace_back(a, b);
                    }
                }
            }
        }
    }
    inline void set_scale(const std::vector<shape<T, vec>> &shapes)
    {
        // Find the largest object in the collection
        auto size = shapes.size();
        if (size > 0)
        {
            // square distance across the extent
            T max = shapes[0].square_size();

            // Calculate the maximum square distance across each extent
            for (K i = 1; i < size; i++)
            {
                // Update the maximum
                T d2 = shapes[i].square_size();
                if (d2 > max)
                {
                    max = d2;
                }
            }

            // Calculate the world cell extent
            T d2 = std::sqrt(_root.square_size());
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
        auto size = shapes.size();
        std::vector<size_t> index(size);
        std::iota(index.begin(), index.end(), 0);

        // lambda function to create sorted array indices
        std::sort(index.begin(), index.end(),
                  [this, &shapes](size_t a, size_t b) {
                      size_t a_key = this->get_key(shapes[a].get_center());
                      size_t b_key = this->get_key(shapes[b].get_center());
                      return a_key < b_key;
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
        size_t key = this->get_key(point);

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
        std::vector<size_t> map = sort(shapes);

        // Rebuild the grid after changing the contents
        build();

        // Return this map so we can interpret the collision indices
        return map;
    }
    inline const std::vector<K> &point_inside(const vec<T> &point) const
    {
        // Get the keys on the cell node
        return this->get_node(point).get_keys();
    }
};
}

#endif