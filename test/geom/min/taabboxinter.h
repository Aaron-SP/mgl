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
#ifndef _MGL_TESTAABBOXINTERSECT_MGL_
#define _MGL_TESTAABBOXINTERSECT_MGL_

#include <min/intersect.h>
#include <min/test.h>
#include <stdexcept>

bool test_aabbox_intersect()
{
    bool out = true;

    // vec2 intersect
    {
        // Local variables
        min::aabbox<double, min::vec2> abox;
        min::aabbox<double, min::vec2> abox2;
        min::vec2<double> a;
        min::vec2<double> b;
        min::vec2<double> c;
        std::vector<min::vec2<double>> v;
        min::ray<double, min::vec2> r;
        min::vec2<double> from;
        min::vec2<double> to;
        min::vec2<double> p;

        // Test aabbox-ray intersection
        a = min::vec2<double>(-2.0, -2.0);
        b = min::vec2<double>(0.0, 0.0);
        c = min::vec2<double>(2.0, 2.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        abox = min::aabbox<double, min::vec2>(v);
        from = min::vec2<double>(3.0, 3.0);
        to = min::vec2<double>(0.0, 0.0);
        r = min::ray<double, min::vec2>(from, to);
        out = out && min::intersect(abox, r, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox-ray intersection");
        }

        // Test intersection point
        out = out && compare(2.0, p.x(), 1E-4);
        out = out && compare(2.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox-ray intersection point");
        }

        // Test no intersection
        p = min::vec2<double>(0.0, 0.0);
        from = min::vec2<double>(3.0, 3.0);
        to = min::vec2<double>(4.0, 4.0);
        r = min::ray<double, min::vec2>(from, to);
        out = out && !min::intersect(abox, r, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox-ray no intersection");
        }

        // Test no intersection point
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox-ray no intersection point");
        }

        // Test near miss
        // Test aabbox-ray intersection
        p = min::vec2<double>(0.0, 0.0);
        a = min::vec2<double>(-2.0, -2.0);
        b = min::vec2<double>(0.0, 0.0);
        c = min::vec2<double>(2.0, 2.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        abox = min::aabbox<double, min::vec2>(v);
        from = min::vec2<double>(-6.0, 2.0);
        to = min::vec2<double>(2.0, 3.0);
        r = min::ray<double, min::vec2>(from, to);
        out = out && !min::intersect(abox, r, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox-ray near miss intersection");
        }

        // Test no intersection near miss point
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox-ray no intersection point");
        }

        // Test aabbox-aabbox intersection
        p = min::vec2<double>(0.0, 0.0);
        a = min::vec2<double>(-1.0, -1.0);
        b = min::vec2<double>(2.0, 2.0);
        c = min::vec2<double>(3.0, 3.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        abox2 = min::aabbox<double, min::vec2>(v);
        out = out && min::intersect(abox, abox2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox-aabbox intersection");
        }

        // Test intersection point to be abox center
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 aabbox-aabbox intersection point");
        }
    }

    // vec3 intersect
    {
        // Local variables
        min::aabbox<double, min::vec3> abox;
        min::aabbox<double, min::vec3> abox2;
        min::vec3<double> a;
        min::vec3<double> b;
        min::vec3<double> c;
        std::vector<min::vec3<double>> v;
        min::ray<double, min::vec3> r;
        min::vec3<double> from;
        min::vec3<double> to;
        min::vec3<double> p;

        // Test aabbox-ray intersection
        a = min::vec3<double>(-2.0, -2.0, -2.0);
        b = min::vec3<double>(0.0, 0.0, 0.0);
        c = min::vec3<double>(2.0, 2.0, 2.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        abox = min::aabbox<double, min::vec3>(v);
        from = min::vec3<double>(3.0, 3.0, 3.0);
        to = min::vec3<double>(0.0, 0.0, 0.0);
        r = min::ray<double, min::vec3>(from, to);
        out = out && min::intersect(abox, r, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabbox-ray intersection");
        }

        // Test intersection point
        out = out && compare(2.0, p.x(), 1E-4);
        out = out && compare(2.0, p.y(), 1E-4);
        out = out && compare(2.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabbox-ray intersection point");
        }

        // Test no intersection
        p = min::vec3<double>(0.0, 0.0, 0.0);
        from = min::vec3<double>(3.0, 3.0, 3.0);
        to = min::vec3<double>(4.0, 4.0, 4.0);
        r = min::ray<double, min::vec3>(from, to);
        out = out && !min::intersect(abox, r, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabbox-ray no intersection");
        }

        // Test no intersection point
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        out = out && compare(0.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabbox-ray no intersection point");
        }

        // Test near miss
        // Test aabbox-ray intersection
        p = min::vec3<double>(0.0, 0.0, 0.0);
        a = min::vec3<double>(-2.0, -2.0, -2.0);
        b = min::vec3<double>(0.0, 0.0, 0.0);
        c = min::vec3<double>(2.0, 2.0, 2.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        abox = min::aabbox<double, min::vec3>(v);
        from = min::vec3<double>(-6.0, 2.0, 2.0);
        to = min::vec3<double>(2.0, 3.0, 2.0);
        r = min::ray<double, min::vec3>(from, to);
        out = out && !min::intersect(abox, r, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabbox-ray near miss intersection");
        }

        // Test no intersection near miss point
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        out = out && compare(0.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabbox-ray no intersection point");
        }

        // Test aabbox-aabbox intersection
        p = min::vec3<double>(0.0, 0.0, 0.0);
        a = min::vec3<double>(-1.0, -1.0, -1.0);
        b = min::vec3<double>(2.0, 2.0, 2.0);
        c = min::vec3<double>(3.0, 3.0, 3.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        abox2 = min::aabbox<double, min::vec3>(v);
        out = out && min::intersect(abox, abox2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabbox-aabbox intersection");
        }

        // Test intersection point to be abox center
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        out = out && compare(0.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 aabbox-aabbox intersection point");
        }
    }

    // vec4 intersect
    {
        // Local variables
        min::aabbox<double, min::vec4> abox;
        min::aabbox<double, min::vec4> abox2;
        min::vec4<double> a;
        min::vec4<double> b;
        min::vec4<double> c;
        std::vector<min::vec4<double>> v;
        min::ray<double, min::vec4> r;
        min::vec4<double> from;
        min::vec4<double> to;
        min::vec4<double> p;

        // Test aabbox-ray intersection
        a = min::vec4<double>(-2.0, -2.0, -2.0, 0.0);
        b = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
        c = min::vec4<double>(2.0, 2.0, 2.0, 0.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        abox = min::aabbox<double, min::vec4>(v);
        from = min::vec4<double>(3.0, 3.0, 3.0, 0.0);
        to = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
        r = min::ray<double, min::vec4>(from, to);
        out = out && min::intersect(abox, r, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabbox-ray intersection");
        }

        // Test intersection point
        out = out && compare(2.0, p.x(), 1E-4);
        out = out && compare(2.0, p.y(), 1E-4);
        out = out && compare(2.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabbox-ray intersection point");
        }

        // Test no intersection
        p = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
        from = min::vec4<double>(3.0, 3.0, 3.0, 0.0);
        to = min::vec4<double>(4.0, 4.0, 4.0, 0.0);
        r = min::ray<double, min::vec4>(from, to);
        out = out && !min::intersect(abox, r, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabbox-ray no intersection");
        }

        // Test no intersection point
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        out = out && compare(0.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabbox-ray no intersection point");
        }

        // Test near miss
        // Test aabbox-ray intersection
        p = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
        a = min::vec4<double>(-2.0, -2.0, -2.0, 0.0);
        b = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
        c = min::vec4<double>(2.0, 2.0, 2.0, 0.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        abox = min::aabbox<double, min::vec4>(v);
        from = min::vec4<double>(-6.0, 2.0, 2.0, 0.0);
        to = min::vec4<double>(2.0, 3.0, 2.0, 0.0);
        r = min::ray<double, min::vec4>(from, to);
        out = out && !min::intersect(abox, r, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabbox-ray near miss intersection");
        }

        // Test no intersection near miss point
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        out = out && compare(0.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabbox-ray no intersection point");
        }

        // Test aabbox-aabbox intersection
        p = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
        a = min::vec4<double>(-1.0, -1.0, -1.0, 0.0);
        b = min::vec4<double>(2.0, 2.0, 2.0, 0.0);
        c = min::vec4<double>(3.0, 3.0, 3.0, 0.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        abox2 = min::aabbox<double, min::vec4>(v);
        out = out && min::intersect(abox, abox2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabbox-aabbox intersection");
        }

        // Test intersection point to be abox center
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        out = out && compare(0.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 aabbox-aabbox intersection point");
        }
    }

    return out;
}

#endif
