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
#ifndef __TESTCAMERA__
#define __TESTCAMERA__

#include <min/camera.h>
#include <min/frustum.h>
#include <min/mat4.h>
#include <min/test.h>
#include <stdexcept>

bool test_camera()
{
    bool out = true;

    // Local variables
    min::camera<double> c;
    min::vec3<double> p;
    min::vec4<double> ndc;
    min::mat4<double> mat;

    // Test set look at update frustum
    c = min::camera<double>();
    p = min::vec3<double>(0.0, 2.0, 0.0);
    c.set_look_at(p);

    // get_pv_matrix() updates the camera and frustum internals
    mat = c.get_pv_matrix();
    const min::frustum<double> &cf = c.get_frustum();
    p = cf.get_center();
    out = out && compare(0.0, p.x(), 1E-4);
    out = out && compare(100.05, p.y(), 1E-4);
    out = out && compare(0.0, p.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed camera frustum get_center");
    }

    // Set set position
    p = min::vec3<double>(0.0, 2.0, 0.0);
    c.set_position(p);
    mat = c.get_pv_matrix();
    p = cf.get_center();
    out = out && compare(0.0, p.x(), 1E-4);
    out = out && compare(102.05, p.y(), 1E-4);
    out = out && compare(0.0, p.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed camera set_position");
    }

    // Set lookat after position change
    p = min::vec3<double>(0.0, 2.0, -2.0);
    c.set_look_at(p);
    mat = c.get_pv_matrix();
    p = cf.get_center();
    out = out && compare(0.0, p.x(), 1E-4);
    out = out && compare(2.0, p.y(), 1E-4);
    out = out && compare(-100.05, p.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed camera frustum set lookat");
    }

    // Check the right vector
    p = cf.get_right();
    out = out && compare(-1.0, p.x(), 1E-4);
    out = out && compare(0.0, p.y(), 1E-4);
    out = out && compare(0.0, p.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed camera get_right");
    }

    // Check frustum sets and camera update
    min::frustum<double> &f = c.get_frustum();
    f.set_aspect_ratio(3.0, 2.0);
    f.set_fov(60.0);
    f.set_near(10.0);
    f.set_far(60.0);
    c.make_dirty();          // must call this to update camera
    mat = c.get_pv_matrix(); // must call this to update camera
    p = cf.get_center();
    out = out && compare(0.0, p.x(), 1E-4);
    out = out && compare(2.0, p.y(), 1E-4);
    out = out && compare(-35.0, p.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed camera update frustum view properties");
    }

    // Check orthographic NDC transformation
    ndc = mat * min::vec4<double>(0.0, 2.0, -2.0, 1.0);
    out = out && compare(0.0, ndc.x(), 1E-4);
    out = out && compare(0, ndc.y(), 1E-4);
    out = out && compare(1.32, ndc.z(), 1E-4);
    out = out && compare(1.0, ndc.w(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed camera orthographic ndc transformation behind");
    }

    // Check orthographic NDC transformation
    ndc = mat * min::vec4<double>(0.0, 2.0, -35.0, 1.0);
    out = out && compare(0.0, ndc.x(), 1E-4);
    out = out && compare(0, ndc.y(), 1E-4);
    out = out && compare(0.0, ndc.z(), 1E-4);
    out = out && compare(1.0, ndc.w(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed camera orthographic ndc transformation middle");
    }

    // Check orthographic NDC transformation
    ndc = mat * min::vec4<double>(0.0, 2.0, -61.0, 1.0);
    out = out && compare(0.0, ndc.x(), 1E-4);
    out = out && compare(0, ndc.y(), 1E-4);
    out = out && compare(-1.04, ndc.z(), 1E-4);
    out = out && compare(1.0, ndc.w(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed camera orthographic ndc transformation outside");
    }

    // Change camera projection matrix
    c.set_perspective();
    mat = c.get_pv_matrix();

    // Check perspective NDC transformation behind
    ndc = mat * min::vec4<double>(0.0, 2.0, -2.0, 1.0);
    out = out && compare(0.0, ndc.x(), 1E-4);
    out = out && compare(0, ndc.y(), 1E-4);
    out = out && compare(21.2, ndc.z(), 1E-4);
    out = out && compare(2, ndc.w(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed camera perspective ndc transformation behind");
    }

    // Check perspective NDC transformation behind
    ndc = mat * min::vec4<double>(0.0, 2.0, -35.0, 1.0);
    out = out && compare(0.0, ndc.x(), 1E-4);
    out = out && compare(0, ndc.y(), 1E-4);
    out = out && compare(-25, ndc.z(), 1E-4);
    out = out && compare(35, ndc.w(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed camera perspective ndc transformation inside");
    }

    // Check perspective NDC transformation behind
    ndc = mat * min::vec4<double>(0.0, 2.0, -61.0, 1.0);
    out = out && compare(0.0, ndc.x(), 1E-4);
    out = out && compare(0, ndc.y(), 1E-4);
    out = out && compare(-61.4, ndc.z(), 1E-4);
    out = out && compare(61, ndc.w(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed camera perspective ndc transformation outside");
    }

    // Test move fps camera +1 degree around right axis +1 degree around up axis
    // Transform (0.0, 2.0, -2.0) -> (-0.0349, 1.9651, -1.9993)
    // Set lookat after position change
    c.move_look_at(1.0, 1.0);
    mat = c.get_pv_matrix();
    p = c.get_look_at();
    out = out && compare(-0.0349, p.x(), 1E-4);
    out = out && compare(1.9651, p.y(), 1E-4);
    out = out && compare(-1.9993, p.z(), 1E-4);

    if (!out)
    {
        throw std::runtime_error("Failed camera move look at");
    }

    return out;
}

#endif
