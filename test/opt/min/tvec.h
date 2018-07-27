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
#ifndef _MGL_TESTVECTOR_MGL_
#define _MGL_TESTVECTOR_MGL_

#include <min/test.h>
#include <min/vec.h>

bool test_vector()
{
    bool out = true;

    // Test vector operations
    min::vector<double, 2> v1;
    min::vector<double, 2> v2;

    // Test identity vector
    out = out && test(0.0, v1[0], 1E-4, "Failed vector identity");
    out = out && test(0.0, v1[1], 1E-4, "Failed vector identity");

    // Test v2 for add
    v2[0] = 1.0;
    v2[1] = 2.0;

    // Test add
    v1 += v2 + v2;
    out = out && test(2.0, v1[0], 1E-4, "Failed vector add");
    out = out && test(4.0, v1[1], 1E-4, "Failed vector add");

    // Test v2 for sub
    v2[0] = 3.0;
    v2[1] = 6.0;

    // Test sub
    v1 -= v1 - v2;
    out = out && test(3.0, v1[0], 1E-4, "Failed vector sub");
    out = out && test(6.0, v1[1], 1E-4, "Failed vector sub");

    // Test v2 for mult
    v2[0] = 2.0;
    v2[1] = 4.0;

    // Test mult
    v1 *= v1 * v2;
    out = out && test(18.0, v1[0], 1E-4, "Failed vector mult");
    out = out && test(144.0, v1[1], 1E-4, "Failed vector mult");

    // Test v2 for div
    v2[0] = 4.0;
    v2[1] = 2.0;

    // Test div
    v1 /= v1 / v2;
    out = out && test(4.0, v1[0], 1E-4, "Failed vector div");
    out = out && test(2.0, v1[1], 1E-4, "Failed vector div");

    // Test constant mathematical operations
    v1 = (((v2 + 1.0) - 2.0) * 3.0) / 4.0;
    out = out && test(2.25, v1[0], 1E-4, "Failed vector constant operations");
    out = out && test(0.75, v1[1], 1E-4, "Failed vector constant operations");

    // Test constant mathematical operations
    v2 += 1.0;
    v2 -= 2.0;
    v2 *= 3.0;
    v2 /= 4.0;

    out = out && test(2.25, v2[0], 1E-4, "Failed vector constant operations");
    out = out && test(0.75, v2[1], 1E-4, "Failed vector constant operations");

    // Test operator=
    v1 = v2;
    out = out && test(2.25, v1[0], 1E-4, "Failed vector operator equal");
    out = out && test(0.75, v1[1], 1E-4, "Failed vector operator equal");

    // Test zero
    v1.zero();
    out = out && test(0.0, v1[0], 1E-4, "Failed vector zero");
    out = out && test(0.0, v1[1], 1E-4, "Failed vector zero");

    // Test square magnitude
    v2[0] = 6.0;
    v2[1] = 8.0;
    out = out && test(100.0, v2.square_magnitude(), 1E-4, "Failed vector square magnitude");

    return out;
}

#endif
