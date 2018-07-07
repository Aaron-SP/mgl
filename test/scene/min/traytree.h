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
#ifndef __TESTRAYTREE__
#define __TESTRAYTREE__

#include <min/aabbox.h>
#include <min/ray.h>
#include <min/sphere.h>
#include <min/test.h>
#include <min/tree.h>
#include <min/vec3.h>
#include <stdexcept>

bool test_ray_tree()
{
    bool out = true;

    // vec2 sphere tree
    {
        // Local variables
        min::vec2<double> minW(-100000.0, -100000.0);
        min::vec2<double> maxW(100000.0, 100000.0);
        min::vec2<double> origin;
        min::sphere<double, min::vec2> world(minW, maxW);
        min::tree<double, uint_fast16_t, uint_fast32_t, min::vec2, min::sphere, min::sphere> g(world);

        // Number of spheres to make
        const size_t N = 100;
        std::vector<min::sphere<double, min::vec2>> items;
        items.reserve(N);

        // Create spheres for test
        // Local variables
        const double low = -100000.0;
        const double high = 100000.0;
        double current = low;

        // Create 'N' random cubic sphere's
        for (size_t i = 0; i < N; i++)
        {
            // Update current placement
            current += 10.0 * (i + 1);

            // Calculate sphere center and extent
            min::vec2<double> center = min::vec2<double>().set_all(current);
            const double radius = 4.5 * (i + 1);

            // Create the sphere
            items.emplace_back(center, radius);
        }

        // Insert the spheres into the tree
        g.insert(items);

        // Shoot a bunch of rays
        for (size_t i = 0; i < N; i++)
        {
            // Shoot from positive Y
            {
                // Create ray from origin to shape
                min::vec2<double> shoot_from = items[i].get_center();
                shoot_from.x(low + 1.0);
                shoot_from.y(high - 1.0);
                min::ray<double, min::vec2> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec2<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed sphere tree vec2 ray pos-y failed");
                }
            }
            // Shoot from negative Y
            {
                // Create ray from origin to shape
                min::vec2<double> shoot_from = items[i].get_center();
                shoot_from.x(high - 1.0);
                shoot_from.y(low + 1.0);
                min::ray<double, min::vec2> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec2<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed sphere tree vec2 ray neg-y failed");
                }
            }
        }
    }

    // vec2 aabb tree
    {
        // Local variables
        min::vec2<double> minW(-100000.0, -100000.0);
        min::vec2<double> maxW(100000.0, 100000.0);
        min::vec2<double> origin;
        min::aabbox<double, min::vec2> world(minW, maxW);
        min::tree<double, uint_fast16_t, uint_fast32_t, min::vec2, min::aabbox, min::aabbox> g(world);

        // Number of aabboxs to make
        const size_t N = 100;
        std::vector<min::aabbox<double, min::vec2>> items;
        items.reserve(N);

        // Create aabboxs for test
        // Local variables
        const double low = -100000.0;
        const double high = 100000.0;
        double current = low;

        // Create 'N' random cubic aabbox's
        for (size_t i = 0; i < N; i++)
        {
            // Update current placement
            current += 10.0 * (i + 1);

            // Calculate AABB center and extent
            min::vec2<double> center = min::vec2<double>().set_all(current);
            const double extent = 4.5 * (i + 1);

            // Create the AABB
            const min::vec2<double> min = center - extent;
            const min::vec2<double> max = center + extent;
            items.emplace_back(min, max);
        }

        // Insert the spheres into the tree
        g.insert(items);

        // Shoot a bunch of rays
        for (size_t i = 0; i < N; i++)
        {
            // Shoot from positive Y
            {
                // Create ray from origin to shape
                min::vec2<double> shoot_from = items[i].get_center();
                shoot_from.x(low + 1.0);
                shoot_from.y(high - 1.0);
                min::ray<double, min::vec2> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec2<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed aabbox tree vec2 ray pos-y failed");
                }
            }
            // Shoot from negative Y
            {
                // Create ray from origin to shape
                min::vec2<double> shoot_from = items[i].get_center();
                shoot_from.x(high - 1.0);
                shoot_from.y(low + 1.0);
                min::ray<double, min::vec2> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec2<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed aabbox tree vec2 ray neg-y failed");
                }
            }
        }
    }

    // vec2 oobb tree
    {
        // Local variables
        min::vec2<double> minW(-100000.0, -100000.0);
        min::vec2<double> maxW(100000.0, 100000.0);
        min::vec2<double> origin;
        min::oobbox<double, min::vec2> world(minW, maxW);
        min::tree<double, uint_fast16_t, uint_fast32_t, min::vec2, min::oobbox, min::oobbox> g(world);

        // Number of oobboxs to make
        const size_t N = 100;
        std::vector<min::oobbox<double, min::vec2>> items;
        items.reserve(N);

        // Create oobboxs for test
        // Local variables
        const double low = -100000.0;
        const double high = 100000.0;
        double current = low;

        // Create 'N' random cubic oobbox's
        for (size_t i = 0; i < N; i++)
        {
            // Update current placement
            current += 10.0 * (i + 1);

            // Calculate oobb center and extent
            min::vec2<double> center = min::vec2<double>().set_all(current);
            const double extent = 4.5 * (i + 1);

            // Create the oobb
            const min::vec2<double> min = center - extent;
            const min::vec2<double> max = center + extent;
            items.emplace_back(min, max);
        }

        // Insert the spheres into the tree
        g.insert(items);

        // Shoot a bunch of rays
        for (size_t i = 0; i < N; i++)
        {
            // Shoot from positive Y
            {
                // Create ray from origin to shape
                min::vec2<double> shoot_from = items[i].get_center();
                shoot_from.x(low + 1.0);
                shoot_from.y(high - 1.0);
                min::ray<double, min::vec2> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec2<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed oobbox tree vec2 ray pos-y failed");
                }
            }
            // Shoot from negative Y
            {
                // Create ray from origin to shape
                min::vec2<double> shoot_from = items[i].get_center();
                shoot_from.x(high - 1.0);
                shoot_from.y(low + 1.0);
                min::ray<double, min::vec2> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec2<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed oobbox tree vec2 ray neg-y failed");
                }
            }
        }
    }

    // vec3 sphere tree
    {
        // Local variables
        min::vec3<double> minW(-100000.0, -100000.0, -100000.0);
        min::vec3<double> maxW(100000.0, 100000.0, 100000.0);
        min::vec3<double> origin;
        min::sphere<double, min::vec3> world(minW, maxW);
        min::tree<double, uint_fast16_t, uint_fast32_t, min::vec3, min::sphere, min::sphere> g(world);

        // Number of spheres to make
        const size_t N = 100;
        std::vector<min::sphere<double, min::vec3>> items;
        items.reserve(N);

        // Create spheres for test
        // Local variables
        const double low = -100000.0;
        const double high = 100000.0;
        double current = low;

        // Create 'N' random cubic sphere's
        for (size_t i = 0; i < N; i++)
        {
            // Update current placement
            current += 10.0 * (i + 1);

            // Calculate sphere center and extent
            min::vec3<double> center = min::vec3<double>().set_all(current);
            const double radius = 4.5 * (i + 1);

            // Create the sphere
            items.emplace_back(center, radius);
        }

        // Insert the spheres into the tree
        g.insert(items);

        // Shoot a bunch of rays
        for (size_t i = 0; i < N; i++)
        {
            // Shoot from positive Y
            {
                // Create ray from origin to shape
                min::vec3<double> shoot_from = items[i].get_center();
                shoot_from.x(low + 1.0);
                shoot_from.y(high - 1.0);
                shoot_from.z(high - 1.0);
                min::ray<double, min::vec3> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec3<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed sphere tree vec3 ray pos-y failed");
                }
            }
            // Shoot from negative Y
            {
                // Create ray from origin to shape
                min::vec3<double> shoot_from = items[i].get_center();
                shoot_from.x(high - 1.0);
                shoot_from.y(low + 1.0);
                shoot_from.z(low + 1.0);
                min::ray<double, min::vec3> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec3<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed sphere tree vec3 ray neg-y failed");
                }
            }
        }
    }

    // vec3 aabb tree
    {
        // Local variables
        min::vec3<double> minW(-100000.0, -100000.0, -100000.0);
        min::vec3<double> maxW(100000.0, 100000.0, 100000.0);
        min::vec3<double> origin;
        min::aabbox<double, min::vec3> world(minW, maxW);
        min::tree<double, uint_fast16_t, uint_fast32_t, min::vec3, min::aabbox, min::aabbox> g(world);

        // Number of aabboxs to make
        const size_t N = 100;
        std::vector<min::aabbox<double, min::vec3>> items;
        items.reserve(N);

        // Create aabboxs for test
        // Local variables
        const double low = -100000.0;
        const double high = 100000.0;
        double current = low;

        // Create 'N' random cubic aabbox's
        for (size_t i = 0; i < N; i++)
        {
            // Update current placement
            current += 10.0 * (i + 1);

            // Calculate AABB center and extent
            min::vec3<double> center = min::vec3<double>().set_all(current);
            const double extent = 4.5 * (i + 1);

            // Create the AABB
            const min::vec3<double> min = center - extent;
            const min::vec3<double> max = center + extent;
            items.emplace_back(min, max);
        }

        // Insert the spheres into the tree
        g.insert(items);

        // Shoot a bunch of rays
        for (size_t i = 0; i < N; i++)
        {
            // Shoot from positive Y
            {
                // Create ray from origin to shape
                min::vec3<double> shoot_from = items[i].get_center();
                shoot_from.x(low + 1.0);
                shoot_from.y(high - 1.0);
                shoot_from.z(high - 1.0);
                min::ray<double, min::vec3> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec3<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed aabbox tree vec3 ray pos-y failed");
                }
            }
            // Shoot from negative Y
            {
                // Create ray from origin to shape
                min::vec3<double> shoot_from = items[i].get_center();
                shoot_from.x(high - 1.0);
                shoot_from.y(low + 1.0);
                shoot_from.z(low + 1.0);
                min::ray<double, min::vec3> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec3<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed aabbox tree vec3 ray neg-y failed");
                }
            }
        }
    }

    // vec3 oobb tree
    {
        // Local variables
        min::vec3<double> minW(-100000.0, -100000.0, -100000.0);
        min::vec3<double> maxW(100000.0, 100000.0, 100000.0);
        min::vec3<double> origin;
        min::oobbox<double, min::vec3> world(minW, maxW);
        min::tree<double, uint_fast16_t, uint_fast32_t, min::vec3, min::oobbox, min::oobbox> g(world);

        // Number of oobboxs to make
        const size_t N = 100;
        std::vector<min::oobbox<double, min::vec3>> items;
        items.reserve(N);

        // Create oobboxs for test
        // Local variables
        const double low = -100000.0;
        const double high = 100000.0;
        double current = low;

        // Create 'N' random cubic oobbox's
        for (size_t i = 0; i < N; i++)
        {
            // Update current placement
            current += 10.0 * (i + 1);

            // Calculate oobb center and extent
            min::vec3<double> center = min::vec3<double>().set_all(current);
            const double extent = 4.5 * (i + 1);

            // Create the oobb
            const min::vec3<double> min = center - extent;
            const min::vec3<double> max = center + extent;
            items.emplace_back(min, max);
        }

        // Insert the spheres into the tree
        g.insert(items);

        // Shoot a bunch of rays
        for (size_t i = 0; i < N; i++)
        {
            // Shoot from positive Y
            {
                // Create ray from origin to shape
                min::vec3<double> shoot_from = items[i].get_center();
                shoot_from.x(low + 1.0);
                shoot_from.y(high - 1.0);
                shoot_from.z(high - 1.0);
                min::ray<double, min::vec3> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec3<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed oobbox tree vec3 ray pos-y failed");
                }
            }
            // Shoot from negative Y
            {
                // Create ray from origin to shape
                min::vec3<double> shoot_from = items[i].get_center();
                shoot_from.x(high - 1.0);
                shoot_from.y(low + 1.0);
                shoot_from.z(low + 1.0);
                min::ray<double, min::vec3> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec3<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed oobbox tree vec3 ray neg-y failed");
                }
            }
        }
    }

    // vec4 sphere tree
    {
        // Local variables
        min::vec4<double> minW(-100000.0, -100000.0, -100000.0, 1.0);
        min::vec4<double> maxW(100000.0, 100000.0, 100000.0, 1.0);
        min::vec4<double> origin;
        min::sphere<double, min::vec4> world(minW, maxW);
        min::tree<double, uint_fast16_t, uint_fast32_t, min::vec4, min::sphere, min::sphere> g(world);

        // Number of spheres to make
        const size_t N = 100;
        std::vector<min::sphere<double, min::vec4>> items;
        items.reserve(N);

        // Create spheres for test
        // Local variables
        const double low = -100000.0;
        const double high = 100000.0;
        double current = low;

        // Create 'N' random cubic sphere's
        for (size_t i = 0; i < N; i++)
        {
            // Update current placement
            current += 10.0 * (i + 1);

            // Calculate sphere center and extent
            min::vec4<double> center = min::vec4<double>().set_all(current);
            const double radius = 4.5 * (i + 1);

            // Create the sphere
            items.emplace_back(center, radius);
        }

        // Insert the spheres into the tree
        g.insert(items);

        // Shoot a bunch of rays
        for (size_t i = 0; i < N; i++)
        {
            // Shoot from positive Y
            {
                // Create ray from origin to shape
                min::vec4<double> shoot_from = items[i].get_center();
                shoot_from.x(low + 1.0);
                shoot_from.y(high - 1.0);
                shoot_from.z(high - 1.0);
                min::ray<double, min::vec4> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec4<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed sphere tree vec4 ray pos-y failed");
                }
            }
            // Shoot from negative Y
            {
                // Create ray from origin to shape
                min::vec4<double> shoot_from = items[i].get_center();
                shoot_from.x(high - 1.0);
                shoot_from.y(low + 1.0);
                shoot_from.z(low + 1.0);
                min::ray<double, min::vec4> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec4<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed sphere tree vec4 ray neg-y failed");
                }
            }
        }
    }

    // vec4 aabb tree
    {
        // Local variables
        min::vec4<double> minW(-100000.0, -100000.0, -100000.0, 1.0);
        min::vec4<double> maxW(100000.0, 100000.0, 100000.0, 1.0);
        min::vec4<double> origin;
        min::aabbox<double, min::vec4> world(minW, maxW);
        min::tree<double, uint_fast16_t, uint_fast32_t, min::vec4, min::aabbox, min::aabbox> g(world);

        // Number of aabboxs to make
        const size_t N = 100;
        std::vector<min::aabbox<double, min::vec4>> items;
        items.reserve(N);

        // Create aabboxs for test
        // Local variables
        const double low = -100000.0;
        const double high = 100000.0;
        double current = low;

        // Create 'N' random cubic aabbox's
        for (size_t i = 0; i < N; i++)
        {
            // Update current placement
            current += 10.0 * (i + 1);

            // Calculate AABB center and extent
            min::vec4<double> center = min::vec4<double>().set_all(current);
            const double extent = 4.5 * (i + 1);

            // Create the AABB
            const min::vec4<double> min = center - extent;
            const min::vec4<double> max = center + extent;
            items.emplace_back(min, max);
        }

        // Insert the spheres into the tree
        g.insert(items);

        // Shoot a bunch of rays
        for (size_t i = 0; i < N; i++)
        {
            // Shoot from positive Y
            {
                // Create ray from origin to shape
                min::vec4<double> shoot_from = items[i].get_center();
                shoot_from.x(low + 1.0);
                shoot_from.y(high - 1.0);
                shoot_from.z(high - 1.0);
                min::ray<double, min::vec4> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec4<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed aabbox tree vec4 ray pos-y failed");
                }
            }
            // Shoot from negative Y
            {
                // Create ray from origin to shape
                min::vec4<double> shoot_from = items[i].get_center();
                shoot_from.x(high - 1.0);
                shoot_from.y(low + 1.0);
                shoot_from.z(low + 1.0);
                min::ray<double, min::vec4> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec4<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed aabbox tree vec4 ray neg-y failed");
                }
            }
        }
    }

    // vec4 oobb tree
    {
        // Local variables
        min::vec4<double> minW(-100000.0, -100000.0, -100000.0, 1.0);
        min::vec4<double> maxW(100000.0, 100000.0, 100000.0, 1.0);
        min::vec4<double> origin;
        min::oobbox<double, min::vec4> world(minW, maxW);
        min::tree<double, uint_fast16_t, uint_fast32_t, min::vec4, min::oobbox, min::oobbox> g(world);

        // Number of oobboxs to make
        const size_t N = 100;
        std::vector<min::oobbox<double, min::vec4>> items;
        items.reserve(N);

        // Create oobboxs for test
        // Local variables
        const double low = -100000.0;
        const double high = 100000.0;
        double current = low;

        // Create 'N' random cubic oobbox's
        for (size_t i = 0; i < N; i++)
        {
            // Update current placement
            current += 10.0 * (i + 1);

            // Calculate oobb center and extent
            min::vec4<double> center = min::vec4<double>().set_all(current);
            const double extent = 4.5 * (i + 1);

            // Create the oobb
            const min::vec4<double> min = center - extent;
            const min::vec4<double> max = center + extent;
            items.emplace_back(min, max);
        }

        // Insert the spheres into the tree
        g.insert(items);

        // Shoot a bunch of rays
        for (size_t i = 0; i < N; i++)
        {
            // Shoot from positive Y
            {
                // Create ray from origin to shape
                min::vec4<double> shoot_from = items[i].get_center();
                shoot_from.x(low + 1.0);
                shoot_from.y(high - 1.0);
                shoot_from.z(high - 1.0);
                min::ray<double, min::vec4> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec4<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed oobbox tree vec4 ray pos-y failed");
                }
            }
            // Shoot from negative Y
            {
                // Create ray from origin to shape
                min::vec4<double> shoot_from = items[i].get_center();
                shoot_from.x(high - 1.0);
                shoot_from.y(low + 1.0);
                shoot_from.z(low + 1.0);
                min::ray<double, min::vec4> r(shoot_from, items[i].get_center());

                // Get collisions with ray
                const std::vector<std::pair<uint_fast16_t, min::vec4<double>>> &collisions = g.get_collisions(r);

                // Test if we got a collision with ray
                out = out && (collisions.size() == 1);
                if (!out)
                {
                    throw std::runtime_error("Failed oobbox tree vec4 ray neg-y failed");
                }
            }
        }
    }

    return out;
}

#endif
