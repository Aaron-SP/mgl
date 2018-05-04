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
#ifndef __BENCHSPATIAL__
#define __BENCHSPATIAL__

#include <chrono>
#include <iostream>
#include <min/aabbox.h>
#include <min/oobbox.h>
#include <min/sphere.h>
#include <random>
#include <stdexcept>

template <typename T, template <typename> class vec>
constexpr min::sphere<T, vec> make_sphere()
{
    const T low = -100000.0;
    const T high = 100000.0;
    const vec<T> minW = vec<T>().set_all(low);
    const vec<T> maxW = vec<T>().set_all(high);

    return min::sphere<T, vec>(minW, maxW);
}

template <typename T, template <typename> class vec>
constexpr min::aabbox<T, vec> make_aabbox()
{
    const T low = -100000.0;
    const T high = 100000.0;
    const vec<T> minW = vec<T>().set_all(low);
    const vec<T> maxW = vec<T>().set_all(high);

    return min::aabbox<T, vec>(minW, maxW);
}

template <typename T, template <typename> class vec>
constexpr min::oobbox<T, vec> make_oobbox()
{
    const T low = -100000.0;
    const T high = 100000.0;
    const vec<T> minW = vec<T>().set_all(low);
    const vec<T> maxW = vec<T>().set_all(high);

    return min::oobbox<T, vec>(minW, maxW);
}

template <typename T, template <typename> class vec>
constexpr const std::vector<min::aabbox<T, vec>> make_aabb_boxes(const size_t N)
{
    // Local variables
    const T low = -99999.999;
    const T high = 99999.999;
    std::vector<min::aabbox<T, vec>> items;
    items.reserve(N);

    // The objects will be between 1.0 and 100, x = z
    std::uniform_real_distribution<T> x(low, high);
    std::uniform_real_distribution<T> size(1.0, 100.0);

    // Mersenne Twister: Good quality random number generator
    std::mt19937 rng;
    // Initialize with fixed seed
    rng.seed(1337);

    // Create 'N' random cubic aabb's
    for (size_t i = 0; i < N; i++)
    {
        // Calculate AABB center and extent
        vec<T> center = vec<T>().set_all(x(rng));
        const T extent = size(rng);

        // Create the AABB
        const vec<T> min = center - extent;
        const vec<T> max = center + extent;
        items.emplace_back(min, max);
    }

    return items;
}

template <typename T, template <typename> class vec>
constexpr const std::vector<min::oobbox<T, vec>> make_oobb_boxes(const size_t N)
{
    // Local variables
    const T low = -99999.999;
    const T high = 99999.999;
    std::vector<min::oobbox<T, vec>> items;
    items.reserve(N);

    // The objects will be between 1.0 and 100, x = z
    std::uniform_real_distribution<T> x(low, high);
    std::uniform_real_distribution<T> size(1.0, 100.0);

    // Mersenne Twister: Good quality random number generator
    std::mt19937 rng;
    // Initialize with fixed seed
    rng.seed(1337);

    // Create 'N' random cubic oobb's
    for (size_t i = 0; i < N; i++)
    {
        // Calculate OOBB center and extent
        vec<T> center = vec<T>().set_all(x(rng));
        const T extent = size(rng);

        // Create the OOBB
        const vec<T> min = center - extent;
        const vec<T> max = center + extent;
        items.emplace_back(min, max);
    }

    return items;
}

template <typename T, template <typename> class vec>
constexpr const std::vector<min::sphere<T, vec>> make_spheres(const size_t N)
{
    // Local variables
    const T low = -99999.999;
    const T high = 99999.999;
    const T factor = vec<T>::unit_length();
    std::vector<min::sphere<T, vec>> items;
    items.reserve(N);

    // The objects will be between 1.0 and 100, x = z
    std::uniform_real_distribution<T> x(low, high);
    std::uniform_real_distribution<T> size(1.0, 100.0);

    // Mersenne Twister: Good quality random number generator
    std::mt19937 rng;
    // Initialize with fixed seed
    rng.seed(1337);

    // Create 'N' random cubic sphere's
    for (size_t i = 0; i < N; i++)
    {
        // Calculate sphere center and extent
        vec<T> center = vec<T>().set_all(x(rng));
        const T radius = factor * size(rng);

        // Create the sphere
        items.emplace_back(center, radius);
    }

    return items;
}

