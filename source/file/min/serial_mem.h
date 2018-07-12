#ifndef __SERIALIZE_MEM__
#define __SERIALIZE_MEM__

#include <cstdint>
#include <cstring>
#include <min/serial.h>
#include <min/vec2.h>
#include <min/vec3.h>
#include <min/vec4.h>
#include <stdexcept>
#include <string>
#include <vector>

// A CPU register is neither big or little endian
// However accessing bytes through a pointer is big or little endian specific
// Therefore we use bit shifts to hide the machine endianess from us, this also simplifies endianness issues
// A stream is declared either big or little endian and we convert the bytes to the host machine byte order using CPU register shifts
// Although this produces more instructions for the worst case,
// we do not have to constantly check the machine byte order through the program lifetime and this eliminates branching

namespace min
{

class mem_file
{
  private:
    std::vector<uint8_t> *const _data;
    size_t _offset;
    size_t _size;

  public:
    mem_file(std::vector<uint8_t> *const data, const size_t offset, const size_t size)
        : _data(data), _offset(offset), _size(size) {}
    inline const uint8_t &operator[](const size_t index) const
    {
        return (*_data)[_offset + index];
    }
    inline uint8_t &operator[](const size_t index)
    {
        return (*_data)[_offset + index];
    }
    inline size_t offset() const
    {
        return _offset;
    }
    inline size_t size() const
    {
        return _size;
    }
    inline std::string to_string() const
    {
        // Allocate space for the bytes
        std::string out(_size, 0);

        // Copy data into string
        std::memcpy(&out[0], &(*_data)[_offset], _size);

        // Return the copied string
        return out;
    }
};
}
#endif
