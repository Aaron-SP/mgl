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
#ifndef _MGL_TESTBMP_MGL_
#define _MGL_TESTBMP_MGL_

#include <min/bmp.h>
#include <min/test.h>
#include <stdexcept>

bool test_bmp()
{
    bool out = true;

    // Print size and alignment of class
    std::cout << "bmp_size: " << sizeof(min::bmp) << std::endl;
    std::cout << "bmp_align: " << alignof(min::bmp) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "tbmp.h: Testing alignment" << std::endl;
    out = out && test(sizeof(void *) * 2 + 16, sizeof(min::bmp), "Failed bmp sizeof");
    out = out && test(sizeof(void *), alignof(min::bmp), "Failed bmp alignof");
#endif

    // Windows paint BMP 24 bit
    {
        // Local variables
        const min::bmp image = min::bmp("data/texture/art_cube.bmp");
        const char *msg = "Failed bmp win 24 bit image constructor properties";
        out = out && test(256, image.get_width(), msg);
        out = out && test(256, image.get_height(), msg);
        out = out && test(196608, image.get_size(), msg);

        // Check pixel count
        out = out && test(196608, image.get_pixels().size(), "Failed bmp win 24 bit image size");
    }

    // GIMP BMP 24 bit
    {
        // Local variables
        const min::bmp image = min::bmp("data/texture/gimp.bmp");
        const char *msg = "Failed bmp gimp 24 bit image constructor properties";
        out = out && test(256, image.get_width(), msg);
        out = out && test(256, image.get_height(), msg);
        out = out && test(196608, image.get_size(), msg);

        // Check pixel count
        out = out && test(196608, image.get_pixels().size(), "Failed bmp gimp 24 bit image size");
    }

    // GIMP BMP 32 bit
    {
        // Local variables
        const min::bmp image = min::bmp("data/texture/stone.bmp");
        const char *msg = "Failed bmp 32 bit image constructor properties";
        out = out && test(256, image.get_width(), msg);
        out = out && test(256, image.get_height(), msg);
        out = out && test(262144, image.get_size(), msg);

        // Check pixel count
        out = out && test(262144, image.get_pixels().size(), "Failed bmp 32 bit image size");
    }
    return out;
}

#endif
