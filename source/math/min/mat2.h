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
#ifndef _MGL_MATRIX2_MGL_
#define _MGL_MATRIX2_MGL_

namespace min
{
template <typename T>
class vec2;
template <typename T>
class mat3;
}

#include <cmath>
#include <min/mat3.h>
#include <min/utility.h>
#include <min/vec2.h>

// This matrix is left handed and follows the following conventions:
// (+) angle -> CCW
// (-) angle -> CW
// y == z-axis
// x == x-axis
// Rotation takes place around the 'Y' axis perpendicular to the 'screen'
// Transformation to 3D
//          (y)  (z)
//           ^  /
//           | /
//           |/
//           ~-------- (x)
// This matrix is always orthogonal

namespace min
{

template <typename T>
class mat2
{
    friend class mat3<T>;

  private:
    T _xc; // xcos(0)
    T _ys; // -ysin(0)
    T _xs; // xsin(0)
    T _yc; // ycos(0)
  public:
    // constructs an identity matrix
    mat2()
        : _xc(1.0), _ys(0.0), _xs(0.0), _yc(1.0) {}

    // convenience constructor for direct loading
    mat2(T xc, T ys, T xs, T yc)
        : _xc(xc), _ys(ys), _xs(xs), _yc(yc) {}

    // constructs an orthogonal 2D rotation matrix around the screen axis
    mat2(const T angle) // Angle in degrees
    {
        if (angle > 0.0)
        {
            T posAngle = deg_to_rad(std::abs(angle));
            T sine = std::sin(posAngle);
            T cosine = std::cos(posAngle);

            _xc = cosine;
            _ys = sine;
            _xs = -sine;
            _yc = cosine;
        }
        else if (angle < 0.0)
        {
            T negAngle = deg_to_rad(std::abs(angle));
            T sine = std::sin(negAngle);
            T cosine = std::cos(negAngle);
            _xc = cosine;
            _ys = -sine;
            _xs = sine;
            _yc = cosine;
        }
        else
        {
            _xc = 1.0;
            _ys = 0.0;
            _xs = 0.0;
            _yc = 1.0;
        }
    }
    inline mat2<T> &operator*=(const mat2<T> &A)
    {
        _xc = _xc * A._xc + _xs * A._ys;
        _ys = _ys * A._xc + _yc * A._ys;
        _xs = _xc * A._xs + _xs * A._yc;
        _yc = _ys * A._xs + _yc * A._yc;
        return *this;
    }
    inline mat2<T> operator*(const mat2<T> &A) const
    {
        T xc = _xc * A._xc + _xs * A._ys;
        T ys = _ys * A._xc + _yc * A._ys;
        T xs = _xc * A._xs + _xs * A._yc;
        T yc = _ys * A._xs + _yc * A._yc;
        return mat2<T>(xc, ys, xs, yc);
    }
    inline vec2<T> operator*(const vec2<T> &vec) const
    {
        T x = _xc * vec.x() + _xs * vec.y();
        T y = _ys * vec.x() + _yc * vec.y();
        return vec2<T>(x, y);
    }
    inline mat2<T> inverse() const
    {
        mat2<T> copy = *this;
        return copy.invert();
    }
    inline mat2<T> &invert()
    {
        // this matrix is always orthogonal; use transpose()
        return transpose();
    }
    inline vec2<T> transform(const vec2<T> &v) const
    {
        // This matches quat<T> API!
        return this->operator*(v);
    }
    inline mat2<T> &transpose()
    {
        T temp = _ys;
        _ys = _xs;
        _xs = temp;
        return *this;
    }
};
}

#endif
