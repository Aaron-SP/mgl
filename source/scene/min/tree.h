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
#ifndef __TREE__
#define __TREE__

#include <cmath>
#include <min/intersect.h>
#include <min/utility.h>
#include <numeric>
#include <vector>

// Forward declaration for tree_node
namespace min
{
template <typename T, typename K, typename L, template <typename> class vec, template <typename, template <typename> class> class cell, template <typename, template <typename> class> class bound>
class tree;
}

namespace min
{

template <typename T, typename K, typename L, template <typename> class vec, template <typename, template <typename> class> class cell, template <typename, template <typename> class> class bound>
class tree_node
{
    friend class tree<T, K, L, vec, cell, bound>;

  private:
    std::vector<tree_node<T, K, L, vec, cell, bound>> _child;
    std::vector<K> _keys;
    cell<T, vec> _cell;

    inline void add_key(K key)
    {
        _keys.push_back(key);
    }
    inline void clear()
    {
        // Clear out the node data
        _child.clear();
        _keys.clear();
    }
    inline std::vector<tree_node<T, K, L, vec, cell, bound>> &get_children()
    {
        return _child;
    }
    inline std::vector<K> &get_keys()
    {
        return _keys;
    }

  public:
    tree_node(const cell<T, vec> &c) : _cell(c) {}
    inline const std::vector<tree_node<T, K, L, vec, cell, bound>> &get_children() const
    {
        return _child;
    }
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
        return _cell.point_inside(point);
    }
    inline K size() const
    {
        return _keys.size();
    }
};

template <typename T, typename K, typename L, template <typename> class vec, template <typename, template <typename> class> class cell, template <typename, template <typename> class> class bound>
class tree
{
  private:
    std::vector<bound<T, vec>> _bounds;
    mutable bit_flag<K, L> _flags;
    mutable std::vector<std::pair<K, K>> _hits;
    tree_node<T, K, L, vec, cell, bound> _root;
    K _depth;
    K _scale;
    vec<T> _cell_extent;
    bool _depth_override;

