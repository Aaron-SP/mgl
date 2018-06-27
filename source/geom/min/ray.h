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
#ifndef __RAY__
#define __RAY__

#include <min/utility.h>

namespace min
{

template <typename T, template <typename> class vec>
class ray
{
  private:
    vec<T> _origin;
    vec<T> _dir;
    vec<T> _inv;

  public:
    ray() : _origin(), _dir(), _inv() {}
    ray(const vec<T> &from, const vec<T> &to) : _origin(from)
    {
        // Calculate a ray in direction to - from
        _dir = (to - from).normalize_unsafe();

        // If ray is axis aligned, element will be MAX(T)
        _inv = _dir.inverse_safe();
    }
    inline T set(const vec<T> &from, const vec<T> &to)
    {
        _origin = from;

        // Calculate the direction vector
        const vec<T> dir = to - from;
        const T length = dir.magnitude();
        const T inv_len = 1.0 / length;

        // Normalize
        if (length > var<T>::TOL_REL)
        {
            _dir = dir * inv_len;
            _inv = _dir.inverse_safe();
        }

        return length;
    }
    inline const vec<T> &get_origin() const { return _origin; }
    inline const vec<T> &get_direction() const { return _dir; }
    inline const vec<T> &get_inverse() const { return _inv; }
    inline vec<T> interpolate(const T t) const
    {
        // Return the point on the ray
        return _origin + _dir * t;
    }
};
}

#endif
