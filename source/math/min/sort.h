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
#ifndef __UINT_SORT__
#define __UINT_SORT__

#include <algorithm>
#include <cstdint>
#include <vector>

namespace min
{
// Radix sort for unsigned integers
template <typename T, typename F>
inline void uint_sort(std::vector<T> &uints, std::vector<T> &copy, F &&key_function)
{
    const T size = uints.size();

    // Divert to std::sort for size < 128; 2*N > N*log(N)
    if (size < 128)
    {
        return std::sort(uints.begin(), uints.end(), [key_function](const T a, const T b) {
            const T a_key = key_function(a);
            const T b_key = key_function(b);
            return a_key < b_key;
        });
    }

    // Initialize copy vector
    copy.resize(size, 0);
    std::vector<T> *from = &uints;
    std::vector<T> *to = &copy;
    std::vector<T> *temp = nullptr;

    // Number of passes
    T counts[256];
    const int passes = sizeof(T) / sizeof(uint8_t);

    for (int i = 0; i < passes; i++)
    {
        // Zero counts
        for (T &count : counts)
        {
            count = 0;
        }

        // Count frequency
        for (const auto &ui : *from)
        {
            // Extract the key
            const uint_fast8_t key = (key_function(ui) >> 8 * i) & (0xFF);

            // Count the frequency of key
            counts[key]++;
        }

        // Prefix sum
        T total = 0;
        for (T &count : counts)
        {
            const T old_count = count;
            count = total;
            total += old_count;
        }

        // Sort
        for (const auto &ui : *from)
        {
            // Extract the key
            const uint_fast8_t key = (key_function(ui) >> 8 * i) & (0xFF);

            // Perform copy sort
            (*to)[counts[key]++] = ui;
        }

        // Swap from/to for next pass
        temp = from;
        from = to;
        to = temp;
    }

    // *from was the last *to
    // Copy sorted array into output, if needed
    if (from != &uints)
    {
        for (T i = 0; i < size; i++)
        {
            uints[i] = (*from)[i];
        }
    }
}
}

#endif
