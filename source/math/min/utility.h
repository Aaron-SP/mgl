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
#ifndef __UTILITY__
#define __UTILITY__

#include <cstdint>
#include <vector>

namespace min
{

template <typename T>
class var
{
  public:
    const static T PI;
    const static T SQRT2;
    const static T INV_SQRT2;
    const static T SQRT3;
    const static T INV_SQRT3;
};

template <class T>
const T var<T>::PI = 3.14159265358979323846;
template <class T>
const T var<T>::SQRT2 = 1.4142135623731;
template <class T>
const T var<T>::INV_SQRT2 = 0.707106781186547;
template <class T>
const T var<T>::SQRT3 = 1.73205080756888;
template <class T>
const T var<T>::INV_SQRT3 = 0.577350269189626;

template <typename T>
constexpr inline T deg_to_rad(T degrees)
{
    // Converts degrees to radians
    return degrees * var<T>::PI / 180.0;
}
template <typename T>
constexpr inline T deg_to_rad2(T degrees)
{
    // Converts degrees to radians / 2
    return degrees * var<T>::PI / 360.0;
}

template <typename T>
inline void clamp(T &val, T min, T max)
{
    // Clamps val between min and max
    if (val < min)
        val = min;
    else if (val > max)
        val = max;
}

template <typename T>
inline T clamp_direction(T &val, T min, T max)
{
    // Clamps val between min and max
    if (val < min)
    {
        val = min;
        return -1.0;
    }
    else if (val > max)
    {
        val = max;
        return -1.0;
    }

    return 1.0;
}

template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

template <typename T>
inline void swap(T &a, T &b)
{
    // Swaps a and b
    T _a = a;
    a = b;
    b = _a;
}

template <typename T>
inline void move(std::vector<T> &&src, std::vector<T> &dst)
{
    // This is necessary to steal data on GCC 6.3
    if (dst.size() == 0)
    {
        // This actually moves the data
        dst = std::move(src);
    }
    else
    {
        // Copies the data to the end of destination vector
        std::move(src.begin(), src.end(), std::back_inserter(dst));
    }
}

// Typename must be an unsigned integer type
template <typename K, typename L>
class bit_flag
{
  private:
    K _row;
    K _col;
    std::vector<uint8_t> _flags;

    inline std::pair<L, uint8_t> get_address(const L row, const L col) const
    {
        // Divide by 8 to get into bytes
        const L position = (row * _col) + col;
        const L byte = position >> 3;

        // Get the offset
        // 0-7 value
        const uint8_t offset = position % 8;

        // Return address
        return std::make_pair(byte, offset);
    }

  public:
    bit_flag() : _row(0), _col(0) {}
    bit_flag(const L row, const L col) : _row(row), _col(col), _flags((row * col >> 3) + 1, 0) {}
    inline void clear()
    {
        // Zero out the bit buffer
        std::fill(_flags.begin(), _flags.end(), 0);
    }
    inline bool get(const K row, const K col) const
    {
        // Get the address
        const std::pair<L, uint8_t> addr = get_address(row, col);

        // Return 1 if on and zero if off
        return (_flags[addr.first] >> addr.second) & 0x1;
    }
    inline void set_on(const K row, const K col)
    {
        // Get the address
        const std::pair<L, uint8_t> addr = get_address(row, col);
        _flags[addr.first] |= (0x1 << addr.second);
    }
    inline void set_off(const K row, const K col)
    {
        // Get the address
        const std::pair<L, uint8_t> addr = get_address(row, col);
        _flags[addr.first] &= ~(0x1 << addr.second);
    }
};
}

#endif