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
#ifndef __BENCHPHYSICS__
#define __BENCHPHYSICS__

#include <chrono>
#include <iostream>
#include <min/aabbox.h>
#include <min/grid.h>
#include <min/physics.h>
#include <min/sphere.h>
#include <min/tree.h>
#include <random>
#include <stdexcept>

template <typename T, template <typename> class vec,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
double bench_physics_aabb_aabb(const size_t N, const min::aabbox<T, vec> &world, const std::vector<min::aabbox<T, vec>> &boxes)
{
    // Running aabb_aabbox test
    std::cout << "physics_aabb_aabb: Starting benchmark with " << N << " 2-body collisions" << std::endl;

    // Start the time clock
    auto start = std::chrono::high_resolution_clock::now();

    // Create simulation
    vec<T> gravity = vec<T>::up() * -10.0;
    min::physics<T, uint16_t, uint32_t, vec, min::aabbox, min::aabbox, spatial> simulation(world, gravity);
    simulation.reserve(N);

    // Create 'N' random cubic sphere's
    for (const auto &b : boxes)
    {
        simulation.add_body(b, 100.0);
    }

    // Solve simulation step
    simulation.solve(0.001, 0.01);

    // Calculate the difference between start and end
    auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Print the execution time
    double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "physics_aabb_aabb: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}

template <typename T, template <typename> class vec,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
double bench_physics_aabb_sphere(const size_t N, const min::aabbox<T, vec> &world, const std::vector<min::sphere<T, vec>> &spheres)
{
    // Running aabb_sphere test
    std::cout << "physics_aabb_sphere: Starting benchmark with " << N << " 2-body collisions" << std::endl;

    // Start the time clock
    auto start = std::chrono::high_resolution_clock::now();

    // Create simulation
    vec<T> gravity = vec<T>::up() * -10.0;
    min::physics<T, uint16_t, uint32_t, vec, min::aabbox, min::sphere, spatial> simulation(world, gravity);
    simulation.reserve(N);

    // Create 'N' random cubic sphere's
    for (const auto &s : spheres)
    {
        simulation.add_body(s, 100.0);
    }

    // Solve simulation step
    simulation.solve(0.001, 0.01);

    // Calculate the difference between start and end
    auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Print the execution time
    double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "physics_aabb_sphere: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}

template <typename T, template <typename> class vec,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
double bench_physics_sphere_aabb(const size_t N, const min::sphere<T, vec> &world, const std::vector<min::aabbox<T, vec>> &boxes)
{
    // Running sphere_aabbox test
    std::cout << "physics_sphere_aabb: Starting benchmark with " << N << " 2-body collisions" << std::endl;

    // Start the time clock
    auto start = std::chrono::high_resolution_clock::now();

    // Create simulation
    vec<T> gravity = vec<T>::up() * -10.0;
    min::physics<T, uint16_t, uint32_t, vec, min::sphere, min::aabbox, spatial> simulation(world, gravity);
    simulation.reserve(N);

    // Create 'N' random cubic sphere's
    for (const auto &b : boxes)
    {
        simulation.add_body(b, 100.0);
    }

    // Solve simulation step
    simulation.solve(0.001, 0.01);

    // Calculate the difference between start and end
    auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Print the execution time
    double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "physics_sphere_aabb: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}

template <typename T, template <typename> class vec,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
double bench_physics_sphere_sphere(const size_t N, const min::sphere<T, vec> &world, const std::vector<min::sphere<T, vec>> &spheres)
{
    // Running sphere_sphere test
    std::cout << "physics_sphere_sphere: Starting benchmark with " << N << " 2-body collisions" << std::endl;

    // Start the time clock
    auto start = std::chrono::high_resolution_clock::now();

    // Create simulation
    vec<T> gravity = vec<T>::up() * -10.0;
    min::physics<T, uint16_t, uint32_t, vec, min::sphere, min::sphere, spatial> simulation(world, gravity);
    simulation.reserve(N);

    // Create 'N' random cubic sphere's
    for (const auto &s : spheres)
    {
        simulation.add_body(s, 100.0);
    }

    // Solve simulation step
    simulation.solve(0.001, 0.01);

    // Calculate the difference between start and end
    auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Print the execution time
    double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "physics_sphere_sphere: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}

#endif