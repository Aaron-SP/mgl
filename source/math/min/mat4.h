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
#ifndef _MGL_MATRIX4_MGL_
#define _MGL_MATRIX4_MGL_

namespace min
{
template <typename T>
class mat3;
template <typename T>
class quat;
template <typename T>
class vec3;
}

#include <cmath>
#include <min/mat3.h>
#include <min/quat.h>
#include <min/vec3.h>

// This matrix is left handed and follows the following conventions:
// Rotation can be instantiated from a mat2<T>, mat3<T>, or a quat<T>
// Rotation takes place around the 'Y' axis
// Transformation to 3D
//          (y)  (z)
//           ^  /
//           | /
//           |/
//           ~-------- (x)
// This matrix is can be orthogonal or can be a transformation matrix in 3D

namespace min
{

template <typename T>
class mat4
{
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
    T _j;
    T _k;
    T _l;
    T _m;
    T _n;
    T _o;
    T _p;

  public:
    // constructs an identity matrix
    mat4()
        : _a(1.0f), _b(0.0f), _c(0.0f), _d(0.0f), _e(0.0f), _f(1.0f), _g(0.0f), _h(0.0f), _i(0.0f), _j(0.0f), _k(1.0f), _l(0.0f), _m(0.0f), _n(0.0f), _o(0.0f), _p(1.0f) {}

    // convenience constructor for direct loading
    mat4(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j, T k, T l, T m, T n, T o, T p)
        : _a(a), _b(b), _c(c), _d(d), _e(e), _f(f), _g(g), _h(h), _i(i), _j(j), _k(k), _l(l), _m(m), _n(n), _o(o), _p(p) {}

    // constructs a translation matrix
    mat4(const vec3<T> &t)
        : _a(1.0f), _b(0.0f), _c(0.0f), _d(0.0f), _e(0.0f), _f(1.0f), _g(0.0f), _h(0.0f), _i(0.0f), _j(0.0f), _k(1.0f), _l(0.0f), _m(t.x()), _n(t.y()), _o(t.z()), _p(1.0f) {}

    // constructs a 3D rotation matrix
    mat4(const mat3<T> &r)
        : _a(r._a), _b(r._b), _c(r._c), _d(0.0f), _e(r._d), _f(r._e), _g(r._f), _h(0.0f), _i(r._g), _j(r._h), _k(r._i), _l(0.0f), _m(0.0f), _n(0.0f), _o(0.0f), _p(1.0f) {}

    // constructs a matrix that first rotates then translates in 3D
    mat4(const vec3<T> &t, const mat3<T> &r)
        : _a(r._a), _b(r._b), _c(r._c), _d(0.0f), _e(r._d), _f(r._e), _g(r._f), _h(0.0f), _i(r._g), _j(r._h), _k(r._i), _l(0.0f), _m(t.x()), _n(t.y()), _o(t.z()), _p(1.0f) {}

    // constructs a matrix that first rotates then translates in 3D, then scales the result
    mat4(const vec3<T> &t, const mat3<T> &r, const vec3<T> &s)
        : _a(r._a), _b(r._b), _c(r._c), _d(0.0f), _e(r._d), _f(r._e), _g(r._f), _h(0.0f), _i(r._g), _j(r._h), _k(r._i), _l(0.0f), _m(t.x()), _n(t.y()), _o(t.z()), _p(1.0f) { scale(s); }

    // constructs a lookat matrix from 4 vectors: right(x), up(y), forward(z), and eye
    mat4(const vec3<T> &x, const vec3<T> &y, const vec3<T> &z, const vec3<T> &e)
        : _a(x.x()), _b(x.y()), _c(x.z()), _d(-x.dot(e)), _e(y.x()), _f(y.y()), _g(y.z()), _h(-y.dot(e)), _i(z.x()), _j(z.y()), _k(z.z()), _l(-z.dot(e)), _m(0.0f), _n(0.0f), _o(0.0f), _p(1.0f) {}

    // constructs an orthographic projection matrix
    mat4(const T dx, const T dy, const T near, const T far)
        : _a(1.0f / dx), _b(0.0f), _c(0.0f), _d(0.0f), _e(0.0f), _f(1.0f / dy), _g(0.0f), _h(0.0f), _i(0.0f), _j(0.0f), _k(-2.0f / (far - near)), _l((far + near) / (far - near)), _m(0.0f), _n(0.0f), _o(0.0f), _p(1.0f) {}

