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
#ifndef __TESTVECTOR2__
#define __TESTVECTOR2__

#include <min/test.h>
#include <min/vec2.h>
#include <stdexcept>

bool test_vec2()
{
    bool out = true;

    // Local variables
    min::vec2<double> one;
    min::vec2<double> two;
    min::vec2<double> three;
    min::vec2<double> vmin;
    min::vec2<double> vmax;

    // Test arithmetic
    one = min::vec2<double>(3.0, 1.0);
    one += 2.0;
    one -= 1.0;
    one *= 3.0;
    one /= 2.0;

    two = min::vec2<double>(2.0, 3.0);
    three = min::vec2<double>(1.0, 2.0);
    one += two;
    one -= three;
    one *= two;
    one /= three;

    two = min::vec2<double>(1.0, 1.0);
    one = one + two;
    one = one - two;
    two = min::vec2<double>(2.0, 2.0);
    one = one * two;
    one = one / two;

    // Should be 14.0, 6.0
    out = out && compare(14.0, one.x(), 1E-4);
    out = out && compare(6.0, one.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 arithmetic operation");
    }

    // Test LERP
    one = min::vec2<double>(1.0, 2.0);
    two = min::vec2<double>(2.0, 4.0);
    three = min::vec2<double>::lerp(one, two, 0.5);

    // Should be 1.5, 3
    out = out && compare(1.5, three.x(), 1E-4);
    out = out && compare(3.0, three.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 lerp operation");
    }

    // Test dot product; should be 7.5
    double dot = one.dot(three);
    out = out && compare(7.5, dot, 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 dot product operation");
    }

    // Test cross product; should be 1 Z
    one = min::vec2<double>(1.0, 0.0);
    two = min::vec2<double>(0.0, 1.0);
    double cross = one.cross(two);
    out = out && compare(1.0, cross, 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 cross product operation");
    }

    // Test magnitude; should be 2.236
    one = min::vec2<double>(1.0, 2.0);
    double mag = one.magnitude();
    out = out && compare(2.236, mag, 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 magnitude operation");
    }

    // Test normalize
    one = min::vec2<double>(1.0, 2.0);
    one.normalize();
    out = out && compare(0.4472, one.x(), 1E-4);
    out = out && compare(0.8944, one.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 normalize operation");
    }

    // Test clamp
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(-2.0, 4.0);
    three = min::vec2<double>(3.0, 3.0);
    two = two.clamp(one, three);
    out = out && compare(-1.0, two.x(), 1E-4);
    out = out && compare(3.0, two.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 clamp operation");
    }

    // Test clamp direction
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(2.0, 4.0);
    three = min::vec2<double>(3.0, 3.0);
    two = two.clamp_direction(one, three);
    out = out && compare(1.0, two.x(), 1E-4);
    out = out && compare(-1.0, two.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 clamp direction operation");
    }

    // Test any_zero
    one = min::vec2<double>(-1.0, -1E-7);
    out = out && one.any_zero();
    if (!out)
    {
        throw std::runtime_error("Failed vec2 any_zero operation");
    }

    // Test not any_zero
    one = min::vec2<double>(-1.0, -1E-5);
    out = out && !one.any_zero();
    if (!out)
    {
        throw std::runtime_error("Failed vec2 not any_zero operation");
    }

    // Test inverse
    one = min::vec2<double>(-2.0, 2.0);
    one = one.inverse();
    out = out && compare(-0.5, one.x(), 1E-4);
    out = out && compare(0.5, one.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 inverse operation");
    }

    // Test max
    one = min::vec2<double>(-2.0, 2.0);
    double max = one.max();
    out = out && compare(2.0, max, 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 max operation");
    }

    // Test min
    one = min::vec2<double>(-2.0, 2.0);
    double min = one.min();
    out = out && compare(-2.0, min, 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 min operation");
    }

    // Test order
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(-2.0, 4.0);
    min::vec2<double>::order(one, two);
    out = out && compare(-2.0, one.x(), 1E-4);
    out = out && compare(-1.0, one.y(), 1E-4);
    out = out && compare(-1.0, two.x(), 1E-4);
    out = out && compare(4.0, two.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 order operation");
    }

    // Test within
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(0.0, 0.0);
    three = min::vec2<double>(3.0, 3.0);
    min::vec2<double>::order(one, two);
    out = out && two.within(one, three);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 within operation");
    }

    // Test greater than
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(0.0, 0.0);
    out = out && two > one;
    if (!out)
    {
        throw std::runtime_error("Failed vec2 less than operation");
    }

    // Test less than
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(0.0, 0.0);
    out = out && one < two;
    if (!out)
    {
        throw std::runtime_error("Failed vec2 greater than operation");
    }

    // Test greater than equal
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(0.0, 0.0);
    out = out && two >= one;
    if (!out)
    {
        throw std::runtime_error("Failed vec2 greater than equal operation");
    }

    // Test less than equal
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(0.0, 0.0);
    out = out && one <= two;
    if (!out)
    {
        throw std::runtime_error("Failed vec2 less than equal operation");
    }

    // Test sub-divide and sub-divide center
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(1.0, 1.0);
    auto sub = min::vec2<double>::subdivide(one, two);
    auto subc = min::vec2<double>::subdivide_center(one, two, 1.4141);

    // Test zero
    out = out && compare(-1.0, sub[0].first.x(), 1E-4);
    out = out && compare(-1.0, sub[0].first.y(), 1E-4);
    out = out && compare(0.0, sub[0].second.x(), 1E-4);
    out = out && compare(0.0, sub[0].second.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 subdivide 0");
    }

    // Test zero center
    out = out && compare(-0.5, subc[0].first.x(), 1E-4);
    out = out && compare(-0.5, subc[0].first.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 subdivide_center 0");
    }

    // Test one
    out = out && compare(-1.0, sub[1].first.x(), 1E-4);
    out = out && compare(0.0, sub[1].first.y(), 1E-4);
    out = out && compare(0.0, sub[1].second.x(), 1E-4);
    out = out && compare(1.0, sub[1].second.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 subdivide 1");
    }

    // Test one center
    out = out && compare(-0.5, subc[1].first.x(), 1E-4);
    out = out && compare(0.5, subc[1].first.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 subdivide_center 1");
    }

    // Test two
    out = out && compare(0.0, sub[2].first.x(), 1E-4);
    out = out && compare(-1.0, sub[2].first.y(), 1E-4);
    out = out && compare(1.0, sub[2].second.x(), 1E-4);
    out = out && compare(0.0, sub[2].second.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 subdivide 2");
    }

    // Test two center
    out = out && compare(0.5, subc[2].first.x(), 1E-4);
    out = out && compare(-0.5, subc[2].first.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 subdivide_center 2");
    }

    // Test three
    out = out && compare(0.0, sub[3].first.x(), 1E-4);
    out = out && compare(0.0, sub[3].first.y(), 1E-4);
    out = out && compare(1.0, sub[3].second.x(), 1E-4);
    out = out && compare(1.0, sub[3].second.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 subdivide 3");
    }

    // Test three center
    out = out && compare(0.5, subc[3].first.x(), 1E-4);
    out = out && compare(0.5, subc[3].first.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 subdivide_center 3");
    }

    // Test sub_overlap
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(1.0, 1.0);
    three = min::vec2<double>(0.0, 0.0);
    std::vector<uint8_t> sub_over = min::vec2<double>::sub_overlap(one, two, three);
    out = out && compare(0, sub_over[0]);
    out = out && compare(1, sub_over[1]);
    out = out && compare(2, sub_over[2]);
    out = out && compare(3, sub_over[3]);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 sub_overlap");
    }

    // Test grid and grid center
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(1.0, 1.0);
    auto grid = min::vec2<double>::grid(one, two, 2);
    auto gridc = min::vec2<double>::grid_center(one, two, 2, 1.414);

    // Test zero
    out = out && compare(-1.0, grid[0].first.x(), 1E-4);
    out = out && compare(-1.0, grid[0].first.y(), 1E-4);
    out = out && compare(0.0, grid[0].second.x(), 1E-4);
    out = out && compare(0.0, grid[0].second.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 grid 0");
    }

    // Test zero center
    out = out && compare(-0.5, gridc[0].first.x(), 1E-4);
    out = out && compare(-0.5, gridc[0].first.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 grid_center 0");
    }

    // Test one
    out = out && compare(-1.0, grid[1].first.x(), 1E-4);
    out = out && compare(0.0, grid[1].first.y(), 1E-4);
    out = out && compare(0.0, grid[1].second.x(), 1E-4);
    out = out && compare(1.0, grid[1].second.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 grid 1");
    }

    // Test one center
    out = out && compare(-0.5, gridc[1].first.x(), 1E-4);
    out = out && compare(0.5, gridc[1].first.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 grid_center 1");
    }

    // Test two
    out = out && compare(0.0, grid[2].first.x(), 1E-4);
    out = out && compare(-1.0, grid[2].first.y(), 1E-4);
    out = out && compare(1.0, grid[2].second.x(), 1E-4);
    out = out && compare(0.0, grid[2].second.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 grid 2");
    }

    // Test two center
    out = out && compare(0.5, gridc[2].first.x(), 1E-4);
    out = out && compare(-0.5, gridc[2].first.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 grid_center 2");
    }

    // Test three
    out = out && compare(0.0, grid[3].first.x(), 1E-4);
    out = out && compare(0.0, grid[3].first.y(), 1E-4);
    out = out && compare(1.0, grid[3].second.x(), 1E-4);
    out = out && compare(1.0, grid[3].second.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 grid 3");
    }

    // Test three center
    out = out && compare(0.5, gridc[3].first.x(), 1E-4);
    out = out && compare(0.5, gridc[3].first.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 grid_center 3");
    }

    // Test grid key 1
    three = min::vec2<double>(-0.5, 0.5);
    size_t key = min::vec2<double>::grid_key(one, two, 2, three);
    out = out && compare(1, key);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 grid key 1");
    }

    // Test grid key 3
    three = min::vec2<double>(0.5, 0.5);
    key = min::vec2<double>::grid_key(one, two, 2, three);
    out = out && compare(3, key);
    if (!out)
    {
        throw std::runtime_error("Failed vec2 grid key 3");
    }

    // Test grid overlap
    one = min::vec2<double>(-10000.0, -10000.0);
    two = min::vec2<double>(10000.0, 10000.0);
    vmin = min::vec2<double>(-100.0, -100.0);
    vmax = min::vec2<double>(100.0, 100.0);
    std::vector<size_t> over = min::vec2<double>::grid_overlap(one, (two - one) / 40, 40, vmin, vmax);
    out = out && compare(4, over.size());
    if (!out)
    {
        throw std::runtime_error("Failed vec2 grid_overlap");
    }

    return out;
}

#endif