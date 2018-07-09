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
#ifndef __OOBBOX__
#define __OOBBOX__

#include <cmath>
#include <min/coord_sys.h>
#include <min/stack_vector.h>
#include <min/utility.h>
#include <vector>

namespace min
{

template <typename T, template <typename> class vec, template <typename> class rot>
class oobbox_base
{
  private:
    coord_sys<T, vec> _axes;
    vec<T> _center;
    vec<T> _half_extent;
    rot<T> _rotation;

    inline void add(const std::vector<vec<T>> &verts)
    {
        // Calculate the maximum extent of the point list
        const auto size = verts.size();
        vec<T> min, max;
        if (size >= 2)
        {
            const std::pair<vec<T>, vec<T>> p = vec<T>::extents(verts);
            min = p.first - var<T>::TOL_REL2;
            max = p.second + var<T>::TOL_REL2;
        }
        else if (size == 1)
        {
            min = verts[0];
            max = verts[0];
        }

        // Create box internals
        _center = (max + min) * 0.5;
        _half_extent = (max - min) * 0.5;
    }
    inline const vec<T> get_local_min() const
    {
        // This returns min in object space (AABB)
        return _half_extent * -1.0;
    }
    inline const vec<T> get_local_max() const
    {
        // This returns max in object space (AABB)
        return _half_extent;
    }

  public:
    oobbox_base() : _axes(vec<T>::axes()) {}
    oobbox_base(const vec<T> &min, const vec<T> &max)
        : _axes(vec<T>::axes()), _center((max + min) * 0.5), _half_extent((max - min) * 0.5) {}
    oobbox_base(const std::vector<vec<T>> &verts) : _axes(vec<T>::axes())
    {
        add(verts);
    }
    inline vec<T> align(const vec<T> &v) const
    {
        // Transform the point in object space
        return _axes.align(v);
    }
    inline vec<T> closest_point(const vec<T> &p) const
    {
        // Transform to local coordinates
        // Project the point vector along local axes and clamp to half extent
        const vec<T> out = (p - _center).project_point(_axes, _half_extent);

        // Return closest point
        return out + _center;
    }
    inline const coord_sys<T, vec> &get_axes() const
    {
        return _axes;
    }
    inline const vec<T> &get_center() const
    {
        return _center;
    }
    inline const vec<T> &get_half_extent() const
    {
        return _half_extent;
    }
    inline vec<T> get_extent() const
    {
        return _half_extent * 2.0;
    }
    inline const vec<T> get_min() const
    {
        // This returns min in world space (AABB)
        return _center - _half_extent;
    }
    inline const vec<T> get_max() const
    {
        // This returns max in world space (AABB)
        return _center + _half_extent;
    }
    inline const rot<T> &get_rotation() const
    {
        return _rotation;
    }
    template <typename C>
    inline void grid(C &v, const size_t scale) const
    {
        // Create the grid cells
        vec<T>::grid(v, get_min(), get_max(), scale);
    }
    inline bool point_inside(const vec<T> &p) const
    {
        // Transform the point into object's coordinate system
        const vec<T> t = align(p - _center);

        // Test if point is in aabb in object space
        return t.within(get_local_min(), get_local_max());
    }
    inline void set_position(const vec<T> &position)
    {
        _center = position;
    }
    inline void set_rotation(const rot<T> &r)
    {
        // Reset rotation
        _rotation = r;

        // Get a copy of the reference axes
        coord_sys<T, vec> ref_axes = vec<T>::axes();

        // Rotate the coordinate axes
        ref_axes.rotate(r);

        // Reassign to local axes
        _axes = ref_axes;
    }
    inline T square_distance(const vec<T> &p) const
    {
        // Transform to local coordinates
        // Return square distance from point P
        return (p - _center).project_length(_axes, _half_extent);
    }
    inline T square_size() const
    {
        // This assumes a non rotated box (aabb)!
        // Calculates the squared distance across the box extent
        const vec<T> extent = get_extent();
        return extent.dot(extent);
    }
    inline min::stack_vector<std::pair<vec<T>, vec<T>>, vec<T>::sub_size()> subdivide() const
    {
        // Create the subdivided space in world space AABB
        return vec<T>::subdivide(get_min(), get_max());
    }
};

template <typename T, template <typename> class vec>
class oobbox : public oobbox_base<T, vec, quat>
{
};

// Partial specialization for resolving the type of rotation for vec2 = mat<T>
template <typename T>
class oobbox<T, vec2> : public oobbox_base<T, vec2, mat2>
{
  public:
    oobbox() : oobbox_base<T, vec2, mat2>() {}
    oobbox(const vec2<T> &min, const vec2<T> &max) : oobbox_base<T, vec2, mat2>(min, max) {}
    oobbox(const std::vector<vec2<T>> &verts) : oobbox_base<T, vec2, mat2>(verts) {}
};

// Partial specialization for resolving the type of rotation for vec3 = quat<T>
template <typename T>
class oobbox<T, vec3> : public oobbox_base<T, vec3, quat>
{
  public:
    oobbox() : oobbox_base<T, vec3, quat>() {}
    oobbox(const vec3<T> &min, const vec3<T> &max) : oobbox_base<T, vec3, quat>(min, max) {}
    oobbox(const std::vector<vec3<T>> &verts) : oobbox_base<T, vec3, quat>(verts) {}
};

// Partial specialization for resolving the type of rotation for vec4 = quat<T>
template <typename T>
class oobbox<T, vec4> : public oobbox_base<T, vec4, quat>
{
  public:
    oobbox() : oobbox_base<T, vec4, quat>() {}
    oobbox(const vec4<T> &min, const vec4<T> &max) : oobbox_base<T, vec4, quat>(min, max) {}
    oobbox(const std::vector<vec4<T>> &verts) : oobbox_base<T, vec4, quat>(verts) {}
};
}

#endif
