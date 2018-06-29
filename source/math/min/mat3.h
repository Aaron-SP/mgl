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
#ifndef __MATRIX3__
#define __MATRIX3__

namespace min
{
template <typename T>
class mat2;
template <typename T>
class mat4;
template <typename T>
class quat;
template <typename T>
class vec2;
template <typename T>
class vec3;
}

#include <cmath>
#include <min/mat2.h>
#include <min/mat4.h>
#include <min/quat.h>
#include <min/vec2.h>
#include <min/vec3.h>

// This matrix is left handed and follows the following conventions:
// x-axis: (+) angle -> CW
// x-axis: (-) angle -> CCW
// y-axis: (+) angle -> CW
// y-axis: (-) angle -> CCW
// z-axis: (+) angle -> CCW
// z-axis: (-) angle -> CW
// Rotation takes place around the 'Y' axis
// Transformation to 3D
//          (y)  (z)
//           ^  /
//           | /
//           |/
//           ~-------- (x)
// This matrix is can be orthogonal or can be a transformation matrix in 2D

namespace min
{

template <typename T>
class mat3
{
    friend class mat4<T>;

  private:
    T _a;
    T _b;
    T _c;
    T _d;
    T _e;
    T _f;
    T _g;
    T _h;
    T _i;

  public:
    // constructs an identity matrix
    mat3()
        : _a(1.0), _b(0.0), _c(0.0), _d(0.0), _e(1.0), _f(0.0), _g(0.0), _h(0.0), _i(1.0) {}

    // constructs a translation matrix
    mat3(const vec2<T> &t)
        : _a(1.0), _b(0.0), _c(0.0), _d(0.0), _e(1.0), _f(0.0), _g(t.x()), _h(t.y()), _i(1.0) {}

    // constructs an orthogonal 3D rotation matrix around the Z axis
    mat3(const mat2<T> &r)
        : _a(r._xc), _b(r._ys), _c(0.0), _d(r._xs), _e(r._yc), _f(0.0), _g(0.0), _h(0.0), _i(1.0) {}

    // constructs a matrix that first rotates then translates in 2D
    mat3(const vec2<T> &t, const mat2<T> &r)
        : _a(r._xc), _b(r._ys), _c(0.0), _d(r._xs), _e(r._yc), _f(0.0), _g(t.x()), _h(t.y()), _i(1.0) {}

    // constructs a matrix that first rotates then translates in 2D, then scales the result
    mat3(const vec2<T> &t, const mat2<T> &r, const vec2<T> &s)
        : _a(r._xc), _b(r._ys), _c(0.0), _d(r._xs), _e(r._yc), _f(0.0), _g(t.x()), _h(t.y()), _i(1.0) { scale(s); }

    inline void one(vec3<T> &v)
    {
        _a = v.x();
        _b = v.y();
        _c = v.z();
    }
    inline void two(vec3<T> &v)
    {
        _d = v.x();
        _e = v.y();
        _f = v.z();
    }
    inline void three(vec3<T> &v)
    {
        _g = v.x();
        _h = v.y();
        _i = v.z();
    }
    inline void w(const T w)
    {
        _i = w;
    }
    inline T w() const
    {
        return _i;
    }

