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
#ifndef __TRANSFORM3__
#define __TRANSFORM3__

namespace min
{
template <typename T>
class vec3;
}
#include <min/vec3.h>
namespace min
{
template <typename T>
class quat;
}
#include <min/quat.h>
namespace min
{
template <typename T>
class mat4;
}
#include <min/mat4.h>
#include <cmath>

namespace min
{

template <typename T>
class tran3
{
  private:
    mat4<T> _m;

  public:
    tran3() : _m() {}
    tran3(const vec3<T> &t) : _m(t) {}
    tran3(const quat<T> &r) : _m(r) {}
    tran3(const vec3<T> &t, const quat<T> &r) : _m(t, r) {}
    tran3(const vec3<T> &t, const quat<T> &r, const vec3<T> &s) : _m(t, r) { scale(s); }
    inline tran3<T> &translate(const T x, const T y, const T z)
    {
        return translate(vec3<T>(x, y, z));
    }
    inline tran3<T> &translate(const vec3<T> &t)
    {
        mat4<T> translation = mat4<T>(t);
        _m *= translation;
        return *this;
    }
    inline tran3<T> &rotate(const quat<T> &r)
    {
        mat4<T> rotation = mat4<T>(r);
        _m *= rotation;
        return *this;
    }
    inline tran3<T> &rotate_x(const mat2<T> &r)
    {
        mat4<T> rotation = mat4<T>(mat3<T>().set_rotation_x(r));
        _m *= rotation;
        return *this;
    }
    inline tran3<T> &rotate_y(const mat2<T> &r)
    {
        mat4<T> rotation = mat4<T>(mat3<T>().set_rotation_y(r));
        _m *= rotation;
        return *this;
    }
    // Default rotation is about the Z axis
    inline tran3<T> &rotate_z(const mat2<T> &r)
    {
        mat4<T> rotation = mat4<T>(mat3<T>(r));
        _m *= rotation;
        return *this;
    }
    inline tran3<T> &scale(const T x, const T y, const T z)
    {
        return scale(vec3<T>(x, y, z));
    }
    inline tran3<T> &scale(const vec3<T> &s)
    {
        mat4<T> scale = mat4<T>();
        scale.set_scale(s);
        _m *= scale;
        return *this;
    }
    inline tran3<T> &transpose()
    {
        _m.transpose();
        return *this;
    }
    inline tran3<T> &invert()
    {
        _m.invert();
        return *this;
    }
    inline vec4<T> transform(const vec4<T> &v) const
    {
        return _m * v;
    }
    inline const mat4<T> &m() const
    {
        return _m;
    }

  private:
};
}

#endif