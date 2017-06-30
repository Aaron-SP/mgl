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
#ifndef __OOBBOX__
#define __OOBBOX__

#include <cmath>
#include <min/coord_sys.h>
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

  public:
    oobbox_base() {}
    oobbox_base(const vec<T> &min, const vec<T> &max)
        : _axes(vec<T>::axises()), _center((max + min) * 0.5), _half_extent((max - min) * 0.5) {}
    inline vec<T> closest_point(const vec<T> &p) const
    {
        // Transform to local coordinates
        // Project the point vector along local axises and clamp to half extent
        const vec<T> out = (p - _center).project_point(_axes, _half_extent);

        // Return closest point
        return out + _center;
    }
    inline vec<T> get_center() const
    {
        return _center;
    }
    inline rot<T> &get_rotation() const
    {
        return _rotation;
    }
    inline void set_rotation(const rot<T> &r)
    {
        // Reset rotation
        _rotation = r;

        // Get a copy of the reference axes
        coord_sys<T, vec> ref_axes = vec<T>::axises();

        // Rotate the coordinate axises
        ref_axes.rotate(r);

        // Reassign to local axes
        _axes = ref_axes;
    }
    inline vec<T> get_extent() const
    {
        // This assumes a non rotated box (aabb)!
        return _half_extent * 2.0;
    }
    inline const vec<T> get_min() const
    {
        // This assumes a non rotated box (aabb)!
        return _center - _half_extent;
    }
    inline const vec<T> get_max() const
    {
        // This assumes a non rotated box (aabb)!
        return _center + _half_extent;
    }
    // inline vec<T> normal(const vec<T> &p, T &length, const T tolerance) const
    // {
    // }
    inline bool point_inside(const vec<T> &p) const
    {
        // Transform to local coordinates
        const vec<T> v = p - _center;

        // Calculate the inverse rotation
        const rot<T> inv_rot = _rotation.inverse();

        // Transform the point, and convert to world coordinates
        const vec<T> t = inv_rot.transform(v) + _center;

        // Test if point is in aabb
        return t.within(get_min(), get_max());
    }
    inline void set_position(const vec<T> &position)
    {
        _center = position;
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
        vec<T> extent = get_extent();
        return extent.dot(extent);
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
};

// Partial specialization for resolving the type of rotation for vec3 = quat<T>
template <typename T>
class oobbox<T, vec3> : public oobbox_base<T, vec3, quat>
{
  public:
    oobbox() : oobbox_base<T, vec3, quat>() {}
    oobbox(const vec3<T> &min, const vec3<T> &max) : oobbox_base<T, vec3, quat>(min, max) {}
};

// Partial specialization for resolving the type of rotation for vec4 = quat<T>
template <typename T>
class oobbox<T, vec4> : public oobbox_base<T, vec4, quat>
{
  public:
    oobbox() : oobbox_base<T, vec4, quat>() {}
    oobbox(const vec4<T> &min, const vec4<T> &max) : oobbox_base<T, vec4, quat>(min, max) {}
};
}

#endif