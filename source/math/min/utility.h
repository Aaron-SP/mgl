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

#include <algorithm>
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
bool between(const T val, const T min, const T max)
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
int sgn(const T val)
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
    inline bool get_set_on(const K row, const K col)
    {
        // Get the address
        const std::pair<L, uint8_t> addr = get_address(row, col);

        // Cache shift for the read/write operation
        const auto shift = (0x1 << addr.second);

        // Return 1 if on and zero if off
        const bool out = _flags[addr.first] & shift;

        // Set bit on
        _flags[addr.first] |= shift;

        return out;
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

// radix sort for unsigned integers
template <typename T, typename F>
inline void uint_sort(std::vector<T> &uints, F &&key_function)
{
    const size_t size = uints.size();

    // Divert to std::sort for size < 128; 2*N > N*log(N)
    if (size < 128)
    {
        return std::sort(uints.begin(), uints.end(), [key_function](const size_t a, const size_t b) {
            const size_t a_key = key_function(a);
            const size_t b_key = key_function(b);
            return a_key < b_key;
        });
    }

    // Initialize copy vector
    std::vector<T> copy(size, 0);
    std::vector<T> *from = &uints;
    std::vector<T> *to = &copy;
    std::vector<T> *temp = nullptr;

    // Number of passes
    size_t counts[256];
    const int passes = sizeof(T) / sizeof(uint8_t);

    for (int i = 0; i < passes; i++)
    {
        // zero counts
        for (size_t &count : counts)
        {
            count = 0;
        }

        // count frequency
        for (const auto &ui : *from)
        {
            // Extract the key
            const uint8_t key = (key_function(ui) >> 8 * i) & (0xFF);

            // Count the frequency of key
            counts[key]++;
        }

        // prefix sum
        size_t total = 0;
        for (size_t &count : counts)
        {
            const size_t old_count = count;
            count = total;
            total += old_count;
        }

        // sort
        for (const auto &ui : *from)
        {
            // Extract the key
            const uint8_t key = (key_function(ui) >> 8 * i) & (0xFF);

            // perform copy sort
            (*to)[counts[key]++] = ui;
        }

        // Swap from/to for next pass
        temp = from;
        from = to;
        to = temp;
    }

    // *from was the last *to
    // Copy sorted array into output, if needed
    if (from != &uints)
    {
        for (size_t i = 0; i < size; i++)
        {
            uints[i] = (*from)[i];
        }
    }
}
}

#endif
