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
#ifndef __TESTOOBBRESOLVE__
#define __TESTOOBBRESOLVE__

#include <min/intersect.h>
#include <min/oobbox.h>
#include <min/test.h>
#include <stdexcept>

bool test_oobb_resolve()
{
    bool out = true;

    // vec2 resolve
    {
        // Local variables
        min::oobbox<double, min::vec2> b1;
        min::oobbox<double, min::vec2> b2;
        min::vec2<double> intersect;
        min::vec2<double> offset;
        min::vec2<double> normal;

        // Test oobb-oobb intersection
        b1 = min::oobbox<double, min::vec2>(min::vec2<double>(0.1, 0.1), min::vec2<double>(2.0, 2.0));
        b2 = min::oobbox<double, min::vec2>(min::vec2<double>(1.0, 1.0), min::vec2<double>(3.0, 3.0));

        // Rotate the oobb's by 45 degrees around z axis
        b1.set_rotation(45.0);
        b2.set_rotation(45.0);

        // Test intersection of oobbs and intersection point (b1->b2)
        out = out && min::intersect<double, min::vec2>(b1, b2, intersect);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobb-oobb resolution intersection 1");
        }

        // Test oobb-oobb intersection for validation
        out = out && compare(1.2928, intersect.x(), 1E-4);
        out = out && compare(1.2928, intersect.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobb-oobb resolution intersection validation 1");
        }

        // Test intersection of oobbs and intersection point (b2->b1)
        out = out && min::intersect<double, min::vec2>(b2, b1, intersect);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobb-oobb resolution intersection 2");
        }

        // Test oobb-oobb intersection for validation
        out = out && compare(1.72175, intersect.x(), 1E-4);
        out = out && compare(1.72175, intersect.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobb-oobb resolution intersection validation 2");
        }

        // Resolve intersecting oobbs
        offset = min::resolve<double, min::vec2>(b1, b2, normal, intersect, 1E-3);

        // Test normal vector
        out = out && compare(-0.7071, normal.x(), 1E-4);
        out = out && compare(-0.7071, normal.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobb-oobb normal vector validation");
        }

        // Test oobb-oobb intersection for validation
        out = out && compare(1.5073, intersect.x(), 1E-4);
        out = out && compare(1.5073, intersect.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobb-oobb resolution intersection validation");
        }

        // Resolve the intersection by moving b1
        b1.set_position(b1.get_center() + offset);

        // Test no intersection of oobbs and intersection point
        out = out && !min::intersect<double, min::vec2>(b1, b2);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobb-oobb resolution validation");
        }

        // Test b1 position
        out = out && compare(0.6190, b1.get_center().x(), 1E-4);
        out = out && compare(0.6190, b1.get_center().y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobb-oobb resolution offset");
        }

        // Test magnitude between oobb centers
        double distance = (b1.get_center() - b2.get_center()).magnitude();
        out = out && compare(1.9529, distance, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 oobb-oobb resolution distance");
        }
    }

    // vec3 resolve
    {
        // Local variables
        min::oobbox<double, min::vec3> b1;
        min::oobbox<double, min::vec3> b2;
        min::vec3<double> intersect;
        min::vec3<double> offset;
        min::vec3<double> normal;

        // Test oobb-oobb intersection
        b1 = min::oobbox<double, min::vec3>(min::vec3<double>(0.1, 0.1, 0.1), min::vec3<double>(2.0, 2.0, 2.0));
        b2 = min::oobbox<double, min::vec3>(min::vec3<double>(1.0, 1.0, 1.0), min::vec3<double>(3.0, 3.0, 3.0));

        // Rotate the oobb's by 45 degrees around z axis
        min::vec3<double> z(0.0, 0.0, 1.0);
        b1.set_rotation(min::quat<double>(z, 45.0));
        b2.set_rotation(min::quat<double>(z, 45.0));

        // Test intersection of oobbs and intersection point
        out = out && min::intersect<double, min::vec3>(b1, b2, intersect);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobb-oobb resolution intersection 1");
        }

        // Test oobb-oobb intersection for validation
        out = out && compare(1.2928, intersect.x(), 1E-4);
        out = out && compare(1.2928, intersect.y(), 1E-4);
        out = out && compare(1.05, intersect.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobb-oobb resolution intersection validation 1");
        }

        // Test intersection of oobbs and intersection point (b2->b1)
        out = out && min::intersect<double, min::vec3>(b2, b1, intersect);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobb-oobb resolution intersection 2");
        }

        // Test oobb-oobb intersection for validation
        out = out && compare(1.7217, intersect.x(), 1E-4);
        out = out && compare(1.7217, intersect.y(), 1E-4);
        out = out && compare(2.0, intersect.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobb-oobb resolution intersection validation 2");
        }

        // Resolve intersecting oobbs
        offset = min::resolve<double, min::vec3>(b1, b2, normal, intersect, 1E-3);

        // Test normal vector, this normal is off center box1 center is within box2
        out = out && compare(-0.7071, normal.x(), 1E-4);
        out = out && compare(-0.7071, normal.y(), 1E-4);
        out = out && compare(0.0, normal.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobb-oobb normal vector validation");
        }

        // Test oobb-oobb intersection for validation
        out = out && compare(1.5073, intersect.x(), 1E-4);
        out = out && compare(1.5073, intersect.y(), 1E-4);
        out = out && compare(1.525, intersect.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobb-oobb resolution intersection validation");
        }

        // Resolve the intersection by moving b1
        b1.set_position(b1.get_center() + offset);

        // Test no intersection of oobbs and intersection point
        out = out && !min::intersect<double, min::vec3>(b1, b2);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobb-oobb resolution validation");
        }

        // Test b1 position
        out = out && compare(0.6184, b1.get_center().x(), 1E-4);
        out = out && compare(0.6184, b1.get_center().y(), 1E-4);
        out = out && compare(1.05, b1.get_center().z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobb-oobb resolution offset");
        }

        // Test magnitude between oobb centers
        double distance = (b1.get_center() - b2.get_center()).magnitude();
        out = out && compare(2.1725, distance, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 oobb-oobb resolution distance");
        }
    }

    // vec4 resolve
    {
        // Local variables
        min::oobbox<double, min::vec4> b1;
        min::oobbox<double, min::vec4> b2;
        min::vec4<double> intersect;
        min::vec4<double> offset;
        min::vec4<double> normal;

        // Test oobb-oobb intersection
        b1 = min::oobbox<double, min::vec4>(min::vec4<double>(0.1, 0.1, 0.1, 1.0), min::vec4<double>(2.0, 2.0, 2.0, 1.0));
        b2 = min::oobbox<double, min::vec4>(min::vec4<double>(1.0, 1.0, 1.0, 1.0), min::vec4<double>(3.0, 3.0, 3.0, 1.0));

        // Rotate the oobb's by 45 degrees around z axis
        min::vec3<double> z(0.0, 0.0, 1.0);
        b1.set_rotation(min::quat<double>(z, 45.0));
        b2.set_rotation(min::quat<double>(z, 45.0));

        // Test intersection of oobbs and intersection point
        out = out && min::intersect<double, min::vec4>(b1, b2, intersect);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobb-oobb resolution intersection 1");
        }

        // Test oobb-oobb intersection for validation
        out = out && compare(1.2928, intersect.x(), 1E-4);
        out = out && compare(1.2928, intersect.y(), 1E-4);
        out = out && compare(1.05, intersect.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobb-oobb resolution intersection validation 1");
        }

        // Test intersection of oobbs and intersection point (b2->b1)
        out = out && min::intersect<double, min::vec4>(b2, b1, intersect);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobb-oobb resolution intersection 2");
        }

        // Test oobb-oobb intersection for validation
        out = out && compare(1.7217, intersect.x(), 1E-4);
        out = out && compare(1.7217, intersect.y(), 1E-4);
        out = out && compare(2.0, intersect.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobb-oobb resolution intersection validation 2");
        }

        // Resolve intersecting oobbs
        offset = min::resolve<double, min::vec4>(b1, b2, normal, intersect, 1E-3);

        // Test normal vector
        out = out && compare(-0.7071, normal.x(), 1E-4);
        out = out && compare(-0.7071, normal.y(), 1E-4);
        out = out && compare(0.0, normal.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobb-oobb normal vector validation");
        }

        // Test oobb-oobb intersection for validation
        out = out && compare(1.5073, intersect.x(), 1E-4);
        out = out && compare(1.5073, intersect.y(), 1E-4);
        out = out && compare(1.525, intersect.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobb-oobb resolution intersection validation");
        }

        // Resolve the intersection by moving b1
        b1.set_position(b1.get_center() + offset);

        // Test no intersection of oobbs and intersection point
        out = out && !min::intersect<double, min::vec4>(b1, b2);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobb-oobb resolution validation");
        }

        // Test b1 position
        out = out && compare(0.6184, b1.get_center().x(), 1E-4);
        out = out && compare(0.6184, b1.get_center().y(), 1E-4);
        out = out && compare(1.05, b1.get_center().z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobb-oobb resolution offset");
        }

        // Test magnitude between oobb centers
        double distance = (b1.get_center() - b2.get_center()).magnitude();
        out = out && compare(2.1725, distance, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 oobb-oobb resolution distance");
        }
    }

    return out;
}

#endif
