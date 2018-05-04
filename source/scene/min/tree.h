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
#ifndef __TREE__
#define __TREE__

#include <cmath>
#include <min/intersect.h>
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

// Forward declaration for tree_node
namespace min
{
template <typename T, typename K, typename L, template <typename> class vec, template <typename, template <typename> class> class cell, template <typename, template <typename> class> class shape>
class tree;
}

namespace min
{

template <typename T, typename K, typename L, template <typename> class vec, template <typename, template <typename> class> class cell, template <typename, template <typename> class> class shape>
class tree_node
{
    friend class tree<T, K, L, vec, cell, shape>;

  private:
    std::vector<tree_node<T, K, L, vec, cell, shape>> _child;
    std::vector<K> _keys;
    cell<T, vec> _cell;
    mutable std::vector<size_t> _sub_ray_cache;

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
    inline std::vector<tree_node<T, K, L, vec, cell, shape>> &get_children()
    {
        return _child;
    }
    inline std::vector<K> &get_keys()
    {
        return _keys;
    }

  public:
    tree_node(const cell<T, vec> &c) : _cell(c) {}
    inline const std::vector<tree_node<T, K, L, vec, cell, shape>> &get_children() const
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
    inline std::vector<size_t> &get_sub_ray_cache() const
    {
        return _sub_ray_cache;
    }
    inline bool point_inside(const vec<T> &point) const
    {
        // The grid_key is assumed to be a box, so can't use _root.point_inside()!
        return point.inside(_cell.get_min(), _cell.get_max());
    }
    inline K size() const
    {
        return _keys.size();
    }
};

template <typename T, typename K, typename L, template <typename> class vec, template <typename, template <typename> class> class cell, template <typename, template <typename> class> class shape>
class tree
{
  private:
    std::vector<shape<T, vec>> _shapes;
    std::vector<K> _index_map;
    std::vector<size_t> _key_cache;
    std::vector<K> _sort_copy;
    mutable std::vector<uint8_t> _sub_overlap;
    mutable bit_flag<K, L> _flags;
    mutable std::vector<std::pair<K, K>> _hits;
    mutable std::vector<std::pair<K, vec<T>>> _ray_hits;
    tree_node<T, K, L, vec, cell, shape> _root;
    vec<T> _lower_bound;
    vec<T> _upper_bound;
    K _depth;
    K _scale;
    vec<T> _cell_extent;
    bool _depth_override;
    size_t _flag_size;

