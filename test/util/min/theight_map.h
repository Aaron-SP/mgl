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
#ifndef __TEST_HEIGHT_MAP__
#define __TEST_HEIGHT_MAP__

#include <min/height_map.h>
#include <min/test.h>
#include <stdexcept>

bool test_height_map()
{
    bool out = true;

    // Create height map, generate a 257x257 image; 2^8 + 1 = 257
    min::height_map<float> map(8, 4.0, 8.0);

    // Test size of height map
    out = out && compare(257, map.size());
    if (!out)
    {
        throw std::runtime_error("Failed height map size");
    }

    // Loop through every pixel in the height map
    bool pass = true;
    for (size_t i = 0; i < 257; i++)
    {
        for (size_t j = 0; j < 257; j++)
        {
            if (map.get(i, j) < 4.0)
            {
                pass = false;
            }
        }
    }

    // Blur the image
    map.gauss_blur_5x5();
    for (size_t i = 0; i < 257; i++)
    {
        for (size_t j = 0; j < 257; j++)
        {
            if (map.get(i, j) < 4.0)
            {
                pass = false;
            }
        }
    }

    // Test height map
    out = out && pass;
    if (!out)
    {
        throw std::runtime_error("Failed height map test");
    }

    // return status
    return out;
}

#endif
