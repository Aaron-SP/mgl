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
#ifndef __TESTMATRIX3__
#define __TESTMATRIX3__

#include <min/mat3.h>
#include <min/test.h>
#include <stdexcept>

bool test_mat3()
{
    bool out = true;

    // Local variables
    min::mat3<double> mat;
    min::vec2<double> v2;
    min::vec3<double> v3;
    min::vec3<double> x;
    min::vec3<double> y;
    min::vec3<double> z;
    min::quat<double> q;

    // Test identity matrix
    mat = min::mat3<double>();
    v3 = min::vec3<double>(1.0, 2.0, 1.0);
    v3 = mat * v3;
    out = out && compare(1.0, v3.x(), 1E-4);
    out = out && compare(2.0, v3.y(), 1E-4);
    out = out && compare(1.0, v3.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 identity matrix");
    }

    // Test translation vec2, *=
    mat = min::mat3<double>(min::vec2<double>(1.0, 2.0));
    mat *= min::mat3<double>().set_translation(min::vec2<double>(1.0, 2.0));
    v2 = mat.get_translation();
    out = out && compare(2.0, v2.x(), 1E-4);
    out = out && compare(4.0, v2.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 translate *= matrix");
    }

    // Test translation vec2, *
    mat = mat * min::mat3<double>().set_translation(min::vec2<double>(2.0, 3.0));
    v2 = mat.get_translation();
    out = out && compare(4.0, v2.x(), 1E-4);
    out = out && compare(7.0, v2.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 translate * matrix");
    }

    // Test translation vec3
    v3 = min::vec3<double>(1.0, 2.0, 1.0);
    mat = min::mat3<double>(min::vec2<double>(1.0, 2.0));
    v3 = mat * v3;
    out = out && compare(2.0, v3.x(), 1E-4);
    out = out && compare(4.0, v3.y(), 1E-4);
    out = out && compare(1.0, v3.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 translate vec3 matrix");
    }

    // Test identity rotation (1.0, 0.0) -> (0.0, 1.0)
    min::mat2<double> m = min::mat2<double>(90.0);
    mat = min::mat3<double>();
    mat *= min::mat3<double>(m);
    v3 = min::vec3<double>(1.0, 1.0, 1.0);
    v3 = mat * v3;
    out = out && compare(-1.0, v3.x(), 1E-4);
    out = out && compare(1.0, v3.y(), 1E-4);
    out = out && compare(1.0, v3.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 identity rotation vec3 matrix");
    }

    // Test rotation+translation constructor (1.0, 0.0) -> (0.0, 2.0)
    v2 = min::vec2<double>(1.0, 0.0);
    mat = min::mat3<double>(v2, 90.0);
    v3 = min::vec3<double>(1.0, 0.0, 1.0);
    v3 = mat * v3;
    out = out && compare(1.0, v3.x(), 1E-4);
    out = out && compare(1.0, v3.y(), 1E-4);
    out = out && compare(1.0, v3.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 translation/rotation constructor");
    }

    // Test rotation 90 degrees CCW on screen (1.0, 0.0) -> (0.0, 1.0)
    m = min::mat2<double>(90.0);
    mat = min::mat3<double>(m);
    v3 = min::vec3<double>(1.0, 0.0, 0.0);
    v3 = mat * v3;
    out = out && compare(0.0, v3.x(), 1E-4);
    out = out && compare(1.0, v3.y(), 1E-4);
    out = out && compare(0.0, v3.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 rotation vec3 matrix");
    }

    // Test invert (0.0, 1.0) -> (1.0, 0.0)
    mat.invert();
    v3 = mat * v3;
    out = out && compare(1.0, v3.x(), 1E-4);
    out = out && compare(0.0, v3.y(), 1E-4);
    out = out && compare(0.0, v3.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 inverse matrix");
    }

    // Test orthogonal invert (1.0, 0.0) -> (0.0, 1.0)
    m = mat.get_rotation();
    mat = min::mat3<double>().set_rotation(m);
    mat.transpose();
    v3 = mat * v3;
    out = out && compare(0.0, v3.x(), 1E-4);
    out = out && compare(1.0, v3.y(), 1E-4);
    out = out && compare(0.0, v3.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 transpose matrix");
    }

    // Test set scale
    mat = min::mat3<double>().set_scale(min::vec2<double>(2.0, 2.0));
    v3 = min::vec3<double>(1.0, 2.0, 1.0);
    v3 = mat * v3;
    out = out && compare(2.0, v3.x(), 1E-4);
    out = out && compare(4.0, v3.y(), 1E-4);
    out = out && compare(1.0, v3.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 set scale");
    }

    // Test get scale
    v2 = mat.get_scale();
    out = out && compare(2.0, v2.x(), 1E-4);
    out = out && compare(2.0, v2.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 get scale");
    }

    // Test rotation quaternion (1.0, 1.0, 1.0) -> (-1.0, 1.0, -1.0)
    v3 = min::vec3<double>(1.0, 1.0, 1.0);
    x = v3;
    x.normalize();
    y = min::vec3<double>(-1.0, 1.0, -1.0).normalize();
    q = min::quat<double>(x, y);
    mat = min::mat3<double>(q);
    v3 = mat * v3;
    out = out && compare(-1.0, v3.x(), 1E-4);
    out = out && compare(1.0, v3.y(), 1E-4);
    out = out && compare(-1.0, v3.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 quaternion transform 1");
    }

    // Test rotation quaternion (1.0, 1.0, 1.0) -> (-1.0, -1.0, -1.0)
    v3 = min::vec3<double>(1.0, 1.0, 1.0);
    x = v3;
    x.normalize();
    y = min::vec3<double>(-1.0, -1.0, -1.0).normalize();
    q = min::quat<double>(x, y);
    mat = min::mat3<double>(q);
    v3 = mat * v3;
    out = out && compare(-1.0, v3.x(), 1E-4);
    out = out && compare(-1.0, v3.y(), 1E-4);
    out = out && compare(-1.0, v3.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 quaternion transform 2");
    }

    // Test rotate y-axis -> z-axis; indicates positive rotation
    y = min::vec3<double>(0.0, 1.0, 0.0);
    mat = min::mat3<double>().set_rotation_x(90.0);
    z = mat * y;
    out = out && compare(0.0, z.x(), 1E-4);
    out = out && compare(0.0, z.y(), 1E-4);
    out = out && compare(1.0, z.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 x-axis rotation");
    }

    // Test rotate y-axis -> -z-axis; indicates negative rotation
    y = min::vec3<double>(0.0, 1.0, 0.0);
    mat = min::mat3<double>().set_rotation_x(-90.0);
    z = mat * y;
    out = out && compare(0.0, z.x(), 1E-4);
    out = out && compare(0.0, z.y(), 1E-4);
    out = out && compare(-1.0, z.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 negative x-axis rotation");
    }

    // Test rotate y-axis -> -z-axis
    x = min::vec3<double>(1.0, 0.0, 0.0);
    y = min::vec3<double>(0.0, 1.0, 0.0);
    q = min::quat<double>(x, -90.0);
    mat = min::mat3<double>(q);
    z = mat * y;
    out = out && compare(0.0, z.x(), 1E-4);
    out = out && compare(0.0, z.y(), 1E-4);
    out = out && compare(-1.0, z.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 negative x-axis quat rotation");
    }

    // Test rotate z-axis -> x-axis; indicates positive rotation
    z = min::vec3<double>(0.0, 0.0, 1.0);
    mat = min::mat3<double>().set_rotation_y(90.0);
    x = mat * z;
    out = out && compare(1.0, x.x(), 1E-4);
    out = out && compare(0.0, x.y(), 1E-4);
    out = out && compare(0.0, x.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 y-axis rotation");
    }

    // Test rotate z-axis -> -x-axis; indicates negative rotation
    z = min::vec3<double>(0.0, 0.0, 1.0);
    mat = min::mat3<double>().set_rotation_y(-90.0);
    x = mat * z;
    out = out && compare(-1.0, x.x(), 1E-4);
    out = out && compare(0.0, x.y(), 1E-4);
    out = out && compare(0.0, x.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 negative y-axis rotation");
    }

    // Test rotate z-axis -> -x-axis
    z = min::vec3<double>(0.0, 0.0, 1.0);
    y = min::vec3<double>(0.0, 1.0, 0.0);
    q = min::quat<double>(y, -90.0);
    mat = min::mat3<double>(q);
    x = mat * z;
    out = out && compare(-1.0, x.x(), 1E-4);
    out = out && compare(0.0, x.y(), 1E-4);
    out = out && compare(0.0, x.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 negative y-axis quat rotation");
    }

    // Test rotate x-axis -> y-axis; indicates positive rotation
    x = min::vec3<double>(1.0, 0.0, 0.0);
    mat = min::mat3<double>(90.0);
    y = mat * x;
    out = out && compare(0.0, y.x(), 1E-4);
    out = out && compare(1.0, y.y(), 1E-4);
    out = out && compare(0.0, y.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 z-axis rotation");
    }

    // Test rotate x-axis -> y-axis; indicates negative rotation
    x = min::vec3<double>(1.0, 0.0, 0.0);
    mat = min::mat3<double>(-90.0);
    y = mat * x;
    out = out && compare(0.0, y.x(), 1E-4);
    out = out && compare(-1.0, y.y(), 1E-4);
    out = out && compare(0.0, y.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 negative z-axis rotation");
    }

    // Test rotate x-axis -> -y-axis
    x = min::vec3<double>(1.0, 0.0, 0.0);
    z = min::vec3<double>(0.0, 0.0, 1.0);
    q = min::quat<double>(z, -90.0);
    mat = min::mat3<double>(q);
    y = mat * x;
    out = out && compare(0.0, y.x(), 1E-4);
    out = out && compare(-1.0, y.y(), 1E-4);
    out = out && compare(0.0, y.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat3 negative z-axis quat rotation");
    }

    return out;
}

#endif