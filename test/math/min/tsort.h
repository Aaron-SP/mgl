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
#ifndef _MGL_TESTSORT_MGL_
#define _MGL_TESTSORT_MGL_

#include <cstdint>
#include <min/sort.h>
#include <min/test.h>
#include <stdexcept>

bool test_uint_sort()
{
    bool out = true;

    // Creator vector N > 128 to sort
    std::vector<size_t> uints(256, 100000000);
    std::vector<size_t> sort_copy;
    uints[0] = 100000;
    uints[1] = 70000;
    uints[37] = 130000;
    uints[64] = 3;
    uints[102] = 1;
    uints[200] = 5000000;

    // Test uint radix sort
    min::uint_sort<size_t>(uints, sort_copy, [](const size_t a) {
        return a;
    });

    // Check array is sorted
    out = out && compare(1ull, uints[0]);
    out = out && compare(3ull, uints[1]);
    out = out && compare(70000ull, uints[2]);
    out = out && compare(100000ull, uints[3]);
    out = out && compare(130000ull, uints[4]);
    out = out && compare(5000000ull, uints[5]);
    out = out && compare(100000000ull, uints[6]);
    if (!out)
    {
        throw std::runtime_error("Failed uint radix sort");
    }

    // Test ordered sort
    for (size_t i = 0; i < 256; i++)
    {
        uints[i] = i;
    }

    // Test uint radix sort ordered
    min::uint_sort<size_t>(uints, sort_copy, [](const size_t a) {
        return a;
    });

    // Verify ordered
    for (size_t i = 0; i < 256; i++)
    {
        out = out && compare(i, uints[i]);
        if (!out)
        {
            throw std::runtime_error("Failed uint radix sort ordered");
        }
    }

    // Test reverse order sort
    for (size_t i = 0; i < 256; i++)
    {
        uints[i] = 256 - (i + 1);
    }

    // Test uint radix sort reverse
    min::uint_sort<size_t>(uints, sort_copy, [](const size_t a) {
        return a;
    });

    // Verify reverse
    for (size_t i = 0; i < 256; i++)
    {
        out = out && compare(i, uints[i]);
        if (!out)
        {
            throw std::runtime_error("Failed uint radix sort reverse");
        }
    }

    // Test same sort
    for (size_t i = 0; i < 256; i++)
    {
        uints[i] = 256;
    }

    // Test uint radix sort same
    min::uint_sort<size_t>(uints, sort_copy, [](const size_t a) {
        return a;
    });

    // Verify same
    for (size_t i = 0; i < 256; i++)
    {
        out = out && compare(256, uints[i]);
        if (!out)
        {
            throw std::runtime_error("Failed uint radix sort same");
        }
    }

    // Test duplicates
    for (size_t i = 0; i < 128; i++)
    {
        uints[i] = 211;
    }

    // Test duplicates
    for (size_t i = 128; i < 256; i++)
    {
        uints[i] = 203;
    }

    // Test uint radix sort same
    min::uint_sort<size_t>(uints, sort_copy, [](const size_t a) {
        return a;
    });

    // Verify same
    for (size_t i = 0; i < 128; i++)
    {
        out = out && compare(203, uints[i]);
        if (!out)
        {
            throw std::runtime_error("Failed uint radix sort duplicates");
        }
    }

    // Verify same
    for (size_t i = 128; i < 256; i++)
    {
        out = out && compare(211, uints[i]);
        if (!out)
        {
            throw std::runtime_error("Failed uint radix sort duplicates");
        }
    }

    return out;
}

#endif
