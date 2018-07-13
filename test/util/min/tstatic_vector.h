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
#ifndef __TEST_STATIC_VECTOR__
#define __TEST_STATIC_VECTOR__

#include <min/static_vector.h>
#include <min/test.h>
#include <stdexcept>

bool test_static_vector()
{
    bool out = true;

    constexpr size_t size = 10;
    constexpr size_t size2 = 5;

    // Print size and alignment of class
    std::cout << "static_vector_size: " << sizeof(min::static_vector<int32_t>) << std::endl;
    std::cout << "static_vector_align: " << alignof(min::static_vector<int32_t>) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "tstatic_vector.h: Testing alignment" << std::endl;
    out = out && test(sizeof(void *) * 2, sizeof(min::static_vector<int32_t>), "Failed static_vector sizeof");
    out = out && test(sizeof(void *), alignof(min::static_vector<int32_t>), "Failed static_vector alignof");
#endif

    // Test push_back
    min::static_vector<int> v;
    out = out && test(0, v.size(), "Failed static_vector default construct");
    const min::static_vector<int> &v2 = static_cast<const min::static_vector<int> &>(v);
    v.resize(size);
    for (size_t i = 0; i < size; i++)
    {
        v[i] = i;
    }

    // Test range for
    int sum = 0;
    for (const auto &i : v)
    {
        sum += i;
    }
    out = out && test(45, sum, "Failed static_vector sum1");

    // Test ref access
    for (size_t i = 0; i < size; i++)
    {
        v[i] = i + 1;
    }

    // Test const ref access
    sum = 0;
    for (size_t i = 0; i < size; i++)
    {
        sum += v2[i];
    }
    out = out && test(55, sum, "Failed static_vector sum2");

    // Test fill
    v.fill(1);
    for (size_t i = 0; i < size; i++)
    {
        test(1, v2[i], "Failed static_vector fill");
    }

    // Test zero
    v.zero();
    for (size_t i = 0; i < size; i++)
    {
        test(0, v2[i], "Failed static_vector zero");
    }

    // Test assign
    v = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    sum = 0;
    for (size_t i = 0; i < size; i++)
    {
        sum += v2[i];
    }
    out = out && test(65, sum, "Failed static_vector operator=");
    out = out && test(size, v.size(), "Failed static_vector operator=");

    // Test assign less size
    v = {2, 3, 4, 5, 6, 7, 8, 9, 10};

    sum = 0;
    for (size_t i = 0; i < size - 1; i++)
    {
        sum += v2[i];
    }
    out = out && test(54, sum, "Failed static_vector operator= less size");
    out = out && test(size - 1, v.size(), "Failed static_vector operator= less size");

    // Resize the vector
    v.resize(size2);
    sum = 0;
    for (size_t i = 0; i < size2; i++)
    {
        sum += v2[i];
    }
    out = out && test(20, sum, "Failed static_vector resize");
    out = out && test(size2, v.size(), "Failed static_vector resize");

    // return status
    return out;
}

#endif
