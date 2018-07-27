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
#ifndef _MGL_TESTEVOLUTIONNEAT_MGL_
#define _MGL_TESTEVOLUTIONNEAT_MGL_

#include <min/evolution.h>
#include <min/nneat.h>
#include <min/test.h>
#include <min/vec.h>

bool test_evolve_neat()
{
    bool out = true;

    // Create seed to load evolution
    min::vector<double, 3> in;
    min::nneat<double, 3, 3> seed;
    seed.set_topology_constants(11, 13, 11, 3);
    // Create a threadpool for doing work in parallel
    min::thread_pool pool;

    // Create evolution
    min::evolution<double, 3, 3, min::nneat, 512, 8, 1, 60> evolve(pool, seed);

    // Create fitness function
    const std::function<double(const min::nneat<double, 3, 3> &)> &fitness = [&in](const min::nneat<double, 3, 3> &net) {
        // Set input to network
        net.set_input(in);

        // Calculate output
        min::vector<double, 3> out = net.calculate();

        // Calculate fitness
        double fitness = 1.0;
        for (size_t i = 0; i < 3; i++)
        {
            // Testing XOR bit, error should always be 1.0
            const double error = std::abs(std::abs(out[i] - in[i]) - 1.0);
            fitness -= error;
        }

        // Calculate the score
        return fitness;
    };

    // Train the network on all input data
    unsigned count = 0;
    for (size_t i = 0; i < 600; i++)
    {
        // Set 3 bit flags
        for (size_t j = 0; j < 3; j++)
        {
            // Extract bit 0 or 1
            unsigned bit = (count >> j) & 0x1;

            // Set input XOR bit
            in[j] = static_cast<double>(bit);
        }

        // Increment count, b111 = d7
        count++;
        if (count == 8)
        {
            count = 0;
        }

        // Evolve loop
        evolve.evolve(pool, fitness);
    }

    min::nneat<double, 3, 3> test_nn = evolve.top_net();

    // Evaluate all possibilities
    count = 0;
    int passed = 0;
    for (size_t i = 0; i < 8; i++)
    {
        // Set 3 bit flags
        for (size_t j = 0; j < 3; j++)
        {
            // Extract bit 0 or 1
            unsigned bit = (count >> j) & 0x1;

            // Set input XOR bit
            in[j] = static_cast<double>(bit);
        }

        // Increment count, b111 = d7
        count++;

        // Evolve loop
        if (fitness(test_nn) > 0.5)
        {
            passed++;
        }
    }

    out = out && test(8, passed, "Failed NEAT evolution");

    return out;
}

#endif
