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
#ifndef __BENCH_STATIC_VECTOR__
#define __BENCH_STATIC_VECTOR__

#include <chrono>
#include <iostream>
#include <limits>
#include <min/static_vector.h>
#include <random>
#include <vector>

double bench_static_vector()
{
    const size_t size = 100000000;
    std::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<uint8_t> dist(1, 128);

    // Running vector test
    std::cout
        << std::endl
        << "min::static_vector: initializing a " << size << " size vector" << std::endl;

    // Start the time clock
    auto start = std::chrono::high_resolution_clock::now();
    {
        // Create array and fill it with 1's then sum in random values
        min::static_vector<uint8_t> sv(size);
        sv.fill(1);
        size_t sum = 0;
        for (auto &i : sv)
        {
            i += dist(gen);
        }

        // Sum the random ints
        size_t count = 0;
        for (auto &i : sv)
        {
            count += i;
        }

        std::cout << std::endl
                  << "min::static_vector: sum result, " << count << std::endl;
    }

    // Calculate the difference between start and end
    auto dtime = std::chrono::high_resolution_clock::now() - start;
    const double min_v = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "min::static_vector: " << min_v << " ms" << std::endl;

    // Start the time clock
    start = std::chrono::high_resolution_clock::now();
    {
        // Create array and fill it with 1's then sum in random values
        std::vector<uint8_t> v(size, 1);
        size_t sum = 0;
        for (auto &i : v)
        {
            i += dist(gen);
        }

        // Sum the random ints
        size_t count = 0;
        for (auto &i : v)
        {
            count += i;
        }
        std::cout << std::endl
                  << "std::vector: sum result, " << count << std::endl;
    }

    // Calculate the difference between start and end
    dtime = std::chrono::high_resolution_clock::now() - start;
    const double std_v = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "std::vector: " << std_v << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return min_v;
}

#endif
