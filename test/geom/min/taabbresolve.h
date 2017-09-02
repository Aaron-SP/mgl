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
#ifndef __TESTAABBRESOLVE__
#define __TESTAABBRESOLVE__

#include <min/aabbox.h>
#include <min/intersect.h>
#include <min/test.h>
#include <stdexcept>

bool test_aabb_resolve()
{
    bool out = true;

    // vec2 resolve
    {
        // Local variables
        min::aabbox<double, min::vec2> b1;
        min::aabbox<double, min::vec2> b2;
        min::vec2<double> intersect;
        min::vec2<double> offset;
        min::vec2<double> normal;

        // Test aabb-aabb intersection
        b1 = min::aabbox<double, min::vec2>(min::vec2<double>(0.1, 0.1), min::vec2<double>(2.0, 2.0));
        b2 = min::aabbox<double, min::vec2>(min::vec2<double>(1.0, 1.0), min::vec2<double>(3.0, 3.0));

        // Test intersection of aabbs and intersection point
        out = out && min::intersect<double, min::vec2>(b1, b2, intersect);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabb-aabb resolution intersection");
        }

        // Test aabb-aabb intersection for validation
        out = out && compare(1.05, intersect.x(), 1E-4);
        out = out && compare(1.05, intersect.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabb-aabb resolution intersection validation");
        }

        // Resolve intersecting aabbs
        offset = min::resolve<double, min::vec2>(b1, b2, normal, intersect, 1E-3);

        // Test normal vector
        out = out && compare(-1.0, normal.x(), 1E-4);
        out = out && compare(0.0, normal.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabb-aabb normal vector validation");
        }

        // Test aabb-aabb intersection for validation
        out = out && compare(1.525, intersect.x(), 1E-4);
        out = out && compare(1.525, intersect.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabb-aabb resolution intersection validation");
        }

        // Resolve the intersection by moving b1
        b1.set_position(b1.get_center() + offset);

        // Test no intersection of aabbs and intersection point
        out = out && !min::intersect<double, min::vec2>(b1, b2);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabb-aabb resolution validation");
        }

        // Test b1 position
        out = out && compare(0.048, b1.get_center().x(), 1E-4);
        out = out && compare(1.05, b1.get_center().y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabb-aabb resolution offset");
        }

        // Test magnitude between aabb centers
        double distance = (b1.get_center() - b2.get_center()).magnitude();
        out = out && compare(2.1709, distance, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabb-aabb resolution distance");
        }
    }

    // vec3 resolve
    {
        // Local variables
        min::aabbox<double, min::vec3> b1;
        min::aabbox<double, min::vec3> b2;
        min::vec3<double> intersect;
        min::vec3<double> offset;
        min::vec3<double> normal;

        // Test aabb-aabb intersection
        b1 = min::aabbox<double, min::vec3>(min::vec3<double>(0.1, 0.1, 0.1), min::vec3<double>(2.0, 2.0, 2.0));
        b2 = min::aabbox<double, min::vec3>(min::vec3<double>(1.0, 1.0, 1.0), min::vec3<double>(3.0, 3.0, 3.0));

        // Test intersection of aabbs and intersection point
        out = out && min::intersect<double, min::vec3>(b1, b2, intersect);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabb-aabb resolution intersection");
        }

        // Test aabb-aabb intersection for validation
        out = out && compare(1.05, intersect.x(), 1E-4);
        out = out && compare(1.05, intersect.y(), 1E-4);
        out = out && compare(1.05, intersect.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabb-aabb resolution intersection validation");
        }

        // Resolve intersecting aabbs
        offset = min::resolve<double, min::vec3>(b1, b2, normal, intersect, 1E-3);

        // Test normal vector
        out = out && compare(-1.0, normal.x(), 1E-4);
        out = out && compare(0.0, normal.y(), 1E-4);
        out = out && compare(0.0, normal.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabb-aabb normal vector validation");
        }

        // Test aabb-aabb intersection for validation
        out = out && compare(1.525, intersect.x(), 1E-4);
        out = out && compare(1.525, intersect.y(), 1E-4);
        out = out && compare(1.525, intersect.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabb-aabb resolution intersection validation");
        }

        // Resolve the intersection by moving b1
        b1.set_position(b1.get_center() + offset);

        // Test no intersection of aabbs and intersection point
        out = out && !min::intersect<double, min::vec3>(b1, b2);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabb-aabb resolution validation");
        }

        // Test b1 position
        out = out && compare(0.04615, b1.get_center().x(), 1E-4);
        out = out && compare(1.05, b1.get_center().y(), 1E-4);
        out = out && compare(1.05, b1.get_center().z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabb-aabb resolution offset");
        }

        // Test magnitude between aabb centers
        double distance = (b1.get_center() - b2.get_center()).magnitude();
        out = out && compare(2.3711, distance, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabb-aabb resolution distance");
        }
    }

    // vec4 resolve
    {
        // Local variables
        min::aabbox<double, min::vec4> b1;
        min::aabbox<double, min::vec4> b2;
        min::vec4<double> intersect;
        min::vec4<double> offset;
        min::vec4<double> normal;

        // Test aabb-aabb intersection
        b1 = min::aabbox<double, min::vec4>(min::vec4<double>(0.1, 0.1, 0.1, 1.0), min::vec4<double>(2.0, 2.0, 2.0, 1.0));
        b2 = min::aabbox<double, min::vec4>(min::vec4<double>(1.0, 1.0, 1.0, 1.0), min::vec4<double>(3.0, 3.0, 3.0, 1.0));

        // Test intersection of aabbs and intersection point
        out = out && min::intersect<double, min::vec4>(b1, b2, intersect);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabb-aabb resolution intersection");
        }

        // Test aabb-aabb intersection for validation
        out = out && compare(1.05, intersect.x(), 1E-4);
        out = out && compare(1.05, intersect.y(), 1E-4);
        out = out && compare(1.05, intersect.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabb-aabb resolution intersection validation");
        }

        // Resolve intersecting aabbs
        offset = min::resolve<double, min::vec4>(b1, b2, normal, intersect, 1E-3);

        // Test normal vector
        out = out && compare(-1.0, normal.x(), 1E-4);
        out = out && compare(0.0, normal.y(), 1E-4);
        out = out && compare(0.0, normal.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabb-aabb normal vector validation");
        }

        // Test aabb-aabb intersection for validation
        out = out && compare(1.525, intersect.x(), 1E-4);
        out = out && compare(1.525, intersect.y(), 1E-4);
        out = out && compare(1.525, intersect.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabb-aabb resolution intersection validation");
        }

        // Resolve the intersection by moving b1
        b1.set_position(b1.get_center() + offset);

        // Test no intersection of aabbs and intersection point
        out = out && !min::intersect<double, min::vec4>(b1, b2);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabb-aabb resolution validation");
        }

        // Test b1 position
        out = out && compare(0.04615, b1.get_center().x(), 1E-4);
        out = out && compare(1.05, b1.get_center().y(), 1E-4);
        out = out && compare(1.05, b1.get_center().z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabb-aabb resolution offset");
        }

        // Test magnitude between aabb centers
        double distance = (b1.get_center() - b2.get_center()).magnitude();
        out = out && compare(2.3711, distance, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabb-aabb resolution distance");
        }
    }

    return out;
}

#endif
