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
#ifndef __TESTVECTOR3__
#define __TESTVECTOR3__

#include <min/test.h>
#include <min/vec3.h>
#include <stdexcept>

bool test_vec3()
{
    bool out = true;

    // Local variables
    min::vec3<double> one;
    min::vec3<double> two;
    min::vec3<double> three;
    min::vec3<double> vmin;
    min::vec3<double> vmax;

    // Test arithmetic
    one = min::vec3<double>(3.0, 1.0, 2.0);
    one += 2.0;
    one -= 1.0;
    one *= 3.0;
    one /= 2.0;

    two = min::vec3<double>(2.0, 3.0, 4.0);
    three = min::vec3<double>(1.0, 2.0, 3.0);
    one += two;
    one -= three;
    one *= two;
    one /= three;

    two = min::vec3<double>(1.0, 1.0, 1.0);
    one = one + two;
    one = one - two;
    two = min::vec3<double>(2.0, 2.0, 2.0);
    one = one * two;
    one = one / two;

    // Should be 14.0, 6.0, 7.333
    out = out && compare(14.0, one.x(), 1E-4);
    out = out && compare(6.0, one.y(), 1E-4);
    out = out && compare(7.3333, one.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 arithmetic operation");
    }

    // Test LERP
    one = min::vec3<double>(1.0, 2.0, 3.0);
    two = min::vec3<double>(2.0, 4.0, 6.0);
    three = min::vec3<double>::lerp(one, two, 0.5);

    // Should be 1.5, 3, 4.5
    out = out && compare(1.5, three.x(), 1E-4);
    out = out && compare(3.0, three.y(), 1E-4);
    out = out && compare(4.5, three.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 lerp operation");
    }

    // Test dot product; should be 21
    double dot = one.dot(three);
    out = out && compare(21.0, dot, 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 dot product operation");
    }

    // Test cross product; should be 1 Z
    one = min::vec3<double>(1.0, 0.0, 0.0);
    two = min::vec3<double>(0.0, 1.0, 0.0);
    three = one.cross(two);
    out = out && compare(0.0, three.x(), 1E-4);
    out = out && compare(0.0, three.y(), 1E-4);
    out = out && compare(1.0, three.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 cross product operation");
    }

    // Test magnitude; should be 3.74
    one = min::vec3<double>(1.0, 2.0, 3.0);
    double mag = one.magnitude();
    out = out && compare(3.7416, mag, 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 magnitude operation");
    }

    // Test normalize
    one = min::vec3<double>(1.0, 2.0, 3.0);
    one.normalize();
    out = out && compare(0.2672, one.x(), 1E-4);
    out = out && compare(0.5345, one.y(), 1E-4);
    out = out && compare(0.8017, one.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 normalize operation");
    }

    // Test clamp
    one = min::vec3<double>(-1.0, -1.0, -2.0);
    two = min::vec3<double>(-2.0, 4.0, -3.0);
    three = min::vec3<double>(3.0, 3.0, 3.0);
    two = two.clamp(one, three);
    out = out && compare(-1.0, two.x(), 1E-4);
    out = out && compare(3.0, two.y(), 1E-4);
    out = out && compare(-2.0, two.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 clamp operation");
    }

    // Test clamp direction
    one = min::vec3<double>(-1.0, -1.0, -2.0);
    two = min::vec3<double>(2.0, 4.0, -3.0);
    three = min::vec3<double>(3.0, 3.0, 3.0);
    two = two.clamp_direction(one, three);
    out = out && compare(1.0, two.x(), 1E-4);
    out = out && compare(-1.0, two.y(), 1E-4);
    out = out && compare(-1.0, two.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 clamp operation");
    }

    // Test any_zero
    one = min::vec3<double>(-1.0, -1E-7, 2.0);
    out = out && one.any_zero();
    if (!out)
    {
        throw std::runtime_error("Failed vec3 any_zero operation");
    }

    // Test not any_zero
    one = min::vec3<double>(-1.0, -1E-5, 2.0);
    out = out && !one.any_zero();
    if (!out)
    {
        throw std::runtime_error("Failed vec3 not any_zero operation");
    }

    // Test inverse
    one = min::vec3<double>(-2.0, 2.0, 3.0);
    one = one.inverse();
    out = out && compare(-0.5, one.x(), 1E-4);
    out = out && compare(0.5, one.y(), 1E-4);
    out = out && compare(0.3333, one.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 inverse operation");
    }

    // Test max
    one = min::vec3<double>(-2.0, 2.0, 5.0);
    double max = one.max();
    out = out && compare(5.0, max, 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 max operation");
    }

    // Test min
    one = min::vec3<double>(-2.0, 2.0, -10.0);
    double min = one.min();
    out = out && compare(-10.0, min, 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 min operation");
    }

    // Test order
    one = min::vec3<double>(-1.0, -1.0, 5.0);
    two = min::vec3<double>(-2.0, 4.0, 1.0);
    min::vec3<double>::order(one, two);
    out = out && compare(-2.0, one.x(), 1E-4);
    out = out && compare(-1.0, one.y(), 1E-4);
    out = out && compare(1.0, one.z(), 1E-4);
    out = out && compare(-1.0, two.x(), 1E-4);
    out = out && compare(4.0, two.y(), 1E-4);
    out = out && compare(5.0, two.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 order operation");
    }

    // Test within
    one = min::vec3<double>(-1.0, -1.0, -1.0);
    two = min::vec3<double>(0.0, 0.0, 0.0);
    three = min::vec3<double>(3.0, 3.0, 3.0);
    min::vec3<double>::order(one, two);
    out = out && two.within(one, three);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 within operation");
    }

    // Test greater than
    one = min::vec3<double>(-1.0, -1.0, -1.0);
    two = min::vec3<double>(0.0, 0.0, 0.0);
    out = out && two > one;
    if (!out)
    {
        throw std::runtime_error("Failed vec3 less than operation");
    }

    // Test less than
    one = min::vec3<double>(-1.0, -1.0, -1.0);
    two = min::vec3<double>(0.0, 0.0, 0.0);
    out = out && one < two;
    if (!out)
    {
        throw std::runtime_error("Failed vec3 greater than operation");
    }

    // Test greater than equal
    one = min::vec3<double>(-1.0, -1.0, -1.0);
    two = min::vec3<double>(0.0, 0.0, 0.0);
    out = out && two >= one;
    if (!out)
    {
        throw std::runtime_error("Failed vec3 greater than equal operation");
    }

    // Test less than equal
    one = min::vec3<double>(-1.0, -1.0, -1.0);
    two = min::vec3<double>(0.0, 0.0, 0.0);
    out = out && one <= two;
    if (!out)
    {
        throw std::runtime_error("Failed vec3 less than equal operation");
    }

    // Test sub-divide
    one = min::vec3<double>(-1.0, -1.0, -1.0);
    two = min::vec3<double>(1.0, 1.0, 1.0);
    auto sub = min::vec3<double>::subdivide(one, two);
    auto subc = min::vec3<double>::subdivide_center(one, two, 1.7320);

    // Test zero
    out = out && compare(-1.0, sub[0].first.x(), 1E-4);
    out = out && compare(-1.0, sub[0].first.y(), 1E-4);
    out = out && compare(-1.0, sub[0].first.z(), 1E-4);
    out = out && compare(0.0, sub[0].second.x(), 1E-4);
    out = out && compare(0.0, sub[0].second.y(), 1E-4);
    out = out && compare(0.0, sub[0].second.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide 0");
    }

    // Test zero center
    out = out && compare(-0.5, subc[0].first.x(), 1E-4);
    out = out && compare(-0.5, subc[0].first.y(), 1E-4);
    out = out && compare(-0.5, subc[0].first.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide_center 0");
    }

    // Test one
    out = out && compare(-1.0, sub[1].first.x(), 1E-4);
    out = out && compare(-1.0, sub[1].first.y(), 1E-4);
    out = out && compare(0.0, sub[1].first.z(), 1E-4);
    out = out && compare(0.0, sub[1].second.x(), 1E-4);
    out = out && compare(0.0, sub[1].second.y(), 1E-4);
    out = out && compare(1.0, sub[1].second.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide 1");
    }

    // Test one center
    out = out && compare(-0.5, subc[1].first.x(), 1E-4);
    out = out && compare(-0.5, subc[1].first.y(), 1E-4);
    out = out && compare(0.5, subc[1].first.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide_center 1");
    }

    // Test two
    out = out && compare(-1.0, sub[2].first.x(), 1E-4);
    out = out && compare(0.0, sub[2].first.y(), 1E-4);
    out = out && compare(-1.0, sub[2].first.z(), 1E-4);
    out = out && compare(0.0, sub[2].second.x(), 1E-4);
    out = out && compare(1.0, sub[2].second.y(), 1E-4);
    out = out && compare(0.0, sub[2].second.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide 2");
    }

    // Test two center
    out = out && compare(-0.5, subc[2].first.x(), 1E-4);
    out = out && compare(0.5, subc[2].first.y(), 1E-4);
    out = out && compare(-0.5, subc[2].first.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide_center 2");
    }

    // Test three
    out = out && compare(-1.0, sub[3].first.x(), 1E-4);
    out = out && compare(0.0, sub[3].first.y(), 1E-4);
    out = out && compare(0.0, sub[3].first.z(), 1E-4);
    out = out && compare(0.0, sub[3].second.x(), 1E-4);
    out = out && compare(1.0, sub[3].second.y(), 1E-4);
    out = out && compare(1.0, sub[3].second.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide 3");
    }

    // Test three center
    out = out && compare(-0.5, subc[3].first.x(), 1E-4);
    out = out && compare(0.5, subc[3].first.y(), 1E-4);
    out = out && compare(0.5, subc[3].first.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide_center 3");
    }

    // Test four
    out = out && compare(0.0, sub[4].first.x(), 1E-4);
    out = out && compare(-1.0, sub[4].first.y(), 1E-4);
    out = out && compare(-1.0, sub[4].first.z(), 1E-4);
    out = out && compare(1.0, sub[4].second.x(), 1E-4);
    out = out && compare(0.0, sub[4].second.y(), 1E-4);
    out = out && compare(0.0, sub[4].second.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide 4");
    }

    // Test four center
    out = out && compare(0.5, subc[4].first.x(), 1E-4);
    out = out && compare(-0.5, subc[4].first.y(), 1E-4);
    out = out && compare(-0.5, subc[4].first.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide_center 4");
    }

    // Test five
    out = out && compare(0.0, sub[5].first.x(), 1E-4);
    out = out && compare(-1.0, sub[5].first.y(), 1E-4);
    out = out && compare(0.0, sub[5].first.z(), 1E-4);
    out = out && compare(1.0, sub[5].second.x(), 1E-4);
    out = out && compare(0.0, sub[5].second.y(), 1E-4);
    out = out && compare(1.0, sub[5].second.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide 5");
    }

    // Test five center
    out = out && compare(0.5, subc[5].first.x(), 1E-4);
    out = out && compare(-0.5, subc[5].first.y(), 1E-4);
    out = out && compare(0.5, subc[5].first.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide_center 5");
    }

    // Test six
    out = out && compare(0.0, sub[6].first.x(), 1E-4);
    out = out && compare(0.0, sub[6].first.y(), 1E-4);
    out = out && compare(-1.0, sub[6].first.z(), 1E-4);
    out = out && compare(1.0, sub[6].second.x(), 1E-4);
    out = out && compare(1.0, sub[6].second.y(), 1E-4);
    out = out && compare(0.0, sub[6].second.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide 6");
    }

    // Test six center
    out = out && compare(0.5, subc[6].first.x(), 1E-4);
    out = out && compare(0.5, subc[6].first.y(), 1E-4);
    out = out && compare(-0.5, subc[6].first.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide_center 6");
    }

    // Test seven
    out = out && compare(0.0, sub[7].first.x(), 1E-4);
    out = out && compare(0.0, sub[7].first.y(), 1E-4);
    out = out && compare(0.0, sub[7].first.z(), 1E-4);
    out = out && compare(1.0, sub[7].second.x(), 1E-4);
    out = out && compare(1.0, sub[7].second.y(), 1E-4);
    out = out && compare(1.0, sub[7].second.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide 7");
    }

    // Test seven center
    out = out && compare(0.5, subc[7].first.x(), 1E-4);
    out = out && compare(0.5, subc[7].first.y(), 1E-4);
    out = out && compare(0.5, subc[7].first.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide_center 7");
    }

    // Test sub_overlap
    one = min::vec3<double>(-1.0, -1.0, -1.0);
    two = min::vec3<double>(1.0, 1.0, 1.0);
    three = min::vec3<double>(0.0, 0.0, 0.0);
    std::vector<uint8_t> sub_over = min::vec3<double>::sub_overlap(one, two, three);
    out = out && compare(0, sub_over[0]);
    out = out && compare(1, sub_over[1]);
    out = out && compare(2, sub_over[2]);
    out = out && compare(3, sub_over[3]);
    out = out && compare(4, sub_over[4]);
    out = out && compare(5, sub_over[5]);
    out = out && compare(6, sub_over[6]);
    out = out && compare(7, sub_over[7]);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 sub_overlap");
    }

    // Test grid and grid center
    one = min::vec3<double>(-1.0, -1.0, -1.0);
    two = min::vec3<double>(1.0, 1.0, 1.0);
    auto grid = min::vec3<double>::grid(one, two, 2);
    auto gridc = min::vec3<double>::grid_center(one, two, 2, 1.732);

    // Test zero
    out = out && compare(-1.0, grid[0].first.x(), 1E-4);
    out = out && compare(-1.0, grid[0].first.y(), 1E-4);
    out = out && compare(-1.0, grid[0].first.z(), 1E-4);
    out = out && compare(0.0, grid[0].second.x(), 1E-4);
    out = out && compare(0.0, grid[0].second.y(), 1E-4);
    out = out && compare(0.0, grid[0].second.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid 0");
    }

    // Test zero center
    out = out && compare(-0.5, gridc[0].first.x(), 1E-4);
    out = out && compare(-0.5, gridc[0].first.y(), 1E-4);
    out = out && compare(-0.5, gridc[0].first.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_center 0");
    }

    // Test one
    out = out && compare(-1.0, grid[1].first.x(), 1E-4);
    out = out && compare(-1.0, grid[1].first.y(), 1E-4);
    out = out && compare(0.0, grid[1].first.z(), 1E-4);
    out = out && compare(0.0, grid[1].second.x(), 1E-4);
    out = out && compare(0.0, grid[1].second.y(), 1E-4);
    out = out && compare(1.0, grid[1].second.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid 1");
    }

    // Test one center
    out = out && compare(-0.5, gridc[1].first.x(), 1E-4);
    out = out && compare(-0.5, gridc[1].first.y(), 1E-4);
    out = out && compare(0.5, gridc[1].first.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_center 1");
    }

    // Test two
    out = out && compare(-1.0, grid[2].first.x(), 1E-4);
    out = out && compare(0.0, grid[2].first.y(), 1E-4);
    out = out && compare(-1.0, grid[2].first.z(), 1E-4);
    out = out && compare(0.0, grid[2].second.x(), 1E-4);
    out = out && compare(1.0, grid[2].second.y(), 1E-4);
    out = out && compare(0.0, grid[2].second.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid 2");
    }

    // Test two center
    out = out && compare(-0.5, gridc[2].first.x(), 1E-4);
    out = out && compare(0.5, gridc[2].first.y(), 1E-4);
    out = out && compare(-0.5, gridc[2].first.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_center 2");
    }

    // Test three
    out = out && compare(-1.0, grid[3].first.x(), 1E-4);
    out = out && compare(0.0, grid[3].first.y(), 1E-4);
    out = out && compare(0.0, grid[3].first.z(), 1E-4);
    out = out && compare(0.0, grid[3].second.x(), 1E-4);
    out = out && compare(1.0, grid[3].second.y(), 1E-4);
    out = out && compare(1.0, grid[3].second.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid 3");
    }

    // Test three center
    out = out && compare(-0.5, gridc[3].first.x(), 1E-4);
    out = out && compare(0.5, gridc[3].first.y(), 1E-4);
    out = out && compare(0.5, gridc[3].first.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_center 3");
    }

    // Test four
    out = out && compare(0.0, grid[4].first.x(), 1E-4);
    out = out && compare(-1.0, grid[4].first.y(), 1E-4);
    out = out && compare(-1.0, grid[4].first.z(), 1E-4);
    out = out && compare(1.0, grid[4].second.x(), 1E-4);
    out = out && compare(0.0, grid[4].second.y(), 1E-4);
    out = out && compare(0.0, grid[4].second.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid 4");
    }

    // Test four center
    out = out && compare(0.5, gridc[4].first.x(), 1E-4);
    out = out && compare(-0.5, gridc[4].first.y(), 1E-4);
    out = out && compare(-0.5, gridc[4].first.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_center 4");
    }

    // Test five
    out = out && compare(0.0, grid[5].first.x(), 1E-4);
    out = out && compare(-1.0, grid[5].first.y(), 1E-4);
    out = out && compare(0.0, grid[5].first.z(), 1E-4);
    out = out && compare(1.0, grid[5].second.x(), 1E-4);
    out = out && compare(0.0, grid[5].second.y(), 1E-4);
    out = out && compare(1.0, grid[5].second.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid 5");
    }

    // Test five center
    out = out && compare(0.5, gridc[5].first.x(), 1E-4);
    out = out && compare(-0.5, gridc[5].first.y(), 1E-4);
    out = out && compare(0.5, gridc[5].first.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_center 5");
    }

    // Test six
    out = out && compare(0.0, grid[6].first.x(), 1E-4);
    out = out && compare(0.0, grid[6].first.y(), 1E-4);
    out = out && compare(-1.0, grid[6].first.z(), 1E-4);
    out = out && compare(1.0, grid[6].second.x(), 1E-4);
    out = out && compare(1.0, grid[6].second.y(), 1E-4);
    out = out && compare(0.0, grid[6].second.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid 6");
    }

    // Test six center
    out = out && compare(0.5, gridc[6].first.x(), 1E-4);
    out = out && compare(0.5, gridc[6].first.y(), 1E-4);
    out = out && compare(-0.5, gridc[6].first.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_center 6");
    }

    // Test seven
    out = out && compare(0.0, grid[7].first.x(), 1E-4);
    out = out && compare(0.0, grid[7].first.y(), 1E-4);
    out = out && compare(0.0, grid[7].first.z(), 1E-4);
    out = out && compare(1.0, grid[7].second.x(), 1E-4);
    out = out && compare(1.0, grid[7].second.y(), 1E-4);
    out = out && compare(1.0, grid[7].second.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid 7");
    }

    // Test seven center
    out = out && compare(0.5, gridc[7].first.x(), 1E-4);
    out = out && compare(0.5, gridc[7].first.y(), 1E-4);
    out = out && compare(0.5, gridc[7].first.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_center 7");
    }

    // Test grid key 6
    three = min::vec3<double>(0.5, 0.5, -0.5);
    size_t key = min::vec3<double>::grid_key(one, two, 2, three);
    out = out && compare(6, key);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid key 6");
    }

    // Test grid key 7
    three = min::vec3<double>(0.5, 0.5, 0.5);
    key = min::vec3<double>::grid_key(one, two, 2, three);
    out = out && compare(7, key);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid key 7");
    }

    // Test grid overlap
    one = min::vec3<double>(-10000.0, -10000.0, -10000.0);
    two = min::vec3<double>(10000.0, 10000.0, 10000.0);
    vmin = min::vec3<double>(-100.0, -100.0, -100.0);
    vmax = min::vec3<double>(100.0, 100.0, 100.0);
    std::vector<size_t> over = min::vec3<double>::grid_overlap(one, (two - one) / 40, 40, vmin, vmax);
    out = out && compare(8, over.size());
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_overlap");
    }

    // For normal_box_aligned tests
    vmin = min::vec3<double>(-100.0, -100.0, -100.0);
    vmax = min::vec3<double>(100.0, 100.0, 100.0);

    // Test normal_box_aligned left
    one = min::vec3<double>(-150.0, 0.0, 0.0);
    two = min::vec3<double>::normal_box_aligned(one, vmin, vmax);
    out = out && compare(-1.0, two.x(), 1E-4);
    out = out && compare(0.0, two.y(), 1E-4);
    out = out && compare(0.0, two.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 normal_box_aligned left");
    }

    // Test normal_box_aligned right
    one = min::vec3<double>(150.0, 0.0, 0.0);
    two = min::vec3<double>::normal_box_aligned(one, vmin, vmax);
    out = out && compare(1.0, two.x(), 1E-4);
    out = out && compare(0.0, two.y(), 1E-4);
    out = out && compare(0.0, two.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 normal_box_aligned right");
    }

    // Test normal_box_aligned bottom
    one = min::vec3<double>(50.0, -150.0, 50.0);
    two = min::vec3<double>::normal_box_aligned(one, vmin, vmax);
    out = out && compare(0.0, two.x(), 1E-4);
    out = out && compare(-1.0, two.y(), 1E-4);
    out = out && compare(0.0, two.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 normal_box_aligned bottom");
    }

    // Test normal_box_aligned top
    one = min::vec3<double>(50.0, 150.0, 50.0);
    two = min::vec3<double>::normal_box_aligned(one, vmin, vmax);
    out = out && compare(0.0, two.x(), 1E-4);
    out = out && compare(1.0, two.y(), 1E-4);
    out = out && compare(0.0, two.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 normal_box_aligned top");
    }

    // Test normal_box_aligned back
    one = min::vec3<double>(50.0, 50.0, -150.0);
    two = min::vec3<double>::normal_box_aligned(one, vmin, vmax);
    out = out && compare(0.0, two.x(), 1E-4);
    out = out && compare(0.0, two.y(), 1E-4);
    out = out && compare(-1.0, two.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 normal_box_aligned back");
    }

    // Test normal_box_aligned front
    one = min::vec3<double>(50.0, 50.0, 150.0);
    two = min::vec3<double>::normal_box_aligned(one, vmin, vmax);
    out = out && compare(0.0, two.x(), 1E-4);
    out = out && compare(0.0, two.y(), 1E-4);
    out = out && compare(1.0, two.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 normal_box_aligned front");
    }

    return out;
}

#endif
