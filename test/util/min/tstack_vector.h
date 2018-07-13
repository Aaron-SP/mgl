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
#ifndef __TEST_STACK_VECTOR__
#define __TEST_STACK_VECTOR__

#include <cstdint>
#include <min/stack_vector.h>
#include <min/test.h>
#include <stdexcept>

bool test_stack_vector()
{
    bool out = true;

    constexpr size_t size = 10;

    // Print size and alignment of class
    std::cout << "stack_vector_size: " << sizeof(min::stack_vector<int32_t, size>) << std::endl;
    std::cout << "stack_vector_align: " << alignof(min::stack_vector<int32_t, size>) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "tstack_vector.h: Testing alignment" << std::endl;
    out = out && test(sizeof(void *) + 40, sizeof(min::stack_vector<int32_t, size>), "Failed stack_vector sizeof");
    out = out && test(sizeof(void *), alignof(min::stack_vector<int32_t, size>), "Failed stack_vector alignof");
#endif

    // Test push_back
    min::stack_vector<int32_t, size> v;
    out = out && test(0, v.size(), "Failed stack_vector default construct");
    const min::stack_vector<int32_t, size> &v2 = static_cast<const min::stack_vector<int32_t, size> &>(v);
    for (size_t i = 0; i < size; i++)
    {
        v.push_back(i);
    }

    // Test range for
    int32_t sum = 0;
    for (const auto &i : v)
    {
        sum += i;
    }
    out = out && test(45, sum, "Failed stack_vector sum1");

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
    out = out && test(55, sum, "Failed stack_vector sum2");

    // Test fill
    v.fill(1);
    for (size_t i = 0; i < size; i++)
    {
        test(1, v2[i], "Failed stack_vector fill");
    }

    // Test zero
    v.zero();
    for (size_t i = 0; i < size; i++)
    {
        test(0, v2[i], "Failed stack_vector zero");
    }

    // Test clear
    v.clear();
    out = out && test(0, v.size(), "Failed stack_vector clear");

    // Test assign same size
    v = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    sum = 0;
    for (size_t i = 0; i < size; i++)
    {
        sum += v2[i];
    }
    out = out && test(65, sum, "Failed stack_vector operator= same size");
    out = out && test(size, v.size(), "Failed stack_vector operator= same size");

    // Test assign less size
    v = {2, 3, 4, 5, 6, 7, 8, 9, 10};

    sum = 0;
    for (size_t i = 0; i < size - 1; i++)
    {
        sum += v2[i];
    }
    out = out && test(54, sum, "Failed stack_vector operator= less size");
    out = out && test(size - 1, v.size(), "Failed stack_vector operator= less size");

    // return status
    return out;
}

#endif
