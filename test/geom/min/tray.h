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
#ifndef __TESTRAY__
#define __TESTRAY__

#include <min/ray.h>
#include <min/test.h>
#include <min/vec2.h>
#include <min/vec3.h>
#include <min/vec4.h>
#include <stdexcept>

bool test_ray()
{
    bool out = true;

    // vec2 ray
    {
        // Print size and alignment of class
        std::cout << "ray_vec2_size: " << sizeof(min::ray<float, min::vec2>) << std::endl;
        std::cout << "ray_vec2_align: " << alignof(min::ray<float, min::vec2>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "tray.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 6, sizeof(min::ray<float, min::vec2>), "Failed ray vec2 sizeof");
        out = out && test(sizeof(float), alignof(min::ray<float, min::vec2>), "Failed ray vec2 alignof");
#endif

        // Local variables
        min::ray<double, min::vec2> r;
        min::vec2<double> to;
        min::vec2<double> from;
        min::vec2<double> dir;

        // Test ray constructor
        from = min::vec2<double>(1.0, 1.0);
        to = min::vec2<double>(2.0, 2.0);
        r = min::ray<double, min::vec2>(from, to);
        dir = r.get_direction();
        out = out && compare(0.7071, dir.x(), 1E-4);
        out = out && compare(0.7071, dir.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 ray constructor");
        }

        // Test ray inverse direction
        dir = r.get_inverse();
        out = out && compare(1.4142, dir.x(), 1E-4);
        out = out && compare(1.4142, dir.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 ray inverse direction");
        }
    }

    // vec3 ray
    {
        // Print size and alignment of class
        std::cout << "ray_vec3_size: " << sizeof(min::ray<float, min::vec3>) << std::endl;
        std::cout << "ray_vec3_align: " << alignof(min::ray<float, min::vec3>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "tray.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 9, sizeof(min::ray<float, min::vec3>), "Failed ray vec3 sizeof");
        out = out && test(sizeof(float), alignof(min::ray<float, min::vec3>), "Failed ray vec3 alignof");
#endif

        // Local variables
        min::ray<double, min::vec3> r;
        min::vec3<double> to;
        min::vec3<double> from;
        min::vec3<double> dir;

        // Test ray constructor
        from = min::vec3<double>(1.0, 1.0, 1.0);
        to = min::vec3<double>(2.0, 2.0, 2.0);
        r = min::ray<double, min::vec3>(from, to);
        dir = r.get_direction();
        out = out && compare(0.5773, dir.x(), 1E-4);
        out = out && compare(0.5773, dir.y(), 1E-4);
        out = out && compare(0.5773, dir.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 ray constructor");
        }

        // Test ray inverse direction
        dir = r.get_inverse();
        out = out && compare(1.732, dir.x(), 1E-4);
        out = out && compare(1.732, dir.y(), 1E-4);
        out = out && compare(1.732, dir.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 ray inverse direction");
        }
    }

    // vec4 ray
    {
        // Print size and alignment of class
        std::cout << "ray_vec4_size: " << sizeof(min::ray<float, min::vec4>) << std::endl;
        std::cout << "ray_vec4_align: " << alignof(min::ray<float, min::vec4>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "tray.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 12, sizeof(min::ray<float, min::vec4>), "Failed ray vec4 sizeof");
        out = out && test(sizeof(float), alignof(min::ray<float, min::vec4>), "Failed ray vec4 alignof");
#endif

        // Local variables
        min::ray<double, min::vec4> r;
        min::vec4<double> to;
        min::vec4<double> from;
        min::vec4<double> dir;

        // Test ray constructor
        from = min::vec4<double>(1.0, 1.0, 1.0, 1.0);
        to = min::vec4<double>(2.0, 2.0, 2.0, 1.0);
        r = min::ray<double, min::vec4>(from, to);
        dir = r.get_direction();
        out = out && compare(0.5773, dir.x(), 1E-4);
        out = out && compare(0.5773, dir.y(), 1E-4);
        out = out && compare(0.5773, dir.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 ray constructor");
        }

        // Test ray inverse direction
        dir = r.get_inverse();
        out = out && compare(1.732, dir.x(), 1E-4);
        out = out && compare(1.732, dir.y(), 1E-4);
        out = out && compare(1.732, dir.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 ray inverse direction");
        }
    }
    return out;
}
#endif
