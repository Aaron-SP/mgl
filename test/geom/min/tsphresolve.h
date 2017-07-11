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
#ifndef __TESTSPHERERESOLVE__
#define __TESTSPHERERESOLVE__

#include <min/intersect.h>
#include <min/sphere.h>
#include <min/test.h>
#include <stdexcept>

bool test_sphere_resolve()
{
    bool out = true;

    // vec2 resolve
    {
        // Local variables
        min::sphere<double, min::vec2> s1;
        min::sphere<double, min::vec2> s2;
        min::vec2<double> intersect;
        min::vec2<double> offset;
        min::vec2<double> normal;

        // Test sphere-sphere intersection
        s1 = min::sphere<double, min::vec2>(min::vec2<double>(1.0, 1.0), 1.0);
        s2 = min::sphere<double, min::vec2>(min::vec2<double>(2.0, 2.0), 1.0);

        // Test intersection of spheres and intersection point
        out = out && min::intersect<double, min::vec2>(s1, s2, intersect);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere-sphere resolution intersection");
        }

        // Test sphere-sphere intersection for validation
        out = out && compare(1.2928, intersect.x(), 1E-4);
        out = out && compare(1.2928, intersect.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere-sphere resolution intersection validation");
        }

        // Resolve intersecting spheres
        offset = min::resolve<double, min::vec2>(s1, s2, normal, intersect, 1E-3);

        // Test sphere-sphere intersection for validation
        out = out && compare(1.2928, intersect.x(), 1E-4);
        out = out && compare(1.2928, intersect.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere-sphere resolution intersection validation");
        }

        // Resolve the intersection by moving s1
        s1.set_position(s1.get_center() + offset);

        // Test no intersection of spheres and intersection point
        out = out && !min::intersect<double, min::vec2>(s1, s2);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere-sphere resolution validation");
        }

        // Test s1 position
        out = out && compare(0.5850, s1.get_center().x(), 1E-4);
        out = out && compare(0.5850, s1.get_center().y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere-sphere resolution offset");
        }

        // Test magnitude between sphere centers
        double distance = (s1.get_center() - s2.get_center()).magnitude();
        out = out && compare(2.001, distance, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere-sphere resolution distance");
        }
    }

    // vec3 resolve
    {
        // Local variables
        min::sphere<double, min::vec3> s1;
        min::sphere<double, min::vec3> s2;
        min::vec3<double> intersect;
        min::vec3<double> offset;
        min::vec3<double> normal;

        // Test sphere-sphere intersection
        s1 = min::sphere<double, min::vec3>(min::vec3<double>(1.0, 1.0, 1.0), 1.0);
        s2 = min::sphere<double, min::vec3>(min::vec3<double>(2.0, 2.0, 2.0), 1.0);

        // Test intersection of spheres and intersection point
        out = out && min::intersect<double, min::vec3>(s1, s2, intersect);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere-sphere resolution intersection");
        }

        // Test sphere-sphere intersection for validation
        out = out && compare(1.42265, intersect.x(), 1E-4);
        out = out && compare(1.42265, intersect.y(), 1E-4);
        out = out && compare(1.42265, intersect.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere-sphere resolution intersection validation");
        }

        // Resolve intersecting spheres
        offset = min::resolve<double, min::vec3>(s1, s2, normal, intersect, 1E-3);

        // Test sphere-sphere intersection for validation
        out = out && compare(1.42265, intersect.x(), 1E-4);
        out = out && compare(1.42265, intersect.y(), 1E-4);
        out = out && compare(1.42265, intersect.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere-sphere resolution intersection validation");
        }

        // Resolve the intersection by moving s1
        s1.set_position(s1.get_center() + offset);

        // Test no intersection of spheres and intersection point
        out = out && !min::intersect<double, min::vec3>(s1, s2);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere-sphere resolution validation");
        }

        // Test s1 position
        out = out && compare(0.844722, s1.get_center().x(), 1E-4);
        out = out && compare(0.844722, s1.get_center().y(), 1E-4);
        out = out && compare(0.844722, s1.get_center().z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere-sphere resolution offset");
        }

        // Test magnitude between sphere centers
        double distance = (s1.get_center() - s2.get_center()).magnitude();
        out = out && compare(2.001, distance, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere-sphere resolution distance");
        }
    }

    // vec4 resolve
    {
        // Local variables
        min::sphere<double, min::vec4> s1;
        min::sphere<double, min::vec4> s2;
        min::vec4<double> intersect;
        min::vec4<double> offset;
        min::vec4<double> normal;

        // Test sphere-sphere intersection
        s1 = min::sphere<double, min::vec4>(min::vec4<double>(1.0, 1.0, 1.0, 1.0), 1.0);
        s2 = min::sphere<double, min::vec4>(min::vec4<double>(2.0, 2.0, 2.0, 1.0), 1.0);

        // Test intersection of spheres and intersection point
        out = out && min::intersect<double, min::vec4>(s1, s2, intersect);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere-sphere resolution intersection");
        }

        // Test sphere-sphere intersection for validation
        out = out && compare(1.42265, intersect.x(), 1E-4);
        out = out && compare(1.42265, intersect.y(), 1E-4);
        out = out && compare(1.42265, intersect.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere-sphere resolution intersection validation");
        }

        // Resolve intersecting spheres
        offset = min::resolve<double, min::vec4>(s1, s2, normal, intersect, 1E-3);

        // Test sphere-sphere intersection for validation
        out = out && compare(1.42265, intersect.x(), 1E-4);
        out = out && compare(1.42265, intersect.y(), 1E-4);
        out = out && compare(1.42265, intersect.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere-sphere resolution intersection validation");
        }

        // Resolve the intersection by moving s1
        s1.set_position(s1.get_center() + offset);

        // Test no intersection of spheres and intersection point
        out = out && !min::intersect<double, min::vec4>(s1, s2);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere-sphere resolution validation");
        }

        // Test s1 position
        out = out && compare(0.844722, s1.get_center().x(), 1E-4);
        out = out && compare(0.844722, s1.get_center().y(), 1E-4);
        out = out && compare(0.844722, s1.get_center().z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere-sphere resolution offset");
        }

        // Test magnitude between sphere centers
        double distance = (s1.get_center() - s2.get_center()).magnitude();
        out = out && compare(2.001, distance, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere-sphere resolution distance");
        }
    }

    return out;
}

#endif
