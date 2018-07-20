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
#ifndef _MGL_BIT_FLAG_MGL_
#define _MGL_BIT_FLAG_MGL_

#include <cstdint>
#include <min/static_vector.h>
#include <utility>

namespace min
{
// Typename must be an unsigned integer type
template <typename K, typename L>
class bit_flag
{
  private:
    min::static_vector<uint8_t> _flags;
    K _col;
    K _row;

    inline std::pair<L, uint_fast8_t> get_address(const L row, const L col) const
    {
        // Divide by 8 to get into bytes
        const L position = (row * _col) + col;
        const L byte = position >> 3;

        // Get the offset
        // 0-7 value
        const uint_fast8_t offset = position % 8;

        // Return address
        return std::make_pair(byte, offset);
    }
    static inline L calc_size(const L row, const L col)
    {
        return (row * col >> 3) + 1;
    }

  public:
    bit_flag() : _col(0), _row(0) {}
    bit_flag(const L row, const L col)
        : _flags(calc_size(row, col)), _col(col), _row(row)
    {
        clear();
    }
    inline void clear()
    {
        _flags.zero();
    }
    inline bool get(const K row, const K col) const
    {
        // Get the address
        const std::pair<L, uint_fast8_t> addr = get_address(row, col);

        // Return 1 if on and zero if off
        return (_flags[addr.first] >> addr.second) & 0x1;
    }
    inline bool get_set_on(const K row, const K col)
    {
        // Get the address
        const std::pair<L, uint_fast8_t> addr = get_address(row, col);

        // Cache shift for the read/write operation
        const auto shift = (0x1 << addr.second);

        // Return 1 if on and zero if off
        const bool out = _flags[addr.first] & shift;

        // Set bit on
        _flags[addr.first] |= shift;

        return out;
    }
    inline void resize(const L row, const L col)
    {
        // Resize the flag buffer
        _flags.resize(calc_size(row, col));

        // Capture new size
        _col = col;
        _row = row;

        // Initialize memory
        clear();
    }
    inline void set_on(const K row, const K col)
    {
        // Get the address
        const std::pair<L, uint_fast8_t> addr = get_address(row, col);
        _flags[addr.first] |= (0x1 << addr.second);
    }
    inline void set_off(const K row, const K col)
    {
        // Get the address
        const std::pair<L, uint_fast8_t> addr = get_address(row, col);
        _flags[addr.first] &= ~(0x1 << addr.second);
    }
    inline K col() const
    {
        return _col;
    }
    inline K row() const
    {
        return _row;
    }
};
}

#endif
