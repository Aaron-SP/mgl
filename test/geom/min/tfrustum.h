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
#ifndef __TESTFRUSTUM__
#define __TESTFRUSTUM__

#include <min/frustum.h>
#include <min/mat4.h>
#include <min/test.h>
#include <stdexcept>

bool test_frustum()
{
    bool out = true;

    // Local variables
    min::frustum<double> f;
    min::vec3<double> p;
    min::vec3<double> eye;
    min::vec3<double> look;
    min::vec3<double> forward;
    min::vec3<double> up = min::vec3<double>::up();
    min::mat4<double> proj;
    min::mat4<double> view;

    // Test point inside
    f = min::frustum<double>(1.33, 45.0, 0.1, 5);
    p = min::vec3<double>(0.0, 0.0, 1.0);
    eye = min::vec3<double>(0.0, 0.0, 0.0);
    look = min::vec3<double>(0.0, 0.0, 5.0);
    forward = (look - eye).normalize();
    proj = f.orthographic();
    view = f.look_at(eye, forward, up);
    out = out && f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum z-axis point inside");
    }

    // Test point outside behind
    p = min::vec3<double>(0.0, 0.0, -1.0);
    out = out && !f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum z-axis point outside behind");
    }

    // Test point inside left
    p = min::vec3<double>(-0.550, 0.0, 1.0);
    out = out && f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum z-axis point inside left");
    }

    // Test point outside left
    p = min::vec3<double>(-0.551, 0.0, 1.0);
    out = out && !f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum z-axis point outside left");
    }

    // Test point inside right
    p = min::vec3<double>(0.550, 0.0, 1.0);
    out = out && f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum z-axis point inside right");
    }

    // Test point outside right
    p = min::vec3<double>(0.551, 0.0, 1.0);
    out = out && !f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum z-axis point outside right");
    }

    // Test point inside top
    p = min::vec3<double>(0.0, 0.4142, 1.0);
    out = out && f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum z-axis point inside top");
    }

    // Test point outside top
    p = min::vec3<double>(0.0, 0.4143, 1.0);
    out = out && !f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum z-axis point outside top");
    }

    // Test point inside bottom
    p = min::vec3<double>(0.0, -0.4142, 1.0);
    out = out && f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum z-axis point inside bottom");
    }

    // Test point outside bottom
    p = min::vec3<double>(0.0, -0.4143, 1.0);
    out = out && !f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum z-axis point outside bottom");
    }

    // Test frustum in x-axis
    p = min::vec3<double>(1.0, 0.0, 0.0);
    eye = min::vec3<double>(0.0, 0.0, 0.0);
    look = min::vec3<double>(5.0, 0.0, 0.0);
    forward = (look - eye).normalize();
    view = f.look_at(eye, forward, up);
    out = out && f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum x-axis point inside");
    }

    // Test point outside behind
    p = min::vec3<double>(-1.0, 0.0, 0.0);
    out = out && !f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum x-axis point outside behind");
    }

    // Test point inside left
    p = min::vec3<double>(1.0, 0.0, -0.550);
    out = out && f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum x-axis point inside left");
    }

    // Test point outside left
    p = min::vec3<double>(1.0, 0.0, -0.551);
    out = out && !f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum x-axis point outside left");
    }

    // Test point inside right
    p = min::vec3<double>(1.0, 0.0, 0.550);
    out = out && f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum x-axis point inside right");
    }

    // Test point outside right
    p = min::vec3<double>(1.0, 0.0, 0.551);
    out = out && !f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum x-axis point outside right");
    }

    // Test point inside top
    p = min::vec3<double>(1.0, 0.4142, 0.0);
    out = out && f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum x-axis point inside top");
    }

    // Test point outside top
    p = min::vec3<double>(1.0, 0.4143, 0.0);
    out = out && !f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum x-axis point outside top");
    }

    // Test point inside bottom
    p = min::vec3<double>(1.0, -0.4142, 0.0);
    out = out && f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum x-axis point inside bottom");
    }

    // Test point outside bottom
    p = min::vec3<double>(1.0, -0.4143, 0.0);
    out = out && !f.point_inside(p);
    if (!out)
    {
        throw std::runtime_error("Failed frustum point outside bottom");
    }

    // Test closest point on frustum bottom
    p = min::vec3<double>(1.0, -0.4143, 0.0);
    p = f.closest_point(p);
    out = out && compare(1.0, p.x(), 1E-4);
    out = out && compare(-0.4142, p.y(), 1E-4);
    out = out && compare(0.0, p.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed frustum closest point");
    }

    // Test closest point on frustum top
    p = min::vec3<double>(1.0, 0.4143, 0.0);
    p = f.closest_point(p);
    out = out && compare(1.0, p.x(), 1E-4);
    out = out && compare(0.4142, p.y(), 1E-4);
    out = out && compare(0.0, p.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed frustum closest point");
    }

    return out;
}

#endif
