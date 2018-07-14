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
#ifndef __TESTSPHERETREE__
#define __TESTSPHERETREE__

#include <min/sphere.h>
#include <min/test.h>
#include <min/tree.h>
#include <min/vec3.h>
#include <stdexcept>

bool test_sphere_tree()
{
    bool out = true;

    // vec2 tree
    {
        // Local variables
        min::vec2<double> minW(-10.0, -10.0);
        min::vec2<double> maxW(10.0, 10.0);
        min::vec2<double> min;
        min::vec2<double> max;
        min::vec2<double> p;
        double r;
        std::vector<uint_fast16_t> hits;
        std::vector<std::pair<uint_fast16_t, uint_fast16_t>> collisions;
        min::sphere<double, min::vec2> world(minW, maxW);
        std::vector<min::sphere<double, min::vec2>> items;
        min::tree<double, uint_fast16_t, uint_fast32_t, min::vec2, min::sphere, min::sphere> t(world);

        // Create three boxes and insert into vector

        // Box A
        min = min::vec2<double>(-1.0, -1.0);
        max = min::vec2<double>(1.0, 1.0);
        items.push_back(min::sphere<double, min::vec2>(min, max));

        // Box B
        min = min::vec2<double>(-2.0, -2.0);
        max = min::vec2<double>(2.0, 2.0);
        items.push_back(min::sphere<double, min::vec2>(min, max));

        // Box C
        min = min::vec2<double>(-3.0, -3.0);
        max = min::vec2<double>(3.0, 3.0);
        items.push_back(min::sphere<double, min::vec2>(min, max));

        t.insert(items);

        // Maximum extent is sqrt(2*6^2) = sqrt(72) = 8.485 - Box A
        // Extent depth 2 is sqrt(800)/2/2 = 7.071
        int depth = t.get_depth();
        out = out && compare(2, depth);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec2 optimum depth");
        }

        // Insert into tree twice, should reset and rebuild
        t.insert(items, 5);

        // Test point inside
        p = min::vec2<double>(2.9, 2.9);
        hits = t.point_inside(p);
        out = out && compare(1, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec2 point_inside 1 hit");
        }

        // Test get_cell center
        auto *node = &t.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(3.75, p.x(), 1E-4);
        out = out && compare(3.75, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec2 get_cell 1 get_center");
        }

        // Check the cell extents
        // Extent max depth 5, depth 3 should be 20/2/2/2 = 2.5
        p = node->get_cell().get_extent();
        out = out && compare(2.5, p.x(), 1E-4);
        out = out && compare(2.5, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec2 get_cell 1 cell_extents");
        }

        // Test get radius; r = sqrt(x^2 + y^2) / 2 = 1.76777
        r = node->get_cell().get_radius();
        out = out && compare(1.7677, r, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec2 get_cell radius");
        }

        // Test point inside
        p = min::vec2<double>(1.9, 1.9);
        hits = t.point_inside(p);
        out = out && compare(2, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec2 point_inside 2 hit");
        }

        // Test get leaf center
        node = &t.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(2.1875, p.x(), 1E-4);
        out = out && compare(2.1875, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec2 get_cell 2 get_center");
        }

        // Test point inside
        p = min::vec2<double>(0.9, 0.9);
        hits = t.point_inside(p);
        out = out && compare(3, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec2 point_inside 3 hit");
        }

        // Test get leaf center
        node = &t.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(0.9375, p.x(), 1E-4);
        out = out && compare(0.9375, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec2 get_cell 3 get_center");
        }

        // Test get collisions
        // A int B and B int C and A int C
        collisions = t.get_collisions();
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec2 get collisions");
        }

        // Test get collisions
        // B int C
        p = min::vec2<double>(1.9, 1.9);
        collisions = t.get_collisions(p);
        out = out && compare(1, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec2 get collision point");
        }

        // Test overlap entire world
        collisions = t.get_overlap(world);
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec2 get overlap 1");
        }

        // Box D
        min = min::vec2<double>(-7.0, -7.0);
        max = min::vec2<double>(-4.0, -4.0);
        items.push_back(min::sphere<double, min::vec2>(min, max));

        // Insert into tree
        t.insert(items, 5);

        // Test overlap upper right quadrant, center 5.0, 5.0
        min = min::vec2<double>(0.0, 0.0);
        max = min::vec2<double>(10.0, 10.0);
        collisions = t.get_overlap(min::sphere<double, min::vec2>(min, max));
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec2 get overlap 2");
        }

        // Test overlap box same cell
        min = min::vec2<double>(1.0, 1.0);
        max = min::vec2<double>(1.0, 1.0);
        collisions = t.get_overlap(min::sphere<double, min::vec2>(min, max));
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec2 get overlap 3");
        }
    }

    // vec3 tree
    {
        // Local variables
        min::vec3<double> minW(-10.0, -10.0, -10.0);
        min::vec3<double> maxW(10.0, 10.0, 10.0);
        min::vec3<double> min;
        min::vec3<double> max;
        min::vec3<double> p;
        std::vector<uint_fast16_t> hits;
        std::vector<std::pair<uint_fast16_t, uint_fast16_t>> collisions;
        double r;
        min::sphere<double, min::vec3> world(minW, maxW);
        std::vector<min::sphere<double, min::vec3>> items;
        min::tree<double, uint_fast16_t, uint_fast32_t, min::vec3, min::sphere, min::sphere> t(world);

        // Create three boxes and insert into vector

        // Box A
        min = min::vec3<double>(-1.0, -1.0, -1.0);
        max = min::vec3<double>(1.0, 1.0, 1.0);
        items.push_back(min::sphere<double, min::vec3>(min, max));

        // Box B
        min = min::vec3<double>(-2.0, -2.0, -2.0);
        max = min::vec3<double>(2.0, 2.0, 2.0);
        items.push_back(min::sphere<double, min::vec3>(min, max));

        // Box C
        min = min::vec3<double>(-3.0, -3.0, -3.0);
        max = min::vec3<double>(3.0, 3.0, 3.0);
        items.push_back(min::sphere<double, min::vec3>(min, max));

        t.insert(items);

        // Maximum extent is sqrt(3*6^2) = sqrt(108) = 10.392 - Box A
        // Extent depth 2 is sqrt(1200)/2/2 = 8.660
        int depth = t.get_depth();
        out = out && compare(2, depth);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec3 optimum depth");
        }

        // Insert into tree twice, should reset and rebuild
        t.insert(items, 5);

        // Test point inside
        p = min::vec3<double>(2.9, 2.9, 2.9);
        hits = t.point_inside(p);
        out = out && compare(1, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec3 point_inside 1 hit");
        }

        // Test get_cell center
        auto *node = &t.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(3.75, p.x(), 1E-4);
        out = out && compare(3.75, p.y(), 1E-4);
        out = out && compare(3.75, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec3 get_cell 1 get_center");
        }

        // Check the cell extents
        // Extent max depth 5, depth 3 should be 20/2/2/2 = 2.5
        p = node->get_cell().get_extent();
        out = out && compare(2.5, p.x(), 1E-4);
        out = out && compare(2.5, p.y(), 1E-4);
        out = out && compare(2.5, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec3 get_cell 1 cell_extents");
        }

        // Check radius r = sqrt(x^2 + y^2 + z^2) / 2 = 2.1650
        r = node->get_cell().get_radius();
        out = out && compare(2.1650, r, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec3 get_cell radius");
        }

        // Test point inside
        p = min::vec3<double>(1.9, 1.9, 1.9);
        hits = t.point_inside(p);
        out = out && compare(2, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec3 point_inside 2 hit");
        }

        // Test get leaf center
        node = &t.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(2.1875, p.x(), 1E-4);
        out = out && compare(2.1875, p.y(), 1E-4);
        out = out && compare(2.1875, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec3 get_cell 2 get_center");
        }

        // Test point inside
        p = min::vec3<double>(0.9, 0.9, 0.9);
        hits = t.point_inside(p);
        out = out && compare(3, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec3 point_inside 3 hit");
        }

        // Test get leaf center
        node = &t.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(0.9375, p.x(), 1E-4);
        out = out && compare(0.9375, p.y(), 1E-4);
        out = out && compare(0.9375, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec3 get_cell 3 get_center");
        }

        // Test get collisions
        // A int B and B int C and A int C
        collisions = t.get_collisions();
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec3 get collisions");
        }

        // Test get collisions
        // B int C
        p = min::vec3<double>(1.9, 1.9, 1.9);
        collisions = t.get_collisions(p);
        out = out && compare(1, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec3 get collision point");
        }

        // Test overlap entire world
        collisions = t.get_overlap(world);
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec3 get overlap 1");
        }

        // Box D
        min = min::vec3<double>(-7.0, -7.0, -7.0);
        max = min::vec3<double>(-4.0, -4.0, -4.0);
        items.push_back(min::sphere<double, min::vec3>(min, max));

        // Insert into tree
        t.insert(items, 5);

        // Test overlap upper right quadrant, center 5.0, 5.0
        min = min::vec3<double>(0.0, 0.0, 0.0);
        max = min::vec3<double>(10.0, 10.0, 10.0);
        collisions = t.get_overlap(min::sphere<double, min::vec3>(min, max));
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec3 get overlap 2");
        }

        // Test overlap box same cell
        min = min::vec3<double>(1.0, 1.0, 1.0);
        max = min::vec3<double>(1.0, 1.0, 1.0);
        collisions = t.get_overlap(min::sphere<double, min::vec3>(min, max));
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec3 get overlap 3");
        }
    }

    // vec4 tree
    {
        // Local variables
        min::vec4<double> minW(-10.0, -10.0, -10.0, 0.0);
        min::vec4<double> maxW(10.0, 10.0, 10.0, 0.0);
        min::vec4<double> min;
        min::vec4<double> max;
        min::vec4<double> p;
        std::vector<uint_fast16_t> hits;
        std::vector<std::pair<uint_fast16_t, uint_fast16_t>> collisions;
        double r;
        min::sphere<double, min::vec4> world(minW, maxW);
        std::vector<min::sphere<double, min::vec4>> items;
        min::tree<double, uint_fast16_t, uint_fast32_t, min::vec4, min::sphere, min::sphere> t(world);

        // Create three boxes and insert into vector

        // Box A
        min = min::vec4<double>(-1.0, -1.0, -1.0, 0.0);
        max = min::vec4<double>(1.0, 1.0, 1.0, 0.0);
        items.push_back(min::sphere<double, min::vec4>(min, max));

        // Box B
        min = min::vec4<double>(-2.0, -2.0, -2.0, 0.0);
        max = min::vec4<double>(2.0, 2.0, 2.0, 0.0);
        items.push_back(min::sphere<double, min::vec4>(min, max));

        // Box C
        min = min::vec4<double>(-3.0, -3.0, -3.0, 0.0);
        max = min::vec4<double>(3.0, 3.0, 3.0, 0.0);
        items.push_back(min::sphere<double, min::vec4>(min, max));

        t.insert(std::move(items));

        // Maximum extent is sqrt(3*6^2) = sqrt(108) = 10.392 - Box A
        // Extent depth 2 is sqrt(1200)/2/2 = 8.660
        int depth = t.get_depth();
        out = out && compare(2, depth);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec4 optimum depth");
        }

        // Insert into tree twice, should reset and rebuild
        t.insert(items, 5);

        // Test point inside
        p = min::vec4<double>(2.9, 2.9, 2.9, 1.0);
        hits = t.point_inside(p);
        out = out && compare(1, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec4 point_inside 1 hit");
        }

        // Test get_cell center
        auto *node = &t.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(3.75, p.x(), 1E-4);
        out = out && compare(3.75, p.y(), 1E-4);
        out = out && compare(3.75, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec4 get_cell 1 get_center");
        }

        // Check the cell extents
        // Extent max depth 5, depth 3 should be 20/2/2/2 = 2.5
        p = node->get_cell().get_extent();
        out = out && compare(2.5, p.x(), 1E-4);
        out = out && compare(2.5, p.y(), 1E-4);
        out = out && compare(2.5, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec4 get_cell 1 cell_extents");
        }

        // Check radius r = sqrt(x^2 + y^2 + z^2) / 2 = 2.16507
        r = node->get_cell().get_radius();
        out = out && compare(2.1650, r, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec4 get_cell radius");
        }

        // Test point inside
        p = min::vec4<double>(1.9, 1.9, 1.9, 1.0);
        hits = t.point_inside(p);
        out = out && compare(2, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec4 point_inside 2 hit");
        }

        // Test get leaf center
        node = &t.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(2.1875, p.x(), 1E-4);
        out = out && compare(2.1875, p.y(), 1E-4);
        out = out && compare(2.1875, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec4 get_cell 2 get_center");
        }

        // Test point inside
        p = min::vec4<double>(0.9, 0.9, 0.9, 1.0);
        hits = t.point_inside(p);
        out = out && compare(3, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec4 point_inside 3 hit");
        }

        // Test get leaf center
        node = &t.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(0.9375, p.x(), 1E-4);
        out = out && compare(0.9375, p.y(), 1E-4);
        out = out && compare(0.9375, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec4 get_cell 3 get_center");
        }

        // Test get collisions
        // A int B and B int C and A int C
        collisions = t.get_collisions();
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec4 get collisions");
        }

        // Test get collisions
        // B int C
        p = min::vec4<double>(1.9, 1.9, 1.9, 1.0);
        collisions = t.get_collisions(p);
        out = out && compare(1, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec4 get collision point");
        }

        // Test overlap entire world
        collisions = t.get_overlap(world);
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec4 get overlap 1");
        }

        // Box D
        min = min::vec4<double>(-7.0, -7.0, -7.0, 1.0);
        max = min::vec4<double>(-4.0, -4.0, -4.0, 1.0);
        items.push_back(min::sphere<double, min::vec4>(min, max));

        // Insert into tree
        t.insert(items, 5);

        // Test overlap upper right quadrant, center 5.0, 5.0
        min = min::vec4<double>(0.0, 0.0, 0.0, 1.0);
        max = min::vec4<double>(10.0, 10.0, 10.0, 1.0);
        collisions = t.get_overlap(min::sphere<double, min::vec4>(min, max));
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec4 get overlap 2");
        }

        // Test overlap box same cell
        min = min::vec4<double>(1.0, 1.0, 1.0, 1.0);
        max = min::vec4<double>(1.0, 1.0, 1.0, 1.0);
        collisions = t.get_overlap(min::sphere<double, min::vec4>(min, max));
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed sphere tree vec4 get overlap 3");
        }
    }
    return out;
}

#endif
