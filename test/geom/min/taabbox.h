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
#ifndef _MGL_TESTAABBOX_MGL_
#define _MGL_TESTAABBOX_MGL_

#include <min/aabbox.h>
#include <min/test.h>
#include <min/vec2.h>
#include <min/vec3.h>
#include <min/vec4.h>
#include <stdexcept>
#include <vector>

template <typename T, template <typename> class vec>
bool points_inside(const min::aabbox<T, vec> &box, const std::vector<vec<T>> &verts)
{
    bool out = true;
    for (const auto &v : verts)
    {
        out = out && box.point_inside(v);
    }

    return out;
}

template bool points_inside<double, min::vec2>(const min::aabbox<double, min::vec2> &, const std::vector<min::vec2<double>> &);
template bool points_inside<double, min::vec3>(const min::aabbox<double, min::vec3> &, const std::vector<min::vec3<double>> &);
template bool points_inside<double, min::vec4>(const min::aabbox<double, min::vec4> &, const std::vector<min::vec4<double>> &);

bool test_aabbox()
{
    bool out = true;

    // vec2 aabbox = circle
    {
        // Print size and alignment of class
        std::cout << "aabbox_vec2_size: " << sizeof(min::aabbox<float, min::vec2>) << std::endl;
        std::cout << "aabbox_vec2_align: " << alignof(min::aabbox<float, min::vec2>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "taabbox.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 4, sizeof(min::aabbox<float, min::vec2>), "Failed aabbox vec2 sizeof");
        out = out && test(sizeof(float), alignof(min::aabbox<float, min::vec2>), "Failed aabbox vec2 alignof");
#endif

        // Local variables
        min::aabbox<double, min::vec2> box;
        min::vec2<double> a;
        min::vec2<double> b;
        min::vec2<double> c;
        min::vec2<double> ext;
        double r;
        std::vector<min::vec2<double>> v;

        // Test aabbox constructor
        a = min::vec2<double>(1.0, 1.0);
        b = min::vec2<double>(2.0, 2.0);
        c = min::vec2<double>(3.0, 3.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        box = min::aabbox<double, min::vec2>(v);
        out = out && points_inside(box, v);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox constructor");
        }

        // Test aabbox get_center
        c = box.get_center();
        out = out && compare(2.0, c.x(), 1E-4);
        out = out && compare(2.0, c.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox get_center");
        }

        // Test aabbox get_min
        ext = box.get_min();
        out = out && compare(1.0, ext.x(), 1E-4);
        out = out && compare(1.0, ext.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox get_min");
        }

        // Test aabbox get_max
        ext = box.get_max();
        out = out && compare(3.0, ext.x(), 1E-4);
        out = out && compare(3.0, ext.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox get_max");
        }

        // Test aabbox get_extent
        ext = box.get_extent();
        out = out && compare(2.0, ext.x(), 1E-4);
        out = out && compare(2.0, ext.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox get_extent");
        }

        // Test add points_inside
        v.clear();
        a = min::vec2<double>(0.0, 0.0);
        b = min::vec2<double>(1.5, 1.5);
        c = min::vec2<double>(4.0, 4.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        box.add(v);
        out = out && points_inside(box, v);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox add points");
        }

        // Test add aabbox get_center
        c = box.get_center();
        out = out && compare(2.0, c.x(), 1E-4);
        out = out && compare(2.0, c.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox add get_center");
        }

        // Test aabbox get_min
        ext = box.get_min();
        out = out && compare(0.0, ext.x(), 1E-4);
        out = out && compare(0.0, ext.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox add get_min");
        }

        // Test aabbox get_max
        ext = box.get_max();
        out = out && compare(4.0, ext.x(), 1E-4);
        out = out && compare(4.0, ext.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox add get_max");
        }

        // Test aabbox get_extent
        ext = box.get_extent();
        out = out && compare(4.0, ext.x(), 1E-4);
        out = out && compare(4.0, ext.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox add get_extent");
        }

        // Test closest_point
        b = min::vec2<double>(6.0, 6.0);
        c = box.closest_point(b);
        out = out && compare(4.0, c.x(), 1E-4);
        out = out && compare(4.0, c.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox closest_point");
        }

        // Test square_size()
        a = min::vec2<double>(-1.0, -1.0);
        b = min::vec2<double>(1.0, 1.0);
        box = min::aabbox<double, min::vec2>(a, b);
        r = box.square_size();
        out = out && compare(8.0, r, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox square_size");
        }
    }

    // vec3 aabbox
    {
        // Print size and alignment of class
        std::cout << "aabbox_vec3_size: " << sizeof(min::aabbox<float, min::vec3>) << std::endl;
        std::cout << "aabbox_vec3_align: " << alignof(min::aabbox<float, min::vec3>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "taabbox.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 6, sizeof(min::aabbox<float, min::vec3>), "Failed aabbox vec3 sizeof");
        out = out && test(sizeof(float), alignof(min::aabbox<float, min::vec3>), "Failed aabbox vec3 alignof");
#endif

        // Local variables
        min::aabbox<double, min::vec3> box;
        min::vec3<double> a;
        min::vec3<double> b;
        min::vec3<double> c;
        min::vec3<double> ext;
        double r;
        std::vector<min::vec3<double>> v;

        // Test aabbox constructor
        a = min::vec3<double>(1.0, 1.0, 1.0);
        b = min::vec3<double>(2.0, 2.0, 2.0);
        c = min::vec3<double>(3.0, 3.0, 3.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        box = min::aabbox<double, min::vec3>(v);
        out = out && points_inside(box, v);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabbox constructor");
        }

        // Test aabbox get_center
        c = box.get_center();
        out = out && compare(2.0, c.x(), 1E-4);
        out = out && compare(2.0, c.y(), 1E-4);
        out = out && compare(2.0, c.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabbox get_center");
        }

        // Test aabbox get_min
        ext = box.get_min();
        out = out && compare(1.0, ext.x(), 1E-4);
        out = out && compare(1.0, ext.y(), 1E-4);
        out = out && compare(1.0, ext.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox get_min");
        }

        // Test aabbox get_max
        ext = box.get_max();
        out = out && compare(3.0, ext.x(), 1E-4);
        out = out && compare(3.0, ext.y(), 1E-4);
        out = out && compare(3.0, ext.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox get_max");
        }

        // Test aabbox get_extent
        ext = box.get_extent();
        out = out && compare(2.0, ext.x(), 1E-4);
        out = out && compare(2.0, ext.y(), 1E-4);
        out = out && compare(2.0, ext.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox get_extent");
        }

        // Test add points_inside
        v.clear();
        a = min::vec3<double>(0.0, 0.0, 0.0);
        b = min::vec3<double>(1.5, 1.5, 1.5);
        c = min::vec3<double>(4.0, 4.0, 4.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        box.add(v);
        out = out && points_inside(box, v);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabbox add points");
        }

        // Test add aabbox get_center
        c = box.get_center();
        out = out && compare(2.0, c.x(), 1E-4);
        out = out && compare(2.0, c.y(), 1E-4);
        out = out && compare(2.0, c.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabbox add get_center");
        }

        // Test aabbox get_min
        ext = box.get_min();
        out = out && compare(0.0, ext.x(), 1E-4);
        out = out && compare(0.0, ext.y(), 1E-4);
        out = out && compare(0.0, ext.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox add get_min");
        }

        // Test aabbox get_max
        ext = box.get_max();
        out = out && compare(4.0, ext.x(), 1E-4);
        out = out && compare(4.0, ext.y(), 1E-4);
        out = out && compare(4.0, ext.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox add get_max");
        }

        // Test aabbox get_extent
        ext = box.get_extent();
        out = out && compare(4.0, ext.x(), 1E-4);
        out = out && compare(4.0, ext.y(), 1E-4);
        out = out && compare(4.0, ext.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox add get_extent");
        }

        // Test closest_point
        b = min::vec3<double>(6.0, 6.0, 6.0);
        c = box.closest_point(b);
        out = out && compare(4.0, c.x(), 1E-4);
        out = out && compare(4.0, c.y(), 1E-4);
        out = out && compare(4.0, c.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabbox closest_point");
        }

        // Test square_size()
        a = min::vec3<double>(-1.0, -1.0, -1.0);
        b = min::vec3<double>(1.0, 1.0, 1.0);
        box = min::aabbox<double, min::vec3>(a, b);
        r = box.square_size();
        out = out && compare(12.0, r, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabbox square_size");
        }
    }

    // vec4 aabbox
    {
        // Print size and alignment of class
        std::cout << "aabbox_vec4_size: " << sizeof(min::aabbox<float, min::vec4>) << std::endl;
        std::cout << "aabbox_vec4_align: " << alignof(min::aabbox<float, min::vec4>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "taabbox.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 8, sizeof(min::aabbox<float, min::vec4>), "Failed aabbox vec4 sizeof");
        out = out && test(sizeof(float), alignof(min::aabbox<float, min::vec4>), "Failed aabbox vec4 alignof");
#endif

        // Local variables
        min::aabbox<double, min::vec4> box;
        min::vec4<double> a;
        min::vec4<double> b;
        min::vec4<double> c;
        min::vec4<double> ext;
        double r;
        std::vector<min::vec4<double>> v;

        // Test aabbox constructor
        a = min::vec4<double>(1.0, 1.0, 1.0, 0.0);
        b = min::vec4<double>(2.0, 2.0, 2.0, 0.0);
        c = min::vec4<double>(3.0, 3.0, 3.0, 0.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        box = min::aabbox<double, min::vec4>(v);
        out = out && points_inside(box, v);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabbox constructor");
        }

        // Test aabbox get_center
        c = box.get_center();
        out = out && compare(2.0, c.x(), 1E-4);
        out = out && compare(2.0, c.y(), 1E-4);
        out = out && compare(2.0, c.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabbox get_center");
        }

        // Test aabbox get_min
        ext = box.get_min();
        out = out && compare(1.0, ext.x(), 1E-4);
        out = out && compare(1.0, ext.y(), 1E-4);
        out = out && compare(1.0, ext.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox get_min");
        }

        // Test aabbox get_max
        ext = box.get_max();
        out = out && compare(3.0, ext.x(), 1E-4);
        out = out && compare(3.0, ext.y(), 1E-4);
        out = out && compare(3.0, ext.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox get_max");
        }

        // Test aabbox get_extent
        ext = box.get_extent();
        out = out && compare(2.0, ext.x(), 1E-4);
        out = out && compare(2.0, ext.y(), 1E-4);
        out = out && compare(2.0, ext.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox get_extent");
        }

        // Test add points_inside
        v.clear();
        a = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
        b = min::vec4<double>(1.5, 1.5, 1.5, 0.0);
        c = min::vec4<double>(4.0, 4.0, 4.0, 0.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        box.add(v);
        out = out && points_inside(box, v);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabbox add points");
        }

        // Test add aabbox get_center
        c = box.get_center();
        out = out && compare(2.0, c.x(), 1E-4);
        out = out && compare(2.0, c.y(), 1E-4);
        out = out && compare(2.0, c.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabbox add get_center");
        }

        // Test aabbox get_min
        ext = box.get_min();
        out = out && compare(0.0, ext.x(), 1E-4);
        out = out && compare(0.0, ext.y(), 1E-4);
        out = out && compare(0.0, ext.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox add get_min");
        }

        // Test aabbox get_max
        ext = box.get_max();
        out = out && compare(4.0, ext.x(), 1E-4);
        out = out && compare(4.0, ext.y(), 1E-4);
        out = out && compare(4.0, ext.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox add get_max");
        }

        // Test aabbox get_extent
        ext = box.get_extent();
        out = out && compare(4.0, ext.x(), 1E-4);
        out = out && compare(4.0, ext.y(), 1E-4);
        out = out && compare(4.0, ext.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox add get_extent");
        }

        // Test closest_point
        b = min::vec4<double>(6.0, 6.0, 6.0, 0.0);
        c = box.closest_point(b);
        out = out && compare(4.0, c.x(), 1E-4);
        out = out && compare(4.0, c.y(), 1E-4);
        out = out && compare(4.0, c.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabbox closest_point");
        }

        // Test square_size()
        a = min::vec4<double>(-1.0, -1.0, -1.0, 1.0);
        b = min::vec4<double>(1.0, 1.0, 1.0, 1.0);
        box = min::aabbox<double, min::vec4>(a, b);
        r = box.square_size();
        out = out && compare(12.0, r, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabbox square_size");
        }
    }
    return out;
}
#endif
