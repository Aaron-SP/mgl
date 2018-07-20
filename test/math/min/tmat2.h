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
#ifndef _MGL_TESTMATRIX2_MGL_
#define _MGL_TESTMATRIX2_MGL_

#include <min/mat2.h>
#include <min/test.h>
#include <stdexcept>

bool test_mat2()
{
    bool out = true;

    // Print size and alignment of class
    std::cout << "mat2_size: " << sizeof(min::mat2<float>) << std::endl;
    std::cout << "mat2_align: " << alignof(min::mat2<float>) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "tmat2.h: Testing alignment" << std::endl;
    out = out && test(sizeof(float) * 4, sizeof(min::mat2<float>), "Failed mat2 sizeof");
    out = out && test(sizeof(float), alignof(min::mat2<float>), "Failed mat2 alignof");
#endif

    // Local variables
    min::mat2<double> mat;
    min::vec2<double> v2;

    // Test identity matrix
    mat = min::mat2<double>();
    v2 = min::vec2<double>(1.0, 2.0);
    v2 = mat * v2;
    out = out && compare(1.0, v2.x(), 1E-4);
    out = out && compare(2.0, v2.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat2 identity matrix");
    }

    // Rotate 90 degrees CW (1.0, 0.0) -> (0.0, -1.0)
    v2 = min::vec2<double>(1.0, 0.0);
    min::mat2<double> rot = min::mat2<double>(-90.0);
    v2 = rot * v2;
    out = out && compare(0.0, v2.x(), 1E-4);
    out = out && compare(-1.0, v2.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat2 rotation 90 CW matrix");
    }

    // Reverse rotation with transpose (0.0, -1.0) -> (1.0, 0.0)
    rot.transpose();
    v2 = rot * v2;
    out = out && compare(1.0, v2.x(), 1E-4);
    out = out && compare(0.0, v2.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat2 transpose");
    }

    // Rotate 90 degrees CCW (1.0, 0.0) -> (0.0, 1.0)
    rot = min::mat2<double>(90.0);
    v2 = rot * v2;
    out = out && compare(0.0, v2.x(), 1E-4);
    out = out && compare(1.0, v2.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat2 rotation 90 CCW matrix");
    }

    // Invert the last rotation (0.0, 1.0) -> (1.0, 0.0)
    rot.invert();
    v2 = rot * v2;
    out = out && compare(1.0, v2.x(), 1E-4);
    out = out && compare(0.0, v2.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat2 inverse rotation matrix");
    }

    // Invert the last rotation twice (1.0, 0.0) -> (-1.0, 0.0)
    rot.invert();
    rot = rot * rot;
    v2 = rot * v2;
    out = out && compare(-1.0, v2.x(), 1E-4);
    out = out && compare(0.0, v2.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed mat2 inverse multiplication 180 rotation matrix");
    }

    return out;
}
#endif
