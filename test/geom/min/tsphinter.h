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
#ifndef __TESTSPHEREINTERSECT__
#define __TESTSPHEREINTERSECT__

#include <min/intersect.h>
#include <min/test.h>
#include <stdexcept>

bool test_sphere_intersect()
{
    bool out = true;

    // vec2 intersect
    {
        // Local variables
        min::sphere<double, min::vec2> s;
        min::sphere<double, min::vec2> s2;
        min::aabbox<double, min::vec2> abox;
        min::vec2<double> a;
        min::vec2<double> b;
        min::vec2<double> c;
        std::vector<min::vec2<double>> v;
        min::ray<double, min::vec2> r;
        min::vec2<double> from;
        min::vec2<double> to;
        min::vec2<double> p;

        // Test sphere-ray intersection
        a = min::vec2<double>(-2.0, -2.0);
        b = min::vec2<double>(0.0, 0.0);
        c = min::vec2<double>(2.0, 2.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        s = min::sphere<double, min::vec2>(v);
        from = min::vec2<double>(3.0, 3.0);
        to = min::vec2<double>(0.0, 0.0);
        r = min::ray<double, min::vec2>(from, to);
        out = out && min::intersect(s, r, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere-ray intersection");
        }

        // Test intersection point
        out = out && compare(2.0, p.x(), 1E-4);
        out = out && compare(2.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere-ray intersection point");
        }

        // Test no intersection
        p = min::vec2<double>(0.0, 0.0);
        from = min::vec2<double>(3.0, 3.0);
        to = min::vec2<double>(4.0, 4.0);
        r = min::ray<double, min::vec2>(from, to);
        out = out && !min::intersect(s, r, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere-ray no intersection");
        }

        // Test no intersection point
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere-ray no intersection point");
        }

        // Test sphere-sphere intersection
        p = min::vec2<double>(0.0, 0.0);
        a = min::vec2<double>(1.99, 1.99);
        b = min::vec2<double>(7.0, 7.0);
        c = min::vec2<double>(10.0, 10.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        s2 = min::sphere<double, min::vec2>(v);
        out = out && min::intersect(s, s2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere-sphere intersection");
        }

        // Test sphere-sphere intersection always return sphere edge
        out = out && compare(1.99, p.x(), 1E-4);
        out = out && compare(1.99, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere-sphere intersection point");
        }

        // Test no sphere-sphere intersection
        p = min::vec2<double>(0.0, 0.0);
        a = min::vec2<double>(2.01, 2.01);
        b = min::vec2<double>(7.0, 7.0);
        c = min::vec2<double>(10.0, 10.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        s2 = min::sphere<double, min::vec2>(v);
        out = out && !min::intersect(s, s2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 no sphere-sphere intersection");
        }

        // Test sphere-box intersection
        p = min::vec2<double>(0.0, 0.0);
        a = min::vec2<double>(1.99, 1.99);
        b = min::vec2<double>(7.0, 7.0);
        c = min::vec2<double>(10.0, 10.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        abox = min::aabbox<double, min::vec2>(v);
        out = out && min::intersect(s, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere-box intersection");
        }

        // Test sphere-box intersection sphere center is not in box, so return box edge
        out = out && compare(1.99, p.x(), 1E-4);
        out = out && compare(1.99, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sphere-box intersection point");
        }

        // Test no sphere-box intersection
        p = min::vec2<double>(0.0, 0.0);
        a = min::vec2<double>(2.01, 2.01);
        b = min::vec2<double>(7.0, 7.0);
        c = min::vec2<double>(10.0, 10.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        abox = min::aabbox<double, min::vec2>(v);
        out = out && !min::intersect(s, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 no sphere-box intersection");
        }
    }

    // vec3 intersect
    {
        // Local variables
        min::sphere<double, min::vec3> s;
        min::sphere<double, min::vec3> s2;
        min::aabbox<double, min::vec3> abox;
        min::vec3<double> a;
        min::vec3<double> b;
        min::vec3<double> c;
        std::vector<min::vec3<double>> v;
        min::ray<double, min::vec3> r;
        min::vec3<double> from;
        min::vec3<double> to;
        min::vec3<double> p;

        // Test sphere-ray intersection
        a = min::vec3<double>(-2.0, -2.0, -2.0);
        b = min::vec3<double>(0.0, 0.0, 0.0);
        c = min::vec3<double>(2.0, 2.0, 2.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        s = min::sphere<double, min::vec3>(v);
        from = min::vec3<double>(3.0, 3.0, 3.0);
        to = min::vec3<double>(0.0, 0.0, 0.0);
        r = min::ray<double, min::vec3>(from, to);
        out = out && min::intersect(s, r, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere-ray intersection");
        }

        // Test intersection point
        out = out && compare(2.0, p.x(), 1E-4);
        out = out && compare(2.0, p.y(), 1E-4);
        out = out && compare(2.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere-ray intersection point");
        }

        // Test no intersection
        p = min::vec3<double>(0.0, 0.0, 0.0);
        from = min::vec3<double>(3.0, 3.0, 3.0);
        to = min::vec3<double>(4.0, 4.0, 4.0);
        r = min::ray<double, min::vec3>(from, to);
        out = out && !min::intersect(s, r, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere-ray no intersection");
        }

        // Test no intersection point
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        out = out && compare(0.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere-ray no intersection point");
        }

        // Test sphere-sphere intersection
        p = min::vec3<double>(0.0, 0.0, 0.0);
        a = min::vec3<double>(1.99, 1.99, 1.99);
        b = min::vec3<double>(7.0, 7.0, 7.0);
        c = min::vec3<double>(10.0, 10.0, 10.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        s2 = min::sphere<double, min::vec3>(v);
        out = out && min::intersect(s, s2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere-sphere intersection");
        }

        // Test sphere-sphere intersection always return sphere edge
        out = out && compare(1.99, p.x(), 1E-4);
        out = out && compare(1.99, p.y(), 1E-4);
        out = out && compare(1.99, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere-sphere intersection point");
        }

        // Test no sphere-sphere intersection
        p = min::vec3<double>(0.0, 0.0, 0.0);
        a = min::vec3<double>(2.01, 2.01, 2.01);
        b = min::vec3<double>(7.0, 7.0, 7.0);
        c = min::vec3<double>(10.0, 10.0, 10.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        s2 = min::sphere<double, min::vec3>(v);
        out = out && !min::intersect(s, s2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 no sphere-sphere intersection");
        }

        // Test sphere-box intersection
        p = min::vec3<double>(0.0, 0.0, 0.0);
        a = min::vec3<double>(1.99, 1.99, 1.99);
        b = min::vec3<double>(7.0, 7.0, 7.0);
        c = min::vec3<double>(10.0, 10.0, 10.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        abox = min::aabbox<double, min::vec3>(v);
        out = out && min::intersect(s, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere-box intersection");
        }

        // Test sphere-box intersection sphere center is not in box, so return box edge
        out = out && compare(1.99, p.x(), 1E-4);
        out = out && compare(1.99, p.y(), 1E-4);
        out = out && compare(1.99, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sphere-box intersection point");
        }

        // Test no sphere-box intersection
        p = min::vec3<double>(0.0, 0.0, 0.0);
        a = min::vec3<double>(2.01, 2.01, 2.01);
        b = min::vec3<double>(7.0, 7.0, 7.0);
        c = min::vec3<double>(10.0, 10.0, 10.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        abox = min::aabbox<double, min::vec3>(v);
        out = out && !min::intersect(s, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 no sphere-box intersection");
        }
    }

    // vec4 intersect
    {
        // Local variables
        min::sphere<double, min::vec4> s;
        min::sphere<double, min::vec4> s2;
        min::aabbox<double, min::vec4> abox;
        min::vec4<double> a;
        min::vec4<double> b;
        min::vec4<double> c;
        std::vector<min::vec4<double>> v;
        min::ray<double, min::vec4> r;
        min::vec4<double> from;
        min::vec4<double> to;
        min::vec4<double> p;

        // Test sphere-ray intersection
        a = min::vec4<double>(-2.0, -2.0, -2.0, 1.0);
        b = min::vec4<double>(0.0, 0.0, 0.0, 1.0);
        c = min::vec4<double>(2.0, 2.0, 2.0, 1.0);
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        s = min::sphere<double, min::vec4>(v);
        from = min::vec4<double>(3.0, 3.0, 3.0, 1.0);
        to = min::vec4<double>(0.0, 0.0, 0.0, 1.0);
        r = min::ray<double, min::vec4>(from, to);
        out = out && min::intersect(s, r, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere-ray intersection");
        }

        // Test intersection point
        out = out && compare(2.0, p.x(), 1E-4);
        out = out && compare(2.0, p.y(), 1E-4);
        out = out && compare(2.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere-ray intersection point");
        }

        // Test no intersection
        p = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
        from = min::vec4<double>(3.0, 3.0, 3.0, 1.0);
        to = min::vec4<double>(4.0, 4.0, 4.0, 1.0);
        r = min::ray<double, min::vec4>(from, to);
        out = out && !min::intersect(s, r, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere-ray no intersection");
        }

        // Test no intersection point
        out = out && compare(0.0, p.x(), 1E-4);
        out = out && compare(0.0, p.y(), 1E-4);
        out = out && compare(0.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere-ray no intersection point");
        }

        // Test sphere-sphere intersection
        p = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
        a = min::vec4<double>(1.99, 1.99, 1.99, 1.0);
        b = min::vec4<double>(7.0, 7.0, 7.0, 1.0);
        c = min::vec4<double>(10.0, 10.0, 10.0, 1.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        s2 = min::sphere<double, min::vec4>(v);
        out = out && min::intersect(s, s2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere-sphere intersection");
        }

        // Test sphere-sphere intersection always return sphere edge
        out = out && compare(1.99, p.x(), 1E-4);
        out = out && compare(1.99, p.y(), 1E-4);
        out = out && compare(1.99, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere-sphere intersection point");
        }

        // Test no sphere-sphere intersection
        p = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
        a = min::vec4<double>(2.01, 2.01, 2.01, 1.0);
        b = min::vec4<double>(7.0, 7.0, 7.0, 1.0);
        c = min::vec4<double>(10.0, 10.0, 10.0, 1.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        s2 = min::sphere<double, min::vec4>(v);
        out = out && !min::intersect(s, s2, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 no sphere-sphere intersection");
        }

        // Test sphere-box intersection
        p = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
        a = min::vec4<double>(1.99, 1.99, 1.99, 1.0);
        b = min::vec4<double>(7.0, 7.0, 7.0, 1.0);
        c = min::vec4<double>(10.0, 10.0, 10.0, 1.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        abox = min::aabbox<double, min::vec4>(v);
        out = out && min::intersect(s, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere-box intersection");
        }

        // Test sphere-box intersection sphere center is not in box, so return box edge
        out = out && compare(1.99, p.x(), 1E-4);
        out = out && compare(1.99, p.y(), 1E-4);
        out = out && compare(1.99, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sphere-box intersection point");
        }

        // Test no sphere-box intersection
        p = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
        a = min::vec4<double>(2.01, 2.01, 2.01, 1.0);
        b = min::vec4<double>(7.0, 7.0, 7.0, 1.0);
        c = min::vec4<double>(10.0, 10.0, 10.0, 1.0);
        v.clear();
        v.push_back(a);
        v.push_back(b);
        v.push_back(c);
        abox = min::aabbox<double, min::vec4>(v);
        out = out && !min::intersect(s, abox, p);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 no sphere-box intersection");
        }
    }

    return out;
}

#endif
