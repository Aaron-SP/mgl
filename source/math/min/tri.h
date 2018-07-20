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
#ifndef _MGL_MGL_TRI_MGL_
#define _MGL_MGL_TRI_MGL_

namespace min
{

template <typename T>
class tri
{
  private:
    T _x;
    T _y;
    T _z;

  public:
    tri() : _x(0.0), _y(0.0), _z(0.0) {}
    tri(const T x, const T y, const T z) : _x(x), _y(y), _z(z) {}
    inline T x() const
    {
        return _x;
    }
    inline T &x_ref()
    {
        return _x;
    }
    inline T y() const
    {
        return _y;
    }
    inline T &y_ref()
    {
        return _y;
    }
    inline T z() const
    {
        return _z;
    }
    inline T &z_ref()
    {
        return _z;
    }
    inline void x(const T x)
    {
        _x = x;
    }
    inline void y(const T y)
    {
        _y = y;
    }
    inline void z(const T z)
    {
        _z = z;
    }
    inline tri<T> &set_all(const T v)
    {
        _x = v;
        _y = v;
        _z = v;

        return *this;
    }
    inline T dot(const tri<T> &A) const
    {
        return _x * A.x() + _y * A.y() + _z * A.z();
    }
    inline T max() const
    {
        return std::max(std::max(_x, _y), _z);
    }
    inline T min() const
    {
        return std::min(std::min(_x, _y), _z);
    }
    inline tri<T> &operator+=(const T a)
    {
        _x += a;
        _y += a;
        _z += a;
        return *this;
    }
    inline tri<T> &operator+=(const tri<T> &A)
    {
        _x += A.x();
        _y += A.y();
        _z += A.z();
        return *this;
    }
    inline tri<T> &operator-=(const T a)
    {
        _x -= a;
        _y -= a;
        _z -= a;
        return *this;
    }
    inline tri<T> &operator-=(const tri<T> &A)
    {
        _x -= A.x();
        _y -= A.y();
        _z -= A.z();
        return *this;
    }
    inline tri<T> &operator*=(const T a)
    {
        _x *= a;
        _y *= a;
        _z *= a;
        return *this;
    }
    inline tri<T> &operator*=(const tri<T> &A)
    {
        _x *= A.x();
        _y *= A.y();
        _z *= A.z();
        return *this;
    }
    inline tri<T> &operator/=(const T a)
    {
        _x /= a;
        _y /= a;
        _z /= a;
        return *this;
    }
    inline tri<T> &operator/=(const tri<T> &A)
    {
        _x /= A.x();
        _y /= A.y();
        _z /= A.z();
        return *this;
    }
    inline tri<T> operator+(const T a) const
    {
        tri<T> temp = *this;
        temp += a;
        return temp;
    }
    inline tri<T> operator+(const tri<T> &A) const
    {
        tri<T> temp = *this;
        temp += A;
        return temp;
    }
    inline tri<T> operator-(const T a) const
    {
        tri<T> temp = *this;
        temp -= a;
        return temp;
    }
    inline tri<T> operator-(const tri<T> &A) const
    {
        tri<T> temp = *this;
        temp -= A;
        return temp;
    }
    inline tri<T> operator*(const T a) const
    {
        tri<T> temp = *this;
        temp *= a;
        return temp;
    }
    inline tri<T> operator*(const tri<T> &A) const
    {
        tri<T> temp = *this;
        temp *= A;
        return temp;
    }
    inline tri<T> operator/(const T a) const
    {
        tri<T> temp = *this;
        temp /= a;
        return temp;
    }
    inline tri<T> operator/(const tri<T> &A) const
    {
        tri<T> temp = *this;
        temp /= A;
        return temp;
    }
    inline bool operator>(const tri<T> &A) const
    {
        return _x > A._x && _y > A._y && _z > A._z;
    }
    inline bool operator>=(const tri<T> &A) const
    {
        return _x >= A._x && _y >= A._y && _z >= A._z;
    }
    inline bool operator<(const tri<T> &A) const
    {
        return _x < A._x && _y < A._y && _z < A._z;
    }
    inline bool operator<=(const tri<T> &A) const
    {
        return _x <= A._x && _y <= A._y && _z <= A._z;
    }
};
}

#endif
