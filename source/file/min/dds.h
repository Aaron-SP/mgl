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
#ifndef _MGL_DDS_MGL_
#define _MGL_DDS_MGL_

#include <algorithm>
#include <cstring>
#include <fstream>
#include <min/mem_chunk.h>
#include <min/static_vector.h>
#include <string>

namespace min
{

class dds
{
  public:
    static constexpr uint32_t DXT1 = 0x31545844;
    static constexpr uint32_t DXT3 = 0x33545844;
    static constexpr uint32_t DXT5 = 0x35545844;

  private:
    static constexpr uint8_t DDS_HEADER_SIZE = 128;
    min::static_vector<uint8_t> _pixel;
    uint32_t _w;
    uint32_t _h;
    uint32_t _size;
    uint32_t _bpp;
    uint32_t _mips;
    uint32_t _format;

    inline uint32_t calculate_size() const
    {
        // Calculate the expected size of the pixel buffer
        unsigned int block_size = (_format == DXT1) ? 8 : 16;
        unsigned int width = _w;
        unsigned int height = _h;
        unsigned int total = 0;

        // Calculate the total size of the image
        for (size_t i = 0; i < _mips; i++)
        {
            total += ((width + 3) / 4) * ((height + 3) / 4) * block_size;

            // Calculate width and height for next level, accurate for non-power of two textures
            width = std::max(1u, width / 2);
            height = std::max(1u, height / 2);
        }

        return total;
    }
    inline void check_size()
    {
        // Verify dds has correct size
        uint32_t expected = calculate_size();
        if (_size != expected)
        {
            throw std::runtime_error("dds: Expected image size '" + std::to_string(expected) + "' got '" + std::to_string(_size) + "'");
        }
    }
    inline void load(const std::string _file)
    {
        std::ifstream file(_file, std::ios::in | std::ios::binary | std::ios::ate);
        if (file.is_open())
        {
            // Get the size of the file
            const size_t size = static_cast<size_t>(file.tellg());

            // Adjust file pointer to beginning
            file.seekg(0, std::ios::beg);

            // Allocate space for new file
            min::static_vector<uint8_t> data(size);

            // Read bytes and close the file
            char *ptr = reinterpret_cast<char *>(data.data());
            file.read(ptr, size);

            // Close the file
            file.close();

            // Process the DDS file
            load<min::static_vector<uint8_t>>(data);
        }
        else
        {
            throw std::runtime_error("dds: Could not load file '" + _file + "'");
        }
    }
    template <class T>
    inline void load(const T &data)
    {
        // Check that nothing funky is going on with char and uint8_t
        static_assert(sizeof(char) == 1, "Size of char is not 1");
        static_assert(sizeof(uint8_t) == 1, "Size of uint8_t is not 1");
        static_assert(std::is_same<std::uint8_t, unsigned char>::value,
                      "std::uint8_t must be implemented as unsigned char");

        // Get the size of the file and check against header size
        const size_t size = data.size();
        if (size < DDS_HEADER_SIZE)
        {
            throw std::runtime_error("dds: File not large enough to be dds file");
        }

        // Read the DDS_HEADER_SIZE byte header
        if (data[0] != 'D' && data[1] != 'D' && data[2] != 'S' && data[3] != ' ')
        {
            throw std::runtime_error("dds: Invalid dds header");
        }

        // 4 bytes the height of the image
        size_t next = 12;
        _h = read_le<uint32_t>(data, next);

        // 4 bytes the width of the image
        next = 16;
        _w = read_le<uint32_t>(data, next);

        // 4 bytes the linear size of the image
        next = 20;
        _size = read_le<uint32_t>(data, next);

        // 4 bytes the number of mip maps in this image
        next = 28;
        _mips = read_le<uint32_t>(data, next);

        // 4 bytes the fourCC value from the data
        next = 84;
        _format = read_le<uint32_t>(data, next);

        // Check format
        if (_format != DXT1 && _format != DXT3 && _format != DXT5)
        {
            throw std::runtime_error("dds: Unsupported DXT format value of '" + std::to_string(_format) + "'");
        }

        // If _format == DXT1 channels = 3, else 4
        _bpp = (_format == DXT1) ? 3 : 4;

        // Verify dds has correct size
        check_size();

        // Check that we have data
        if (_size == 0)
        {
            throw std::runtime_error("dds: image has zero pixel data");
        }

        // Check the file size against image size
        if (size < DDS_HEADER_SIZE + _size)
        {
            throw std::runtime_error("dds: File image size is corrupted, possibly missing data");
        }

        // Read the pixel data
        _pixel.resize(_size);
        std::memcpy(_pixel.data(), &data[DDS_HEADER_SIZE], _size);
    }

  public:
    dds(const std::string &file)
    {
        load(file);
    }
    dds(const mem_file &mem)
    {
        load<mem_file>(mem);
    }
    dds(const uint32_t w, const uint32_t h, const uint32_t mips, const uint32_t format, const min::static_vector<uint8_t> &pixel)
        : _pixel(pixel), _w(w), _h(h), _mips(mips), _format(format)
    {
        // Check that we have data
        if (_pixel.size() == 0)
        {
            throw std::runtime_error("dds: No pixel data provided to explicit constructor");
        }

        // Set the size of pixel data
        _size = _pixel.size();

        // Check format
        if (_format != DXT1 && _format != DXT3 && _format != DXT5)
        {
            throw std::runtime_error("dds: Unsupported DXT format value of '" + std::to_string(_format) + "'");
        }

        // If _format == DXT1 channels = 3, else 4
        _bpp = (_format == DXT1) ? 3 : 4;

        // Check mip levels
        if (_mips <= 0)
        {
            throw std::runtime_error("dds: Atleast one mipmap level is required");
        }

        // Verify dds has correct size
        check_size();
    }
    inline uint32_t get_format() const
    {
        return _format;
    }
    inline uint32_t get_mips() const
    {
        return _mips;
    }
    inline uint32_t get_width() const
    {
        return _w;
    }
    inline uint32_t get_height() const
    {
        return _h;
    }
    inline uint32_t get_size() const
    {
        return _size;
    }
    inline const min::static_vector<uint8_t> &get_pixels() const
    {
        return _pixel;
    }
    inline min::static_vector<uint8_t> to_file() const
    {
        // Write out the dds file to a byte buffer for writing to file
        size_t size = DDS_HEADER_SIZE + _size;
        min::static_vector<uint8_t> out(size);

        // The DDS header
        const char *head = "DDS ";
        memcpy(out.data(), head, 4 * sizeof(char));

        // 4 bytes the height of the image
        write_le<uint32_t>(out, _h, 12);

        // 4 bytes the width of the image
        write_le<uint32_t>(out, _w, 16);

        // 4 bytes the linear size of the image
        write_le<uint32_t>(out, _size, 20);

        // 4 bytes the number of mip maps in this image
        write_le<uint32_t>(out, _mips, 28);

        // 4 bytes the fourCC value from the header
        write_le<uint32_t>(out, _format, 84);

        // _size bytes the compressed pixel data
        memcpy(&out[DDS_HEADER_SIZE], _pixel.data(), _size);

        return out;
    }
};
}
#endif
