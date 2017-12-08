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
#ifndef __TESTPLANE__
#define __TESTPLANE__

#include <min/plane.h>
#include <min/test.h>
#include <min/vec2.h>
#include <min/vec3.h>
#include <min/vec4.h>
#include <stdexcept>

bool test_plane()
{
    bool out = true;

    // vec2 plane
    {
        // Local variables
        min::plane<double, min::vec2> p;
        min::vec2<double> a;
        min::vec2<double> b;
        min::vec2<double> n;
        double d;

        // Test plane normal vector on x(y=0) plane
        a = min::vec2<double>(2.0, 0.0);
        b = min::vec2<double>(1.0, 0.0);
        p = min::plane<double, min::vec2>(a, b);
        n = p.get_normal();
        out = out && compare(0.0, n.x(), 1E-4);
        out = out && compare(1.0, n.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 plane x(y=0) constructor");
        }

        // Test plane point on plane zero distance
        a = min::vec2<double>(-5.0, 0.0);
        d = p.get_distance(a);
        out = out && compare(0.0, d, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 plane x(y=0) on plane");
        }

        // Test distance above plane positive
        a = min::vec2<double>(0.0, 5.0);
        d = p.get_distance(a);
        out = out && compare(5.0, d, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 plane x(y=0) above plane");
        }

        // Test plane closest point
        a = min::vec2<double>(10.0, 1.0);
        a = p.get_closest_point(a);
        out = out && compare(10.0, a.x(), 1E-4);
        out = out && compare(0.0, a.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 plane x(y=0) closest point on plane");
        }

        // Test plane normal vector on x=y plane
        a = min::vec2<double>(2.0, 2.0);
        b = min::vec2<double>(1.0, 1.0);
        p = min::plane<double, min::vec2>(a, b);
        n = p.get_normal();
        out = out && compare(-0.7071, n.x(), 1E-4);
        out = out && compare(0.7071, n.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 plane x=y constructor");
        }

        // Test plane point on plane zero distance
        a = min::vec2<double>(-5.0, -5.0);
        d = p.get_distance(a);
        out = out && compare(0.0, d, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 plane x=y on plane");
        }

        // Test plane closest point
        a = min::vec2<double>(0.0, 1.0);
        a = p.get_closest_point(a);
        out = out && compare(0.5, a.x(), 1E-4);
        out = out && compare(0.5, a.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 plane x=y closest point on plane");
        }
    }

    // vec3 plane
    {
        // Local variables
        min::plane<double, min::vec3> p;
        min::vec3<double> a;
        min::vec3<double> b;
        min::vec3<double> c;
        min::vec3<double> n;
        double d;

        // Test plane normal vector on xz plane
        a = min::vec3<double>(2.0, 0.0, 2.0);
        b = min::vec3<double>(1.0, 0.0, -1.0);
        c = min::vec3<double>(3.0, 0.0, 7.0);
        p = min::plane<double, min::vec3>(a, b, c);
        n = p.get_normal();
        out = out && compare(0.0, n.x(), 1E-4);
        out = out && compare(1.0, n.y(), 1E-4);
        out = out && compare(0.0, n.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 plane xz(y=0) constructor");
        }

        // Test plane point on plane zero distance
        a = min::vec3<double>(-5.0, 0.0, 0.0);
        d = p.get_distance(a);
        out = out && compare(0.0, d, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 plane xz(y=0) on plane");
        }

        // Test distance above plane positive
        a = min::vec3<double>(0.0, 5.0, 0.0);
        d = p.get_distance(a);
        out = out && compare(5.0, d, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 plane x(y=0) above plane");
        }

        // Test plane closest point
        a = min::vec3<double>(10.0, 1.0, 0.0);
        a = p.get_closest_point(a);
        out = out && compare(10.0, a.x(), 1E-4);
        out = out && compare(0.0, a.y(), 1E-4);
        out = out && compare(0.0, a.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 plane xz(y=0) closest point on plane");
        }

        // Test plane normal vector on x=y plane
        a = min::vec3<double>(2.0, 2.0, -1.0);
        b = min::vec3<double>(3.0, 3.0, -4.0);
        c = min::vec3<double>(1.0, 1.0, -2.0);
        p = min::plane<double, min::vec3>(a, b, c);
        n = p.get_normal();
        out = out && compare(-0.7071, n.x(), 1E-4);
        out = out && compare(0.7071, n.y(), 1E-4);
        out = out && compare(0.0000, n.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 plane x=y constructor");
        }

        // Test plane point on plane zero distance
        a = min::vec3<double>(-5.0, -5.0, 0.0);
        d = p.get_distance(a);
        out = out && compare(0.0, d, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 plane x=y on plane");
        }

        // Test plane closest point
        a = min::vec3<double>(0.0, 1.0, 0.0);
        a = p.get_closest_point(a);
        out = out && compare(0.5, a.x(), 1E-4);
        out = out && compare(0.5, a.y(), 1E-4);
        out = out && compare(0.0, a.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 plane x=y closest point on plane");
        }
    }

    // vec4 plane
    {
        // Local variables
        min::plane<double, min::vec4> p;
        min::vec4<double> a;
        min::vec4<double> b;
        min::vec4<double> c;
        min::vec4<double> n;
        double d;

        // Test plane normal vector on xz plane
        a = min::vec4<double>(2.0, 0.0, 2.0, 0.0);
        b = min::vec4<double>(1.0, 0.0, -1.0, 0.0);
        c = min::vec4<double>(3.0, 0.0, 7.0, 0.0);
        p = min::plane<double, min::vec4>(a, b, c);
        n = p.get_normal();
        out = out && compare(0.0, n.x(), 1E-4);
        out = out && compare(1.0, n.y(), 1E-4);
        out = out && compare(0.0, n.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 plane xz(y=0) constructor");
        }

        // Test plane point on plane zero distance
        a = min::vec4<double>(-5.0, 0.0, 0.0, 0.0);
        d = p.get_distance(a);
        out = out && compare(0.0, d, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 plane xz(y=0) on plane");
        }

        // Test distance above plane positive
        a = min::vec4<double>(0.0, 5.0, 0.0, 0.0);
        d = p.get_distance(a);
        out = out && compare(5.0, d, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 plane x(y=0) above plane");
        }

        // Test plane closest point
        a = min::vec4<double>(10.0, 1.0, 0.0, 0.0);
        a = p.get_closest_point(a);
        out = out && compare(10.0, a.x(), 1E-4);
        out = out && compare(0.0, a.y(), 1E-4);
        out = out && compare(0.0, a.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 plane xz(y=0) closest point on plane");
        }

        // Test plane normal vector on x=y plane
        a = min::vec4<double>(1.0, 1.0, -1.0, 0.0);
        b = min::vec4<double>(2.0, 2.0, -2.0, 0.0);
        c = min::vec4<double>(3.0, 3.0, -4.0, 0.0);
        p = min::plane<double, min::vec4>(a, b, c);
        n = p.get_normal();
        out = out && compare(-0.7071, n.x(), 1E-4);
        out = out && compare(0.7071, n.y(), 1E-4);
        out = out && compare(0.0000, n.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 plane x=y constructor");
        }

        // Test plane point on plane zero distance
        a = min::vec4<double>(-5.0, -5.0, 0.0, 0.0);
        d = p.get_distance(a);
        out = out && compare(0.0, d, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 plane x=y on plane");
        }

        // Test plane closest point
        a = min::vec4<double>(0.0, 1.0, 0.0, 0.0);
        a = p.get_closest_point(a);
        out = out && compare(0.5, a.x(), 1E-4);
        out = out && compare(0.5, a.y(), 1E-4);
        out = out && compare(0.0, a.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 plane x=y closest point on plane");
        }
    }
    return out;
}
#endif
