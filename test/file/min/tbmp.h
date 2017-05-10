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
#ifndef __TESTBMP__
#define __TESTBMP__

#include <min/bmp.h>
#include <min/test.h>
#include <stdexcept>

bool test_bmp()
{
    bool out = true;

    // Windows paint BMP
    {
        // Local variables
        int s;
        int w;
        int h;
        std::vector<uint8_t> data;
        const min::bmp image = min::bmp("data/texture/paint.bmp");
        s = image.get_size();
        w = image.get_width();
        h = image.get_height();
        out = out && compare(256, w);
        out = out && compare(256, h);
        out = out && compare(196608, s);
        if (!out)
        {
            throw std::runtime_error("Failed bmp image constructor properties");
        }

        data = image.get_pixels();
        out = out && compare(196608, data.size());
        if (!out)
        {
            throw std::runtime_error("Failed bmp image size");
        }
    }

    // GIMP BMP
    {
        // Local variables
        int s;
        int w;
        int h;
        std::vector<uint8_t> data;
        const min::bmp image = min::bmp("data/texture/gimp.bmp");
        s = image.get_size();
        w = image.get_width();
        h = image.get_height();
        out = out && compare(256, w);
        out = out && compare(256, h);
        out = out && compare(196608, s);
        if (!out)
        {
            throw std::runtime_error("Failed bmp image constructor properties");
        }

        data = image.get_pixels();
        out = out && compare(196608, data.size());
        if (!out)
        {
            throw std::runtime_error("Failed bmp image size");
        }
    }
    return out;
}

#endif