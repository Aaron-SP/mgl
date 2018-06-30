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
#ifndef __MGL_BI__
#define __MGL_BI__

namespace min
{

template <typename T>
class bi
{
  private:
    T _x;
    T _y;

  public:
    bi() : _x(0.0), _y(0.0) {}
    bi(const T x, const T y) : _x(x), _y(y) {}
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
    inline void x(const T x)
    {
        _x = x;
    }
    inline void y(const T y)
    {
        _y = y;
    }
    inline bi<T> &set_all(const T v)
    {
        _x = v;
        _y = v;

        return *this;
    }
    inline T dot(const bi<T> &A) const
    {
        return _x * A.x() + _y * A.y();
    }
    inline T max() const
    {
        return std::max(_x, _y);
    }
    inline T min() const
    {
        return std::min(_x, _y);
    }
    inline bi<T> &operator+=(const T a)
    {
        _x += a;
        _y += a;
        return *this;
    }
    inline bi<T> &operator+=(const bi<T> &A)
    {
        _x += A.x();
        _y += A.y();
        return *this;
    }
    inline bi<T> &operator-=(const T a)
    {
        _x -= a;
        _y -= a;
        return *this;
    }
    inline bi<T> &operator-=(const bi<T> &A)
    {
        _x -= A.x();
        _y -= A.y();
        return *this;
    }
    inline bi<T> &operator*=(const T a)
    {
        _x *= a;
        _y *= a;
        return *this;
    }
    inline bi<T> &operator*=(const bi<T> &A)
    {
        _x *= A.x();
        _y *= A.y();
        return *this;
    }
    inline bi<T> &operator/=(const T a)
    {
        _x /= a;
        _y /= a;
        return *this;
    }
    inline bi<T> &operator/=(const bi<T> &A)
    {
        _x /= A.x();
        _y /= A.y();
        return *this;
    }
    inline bi<T> operator+(const T a) const
    {
        bi<T> temp = *this;
        temp += a;
        return temp;
    }
    inline bi<T> operator+(const bi<T> &A) const
    {
        bi<T> temp = *this;
        temp += A;
        return temp;
    }
    inline bi<T> operator-(const T a) const
    {
        bi<T> temp = *this;
        temp -= a;
        return temp;
    }
    inline bi<T> operator-(const bi<T> &A) const
    {
        bi<T> temp = *this;
        temp -= A;
        return temp;
    }
    inline bi<T> operator*(const T a) const
    {
        bi<T> temp = *this;
        temp *= a;
        return temp;
    }
    inline bi<T> operator*(const bi<T> &A) const
    {
        bi<T> temp = *this;
        temp *= A;
        return temp;
    }
    inline bi<T> operator/(const T a) const
    {
        bi<T> temp = *this;
        temp /= a;
        return temp;
    }
    inline bi<T> operator/(const bi<T> &A) const
    {
        bi<T> temp = *this;
        temp /= A;
        return temp;
    }
    inline bool operator>(const bi<T> &A) const
    {
        return _x > A._x && _y > A._y;
    }
    inline bool operator>=(const bi<T> &A) const
    {
        return _x >= A._x && _y >= A._y;
    }
    inline bool operator<(const bi<T> &A) const
    {
        return _x < A._x && _y < A._y;
    }
    inline bool operator<=(const bi<T> &A) const
    {
        return _x <= A._x && _y <= A._y;
    }
};
}

#endif