    inline void build(tree_node<T, K, L, vec, cell, shape> &node, const K depth)
    {
        // We are at a leaf node and we have hit the stopping criteria
        if (depth == 0)
        {
            return;
        }

        // Calculate sub cell regions in this node, and set the node children cells
        auto &children = node.get_children();
        const auto sub_cell = node.get_cell().subdivide();

        // Reserve space for this node
        children.reserve(sub_cell.size());
        for (auto &sc : sub_cell)
        {
            children.emplace_back(cell<T, vec>(sc.first, sc.second));
        }

        // Get this node center
        const vec<T> &center = node.get_cell().get_center();

        // Calculate intersections of sub cell with list of shapes
        const std::vector<K> &keys = node.get_keys();
        for (const auto key : keys)
        {
            // Get shape in main tree buffer with key, extent and node center
            const shape<T, vec> &b = _shapes[key];
            const vec<T> &min = b.get_min();
            const vec<T> &max = b.get_max();

            // Calculate intersection between shape and the node sub cells, sub_over.size() < 8
            vec<T>::sub_overlap(_sub_overlap, min, max, center);
            for (const auto &sub : _sub_overlap)
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
        const auto size = _shapes.size();

        // Resize the keys vector and fill with increasing series
        std::vector<K> &keys = _root.get_keys();
        keys.resize(size);
        std::iota(keys.begin(), keys.end(), 0);

        // Reserve capacity for collisions and create flags index
        _hits.reserve(size);

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
    inline size_t get_sorting_key(const vec<T> &point) const
    {
        // This must be guaranteed to be safe by callers
        // Use grid to sort all shapes in tree since it is a global identifier
        return vec<T>::grid_key(_root.get_cell().get_min(), _cell_extent, _scale, point);
    }
    inline void get_overlap(const tree_node<T, K, L, vec, cell, shape> &node) const
    {
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
    inline void get_overlap(const tree_node<T, K, L, vec, cell, shape> &node, const vec<T> &min, const vec<T> &max, const K depth) const
    {
        // Returns all overlapping keys
        // We are at a leaf node and we have hit the stopping criteria
        if (depth == 0)
        {
            // Get the overlapping keys in this cell
            get_overlap(node);
        }

        // For all child nodes of this node check overlap
        const vec<T> &center = node.get_cell().get_center();

        // Search for all children
        const auto &children = node.get_children();
        for (const auto &child : children)
        {
            // Recursively search for overlap in all children
            if (child.size() > 0)
            {
                // Calculate intersection between overlap shape and the node sub cells, sub_over.size() < 8
                vec<T>::sub_overlap(_sub_overlap, min, max, center);
                for (const auto &sub : _sub_overlap)
                {
                    get_overlap(children[sub], min, max, depth - 1);
                }
            }
        }
    }
    inline void get_pairs(const tree_node<T, K, L, vec, cell, shape> &node) const
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
    inline void get_pairs(const tree_node<T, K, L, vec, cell, shape> &node, const K depth) const
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
    inline void get_ray_intersect(const tree_node<T, K, L, vec, cell, shape> &node, const ray<T, vec> &r, const K depth) const
    {
        // We are at a leaf node and we have hit the stopping criteria
        if (depth == 0)
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

        // Further recursion if we have children
        const auto &children = node.get_children();
        if (children.size() > 0)
        {
            // Get the current node cell
            const cell<T, vec> &c = node.get_cell();

            // For all child nodes intersecting ray
            std::vector<size_t> &keys = node.get_sub_ray_cache();
            vec<T>::subdivide_ray(keys, c.get_min(), c.get_max(), r.get_origin(), r.get_direction(), r.get_inverse());
            for (const size_t k : keys)
            {
                // If we haven't hit anything yet
                if (_ray_hits.size() == 0)
                {
                    get_ray_intersect(children[k], r, depth - 1);
                }
            }
        }
    }
    inline K optimize_depth(const std::vector<shape<T, vec>> &shapes)
    {
        // Find the largest object in the collection
        const auto size = shapes.size();
        if (size > 0)
        {
            if (!_depth_override)
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
                const T d2 = std::sqrt(_root.get_cell().square_size());
                max = std::sqrt(max);

                // Calculate the depth of the tree
                _depth = std::ceil(std::log2(d2 / max));
            }

            // Set the tree cell extent 2^depth
            _scale = static_cast<K>((0x1 << _depth));
            _cell_extent = _root.get_cell().get_extent() / _scale;
        }

        return _depth;
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
            _key_cache[i] = this->get_sorting_key(shapes[i].get_center());
        }

        // use uint radix sort for sorting keys
        // lambda function to create sorted array indices based on tree key
        uint_sort<K>(_index_map, _sort_copy, [this](const K a) {
            return this->_key_cache[a];
        });

        // Iterate over sorted indices and store sorted shapes
        _shapes.clear();
        _shapes.reserve(size);
        for (const auto &i : _index_map)
        {
            _shapes.emplace_back(shapes[i]);
        }
    }

  public:
    tree(const cell<T, vec> &c)
        : _root(c),
          _lower_bound(_root.get_cell().get_min() + var<T>::TOL_PHYS_EDGE),
          _upper_bound(_root.get_cell().get_max() - var<T>::TOL_PHYS_EDGE),
          _depth_override(false), _flag_size(0) {}
    inline void resize(const cell<T, vec> &c)
    {
        _root = c;
        _lower_bound = _root.get_cell().get_min() + var<T>::TOL_PHYS_EDGE;
        _upper_bound = _root.get_cell().get_max() - var<T>::TOL_PHYS_EDGE;
    }
    inline void check_size(const std::vector<shape<T, vec>> &shapes) const
    {
        // Check size of the number of objects to insert into tree
        if (shapes.size() > std::numeric_limits<K>::max() - 1)
        {
            throw std::runtime_error("tree(): too many objects to insert, max supported is " + std::to_string(std::numeric_limits<K>::max()));
        }
    }
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
    inline const tree_node<T, K, L, vec, cell, shape> &get_node(const vec<T> &point) const
    {
        // This function computes the octree key location code
        // for traversing the octree to the leaf nodes without needing to use recursion

        // Calculate ratio between 0.0 and 1.0
        const vec<T> &min = _root.get_cell().get_min();
        const vec<T> &max = _root.get_cell().get_max();
        vec<T> ratio = vec<T>::ratio(min, max, point);

        // Start traversing at the root node
        const tree_node<T, K, L, vec, cell, shape> *child = &_root;

        T middle = 0.5;
        for (K i = 0; i < _depth; i++)
        {
            // Get the key from the point ratio
            const uint8_t key = ratio.subdivide_key(middle);

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
    inline K get_scale() const
    {
        return _scale;
    }
    inline const std::vector<shape<T, vec>> &get_shapes()
    {
        return _shapes;
    }
    inline const std::vector<std::pair<K, K>> &get_collisions() const
    {
        // Clear out the old collision sets and vectors
        _flags.clear();
        _hits.clear();
        _hits.reserve(_shapes.size());

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
        _hits.reserve(_shapes.size());

        // Clamp point into world bounds
        const vec<T> clamped = clamp_bounds(point);

        // get the node from the point
        const tree_node<T, K, L, vec, cell, shape> &node = get_node(clamped);

        // Get the intersecting pairs in this cell
        get_pairs(node);

        // Return the list
        return _hits;
    }
    inline const std::vector<std::pair<K, vec<T>>> &get_collisions(const ray<T, vec> &r) const
    {
        // Output vector
        _ray_hits.clear();
        _ray_hits.reserve(_shapes.size());

        // Get shapes intersecting ray with early stop
        get_ray_intersect(_root, r, _depth);

        // Return the collision list
        return _ray_hits;
    }
    inline K get_depth() const
    {
        return _depth;
    }
    inline const std::vector<K> &get_index_map() const
    {
        return _index_map;
    }
    inline const std::vector<std::pair<K, K>> &get_overlap(const shape<T, vec> &overlap) const
    {
        // Check if tree is not built yet
        if (_root.get_children().size() == 0)
        {
            return _hits;
        }

        // Clear out the old collision sets and vectors
        _flags.clear();
        _hits.clear();
        _hits.reserve(_shapes.size());

        // Get the overlapping shapes in this cell
        get_overlap(_root, overlap.get_min(), overlap.get_max(), _depth);

        // Return the list
        return _hits;
    }
    bool inside(const vec<T> &point) const
    {
        return _root.get_cell().point_inside(point);
    }
    inline void insert(const std::vector<shape<T, vec>> &shapes)
    {
        // Set the tree depth
        optimize_depth(shapes);

        // Sort shapes by grid key id
        sort(shapes);

        // Clear out the root node
        _root.clear();

        // Create box keys
        create_keys();

        // Rebuild the tree after changing the contents
        build(_root, _depth);
    }
    inline void insert(const std::vector<shape<T, vec>> &shapes, const K depth)
    {
        // Set the depth
        _depth = depth;

        // Set the tree cell extent 2^depth
        _scale = 0x1 << _depth;
        _cell_extent = _root.get_cell().get_extent() / _scale;

        // Sort shapes by grid key id
        sort(shapes);

        // Clear out the root node
        _root.clear();

        // Create box keys
        create_keys();

        // Rebuild the tree after changing the contents
        build(_root, _depth);
    }
    inline void insert_no_sort(const std::vector<shape<T, vec>> &shapes)
    {
        // Set the tree depth
        optimize_depth(shapes);

        // insert shapes without sorting
        _shapes.clear();
        _shapes.insert(_shapes.end(), shapes.begin(), shapes.end());

        // Clear out the root node
        _root.clear();

        // Create box keys
        create_keys();

        // Rebuild the tree after changing the contents
        build(_root, _depth);
    }
    inline const std::vector<K> &point_inside(const vec<T> &point) const
    {
        // Clamp point into world bounds
        const vec<T> clamped = clamp_bounds(point);

        // Get the keys on the leaf node
        return get_node(clamped).get_keys();
    }
    inline void set_depth(const K depth)
    {
        // Set the depth and indicate overriding calculated depth
        _depth_override = true;
        _depth = depth;
    }
};
}

#endif
