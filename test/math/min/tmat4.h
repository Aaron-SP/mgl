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
#ifndef _MGL_TESTMATRIX4_MGL_
#define _MGL_TESTMATRIX4_MGL_

#include <min/mat4.h>
#include <min/test.h>
#include <stdexcept>

bool test_mat4()
{
    bool out = true;

    // Print size and alignment of class
    std::cout << "mat4_size: " << sizeof(min::mat4<float>) << std::endl;
    std::cout << "mat4_align: " << alignof(min::mat4<float>) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "tmat4.h: Testing alignment" << std::endl;
    out = out && test(sizeof(float) * 16, sizeof(min::mat4<float>), "Failed mat4 sizeof");
    out = out && test(sizeof(float), alignof(min::mat4<float>), "Failed mat4 alignof");
#endif

    // Local variables
    min::mat4<double> mat;
    min::vec3<double> v3;
    min::vec3<double> x;
    min::vec3<double> y;
    min::vec3<double> p;
    min::vec3<double> t;
    min::vec3<double> s;
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

    // Test identity translate matrix
    t = min::vec3<double>(1.0, 2.0, 3.0);
    v4 = min::mat4<double>().transform(t);
    out = out && compare(1.0, v4.x(), 1E-4);
    out = out && compare(2.0, v4.y(), 1E-4);
    out = out && compare(3.0, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 identity translation");
    }

    // Test identity rotate matrix
    t = min::vec3<double>(0.5773, 0.5773, 0.5773);
    p = min::vec3<double>(0.4082, 0.8165, -0.4082);
    q = min::quat<double>(t, p);
    v4 = min::mat4<double>().rotate(q).transform(t);
    out = out && compare(0.4082, v4.x(), 1E-4);
    out = out && compare(0.8164, v4.y(), 1E-4);
    out = out && compare(-0.4081, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 identity rotation");
    }

    // Test translate matrix (t -> t + p)
    t = min::vec3<double>(0.5773, 0.5773, 0.5773);
    p = min::vec3<double>(0.4082, 0.8165, -0.4082);
    v4 = min::mat4<double>().translate(p).transform(t);
    out = out && compare(0.9855, v4.x(), 1E-4);
    out = out && compare(1.3938, v4.y(), 1E-4);
    out = out && compare(0.1691, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 translate");
    }

    // Test identity translate-rotate matrix () (t -> p -> p + t)
    t = min::vec3<double>(0.5773, 0.5773, 0.5773);
    p = min::vec3<double>(0.4082, 0.8165, -0.4082);
    q = min::quat<double>(t, p);
    v4 = min::mat4<double>(t, q).transform(t);
    out = out && compare(0.9855, v4.x(), 1E-4);
    out = out && compare(1.3938, v4.y(), 1E-4);
    out = out && compare(0.1691, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 translate-rotate matrix");
    }

    // Test identity rotate+translate matrix (t -> p -> p + t)
    t = min::vec3<double>(0.5773, 0.5773, 0.5773);
    p = min::vec3<double>(0.4082, 0.8165, -0.4082);
    q = min::quat<double>(t, p);
    v4 = min::mat4<double>(q).translate(t).transform(t);
    out = out && compare(0.9855, v4.x(), 1E-4);
    out = out && compare(1.3937, v4.y(), 1E-4);
    out = out && compare(0.1691, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 rotate + translate");
    }

    // Test identity translate-rotate-scale matrix
    // (1.0, 1.0, 1.0) -> (-1.0, 1.0, -1.0) -> (0.0, 2.0, 0.0) -> (0.0, 4.0, 0.0)
    t = min::vec3<double>(1.0, 1.0, 1.0);
    t.normalize();
    p = min::vec3<double>(-1.0, 1.0, -1.0);
    p.normalize();
    q = min::quat<double>(t, p);
    t = min::vec3<double>(1.0, 1.0, 1.0);
    s = min::vec3<double>(2.0, 2.0, 2.0);
    v4 = min::mat4<double>(t, q, s).transform(t);
    out = out && compare(0.0, v4.x(), 1E-4);
    out = out && compare(4.0, v4.y(), 1E-4);
    out = out && compare(0.0, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 translate-rotate-scale");
    }

    // Test identity translate-rotate-scale matrix
    // (1.0, 1.0, 1.0) -> (2.0, 2.0, 2.0) -> (-2.0, 2.0, -2.0) -> (-4.0, 4.0, -4.0)
    t = min::vec3<double>(1.0, 1.0, 1.0);
    t.normalize();
    p = min::vec3<double>(-1.0, 1.0, -1.0);
    p.normalize();
    q = min::quat<double>(t, p);
    t = min::vec3<double>(1.0, 1.0, 1.0);
    s = min::vec3<double>(2.0, 2.0, 2.0);
    v4 = min::mat4<double>().translate(t).rotate(q).scale(s).transform(t);
    out = out && compare(-4.0, v4.x(), 1E-4);
    out = out && compare(4.0, v4.y(), 1E-4);
    out = out && compare(-4.0, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 translate-rotate-scale");
    }

    // Test orthogonal decomposition
    // Test identity rotate+translate matrix (t -> p -> p + t)
    t = min::vec3<double>(0.5773, 0.5773, 0.5773);
    p = min::vec3<double>(0.4082, 0.8165, -0.4082);
    q = min::quat<double>(t, p);
    t = min::vec3<double>(-0.5773, -0.5773, -0.5773);
    q = min::mat4<double>(q).transpose().get_rotation();
    mat = min::mat4<double>(t).rotate(q);
    t = min::vec3<double>(0.9855, 1.3937, 0.1691);
    v4 = mat.transform(t);
    out = out && compare(0.5773, v4.x(), 1E-4);
    out = out && compare(0.5773, v4.y(), 1E-4);
    out = out && compare(0.5771, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 orthogonal decomposition");
    }

    // Test non-orthogonal inverse
    // (-4.0, 4.0, -4.0) -> (-2.0, 2.0, -2.0) -> (2.0, 2.0, 2.0) -> (1.0, 1.0, 1.0)
    t = min::vec3<double>(1.0, 1.0, 1.0);
    t.normalize();
    p = min::vec3<double>(-1.0, 1.0, -1.0);
    p.normalize();
    q = min::quat<double>(t, p);
    t = min::vec3<double>(1.0, 1.0, 1.0);
    s = min::vec3<double>(2.0, 2.0, 2.0);
    mat = min::mat4<double>().translate(t).rotate(q).scale(s);
    mat.invert();
    t = min::vec3<double>(-4.0, 4.0, -4.0);
    v4 = mat.transform(t);
    out = out && compare(1.0, v4.x(), 1E-4);
    out = out && compare(1.0, v4.y(), 1E-4);
    out = out && compare(1.0, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 non-orthogonal inverse");
    }

    // Test rotation about x-axis
    t = min::vec3<double>(0.0, 0.0, 3.0);
    mat = min::mat4<double>().rotate_x(-90.0);
    v4 = mat.transform(t);
    out = out && compare(0.0, v4.x(), 1E-4);
    out = out && compare(3.0, v4.y(), 1E-4);
    out = out && compare(0.0, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 x-axis rotation");
    }

    // Test rotation about y-axis
    t = min::vec3<double>(0.0, 0.0, 3.0);
    mat = min::mat4<double>().rotate_y(-90.0);
    v4 = mat.transform(t);
    out = out && compare(-3.0, v4.x(), 1E-4);
    out = out && compare(0.0, v4.y(), 1E-4);
    out = out && compare(0.0, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 y-axis rotation");
    }

    // Test rotation about z-axis
    t = min::vec3<double>(3.0, 0.0, 0.0);
    mat = min::mat4<double>().rotate_z(-90.0);
    v4 = mat.transform(t);
    out = out && compare(0.0, v4.x(), 1E-4);
    out = out && compare(-3.0, v4.y(), 1E-4);
    out = out && compare(0.0, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat4 y-axis rotation");
    }

    return out;
}

#endif
