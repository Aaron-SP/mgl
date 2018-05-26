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
#ifndef __TESTAABBGRID__
#define __TESTAABBGRID__

#include <min/aabbox.h>
#include <min/grid.h>
#include <min/test.h>
#include <min/vec3.h>
#include <stdexcept>

bool test_aabb_grid()
{
    bool out = true;

    // vec2 grid
    {
        // Local variables
        min::vec2<double> minW(-10.0, -10.0);
        min::vec2<double> maxW(10.0, 10.0);
        min::vec2<double> min;
        min::vec2<double> max;
        min::vec2<double> p;
        std::vector<uint_fast16_t> hits;
        std::vector<std::pair<uint_fast16_t, uint_fast16_t>> collisions;
        min::aabbox<double, min::vec2> world(minW, maxW);
        std::vector<min::aabbox<double, min::vec2>> items;
        min::grid<double, uint_fast16_t, uint_fast32_t, min::vec2, min::aabbox, min::aabbox> g(world);

        // Create three boxes and insert into vector

        // Box A
        min = min::vec2<double>(-1.0, -1.0);
        max = min::vec2<double>(3.0, 4.0);
        items.push_back(min::aabbox<double, min::vec2>(min, max));

        // Box B
        min = min::vec2<double>(-2.0, -2.0);
        max = min::vec2<double>(1.0, 2.0);
        items.push_back(min::aabbox<double, min::vec2>(min, max));

        // Box C
        min = min::vec2<double>(-3.0, -3.0);
        max = min::vec2<double>(4.0, 5.0);
        items.push_back(min::aabbox<double, min::vec2>(min, max));

        g.insert(items);

        // Maximum extent is sqrt(2*6^2) = sqrt(72) = 8.485 - Box A
        // Extent scale 2 is sqrt(800)/2/2 = 7.071; ceil(cbrt(3)) == 2
        int scale = g.get_scale();
        out = out && compare(2, scale);
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec2 optimum scale");
        }

        // Insert into grid twice, should reset and rebuild
        g.insert(items);

        // Test point inside
        p = min::vec2<double>(2.9, 2.9);
        hits = g.point_inside(p);
        out = out && compare(3, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec2 point_inside hit 1");
        }

        // Test get_cell center
        auto *node = &g.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(5.0, p.x(), 1E-4);
        out = out && compare(5.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec2 get_cell 1 get_center");
        }

        // Check the cell extents
        // Extent scale 2 should 20/2 = 10.0
        p = node->get_cell().get_extent();
        out = out && compare(10.0, p.x(), 1E-4);
        out = out && compare(10.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec2 get_cell 1 cell_extents");
        }

        // Test point inside
        p = min::vec2<double>(1.9, 1.9);
        hits = g.point_inside(p);
        out = out && compare(3, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec2 point_inside hit 2");
        }

        // Test get leaf center
        node = &g.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(5.0, p.x(), 1E-4);
        out = out && compare(5.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec2 get_cell 2 get_center");
        }

        // Test point inside
        p = min::vec2<double>(0.9, 0.9);
        hits = g.point_inside(p);
        out = out && compare(3, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec2 point_inside hit 3");
        }

        // Test get leaf center
        node = &g.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(5.0, p.x(), 1E-4);
        out = out && compare(5.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec2 get_cell 3 get_center");
        }

        // Test get collisions
        // A int B and B int C and A int C
        collisions = g.get_collisions();
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec2 get collisions");
        }

        // Test get collisions
        // B int C
        p = min::vec2<double>(1.9, 1.9);
        collisions = g.get_collisions(p);
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec2 get collision point");
        }

        // Test overlap entire world
        collisions = g.get_overlap(world);
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec2 get overlap 1");
        }

        // Box D
        min = min::vec2<double>(-7.0, -7.0);
        max = min::vec2<double>(-4.0, -4.0);
        items.push_back(min::aabbox<double, min::vec2>(min, max));

        // Insert into grid
        g.insert(items);

        // Test overlap upper right quadrant, center 5.0, 5.0
        min = min::vec2<double>(0.0, 0.0);
        max = min::vec2<double>(10.0, 10.0);
        collisions = g.get_overlap(min::aabbox<double, min::vec2>(min, max));
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec2 get overlap 2");
        }

        // Test overlap box same cell
        min = min::vec2<double>(5.0, 5.0);
        max = min::vec2<double>(5.0, 5.0);
        collisions = g.get_overlap(min::aabbox<double, min::vec2>(min, max));
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec2 get overlap 3");
        }
    }

    // vec3 grid
    {
        // Local variables
        min::vec3<double> minW(-10.0, -10.0, -10.0);
        min::vec3<double> maxW(10.0, 10.0, 10.0);
        min::vec3<double> min;
        min::vec3<double> max;
        min::vec3<double> p;
        std::vector<uint_fast16_t> hits;
        std::vector<std::pair<uint_fast16_t, uint_fast16_t>> collisions;
        min::aabbox<double, min::vec3> world(minW, maxW);
        std::vector<min::aabbox<double, min::vec3>> items;
        min::grid<double, uint_fast16_t, uint_fast32_t, min::vec3, min::aabbox, min::aabbox> g(world);

        // Create three boxes and insert into vector

        // Box A
        min = min::vec3<double>(-1.0, -1.0, -1.0);
        max = min::vec3<double>(3.0, 4.0, 5.0);
        items.push_back(min::aabbox<double, min::vec3>(min, max));

        // Box B
        min = min::vec3<double>(-2.0, -2.0, -2.0);
        max = min::vec3<double>(1.0, 2.0, 3.0);
        items.push_back(min::aabbox<double, min::vec3>(min, max));

        // Box C
        min = min::vec3<double>(-3.0, -3.0, -3.0);
        max = min::vec3<double>(4.0, 5.0, 6.0);
        items.push_back(min::aabbox<double, min::vec3>(min, max));

        g.insert(items);

        // Maximum extent is sqrt(3*6^2) = sqrt(108) = 10.392 - Box A
        // Extent scale 2 is sqrt(1200)/2/2 = 8.660; ceil(cbrt(3)) == 2
        int scale = g.get_scale();
        out = out && compare(2, scale);
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec3 optimum scale");
        }

        // Insert into grid twice, should reset and rebuild
        g.insert(items);

        // Test point inside
        p = min::vec3<double>(2.9, 2.9, 2.9);
        hits = g.point_inside(p);
        out = out && compare(3, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec3 point_inside hit 1");
        }

        // Test get_cell center
        auto *node = &g.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(5.0, p.x(), 1E-4);
        out = out && compare(5.0, p.y(), 1E-4);
        out = out && compare(5.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec3 get_cell 1 get_center");
        }

        // Check the cell extents
        // Extent scale 2 should 20/2 = 10.0
        p = node->get_cell().get_extent();
        out = out && compare(10.0, p.x(), 1E-4);
        out = out && compare(10.0, p.y(), 1E-4);
        out = out && compare(10.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec3 get_cell 1 cell_extents");
        }

        // Test point inside
        p = min::vec3<double>(1.9, 1.9, 1.9);
        hits = g.point_inside(p);
        out = out && compare(3, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec3 point_inside hit 2");
        }

        // Test get leaf center
        node = &g.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(5.0, p.x(), 1E-4);
        out = out && compare(5.0, p.y(), 1E-4);
        out = out && compare(5.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec3 get_cell 2 get_center");
        }

        // Test point inside
        p = min::vec3<double>(0.9, 0.9, 0.9);
        hits = g.point_inside(p);
        out = out && compare(3, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec3 point_inside hit 3");
        }

        // Test get leaf center
        node = &g.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(5.0, p.x(), 1E-4);
        out = out && compare(5.0, p.y(), 1E-4);
        out = out && compare(5.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec3 get_cell 3 get_center");
        }

        // Test get collisions
        // A int B and B int C and A int C
        collisions = g.get_collisions();
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec3 get collisions");
        }

        // Test get collisions
        // B int C
        p = min::vec3<double>(1.9, 1.9, 1.9);
        collisions = g.get_collisions(p);
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec3 get collision point");
        }

        // Test overlap entire world
        collisions = g.get_overlap(world);
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec3 get overlap 1");
        }

        // Box D
        min = min::vec3<double>(-7.0, -7.0, -7.0);
        max = min::vec3<double>(-4.0, -4.0, -4.0);
        items.push_back(min::aabbox<double, min::vec3>(min, max));

        // Insert into grid
        g.insert(items);

        // Test overlap upper right quadrant, center 5.0, 5.0
        min = min::vec3<double>(0.0, 0.0, 0.0);
        max = min::vec3<double>(10.0, 10.0, 10.0);
        collisions = g.get_overlap(min::aabbox<double, min::vec3>(min, max));
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec3 get overlap 2");
        }

        // Test overlap box same cell
        min = min::vec3<double>(5.0, 5.0, 5.0);
        max = min::vec3<double>(5.0, 5.0, 5.0);
        collisions = g.get_overlap(min::aabbox<double, min::vec3>(min, max));
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec3 get overlap 3");
        }
    }

    //vec4 grid
    {
        // Local variables
        min::vec4<double> minW(-10.0, -10.0, -10.0, 0.0);
        min::vec4<double> maxW(10.0, 10.0, 10.0, 0.0);
        min::vec4<double> min;
        min::vec4<double> max;
        min::vec4<double> p;
        std::vector<uint_fast16_t> hits;
        std::vector<std::pair<uint_fast16_t, uint_fast16_t>> collisions;
        min::aabbox<double, min::vec4> world(minW, maxW);
        std::vector<min::aabbox<double, min::vec4>> items;
        min::grid<double, uint_fast16_t, uint_fast32_t, min::vec4, min::aabbox, min::aabbox> g(world);

        // Create three boxes and insert into vector

        // Box A
        min = min::vec4<double>(-1.0, -1.0, -1.0, 1.0);
        max = min::vec4<double>(3.0, 4.0, 5.0, 1.0);
        items.push_back(min::aabbox<double, min::vec4>(min, max));

        // Box B
        min = min::vec4<double>(-2.0, -2.0, -2.0, 1.0);
        max = min::vec4<double>(1.0, 2.0, 3.0, 1.0);
        items.push_back(min::aabbox<double, min::vec4>(min, max));

        // Box C
        min = min::vec4<double>(-3.0, -3.0, -3.0, 1.0);
        max = min::vec4<double>(4.0, 5.0, 6.0, 1.0);
        items.push_back(min::aabbox<double, min::vec4>(min, max));

        g.insert(std::move(items));

        // Maximum extent is sqrt(3*6^2) = sqrt(108) = 10.392 - Box A
        // Extent scale 2 is sqrt(1200)/2/2 = 8.660; ceil(cbrt(3)) == 2
        int scale = g.get_scale();
        out = out && compare(2, scale);
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec4 optimum scale");
        }

        // Insert into grid twice, should reset and rebuild
        g.insert(items);

        // Test point inside
        p = min::vec4<double>(2.9, 2.9, 2.9, 1.0);
        hits = g.point_inside(p);
        out = out && compare(3, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec4 point_inside hit 1");
        }

        // Test get_cell center
        auto *node = &g.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(5.0, p.x(), 1E-4);
        out = out && compare(5.0, p.y(), 1E-4);
        out = out && compare(5.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec4 get_cell 1 get_center");
        }

        // Check the cell extents
        // Extent scale 2 should 20/2 = 10.0
        p = node->get_cell().get_extent();
        out = out && compare(10.0, p.x(), 1E-4);
        out = out && compare(10.0, p.y(), 1E-4);
        out = out && compare(10.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec4 get_cell 1 cell_extents");
        }

        // Test point inside
        p = min::vec4<double>(1.9, 1.9, 1.9, 1.0);
        hits = g.point_inside(p);
        out = out && compare(3, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec4 point_inside hit 2");
        }

        // Test get leaf center
        node = &g.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(5.0, p.x(), 1E-4);
        out = out && compare(5.0, p.y(), 1E-4);
        out = out && compare(5.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec4 get_cell 2 get_center");
        }

        // Test point inside
        p = min::vec4<double>(0.9, 0.9, 0.9, 1.0);
        hits = g.point_inside(p);
        out = out && compare(3, hits.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec4 point_inside hit 3");
        }

        // Test get leaf center
        node = &g.get_node(p);
        p = node->get_cell().get_center();
        out = out && compare(5.0, p.x(), 1E-4);
        out = out && compare(5.0, p.y(), 1E-4);
        out = out && compare(5.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec4 get_cell 3 get_center");
        }

        // Test get collisions
        // A int B and B int C and A int C
        collisions = g.get_collisions();
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec4 get collisions");
        }

        // Test get collisions
        // B int C
        p = min::vec4<double>(1.9, 1.9, 1.9, 1.0);
        collisions = g.get_collisions(p);
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec4 get collision point");
        }

        // Test overlap entire world
        collisions = g.get_overlap(world);
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec4 get overlap 1");
        }

        // Box D
        min = min::vec4<double>(-7.0, -7.0, -7.0, 1.0);
        max = min::vec4<double>(-4.0, -4.0, -4.0, 1.0);
        items.push_back(min::aabbox<double, min::vec4>(min, max));

        // Insert into grid
        g.insert(items);

        // Test overlap upper right quadrant, center 5.0, 5.0
        min = min::vec4<double>(0.0, 0.0, 0.0, 1.0);
        max = min::vec4<double>(10.0, 10.0, 10.0, 1.0);
        collisions = g.get_overlap(min::aabbox<double, min::vec4>(min, max));
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec4 get overlap 2");
        }

        // Test overlap box same cell
        min = min::vec4<double>(5.0, 5.0, 5.0, 1.0);
        max = min::vec4<double>(5.0, 5.0, 5.0, 1.0);
        collisions = g.get_overlap(min::aabbox<double, min::vec4>(min, max));
        out = out && compare(3, collisions.size());
        if (!out)
        {
            throw std::runtime_error("Failed aabb grid vec4 get overlap 3");
        }
    }
    return out;
}

#endif
