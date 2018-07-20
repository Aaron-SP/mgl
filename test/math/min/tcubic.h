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
#ifndef _MGL_TESTCUBIC_MGL_
#define _MGL_TESTCUBIC_MGL_

#include <min/cubic.h>
#include <min/test.h>
#include <min/vec2.h>
#include <min/vec3.h>
#include <min/vec4.h>
#include <stdexcept>

bool test_bezier_cubic()
{
    bool out = true;

    // vec2 cubic
    {
        // Print size and alignment of class
        std::cout << "bezier_vec2_size: " << sizeof(min::bezier<float, min::vec2>) << std::endl;
        std::cout << "bezier_vec2_align: " << alignof(min::bezier<float, min::vec2>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "tcubic.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 8, sizeof(min::bezier<float, min::vec2>), "Failed bezier_vec2 sizeof");
        out = out && test(sizeof(float), alignof(min::bezier<float, min::vec2>), "Failed bezier_vec2 alignof");
#endif

        const min::vec2<float> p1(0.0, 0.0);
        const min::vec2<float> p2(0.5, 0.25);
        const min::vec2<float> p3(1.0, 0.5);
        const min::vec2<float> p4(1.0, 1.0);
        min::vec3<float> p;

        // Local variables
        min::bezier<float, min::vec2> b(p1, p2, p3, p4);

        // Test bezier interpolation at t = 0.0
        p = b.interpolate(0.0);
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec2 interpolation t=0.0");
        }

        // Test bezier interpolation at t = 0.0625
        p = b.interpolate(0.0625);
        out = out && compare(0.0936, p.x(), 1E-4);
        out = out && compare(0.0469, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec2 interpolation t=0.0625");
        }

        // Test bezier interpolation at t = 0.125
        p = b.interpolate(0.125);
        out = out && compare(0.1865, p.x(), 1E-4);
        out = out && compare(0.0942, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec2 interpolation t=0.125");
        }

        // Test bezier interpolation at t = 0.25
        p = b.interpolate(0.25);
        out = out && compare(0.3671, p.x(), 1E-4);
        out = out && compare(0.1914, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec2 interpolation t=0.25");
        }

        // Test bezier interpolation at t = 0.50
        p = b.interpolate(0.50);
        out = out && compare(0.6875, p.x(), 1E-4);
        out = out && compare(0.4062, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec2 interpolation t=0.5");
        }

        // Test bezier interpolation at t = 0.75
        p = b.interpolate(0.75);
        out = out && compare(0.9140, p.x(), 1E-4);
        out = out && compare(0.6679, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec2 interpolation t=0.75");
        }

        // Test bezier interpolation at t = 1.00
        p = b.interpolate(1.0);
        out = out && compare(1.0, p.x(), 1E-4);
        out = out && compare(1.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec2 interpolation t=1.0");
        }
    }

    // vec3 cubic
    {
        // Print size and alignment of class
        std::cout << "bezier_vec3_size: " << sizeof(min::bezier<float, min::vec3>) << std::endl;
        std::cout << "bezier_vec3_align: " << alignof(min::bezier<float, min::vec3>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "tcubic.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 12, sizeof(min::bezier<float, min::vec3>), "Failed bezier_vec3 sizeof");
        out = out && test(sizeof(float), alignof(min::bezier<float, min::vec3>), "Failed bezier_vec3 alignof");
#endif

        const min::vec3<float> p1(0.0, 0.0, 0.0);
        const min::vec3<float> p2(1.0, 0.0, 0.0);
        const min::vec3<float> p3(1.0, 1.0, 0.0);
        const min::vec3<float> p4(1.0, 1.0, 1.0);
        min::vec3<float> p;

        // Local variables
        min::bezier<float, min::vec3> b(p1, p2, p3, p4);

        // Test bezier interpolation at t = 0.0
        p = b.interpolate(0.0);
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        out = out && compare(0.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec3 interpolation t=0.0");
        }

        // Test bezier interpolation at t = 0.0625
        p = b.interpolate(0.0625);
        out = out && compare(0.1760, p.x(), 1E-4);
        out = out && compare(0.0112, p.y(), 1E-4);
        out = out && compare(0.0002, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec3 interpolation t=0.0625");
        }

        // Test bezier interpolation at t = 0.125
        p = b.interpolate(0.125);
        out = out && compare(0.3300, p.x(), 1E-4);
        out = out && compare(0.0429, p.y(), 1E-4);
        out = out && compare(0.0019, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec3 interpolation t=0.125");
        }

        // Test bezier interpolation at t = 0.25
        p = b.interpolate(0.25);
        out = out && compare(0.5781, p.x(), 1E-4);
        out = out && compare(0.1562, p.y(), 1E-4);
        out = out && compare(0.0156, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec3 interpolation t=0.25");
        }

        // Test bezier interpolation at t = 0.50
        p = b.interpolate(0.50);
        out = out && compare(0.8750, p.x(), 1E-4);
        out = out && compare(0.5000, p.y(), 1E-4);
        out = out && compare(0.1250, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec3 interpolation t=0.5");
        }

        // Test bezier interpolation at t = 0.75
        p = b.interpolate(0.75);
        out = out && compare(0.9843, p.x(), 1E-4);
        out = out && compare(0.8437, p.y(), 1E-4);
        out = out && compare(0.4218, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec3 interpolation t=0.75");
        }

        // Test bezier interpolation at t = 1.00
        p = b.interpolate(1.0);
        out = out && compare(1.0, p.x(), 1E-4);
        out = out && compare(1.0, p.y(), 1E-4);
        out = out && compare(1.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec3 interpolation t=1.0");
        }
    }

    // vec4 cubic
    {
        // Print size and alignment of class
        std::cout << "bezier_vec4_size: " << sizeof(min::bezier<float, min::vec4>) << std::endl;
        std::cout << "bezier_vec4_align: " << alignof(min::bezier<float, min::vec4>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "tcubic.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 16, sizeof(min::bezier<float, min::vec4>), "Failed bezier_vec4 sizeof");
        out = out && test(sizeof(float), alignof(min::bezier<float, min::vec4>), "Failed bezier_vec4 alignof");
#endif

        const min::vec4<float> p1(0.0, 0.0, 0.0, 1.0);
        const min::vec4<float> p2(1.0, 0.0, 0.0, 1.0);
        const min::vec4<float> p3(1.0, 1.0, 0.0, 1.0);
        const min::vec4<float> p4(1.0, 1.0, 1.0, 1.0);
        min::vec4<float> p;

        // Local variables
        min::bezier<float, min::vec4> b(p1, p2, p3, p4);

        // Test bezier interpolation at t = 0.0
        p = b.interpolate(0.0);
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        out = out && compare(0.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec4 interpolation t=0.0");
        }

        // Test bezier interpolation at t = 0.0625
        p = b.interpolate(0.0625);
        out = out && compare(0.1760, p.x(), 1E-4);
        out = out && compare(0.0112, p.y(), 1E-4);
        out = out && compare(0.0002, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec4 interpolation t=0.0625");
        }

        // Test bezier interpolation at t = 0.125
        p = b.interpolate(0.125);
        out = out && compare(0.3300, p.x(), 1E-4);
        out = out && compare(0.0429, p.y(), 1E-4);
        out = out && compare(0.0019, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec4 interpolation t=0.125");
        }

        // Test bezier interpolation at t = 0.25
        p = b.interpolate(0.25);
        out = out && compare(0.5781, p.x(), 1E-4);
        out = out && compare(0.1562, p.y(), 1E-4);
        out = out && compare(0.0156, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec4 interpolation t=0.25");
        }

        // Test bezier interpolation at t = 0.50
        p = b.interpolate(0.50);
        out = out && compare(0.8750, p.x(), 1E-4);
        out = out && compare(0.5000, p.y(), 1E-4);
        out = out && compare(0.1250, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec4 interpolation t=0.5");
        }

        // Test bezier interpolation at t = 0.75
        p = b.interpolate(0.75);
        out = out && compare(0.9843, p.x(), 1E-4);
        out = out && compare(0.8437, p.y(), 1E-4);
        out = out && compare(0.4218, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec4 interpolation t=0.75");
        }

        // Test bezier interpolation at t = 1.00
        p = b.interpolate(1.0);
        out = out && compare(1.0, p.x(), 1E-4);
        out = out && compare(1.0, p.y(), 1E-4);
        out = out && compare(1.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bezier vec4 interpolation t=1.0");
        }
    }

    return out;
}

bool test_bspline_cubic()
{
    bool out = true;

    // vec2 cubic
    {
        // Print size and alignment of class
        std::cout << "bspline_vec2_size: " << sizeof(min::bspline<float, min::vec2>) << std::endl;
        std::cout << "bspline_vec2_align: " << alignof(min::bspline<float, min::vec2>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "tcubic.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 8, sizeof(min::bspline<float, min::vec2>), "Failed bspline_vec2 sizeof");
        out = out && test(sizeof(float), alignof(min::bspline<float, min::vec2>), "Failed bspline_vec2 alignof");
#endif

        // p1 and p4 are control points not curve end points!
        const min::vec2<float> p1(0.0, 0.0);
        const min::vec2<float> p2(0.5, 0.25);
        const min::vec2<float> p3(1.0, 0.5);
        const min::vec2<float> p4(1.0, 1.0);
        min::vec3<float> p;

        // Local variables
        min::bspline<float, min::vec2> b(p1, p2, p3, p4);

        // Test bspline interpolation at t = 0.0
        p = b.interpolate(0.0);
        out = out && compare(0.5000, p.x(), 1E-4);
        out = out && compare(0.2500, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec2 interpolation t=0.0");
        }

        // Test bspline interpolation at t = 0.0625
        p = b.interpolate(0.0625);
        out = out && compare(0.5312, p.x(), 1E-4);
        out = out && compare(0.2656, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec2 interpolation t=0.0625");
        }

        // Test bspline interpolation at t = 0.125
        p = b.interpolate(0.125);
        out = out && compare(0.5623, p.x(), 1E-4);
        out = out && compare(0.2813, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec2 interpolation t=0.125");
        }

        // Test bspline interpolation at t = 0.25
        p = b.interpolate(0.25);
        out = out && compare(0.6236, p.x(), 1E-4);
        out = out && compare(0.3131, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec2 interpolation t=0.25");
        }

        // Test bspline interpolation at t = 0.50
        p = b.interpolate(0.50);
        out = out && compare(0.7395, p.x(), 1E-4);
        out = out && compare(0.3802, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec2 interpolation t=0.5");
        }

        // Test bspline interpolation at t = 0.75
        p = b.interpolate(0.75);
        out = out && compare(0.8398, p.x(), 1E-4);
        out = out && compare(0.4550, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec2 interpolation t=0.75");
        }

        // Test bspline interpolation at t = 1.00
        p = b.interpolate(1.0);
        out = out && compare(0.9166, p.x(), 1E-4);
        out = out && compare(0.5416, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec2 interpolation t=1.0");
        }
    }

    // vec3 cubic
    {
        // Print size and alignment of class
        std::cout << "bspline_vec3_size: " << sizeof(min::bspline<float, min::vec3>) << std::endl;
        std::cout << "bspline_vec3_align: " << alignof(min::bspline<float, min::vec3>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "tcubic.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 12, sizeof(min::bspline<float, min::vec3>), "Failed bspline_vec3 sizeof");
        out = out && test(sizeof(float), alignof(min::bspline<float, min::vec3>), "Failed bspline_vec3 alignof");
#endif

        // p1 and p4 are control points not curve end points!
        const min::vec3<float> p1(0.0, 0.0, 0.0);
        const min::vec3<float> p2(1.0, 0.0, 0.0);
        const min::vec3<float> p3(1.0, 1.0, 0.0);
        const min::vec3<float> p4(1.0, 1.0, 1.0);
        min::vec3<float> p;

        // Local variables
        min::bspline<float, min::vec3> b(p1, p2, p3, p4);

        // Test bspline interpolation at t = 0.0
        p = b.interpolate(0.0);
        out = out && compare(0.8333, p.x(), 1E-4);
        out = out && compare(0.1666, p.y(), 1E-4);
        out = out && compare(0.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec3 interpolation t=0.0");
        }

        // Test bspline interpolation at t = 0.0625
        p = b.interpolate(0.0625);
        out = out && compare(0.8626, p.x(), 1E-4);
        out = out && compare(0.1997, p.y(), 1E-4);
        out = out && compare(0.0000, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec3 interpolation t=0.0625");
        }

        // Test bspline interpolation at t = 0.125
        p = b.interpolate(0.125);
        out = out && compare(0.8883, p.x(), 1E-4);
        out = out && compare(0.2363, p.y(), 1E-4);
        out = out && compare(0.0003, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec3 interpolation t=0.125");
        }

        // Test bspline interpolation at t = 0.25
        p = b.interpolate(0.25);
        out = out && compare(0.9296, p.x(), 1E-4);
        out = out && compare(0.3177, p.y(), 1E-4);
        out = out && compare(0.0026, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec3 interpolation t=0.25");
        }

        // Test bspline interpolation at t = 0.50
        p = b.interpolate(0.50);
        out = out && compare(0.9791, p.x(), 1E-4);
        out = out && compare(0.5000, p.y(), 1E-4);
        out = out && compare(0.0208, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec3 interpolation t=0.5");
        }

        // Test bspline interpolation at t = 0.75
        p = b.interpolate(0.75);
        out = out && compare(0.9973, p.x(), 1E-4);
        out = out && compare(0.6822, p.y(), 1E-4);
        out = out && compare(0.0703, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec3 interpolation t=0.75");
        }

        // Test bspline interpolation at t = 1.00
        p = b.interpolate(1.0);
        out = out && compare(1.0, p.x(), 1E-4);
        out = out && compare(0.8333, p.y(), 1E-4);
        out = out && compare(0.1666, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec3 interpolation t=1.0");
        }
    }

    // vec4 cubic
    {
        // Print size and alignment of class
        std::cout << "bspline_vec4_size: " << sizeof(min::bspline<float, min::vec4>) << std::endl;
        std::cout << "bspline_vec4_align: " << alignof(min::bspline<float, min::vec4>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "tcubic.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 16, sizeof(min::bspline<float, min::vec4>), "Failed bspline_vec4 sizeof");
        out = out && test(sizeof(float), alignof(min::bspline<float, min::vec4>), "Failed bspline_vec4 alignof");
#endif

        // p1 and p4 are control points not curve end points!
        const min::vec4<float> p1(0.0, 0.0, 0.0, 1.0);
        const min::vec4<float> p2(1.0, 0.0, 0.0, 1.0);
        const min::vec4<float> p3(1.0, 1.0, 0.0, 1.0);
        const min::vec4<float> p4(1.0, 1.0, 1.0, 1.0);
        min::vec4<float> p;

        // Local variables
        min::bspline<float, min::vec4> b(p1, p2, p3, p4);

        // Test bspline interpolation at t = 0.0
        p = b.interpolate(0.0);
        out = out && compare(0.8333, p.x(), 1E-4);
        out = out && compare(0.1666, p.y(), 1E-4);
        out = out && compare(0.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec4 interpolation t=0.0");
        }

        // Test bspline interpolation at t = 0.0625
        p = b.interpolate(0.0625);
        out = out && compare(0.8626, p.x(), 1E-4);
        out = out && compare(0.1997, p.y(), 1E-4);
        out = out && compare(0.0000, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec4 interpolation t=0.0625");
        }

        // Test bspline interpolation at t = 0.125
        p = b.interpolate(0.125);
        out = out && compare(0.8883, p.x(), 1E-4);
        out = out && compare(0.2363, p.y(), 1E-4);
        out = out && compare(0.0003, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec4 interpolation t=0.125");
        }

        // Test bspline interpolation at t = 0.25
        p = b.interpolate(0.25);
        out = out && compare(0.9296, p.x(), 1E-4);
        out = out && compare(0.3177, p.y(), 1E-4);
        out = out && compare(0.0026, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec4 interpolation t=0.25");
        }

        // Test bspline interpolation at t = 0.50
        p = b.interpolate(0.50);
        out = out && compare(0.9791, p.x(), 1E-4);
        out = out && compare(0.5000, p.y(), 1E-4);
        out = out && compare(0.0208, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec4 interpolation t=0.5");
        }

        // Test bspline interpolation at t = 0.75
        p = b.interpolate(0.75);
        out = out && compare(0.9973, p.x(), 1E-4);
        out = out && compare(0.6822, p.y(), 1E-4);
        out = out && compare(0.0703, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec4 interpolation t=0.75");
        }

        // Test bspline interpolation at t = 1.00
        p = b.interpolate(1.0);
        out = out && compare(1.0, p.x(), 1E-4);
        out = out && compare(0.8333, p.y(), 1E-4);
        out = out && compare(0.1666, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed bspline vec4 interpolation t=1.0");
        }
    }

    return out;
}

bool test_hermite_cubic()
{
    bool out = true;

    // vec2 cubic
    {
        // Print size and alignment of class
        std::cout << "hermite_vec2_size: " << sizeof(min::hermite<float, min::vec2>) << std::endl;
        std::cout << "hermite_vec2_align: " << alignof(min::hermite<float, min::vec2>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "tcubic.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 8, sizeof(min::hermite<float, min::vec2>), "Failed hermite_vec2 sizeof");
        out = out && test(sizeof(float), alignof(min::hermite<float, min::vec2>), "Failed hermite_vec2 alignof");
#endif

        const min::vec2<float> p1(0.0, 0.0);
        const min::vec2<float> p2(1.0, 1.0);
        const min::vec2<float> t1(0.0, 1.0);
        const min::vec2<float> t2(1.0, 0.0);
        min::vec3<float> p;

        // Local variables
        min::hermite<float, min::vec2> b(p1, p2, t1, t2);

        // Test hermite interpolation at t = 0.0
        p = b.interpolate(0.0);
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec2 interpolation t=0.0");
        }

        // Test hermite interpolation at t = 0.0625
        p = b.interpolate(0.0625);
        out = out && compare(0.0148, p.x(), 1E-4);
        out = out && compare(0.0661, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec2 interpolation t=0.0625");
        }

        // Test hermite interpolation at t = 0.125
        p = b.interpolate(0.125);
        out = out && compare(0.0566, p.x(), 1E-4);
        out = out && compare(0.1386, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec2 interpolation t=0.125");
        }

        // Test hermite interpolation at t = 0.25
        p = b.interpolate(0.25);
        out = out && compare(0.2031, p.x(), 1E-4);
        out = out && compare(0.2968, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec2 interpolation t=0.25");
        }

        // Test hermite interpolation at t = 0.50
        p = b.interpolate(0.50);
        out = out && compare(0.6250, p.x(), 1E-4);
        out = out && compare(0.6250, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec2 interpolation t=0.5");
        }

        // Test hermite interpolation at t = 0.75
        p = b.interpolate(0.75);
        out = out && compare(0.9843, p.x(), 1E-4);
        out = out && compare(0.8906, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec2 interpolation t=0.75");
        }

        // Test hermite interpolation at t = 1.00
        p = b.interpolate(1.0);
        out = out && compare(1.0, p.x(), 1E-4);
        out = out && compare(1.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec2 interpolation t=1.0");
        }
    }

    // vec3 cubic
    {
        // Print size and alignment of class
        std::cout << "hermite_vec3_size: " << sizeof(min::hermite<float, min::vec3>) << std::endl;
        std::cout << "hermite_vec3_align: " << alignof(min::hermite<float, min::vec3>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "tcubic.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 12, sizeof(min::hermite<float, min::vec3>), "Failed hermite_vec3 sizeof");
        out = out && test(sizeof(float), alignof(min::hermite<float, min::vec3>), "Failed hermite_vec3 alignof");
#endif

        const min::vec3<float> p1(0.0, 0.0, 0.0);
        const min::vec3<float> p2(1.0, 1.0, 1.0);
        const min::vec3<float> t1(0.0, 1.0, 0.0);
        const min::vec3<float> t2(1.0, 0.0, 1.0);
        min::vec3<float> p;

        // Local variables
        min::hermite<float, min::vec3> b(p1, p2, t1, t2);

        // Test hermite interpolation at t = 0.0
        p = b.interpolate(0.0);
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        out = out && compare(0.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec3 interpolation t=0.0");
        }

        // Test hermite interpolation at t = 0.0625
        p = b.interpolate(0.0625);
        out = out && compare(0.0148, p.x(), 1E-4);
        out = out && compare(0.0661, p.y(), 1E-4);
        out = out && compare(0.0148, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec3 interpolation t=0.0625");
        }

        // Test hermite interpolation at t = 0.125
        p = b.interpolate(0.125);
        out = out && compare(0.0566, p.x(), 1E-4);
        out = out && compare(0.1386, p.y(), 1E-4);
        out = out && compare(0.0566, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec3 interpolation t=0.125");
        }

        // Test hermite interpolation at t = 0.25
        p = b.interpolate(0.25);
        out = out && compare(0.2031, p.x(), 1E-4);
        out = out && compare(0.2968, p.y(), 1E-4);
        out = out && compare(0.2031, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec3 interpolation t=0.25");
        }

        // Test hermite interpolation at t = 0.50
        p = b.interpolate(0.50);
        out = out && compare(0.6250, p.x(), 1E-4);
        out = out && compare(0.6250, p.y(), 1E-4);
        out = out && compare(0.6250, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec3 interpolation t=0.5");
        }

        // Test hermite interpolation at t = 0.75
        p = b.interpolate(0.75);
        out = out && compare(0.9843, p.x(), 1E-4);
        out = out && compare(0.8906, p.y(), 1E-4);
        out = out && compare(0.9843, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec3 interpolation t=0.75");
        }

        // Test hermite interpolation at t = 1.00
        p = b.interpolate(1.0);
        out = out && compare(1.0, p.x(), 1E-4);
        out = out && compare(1.0, p.y(), 1E-4);
        out = out && compare(1.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec3 interpolation t=1.0");
        }
    }

    // vec4 cubic
    {
        // Print size and alignment of class
        std::cout << "hermite_vec4_size: " << sizeof(min::hermite<float, min::vec4>) << std::endl;
        std::cout << "hermite_vec4_align: " << alignof(min::hermite<float, min::vec4>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "tcubic.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 16, sizeof(min::hermite<float, min::vec4>), "Failed hermite_vec4 sizeof");
        out = out && test(sizeof(float), alignof(min::hermite<float, min::vec4>), "Failed hermite_vec4 alignof");
#endif

        const min::vec4<float> p1(0.0, 0.0, 0.0, 1.0);
        const min::vec4<float> p2(1.0, 1.0, 1.0, 1.0);
        const min::vec4<float> t1(0.0, 1.0, 0.0, 1.0);
        const min::vec4<float> t2(1.0, 0.0, 1.0, 1.0);
        min::vec4<float> p;

        // Local variables
        min::hermite<float, min::vec4> b(p1, p2, t1, t2);

        // Test hermite interpolation at t = 0.0
        p = b.interpolate(0.0);
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        out = out && compare(0.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec4 interpolation t=0.0");
        }

        // Test hermite interpolation at t = 0.0625
        p = b.interpolate(0.0625);
        out = out && compare(0.0148, p.x(), 1E-4);
        out = out && compare(0.0661, p.y(), 1E-4);
        out = out && compare(0.0148, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec4 interpolation t=0.0625");
        }

        // Test hermite interpolation at t = 0.125
        p = b.interpolate(0.125);
        out = out && compare(0.0566, p.x(), 1E-4);
        out = out && compare(0.1386, p.y(), 1E-4);
        out = out && compare(0.0566, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec4 interpolation t=0.125");
        }

        // Test hermite interpolation at t = 0.25
        p = b.interpolate(0.25);
        out = out && compare(0.2031, p.x(), 1E-4);
        out = out && compare(0.2968, p.y(), 1E-4);
        out = out && compare(0.2031, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec4 interpolation t=0.25");
        }

        // Test hermite interpolation at t = 0.50
        p = b.interpolate(0.50);
        out = out && compare(0.6250, p.x(), 1E-4);
        out = out && compare(0.6250, p.y(), 1E-4);
        out = out && compare(0.6250, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec4 interpolation t=0.5");
        }

        // Test hermite interpolation at t = 0.75
        p = b.interpolate(0.75);
        out = out && compare(0.9843, p.x(), 1E-4);
        out = out && compare(0.8906, p.y(), 1E-4);
        out = out && compare(0.9843, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec4 interpolation t=0.75");
        }

        // Test hermite interpolation at t = 1.00
        p = b.interpolate(1.0);
        out = out && compare(1.0, p.x(), 1E-4);
        out = out && compare(1.0, p.y(), 1E-4);
        out = out && compare(1.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed hermite vec4 interpolation t=1.0");
        }
    }

    return out;
}
#endif
