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
#ifndef __TRANSFORM2__
#define __TRANSFORM2__

namespace min
{
template <typename T>
class mat2;
template <typename T>
class mat3;
template <typename T>
class vec2;
template <typename T>
class vec3;
}

#include <cmath>
#include <min/mat2.h>
#include <min/mat3.h>
#include <min/vec2.h>
#include <min/vec3.h>

namespace min
{

template <typename T>
class tran2
{
  private:
    mat3<T> _m;

  public:
    tran2() : _m() {}
    tran2(const vec2<T> &t) : _m(t) {}
    tran2(const mat2<T> &r) : _m(r) {}
    tran2(const vec2<T> &t, const mat2<T> &r) : _m(t, r) {}
    tran2(const vec2<T> &t, const mat2<T> &r, const vec2<T> &s) : _m(t, r) { scale(s); }
    inline tran2<T> &translate(const T x, const T y)
    {
        return translate(vec2<T>(x, y));
    }
    inline tran2<T> &translate(const vec2<T> &t)
    {
        mat3<T> translation = mat3<T>(t);
        _m *= translation;
        return *this;
    }
    inline tran2<T> &rotate(const mat2<T> &r)
    {
        mat3<T> rotation = mat3<T>(r);
        _m *= rotation;
        return *this;
    }
    inline tran2<T> &scale(const T x, const T y)
    {
        return scale(vec2<T>(x, y));
    }
    inline tran2<T> &scale(const vec2<T> &s)
    {
        mat3<T> scale = mat3<T>();
        scale.set_scale(s);
        _m *= scale;
        return *this;
    }
    inline tran2<T> &transpose()
    {
        _m.transpose();
        return *this;
    }
    inline tran2<T> &invert()
    {
        _m.invert();
        return *this;
    }
    inline vec3<T> transform(const vec3<T> &v) const
    {
        return _m * v;
    }
    inline const mat3<T> &m() const
    {
        return _m;
    }

  private:
};
}

#endif