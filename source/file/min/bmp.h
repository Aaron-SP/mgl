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
#ifndef _MGL_BMP_MGL_
#define _MGL_BMP_MGL_

#include <cstring>
#include <fstream>
#include <min/mem_chunk.h>
#include <min/static_vector.h>
#include <string>

namespace min
{

class bmp
{
  private:
    min::static_vector<uint8_t> _pixel;
    uint32_t _w;
    uint32_t _h;
    uint32_t _size;
    uint32_t _bpp;

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

            // Process the BMP file
            load<min::static_vector<uint8_t>>(data);
        }
        else
        {
            throw std::runtime_error("bmp: Could not load file '" + _file + "'");
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

        // File size in bytes
        const size_t size = data.size();
        if (size < 18)
        {
            throw std::runtime_error("bmp: File not large enough to be a BMP file");
        }

        // Two bytes indicating the BMP version field
        if (data[0] != 'B' && data[1] != 'M')
        {
            throw std::runtime_error("bmp: Invalid bmp header");
        }

        // 4 bytes indicating the size of the BMP file in bytes
        size_t next = 2;
        _size = read_le<uint32_t>(data, next);

        // 4 bytes indicating the starting offset where the bitmap image pixel data can be found
        next = 10;
        const uint32_t data_start = read_le<uint32_t>(data, next);

        // 4 bytes the DIB header size, should be 40
        // 108 and 124 are allowed since they extend the BITMAPINFOHEADER capabilities
        // BITMAPV4HEADER == 108
        // BITMAPV5HEADER == 124
        next = 14;
        const uint32_t dib = read_le<uint32_t>(data, next);
        if (dib != 40 && dib != 108 && dib != 124)
        {
            throw std::runtime_error("bmp: expected dib size of 40, 108, or 124 got '" + std::to_string(dib) + "'");
        }

        // 4 bytes the width of the image in pixels
        next = 18;
        _w = read_le<uint32_t>(data, next);

        // 4 bytes the height of the image in pixels
        next = 22;
        _h = read_le<uint32_t>(data, next);

        // Get the number of image planes
        next = 26;
        const uint16_t plane = read_le<uint16_t>(data, next);
        if (plane != 1)
        {
            throw std::runtime_error("bmp: expected 1 color plane got '" + std::to_string(plane) + "'");
        }

        // Get the number of bits per pixel
        next = 28;
        _bpp = read_le<uint16_t>(data, next);
        if (_bpp != 24 && _bpp != 32)
        {
            throw std::runtime_error("bmp: expected bits per pixel of 24/32 '" + std::to_string(_bpp) + "'");
        }

        // Convert to bytes
        _bpp /= 8;

        // Get the image compression
        next = 30;
        const uint32_t compress = read_le<uint32_t>(data, next);

        // Verify 32 bit RGBA format and uses the extended dib header
        if (_bpp == 4 && compress == 3 && dib >= 108)
        {
            next = 54;
            const uint32_t red_mask = read_le<uint32_t>(data, next);
            const uint32_t green_mask = read_le<uint32_t>(data, next);
            const uint32_t blue_mask = read_le<uint32_t>(data, next);
            const uint32_t alpha_mask = read_le<uint32_t>(data, next);

            // Check the bit mask values
            if (alpha_mask != 0xFF)
            {
                throw std::runtime_error("bmp: unsupported alpha channel mask expected 0xFF got " + std::to_string(alpha_mask));
            }
            else if (blue_mask != 0xFF00)
            {
                throw std::runtime_error("bmp: unsupported blue channel mask expected 0xFF00 got " + std::to_string(blue_mask));
            }
            else if (green_mask != 0xFF0000)
            {
                throw std::runtime_error("bmp: unsupported green channel mask expected 0xFF0000 got " + std::to_string(green_mask));
            }
            else if (red_mask != 0xFF000000)
            {
                throw std::runtime_error("bmp: unsupported red channel mask expected 0xFF000000 got " + std::to_string(red_mask));
            }
        }
        else if (_bpp == 4)
        {
            throw std::runtime_error("bmp: expected 32 bit pixel data, with BI_BITFIELD compression, and with DIB header of BITMAPV4HEADER or greater, unsupported format");
        }
        else if (compress != 0)
        {
            throw std::runtime_error("bmp: expected BI_RGB image compession, got unsupported format of " + std::to_string(compress));
        }

        // Size of the raw bitmap data (including padding)
        next = 34;
        _size = read_le<uint32_t>(data, next);
        const uint32_t expected = _bpp * _w * _h;
        if (_size < expected)
        {
            throw std::runtime_error("bmp: expected image size of atleast, ''" + std::to_string(expected) + "' got '" + std::to_string(_size) + "'");
        }

        // Check that we have data
        if (_size == 0)
        {
            throw std::runtime_error("bmp: image has zero pixel data");
        }

        // Check the file size
        if (size < data_start + _size)
        {
            throw std::runtime_error("bmp: Image size is corrupted, possibly missing data");
        }

        // Read the image data starting at data offset
        _pixel.resize(_size);
        std::memcpy(_pixel.data(), &data[data_start], _size);
    }

