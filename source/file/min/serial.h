#ifndef __SERIALIZE__
#define __SERIALIZE__

#include <cstdint>
#include <min/vec2.h>
#include <min/vec3.h>
#include <min/vec4.h>
#include <stdexcept>
#include <vector>

// A CPU register is neither big or little endian
// However accessing bytes through a pointer is big or little endian specific
// Therefore we use bit shifts to hide the machine endianess from us, this also simplifies endianness issues
// A stream is declared either big or little endian and we convert the bytes to the host machine byte order using CPU register shifts
// Although this produces more instructions for the worst case,
// we do not have to constantly check the machine byte order through the program lifetime and this eliminates branching

namespace min
{

template <typename T>
inline T read_le(const std::vector<uint8_t> &stream, const size_t start)
{
    // Check type is compatible
    static_assert(sizeof(long long) >= sizeof(T), "Invalid type size, sizeof(T) <= sizeof(long long)");

    // Size of T in bytes
    const size_t size = sizeof(T);
    long long temp = 0;
    uint8_t shift = 0;
    for (size_t i = 0; i < size; i++)
    {
        // Unpack little endian stream
        temp |= ((long long)stream[start + i] << shift);
        shift += 8;
    }

    // Avoiding strict aliasing rules of C/C++
    const T *out = reinterpret_cast<T *>(&temp);
    return *out;
}

template <typename T>
inline T read_be(const std::vector<uint8_t> &stream, const size_t start)
{
    // Check type is compatible
    static_assert(sizeof(long long) >= sizeof(T), "Invalid type size, sizeof(T) <= sizeof(long long)");

    // Size of T in bytes
    const size_t size = sizeof(T);
    long long temp = 0;
    uint8_t shift = (sizeof(T) - 1) * 8;
    for (size_t i = 0; i < size; i++)
    {
        // Unpack big endian stream
        temp |= ((long long)stream[start + i] << shift);
        shift -= 8;
    }

    // Avoiding strict aliasing rules of C/C++
    const T *out = reinterpret_cast<T *>(&temp);
    return *out;
}

template <typename T>
inline void write_le(std::vector<uint8_t> &stream, const T data)
{
    // Pointer to data
    uint8_t *const ptr = (uint8_t *)&data;

    // Size of T in bytes
    const size_t size = sizeof(T);
    for (size_t i = 0; i < size; i++)
    {
        // Pack little endian stream
        stream.push_back(ptr[i]);
    }
}

template <typename T>
inline void write_be(std::vector<uint8_t> &stream, const T data)
{
    // Pointer to data
    uint8_t *const ptr = (uint8_t *)&data;

    // Size of T in bytes
    const size_t size = sizeof(T);
    const size_t offset = size - 1;
    for (size_t i = 0; i < size; i++)
    {
        // Pack big endian stream
        stream.push_back(ptr[offset - i]);
    }
}

template <typename T>
inline std::vector<T> read_le_vector(const std::vector<uint8_t> &stream, const size_t start)
{
    const uint32_t size = read_le<uint32_t>(stream, start);

    // Create output vector and reserve memory
    std::vector<T> out;
    out.reserve(size);

    // Calculate data offset
    const size_t offset = start + sizeof(uint32_t);

    // Check that the stream has enough data
    const size_t ssize = stream.size();
    if ((offset + sizeof(T) * size) > ssize)
    {
        throw std::runtime_error("read_le_vector: ran out of data in stream");
    }

    // Read all vector elements
    for (uint32_t i = 0; i < size; i++)
    {
        const T data = read_le<T>(stream, offset + sizeof(T) * i);
        out.push_back(data);
    }

    return out;
}

template <typename T>
inline std::vector<T> read_be_vector(const std::vector<uint8_t> &stream, const size_t start)
{
    const uint32_t size = read_be<uint32_t>(stream, start);

    // Create output vector and reserve memory
    std::vector<T> out;
    out.reserve(size);

    // Calculate data offset
    const size_t offset = start + sizeof(uint32_t);

    // Check that the stream has enough data
    const size_t ssize = stream.size();
    if ((offset + sizeof(T) * size) > ssize)
    {
        throw std::runtime_error("read_be_vector: ran out of data in stream");
    }

    // Read all vector elements
    for (uint32_t i = 0; i < size; i++)
    {
        const T data = read_be<T>(stream, offset + sizeof(T) * i);
        out.push_back(data);
    }

    return out;
}

template <typename T>
inline void write_le_vector(std::vector<uint8_t> &stream, const std::vector<T> &data)
{
    // Get data size, must be less than 2^32-1
    const uint32_t size = data.size();

    // Write vector size to stream, zero vector is allowed
    write_le<uint32_t>(stream, size);

    // Write each data element
    for (uint32_t i = 0; i < size; i++)
    {
        write_le<T>(stream, data[i]);
    }
}

template <typename T>
inline void write_be_vector(std::vector<uint8_t> &stream, const std::vector<T> &data)
{
    // Get data size, must be less than 2^32-1
    const uint32_t size = data.size();

    // Write vector size to stream, zero vector is allowed
    write_be<uint32_t>(stream, size);

    // Write each data element
    for (uint32_t i = 0; i < size; i++)
    {
        write_be<T>(stream, data[i]);
    }
}

template <typename T>
inline min::vec2<T> read_le_vec2(std::vector<uint8_t> &stream, const size_t start)
{
    const T x = read_le<T>(stream, start);
    const T y = read_le<T>(stream, start + sizeof(T));
    return min::vec2<T>(x, y);
}
template <typename T>
inline min::vec2<T> read_be_vec2(std::vector<uint8_t> &stream, const size_t start)
{
    const T x = read_be<T>(stream, start);
    const T y = read_be<T>(stream, start + sizeof(T));
    return min::vec2<T>(x, y);
}
template <typename T>
inline min::vec3<T> read_le_vec3(std::vector<uint8_t> &stream, const size_t start)
{
    const T x = read_le<T>(stream, start);
    const T y = read_le<T>(stream, start + sizeof(T));
    const T z = read_le<T>(stream, start + sizeof(T) * 2);
    return min::vec3<T>(x, y, z);
}
template <typename T>
inline min::vec3<T> read_be_vec3(std::vector<uint8_t> &stream, const size_t start)
{
    const T x = read_be<T>(stream, start);
    const T y = read_be<T>(stream, start + sizeof(T));
    const T z = read_be<T>(stream, start + sizeof(T) * 2);
    return min::vec3<T>(x, y, z);
}
template <typename T>
inline min::vec4<T> read_le_vec4(std::vector<uint8_t> &stream, const size_t start)
{
    const T x = read_le<T>(stream, start);
    const T y = read_le<T>(stream, start + sizeof(T));
    const T z = read_le<T>(stream, start + sizeof(T) * 2);
    const T w = read_le<T>(stream, start + sizeof(T) * 3);
    return min::vec4<T>(x, y, z, w);
}
template <typename T>
inline min::vec4<T> read_be_vec4(std::vector<uint8_t> &stream, const size_t start)
{
    const T x = read_be<T>(stream, start);
    const T y = read_be<T>(stream, start + sizeof(T));
    const T z = read_be<T>(stream, start + sizeof(T) * 2);
    const T w = read_be<T>(stream, start + sizeof(T) * 3);
    return min::vec4<T>(x, y, z, w);
}
template <typename T>
inline void write_le_vec2(std::vector<uint8_t> &stream, const min::vec2<T> &v)
{
    write_le<T>(stream, v.x());
    write_le<T>(stream, v.y());
}
template <typename T>
inline void write_be_vec2(std::vector<uint8_t> &stream, const min::vec2<T> &v)
{
    write_be<T>(stream, v.x());
    write_be<T>(stream, v.y());
}
template <typename T>
inline void write_le_vec3(std::vector<uint8_t> &stream, const min::vec3<T> &v)
{
    write_le<T>(stream, v.x());
    write_le<T>(stream, v.y());
    write_le<T>(stream, v.z());
}
template <typename T>
inline void write_be_vec3(std::vector<uint8_t> &stream, const min::vec3<T> &v)
{
    write_be<T>(stream, v.x());
    write_be<T>(stream, v.y());
    write_be<T>(stream, v.z());
}
template <typename T>
inline void write_le_vec4(std::vector<uint8_t> &stream, const min::vec4<T> &v)
{
    write_le<T>(stream, v.x());
    write_le<T>(stream, v.y());
    write_le<T>(stream, v.z());
    write_le<T>(stream, v.w());
}
template <typename T>
inline void write_be_vec4(std::vector<uint8_t> &stream, const min::vec4<T> &v)
{
    write_be<T>(stream, v.x());
    write_be<T>(stream, v.y());
    write_be<T>(stream, v.z());
    write_be<T>(stream, v.w());
}

template <typename T>
inline std::vector<min::vec2<T>> read_le_vector_vec2(std::vector<uint8_t> &stream, const size_t start)
{
    const uint32_t size = read_le<uint32_t>(stream, start);

    // Create output vector and reserve memory
    std::vector<min::vec2<T>> out;
    out.reserve(size);

    // Calculate data offset
    const size_t offset = start + sizeof(uint32_t);

    // Check that the stream has enough data
    const size_t ssize = stream.size();
    if ((offset + sizeof(min::vec2<T>) * size) > ssize)
    {
        throw std::runtime_error("read_le_vector_vec2: ran out of data in stream");
    }

    // Read all vector elements
    for (uint32_t i = 0; i < size; i++)
    {
        const min::vec2<T> data = read_le_vec2<T>(stream, offset + sizeof(min::vec2<T>) * i);
        out.push_back(data);
    }

    return out;
}
template <typename T>
inline std::vector<min::vec2<T>> read_be_vector_vec2(std::vector<uint8_t> &stream, const size_t start)
{
    const uint32_t size = read_be<uint32_t>(stream, start);

    // Create output vector and reserve memory
    std::vector<min::vec2<T>> out;
    out.reserve(size);

    // Calculate data offset
    const size_t offset = start + sizeof(uint32_t);

    // Check that the stream has enough data
    const size_t ssize = stream.size();
    if ((offset + sizeof(min::vec2<T>) * size) > ssize)
    {
        throw std::runtime_error("read_be_vector_vec2: ran out of data in stream");
    }

    // Read all vector elements
    for (uint32_t i = 0; i < size; i++)
    {
        const min::vec2<T> data = read_be_vec2<T>(stream, offset + sizeof(min::vec2<T>) * i);
        out.push_back(data);
    }

    return out;
}
template <typename T>
inline std::vector<min::vec3<T>> read_le_vector_vec3(std::vector<uint8_t> &stream, const size_t start)
{
    const uint32_t size = read_le<uint32_t>(stream, start);

    // Create output vector and reserve memory
    std::vector<min::vec3<T>> out;
    out.reserve(size);

    // Calculate data offset
    const size_t offset = start + sizeof(uint32_t);

    // Check that the stream has enough data
    const size_t ssize = stream.size();
    if ((offset + sizeof(min::vec3<T>) * size) > ssize)
    {
        throw std::runtime_error("read_le_vector_vec3: ran out of data in stream");
    }

    // Read all vector elements
    for (uint32_t i = 0; i < size; i++)
    {
        const min::vec3<T> data = read_le_vec3<T>(stream, offset + sizeof(min::vec3<T>) * i);
        out.push_back(data);
    }

    return out;
}
template <typename T>
inline std::vector<min::vec3<T>> read_be_vector_vec3(std::vector<uint8_t> &stream, const size_t start)
{
    const uint32_t size = read_be<uint32_t>(stream, start);

    // Create output vector and reserve memory
    std::vector<min::vec3<T>> out;
    out.reserve(size);

    // Calculate data offset
    const size_t offset = start + sizeof(uint32_t);

    // Check that the stream has enough data
    const size_t ssize = stream.size();
    if ((offset + sizeof(min::vec3<T>) * size) > ssize)
    {
        throw std::runtime_error("read_be_vector_vec3: ran out of data in stream");
    }

    // Read all vector elements
    for (uint32_t i = 0; i < size; i++)
    {
        const min::vec3<T> data = read_be_vec3<T>(stream, offset + sizeof(min::vec3<T>) * i);
        out.push_back(data);
    }

    return out;
}
template <typename T>
inline std::vector<min::vec4<T>> read_le_vector_vec4(std::vector<uint8_t> &stream, const size_t start)
{
    const uint32_t size = read_le<uint32_t>(stream, start);

    // Create output vector and reserve memory
    std::vector<min::vec4<T>> out;
    out.reserve(size);

    // Calculate data offset
    const size_t offset = start + sizeof(uint32_t);

    // Check that the stream has enough data
    const size_t ssize = stream.size();
    if ((offset + sizeof(min::vec4<T>) * size) > ssize)
    {
        throw std::runtime_error("read_le_vector_vec4: ran out of data in stream");
    }

    // Read all vector elements
    for (uint32_t i = 0; i < size; i++)
    {
        const min::vec4<T> data = read_le_vec4<T>(stream, offset + sizeof(min::vec4<T>) * i);
        out.push_back(data);
    }

    return out;
}
template <typename T>
inline std::vector<min::vec4<T>> read_be_vector_vec4(std::vector<uint8_t> &stream, const size_t start)
{
    const uint32_t size = read_be<uint32_t>(stream, start);

    // Create output vector and reserve memory
    std::vector<min::vec4<T>> out;
    out.reserve(size);

    // Calculate data offset
    const size_t offset = start + sizeof(uint32_t);

    // Check that the stream has enough data
    const size_t ssize = stream.size();
    if ((offset + sizeof(min::vec4<T>) * size) > ssize)
    {
        throw std::runtime_error("read_be_vector_vec4: ran out of data in stream");
    }

    // Read all vector elements
    for (uint32_t i = 0; i < size; i++)
    {
        const min::vec4<T> data = read_be_vec4<T>(stream, offset + sizeof(min::vec4<T>) * i);
        out.push_back(data);
    }

    return out;
}
template <typename T>
inline void write_le_vector_vec2(std::vector<uint8_t> &stream, const std::vector<vec2<T>> &data)
{
    // Get data size, must be less than 2^32-1
    const uint32_t size = data.size();

    // Write vector size to stream, zero vector is allowed
    write_le<uint32_t>(stream, size);

    // Write each data element
    for (uint32_t i = 0; i < size; i++)
    {
        write_le_vec2<T>(stream, data[i]);
    }
}
template <typename T>
inline void write_be_vector_vec2(std::vector<uint8_t> &stream, const std::vector<vec2<T>> &data)
{
    // Get data size, must be less than 2^32-1
    const uint32_t size = data.size();

    // Write vector size to stream, zero vector is allowed
    write_be<uint32_t>(stream, size);

    // Write each data element
    for (uint32_t i = 0; i < size; i++)
    {
        write_be_vec2<T>(stream, data[i]);
    }
}
template <typename T>
inline void write_le_vector_vec3(std::vector<uint8_t> &stream, const std::vector<vec3<T>> &data)
{
    // Get data size, must be less than 2^32-1
    const uint32_t size = data.size();

    // Write vector size to stream, zero vector is allowed
    write_le<uint32_t>(stream, size);

    // Write each data element
    for (uint32_t i = 0; i < size; i++)
    {
        write_le_vec3<T>(stream, data[i]);
    }
}
template <typename T>
inline void write_be_vector_vec3(std::vector<uint8_t> &stream, const std::vector<vec3<T>> &data)
{
    // Get data size, must be less than 2^32-1
    const uint32_t size = data.size();

    // Write vector size to stream, zero vector is allowed
    write_be<uint32_t>(stream, size);

    // Write each data element
    for (uint32_t i = 0; i < size; i++)
    {
        write_be_vec3<T>(stream, data[i]);
    }
}
template <typename T>
inline void write_le_vector_vec4(std::vector<uint8_t> &stream, const std::vector<vec4<T>> &data)
{
    // Get data size, must be less than 2^32-1
    const uint32_t size = data.size();

    // Write vector size to stream, zero vector is allowed
    write_le<uint32_t>(stream, size);

    // Write each data element
    for (uint32_t i = 0; i < size; i++)
    {
        write_le_vec4<T>(stream, data[i]);
    }
}
template <typename T>
inline void write_be_vector_vec4(std::vector<uint8_t> &stream, const std::vector<vec4<T>> &data)
{
    // Get data size, must be less than 2^32-1
    const uint32_t size = data.size();

    // Write vector size to stream, zero vector is allowed
    write_be<uint32_t>(stream, size);

    // Write each data element
    for (uint32_t i = 0; i < size; i++)
    {
        write_be_vec4<T>(stream, data[i]);
    }
}
}
#endif
