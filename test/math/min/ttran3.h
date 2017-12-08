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
#ifndef __TESTTRANSFORM3__
#define __TESTTRANSFORM3__

#include <min/test.h>
#include <min/tran3.h>
#include <stdexcept>

bool test_tran3()
{
    bool out = true;

    // Local variables
    min::vec3<double> p;
    min::vec3<double> t;
    min::vec3<double> s;
    min::vec4<double> v4;
    min::quat<double> q;
    min::tran3<double> tran;

    // Test identity translate matrix
    t = min::vec3<double>(1.0, 2.0, 3.0);
    v4 = min::tran3<double>().transform(t);
    out = out && compare(1.0, v4.x(), 1E-4);
    out = out && compare(2.0, v4.y(), 1E-4);
    out = out && compare(3.0, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran3 identity translation");
    }

    // Test identity rotate matrix
    t = min::vec3<double>(0.5773, 0.5773, 0.5773);
    p = min::vec3<double>(0.4082, 0.8165, -0.4082);
    q = min::quat<double>(t, p);
    v4 = min::tran3<double>().rotate(q).transform(t);
    out = out && compare(0.4082, v4.x(), 1E-4);
    out = out && compare(0.8164, v4.y(), 1E-4);
    out = out && compare(-0.4081, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran3 identity rotation");
    }

    // Test translate matrix (t -> t + p)
    t = min::vec3<double>(0.5773, 0.5773, 0.5773);
    p = min::vec3<double>(0.4082, 0.8165, -0.4082);
    v4 = min::tran3<double>().translate(p).transform(t);
    out = out && compare(0.9855, v4.x(), 1E-4);
    out = out && compare(1.3938, v4.y(), 1E-4);
    out = out && compare(0.1691, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran3 translate");
    }

    // Test identity translate-rotate matrix () (t -> p -> p + t)
    t = min::vec3<double>(0.5773, 0.5773, 0.5773);
    p = min::vec3<double>(0.4082, 0.8165, -0.4082);
    q = min::quat<double>(t, p);
    v4 = min::tran3<double>(t, q).transform(t);
    out = out && compare(0.9855, v4.x(), 1E-4);
    out = out && compare(1.3938, v4.y(), 1E-4);
    out = out && compare(0.1691, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran3 translate-rotate matrix");
    }

    // Test identity rotate+translate matrix (t -> p -> p + t)
    t = min::vec3<double>(0.5773, 0.5773, 0.5773);
    p = min::vec3<double>(0.4082, 0.8165, -0.4082);
    q = min::quat<double>(t, p);
    v4 = min::tran3<double>(q).translate(t).transform(t);
    out = out && compare(0.9855, v4.x(), 1E-4);
    out = out && compare(1.3937, v4.y(), 1E-4);
    out = out && compare(0.1691, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran3 rotate + translate");
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
    v4 = min::tran3<double>(t, q, s).transform(t);
    out = out && compare(0.0, v4.x(), 1E-4);
    out = out && compare(4.0, v4.y(), 1E-4);
    out = out && compare(0.0, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran3 translate-rotate-scale");
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
    v4 = min::tran3<double>().translate(t).rotate(q).scale(s).transform(t);
    out = out && compare(-4.0, v4.x(), 1E-4);
    out = out && compare(4.0, v4.y(), 1E-4);
    out = out && compare(-4.0, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran3 translate-rotate-scale");
    }

    // Test orthogonal decomposition
    // Test identity rotate+translate matrix (t -> p -> p + t)
    t = min::vec3<double>(0.5773, 0.5773, 0.5773);
    p = min::vec3<double>(0.4082, 0.8165, -0.4082);
    q = min::quat<double>(t, p);
    t = min::vec3<double>(-0.5773, -0.5773, -0.5773);
    q = min::tran3<double>(q).transpose().m().get_rotation();
    tran = min::tran3<double>(t).rotate(q);
    t = min::vec3<double>(0.9855, 1.3937, 0.1691);
    v4 = tran.transform(t);
    out = out && compare(0.5773, v4.x(), 1E-4);
    out = out && compare(0.5773, v4.y(), 1E-4);
    out = out && compare(0.5771, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran3 orthogonal decomposition");
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
    tran = min::tran3<double>().translate(t).rotate(q).scale(s).invert();
    t = min::vec3<double>(-4.0, 4.0, -4.0);
    v4 = tran.transform(t);
    out = out && compare(1.0, v4.x(), 1E-4);
    out = out && compare(1.0, v4.y(), 1E-4);
    out = out && compare(1.0, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran3 non-orthogonal inverse");
    }

    // Test rotation about x-axis
    t = min::vec3<double>(0.0, 0.0, 3.0);
    tran = min::tran3<double>().rotate_x(-90.0);
    v4 = tran.transform(t);
    out = out && compare(0.0, v4.x(), 1E-4);
    out = out && compare(3.0, v4.y(), 1E-4);
    out = out && compare(0.0, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran3 x-axis rotation");
    }

    // Test rotation about y-axis
    t = min::vec3<double>(0.0, 0.0, 3.0);
    tran = min::tran3<double>().rotate_y(-90.0);
    v4 = tran.transform(t);
    out = out && compare(-3.0, v4.x(), 1E-4);
    out = out && compare(0.0, v4.y(), 1E-4);
    out = out && compare(0.0, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran3 y-axis rotation");
    }

    // Test rotation about z-axis
    t = min::vec3<double>(3.0, 0.0, 0.0);
    tran = min::tran3<double>().rotate_z(-90.0);
    v4 = tran.transform(t);
    out = out && compare(0.0, v4.x(), 1E-4);
    out = out && compare(-3.0, v4.y(), 1E-4);
    out = out && compare(0.0, v4.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran3 y-axis rotation");
    }

    return out;
}

#endif