  public:
    bmp(const std::string &file)
    {
        load(file);
    }
    bmp(const mem_file &mem)
    {
        load<mem_file>(mem);
    }
    bmp(const uint32_t w, const uint32_t h, const uint32_t bpp)
        : _w(w), _h(h), _size(w * h * bpp), _bpp(bpp)
    {
        _pixel.resize(_size);
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
    inline uint32_t get_pixel_size() const
    {
        return _bpp;
    }
    inline void set(uint32_t i, const uint8_t r, const uint8_t g, const uint8_t b)
    {
        // Convert to byte address
        i *= 3;

        // Set color channels
        _pixel[i++] = b;
        _pixel[i++] = g;
        _pixel[i++] = r;
    }
    inline void set(uint32_t i, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
    {
        // Convert to byte address
        i *= 4;

        // Set color channels
        _pixel[i++] = r;
        _pixel[i++] = g;
        _pixel[i++] = b;
        _pixel[i++] = a;
    }
    inline min::static_vector<uint8_t> to_file() const
    {
        // BITMAPV4HEADER type
        const uint32_t BMP_HEADER_SIZE = 14;
        const uint32_t BMP_DIB_SIZE = (_bpp == 4) ? 108 : 40;
        const uint32_t BMP_OFFSET = BMP_HEADER_SIZE + BMP_DIB_SIZE;

        // Create file memory buffer
        const uint32_t file_size = BMP_OFFSET + _size;
        min::static_vector<uint8_t> out(file_size);

        // Zero out the rest of the header
        std::memset(out.data(), 0u, BMP_OFFSET * sizeof(char));

        // The BMP header
        const char *head = "BM";
        std::memcpy(out.data(), head, 2 * sizeof(char));

        // 4 bytes the size of the image file
        write_le<uint32_t>(out, file_size, 2);

        // 4 bytes data offset
        write_le<uint32_t>(out, BMP_OFFSET, 10);

        // 4 size of BMP DIB header in bytes
        write_le<uint32_t>(out, BMP_DIB_SIZE, 14);

        // 4 size of BMP width
        write_le<uint32_t>(out, _w, 18);

        // 4 size of BMP height
        write_le<uint32_t>(out, _h, 22);

        // 2 number of color planes (must be 1)
        write_le<uint16_t>(out, 1u, 26);

        // 2 number of bits per pixel
        write_le<uint16_t>(out, _bpp * 8u, 28);

        // 4 the compression method being used
        if (_bpp == 4)
        {
            write_le<uint32_t>(out, 3, 30);
        }

        // 4 Image pixel size
        write_le<uint32_t>(out, _size, 34);

        // 4 print horizontal resolution
        write_le<uint32_t>(out, 2835u, 38);

        // 4 print horizontal resolution
        write_le<uint32_t>(out, 2835u, 42);

        // For extended DIB
        if (_bpp == 4)
        {
            // 4 Red mask
            write_le<uint32_t>(out, 0xFF000000, 54);

            // 4 Green mask
            write_le<uint32_t>(out, 0xFF0000, 58);

            // 4 Blue mask
            write_le<uint32_t>(out, 0xFF00, 62);

            // 4 Alpha mask
            write_le<uint32_t>(out, 0xFF, 66);

            // 4 little endian, color space LCS_WINDOWS_COLOR_SPACE
            write_le<char>(out, 0x20, 70);
            write_le<char>(out, 0x6E, 71);
            write_le<char>(out, 0x69, 72);
            write_le<char>(out, 0x57, 73);
        }

        // Write the pixel data
        std::memcpy(&out[BMP_OFFSET], _pixel.data(), _size);

        // Return
        return out;
    }
};
}
#endif
