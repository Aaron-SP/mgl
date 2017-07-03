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
#ifndef __TESTOOBBOXINTERSECT__
#define __TESTOobox1INTERSECT__

#include <min/intersect.h>
#include <min/test.h>
#include <stdexcept>

bool test_oobbox_intersect()
{
    bool out = true;

    // vec2 intersect
    {
        // Local variables
        min::vec2<double> a(-2.0, -2.0);
        min::vec2<double> b(2.0, 2.0);
        min::vec2<double> c(3.41421, 3.41421);
        min::vec2<double> p;
        min::oobbox<double, min::vec2> obox1(a, b);
        min::oobbox<double, min::vec2> obox2(a, b);
        min::aabbox<double, min::vec2> abox(a, b);

        // Test rotation, Rotate by 45 degrees across Z axis
        obox1.set_rotation(min::mat2<double>(45.0));
        obox1.set_position(c);

        // Test AABB-OOBB intersection
        out = out && min::intersect(obox1, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabb-oobb intersection 1");
        }

        // Test collision point
        out = out && compare(2.0000, p.x(), 1E-4);
        out = out && compare(2.0000, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabb-oobb closest_point 1");
        }

        // Test AABB-OOBB no intersection
        c = min::vec2<double>(3.41422, 3.41422);
        obox1.set_position(c);
        out = out && !min::intersect(obox1, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabb-oobb no intersection 1");
        }

        // Test collision point
        out = out && compare(2.00001, p.x(), 1E-5);
        out = out && compare(2.00001, p.y(), 1E-5);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabb-oobb closest_point 2");
        }

        // Test intersection on the weak axis
        c = min::vec2<double>(-3.41421, 3.41421);
        obox1.set_position(c);

        // Test AABB-OOBB intersection
        out = out && min::intersect(obox1, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabb-oobb intersection 2");
        }

        // Test collision point
        out = out && compare(-2.0000, p.x(), 1E-4);
        out = out && compare(2.0000, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabb-oobb closest_point 3");
        }

        // Test no intersection on the weak axis
        c = min::vec2<double>(-3.41422, 3.41422);
        obox1.set_position(c);

        // Test AABB-OOBB intersection
        out = out && !min::intersect(obox1, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabb-oobb no intersection 2");
        }

        // Test collision point
        out = out && compare(-2.00001, p.x(), 1E-5);
        out = out && compare(2.00001, p.y(), 1E-5);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabb-oobb closest_point 4");
        }

        // Reset box1 position
        c = min::vec2<double>(0.0, 0.0);
        obox1.set_rotation(min::mat2<double>(45.0));
        obox1.set_position(c);

        // Rotate box2 by 45 degrees across Z axis, move to (4,0)
        c = min::vec2<double>(5.65685, 0.0);
        obox2.set_rotation(min::mat2<double>(45.0));
        obox2.set_position(c);

        // Test OOBB-OOBB intersection
        out = out && min::intersect(obox1, obox2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobb-oobb intersection 1");
        }

        // Test collision point
        out = out && compare(2.82842, p.x(), 1E-5);
        out = out && compare(0.00000, p.y(), 1E-5);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobb-oobb closest_point 1");
        }

        // Move box2 out of range
        c = min::vec2<double>(5.657, 0.0);
        obox2.set_position(c);

        // Test OOBB-OOBB no intersection
        out = out && !min::intersect(obox1, obox2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobb-oobb no intersection 1");
        }

        // Move box2 to align on diagonal
        c = min::vec2<double>(2.82842, 2.82842);
        obox2.set_position(c);

        // Test OOBB-OOBB no intersection
        out = out && min::intersect(obox1, obox2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobb-oobb intersection 2");
        }

        // Test collision point
        out = out && compare(1.41421, p.x(), 1E-5);
        out = out && compare(1.41421, p.y(), 1E-5);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobb-oobb closest_point 2");
        }

        // Move box2 out of range
        c = min::vec2<double>(2.82843, 2.82843);
        obox2.set_position(c);

        // Test OOBB-OOBB no intersection
        out = out && !min::intersect(obox1, obox2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobb-oobb no intersection 2");
        }
    }

    // vec3 intersect
    {
        // Local variables
        min::vec3<double> a(-2.0, -2.0, -2.0);
        min::vec3<double> b(2.0, 2.0, 2.0);
        min::vec3<double> c(3.41421, 3.41421, 0.0);
        min::vec3<double> z(0.0, 0.0, 1.0);
        min::vec3<double> p;
        min::oobbox<double, min::vec3> obox1(a, b);
        min::oobbox<double, min::vec3> obox2(a, b);
        min::aabbox<double, min::vec3> abox(a, b);

        // Test rotation, Rotate by 45 degrees across Z axis
        obox1.set_rotation(min::quat<double>(z, 45.0));
        obox1.set_position(c);

        // Test AABB-OOBB intersection
        out = out && min::intersect(obox1, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabb-oobb intersection 1");
        }

        // Test collision point
        out = out && compare(2.0000, p.x(), 1E-4);
        out = out && compare(2.0000, p.y(), 1E-4);
        out = out && compare(0.0000, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabb-oobb closest_point 1");
        }

        // Test AABB-OOBB no intersection
        c = min::vec3<double>(3.41422, 3.41422, 0.0);
        obox1.set_position(c);
        out = out && !min::intersect(obox1, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabb-oobb no intersection 1");
        }

        // Test collision point
        out = out && compare(2.00001, p.x(), 1E-5);
        out = out && compare(2.00001, p.y(), 1E-5);
        out = out && compare(0.00000, p.z(), 1E-5);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabb-oobb closest_point 2");
        }

        // Test intersection on the weak axis
        c = min::vec3<double>(-3.41421, 3.41421, 0.0);
        obox1.set_position(c);

        // Test AABB-OOBB intersection
        out = out && min::intersect(obox1, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabb-oobb intersection 2");
        }

        // Test collision point
        out = out && compare(-2.0000, p.x(), 1E-4);
        out = out && compare(2.0000, p.y(), 1E-4);
        out = out && compare(0.0000, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabb-oobb closest_point 3");
        }

        // Test no intersection on the weak axis
        c = min::vec3<double>(-3.41422, 3.41422, 0.0);
        obox1.set_position(c);

        // Test AABB-OOBB intersection
        out = out && !min::intersect(obox1, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabb-oobb no intersection 2");
        }

        // Test collision point
        out = out && compare(-2.00001, p.x(), 1E-5);
        out = out && compare(2.00001, p.y(), 1E-5);
        out = out && compare(0.00000, p.z(), 1E-5);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabb-oobb closest_point 4");
        }

        // Reset box1 position
        c = min::vec3<double>(0.0, 0.0, 0.0);
        obox1.set_rotation(min::quat<double>(z, 45.0));
        obox1.set_position(c);

        // Rotate box2 by 45 degrees across Z axis, move to (4,0)
        c = min::vec3<double>(5.65685, 0.0, 0.0);
        obox2.set_rotation(min::quat<double>(z, 45.0));
        obox2.set_position(c);

        // Test OOBB-OOBB intersection
        out = out && min::intersect(obox1, obox2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobb-oobb intersection 1");
        }

        // Test collision point
        out = out && compare(2.82842, p.x(), 1E-5);
        out = out && compare(0.00000, p.y(), 1E-5);
        out = out && compare(0.00000, p.z(), 1E-5);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobb-oobb closest_point 1");
        }

        // Move box2 out of range
        c = min::vec3<double>(5.657, 0.0, 0.0);
        obox2.set_position(c);

        // Test OOBB-OOBB no intersection
        out = out && !min::intersect(obox1, obox2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobb-oobb no intersection 1");
        }

        // Move box2 to align on diagonal
        c = min::vec3<double>(2.82842, 2.82842, 0.0);
        obox2.set_position(c);

        // Test OOBB-OOBB no intersection
        out = out && min::intersect(obox1, obox2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobb-oobb intersection 2");
        }

        // Test collision point
        out = out && compare(1.41421, p.x(), 1E-5);
        out = out && compare(1.41421, p.y(), 1E-5);
        out = out && compare(0.0, p.z(), 1E-5);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobb-oobb closest_point 2");
        }

        // Move box2 out of range
        c = min::vec3<double>(2.82843, 2.82843, 0.0);
        obox2.set_position(c);

        // Test OOBB-OOBB no intersection
        out = out && !min::intersect(obox1, obox2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobb-oobb no intersection 2");
        }
    }

    // vec4 intersect
    {
        // Local variables
        min::vec4<double> a(-2.0, -2.0, -2.0, 1.0);
        min::vec4<double> b(2.0, 2.0, 2.0, 1.0);
        min::vec4<double> c(3.41421, 3.41421, 0.0, 1.0);
        min::vec4<double> z(0.0, 0.0, 1.0, 1.0);
        min::vec4<double> p;
        min::oobbox<double, min::vec4> obox1(a, b);
        min::oobbox<double, min::vec4> obox2(a, b);
        min::aabbox<double, min::vec4> abox(a, b);

        // Test rotation, Rotate by 45 degrees across Z axis
        obox1.set_rotation(min::quat<double>(z, 45.0));
        obox1.set_position(c);

        // Test AABB-OOBB intersection
        out = out && min::intersect(obox1, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabb-oobb intersection 1");
        }

        // Test collision point
        out = out && compare(2.0000, p.x(), 1E-4);
        out = out && compare(2.0000, p.y(), 1E-4);
        out = out && compare(0.0000, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabb-oobb closest_point 1");
        }

        // Test AABB-OOBB no intersection
        c = min::vec4<double>(3.41422, 3.41422, 0.0, 1.0);
        obox1.set_position(c);
        out = out && !min::intersect(obox1, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 no aabb-oobb no intersection 1");
        }

        // Test collision point
        out = out && compare(2.00001, p.x(), 1E-5);
        out = out && compare(2.00001, p.y(), 1E-5);
        out = out && compare(0.00000, p.z(), 1E-5);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabb-oobb closest_point 2");
        }

        // Test intersection on the weak axis
        c = min::vec4<double>(-3.41421, 3.41421, 0.0, 1.0);
        obox1.set_position(c);

        // Test AABB-OOBB intersection
        out = out && min::intersect(obox1, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabb-oobb intersection 2");
        }

        // Test collision point
        out = out && compare(-2.0000, p.x(), 1E-4);
        out = out && compare(2.0000, p.y(), 1E-4);
        out = out && compare(0.0000, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabb-oobb closest_point 3");
        }

        // Test no intersection on the weak axis
        c = min::vec4<double>(-3.41422, 3.41422, 0.0, 1.0);
        obox1.set_position(c);

        // Test AABB-OOBB intersection
        out = out && !min::intersect(obox1, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabb-oobb no intersection 2");
        }

        // Test collision point
        out = out && compare(-2.00001, p.x(), 1E-5);
        out = out && compare(2.00001, p.y(), 1E-5);
        out = out && compare(0.00000, p.z(), 1E-5);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabb-oobb closest_point 4");
        }

        // Reset box1 position
        c = min::vec4<double>(0.0, 0.0, 0.0, 1.0);
        obox1.set_rotation(min::quat<double>(z, 45.0));
        obox1.set_position(c);

        // Rotate box2 by 45 degrees across Z axis, move to (4,0)
        c = min::vec4<double>(5.65685, 0.0, 0.0, 1.0);
        obox2.set_rotation(min::quat<double>(z, 45.0));
        obox2.set_position(c);

        // Test OOBB-OOBB intersection
        out = out && min::intersect(obox1, obox2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobb-oobb intersection 1");
        }

        // Test collision point
        out = out && compare(2.82842, p.x(), 1E-5);
        out = out && compare(0.00000, p.y(), 1E-5);
        out = out && compare(0.00000, p.z(), 1E-5);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobb-oobb closest_point 1");
        }

        // Move box2 out of range
        c = min::vec4<double>(5.657, 0.0, 0.0, 1.0);
        obox2.set_position(c);

        // Test OOBB-OOBB no intersection
        out = out && !min::intersect(obox1, obox2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobb-oobb no intersection 1");
        }

        // Move box2 to align on diagonal
        c = min::vec4<double>(2.82842, 2.82842, 0.0, 1.0);
        obox2.set_position(c);

        // Test OOBB-OOBB no intersection
        out = out && min::intersect(obox1, obox2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobb-oobb intersection 2");
        }

        // Test collision point
        out = out && compare(1.41421, p.x(), 1E-5);
        out = out && compare(1.41421, p.y(), 1E-5);
        out = out && compare(0.0, p.z(), 1E-5);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobb-oobb closest_point 2");
        }

        // Move box2 out of range
        c = min::vec4<double>(2.82843, 2.82843, 0.0, 1.0);
        obox2.set_position(c);

        // Test OOBB-OOBB no intersection
        out = out && !min::intersect(obox1, obox2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobb-oobb no intersection 2");
        }
    }

    return out;
}

#endif