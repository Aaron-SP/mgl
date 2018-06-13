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
#ifndef __TESTQUATERNION__
#define __TESTQUATERNION__

#include <min/quat.h>
#include <min/test.h>
#include <min/vec3.h>
#include <stdexcept>

bool test_quat()
{
    bool out = true;

    // Local variables
    min::quat<double> q;
    min::quat<double> one;
    min::quat<double> two;
    min::vec3<double> x;
    min::vec3<double> y;
    min::vec3<double> z;
    double dot;

    // Test basic constructor
    q = min::quat<double>(1.0, 2.0, 3.0);
    out = out && compare(0.2581, q.w(), 1E-4);
    out = out && compare(0.2581, q.x(), 1E-4);
    out = out && compare(0.51639, q.y(), 1E-4);
    out = out && compare(0.7745, q.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat constructor");
    }

    // Test normalization and calculating w
    q = min::quat<double>(1.0, 2.0, 3.0);
    q.calculate_w();
    out = out && compare(0.2581, q.w(), 1E-4);
    out = out && compare(0.2581, q.x(), 1E-4);
    out = out && compare(0.5163, q.y(), 1E-4);
    out = out && compare(0.7745, q.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat normalization and W calculation");
    }

    // Test conjugation
    q = q.conjugate();
    out = out && compare(0.2581, q.w(), 1E-4);
    out = out && compare(-0.2581, q.x(), 1E-4);
    out = out && compare(-0.5163, q.y(), 1E-4);
    out = out && compare(-0.7745, q.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat conjugation");
    }

    // Test dot product
    one = min::quat<double>(1.0, 0.0, 0.0);
    two = min::quat<double>(0.0, 1.0, 0.0);
    q = min::quat<double>(one);
    dot = q.dot(two);
    out = out && compare(0.5, dot, 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat lerp");
    }

    // Test linear lerp
    one = min::quat<double>(1.0, 2.0, 3.0);
    two = min::quat<double>(2.0, 4.0, 6.0);
    q = min::quat<double>::lerp(one, two, 0.5);
    out = out && compare(0.1957, q.w(), 1E-4);
    out = out && compare(0.2620, q.x(), 1E-4);
    out = out && compare(0.5241, q.y(), 1E-4);
    out = out && compare(0.7862, q.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat lerp");
    }

    // Test spherical lerp -> defaults to llerp parallel vectors
    one = min::quat<double>(1.0, 2.0, 3.0);
    two = min::quat<double>(2.0, 4.0, 6.0);
    q = min::quat<double>::slerp(one, two, 0.5);
    out = out && compare(0.1957, q.w(), 1E-4);
    out = out && compare(0.2620, q.x(), 1E-4);
    out = out && compare(0.5241, q.y(), 1E-4);
    out = out && compare(0.7862, q.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat lerp parallel");
    }

    // Test linear lerp x-axis -> y-axis
    one = min::quat<double>(1.0, 1.0, 1.0);
    two = min::quat<double>(-1.0, 1.0, -1.0);
    q = min::quat<double>::lerp(one, two, 0.2);
    out = out && compare(0.6063, q.w(), 1E-4);
    out = out && compare(0.3638, q.x(), 1E-4);
    out = out && compare(0.6063, q.y(), 1E-4);
    out = out && compare(0.3638, q.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat lerp vs slerp");
    }

    // Test spherical lerp x-axis -> y-axis
    one = min::quat<double>(1.0, 1.0, 1.0);
    two = min::quat<double>(-1.0, 1.0, -1.0);
    q = min::quat<double>::slerp(one, two, 0.2);
    out = out && compare(0.6300, q.w(), 1E-4);
    out = out && compare(0.3210, q.x(), 1E-4);
    out = out && compare(0.6300, q.y(), 1E-4);
    out = out && compare(0.3210, q.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat slerp vs lerp");
    }

    // Test transform vector with quat
    x = min::vec3<double>(1.0, 0.0, 0.0);
    q = min::quat<double>(1.0, 1.0, 1.0); // x-axis to y-axis
    y = q.transform(x);
    out = out && compare(0.0, y.x(), 1E-4);
    out = out && compare(1.0, y.y(), 1E-4);
    out = out && compare(0.0, y.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat transform");
    }

    // Test constructing quat from two vectors x-axis and y-axis
    x = min::vec3<double>(1.0, 0.0, 0.0);
    y = min::vec3<double>(0.0, 1.0, 0.0);
    q = min::quat<double>(x, y);
    out = out && compare(0.7071, q.w(), 1E-4);
    out = out && compare(0.0, q.x(), 1E-4);
    out = out && compare(0.0, q.y(), 1E-4);
    out = out && compare(0.7071, q.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat construction between two vectors");
    }

    // Test constructing quat from two parallel vectors - same direction
    x = min::vec3<double>(1.0, 0.0, 0.0);
    y = min::vec3<double>(1.0, 0.0, 0.0);
    q = min::quat<double>(x, y);
    x = q.transform(x);
    out = out && compare(1.0, x.x(), 1E-4);
    out = out && compare(0.0, x.y(), 1E-4);
    out = out && compare(0.0, x.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat transform between two parallel vectors");
    }

    // Test constructing quat from two parallel vectors - opposite direction
    x = min::vec3<double>(1.0, 0.0, 0.0);
    y = min::vec3<double>(-1.0, 0.0, 0.0);
    q = min::quat<double>(x, y);
    x = q.transform(x);
    out = out && compare(-1.0, x.x(), 1E-4);
    out = out && compare(0.0, x.y(), 1E-4);
    out = out && compare(0.0, x.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat transform between two opposite vectors");
    }

    // Test constructing quat from two vectors
    x = min::vec3<double>(1.0, 1.0, 1.0).normalize();
    y = min::vec3<double>(-1.0, 1.0, -1.0).normalize();
    q = min::quat<double>(x, y);
    x = min::vec3<double>(1.0, 1.0, 1.0);
    x = q.transform(x);
    out = out && compare(-1.0, x.x(), 1E-4);
    out = out && compare(1.0, x.y(), 1E-4);
    out = out && compare(-1.0, x.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat transform between two vectors 1");
    }

    // Test constructing quat from two vectors
    x = min::vec3<double>(1.0, 1.0, 1.0).normalize();
    y = min::vec3<double>(-1.0, -1.0, -1.0).normalize();
    q = min::quat<double>(x, y);
    x = min::vec3<double>(1.0, 1.0, 1.0);
    x = q.transform(x);
    out = out && compare(-1.0, x.x(), 1E-4);
    out = out && compare(-1.0, x.y(), 1E-4);
    out = out && compare(-1.0, x.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat transform between two vectors 2");
    }

    // Test constructing quat from x-axis and vector
    y = min::vec3<double>(0.0, 1.0, 0.0).normalize();
    q = min::quat<double>::from_x_axis(-1.0, y);
    x = min::vec3<double>(-1.0, 0.0, 0.0);
    x = q.transform(x);
    out = out && compare(0.0, x.x(), 1E-4);
    out = out && compare(1.0, x.y(), 1E-4);
    out = out && compare(0.0, x.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat transform between x-axis and vector");
    }
    // Test rotate y-axis -> z-axis; indicates positive rotation
    x = min::vec3<double>(1.0, 0.0, 0.0);
    y = min::vec3<double>(0.0, 1.0, 0.0);
    q = min::quat<double>(x, 90.0);
    z = q.transform(y);
    out = out && compare(0.0, z.x(), 1E-4);
    out = out && compare(0.0, z.y(), 1E-4);
    out = out && compare(1.0, z.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat x-axis rotation");
    }

    // Test rotate z-axis -> x-axis; indicates positive rotation
    z = min::vec3<double>(0.0, 0.0, 1.0);
    y = min::vec3<double>(0.0, 1.0, 0.0);
    q = min::quat<double>(y, 90.0);
    x = q.transform(z);
    out = out && compare(1.0, x.x(), 1E-4);
    out = out && compare(0.0, x.y(), 1E-4);
    out = out && compare(0.0, x.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat y-axis rotation");
    }

    // Test rotate x-axis -> y-axis; indicates positive rotation
    x = min::vec3<double>(1.0, 0.0, 0.0);
    z = min::vec3<double>(0.0, 0.0, 1.0);
    q = min::quat<double>(z, 90.0);
    y = q.transform(x);
    out = out && compare(0.0, y.x(), 1E-4);
    out = out && compare(1.0, y.y(), 1E-4);
    out = out && compare(0.0, y.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat z-axis rotation");
    }

    // Test inverse quaternion
    q = q * q.inverse();
    out = out && compare(1.0, q.w(), 1E-4);
    out = out && compare(0.0, q.x(), 1E-4);
    out = out && compare(0.0, q.y(), 1E-4);
    out = out && compare(0.0, q.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed quat inverse z-axis rotation");
    }

    return out;
}

#endif
