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

    // Test cross product; should be Y x Z = X
    one = min::vec3<double>(0.0, 1.0, 0.0);
    two = min::vec3<double>(0.0, 0.0, 1.0);
    three = one.cross(two);
    out = out && compare(1.0, three.x(), 1E-4);
    out = out && compare(0.0, three.y(), 1E-4);
    out = out && compare(0.0, three.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 cross product X operation");
    }

    // Test cross product; should be Z x X = Y
    one = min::vec3<double>(0.0, 0.0, 1.0);
    two = min::vec3<double>(1.0, 0.0, 0.0);
    three = one.cross(two);
    out = out && compare(0.0, three.x(), 1E-4);
    out = out && compare(1.0, three.y(), 1E-4);
    out = out && compare(0.0, three.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 cross product Y operation");
    }

    // Test cross product; should be X x Y = Z
    one = min::vec3<double>(1.0, 0.0, 0.0);
    two = min::vec3<double>(0.0, 1.0, 0.0);
    three = one.cross(two);
    out = out && compare(0.0, three.x(), 1E-4);
    out = out && compare(0.0, three.y(), 1E-4);
    out = out && compare(1.0, three.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 cross product Z operation");
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

    // Test normalize unsafe
    one = min::vec3<double>(1.0, 2.0, 3.0);
    one.normalize_unsafe();
    out = out && compare(0.2672, one.x(), 1E-4);
    out = out && compare(0.5345, one.y(), 1E-4);
    out = out && compare(0.8017, one.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 normalize_unsafe operation");
    }

    // Test normalize_safe
    one = min::vec3<double>(0.0, 0.0, 0.0);
    one.normalize_safe(min::vec3<double>::up());
    out = out && compare(0.0, one.x(), 1E-4);
    out = out && compare(1.0, one.y(), 1E-4);
    out = out && compare(0.0, one.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 normalize_safe operation");
    }

    // Test normalize fallback
    one = min::vec3<double>(0.0, 0.0, 0.0);
    one.normalize();
    out = out && compare(0.0, one.x(), 1E-4);
    out = out && compare(0.0, one.y(), 1E-4);
    out = out && compare(0.0, one.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 normalize fallback operation");
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

    // Test any_zero_outside
    one = min::vec3<double>(0.0, -1.0, 1.0);
    two = min::vec3<double>(1.0, 1.0, 1.0);
    vmin = min::vec3<double>(2.0, 2.0, 2.0);
    vmax = min::vec3<double>(3.0, 3.0, 3.0);
    out = out && two.any_zero_outside(one, vmin, vmax);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 any_zero_outside operation");
    }

    // Test not any_zero_outside
    // Test any_zero_outside
    one = min::vec3<double>(0.0, -1.0, 1.0);
    two = min::vec3<double>(2.5, 2.5, 2.5);
    vmin = min::vec3<double>(2.0, 2.0, 2.0);
    vmax = min::vec3<double>(3.0, 3.0, 3.0);
    out = out && !two.any_zero_outside(one, vmin, vmax);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 not any_zero_outside operation");
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
    std::vector<std::pair<min::vec3<double>, min::vec3<double>>> sub;
    std::vector<std::pair<min::vec3<double>, double>> subc;
    min::vec3<double>::subdivide(sub, one, two);
    min::vec3<double>::subdivide_center(subc, one, two);

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

    // Test subdivide_ray 1
    min::vec3<double> origin(300.0, 100.0, 50.0);
    min::vec3<double> direction = min::vec3<double>(-1.0, -1.0, -1.0).normalize();
    min::vec3<double> inverse = direction.inverse();
    vmin = min::vec3<double>(-100000.0, -100000.0, -100000.0);
    vmax = min::vec3<double>(100000.0, 100000.0, 100000.0);
    min::stack_vector<uint_fast8_t, min::vec3<double>::sub_size()> keys;
    keys = min::vec3<double>::subdivide_ray(vmin, vmax, origin, direction, inverse);
    out = out && compare(4, keys.size());
    out = out && compare(7, keys[0]);
    out = out && compare(6, keys[1]);
    out = out && compare(4, keys[2]);
    out = out && compare(0, keys[3]);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide_ray 1");
    }

    // Test subdivide_ray 2
    origin = min::vec3<double>(-300.0, -50.0, -100.0);
    direction = min::vec3<double>(1.0, 1.0, 1.0).normalize();
    inverse = direction.inverse();
    keys = min::vec3<double>::subdivide_ray(vmin, vmax, origin, direction, inverse);
    out = out && compare(4, keys.size());
    out = out && compare(0, keys[0]);
    out = out && compare(2, keys[1]);
    out = out && compare(3, keys[2]);
    out = out && compare(7, keys[3]);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide_ray 2");
    }

    // Test subdivide_ray 3
    origin = min::vec3<double>(-300.0, -300.0, -300.0);
    direction = min::vec3<double>(-1.0, -1.0, -1.0).normalize();
    inverse = direction.inverse();
    keys = min::vec3<double>::subdivide_ray(vmin, vmax, origin, direction, inverse);
    out = out && compare(8, keys.size());
    out = out && compare(7, keys[0]);
    out = out && compare(6, keys[1]);
    out = out && compare(3, keys[2]);
    out = out && compare(2, keys[3]);
    out = out && compare(5, keys[4]);
    out = out && compare(4, keys[5]);
    out = out && compare(1, keys[6]);
    out = out && compare(0, keys[7]);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide_ray 3");
    }

    // Test subdivide_ray 4
    origin = min::vec3<double>(-1E-7, 1E-7, 1E-7);
    direction = min::vec3<double>(1.0, -1.0, -1.0).normalize();
    inverse = direction.inverse();
    keys = min::vec3<double>::subdivide_ray(vmin, vmax, origin, direction, inverse);
    out = out && compare(8, keys.size());
    out = out && compare(3, keys[0]);
    out = out && compare(7, keys[1]);
    out = out && compare(2, keys[2]);
    out = out && compare(6, keys[3]);
    out = out && compare(1, keys[4]);
    out = out && compare(5, keys[5]);
    out = out && compare(0, keys[6]);
    out = out && compare(4, keys[7]);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide_ray 4");
    }

    // Test subdivide_ray 5
    origin = min::vec3<double>(-99999.0, 99999.0, -99999.0);
    direction = min::vec3<double>(0.0, -1.0, 0.0).normalize();
    inverse = direction.inverse();
    keys = min::vec3<double>::subdivide_ray(vmin, vmax, origin, direction, inverse);
    out = out && compare(2, keys.size());
    out = out && compare(2, keys[0]);
    out = out && compare(0, keys[1]);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 subdivide_ray 5");
    }

    // Test subdivide_overlap
    one = min::vec3<double>(-1.0, -1.0, -1.0);
    two = min::vec3<double>(1.0, 1.0, 1.0);
    three = min::vec3<double>(0.0, 0.0, 0.0);
    min::stack_vector<uint_fast8_t, min::vec3<double>::sub_size()> sub_over;
    sub_over = min::vec3<double>::subdivide_overlap(one, two, three);
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
        throw std::runtime_error("Failed vec3 subdivide_overlap");
    }

    // Test grid and grid center
    one = min::vec3<double>(-1.0, -1.0, -1.0);
    two = min::vec3<double>(1.0, 1.0, 1.0);
    std::vector<std::pair<min::vec3<double>, min::vec3<double>>> grid;
    std::vector<std::pair<min::vec3<double>, double>> gridc;
    min::vec3<double>::grid(grid, one, two, 2);
    min::vec3<double>::grid_center(gridc, one, two, 2, 1.732);

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

    // Test grid_index
    min::vec3<double> cell_extent(1.0, 1.0, 1.0);
    origin = min::vec3<double>(0.5, 0.5, 0.5);
    direction = min::vec3<double>(0.0, 1.0, 0.0);
    inverse = direction.inverse();
    vmin = min::vec3<double>(-100.0, -100.0, -100.0);
    auto index = min::vec3<double>::grid_index(vmin, cell_extent, origin);

    out = out && compare(100, index.x());
    out = out && compare(100, index.y());
    out = out && compare(100, index.z());
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_index 1");
    }

    // Test grid_index from key
    key = min::vec3<double>::grid_key(vmin, cell_extent, 200, origin);
    index = min::vec3<double>::grid_index(key, 200);
    out = out && compare(100, index.x());
    out = out && compare(100, index.y());
    out = out && compare(100, index.z());
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_index 2");
    }

    // Test grid_ray
    auto t = min::vec3<double>::grid_ray(vmin, cell_extent, origin, direction, inverse);
    out = out && compare(1, std::get<0>(t));
    out = out && compare(std::numeric_limits<double>::max(), std::get<1>(t), 1E-4);
    out = out && compare(std::numeric_limits<double>::max(), std::get<2>(t), 1E-4);
    out = out && compare(1, std::get<3>(t));
    out = out && compare(0.5, std::get<4>(t), 1E-4);
    out = out && compare(1.0, std::get<5>(t), 1E-4);
    out = out && compare(1, std::get<6>(t));
    out = out && compare(std::numeric_limits<double>::max(), std::get<7>(t), 1E-4);
    out = out && compare(std::numeric_limits<double>::max(), std::get<8>(t), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_ray");
    }

    // Test grid_ray_next
    bool flag = false;
    key = min::vec3<double>::grid_ray_next(index, t, flag, 200);
    out = out && compare(4020300, key);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_ray_next");
    }

    // Test grid_ray on diagonal
    origin = min::vec3<double>(0.0, 0.0, 0.0);
    direction = min::vec3<double>(1.0, 1.0, 1.0);
    inverse = direction.inverse();
    index = min::vec3<double>::grid_index(vmin, cell_extent, origin);
    t = min::vec3<double>::grid_ray(vmin, cell_extent, origin, direction, inverse);
    out = out && compare(1, std::get<0>(t));
    out = out && compare(1.0, std::get<1>(t), 1E-4);
    out = out && compare(1.0, std::get<2>(t), 1E-4);
    out = out && compare(1, std::get<3>(t));
    out = out && compare(1.0, std::get<4>(t), 1E-4);
    out = out && compare(1.0, std::get<5>(t), 1E-4);
    out = out && compare(1, std::get<6>(t));
    out = out && compare(1.0, std::get<7>(t), 1E-4);
    out = out && compare(1.0, std::get<8>(t), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_ray diagonal");
    }

    flag = false;

    // Starting point x = 100, y = 100, z = 100
    key = min::vec3<double>::grid_key(vmin, cell_extent, 200.0, origin);
    out = out && compare(4020100, key);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_ray_next diagonal 1");
    }
    // point x = 101, y = 100, z = 100
    key = min::vec3<double>::grid_ray_next(index, t, flag, 200.0);
    out = out && compare(4060100, key);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_ray_next diagonal 2");
    }
    // point x = 101, y = 101, z = 100
    key = min::vec3<double>::grid_ray_next(index, t, flag, 200.0);
    out = out && compare(4060300, key);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_ray_next diagonal 3");
    }
    // point x = 101, y = 101, z = 101
    key = min::vec3<double>::grid_ray_next(index, t, flag, 200.0);
    out = out && compare(4060301, key);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_ray_next diagonal 4");
    }

    // Test grid overlap
    one = min::vec3<double>(-10000.0, -10000.0, -10000.0);
    two = min::vec3<double>(10000.0, 10000.0, 10000.0);
    vmin = min::vec3<double>(-100.0, -100.0, -100.0);
    vmax = min::vec3<double>(100.0, 100.0, 100.0);
    min::stack_vector<size_t, min::vec3<double>::over_size()> over;
    over = min::vec3<double>::grid_overlap(one, (two - one) / 40, 40, vmin, vmax);
    out = out && compare(8, over.size());
    if (!out)
    {
        throw std::runtime_error("Failed vec3 grid_overlap");
    }

    // Test grid sat penetration aligned
    one = min::vec3<double>(10.0, 10.0, 10.0);
    two = min::vec3<double>(8.0, 8.0, 8.0);
    vmin = min::vec3<double>(3.0, 3.0, 3.0);
    vmax = min::vec3<double>(3.0, 3.0, 3.0);

    // center, half_extent, center, half_extent
    std::pair<min::vec3<double>, double> p = min::vec3<double>::project_sat_aligned_penetration(one, vmin, two, vmax, 0.01);
    out = out && compare(1.0, p.first.x(), 1E-4);
    out = out && compare(0.0, p.first.y(), 1E-4);
    out = out && compare(0.0, p.first.z(), 1E-4);
    out = out && compare(4.09, p.second, 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 sat penetration aligned");
    }

    // Test grid sat penetration
    one = min::vec3<double>(10.0, 10.1, 10.2);
    two = min::vec3<double>(8.0, 8.0, 8.0);
    vmin = min::vec3<double>(3.0, 3.0, 3.0);
    vmax = min::vec3<double>(3.0, 3.0, 3.0);

    // coord_sys, center, half_extent, coord_sys, center, half_extent
    p = min::vec3<double>::project_sat_penetration(min::vec3<double>::axes(), one, vmin, min::vec3<double>::axes(), two, vmax, 0.01);

    // Z has the least penetration
    out = out && compare(0.0, p.first.x(), 1E-4);
    out = out && compare(0.0, p.first.y(), 1E-4);
    out = out && compare(1.0, p.first.z(), 1E-4);
    out = out && compare(3.89, p.second, 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed vec3 sat penetration");
    }

    return out;
}

#endif
