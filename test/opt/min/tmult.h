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
#ifndef _MGL_TESTMULTIPLY_MGL_
#define _MGL_TESTMULTIPLY_MGL_

#include <min/mat.h>
#include <min/mult.h>
#include <min/test.h>
#include <min/vec.h>

bool test_matrix_multiply()
{
    bool out = true;

    // Test matrix operations
    min::matrix<double, 2, 3> m1;
    min::matrix<double, 3, 2> m2;

    // set m1 for matrix-matrix multiplication test
    m1.get(0, 0) = 1.0;
    m1.get(0, 1) = 2.0;
    m1.get(0, 2) = 3.0;
    m1.get(1, 0) = 4.0;
    m1.get(1, 1) = 5.0;
    m1.get(1, 2) = 6.0;

    // set m2 for matrix-matrix multiplication test
    m2.get(0, 0) = 7.0;
    m2.get(0, 1) = 8.0;
    m2.get(1, 0) = 9.0;
    m2.get(1, 1) = 10.0;
    m2.get(2, 0) = 11.0;
    m2.get(2, 1) = 12.0;

    // multiply matrices
    min::matrix<double, 2, 2> m3 = min::multiply<double, 2, 3, 3, 2>(m1, m2);
    out = out && test(58.0, m3.get(0, 0), 1E-4, "Failed matrix multiply");
    out = out && test(64.0, m3.get(0, 1), 1E-4, "Failed matrix multiply");
    out = out && test(139.0, m3.get(1, 0), 1E-4, "Failed matrix multiply");
    out = out && test(154.0, m3.get(1, 1), 1E-4, "Failed matrix multiply");

    // set m1 for vector-matrix multiplication test
    m1.get(0, 0) = 1.0;
    m1.get(0, 1) = -1.0;
    m1.get(0, 2) = 2.0;
    m1.get(1, 0) = 0.0;
    m1.get(1, 1) = -3.0;
    m1.get(1, 2) = 1.0;

    // set v1 for vector-matrix multiplication test
    min::vector<double, 3> v1;
    v1[0] = 2;
    v1[1] = 1;
    v1[2] = 0;

    // Test vector-matrix multiplication
    min::vector<double, 2> v2 = min::multiply<double, 2, 3>(m1, v1);
    out = out && test(1.0, v2[0], 1E-4, "Failed vector-matrix multiply");
    out = out && test(-3.0, v2[1], 1E-4, "Failed vector-matrix multiply");

    return out;
}

#endif