    void build(tree_node<T, K, L, vec, cell, bound> &node, K depth)
    {
        // We are at a leaf node and we have hit the stopping criteria
        if (depth == 0)
        {
            return;
        }

        // Calculate sub cell regions in this node, and set the node children cells
        auto &children = node.get_children();
        auto sub_cell = node.get_cell().subdivide();
        for (auto &sc : sub_cell)
        {
            children.emplace_back(cell<T, vec>(sc.first, sc.second));
        }

        // Calculate intersections of sub cell with list of bounds
        const std::vector<K> &keys = node.get_keys();
        for (const auto key : keys)
        {
            // Get bound in main tree buffer with key, extent and node center
            const bound<T, vec> &b = _bounds[key];
            const vec<T> &min = b.get_min();
            const vec<T> &max = b.get_max();
            const vec<T> &center = node.get_cell().get_center();

            // Calculate intersection between bound and the node sub cells, sub_over.size() < 8
            std::vector<uint8_t> sub_over = vec<T>::sub_overlap(min, max, center);
            for (const auto &sub : sub_over)
            {
                // Set key for all overlapping sub cells
                children[sub].add_key(key);
            }
        }

        for (auto &child : children)
        {
            // Skip over empty sub cells
            if (child.size() == 0)
                continue;

            // Build all sub cells recursively
            build(child, depth - 1);
        }
    }
    inline void create_keys()
    {
        // Preallocate the key vector and collision cache
        auto size = _bounds.size();

        // Resize the keys vector and fill with increasing series
        std::vector<K> &keys = _root.get_keys();
        keys.resize(size);
        std::iota(keys.begin(), keys.end(), 0);

        // Reserve capacity for collisions and create flags index
        _hits.reserve(size);
        _flags = bit_flag<K, L>(size, size);
    }
    inline size_t get_sorting_key(const vec<T> &point) const
    {
        if (!_root.get_cell().point_inside(point))
        {
            throw std::runtime_error("tree.get_sorting_key(): point is not inside the world cell");
        }

        // Use grid to sort all bounds in tree since it is a global identifier
        return vec<T>::grid_key(_root.get_cell().get_min(), _cell_extent, _scale, point);
    }
    inline void get_pairs(const tree_node<T, K, L, vec, cell, bound> &node) const
    {
        // Perform an N^2-N intersection test for all shapes in this cell
        const std::vector<K> &keys = node.get_keys();
        K size = keys.size();
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
                    const bound<T, vec> &a_bound = _bounds[a];
                    const bound<T, vec> &b_bound = _bounds[b];
                    if (intersect(a_bound, b_bound))
                    {
                        // Add the test to flags to avoid retesting
                        _flags.set_on(a, b);
                        _hits.push_back(std::make_pair(a, b));
                    }
                }
            }
        }
    }
    inline void get_pairs(const tree_node<T, K, L, vec, cell, bound> &node, K depth) const
    {
        // Returns all intersecting key pairs
        // We are at a leaf node and we have hit the stopping criteria
        if (depth == 0)
        {
            // Get the intersecting pairs in this cell
            get_pairs(node);
        }

        // For all child nodes of this node check intersection
        const auto &children = node.get_children();
        for (const auto &child : children)
        {
            // Terminate recursion and test pair
            if (child.size() == 2)
            {
                get_pairs(child);
            }
            // Must have more than one object to be intersecting
            else if (child.size() > 1)
            {
                // Recursively search for intersections in all children
                get_pairs(child, depth - 1);
            }
        }
    }
    inline K optimize_depth(const std::vector<bound<T, vec>> &bounds)
    {
        // Find the largest object in the collection
        auto size = bounds.size();
        if (size > 0)
        {
            if (!_depth_override)
            {
                // square distance across the extent
                T max = bounds[0].square_size();

                // Calculate the maximum square distance across each extent
                for (K i = 1; i < size; i++)
                {
                    // Update the maximum
                    T d2 = bounds[i].square_size();
                    if (d2 > max)
                    {
                        max = d2;
                    }
                }

                // Calculate the world cell extent
                T d2 = std::sqrt(_root.get_cell().square_size());
                max = std::sqrt(max);

                // Calculate the depth of the tree
                _depth = std::ceil(std::log2(d2 / max));
            }

            // Set the grid cell extent 2^depth
            _scale = (0x1 << _depth);
            _cell_extent = _root.get_cell().get_extent() / _scale;
        }

        return _depth;
    }
    inline void sort(const std::vector<bound<T, vec>> &bounds)
    {
        // Create index vector to sort 0 to N
        auto size = bounds.size();
        std::vector<size_t> index(size);
        std::iota(index.begin(), index.end(), 0);

        // lambda function to create sorted array indices
        std::sort(index.begin(), index.end(),
                  [this, &bounds](size_t a, size_t b) {
                      size_t a_key = this->get_sorting_key(bounds[a].get_center());
                      size_t b_key = this->get_sorting_key(bounds[b].get_center());
                      return a_key < b_key;
                  });

        // Iterate over sorted indices and store sorted bounds
        _bounds.clear();
        _bounds.reserve(size);
        for (auto &i : index)
        {
            _bounds.emplace_back(bounds[i]);
        }
    }

  public:
    tree(const cell<T, vec> &c) : _root(c), _depth_override(false)
    {
        // Check that the tree max >= min
        const vec<T> &min = _root.get_cell().get_min();
        const vec<T> &max = _root.get_cell().get_max();
        if (min >= max)
        {
            throw std::runtime_error("tree(): invalid tree root cell dimensions");
        }
    }
    inline const tree_node<T, K, L, vec, cell, bound> &get_node(const vec<T> &point) const
    {
        // This function computes the octree key location code
        // for traversing the octree to the leaf nodes without needing to use recursion

        const vec<T> &min = _root.get_cell().get_min();
        const vec<T> &max = _root.get_cell().get_max();
        if (!_root.get_cell().point_inside(point))
        {
            throw std::runtime_error("tree.get_node(): point is not inside the world cell");
        }

        // Calculate ratio between 0.0 and 1.0
        vec<T> ratio = vec<T>::ratio(min, max, point);

        // Start traversing at the root node
        const tree_node<T, K, L, vec, cell, bound> *child = &_root;

        T middle = 0.5;
        for (K i = 0; i < _depth; i++)
        {
            // Get the key from the point ratio
            uint8_t key = ratio.subdivide_key(middle);

            // Check the calculated key value
            if (key > 7 || key < 0)
            {
                throw std::runtime_error("tree.get_node(): invalid key location code calculated");
            }

            // Go to the next level in the octree
            child = &child->get_children()[key];
            middle *= 0.5;
        }

        // Return the cell node
        return *child;
    }
    inline const std::vector<std::pair<K, K>> &get_collisions() const
    {
        // Clear out the old collision sets and vectors
        _flags.clear();
        _hits.clear();

        // get all intersecting pairs
        get_pairs(_root, _depth);

        // Return the list
        return _hits;
    }
    inline const std::vector<std::pair<K, K>> &get_collisions(const vec<T> &point) const
    {
        // Clear out the old collision sets and vectors
        _flags.clear();
        _hits.clear();

        // get the node from the point
        const tree_node<T, K, L, vec, cell, bound> &node = get_node(point);

        // Get the intersecting pairs in this cell
        get_pairs(node);

        // Return the list
        return _hits;
    }
    inline K get_depth() const
    {
        return _depth;
    }
    inline void insert(const std::vector<bound<T, vec>> &bounds)
    {
        // Check size of the number of objects to insert into grid
        if (bounds.size() > std::numeric_limits<K>::max() - 1)
        {
            throw std::runtime_error("tree(): too many objects to insert, max supported is " + std::to_string(std::numeric_limits<K>::max()));
        }

        // Set the tree depth
        optimize_depth(bounds);

        // Sort bounds by grid key id
        sort(bounds);

        // Clear out the root node
        _root.clear();

        // Create box keys
        create_keys();

        // Rebuild the tree after changing the contents
        build(_root, _depth);
    }
    inline std::vector<K> point_inside(const vec<T> &point) const
    {
        // Get the keys on the leaf node
        return this->get_node(point).get_keys();
    }
    inline void set_depth(K depth)
    {
        // Set the depth and indicate overriding calculated depth
        _depth_override = true;
        _depth = depth;
    }
};
}

#endif