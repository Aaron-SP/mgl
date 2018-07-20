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
#ifndef _MGL_TESTDDS_MGL_
#define _MGL_TESTDDS_MGL_

#include <min/dds.h>
#include <min/test.h>
#include <stdexcept>

bool test_dds()
{
    bool out = true;

    // Print size and alignment of class
    std::cout << "dds_size: " << sizeof(min::dds) << std::endl;
    std::cout << "dds_align: " << alignof(min::dds) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "tdds.h: Testing alignment" << std::endl;
    out = out && test(sizeof(void *) * 2 + 24, sizeof(min::dds), "Failed dds sizeof");
    out = out && test(sizeof(void *), alignof(min::dds), "Failed dds alignof");
#endif

    // Local variables
    const min::dds image = min::dds("data/texture/stone.dds");
    const char *msg = "Failed dds image constructor properties";
    out = out && test(256, image.get_width(), msg);
    out = out && test(256, image.get_height(), msg);
    out = out && test(43704, image.get_size(), msg);
    out = out && test(43704, image.get_pixels().size(), "Failed dds image size");

    return out;
}

#endif
