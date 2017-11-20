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
#ifndef __QUATERNION__
#define __QUATERNION__

namespace min
{
template <typename T>
class vec3;
}

#include <cmath>
#include <min/utility.h>
#include <min/vec3.h>
#include <stdexcept>

namespace min
{

template <typename T>
class quat
{
  private:
    T _w;
    T _x;
    T _y;
    T _z;

    inline quat<T> operator+(const quat<T> &q) const
    {
        const T w = _w + q.w();
        const T x = _x + q.x();
        const T y = _y + q.y();
        const T z = _z + q.z();
        return quat<T>(w, x, y, z);
    }
    inline quat<T> operator*(const T a) const
    {
        const T w = _w * a;
        const T x = _x * a;
        const T y = _y * a;
        const T z = _z * a;
        return quat<T>(w, x, y, z);
    }
    inline quat<T> &operator-=(const quat<T> &q)
    {
        _w -= q.w();
        _x -= q.x();
        _y -= q.y();
        _z -= q.z();
        return *this;
    }

  public:
    quat() : _w(1.0), _x(0.0), _y(0.0), _z(0.0) {}
    quat(const T w, const T x, const T y, const T z) : _w(w), _x(x), _y(y), _z(z) {}
    quat(const T x, const T y, const T z) : _w(1.0), _x(x), _y(y), _z(z)
    {
        normalize();
    }
    quat(const vec3<T> &v) : _w(1.0), _x(v.x()), _y(v.y()), _z(v.z())
    {
        normalize();
    }
    quat(const vec3<T> &vector, const T degAngle)
    {
        const T angle = deg_to_rad(degAngle);
        const T sina_2 = sin(angle / 2.0);
        _w = cos(angle / 2.0);
        _x = vector.x() * sina_2;
        _y = vector.y() * sina_2;
        _z = vector.z() * sina_2;

        // Normalize
        normalize();
    }
    // must be normalized vectors
    quat(const vec3<T> &v1, const vec3<T> &v2)
    {
        const T cos_theta = v1.dot(v2);
        if (cos_theta < var<T>::TOL_NONE)
        {
            vec3<T> c = v1.cross_y();
            if (c.magnitude() < var<T>::TOL_REL)
            {
                c = v1.cross_x();
            }
            c.normalize();
            _w = 0.0;
            _x = c.x();
            _y = c.y();
            _z = c.z();
        }
        else if (cos_theta > var<T>::TOL_PONE)
        {
            _w = 1.0;
            _x = 0.0;
            _y = 0.0;
            _z = 0.0;
        }
        else
        {
            const T s = std::sqrt((1 + cos_theta) * 2);
            const T inv_s = 1.0 / s;
            const vec3<T> c = v1.cross(v2);
            _w = s / 2.0;
            _x = c.x() * inv_s;
            _y = c.y() * inv_s;
            _z = c.z() * inv_s;
            normalize();
        }
    }
    // must be normalized vectors
    quat(const vec3<T> &v1, const vec3<T> &v2, const vec3<T> &v3)
    {
        const T cos_theta = v1.dot(v2);
        if (cos_theta < var<T>::TOL_NONE)
        {
            _w = 0.0;
            _x = v3.x();
            _y = v3.y();
            _z = v3.z();
        }
        else if (cos_theta > var<T>::TOL_PONE)
        {
            _w = 1.0;
            _x = 0.0;
            _y = 0.0;
            _z = 0.0;
        }
        else
        {
            const T s = std::sqrt((1 + cos_theta) * 2);
            const T inv_s = 1.0 / s;
            const vec3<T> c = v1.cross(v2);
            _w = s / 2.0;
            _x = c.x() * inv_s;
            _y = c.y() * inv_s;
            _z = c.z() * inv_s;
            normalize();
        }
    }
    inline void calculate_w()
    {
        const T t = 1.0 - (_x * _x) - (_y * _y) - (_z * _z);
        if (t < 0.0)
        {
            _w = 0.0;
        }
        else
        {
            _w = sqrt(t);
        }

        // Normalize the quaternion to ensure unit length
        normalize();
    }
    inline quat<T> conjugate() const
    {
        const T w = _w;
        const T x = -_x;
        const T y = -_y;
        const T z = -_z;
        return quat<T>(w, x, y, z);
    }
    inline T dot(const quat<T> &q) const
    {
        return _w * q.w() + _x * q.x() + _y * q.y() + _z * q.z();
    }
    inline quat<T> inverse() const
    {
        return conjugate();
    }
    // Linear interpolation
    inline static quat<T> lerp(const quat<T> &v0, const quat<T> &v1, const T interpolation)
    {
        const T w = v0.w() + (v1.w() - v0.w()) * interpolation;
        const T x = v0.x() + (v1.x() - v0.x()) * interpolation;
        const T y = v0.y() + (v1.y() - v0.y()) * interpolation;
        const T z = v0.z() + (v1.z() - v0.z()) * interpolation;
        return quat<T>(w, x, y, z).normalize();
    }
    inline static quat<T> interpolate(const quat<T> &v0, const quat<T> &v1, const T t)
    {
        return slerp(v0, v1, t);
    }
    inline T magnitude() const
    {
        return sqrt(_w * _w + _x * _x + _y * _y + _z * _z);
    }
    inline quat<T> &normalize()
    {
        const T mag = magnitude();
        _w /= mag;
        _x /= mag;
        _y /= mag;
        _z /= mag;

        return *this;
    }
    // Spherical interpolation
    inline static quat<T> slerp(const quat<T> &v0, const quat<T> &v1, const T t)
    {
        quat<T> v2 = v1;
        T cos_theta = v0.dot(v1);

        // If two quaternions are too close,
        if (cos_theta > 0.95)
        {
            return quat<T>::lerp(v0, v1, t);
        }
        // If dot product is negative reverse the quaternion
        // to take shortest path
        else if (cos_theta < 0.0)
        {
            v2 = v1.conjugate();
            cos_theta = -cos_theta;
        }

        clamp<T>(cos_theta, -1.0, 1.0);
        T theta = std::acos(cos_theta) * t;

        // v1 - v0 * cos(theta)
        v2 -= v0 * cos_theta;
        v2.normalize();

        return (v0 * std::cos(theta)) + (v2 * std::sin(theta));
    }
    inline vec3<T> transform(const vec3<T> &p) const
    {
        // Get the pure quaternion P
        const quat<T> P = quat<T>(0.0, p.x(), p.y(), p.z());

        // Transform to Q
        const quat<T> Q = (*this) * P * conjugate();

        // Return the rotated vector part
        return vec3<T>(Q.x(), Q.y(), Q.z());
    }
    inline quat<T> operator*(const quat<T> &b) const
    {
        const T w = b.w() * _w - b.x() * _x - b.y() * _y - b.z() * _z;
        const T x = b.w() * _x + b.x() * _w - b.y() * _z + b.z() * _y;
        const T y = b.w() * _y + b.x() * _z + b.y() * _w - b.z() * _x;
        const T z = b.w() * _z - b.x() * _y + b.y() * _x + b.z() * _w;
        return quat<T>(w, x, y, z);
    }
    inline quat<T> &operator*=(const quat<T> &b)
    {
        const T w = b.w() * _w - b.x() * _x - b.y() * _y - b.z() * _z;
        const T x = b.w() * _x + b.x() * _w - b.y() * _z + b.z() * _y;
        const T y = b.w() * _y + b.x() * _z + b.y() * _w - b.z() * _x;
        const T z = b.w() * _z - b.x() * _y + b.y() * _x + b.z() * _w;

        _w = w;
        _x = x;
        _y = y;
        _z = z;

        return *this;
    }
    inline T w() const
    {
        return _w;
    }
    inline T x() const
    {
        return _x;
    }
    inline void x(const T x)
    {
        _x = x;
    }
    inline T y() const
    {
        return _y;
    }
    inline void y(const T y)
    {
        _y = y;
    }
    inline T z() const
    {
        return _z;
    }
    inline void z(const T z)
    {
        _z = z;
    }
};
}

#endif
