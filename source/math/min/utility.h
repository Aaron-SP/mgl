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
#ifndef __UTILITY__
#define __UTILITY__

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace min
{

template <typename T>
class var
{
  public:
    static constexpr T PI = 3.14159265358979323846;
    static constexpr T SQRT2 = 1.4142135623731;
    static constexpr T INV_SQRT2 = 0.707106781186547;
    static constexpr T SQRT3 = 1.73205080756888;
    static constexpr T INV_SQRT3 = 0.577350269189626;
    static constexpr T TOL_PHYS_EDGE = 1.0;
    static constexpr T TOL_PONE = 1.0 - 1E-6;
    static constexpr T TOL_NONE = -1.0 + 1E-6;
    static constexpr T TOL_REL = 1E-6;
    static constexpr T TOL_REL2 = 2E-6;

    // Use this for taking inverses without getting a NaN
    static constexpr T TOL_ZERO = 1E-3;
};

template <typename T>
constexpr inline T rad_to_deg(const T rad)
{
    // Converts degrees to radians
    return rad * (180.0 / var<T>::PI);
}

template <typename T>
constexpr inline T deg_to_rad(const T degrees)
{
    // Converts degrees to radians
    return degrees * (var<T>::PI / 180.0);
}
template <typename T>
constexpr inline T deg_to_rad2(const T degrees)
{
    // Converts degrees to radians / 2
    return degrees * (var<T>::PI / 360.0);
}

template <typename T>
inline bool between(const T val, const T min, const T max)
{
    return (val > min) && (val < max);
}

template <typename T>
inline void clamp(T &val, const T min, const T max)
{
    // Clamps val between min and max
    if (val < min)
        val = min;
    else if (val > max)
        val = max;
}

template <typename T>
inline T clamp_direction(T &val, const T min, const T max)
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
inline T clamp_value(const T val, const T min, const T minv, const T max, const T maxv)
{
    // Clamps val between min and max and assigns either minv, maxv or zero
    if (val < min)
    {
        return minv;
    }
    else if (val > max)
    {
        return maxv;
    }

    return 0.0;
}

template <typename T>
inline void extend(const T val, T &min, T &max)
{
    // extends min or max to val
    if (val < min)
        min = val;
    else if (val > max)
        max = val;
}

template <typename T>
inline T safe_inverse(const T v)
{
    T out;

    // Test for division by zero
    if (std::abs(v) < var<T>::TOL_REL)
    {
        out = std::numeric_limits<T>::max();
    }
    else
    {
        out = 1.0 / v;
    }

    return out;
}

template <typename T>
inline int sgn(const T val)
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
}

#endif
