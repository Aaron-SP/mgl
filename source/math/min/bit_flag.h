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
#ifndef __BIT_FLAG__
#define __BIT_FLAG__

#include <algorithm>
#include <cstdint>
#include <vector>

namespace min
{
// Typename must be an unsigned integer type
template <typename K, typename L>
class bit_flag
{
  private:
    K _row;
    K _col;
    std::vector<uint8_t> _flags;

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
};
}

#endif
