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
#ifndef _MGL_BENCHRAY_MGL_
#define _MGL_BENCHRAY_MGL_

#include <chrono>
#include <iostream>
#include <min/ray.h>
#include <min/sphere.h>
#include <stdexcept>

template <typename T, template <typename> class vec,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
double bench_ray_aabb(const size_t N, const min::aabbox<T, vec> &world)
{
    // Running ray_aabbox test
    std::cout << "ray_aabb: Starting benchmark with " << N * 4 << " ray collisions" << std::endl;

    // Start the time clock
    const auto start = std::chrono::high_resolution_clock::now();

    // Create spatial structure
    spatial<T, uint_fast16_t, uint_fast32_t, vec, min::aabbox, min::aabbox> g(world);

    // Number of aabboxs to make
    std::vector<min::aabbox<T, vec>> items;
    items.reserve(N);

    // Create aabboxs for test
    // Local variables
    const vec<T> &min = world.get_min();
    const vec<T> &max = world.get_max();
    const vec<T> dir = (max - min).normalize();
    const T radius = 0.9f;

    // We do this for precision purposes
    // Shooting rays from huge distances can result in errors
    const T ray_offset = 1000.0f;
    const T angle = 0.01f;

    // Create 'N' random cubic aabbox's
    for (size_t i = 0; i < N; i++)
    {
        // Calculate aabbox center and extent
        const T step = (i + 1) * 10.0f;
        const vec<T> center = min + (dir * step);

        // Create the aabbox
        const vec<T> bmin = center - radius;
        const vec<T> bmax = center + radius;
        items.emplace_back(bmin, bmax);
    }

    // Insert the aabboxs into the grid
    g.insert(items);

    // Shoot a bunch of rays
    for (size_t i = 0; i < N; i++)
    {
        // Shoot X axis aligned
        {
            // Create ray from origin to shape
            vec<T> shoot_from = items[i].get_center();
            shoot_from.x(shoot_from.x() + ray_offset);
            min::ray<T, vec> r(shoot_from, items[i].get_center());

            // Get collisions with ray
            const std::vector<std::pair<uint_fast16_t, vec<T>>> &collisions = g.get_collisions(r);

            // Test if we got a collision with ray
            if (collisions.size() != 1)
            {
                throw std::runtime_error("ray_aabb: Failed bench_ray_aabbox x-axis benchmark");
            }
        }
        // Shoot X axis with dynamic angle
        {
            // Create ray from origin to shape
            vec<T> shoot_from = items[i].get_center();
            shoot_from.x(shoot_from.x() + ray_offset);
            shoot_from.y(shoot_from.y() + angle * i);
            min::ray<T, vec> r(shoot_from, items[i].get_center());

            // Get collisions with ray
            const std::vector<std::pair<uint_fast16_t, vec<T>>> &collisions = g.get_collisions(r);

            // Test if we got a collision with ray
            if (collisions.size() != 1)
            {
                throw std::runtime_error("ray_aabb: Failed bench_ray_aabbox x-axis dynamic benchmark");
            }
        }
        // Shoot Y axis aligned
        {
            // Create ray from origin to shape
            vec<T> shoot_from = items[i].get_center();
            shoot_from.y(shoot_from.y() + ray_offset);
            min::ray<T, vec> r(shoot_from, items[i].get_center());

            // Get collisions with ray
            const std::vector<std::pair<uint_fast16_t, vec<T>>> &collisions = g.get_collisions(r);

            // Test if we got a collision with ray
            if (collisions.size() != 1)
            {
                throw std::runtime_error("ray_aabb: Failed bench_ray_aabbox y-axis benchmark");
            }
        }
        // Shoot Y axis aligned
        {
            // Create ray from origin to shape
            vec<T> shoot_from = items[i].get_center();
            shoot_from.x(shoot_from.x() + angle * i);
            shoot_from.y(shoot_from.y() + ray_offset);
            min::ray<T, vec> r(shoot_from, items[i].get_center());

            // Get collisions with ray
            const std::vector<std::pair<uint_fast16_t, vec<T>>> &collisions = g.get_collisions(r);

            // Test if we got a collision with ray
            if (collisions.size() != 1)
            {
                throw std::runtime_error("ray_aabb: Failed bench_ray_aabbox y-axis dynamic benchmark");
            }
        }
    }

    // Calculate the difference between start and end
    const auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Print the execution time
    const double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "ray_aabb: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}

template <typename T, template <typename> class vec,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
double bench_ray_oobb(const size_t N, const min::oobbox<T, vec> &world)
{
    // Running ray_oobbox test
    std::cout << "ray_oobb: Starting benchmark with " << N * 4 << " ray collisions" << std::endl;

    // Start the time clock
    const auto start = std::chrono::high_resolution_clock::now();

    // Create spatial structure
    spatial<T, uint_fast16_t, uint_fast32_t, vec, min::oobbox, min::oobbox> g(world);

    // Number of oobboxs to make
    std::vector<min::oobbox<T, vec>> items;
    items.reserve(N);

    // Create oobboxs for test
    // Local variables
    const vec<T> &min = world.get_min();
    const vec<T> &max = world.get_max();
    const vec<T> dir = (max - min).normalize();
    const T radius = 0.9f;

    // We do this for precision purposes
    // Shooting rays from huge distances can result in errors
    const T ray_offset = 1000.0f;
    const T angle = 0.01f;

    // Create 'N' random cubic oobbox's
    for (size_t i = 0; i < N; i++)
    {
        // Calculate oobbox center and extent
        const T step = (i + 1) * 10.0f;
        const vec<T> center = min + (dir * step);

        // Create the oobbox
        const vec<T> bmin = center - radius;
        const vec<T> bmax = center + radius;
        items.emplace_back(bmin, bmax);
    }

    // Insert the oobboxs into the grid
    g.insert(items);

    // Shoot a bunch of rays
    for (size_t i = 0; i < N; i++)
    {
        // Shoot X axis aligned
        {
            // Create ray from origin to shape
            vec<T> shoot_from = items[i].get_center();
            shoot_from.x(shoot_from.x() + ray_offset);
            min::ray<T, vec> r(shoot_from, items[i].get_center());

            // Get collisions with ray
            const std::vector<std::pair<uint_fast16_t, vec<T>>> &collisions = g.get_collisions(r);

            // Test if we got a collision with ray
            if (collisions.size() != 1)
            {
                throw std::runtime_error("ray_oobb: Failed bench_ray_oobbox x-axis benchmark");
            }
        }
        // Shoot X axis with dynamic angle
        {
            // Create ray from origin to shape
            vec<T> shoot_from = items[i].get_center();
            shoot_from.x(shoot_from.x() + ray_offset);
            shoot_from.y(shoot_from.y() + angle * i);
            min::ray<T, vec> r(shoot_from, items[i].get_center());

            // Get collisions with ray
            const std::vector<std::pair<uint_fast16_t, vec<T>>> &collisions = g.get_collisions(r);

            // Test if we got a collision with ray
            if (collisions.size() != 1)
            {
                throw std::runtime_error("ray_oobb: Failed bench_ray_oobbox x-axis dynamic benchmark");
            }
        }
        // Shoot Y axis aligned
        {
            // Create ray from origin to shape
            vec<T> shoot_from = items[i].get_center();
            shoot_from.y(shoot_from.y() + ray_offset);
            min::ray<T, vec> r(shoot_from, items[i].get_center());

            // Get collisions with ray
            const std::vector<std::pair<uint_fast16_t, vec<T>>> &collisions = g.get_collisions(r);

            // Test if we got a collision with ray
            if (collisions.size() != 1)
            {
                throw std::runtime_error("ray_oobb: Failed bench_ray_oobbox y-axis benchmark");
            }
        }
        // Shoot Y axis aligned
        {
            // Create ray from origin to shape
            vec<T> shoot_from = items[i].get_center();
            shoot_from.x(shoot_from.x() + angle * i);
            shoot_from.y(shoot_from.y() + ray_offset);
            min::ray<T, vec> r(shoot_from, items[i].get_center());

            // Get collisions with ray
            const std::vector<std::pair<uint_fast16_t, vec<T>>> &collisions = g.get_collisions(r);

            // Test if we got a collision with ray
            if (collisions.size() != 1)
            {
                throw std::runtime_error("ray_oobb: Failed bench_ray_oobbox y-axis dynamic benchmark");
            }
        }
    }

    // Calculate the difference between start and end
    const auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Print the execution time
    const double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "ray_oobb: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}

template <typename T, template <typename> class vec,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
double bench_ray_sphere(const size_t N, const min::sphere<T, vec> &world)
{
    // Running ray_aabbox test
    std::cout << "ray_sphere: Starting benchmark with " << N * 4 << " ray collisions" << std::endl;

    // Start the time clock
    const auto start = std::chrono::high_resolution_clock::now();

    // Create spatial structure
    spatial<T, uint_fast16_t, uint_fast32_t, vec, min::sphere, min::sphere> g(world);

    // Number of spheres to make
    std::vector<min::sphere<T, vec>> items;
    items.reserve(N);

    // Create spheres for test
    // Local variables
    const vec<T> &min = world.get_min();
    const vec<T> &max = world.get_max();
    const vec<T> dir = (max - min).normalize();
    const T radius = 0.9f;

    // We do this for single float precision purposes
    // Shooting rays from huge distances can result in errors
    const T ray_offset = 1000.0f;
    const T angle = 0.01f;

    // Create 'N' random cubic sphere's
    for (size_t i = 0; i < N; i++)
    {
        // Calculate sphere center and extent
        const T step = (i + 1) * 10.0f;
        const vec<T> center = min + (dir * step);

        // Create the sphere
        items.emplace_back(center, radius);
    }

    // Insert the spheres into the grid
    g.insert(items);

    // Shoot a bunch of rays
    for (size_t i = 0; i < N; i++)
    {
        // Shoot X axis aligned
        {
            // Create ray from origin to shape
            vec<T> shoot_from = items[i].get_center();
            shoot_from.x(shoot_from.x() + ray_offset);
            min::ray<T, vec> r(shoot_from, items[i].get_center());

            // Get collisions with ray
            const std::vector<std::pair<uint_fast16_t, vec<T>>> &collisions = g.get_collisions(r);

            // Test if we got a collision with ray
            if (collisions.size() != 1)
            {
                throw std::runtime_error("ray_sphere: Failed bench_ray_sphere x-axis benchmark");
            }
        }
        // Shoot X axis with dynamic angle
        {
            // Create ray from origin to shape
            vec<T> shoot_from = items[i].get_center();
            shoot_from.x(shoot_from.x() + ray_offset);
            shoot_from.y(shoot_from.y() + angle * i);
            min::ray<T, vec> r(shoot_from, items[i].get_center());

            // Get collisions with ray
            const std::vector<std::pair<uint_fast16_t, vec<T>>> &collisions = g.get_collisions(r);

            // Test if we got a collision with ray
            if (collisions.size() != 1)
            {
                throw std::runtime_error("ray_sphere: Failed bench_ray_sphere x-axis dynamic benchmark");
            }
        }
        // Shoot Y axis aligned
        {
            // Create ray from origin to shape
            vec<T> shoot_from = items[i].get_center();
            shoot_from.y(shoot_from.y() + ray_offset);
            min::ray<T, vec> r(shoot_from, items[i].get_center());

            // Get collisions with ray
            const std::vector<std::pair<uint_fast16_t, vec<T>>> &collisions = g.get_collisions(r);

            // Test if we got a collision with ray
            if (collisions.size() != 1)
            {
                throw std::runtime_error("ray_sphere: Failed bench_ray_sphere y-axis benchmark");
            }
        }
        // Shoot Y axis aligned
        {
            // Create ray from origin to shape
            vec<T> shoot_from = items[i].get_center();
            shoot_from.x(shoot_from.x() + angle * i);
            shoot_from.y(shoot_from.y() + ray_offset);
            min::ray<T, vec> r(shoot_from, items[i].get_center());

            // Get collisions with ray
            const std::vector<std::pair<uint_fast16_t, vec<T>>> &collisions = g.get_collisions(r);

            // Test if we got a collision with ray
            if (collisions.size() != 1)
            {
                throw std::runtime_error("ray_sphere: Failed bench_ray_sphere y-axis dynamic benchmark");
            }
        }
    }

    // Calculate the difference between start and end
    const auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Print the execution time
    const double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "ray_sphere: tests completed in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}
#endif
