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
        out = out && box.point_inside(b);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobbox point_inside 5");
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
        out = out && box.point_inside(b);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobbox point_inside 5");
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
        out = out && box.point_inside(b);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox point_inside 5");
        }

        // Test square size
        d = box.square_size();
        out = out && compare(48.0, d, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobbox square_size");
        }
    }
    return out;
}
#endif
