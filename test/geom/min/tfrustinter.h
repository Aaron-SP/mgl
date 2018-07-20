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
#ifndef _MGL_TESTFRUSTUMINTERSECT_MGL_
#define _MGL_TESTFRUSTUMINTERSECT_MGL_

#include <min/aabbox.h>
#include <min/frustum.h>
#include <min/intersect.h>
#include <min/mat4.h>
#include <min/sphere.h>
#include <min/test.h>
#include <stdexcept>

bool test_frustum_intersect()
{
    bool out = true;

    // Local variables
    min::frustum<double> f;
    min::vec3<double> eye;
    min::vec3<double> look;
    min::vec3<double> forward;
    min::vec3<double> right;
    min::vec3<double> up = min::vec3<double>::up();
    min::vec3<double> center;
    min::mat4<double> proj;
    min::mat4<double> view;
    min::vec3<double> a;
    min::vec3<double> b;
    min::vec3<double> c;
    std::vector<min::vec3<double>> v;
    min::sphere<double, min::vec3> s;
    min::aabbox<double, min::vec3> abox;

    // Test sphere intersects
    f = min::frustum<double>(1.33, 45.0, 0.1, 5);
    eye = min::vec3<double>(0.0, 0.0, 0.0);
    look = min::vec3<double>(0.0, 0.0, 5.0);
    forward = (look - eye).normalize();
    proj = f.orthographic();
    view = f.look_at(eye, forward, right, up, center);
    a = min::vec3<double>(-0.550, 0.0, 1.0);
    b = min::vec3<double>(-1.0, 0.0, 1.0);
    c = min::vec3<double>(-2.0, 0.0, 1.0);
    v.push_back(a);
    v.push_back(b);
    v.push_back(c);
    s = min::sphere<double, min::vec3>(v);
    out = out && intersect(f, s);
    if (!out)
    {
        throw std::runtime_error("Failed frustum sphere intersection");
    }

    // Test sphere no intersect closest point on sphere to frustum center
    // Sphere is parallel to frustum center so the closest point should be 'a'
    a = min::vec3<double>(-1.41, 0.0, 2.55);
    b = min::vec3<double>(-1.42, 0.0, 2.55);
    c = min::vec3<double>(-1.43, 0.0, 2.55);
    v.clear();
    v.push_back(a);
    v.push_back(b);
    v.push_back(c);
    s = min::sphere<double, min::vec3>(v);
    out = out && !intersect(f, s);
    if (!out)
    {
        throw std::runtime_error("Failed frustum no sphere intersection");
    }
    // Test aabbox intersect
    a = min::vec3<double>(-0.550, 0.0, 1.0);
    b = min::vec3<double>(-1.0, 0.0, 1.0);
    c = min::vec3<double>(-2.0, 0.0, 1.0);
    v.clear();
    v.push_back(a);
    v.push_back(b);
    v.push_back(c);
    abox = min::aabbox<double, min::vec3>(v);
    out = out && intersect(f, abox);
    if (!out)
    {
        throw std::runtime_error("Failed frustum aabbox intersection");
    }

    // Test aabbox no intersect
    a = min::vec3<double>(-1.41, 0.0, 2.55);
    b = min::vec3<double>(-1.42, 0.0, 2.55);
    c = min::vec3<double>(-1.43, 0.0, 2.55);
    v.clear();
    v.push_back(a);
    v.push_back(b);
    v.push_back(c);
    abox = min::aabbox<double, min::vec3>(v);
    out = out && !intersect(f, abox);
    if (!out)
    {
        throw std::runtime_error("Failed frustum no aabbox intersection");
    }

    return out;
}

#endif
