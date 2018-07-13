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
#ifndef __TESTOOBBOX__
#define __TESTOOBBOX__

#include <min/oobbox.h>
#include <min/test.h>
#include <min/vec2.h>
#include <min/vec3.h>
#include <min/vec4.h>
#include <stdexcept>
#include <vector>

bool test_oobbox()
{
    bool out = true;

    // vec2 oobbox = circle
    {
        // Print size and alignment of class
        std::cout << "oobbox_vec2_size: " << sizeof(min::oobbox<float, min::vec2>) << std::endl;
        std::cout << "oobbox_vec2_align: " << alignof(min::oobbox<float, min::vec2>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "toobbox.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 12, sizeof(min::oobbox<float, min::vec2>), "Failed oobbox vec2 sizeof");
        out = out && test(sizeof(float), alignof(min::oobbox<float, min::vec2>), "Failed oobbox vec2 alignof");
#endif

        // Local variables
        min::vec2<double> a(-2.0, 0.0);
        min::vec2<double> b(2.0, 4.0);
        min::vec2<double> c(3.0, 5.0);
        min::vec2<double> p;
        min::oobbox<double, min::vec2> box;
        double d;

        // Test oobbox vector constructor
        std::vector<min::vec2<double>> v = {a, b, c};
        box = min::oobbox<double, min::vec2>(v);

        // Test box min
        p = box.get_min();
        out = out && compare(-2.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox vector min");
        }

        // Test box max
        p = box.get_max();
        out = out && compare(3.0, p.x(), 1E-4);
        out = out && compare(5.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox vector max");
        }

        // Test box center
        p = box.get_center();
        out = out && compare(0.5, p.x(), 1E-4);
        out = out && compare(2.5, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox vector center");
        }

        // Test closest point
        box = min::oobbox<double, min::vec2>(a, b);
        p = box.closest_point(c);
        out = out && compare(2.0, p.x(), 1E-4);
        out = out && compare(4.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox closest_point 1");
        }

        // Test roation, Rotate by 45 degrees across Z axis
        box.set_rotation(min::mat2<double>(45.0));

        // Test get_min()
        p = box.get_min();
        out = out && compare(-2.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox get_min");
        }

        // Test get_max()
        p = box.get_max();
        out = out && compare(2.0, p.x(), 1E-4);
        out = out && compare(4.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox get_max");
        }

        // Test get_extent()
        p = box.get_extent();
        out = out && compare(4.0, p.x(), 1E-4);
        out = out && compare(4.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox get_extent");
        }

        // Test point inside
        p = min::vec2<double>(1.0, 3.0);
        out = out && box.point_inside(p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox point_inside");
        }

        // Test not point inside
        out = out && !box.point_inside(b);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox not point_inside");
        }

        // Test closest point
        p = box.closest_point(c);
        out = out && compare(1.4142, p.x(), 1E-4);
        out = out && compare(3.4142, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox closest_point 2");
        }

        // Test closest point
        p = min::vec2<double>(-3.0, 5.0);
        p = box.closest_point(p);
        out = out && compare(-1.4142, p.x(), 1E-4);
        out = out && compare(3.4142, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox closest_point 3");
        }

        // Test point inside
        p = min::vec2<double>(-1.41421, 3.41421);
        out = out && box.point_inside(p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox point_inside 2");
        }

        // Test point inside
        p = min::vec2<double>(-2.0, 2.0);
        out = out && box.point_inside(p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox point_inside 3");
        }

        // Test not point inside
        p = min::vec2<double>(1.41422, 3.41422);
        out = out && !box.point_inside(p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox not point_inside 2");
        }

        // Test not point inside
        p = min::vec2<double>(-1.41422, 3.41422);
        out = out && !box.point_inside(p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox not point_inside 3");
        }

        // Test roation, Rotate by 90 degrees across Z axis
        box.set_rotation(min::mat2<double>(90.0));

        // Test closest point
        p = box.closest_point(c);
        out = out && compare(2.0, p.x(), 1E-4);
        out = out && compare(4.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox closest_point 4");
        }

        // Test square distance
        d = box.square_distance(c);
        out = out && compare(2.0, d, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox square distance");
        }

        // Test point inside
        out = out && box.point_inside(b - 0.01);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox point_inside 5");
        }

        // Test point not inside
        out = out && !box.point_inside(b);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox not point_inside 4");
        }

        // Test square size
        d = box.square_size();
        out = out && compare(32.0, d, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox square_size");
        }
    }

    // vec3 oobbox
    {
        // Print size and alignment of class
        std::cout << "oobbox_vec3_size: " << sizeof(min::oobbox<float, min::vec3>) << std::endl;
        std::cout << "oobbox_vec3_align: " << alignof(min::oobbox<float, min::vec3>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "toobbox.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 19, sizeof(min::oobbox<float, min::vec3>), "Failed oobbox vec3 sizeof");
        out = out && test(sizeof(float), alignof(min::oobbox<float, min::vec3>), "Failed oobbox vec3 alignof");
#endif

        // Local variables
        min::vec3<double> a(-2.0, 0.0, -2.0);
        min::vec3<double> b(2.0, 4.0, 2.0);
        min::vec3<double> c(3.0, 5.0, 3.0);
        min::vec3<double> p;
        min::vec3<double> z(0.0, 0.0, 1.0);
        min::oobbox<double, min::vec3> box;
        double d;

        // Test oobbox vector constructor
        std::vector<min::vec3<double>> v = {a, b, c};
        box = min::oobbox<double, min::vec3>(v);

        // Test box min
        p = box.get_min();
        out = out && compare(-2.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        out = out && compare(-2.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox vector min");
        }

        // Test box max
        p = box.get_max();
        out = out && compare(3.0, p.x(), 1E-4);
        out = out && compare(5.0, p.y(), 1E-4);
        out = out && compare(3.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox vector max");
        }

        // Test box center
        p = box.get_center();
        out = out && compare(0.5, p.x(), 1E-4);
        out = out && compare(2.5, p.y(), 1E-4);
        out = out && compare(0.5, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox vector center");
        }

        // Test closest point
        box = min::oobbox<double, min::vec3>(a, b);
        p = box.closest_point(c);
        out = out && compare(2.0, p.x(), 1E-4);
        out = out && compare(4.0, p.y(), 1E-4);
        out = out && compare(2.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox closest_point 1");
        }

        // Test roation, Rotate by 45 degrees across Z axis
        box.set_rotation(min::quat<double>(z, 45.0));

        // Test get_min()
        p = box.get_min();
        out = out && compare(-2.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        out = out && compare(-2.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox get_min");
        }

        // Test get_max()
        p = box.get_max();
        out = out && compare(2.0, p.x(), 1E-4);
        out = out && compare(4.0, p.y(), 1E-4);
        out = out && compare(2.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox get_max");
        }

        // Test get_extent()
        p = box.get_extent();
        out = out && compare(4.0, p.x(), 1E-4);
        out = out && compare(4.0, p.y(), 1E-4);
        out = out && compare(4.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox get_extent");
        }

        // Test point inside
        p = min::vec3<double>(1.0, 3.0, 0.0);
        out = out && box.point_inside(p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox point_inside");
        }

        // Test not point inside
        out = out && !box.point_inside(b);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox not point_inside");
        }

        // Test closest point
        p = box.closest_point(c);
        out = out && compare(1.4142, p.x(), 1E-4);
        out = out && compare(3.4142, p.y(), 1E-4);
        out = out && compare(2.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox closest_point 2");
        }

        // Test closest point
        p = min::vec3<double>(-3.0, 5.0, 0.0);
        p = box.closest_point(p);
        out = out && compare(-1.4142, p.x(), 1E-4);
        out = out && compare(3.4142, p.y(), 1E-4);
        out = out && compare(0.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox closest_point 3");
        }

        // Test point inside
        p = min::vec3<double>(-1.41421, 3.41421, 0.0);
        out = out && box.point_inside(p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox point_inside 2");
        }

        // Test point inside
        p = min::vec3<double>(-2.0, 2.0, 0.0);
        out = out && box.point_inside(p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox point_inside 3");
        }

        // Test not point inside
        p = min::vec3<double>(1.41422, 3.41422, 0.0);
        out = out && !box.point_inside(p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox not point_inside 2");
        }

        // Test not point inside
        p = min::vec3<double>(-1.41422, 3.41422, 0.0);
        out = out && !box.point_inside(p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox not point_inside 3");
        }

        // Test roation, Rotate by 90 degrees across Z axis
        box.set_rotation(min::quat<double>(z, 90.0));

        // Test closest point
        p = box.closest_point(c);
        out = out && compare(2.0, p.x(), 1E-4);
        out = out && compare(4.0, p.y(), 1E-4);
        out = out && compare(2.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox closest_point 4");
        }

        // Test square distance
        d = box.square_distance(c);
        out = out && compare(3.0, d, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox square distance");
        }

        // Test point inside
        out = out && box.point_inside(b - 0.01);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox point_inside 5");
        }

        // Test point not inside
        out = out && !box.point_inside(b);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox not point_inside 4");
        }

        // Test square size
        d = box.square_size();
        out = out && compare(48.0, d, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox square_size");
        }
    }

    // vec4 oobbox
    {
        // Print size and alignment of class
        std::cout << "oobbox_vec4_size: " << sizeof(min::oobbox<float, min::vec4>) << std::endl;
        std::cout << "oobbox_vec4_align: " << alignof(min::oobbox<float, min::vec4>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "toobbox.h: Testing alignment" << std::endl;
        out = out && test(sizeof(float) * 24, sizeof(min::oobbox<float, min::vec4>), "Failed oobbox vec4 sizeof");
        out = out && test(sizeof(float), alignof(min::oobbox<float, min::vec4>), "Failed oobbox vec4 alignof");
#endif

        // Local variables
        min::vec4<double> a(-2.0, 0.0, -2.0, 1.0);
        min::vec4<double> b(2.0, 4.0, 2.0, 1.0);
        min::vec4<double> c(3.0, 5.0, 3.0, 1.0);
        min::vec4<double> p;
        min::vec4<double> z(0.0, 0.0, 1.0, 1.0);
        min::oobbox<double, min::vec4> box;
        double d;

        // Test oobbox vector constructor
        std::vector<min::vec4<double>> v = {a, b, c};
        box = min::oobbox<double, min::vec4>(v);

        // Test box min
        p = box.get_min();
        out = out && compare(-2.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        out = out && compare(-2.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox vector min");
        }

        // Test box max
        p = box.get_max();
        out = out && compare(3.0, p.x(), 1E-4);
        out = out && compare(5.0, p.y(), 1E-4);
        out = out && compare(3.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox vector max");
        }

        // Test box center
        p = box.get_center();
        out = out && compare(0.5, p.x(), 1E-4);
        out = out && compare(2.5, p.y(), 1E-4);
        out = out && compare(0.5, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox vector center");
        }

        // Test closest point
        box = min::oobbox<double, min::vec4>(a, b);
        p = box.closest_point(c);
        out = out && compare(2.0, p.x(), 1E-4);
        out = out && compare(4.0, p.y(), 1E-4);
        out = out && compare(2.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox closest_point 1");
        }

        // Test roation, Rotate by 45 degrees across Z axis
        box.set_rotation(min::quat<double>(z, 45.0));

        // Test get_min()
        p = box.get_min();
        out = out && compare(-2.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        out = out && compare(-2.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox get_min");
        }

        // Test get_max()
        p = box.get_max();
        out = out && compare(2.0, p.x(), 1E-4);
        out = out && compare(4.0, p.y(), 1E-4);
        out = out && compare(2.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox get_max");
        }

        // Test get_extent()
        p = box.get_extent();
        out = out && compare(4.0, p.x(), 1E-4);
        out = out && compare(4.0, p.y(), 1E-4);
        out = out && compare(4.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox get_extent");
        }

        // Test point inside
        p = min::vec4<double>(1.0, 3.0, 0.0, 1.0);
        out = out && box.point_inside(p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox point_inside");
        }

        // Test not point inside
        out = out && !box.point_inside(b);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox not point_inside");
        }

        // Test closest point
        p = box.closest_point(c);
        out = out && compare(1.4142, p.x(), 1E-4);
        out = out && compare(3.4142, p.y(), 1E-4);
        out = out && compare(2.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox closest_point 2");
        }

        // Test closest point
        p = min::vec4<double>(-3.0, 5.0, 0.0, 1.0);
        p = box.closest_point(p);
        out = out && compare(-1.4142, p.x(), 1E-4);
        out = out && compare(3.4142, p.y(), 1E-4);
        out = out && compare(0.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox closest_point 3");
        }

        // Test point inside
        p = min::vec4<double>(-1.41421, 3.41421, 0.0, 1.0);
        out = out && box.point_inside(p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox point_inside 2");
        }

        // Test point inside
        p = min::vec4<double>(-2.0, 2.0, 0.0, 1.0);
        out = out && box.point_inside(p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox point_inside 3");
        }

        // Test not point inside
        p = min::vec4<double>(1.41422, 3.41422, 0.0, 1.0);
        out = out && !box.point_inside(p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox not point_inside 2");
        }

        // Test not point inside
        p = min::vec4<double>(-1.41422, 3.41422, 0.0, 1.0);
        out = out && !box.point_inside(p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox not point_inside 3");
        }

        // Test roation, Rotate by 90 degrees across Z axis
        box.set_rotation(min::quat<double>(z, 90.0));

        // Test closest point
        p = box.closest_point(c);
        out = out && compare(2.0, p.x(), 1E-4);
        out = out && compare(4.0, p.y(), 1E-4);
        out = out && compare(2.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox closest_point 4");
        }

        // Test square distance
        d = box.square_distance(c);
        out = out && compare(3.0, d, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox square distance");
        }

        // Test point inside
        out = out && box.point_inside(b - 0.01);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox point_inside 5");
        }

        // Test point not inside
        out = out && !box.point_inside(b);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox not point_inside 4");
        }

        // Test square size
        d = box.square_size();
        out = out && compare(48.0, d, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox square_size");
        }

        // Test roation, Rotate by 45 degrees across Z axis
        box.set_rotation(min::quat<double>(z, 45.0));

        // Test box align on (P-center)
        p = box.align(min::vec3<double>(6.0, 6.0, 6.0) - box.get_center());
        out = out && compare(7.0710, p.x(), 1E-4);
        out = out && compare(-1.4142, p.y(), 1E-4);
        out = out && compare(6.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox align z-axis");
        }

        // Test roation, Rotate by 45 degrees across Z axis
        a = min::vec3<double>(1.0, 1.0, 1.0).normalize();
        box.set_rotation(min::quat<double>(a, 25.0));

        // Test box align multi-axis on (P-center)
        p = box.align(min::vec3<double>(6.0, 6.0, 6.0) - box.get_center());
        out = out && compare(5.4495, p.x(), 1E-4);
        out = out && compare(4.1249, p.y(), 1E-4);
        out = out && compare(6.4255, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox align multi-axis");
        }
    }
    return out;
}
#endif