    // constructs a 3D rotation matrix
    mat3(const quat<T> &r)
    {
        T xx = r.x() * r.x();
        T yy = r.y() * r.y();
        T zz = r.z() * r.z();
        T xw = r.x() * r.w();
        T yw = r.y() * r.w();
        T zw = r.z() * r.w();
        T xy = r.x() * r.y();
        T xz = r.x() * r.z();
        T yz = r.y() * r.z();

        _a = 1.0 - (2.0 * (yy + zz));
        _b = 2.0 * (xy + zw);
        _c = 2.0 * (xz - yw);
        _d = 2.0 * (xy - zw);
        _e = 1.0 - (2.0 * (xx + zz));
        _f = 2.0 * (yz + xw);
        _g = 2.0 * (xz + yw);
        _h = 2.0 * (yz - xw);
        _i = 1.0 - (2.0 * (xx + yy));
    }
    inline mat3<T> operator*(const mat3<T> &A) const
    {
        mat3<T> out;
        out._a = _a * A._a + _b * A._d + _c * A._g;
        out._b = _a * A._b + _b * A._e + _c * A._h;
        out._c = _a * A._c + _b * A._f + _c * A._i;
        out._d = _d * A._a + _e * A._d + _f * A._g;
        out._e = _d * A._b + _e * A._e + _f * A._h;
        out._f = _d * A._c + _e * A._f + _f * A._i;
        out._g = _g * A._a + _h * A._d + _i * A._g;
        out._h = _g * A._b + _h * A._e + _i * A._h;
        out._i = _g * A._c + _h * A._f + _i * A._i;

        return out;
    }
    inline mat3<T> &operator*=(const mat3<T> &A)
    {
        T a = _a * A._a + _b * A._d + _c * A._g;
        T b = _a * A._b + _b * A._e + _c * A._h;
        T c = _a * A._c + _b * A._f + _c * A._i;
        T d = _d * A._a + _e * A._d + _f * A._g;
        T e = _d * A._b + _e * A._e + _f * A._h;
        T f = _d * A._c + _e * A._f + _f * A._i;
        T g = _g * A._a + _h * A._d + _i * A._g;
        T h = _g * A._b + _h * A._e + _i * A._h;
        T i = _g * A._c + _h * A._f + _i * A._i;

        _a = a;
        _b = b;
        _c = c;
        _d = d;
        _e = e;
        _f = f;
        _g = g;
        _h = h;
        _i = i;

        return *this;
    }
    inline vec3<T> operator*(const vec3<T> &A) const
    {
        T x = _a * A.x() + _d * A.y() + _g * A.z();
        T y = _b * A.x() + _e * A.y() + _h * A.z();
        T z = _c * A.x() + _f * A.y() + _i * A.z();

        return vec3<T>(x, y, z);
    }
    inline vec2<T> get_translation() const
    {
        return vec2<T>(_g, _h);
    }
    inline mat3<T> &set_translation(const vec2<T> &vec)
    {
        _g = vec.x();
        _h = vec.y();
        return *this;
    }
    inline mat2<T> get_rotation() const
    {
        return mat2<T>(_a, _b, _d, _e);
    }
    // Default rotation is about the Z axis
    inline mat3<T> &set_rotation(const mat2<T> &rot)
    {
        _a = rot._xc;
        _b = rot._ys;
        _d = rot._xs;
        _e = rot._yc;
        return *this;
    }
    inline mat2<T> get_rotation_x() const
    {
        return mat2<T>(_e, _f, _h, _i);
    }
    inline mat3<T> &set_rotation_x(const mat2<T> &rot)
    {
        _e = rot._xc;
        _f = rot._ys;
        _h = rot._xs;
        _i = rot._yc;
        return *this;
    }
    inline mat2<T> get_rotation_y() const
    {
        return mat2<T>(_a, _c, _g, _i);
    }
    inline bool invert()
    {
        T a, b, c, d, e, f, g, h, i, det;

        a = _e * _i - _h * _f;
        b = _h * _c - _b * _i;
        c = _b * _f - _e * _c;
        d = _g * _f - _d * _i;
        e = _a * _i - _g * _c;
        f = _d * _c - _a * _f;
        g = _d * _h - _g * _e;
        h = _g * _b - _a * _h;
        i = _a * _e - _d * _b;

        det = _a * _e * _i + _b * _f * _g + _c * _d * _h - _a * _f * _h - _c * _e * _g - _b * _d * _i;
        if (std::abs(det) <= var<T>::TOL_REL)
        {
            return false;
        }

        det = 1.0 / det;
        _a = a * det;
        _b = b * det;
        _c = c * det;
        _d = d * det;
        _e = e * det;
        _f = f * det;
        _g = g * det;
        _h = h * det;
        _i = i * det;

        return true;
    }
    inline mat3<T> &rotate(const mat2<T> &r)
    {
        const mat3<T> rotation = mat3<T>(r);
        this->operator*=(rotation);
        return *this;
    }
    inline mat3<T> &set_rotation_y(const mat2<T> &rot)
    {
        _a = rot._xc;
        _c = rot._xs;
        _g = rot._ys;
        _i = rot._yc;
        return *this;
    }
    inline vec2<T> get_scale() const
    {
        return vec2<T>(_a, _e);
    }
    inline mat3<T> &scale(const T x, const T y)
    {
        return scale(vec2<T>(x, y));
    }
    inline mat3<T> &scale(const vec2<T> &s)
    {
        const mat3<T> scale = mat3<T>().set_scale(s);
        this->operator*=(scale);
        return *this;
    }
    inline mat3<T> &set_scale(const vec2<T> &s)
    {
        _a = s.x();
        _e = s.y();
        return *this;
    }
    inline mat3<T> &translate(const T x, const T y)
    {
        return translate(vec2<T>(x, y));
    }
    inline mat3<T> &translate(const vec2<T> &t)
    {
        const mat3<T> translation = mat3<T>(t);
        this->operator*=(translation);
        return *this;
    }
    inline vec3<T> transform(const vec3<T> &v) const
    {
        // This matches quat<T> API!
        return this->operator*(v);
    }
    inline mat3<T> &transpose()
    {
        T swap = _b;
        _b = _d;
        _d = swap;

        swap = _c;
        _c = _g;
        _g = swap;

        swap = _f;
        _f = _h;
        _h = swap;

        return *this;
    }
};
}

#endif
