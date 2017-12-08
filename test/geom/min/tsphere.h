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
#ifndef __TESTSPHERE__
#define __TESTSPHERE__

#include <min/sphere.h>
#include <min/test.h>
#include <min/vec2.h>
#include <min/vec3.h>
#include <min/vec4.h>
#include <stdexcept>
#include <vector>

template <typename T, template <typename> class vec>
bool points_inside(const min::sphere<T, vec> &s, const std::vector<vec<T>> &verts)
{
    bool out = true;
    for (const auto &v : verts)
    {
        out = out && s.point_inside(v);
    }

    return out;
}

template bool points_inside<double, min::vec2>(const min::sphere<double, min::vec2> &, const std::vector<min::vec2<double>> &);
template bool points_inside<double, min::vec3>(const min::sphere<double, min::vec3> &, const std::vector<min::vec3<double>> &);
template bool points_inside<double, min::vec4>(const min::sphere<double, min::vec4> &, const std::vector<min::vec4<double>> &);

bool test_sphere()
{
    bool out = true;

    // vec2 sphere = circle
    {
        // Local variables
        min::sphere<double, min::vec2> s;
        min::vec2<double> a;
        min::vec2<double> b;
        min::vec2<double> c;
        min::vec2<double> min;
        min::vec2<double> max;
        std::vector<min::vec2<double>> v;
        double r;

        // Test sphere constructor
        a = min::vec2<double>(1.0, 1.0);
        b = min::vec2<double>(2.0, 2.0);
        c = min::vec2<double>(3.0, 3.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        s = min::sphere<double, min::vec2>(v);
        out = out && points_inside(s, v);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere constructor");
        }

        // Test sphere get_center
        c = s.get_center();
        out = out && compare(2.0, c.x(), 1E-4);
        out = out && compare(2.0, c.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere get_center");
        }

        // Test sphere get_radius
        r = s.get_radius();
        out = out && compare(1.4142, r, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere get radius");
        }

        // Test add points_inside
        v.clear();
        a = min::vec2<double>(0.0, 0.0);
        b = min::vec2<double>(1.5, 1.5);
        c = min::vec2<double>(4.0, 4.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        s.add(v);
        out = out && points_inside(s, v);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere add points");
        }

        // Test add sphere get_center
        c = s.get_center();
        out = out && compare(2.0, c.x(), 1E-4);
        out = out && compare(2.0, c.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere add get_center");
        }

        // Test sphere get_radius
        r = s.get_radius();
        out = out && compare(2.8284, r, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere get add radius");
        }

        // Test closest_point
        b = min::vec2<double>(6.0, 6.0);
        c = s.closest_point(b);
        out = out && compare(4.0, c.x(), 1E-4);
        out = out && compare(4.0, c.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere closest_point");
        }

        // Test sphere min max
        min = min::vec2<double>(-1.0, -1.0);
        max = min::vec2<double>(1.0, 1.0);
        s = min::sphere<double, min::vec2>(min, max);

        // Test center
        c = s.get_center();
        out = out && compare(0.0, c.x(), 1E-4);
        out = out && compare(0.0, c.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere min max get_center");
        }

        // Test extent
        c = s.get_extent();
        out = out && compare(2.0, c.x(), 1E-4);
        out = out && compare(2.0, c.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere min max get_extent");
        }

        // Test min calculation
        min = s.get_min();
        out = out && compare(-1.0, min.x(), 1E-4);
        out = out && compare(-1.0, min.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere get_min");
        }

        // Test max calculation
        max = s.get_max();
        out = out && compare(1.0, max.x(), 1E-4);
        out = out && compare(1.0, max.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere get_max");
        }

        // Test square_size()
        r = s.square_size();
        out = out && compare(8.0, r, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere square_size");
        }

        // Test normal
        a = min::vec2<double>(-2.0, -2.0);
        b = min::vec2<double>(2.0, 2.0);
        c = min::vec2<double>(-1.0, 0.0);
        s = min::sphere<double, min::vec2>(a, b);
        double length;
        min::vec2<double> normal = s.normal(c, length, 1E-3);
        out = out && compare(1.0, length, 1E-4);
        out = out && compare(-1.0, normal.x(), 1E-4);
        out = out && compare(0.0, normal.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere normal");
        }

        // Test normal inside corner
        c = min::vec2<double>(-1.0, -1.0);
        normal = s.normal(c, length, 1E-3);
        out = out && compare(1.4142, length, 1E-4);
        out = out && compare(-0.7071, normal.x(), 1E-4);
        out = out && compare(-0.7071, normal.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere normal inside corner");
        }

        // Test normal corner
        c = min::vec2<double>(-2.0, -2.0);
        normal = s.normal(c, length, 1E-3);
        out = out && compare(2.82843, length, 1E-4);
        out = out && compare(-0.7071, normal.x(), 1E-4);
        out = out && compare(-0.7071, normal.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere normal corner");
        }

        // Test overlap
        c = min::vec2<double>(0.0, 0.0);
        normal = s.normal(c, length, 1E-3);
        out = out && compare(0.0, length, 1E-4);
        out = out && compare(0.0, normal.x(), 1E-4);
        out = out && compare(1.0, normal.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere normal overlap");
        }
    }

    // vec3 sphere
    {
        // Local variables
        min::sphere<double, min::vec3> s;
        min::vec3<double> a;
        min::vec3<double> b;
        min::vec3<double> c;
        min::vec3<double> min;
        min::vec3<double> max;
        std::vector<min::vec3<double>> v;
        double r;

        // Test sphere constructor
        a = min::vec3<double>(1.0, 1.0, 1.0);
        b = min::vec3<double>(2.0, 2.0, 2.0);
        c = min::vec3<double>(3.0, 3.0, 3.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        s = min::sphere<double, min::vec3>(v);
        out = out && points_inside(s, v);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere constructor");
        }

        // Test sphere get_center
        c = s.get_center();
        out = out && compare(2.0, c.x(), 1E-4);
        out = out && compare(2.0, c.y(), 1E-4);
        out = out && compare(2.0, c.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere get_center");
        }

        // Test sphere get_radius
        r = s.get_radius();
        out = out && compare(1.732, r, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere get radius");
        }

        // Test add points_inside
        v.clear();
        a = min::vec3<double>(0.0, 0.0, 0.0);
        b = min::vec3<double>(1.5, 1.5, 1.5);
        c = min::vec3<double>(4.0, 4.0, 4.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        s.add(v);
        out = out && points_inside(s, v);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere add points");
        }

        // Test add sphere get_center
        c = s.get_center();
        out = out && compare(2.0, c.x(), 1E-4);
        out = out && compare(2.0, c.y(), 1E-4);
        out = out && compare(2.0, c.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere add get_center");
        }

        // Test sphere get_radius
        r = s.get_radius();
        out = out && compare(3.4641, r, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere get add radius");
        }

        // Test closest_point
        b = min::vec3<double>(6.0, 6.0, 6.0);
        c = s.closest_point(b);
        out = out && compare(4.0, c.x(), 1E-4);
        out = out && compare(4.0, c.y(), 1E-4);
        out = out && compare(4.0, c.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere closest_point");
        }

        // Test sphere min max
        min = min::vec3<double>(-1.0, -1.0, -1.0);
        max = min::vec3<double>(1.0, 1.0, 1.0);
        s = min::sphere<double, min::vec3>(min, max);

        // Test center
        c = s.get_center();
        out = out && compare(0.0, c.x(), 1E-4);
        out = out && compare(0.0, c.y(), 1E-4);
        out = out && compare(0.0, c.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere min max get_center");
        }

        // Test extent
        c = s.get_extent();
        out = out && compare(2.0, c.x(), 1E-4);
        out = out && compare(2.0, c.y(), 1E-4);
        out = out && compare(2.0, c.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere min max get_extent");
        }

        // Test min calculation
        min = s.get_min();
        out = out && compare(-1.0, min.x(), 1E-4);
        out = out && compare(-1.0, min.y(), 1E-4);
        out = out && compare(-1.0, min.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere get_min");
        }

        // Test max calculation
        max = s.get_max();
        out = out && compare(1.0, max.x(), 1E-4);
        out = out && compare(1.0, max.y(), 1E-4);
        out = out && compare(1.0, max.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere get_max");
        }

        // Test square_size()
        r = s.square_size();
        out = out && compare(12.0, r, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere square_size");
        }

        // Test normal
        a = min::vec3<double>(-2.0, -2.0, -2.0);
        b = min::vec3<double>(2.0, 2.0, 2.0);
        c = min::vec3<double>(-1.0, 0.0, 0.0);
        s = min::sphere<double, min::vec3>(a, b);
        double length;
        min::vec3<double> normal = s.normal(c, length, 1E-3);
        out = out && compare(1.0, length, 1E-4);
        out = out && compare(-1.0, normal.x(), 1E-4);
        out = out && compare(0.0, normal.y(), 1E-4);
        out = out && compare(0.0, normal.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere normal");
        }

        // Test normal inside corner
        c = min::vec3<double>(-1.0, -1.0, -1.0);
        normal = s.normal(c, length, 1E-3);
        out = out && compare(1.7320, length, 1E-4);
        out = out && compare(-0.5773, normal.x(), 1E-4);
        out = out && compare(-0.5773, normal.y(), 1E-4);
        out = out && compare(-0.5773, normal.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere normal inside corner");
        }

        // Test normal corner
        c = min::vec3<double>(-2.0, -2.0, -2.0);
        normal = s.normal(c, length, 1E-3);
        out = out && compare(3.4641, length, 1E-4);
        out = out && compare(-0.5773, normal.x(), 1E-4);
        out = out && compare(-0.5773, normal.y(), 1E-4);
        out = out && compare(-0.5773, normal.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere normal corner");
        }

        // Test overlap
        c = min::vec3<double>(0.0, 0.0, 0.0);
        normal = s.normal(c, length, 1E-3);
        out = out && compare(0.0, length, 1E-4);
        out = out && compare(0.0, normal.x(), 1E-4);
        out = out && compare(1.0, normal.y(), 1E-4);
        out = out && compare(0.0, normal.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere normal overlap");
        }
    }

    // vec4 sphere
    {
        // Local variables
        min::sphere<double, min::vec4> s;
        min::vec4<double> a;
        min::vec4<double> b;
        min::vec4<double> c;
        min::vec4<double> min;
        min::vec4<double> max;
        std::vector<min::vec4<double>> v;
        double r;

        // Test sphere constructor
        a = min::vec4<double>(1.0, 1.0, 1.0, 0.0);
        b = min::vec4<double>(2.0, 2.0, 2.0, 0.0);
        c = min::vec4<double>(3.0, 3.0, 3.0, 0.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        s = min::sphere<double, min::vec4>(v);
        out = out && points_inside(s, v);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere constructor");
        }

        // Test sphere get_center
        c = s.get_center();
        out = out && compare(2.0, c.x(), 1E-4);
        out = out && compare(2.0, c.y(), 1E-4);
        out = out && compare(2.0, c.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere get_center");
        }

        // Test sphere get_radius
        r = s.get_radius();
        out = out && compare(1.732, r, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere get radius");
        }

        // Test add points_inside
        v.clear();
        a = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
        b = min::vec4<double>(1.5, 1.5, 1.5, 0.0);
        c = min::vec4<double>(4.0, 4.0, 4.0, 0.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        s.add(v);
        out = out && points_inside(s, v);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere add points");
        }

        // Test add sphere get_center
        c = s.get_center();
        out = out && compare(2.0, c.x(), 1E-4);
        out = out && compare(2.0, c.y(), 1E-4);
        out = out && compare(2.0, c.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere add get_center");
        }

        // Test sphere get_radius
        r = s.get_radius();
        out = out && compare(3.4641, r, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere get add radius");
        }

        // Test closest_point
        b = min::vec4<double>(6.0, 6.0, 6.0, 0.0);
        c = s.closest_point(b);
        out = out && compare(4.0, c.x(), 1E-4);
        out = out && compare(4.0, c.y(), 1E-4);
        out = out && compare(4.0, c.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere closest_point");
        }

        // Test sphere min max
        min = min::vec4<double>(-1.0, -1.0, -1.0, 1.0);
        max = min::vec4<double>(1.0, 1.0, 1.0, 1.0);
        s = min::sphere<double, min::vec4>(min, max);

        // Test center
        c = s.get_center();
        out = out && compare(0.0, c.x(), 1E-4);
        out = out && compare(0.0, c.y(), 1E-4);
        out = out && compare(0.0, c.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere min max get_center");
        }

        // Test extent
        c = s.get_extent();
        out = out && compare(2.0, c.x(), 1E-4);
        out = out && compare(2.0, c.y(), 1E-4);
        out = out && compare(2.0, c.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere min max get_extent");
        }

        // Test min calculation
        min = s.get_min();
        out = out && compare(-1.0, min.x(), 1E-4);
        out = out && compare(-1.0, min.y(), 1E-4);
        out = out && compare(-1.0, min.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere get_min");
        }

        // Test max calculation
        max = s.get_max();
        out = out && compare(1.0, max.x(), 1E-4);
        out = out && compare(1.0, max.y(), 1E-4);
        out = out && compare(1.0, max.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere get_max");
        }

        // Test square_size()
        r = s.square_size();
        out = out && compare(12.0, r, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere square_size");
        }

        // Test normal
        a = min::vec4<double>(-2.0, -2.0, -2.0, 1.0);
        b = min::vec4<double>(2.0, 2.0, 2.0, 1.0);
        c = min::vec4<double>(-1.0, 0.0, 0.0, 1.0);
        s = min::sphere<double, min::vec4>(a, b);
        double length;
        min::vec4<double> normal = s.normal(c, length, 1E-3);
        out = out && compare(1.0, length, 1E-4);
        out = out && compare(-1.0, normal.x(), 1E-4);
        out = out && compare(0.0, normal.y(), 1E-4);
        out = out && compare(0.0, normal.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere normal");
        }

        // Test normal inside corner
        c = min::vec4<double>(-1.0, -1.0, -1.0, 1.0);
        normal = s.normal(c, length, 1E-3);
        out = out && compare(1.7320, length, 1E-4);
        out = out && compare(-0.5773, normal.x(), 1E-4);
        out = out && compare(-0.5773, normal.y(), 1E-4);
        out = out && compare(-0.5773, normal.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere normal inside corner");
        }

        // Test normal corner
        c = min::vec4<double>(-2.0, -2.0, -2.0, 1.0);
        normal = s.normal(c, length, 1E-3);
        out = out && compare(3.4641, length, 1E-4);
        out = out && compare(-0.5773, normal.x(), 1E-4);
        out = out && compare(-0.5773, normal.y(), 1E-4);
        out = out && compare(-0.5773, normal.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere normal corner");
        }

        // Test overlap
        c = min::vec4<double>(0.0, 0.0, 0.0, 1.0);
        normal = s.normal(c, length, 1E-3);
        out = out && compare(0.0, length, 1E-4);
        out = out && compare(0.0, normal.x(), 1E-4);
        out = out && compare(1.0, normal.y(), 1E-4);
        out = out && compare(0.0, normal.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere normal overlap");
        }
    }
    return out;
}
#endif