    inline void one(vec4<T> &v)
    {
        _a = v.x();
        _b = v.y();
        _c = v.z();
        _d = v.w();
    }
    inline void two(vec4<T> &v)
    {
        _e = v.x();
        _f = v.y();
        _g = v.z();
        _h = v.w();
    }
    inline void three(vec4<T> &v)
    {
        _i = v.x();
        _j = v.y();
        _k = v.z();
        _l = v.w();
    }
    inline void four(vec4<T> &v)
    {
        _m = v.x();
        _n = v.y();
        _o = v.z();
        _p = v.w();
    }
    inline void w(const T w)
    {
        _p = w;
    }
    inline T w() const
    {
        return _p;
    }
    inline mat4<T> operator*(const mat4<T> &A) const
    {
        mat4<T> out;
        out._a = _a * A._a + _b * A._e + _c * A._i + _d * A._m;
        out._b = _a * A._b + _b * A._f + _c * A._j + _d * A._n;
        out._c = _a * A._c + _b * A._g + _c * A._k + _d * A._o;
        out._d = _a * A._d + _b * A._h + _c * A._l + _d * A._p;
        out._e = _e * A._a + _f * A._e + _g * A._i + _h * A._m;
        out._f = _e * A._b + _f * A._f + _g * A._j + _h * A._n;
        out._g = _e * A._c + _f * A._g + _g * A._k + _h * A._o;
        out._h = _e * A._d + _f * A._h + _g * A._l + _h * A._p;
        out._i = _i * A._a + _j * A._e + _k * A._i + _l * A._m;
        out._j = _i * A._b + _j * A._f + _k * A._j + _l * A._n;
        out._k = _i * A._c + _j * A._g + _k * A._k + _l * A._o;
        out._l = _i * A._d + _j * A._h + _k * A._l + _l * A._p;
        out._m = _m * A._a + _n * A._e + _o * A._i + _p * A._m;
        out._n = _m * A._b + _n * A._f + _o * A._j + _p * A._n;
        out._o = _m * A._c + _n * A._g + _o * A._k + _p * A._o;
        out._p = _m * A._d + _n * A._h + _o * A._l + _p * A._p;

        return out;
    }
    inline mat4<T> &operator*=(const mat4<T> &A)
    {
        const T a = _a * A._a + _b * A._e + _c * A._i + _d * A._m;
        const T b = _a * A._b + _b * A._f + _c * A._j + _d * A._n;
        const T c = _a * A._c + _b * A._g + _c * A._k + _d * A._o;
        const T d = _a * A._d + _b * A._h + _c * A._l + _d * A._p;
        const T e = _e * A._a + _f * A._e + _g * A._i + _h * A._m;
        const T f = _e * A._b + _f * A._f + _g * A._j + _h * A._n;
        const T g = _e * A._c + _f * A._g + _g * A._k + _h * A._o;
        const T h = _e * A._d + _f * A._h + _g * A._l + _h * A._p;
        const T i = _i * A._a + _j * A._e + _k * A._i + _l * A._m;
        const T j = _i * A._b + _j * A._f + _k * A._j + _l * A._n;
        const T k = _i * A._c + _j * A._g + _k * A._k + _l * A._o;
        const T l = _i * A._d + _j * A._h + _k * A._l + _l * A._p;
        const T m = _m * A._a + _n * A._e + _o * A._i + _p * A._m;
        const T n = _m * A._b + _n * A._f + _o * A._j + _p * A._n;
        const T o = _m * A._c + _n * A._g + _o * A._k + _p * A._o;
        const T p = _m * A._d + _n * A._h + _o * A._l + _p * A._p;

        _a = a;
        _b = b;
        _c = c;
        _d = d;
        _e = e;
        _f = f;
        _g = g;
        _h = h;
        _i = i;
        _j = j;
        _k = k;
        _l = l;
        _m = m;
        _n = n;
        _o = o;
        _p = p;

        return *this;
    }
    inline vec4<T> operator*(const vec4<T> &A) const
    {
        const T x = _a * A.x() + _e * A.y() + _i * A.z() + _m * A.w();
        const T y = _b * A.x() + _f * A.y() + _j * A.z() + _n * A.w();
        const T z = _c * A.x() + _g * A.y() + _k * A.z() + _o * A.w();
        const T w = _d * A.x() + _h * A.y() + _l * A.z() + _p * A.w();

        return vec4<T>(x, y, z, w);
    }
    inline vec3<T> get_translation() const
    {
        const T x = _m;
        const T y = _n;
        const T z = _o;

        return vec3<T>(x, y, z);
    }
    inline mat4<T> &set_translation(const vec3<T> &t)
    {
        _m = t.x();
        _n = t.y();
        _o = t.z();

        return *this;
    }
    inline quat<T> get_rotation() const
    {
        T w, x, y, z;

        const T trace = _a + _f + _k;
        if (trace > var<T>::TOL_REL)
        {
            const T s = std::sqrt(1.0f + trace) * 2.0f;
            w = 0.25 * s;
            x = (_g - _j) / s;
            y = (_i - _c) / s;
            z = (_b - _e) / s;
        }
        else if (_a > (_f + var<T>::TOL_REL) && _a > (_k + var<T>::TOL_REL))
        {
            const T s = std::sqrt(1.0f + _a - _f - _k) * 2.0f;
            w = (_g - _j) / s;
            x = 0.25 * s;
            y = (_e + _b) / s;
            z = (_i + _c) / s;
        }
        else if (_f > (_k + var<T>::TOL_REL))
        {
            const T s = std::sqrt(1.0f + _f - _a - _k) * 2.0f;
            w = (_i - _c) / s;
            x = (_e + _b) / s;
            y = 0.25 * s;
            z = (_j + _g) / s;
        }
        else
        {
            const T s = sqrt(1.0f + _k - _a - _f) * 2.0f;
            w = (_b - _e) / s;
            x = (_i + _c) / s;
            y = (_j + _g) / s;
            z = 0.25 * s;
        }

        return quat<T>(w, x, y, z);
    }
    inline mat4<T> &set_rotation(const mat3<T> &r)
    {
        _a = r._a;
        _b = r._b;
        _c = r._c;
        _e = r._d;
        _f = r._e;
        _g = r._f;
        _i = r._g;
        _j = r._h;
        _k = r._i;

        return *this;
    }
    inline vec3<T> get_scale() const
    {
        const T x = _a;
        const T y = _f;
        const T z = _k;

        return vec3<T>(x, y, z);
    }
    inline bool invert()
    {
        const T a = _f * _k * _p - _f * _l * _o - _j * _g * _p + _j * _h * _o + _n * _g * _l - _n * _h * _k;
        const T e = -_e * _k * _p + _e * _l * _o + _i * _g * _p - _i * _h * _o - _m * _g * _l + _m * _h * _k;
        const T i = _e * _j * _p - _e * _l * _n - _i * _f * _p + _i * _h * _n + _m * _f * _l - _m * _h * _j;
        const T m = -_e * _j * _o + _e * _k * _n + _i * _f * _o - _i * _g * _n - _m * _f * _k + _m * _g * _j;
        const T b = -_b * _k * _p + _b * _l * _o + _j * _c * _p - _j * _d * _o - _n * _c * _l + _n * _d * _k;
        const T f = _a * _k * _p - _a * _l * _o - _i * _c * _p + _i * _d * _o + _m * _c * _l - _m * _d * _k;
        const T j = -_a * _j * _p + _a * _l * _n + _i * _b * _p - _i * _d * _n - _m * _b * _l + _m * _d * _j;
        const T n = _a * _j * _o - _a * _k * _n - _i * _b * _o + _i * _c * _n + _m * _b * _k - _m * _c * _j;
        const T c = _b * _g * _p - _b * _h * _o - _f * _c * _p + _f * _d * _o + _n * _c * _h - _n * _d * _g;
        const T g = -_a * _g * _p + _a * _h * _o + _e * _c * _p - _e * _d * _o - _m * _c * _h + _m * _d * _g;
        const T k = _a * _f * _p - _a * _h * _n - _e * _b * _p + _e * _d * _n + _m * _b * _h - _m * _d * _f;
        const T o = -_a * _f * _o + _a * _g * _n + _e * _b * _o - _e * _c * _n - _m * _b * _g + _m * _c * _f;
        const T d = -_b * _g * _l + _b * _h * _k + _f * _c * _l - _f * _d * _k - _j * _c * _h + _j * _d * _g;
        const T h = _a * _g * _l - _a * _h * _k - _e * _c * _l + _e * _d * _k + _i * _c * _h - _i * _d * _g;
        const T l = -_a * _f * _l + _a * _h * _j + _e * _b * _l - _e * _d * _j - _i * _b * _h + _i * _d * _f;
        const T p = _a * _f * _k - _a * _g * _j - _e * _b * _k + _e * _c * _j + _i * _b * _g - _i * _c * _f;

        T det = _a * a + _b * e + _c * i + _d * m;

        if (std::abs(det) <= var<T>::TOL_REL)
        {
            return false;
        }

        det = 1.0f / det;
        _a = a * det;
        _b = b * det;
        _c = c * det;
        _d = d * det;
        _e = e * det;
        _f = f * det;
        _g = g * det;
        _h = h * det;
        _i = i * det;
        _j = j * det;
        _k = k * det;
        _l = l * det;
        _m = m * det;
        _n = n * det;
        _o = o * det;
        _p = p * det;

        return true;
    }
    inline mat4<T> &rotate_x(const mat2<T> &r)
    {
        const mat4<T> rotation = mat4<T>(mat3<T>().set_rotation_x(r));
        this->operator*=(rotation);
        return *this;
    }
    inline mat4<T> &rotate_y(const mat2<T> &r)
    {
        const mat4<T> rotation = mat4<T>(mat3<T>().set_rotation_y(r));
        this->operator*=(rotation);
        return *this;
    }
    // Default rotation is about the Z axis
    inline mat4<T> &rotate_z(const mat2<T> &r)
    {
        const mat4<T> rotation = mat4<T>(mat3<T>(r));
        this->operator*=(rotation);
        return *this;
    }
    inline mat4<T> &scale(const T x, const T y, const T z)
    {
        return scale(vec3<T>(x, y, z));
    }
    inline mat4<T> &scale(const vec3<T> &s)
    {
        const mat4<T> scale = mat4<T>().set_scale(s);
        this->operator*=(scale);
        return *this;
    }
    inline mat4<T> &set_scale(const vec3<T> &s)
    {
        _a = s.x();
        _f = s.y();
        _k = s.z();

        return *this;
    }
    inline mat4<T> &translate(const T x, const T y, const T z)
    {
        return translate(vec3<T>(x, y, z));
    }
    inline mat4<T> &translate(const vec3<T> &t)
    {
        const mat4<T> translation = mat4<T>(t);
        this->operator*=(translation);
        return *this;
    }
    inline mat4<T> &rotate(const quat<T> &r)
    {
        const mat4<T> rotation = mat4<T>(r);
        this->operator*=(rotation);
        return *this;
    }
    inline vec4<T> transform(const vec4<T> &v) const
    {
        // This matches quat<T> API!
        return this->operator*(v);
    }
    inline mat4<T> &transpose()
    {
        T temp = _b;
        _b = _e;
        _e = temp;

        temp = _c;
        _c = _i;
        _i = temp;

        temp = _g;
        _g = _j;
        _j = temp;

        temp = _d;
        _d = _m;
        _m = temp;

        temp = _h;
        _h = _n;
        _n = temp;

        temp = _l;
        _l = _o;
        _o = temp;

        return *this;
    }
    inline mat4<T> transpose_multiply(const mat4<T> &A) const
    {
        // This multiplies two matrices and returns the transpose of the product
        mat4<T> out;

        out._a = _a * A._a + _b * A._e + _c * A._i + _d * A._m;
        out._b = _e * A._a + _f * A._e + _g * A._i + _h * A._m;
        out._c = _i * A._a + _j * A._e + _k * A._i + _l * A._m;
        out._d = _m * A._a + _n * A._e + _o * A._i + _p * A._m;
        out._e = _a * A._b + _b * A._f + _c * A._j + _d * A._n;
        out._f = _e * A._b + _f * A._f + _g * A._j + _h * A._n;
        out._g = _i * A._b + _j * A._f + _k * A._j + _l * A._n;
        out._h = _m * A._b + _n * A._f + _o * A._j + _p * A._n;
        out._i = _a * A._c + _b * A._g + _c * A._k + _d * A._o;
        out._j = _e * A._c + _f * A._g + _g * A._k + _h * A._o;
        out._k = _i * A._c + _j * A._g + _k * A._k + _l * A._o;
        out._l = _m * A._c + _n * A._g + _o * A._k + _p * A._o;
        out._m = _a * A._d + _b * A._h + _c * A._l + _d * A._p;
        out._n = _e * A._d + _f * A._h + _g * A._l + _h * A._p;
        out._o = _i * A._d + _j * A._h + _k * A._l + _l * A._p;
        out._p = _m * A._d + _n * A._h + _o * A._l + _p * A._p;

        return out;
    }
};
}

#endif
