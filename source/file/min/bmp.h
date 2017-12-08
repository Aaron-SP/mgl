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
#ifndef __BMP__
#define __BMP__

#include <cstring>
#include <fstream>
#include <string>
#include <vector>

namespace min
{

class bmp
{
  private:
    uint32_t _w;
    uint32_t _h;
    uint32_t _size;
    uint16_t _bpp;
    uint32_t _data;
    uint32_t _dib;
    std::vector<uint8_t> _pixel;

    void load(const std::string _file)
    {
        std::ifstream file(_file, std::ios::in | std::ios::binary | std::ios::ate);
        if (file.is_open())
        {
            // Get the size of the file
            const auto size = file.tellg();
            if (size < 18)
            {
                throw std::runtime_error("bmp: ''" + _file + "' not large enough to be BMP file");
            }

            std::string header(18, 0);
            file.seekg(0, std::ios::beg);
            file.read(&header[0], 18);

            // Two bytes indicating the BMP version field
            if (header[0] != 'B' && header[1] != 'M')
            {
                throw std::runtime_error("bmp: Invalid bmp header, expected BM got '" + std::string(header, 0, 2) + "'");
            }

            // 4 bytes indicating the size of the BMP file in bytes
            std::memcpy(&_size, &header[2], sizeof(uint32_t));

            // 4 bytes indicating the starting offset where the bitmap image pixel data can be found
            std::memcpy(&_data, &header[10], sizeof(uint32_t));

            // 4 bytes the DIB header size, should be 40
            // 108 and 124 are allowed since they extend the BITMAPINFOHEADER capabilities
            // BITMAPV4HEADER == 108
            // BITMAPV5HEADER == 124
            std::memcpy(&_dib, &header[14], sizeof(uint32_t));
            if (_dib != 40 && _dib != 108 && _dib != 124)
            {
                throw std::runtime_error("bmp: expected dib size of 40, 108, or 124 got '" + std::to_string(_dib) + "'");
            }

            // Read more data based on the dib header, update 'size' for later use
            header.resize(18 + _dib);
            file.read(&header[18], _dib);

            // 4 bytes the width of the image in pixels
            std::memcpy(&_w, &header[18], sizeof(uint32_t));

            // 4 bytes the height of the image in pixels
            std::memcpy(&_h, &header[22], sizeof(uint32_t));

            // Get the number of image planes
            uint16_t plane;
            std::memcpy(&plane, &header[26], sizeof(uint16_t));
            if (plane != 1)
            {
                throw std::runtime_error("bmp: expected 1 color plane got '" + std::to_string(plane) + "'");
            }

            // Get the number of bits per pixel
            std::memcpy(&_bpp, &header[28], sizeof(uint16_t));
            if (_bpp != 24 && _bpp != 32)
            {
                throw std::runtime_error("bmp: expected bits per pixel of 24/32 '" + std::to_string(_bpp) + "'");
            }

            // Convert to bytes
            _bpp /= 8;

            // Get the image compression
            uint32_t compress;
            std::memcpy(&compress, &header[30], sizeof(uint32_t));

            // Verify 32 bit RGBA format and uses the extended dib header
            if (_bpp == 4 && compress == 3 && _dib >= 108)
            {
                uint32_t red_mask, green_mask, blue_mask, alpha_mask;
                std::memcpy(&red_mask, &header[54], sizeof(uint32_t));
                std::memcpy(&green_mask, &header[58], sizeof(uint32_t));
                std::memcpy(&blue_mask, &header[62], sizeof(uint32_t));
                std::memcpy(&alpha_mask, &header[66], sizeof(uint32_t));

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
            std::memcpy(&_size, &header[34], sizeof(uint32_t));
            uint32_t expected = _bpp * _w * _h;
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
            if (size < _data + _size)
            {
                throw std::runtime_error("bmp: ''" + _file + "' image size is corrupted, possibly missing data");
            }

            // Read the image data
            file.seekg(_data, std::ios::beg);
            _pixel.resize(_size);
            file.read((char *)&_pixel[0], _size);

            // Close the file
            file.close();
        }
        else
        {
            throw std::runtime_error("bmp: Could not load file '" + _file + "'");
        }
    }

  public:
    bmp(const std::string &file)
    {
        load(file);
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
    uint32_t get_pixel_size() const
    {
        return _bpp;
    }
};
}
#endif
