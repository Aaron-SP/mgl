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
#ifndef __DDS__
#define __DDS__

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

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
    uint32_t _w;
    uint32_t _h;
    uint32_t _size;
    uint32_t _bpp;
    uint32_t _mips;
    uint32_t _format;
    std::vector<uint8_t> _pixel;

    uint32_t calculate_size() const
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
            width = std::max((unsigned)1, width / 2);
            height = std::max((unsigned)1, height / 2);
        }

        return total;
    }

    void check_size()
    {
        // Verify dds has correct size
        uint32_t expected = calculate_size();
        if (_size != expected)
        {
            throw std::runtime_error("dds: Expected image size '" + std::to_string(expected) + "' got '" + std::to_string(_size) + "'");
        }
    }
    void load(const std::string &_file)
    {
        std::ifstream file(_file, std::ios::in | std::ios::binary | std::ios::ate);
        if (file.is_open())
        {

            // Get the size of the file and check against header size
            auto size = file.tellg();
            if (size < DDS_HEADER_SIZE)
            {
                throw std::runtime_error("dds: ''" + _file + "' not large enough to be dds file");
            }

            // Read the DDS_HEADER_SIZE byte header
            std::string header(DDS_HEADER_SIZE, 0);
            file.seekg(0, std::ios::beg);
            file.read(&header[0], DDS_HEADER_SIZE);
            std::string head = std::string(header, 0, 4);
            if (head.compare("DDS ") != 0)
            {
                throw std::runtime_error("dds: Invalid dds header, expected DDS got '" + head + "'");
            }

            // 4 bytes the height of the image
            memcpy(&_h, &header[12], sizeof(uint32_t));

            // 4 bytes the width of the image
            memcpy(&_w, &header[16], sizeof(uint32_t));

            // 4 bytes the linear size of the image
            memcpy(&_size, &header[20], sizeof(uint32_t));

            // 4 bytes the number of mip maps in this image
            memcpy(&_mips, &header[28], sizeof(uint32_t));

            // 4 bytes the fourCC value from the header
            memcpy(&_format, &header[84], sizeof(uint32_t));

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
                throw std::runtime_error("dds: ''" + _file + "' image size is corrupted, possibly missing data");
            }

            // Read the pixel data
            _pixel.resize(_size);
            file.read((char *)&_pixel[0], _size);

            // Close the file
            file.close();
        }
        else
        {
            throw std::runtime_error("dds: Could not load file '" + _file + "'");
        }
    }

  public:
    dds(const std::string &file)
    {
        load(file);
    }
    dds(const uint32_t w, const uint32_t h, const uint32_t mips, const uint32_t format, const std::vector<uint8_t> &pixel)
        : _w(w), _h(h), _mips(mips), _format(format), _pixel(pixel)
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
    uint32_t get_format() const
    {
        return _format;
    }
    uint32_t get_mips() const
    {
        return _mips;
    }
    uint32_t get_width() const
    {
        return _w;
    }
    uint32_t get_height() const
    {
        return _h;
    }
    uint32_t get_size() const
    {
        return _size;
    }
    const std::vector<uint8_t> &get_pixels() const
    {
        return _pixel;
    }
    std::vector<uint8_t> to_file() const
    {
        // Write out the dds file to a byte buffer for writing to file
        size_t size = DDS_HEADER_SIZE + _size;
        std::vector<uint8_t> out(size, 0);

        // The DDS header
        const char *head = "DDS ";
        memcpy(&out[0], head, 4 * sizeof(char));

        // 4 bytes the height of the image
        memcpy(&out[12], &_h, sizeof(uint32_t));

        // 4 bytes the width of the image
        memcpy(&out[16], &_w, sizeof(uint32_t));

        // 4 bytes the linear size of the image
        memcpy(&out[20], &_size, sizeof(uint32_t));

        // 4 bytes the number of mip maps in this image
        memcpy(&out[28], &_mips, sizeof(uint32_t));

        // 4 bytes the fourCC value from the header
        memcpy(&out[84], &_format, sizeof(uint32_t));

        // _size bytes the compressed pixel data
        memcpy(&out[DDS_HEADER_SIZE], &_pixel[0], _size);

        return out;
    }
};
}
#endif
