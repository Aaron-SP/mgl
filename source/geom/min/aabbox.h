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
#ifndef __AABBOX__
#define __AABBOX__

#include <cmath>
#include <cstdio>
#include <min/stack_vector.h>
#include <min/utility.h>
#include <utility>
#include <vector>

namespace min
{

template <typename T, template <typename> class vec>
class aabbox
{
  private:
    vec<T> _min;
    vec<T> _max;

  public:
    aabbox() : _min(), _max() {}
    aabbox(const vec<T> &min, const vec<T> &max) : _min(min), _max(max) {}
    aabbox(const std::vector<vec<T>> &verts) : _min(), _max()
    {
        add(verts);
    }
    inline void add(const std::vector<vec<T>> &verts)
    {
        // Calculate the maximum extent of the point list
        const auto size = verts.size();
        if (size >= 2)
        {
            const std::pair<vec<T>, vec<T>> p = vec<T>::extents(verts);
            _min = p.first - var<T>::TOL_REL2;
            _max = p.second + var<T>::TOL_REL2;
        }
        else if (size == 1)
        {
            _min = verts[0];
            _max = verts[0];
        }
    }
    inline vec<T> closest_point(const vec<T> &p) const
    {
        // Compute the closest point to this aabb
        // Return this point if inside
        vec<T> out = p;
        out.clamp(_min, _max);

        return out;
    }
    inline vec<T> get_center() const
    {
        return (_min + _max) * 0.5;
    }
    inline vec<T> get_extent() const
    {
        return _max - _min;
    }
    inline const vec<T> &get_min() const
    {
        return _min;
    }
    inline const vec<T> &get_max() const
    {
        return _max;
    }
    template <typename C>
    inline void grid(C &v, const size_t scale) const
    {
        // Create the grid cells
        vec<T>::grid(v, _min, _max, scale);
    }
    inline bool point_inside(const vec<T> &p) const
    {
        return p.within(_min, _max);
    }
    inline void set_position(const vec<T> &pos)
    {
        // min = _min - center = (_min - _max) * 0.5
        // max = _max - center = (_max - _min) * 0.5
        const vec<T> min = (_min - _max) * 0.5;
        const vec<T> max = (_max - _min) * 0.5;

        // Update the min and max
        _min = min + pos;
        _max = max + pos;
    }
    inline T square_distance(const vec<T> &p) const
    {
        // Return the distance to this aabb
        // Return 0 if inside it
        const vec<T> closest = closest_point(p);
        const vec<T> d = (p - closest);
        return d.dot(d);
    }
    inline T square_size() const
    {
        // Calculates the squared distance across the aabb extent
        const vec<T> extent = get_extent();
        return extent.dot(extent);
    }
    template <typename C>
    inline void subdivide(C &v) const
    {
        // Create the subdivided space
        vec<T>::subdivide(v, _min, _max);
    }
};
}

#endif
