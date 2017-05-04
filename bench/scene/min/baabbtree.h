/* Copyright [2013-2016] [Aaron Springstroh, Minimal Graphics Library]

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
#ifndef __BENCHAABBTREE__
#define __BENCHAABBTREE__

#include <min/aabbox.h>
#include <min/sphere.h>
#include <min/tree.h>
#include <stdexcept>
#include <chrono>
#include <random>
#include <iostream>

template <typename T, size_t col_count, template <typename> class vec>
double bench_aabb_aabb_tree(size_t N)
{
    // Running aabb_aabb_tree test
    std::cout << "aabb_aabb_tree: Starting benchmark with " << N << " tree insertions" << std::endl;

    // Start the time clock
    auto start = std::chrono::high_resolution_clock::now();

    // Local variables
    T low = -10000.0;
    T high = 10000.0;
    vec<T> minW = vec<T>().set_all(low);
    vec<T> maxW = vec<T>().set_all(high);
    min::aabbox<T, vec> world(minW, maxW);
    min::tree<T, uint16_t, uint32_t, vec, min::aabbox, min::aabbox> t(world);
    std::vector<min::aabbox<T, vec>> items;
    items.reserve(N);

    // The objects will be between 1.0 and 100
    std::uniform_real_distribution<T> position(low, high);
    std::uniform_real_distribution<T> size(1.0, 100.0);

    // Mersenne Twister: Good quality random number generator
    std::mt19937 rng;
    // Initialize with fixed seed
    rng.seed(1337);

    // Create 'N' random cubic aabb's
    for (size_t i = 0; i < N; i++)
    {
        // Calculate AABB center and extent
        T center = position(rng);
        T extent = size(rng);

        // Lower left and upper right corners
        T ll = center - extent;
        T ur = center + extent;

        // Create the AABB
        vec<T> min = vec<T>().set_all(ll);
        vec<T> max = vec<T>().set_all(ur);
        items.emplace_back(min, max);
    }

    // Insert into tree
    t.insert(std::move(items));

    // Get the calculated tree depth
    int depth = t.get_depth();

    // Get all colliding objects
    std::vector<std::pair<uint16_t, uint16_t>> collisions = t.get_collisions();

    // Calculate the difference between start and end
    auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Report tree depth
    std::cout << "aabb_aabb_tree: Tree depth is: " << depth << std::endl;

    // Report collisions found
    std::cout << "aabb_aabb_tree: Collisions found: " << collisions.size() << std::endl;

    if (collisions.size() != col_count)
    {
        std::cout << "aabb_aabb_tree: Failed benchmark, wrong collision count" << std::endl;
    }

    // Print the execution time
    double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "aabb_aabb_tree: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}

template <typename T, size_t col_count, template <typename> class vec>
double bench_aabb_sphere_tree(size_t N)
{
    // Running aabb_sphere_tree test
    std::cout << "aabb_sphere_tree: Starting benchmark with " << N << " tree insertions" << std::endl;

    // Start the time clock
    auto start = std::chrono::high_resolution_clock::now();

    // Local variables
    T low = -10000.0;
    T high = 10000.0;
    T factor = vec<T>::unit_length();
    vec<T> minW = vec<T>().set_all(low);
    vec<T> maxW = vec<T>().set_all(high);
    min::aabbox<T, vec> world(minW, maxW);
    min::tree<T, uint16_t, uint32_t, vec, min::aabbox, min::sphere> t(world);
    std::vector<min::sphere<T, vec>> items;
    items.reserve(N);

    // The objects will be between 1.0 and 100
    std::uniform_real_distribution<T> position(low, high);
    std::uniform_real_distribution<T> size(1.0, 100.0);

    // Mersenne Twister: Good quality random number generator
    std::mt19937 rng;
    // Initialize with fixed seed
    rng.seed(1337);

    // Create 'N' random cubic sphere's
    for (size_t i = 0; i < N; i++)
    {
        // Calculate sphere center and extent
        T center = position(rng);
        T radius = factor * size(rng);

        // Create the sphere
        vec<T> c = vec<T>().set_all(center);
        items.emplace_back(c, radius);
    }

    // Insert into tree
    t.insert(std::move(items));

    // Get the calculated tree depth
    int depth = t.get_depth();

    // Get all colliding objects
    std::vector<std::pair<uint16_t, uint16_t>> collisions = t.get_collisions();

    // Calculate the difference between start and end
    auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Report tree depth
    std::cout << "aabb_sphere_tree: Tree depth is: " << depth << std::endl;

    // Report collisions found
    std::cout << "aabb_sphere_tree: Collisions found: " << collisions.size() << std::endl;

    if (collisions.size() != col_count)
    {
        std::cout << "aabb_sphere_tree: Failed benchmark, wrong collision count." << std::endl;
    }

    // Print the execution time
    double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "aabb_sphere_tree: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}

#endif