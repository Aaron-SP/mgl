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
#ifndef __TESTMATRIX4__
#define __TESTMATRIX4__

#include <min/mat4.h>
#include <min/test.h>
#include <stdexcept>

bool test_mat4()
{
    bool out = true;

    // Local variables
    min::mat4<double> mat;
    min::vec3<double> v3;
    min::vec3<double> x;
    min::vec3<double> y;
    min::vec4<double> v4;
    min::quat<double> q;

    // Test identity matrix
    mat = min::mat4<double>();
    v4 = min::vec4<double>(1.0, 2.0, 2.0, 1.0);
    v4 = mat * v4;
    out = out && compare(1.0, v4.x(), 1E-4);
    out = out && compare(2.0, v4.y(), 1E-4);
    out = out && compare(2.0, v4.z(), 1E-4);
    out = out && compare(1.0, v4.w(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 identity matrix");
    }

    // Test set translation, *=
    v3 = min::vec3<double>(1.0, 2.0, 2.0);
    mat = min::mat4<double>(v3);
    mat *= min::mat4<double>().set_translation(v3);
    v4 = min::vec4<double>(1.0, 2.0, 2.0, 1.0);
    v4 = mat * v4;
    out = out && compare(3.0, v4.x(), 1E-4);
    out = out && compare(6.0, v4.y(), 1E-4);
    out = out && compare(6.0, v4.z(), 1E-4);
    out = out && compare(1.0, v4.w(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 set translation, *= matrix");
    }

    // Test translation, *= matrix
    v4 = min::vec4<double>(0.5773, 0.5773, 0.5773, 1.0);
    x = min::vec3<double>(0.4082, 0.8165, -0.4082);
    mat = min::mat4<double>();
    mat *= min::mat4<double>().set_translation(x);
    v4 = mat * v4;
    out = out && compare(0.9855, v4.x(), 1E-4);
    out = out && compare(1.3938, v4.y(), 1E-4);
    out = out && compare(0.1691, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 translation, *=");
    }

    // Test rotation quaternion x axis to y axis
    x = min::vec3<double>(1.0, 0.0, 0.0);
    y = min::vec3<double>(0.0, 1.0, 0.0);
    q = min::quat<double>(x, y);
    mat = min::mat4<double>(q);
    v4 = min::vec4<double>(1.0, 0.0, 0.0, 1.0);
    v4 = mat * v4;
    out = out && compare(0.0, v4.x(), 1E-4);
    out = out && compare(1.0, v4.y(), 1E-4);
    out = out && compare(0.0, v4.z(), 1E-4);
    out = out && compare(1.0, v4.w(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 axis rotation matrix");
    }

    // Test rotation quaternion (1.0, 1.0, 1.0) -> (-1.0, 1.0, -1.0)
    x = min::vec3<double>(1.0, 1.0, 1.0).normalize();
    y = min::vec3<double>(-1.0, 1.0, -1.0).normalize();
    q = min::quat<double>(x, y);
    mat = min::mat4<double>(q);
    v4 = min::vec4<double>(1.0, 1.0, 1.0, 1.0);
    v4 = mat * v4;
    out = out && compare(-1.0, v4.x(), 1E-4);
    out = out && compare(1.0, v4.y(), 1E-4);
    out = out && compare(-1.0, v4.z(), 1E-4);
    out = out && compare(1.0, v4.w(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 quat transform matrix 1");
    }

    // Test get rotation from mat4
    x = min::vec3<double>(1.0, 1.0, 1.0).normalize();
    y = min::vec3<double>(-1.0, 1.0, -1.0).normalize();
    q = min::quat<double>(x, y);
    mat = min::mat4<double>(q);
    q = mat.get_rotation();
    out = out && compare(0.5773, q.w(), 1E-4);
    out = out && compare(-0.5773, q.x(), 1E-4);
    out = out && compare(0.0, q.y(), 1E-4);
    out = out && compare(0.5773, q.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 get quat 1");
    }

    // Test rotation quaternion (1.0, 1.0, 1.0) -> (-1.0, -1.0, -1.0)
    x = min::vec3<double>(1.0, 1.0, 1.0).normalize();
    y = min::vec3<double>(-1.0, -1.0, -1.0).normalize();
    q = min::quat<double>(x, y);
    mat = min::mat4<double>(q);
    v4 = min::vec4<double>(1.0, 1.0, 1.0, 1.0);
    v4 = mat * v4;
    out = out && compare(-1.0, v4.x(), 1E-4);
    out = out && compare(-1.0, v4.y(), 1E-4);
    out = out && compare(-1.0, v4.z(), 1E-4);
    out = out && compare(1.0, v4.w(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 quat transform matrix 2");
    }

    // Test get rotation from mat4; There is a negative switch but the two quats are equivalent
    x = min::vec3<double>(1.0, 1.0, 1.0).normalize();
    y = min::vec3<double>(-1.0, -1.0, -1.0).normalize();
    q = min::quat<double>(x, y);
    mat = min::mat4<double>(q);
    q = mat.get_rotation();
    out = out && compare(0.0, q.w(), 1E-4);
    out = out && compare(-0.7071, q.x(), 1E-4);
    out = out && compare(0.0, q.y(), 1E-4);
    out = out && compare(0.7071, q.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 get quat 2");
    }

    // Test set rotation and get from mat4; There is a negative switch but the two quats are equivalent
    q = min::quat<double>(0.0, 0.7071, 0.0, -0.7071);
    mat = min::mat4<double>().set_rotation(q);
    q = mat.get_rotation();
    out = out && compare(0.0, q.w(), 1E-4);
    out = out && compare(-0.7071, q.x(), 1E-4);
    out = out && compare(0.0, q.y(), 1E-4);
    out = out && compare(0.7071, q.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 get quat 2");
    }

    // Test scale
    v3 = min::vec3<double>(1.0, 2.0, 2.0);
    mat = min::mat4<double>().set_scale(v3);
    v4 = min::vec4<double>(1.0, 2.0, 2.0, 1.0);
    v4 = mat * v4;
    out = out && compare(1.0, v4.x(), 1E-4);
    out = out && compare(4.0, v4.y(), 1E-4);
    out = out && compare(4.0, v4.z(), 1E-4);
    out = out && compare(1.0, v4.w(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 scale matrix");
    }

    // Test get scale
    v3 = min::vec3<double>(1.0, 2.0, 2.0);
    mat = min::mat4<double>().set_scale(v3);
    v3 = mat.get_scale();
    out = out && compare(1.0, v3.x(), 1E-4);
    out = out && compare(2.0, v3.y(), 1E-4);
    out = out && compare(2.0, v3.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 get scale matrix");
    }

    // Test transpose rotation quaternion (-1.0, -1.0, -1.0) -> (1.0, 1.0, 1.0)
    x = min::vec3<double>(1.0, 1.0, 1.0).normalize();
    y = min::vec3<double>(-1.0, 1.0, -1.0).normalize();
    q = min::quat<double>(x, y);
    mat = min::mat4<double>(q).transpose();
    v4 = min::vec4<double>(-1.0, 1.0, -1.0, 1.0);
    v4 = mat * v4;
    out = out && compare(1.0, v4.x(), 1E-4);
    out = out && compare(1.0, v4.y(), 1E-4);
    out = out && compare(1.0, v4.z(), 1E-4);
    out = out && compare(1.0, v4.w(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 quat transpose rotation");
    }

    // Test inverse matrix
    v3 = min::vec3<double>(3.0, 3.0, 3.0);
    mat = min::mat4<double>(v3);
    mat.invert();
    v4 = min::vec4<double>(1.0, 1.0, 1.0, 1.0);
    v4 = mat * v4;
    out = out && compare(-2.0, v4.x(), 1E-4);
    out = out && compare(-2.0, v4.y(), 1E-4);
    out = out && compare(-2.0, v4.z(), 1E-4);
    out = out && compare(1.0, v4.w(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 invert matrix");
    }

    return out;
}

#endif