template <typename T, size_t col_count, template <typename> class vec,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
double bench_aabb_aabb(const size_t N, const min::aabbox<T, vec> &world, const std::vector<min::aabbox<T, vec>> &boxes)
{
    // Running aabb_aabb test
    std::cout << "aabb_aabb: Starting benchmark with " << N << " insertions" << std::endl;

    // Start the time clock
    const auto start = std::chrono::high_resolution_clock::now();

    // Create the spatial data structure
    spatial<T, uint16_t, uint32_t, vec, min::aabbox, min::aabbox> g(world);

    // Insert into grid
    g.insert(boxes);

    // Get all colliding objects
    std::vector<std::pair<uint16_t, uint16_t>> collisions = g.get_collisions();

    // Calculate the difference between start and end
    const auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Report collisions found
    std::cout << "aabb_aabb: Collisions found: " << collisions.size() << std::endl;

    if (collisions.size() != col_count)
    {
        std::cout << "aabb_aabb: Failed benchmark, wrong collision count" << std::endl;
    }

    // Print the execution time
    const double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "aabb_aabb: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}

template <typename T, size_t col_count, template <typename> class vec,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
double bench_aabb_oobb(const size_t N, const min::aabbox<T, vec> &world, const std::vector<min::oobbox<T, vec>> &boxes)
{
    // Running aabb_oobb test
    std::cout << "aabb_oobb: Starting benchmark with " << N << " insertions" << std::endl;

    // Start the time clock
    const auto start = std::chrono::high_resolution_clock::now();

    // Create the spatial data structure
    spatial<T, uint16_t, uint32_t, vec, min::aabbox, min::oobbox> g(world);

    // Insert into grid
    g.insert(boxes);

    // Get all colliding objects
    std::vector<std::pair<uint16_t, uint16_t>> collisions = g.get_collisions();

    // Calculate the difference between start and end
    const auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Report collisions found
    std::cout << "aabb_oobb: Collisions found: " << collisions.size() << std::endl;

    if (collisions.size() != col_count)
    {
        std::cout << "aabb_oobb: Failed benchmark, wrong collision count" << std::endl;
    }

    // Print the execution time
    const double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "aabb_oobb: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}

template <typename T, size_t col_count, template <typename> class vec,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
double bench_aabb_sphere(const size_t N, const min::aabbox<T, vec> &world, const std::vector<min::sphere<T, vec>> &spheres)
{
    // Running aabb_sphere test
    std::cout << "aabb_sphere: Starting benchmark with " << N << " insertions" << std::endl;

    // Start the time clock
    const auto start = std::chrono::high_resolution_clock::now();

    // Create the spatial data structure
    spatial<T, uint16_t, uint32_t, vec, min::aabbox, min::sphere> g(world);

    // Insert into grid
    g.insert(spheres);

    // Get all colliding objects
    std::vector<std::pair<uint16_t, uint16_t>> collisions = g.get_collisions();

    // Calculate the difference between start and end
    const auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Report collisions found
    std::cout << "aabb_sphere: Collisions found: " << collisions.size() << std::endl;

    if (collisions.size() != col_count)
    {
        std::cout << "aabb_sphere: Failed benchmark, wrong collision count" << std::endl;
    }

    // Print the execution time
    const double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "aabb_sphere: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}

template <typename T, size_t col_count, template <typename> class vec,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
double bench_sphere_aabb(const size_t N, const min::sphere<T, vec> &world, const std::vector<min::aabbox<T, vec>> &boxes)
{
    // Running sphere_aabbox test
    std::cout << "sphere_aabb: Starting benchmark with " << N << " insertions" << std::endl;

    // Start the time clock
    const auto start = std::chrono::high_resolution_clock::now();

    // Create the spatial data structure
    spatial<T, uint16_t, uint32_t, vec, min::sphere, min::aabbox> g(world);

    // Insert into grid
    g.insert(boxes);

    // Get all colliding objects
    std::vector<std::pair<uint16_t, uint16_t>> collisions = g.get_collisions();

    // Calculate the difference between start and end
    const auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Report collisions found
    std::cout << "sphere_aabb: Collisions found: " << collisions.size() << std::endl;

    if (collisions.size() != col_count)
    {
        std::cout << "sphere_aabb: Failed benchmark, wrong collision count" << std::endl;
    }

    // Print the execution time
    const double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "sphere_aabb: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}

template <typename T, size_t col_count, template <typename> class vec,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
double bench_sphere_oobb(const size_t N, const min::sphere<T, vec> &world, const std::vector<min::oobbox<T, vec>> &boxes)
{
    // Running sphere_oobb test
    std::cout << "sphere_oobb: Starting benchmark with " << N << " insertions" << std::endl;

    // Start the time clock
    const auto start = std::chrono::high_resolution_clock::now();

    // Create the spatial data structure
    spatial<T, uint16_t, uint32_t, vec, min::sphere, min::oobbox> g(world);

    // Insert into grid
    g.insert(boxes);

    // Get all colliding objects
    std::vector<std::pair<uint16_t, uint16_t>> collisions = g.get_collisions();

    // Calculate the difference between start and end
    const auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Report collisions found
    std::cout << "sphere_oobb: Collisions found: " << collisions.size() << std::endl;

    if (collisions.size() != col_count)
    {
        std::cout << "sphere_oobb: Failed benchmark, wrong collision count" << std::endl;
    }

    // Print the execution time
    const double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "sphere_oobb: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}

template <typename T, size_t col_count, template <typename> class vec,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
double bench_sphere_sphere(const size_t N, const min::sphere<T, vec> &world, const std::vector<min::sphere<T, vec>> &spheres)
{
    // Running sphere_sphere test
    std::cout << "sphere_sphere: Starting benchmark with " << N << " insertions" << std::endl;

    // Start the time clock
    const auto start = std::chrono::high_resolution_clock::now();

    // Create the spatial data structure
    spatial<T, uint16_t, uint32_t, vec, min::sphere, min::sphere> g(world);

    // Insert into grid
    g.insert(spheres);

    // Get all colliding objects
    std::vector<std::pair<uint16_t, uint16_t>> collisions = g.get_collisions();

    // Calculate the difference between start and end
    const auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Report collisions found
    std::cout << "sphere_sphere: Collisions found: " << collisions.size() << std::endl;

    if (collisions.size() != col_count)
    {
        std::cout << "sphere_sphere: Failed benchmark, wrong collision count" << std::endl;
    }

    // Print the execution time
    const double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "sphere_sphere: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}

template <typename T, size_t col_count, template <typename> class vec,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
double bench_oobb_aabb(const size_t N, const min::oobbox<T, vec> &world, const std::vector<min::aabbox<T, vec>> &boxes)
{
    // Running sphere_aabbox test
    std::cout << "oobb_aabb: Starting benchmark with " << N << " insertions" << std::endl;

    // Start the time clock
    const auto start = std::chrono::high_resolution_clock::now();

    // Create the spatial data structure
    spatial<T, uint16_t, uint32_t, vec, min::oobbox, min::aabbox> g(world);

    // Insert into grid
    g.insert(boxes);

    // Get all colliding objects
    std::vector<std::pair<uint16_t, uint16_t>> collisions = g.get_collisions();

    // Calculate the difference between start and end
    const auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Report collisions found
    std::cout << "oobb_aabb: Collisions found: " << collisions.size() << std::endl;

    if (collisions.size() != col_count)
    {
        std::cout << "oobb_aabb: Failed benchmark, wrong collision count" << std::endl;
    }

    // Print the execution time
    const double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "oobb_aabb: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}

template <typename T, size_t col_count, template <typename> class vec,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
double bench_oobb_oobb(const size_t N, const min::oobbox<T, vec> &world, const std::vector<min::oobbox<T, vec>> &boxes)
{
    // Running sphere_aabbox test
    std::cout << "oobb_oobb: Starting benchmark with " << N << " insertions" << std::endl;

    // Start the time clock
    const auto start = std::chrono::high_resolution_clock::now();

    // Create the spatial data structure
    spatial<T, uint16_t, uint32_t, vec, min::oobbox, min::oobbox> g(world);

    // Insert into grid
    g.insert(boxes);

    // Get all colliding objects
    std::vector<std::pair<uint16_t, uint16_t>> collisions = g.get_collisions();

    // Calculate the difference between start and end
    const auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Report collisions found
    std::cout << "oobb_oobb: Collisions found: " << collisions.size() << std::endl;

    if (collisions.size() != col_count)
    {
        std::cout << "oobb_oobb: Failed benchmark, wrong collision count" << std::endl;
    }

    // Print the execution time
    const double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "oobb_oobb: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}

template <typename T, size_t col_count, template <typename> class vec,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
double bench_oobb_sphere(const size_t N, const min::oobbox<T, vec> &world, const std::vector<min::sphere<T, vec>> &spheres)
{
    // Running oobb_sphere test
    std::cout << "oobb_sphere: Starting benchmark with " << N << " insertions" << std::endl;

    // Start the time clock
    const auto start = std::chrono::high_resolution_clock::now();

    // Create the spatial data structure
    spatial<T, uint16_t, uint32_t, vec, min::oobbox, min::sphere> g(world);

    // Insert into grid
    g.insert(spheres);

    // Get all colliding objects
    std::vector<std::pair<uint16_t, uint16_t>> collisions = g.get_collisions();

    // Calculate the difference between start and end
    const auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Report collisions found
    std::cout << "oobb_sphere: Collisions found: " << collisions.size() << std::endl;

    if (collisions.size() != col_count)
    {
        std::cout << "oobb_sphere: Failed benchmark, wrong collision count" << std::endl;
    }

    // Print the execution time
    const double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "oobb_sphere: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}
#endif
