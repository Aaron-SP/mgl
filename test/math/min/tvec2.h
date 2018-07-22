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
#ifndef _MGL_TESTVECTOR2_MGL_
#define _MGL_TESTVECTOR2_MGL_

#include <min/test.h>
#include <min/vec2.h>

bool test_vec2()
{
    bool out = true;

    // Print size and alignment of class
    std::cout << "vec2_size: " << sizeof(min::vec2<float>) << std::endl;
    std::cout << "vec2_align: " << alignof(min::vec2<float>) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "tvec2.h: Testing alignment" << std::endl;
    out = out && test(sizeof(float) * 2, sizeof(min::vec2<float>), "Failed vec2 sizeof");
    out = out && test(sizeof(float), alignof(min::vec2<float>), "Failed vec2 alignof");
#endif

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
    out = out && test(14.0, one.x(), 1E-4, "Failed vec2 arithmetic operation");
    out = out && test(6.0, one.y(), 1E-4, "Failed vec2 arithmetic operation");

    // Test LERP
    one = min::vec2<double>(1.0, 2.0);
    two = min::vec2<double>(2.0, 4.0);
    three = min::vec2<double>::lerp(one, two, 0.5);

    // Should be 1.5, 3
    out = out && test(1.5, three.x(), 1E-4, "Failed vec2 lerp operation");
    out = out && test(3.0, three.y(), 1E-4, "Failed vec2 lerp operation");

    // Test dot product; should be 7.5
    double dot = one.dot(three);
    out = out && test(7.5, dot, 1E-4, "Failed vec2 dot product operation");

    // Test cross product; should be 1 Z
    one = min::vec2<double>(1.0, 0.0);
    two = min::vec2<double>(0.0, 1.0);
    double cross = one.cross(two);
    out = out && test(1.0, cross, 1E-4, "Failed vec2 cross product operation");

    // Test magnitude; should be 2.236
    one = min::vec2<double>(1.0, 2.0);
    double mag = one.magnitude();
    out = out && test(2.236, mag, 1E-4, "Failed vec2 magnitude operation");

    // Test normalize
    one = min::vec2<double>(1.0, 2.0);
    one.normalize();
    out = out && test(0.4472, one.x(), 1E-4, "Failed vec2 normalize operation");
    out = out && test(0.8944, one.y(), 1E-4, "Failed vec2 normalize operation");

    // Test normalize_unsafe
    one = min::vec2<double>(1.0, 2.0);
    one.normalize_unsafe();
    out = out && test(0.4472, one.x(), 1E-4, "Failed vec2 normalize_unsafe operation");
    out = out && test(0.8944, one.y(), 1E-4, "Failed vec2 normalize_unsafe operation");

    // Test normalize_safe
    one = min::vec2<double>(0.0, 0.0);
    one.normalize_safe(min::vec2<double>::up());
    out = out && test(0.0, one.x(), 1E-4, "Failed vec2 normalize_safe operation");
    out = out && test(1.0, one.y(), 1E-4, "Failed vec2 normalize_safe operation");

    // Test normalize fallback
    one = min::vec2<double>(0.0, 0.0);
    one.normalize();
    out = out && test(0.0, one.x(), 1E-4, "Failed vec2 normalize fallback operation");
    out = out && test(0.0, one.y(), 1E-4, "Failed vec2 normalize fallback operation");

    // Test clamp
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(-2.0, 4.0);
    three = min::vec2<double>(3.0, 3.0);
    two = two.clamp(one, three);
    out = out && test(-1.0, two.x(), 1E-4, "Failed vec2 clamp operation");
    out = out && test(3.0, two.y(), 1E-4, "Failed vec2 clamp operation");

    // Test clamp direction
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(2.0, 4.0);
    three = min::vec2<double>(3.0, 3.0);
    two = two.clamp_direction(one, three);
    out = out && test(1.0, two.x(), 1E-4, "Failed vec2 clamp direction operation");
    out = out && test(-1.0, two.y(), 1E-4, "Failed vec2 clamp direction operation");

    // Test any_zero_outside
    one = min::vec2<double>(0.0, -1.0);
    two = min::vec2<double>(1.0, 1.0);
    vmin = min::vec2<double>(2.0, 2.0);
    vmax = min::vec2<double>(3.0, 3.0);
    out = out && test(true, two.any_zero_outside(one, vmin, vmax), "Failed vec2 any_zero_outside operation");

    // Test not any_zero_outside
    one = min::vec2<double>(0.0, -1.0);
    two = min::vec2<double>(2.5, 2.5);
    vmin = min::vec2<double>(2.0, 2.0);
    vmax = min::vec2<double>(3.0, 3.0);
    out = out && test(false, two.any_zero_outside(one, vmin, vmax), "Failed vec2 not any_zero_outside operation");

    // Test inverse
    one = min::vec2<double>(-2.0, 2.0).inverse();
    out = out && test(-0.5, one.x(), 1E-4, "Failed vec2 inverse operation");
    out = out && test(0.5, one.y(), 1E-4, "Failed vec2 inverse operation");

    // Test max
    one = min::vec2<double>(-2.0, 2.0);
    double max = one.max();
    out = out && test(2.0, max, 1E-4, "Failed vec2 max operation");

    // Test min
    one = min::vec2<double>(-2.0, 2.0);
    double min = one.min();
    out = out && test(-2.0, min, 1E-4, "Failed vec2 min operation");

    // Test order
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(-2.0, 4.0);
    min::vec2<double>::order(one, two);
    out = out && test(-2.0, one.x(), 1E-4, "Failed vec2 order operation");
    out = out && test(-1.0, one.y(), 1E-4, "Failed vec2 order operation");
    out = out && test(-1.0, two.x(), 1E-4, "Failed vec2 order operation");
    out = out && test(4.0, two.y(), 1E-4, "Failed vec2 order operation");

    // Test within
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(0.0, 0.0);
    three = min::vec2<double>(3.0, 3.0);
    min::vec2<double>::order(one, two);
    out = out && test(true, two.within(one, three), "Failed vec2 within operation");

    // Test greater than
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(0.0, 0.0);
    out = out && test(true, two > one, "Failed vec2 less than operation");

    // Test less than
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(0.0, 0.0);
    out = out && test(true, one < two, "Failed vec2 greater than operation");

    // Test greater than equal
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(0.0, 0.0);
    out = out && test(true, two >= one, "Failed vec2 greater than equal operation");

    // Test less than equal
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(0.0, 0.0);
    out = out && test(true, one <= two, "Failed vec2 less than equal operation");

    // Test sub-divide and sub-divide center
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(1.0, 1.0);
    std::vector<std::pair<min::vec2<double>, min::vec2<double>>> sub;
    std::vector<std::pair<min::vec2<double>, double>> subc;
    min::vec2<double>::subdivide(sub, one, two);
    min::vec2<double>::subdivide_center(subc, one, two);

    // Test zero
    out = out && test(-1.0, sub[0].first.x(), 1E-4, "Failed vec2 subdivide 0");
    out = out && test(-1.0, sub[0].first.y(), 1E-4, "Failed vec2 subdivide 0");
    out = out && test(0.0, sub[0].second.x(), 1E-4, "Failed vec2 subdivide 0");
    out = out && test(0.0, sub[0].second.y(), 1E-4, "Failed vec2 subdivide 0");

    // Test zero center
    out = out && test(-0.5, subc[0].first.x(), 1E-4, "Failed vec2 subdivide_center 0");
    out = out && test(-0.5, subc[0].first.y(), 1E-4, "Failed vec2 subdivide_center 0");

    // Test one
    out = out && test(-1.0, sub[1].first.x(), 1E-4, "Failed vec2 subdivide 1");
    out = out && test(0.0, sub[1].first.y(), 1E-4, "Failed vec2 subdivide 1");
    out = out && test(0.0, sub[1].second.x(), 1E-4, "Failed vec2 subdivide 1");
    out = out && test(1.0, sub[1].second.y(), 1E-4, "Failed vec2 subdivide 1");

    // Test one center
    out = out && test(-0.5, subc[1].first.x(), 1E-4, "Failed vec2 subdivide_center 1");
    out = out && test(0.5, subc[1].first.y(), 1E-4, "Failed vec2 subdivide_center 1");

    // Test two
    out = out && test(0.0, sub[2].first.x(), 1E-4, "Failed vec2 subdivide 2");
    out = out && test(-1.0, sub[2].first.y(), 1E-4, "Failed vec2 subdivide 2");
    out = out && test(1.0, sub[2].second.x(), 1E-4, "Failed vec2 subdivide 2");
    out = out && test(0.0, sub[2].second.y(), 1E-4, "Failed vec2 subdivide 2");

    // Test two center
    out = out && test(0.5, subc[2].first.x(), 1E-4, "Failed vec2 subdivide_center 2");
    out = out && test(-0.5, subc[2].first.y(), 1E-4, "Failed vec2 subdivide_center 2");

    // Test three
    out = out && test(0.0, sub[3].first.x(), 1E-4, "Failed vec2 subdivide 3");
    out = out && test(0.0, sub[3].first.y(), 1E-4, "Failed vec2 subdivide 3");
    out = out && test(1.0, sub[3].second.x(), 1E-4, "Failed vec2 subdivide 3");
    out = out && test(1.0, sub[3].second.y(), 1E-4, "Failed vec2 subdivide 3");

    // Test three center
    out = out && test(0.5, subc[3].first.x(), 1E-4, "Failed vec2 subdivide_center 3");
    out = out && test(0.5, subc[3].first.y(), 1E-4, "Failed vec2 subdivide_center 3");

    // Test subdivide_ray 1
    min::vec2<double> origin(300.0, 100.0);
    min::vec2<double> direction = min::vec2<double>(-1.0, -1.0).normalize();
    min::vec2<double> inverse = direction.inverse();
    vmin = min::vec2<double>(-100000.0, -100000.0);
    vmax = min::vec2<double>(100000.0, 100000.0);
    min::stack_vector<uint_fast8_t, min::vec2<double>::sub_size()> keys;
    keys = min::vec2<double>::subdivide_ray(vmin, vmax, origin, direction, inverse);
    out = out && test(3, keys.size(), "Failed vec2 subdivide_ray 1");
    out = out && test(3, keys[0], "Failed vec2 subdivide_ray 1");
    out = out && test(2, keys[1], "Failed vec2 subdivide_ray 1");
    out = out && test(0, keys[2], "Failed vec2 subdivide_ray 1");

    // Test subdivide_ray 2
    origin = min::vec2<double>(-300.0, -100.0);
    direction = min::vec2<double>(1.0, 1.0).normalize();
    inverse = direction.inverse();
    keys = min::vec2<double>::subdivide_ray(vmin, vmax, origin, direction, inverse);
    out = out && test(3, keys.size(), "Failed vec2 subdivide_ray 2");
    out = out && test(0, keys[0], "Failed vec2 subdivide_ray 2");
    out = out && test(1, keys[1], "Failed vec2 subdivide_ray 2");
    out = out && test(3, keys[2], "Failed vec2 subdivide_ray 2");

    // Test subdivide_ray 3
    origin = min::vec2<double>(-300.0, -300.0);
    direction = min::vec2<double>(-1.0, -1.0).normalize();
    inverse = direction.inverse();
    keys = min::vec2<double>::subdivide_ray(vmin, vmax, origin, direction, inverse);
    out = out && test(4, keys.size(), "Failed vec2 subdivide_ray 3");
    out = out && test(3, keys[0], "Failed vec2 subdivide_ray 3");
    out = out && test(2, keys[1], "Failed vec2 subdivide_ray 3");
    out = out && test(1, keys[2], "Failed vec2 subdivide_ray 3");
    out = out && test(0, keys[3], "Failed vec2 subdivide_ray 3");

    // Test subdivide_ray 4
    origin = min::vec2<double>(-1E-7, 1E-7);
    direction = min::vec2<double>(1.0, -1.0).normalize();
    inverse = direction.inverse();
    keys = min::vec2<double>::subdivide_ray(vmin, vmax, origin, direction, inverse);
    out = out && test(4, keys.size(), "Failed vec2 subdivide_ray 4");
    out = out && test(1, keys[0], "Failed vec2 subdivide_ray 4");
    out = out && test(3, keys[1], "Failed vec2 subdivide_ray 4");
    out = out && test(0, keys[2], "Failed vec2 subdivide_ray 4");
    out = out && test(2, keys[3], "Failed vec2 subdivide_ray 4");

    // Test subdivide_ray 5
    origin = min::vec2<double>(-99999.0, 99999.0);
    direction = min::vec2<double>(0.0, -1.0).normalize();
    inverse = direction.inverse();
    keys = min::vec2<double>::subdivide_ray(vmin, vmax, origin, direction, inverse);
    out = out && test(2, keys.size(), "Failed vec2 subdivide_ray 5");
    out = out && test(1, keys[0], "Failed vec2 subdivide_ray 5");
    out = out && test(0, keys[1], "Failed vec2 subdivide_ray 5");

    // Test subdivide_overlap
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(1.0, 1.0);
    three = min::vec2<double>(0.0, 0.0);
    min::stack_vector<uint_fast8_t, min::vec2<double>::sub_size()> sub_over;
    sub_over = min::vec2<double>::subdivide_overlap(one, two, three);
    out = out && test(0, sub_over[0], "Failed vec2 subdivide_overlap");
    out = out && test(1, sub_over[1], "Failed vec2 subdivide_overlap");
    out = out && test(2, sub_over[2], "Failed vec2 subdivide_overlap");
    out = out && test(3, sub_over[3], "Failed vec2 subdivide_overlap");

    // Test grid and grid center
    one = min::vec2<double>(-1.0, -1.0);
    two = min::vec2<double>(1.0, 1.0);
    std::vector<std::pair<min::vec2<double>, min::vec2<double>>> grid;
    std::vector<std::pair<min::vec2<double>, double>> gridc;
    min::vec2<double>::grid(grid, one, two, 2);
    min::vec2<double>::grid_center(gridc, one, two, 2, 1.414);

    // Test zero
    out = out && test(-1.0, grid[0].first.x(), 1E-4, "Failed vec2 grid 0");
    out = out && test(-1.0, grid[0].first.y(), 1E-4, "Failed vec2 grid 0");
    out = out && test(0.0, grid[0].second.x(), 1E-4, "Failed vec2 grid 0");
    out = out && test(0.0, grid[0].second.y(), 1E-4, "Failed vec2 grid 0");

    // Test zero center
    out = out && test(-0.5, gridc[0].first.x(), 1E-4, "Failed vec2 grid_center 0");
    out = out && test(-0.5, gridc[0].first.y(), 1E-4, "Failed vec2 grid_center 0");

    // Test one
    out = out && test(-1.0, grid[1].first.x(), 1E-4, "Failed vec2 grid 1");
    out = out && test(0.0, grid[1].first.y(), 1E-4, "Failed vec2 grid 1");
    out = out && test(0.0, grid[1].second.x(), 1E-4, "Failed vec2 grid 1");
    out = out && test(1.0, grid[1].second.y(), 1E-4, "Failed vec2 grid 1");

    // Test one center
    out = out && test(-0.5, gridc[1].first.x(), 1E-4, "Failed vec2 grid_center 1");
    out = out && test(0.5, gridc[1].first.y(), 1E-4, "Failed vec2 grid_center 1");

    // Test two
    out = out && test(0.0, grid[2].first.x(), 1E-4, "Failed vec2 grid 2");
    out = out && test(-1.0, grid[2].first.y(), 1E-4, "Failed vec2 grid 2");
    out = out && test(1.0, grid[2].second.x(), 1E-4, "Failed vec2 grid 2");
    out = out && test(0.0, grid[2].second.y(), 1E-4, "Failed vec2 grid 2");

    // Test two center
    out = out && test(0.5, gridc[2].first.x(), 1E-4, "Failed vec2 grid_center 2");
    out = out && test(-0.5, gridc[2].first.y(), 1E-4, "Failed vec2 grid_center 2");

    // Test three
    out = out && test(0.0, grid[3].first.x(), 1E-4, "Failed vec2 grid 3");
    out = out && test(0.0, grid[3].first.y(), 1E-4, "Failed vec2 grid 3");
    out = out && test(1.0, grid[3].second.x(), 1E-4, "Failed vec2 grid 3");
    out = out && test(1.0, grid[3].second.y(), 1E-4, "Failed vec2 grid 3");

    // Test three center
    out = out && test(0.5, gridc[3].first.x(), 1E-4, "Failed vec2 grid_center 3");
    out = out && test(0.5, gridc[3].first.y(), 1E-4, "Failed vec2 grid_center 3");

    // Test grid key 1
    three = min::vec2<double>(-0.5, 0.5);
    size_t key = min::vec2<double>::grid_key(one, two, 2, three);
    out = out && test(1, key, "Failed vec2 grid key 1");

    // Test grid key 3
    three = min::vec2<double>(0.5, 0.5);
    key = min::vec2<double>::grid_key(one, two, 2, three);
    out = out && test(3, key, "Failed vec2 grid key 3");

    // Test grid_index from point
    min::vec2<double> cell_extent(1.0, 1.0);
    origin = min::vec2<double>(0.5, 0.5);
    direction = min::vec2<double>(0.0, 1.0);
    inverse = direction.inverse();
    vmin = min::vec2<double>(-100.0, -100.0);
    auto index = min::vec2<double>::grid_index(vmin, cell_extent, origin);
    out = out && test(100, index.x(), "Failed vec2 grid_index 1");
    out = out && test(100, index.y(), "Failed vec2 grid_index 1");

    // Test grid_index from key
    key = min::vec2<double>::grid_key(vmin, cell_extent, 200, origin);
    index = min::vec2<double>::grid_index(key, 200);
    out = out && test(100, index.x(), "Failed vec2 grid_index 2");
    out = out && test(100, index.y(), "Failed vec2 grid_index 2");

    // Test grid_ray
    auto t = min::vec2<double>::grid_ray(vmin, cell_extent, origin, direction, inverse);
    out = out && test(std::numeric_limits<double>::max(), std::get<0>(t), 1E-4, "Failed vec2 grid_ray");
    out = out && test(std::numeric_limits<double>::max(), std::get<1>(t), 1E-4, "Failed vec2 grid_ray");
    out = out && test(0.5, std::get<2>(t), 1E-4, "Failed vec2 grid_ray");
    out = out && test(1.0, std::get<3>(t), 1E-4, "Failed vec2 grid_ray");
    out = out && test(1, std::get<4>(t), "Failed vec2 grid_ray");
    out = out && test(1, std::get<5>(t), "Failed vec2 grid_ray");

    // Test grid_ray_next
    bool flag = false;
    key = min::vec2<double>::grid_ray_next(index, t, flag, 200);
    out = out && test(20101, key, "Failed vec2 grid_ray_next");

    // Test grid_ray on diagonal
    origin = min::vec2<double>(0.0, 0.0);
    direction = min::vec2<double>(1.0, 1.0);
    inverse = direction.inverse();
    index = min::vec2<double>::grid_index(vmin, cell_extent, origin);
    t = min::vec2<double>::grid_ray(vmin, cell_extent, origin, direction, inverse);
    out = out && test(1.0, std::get<0>(t), 1E-4, "Failed vec2 grid_ray diagonal");
    out = out && test(1.0, std::get<1>(t), 1E-4, "Failed vec2 grid_ray diagonal");
    out = out && test(1.0, std::get<2>(t), 1E-4, "Failed vec2 grid_ray diagonal");
    out = out && test(1.0, std::get<3>(t), 1E-4, "Failed vec2 grid_ray diagonal");
    out = out && test(1, std::get<4>(t), "Failed vec2 grid_ray diagonal");
    out = out && test(1, std::get<5>(t), "Failed vec2 grid_ray diagonal");

    flag = false;
    key = min::vec2<double>::grid_key(vmin, cell_extent, 200.0, origin);
    out = out && test(20100, key, "Failed vec2 grid_ray_next diagonal 1");

    key = min::vec2<double>::grid_ray_next(index, t, flag, 200.0);
    out = out && test(20300, key, "Failed vec2 grid_ray_next diagonal 2");

    key = min::vec2<double>::grid_ray_next(index, t, flag, 200.0);
    out = out && test(20301, key, "Failed vec2 grid_ray_next diagonal 3");

    key = min::vec2<double>::grid_ray_next(index, t, flag, 200.0);
    out = out && test(20501, key, "Failed vec2 grid_ray_next diagonal 4");

    key = min::vec2<double>::grid_ray_next(index, t, flag, 200.0);
    out = out && test(20502, key, "Failed vec2 grid_ray_next diagonal 5");

    // Test grid overlap
    one = min::vec2<double>(-10000.0, -10000.0);
    two = min::vec2<double>(10000.0, 10000.0);
    vmin = min::vec2<double>(-100.0, -100.0);
    vmax = min::vec2<double>(100.0, 100.0);
    min::stack_vector<size_t, min::vec2<double>::over_size()> over;
    over = min::vec2<double>::grid_overlap(one, (two - one) / 40, 40, vmin, vmax);
    out = out && test(4, over.size(), "Failed vec2 grid_overlap");

    // Test grid sat penetration aligned
    one = min::vec2<double>(10.0, 10.0);
    two = min::vec2<double>(8.0, 8.0);
    vmin = min::vec2<double>(3.0, 3.0);
    vmax = min::vec2<double>(3.0, 3.0);

    // center, half_extent, center, half_extent
    std::pair<min::vec2<double>, double> p = min::vec2<double>::project_sat_aligned_penetration(one, vmin, two, vmax, 1E-6);
    out = out && test(1.0, p.first.x(), 1E-4, "Failed vec2 sat penetration aligned");
    out = out && test(0.0, p.first.y(), 1E-4, "Failed vec2 sat penetration aligned");
    out = out && test(4.0, p.second, 1E-4, "Failed vec2 sat penetration aligned");

    // Test grid sat penetration
    one = min::vec2<double>(10.0, 10.0);
    two = min::vec2<double>(8.0, 8.0);
    vmin = min::vec2<double>(3.0, 3.0);
    vmax = min::vec2<double>(3.0, 3.0);

    // center, half_extent, center, half_extent
    p = min::vec2<double>::project_sat_penetration(min::vec2<double>::axes(), one, vmin, min::vec2<double>::axes(), two, vmax, 1E-6);
    out = out && test(0.0, p.first.x(), 1E-4, "Failed vec2 sat penetration");
    out = out && test(1.0, p.first.y(), 1E-4, "Failed vec2 sat penetration");
    out = out && test(4.0, p.second, 1E-4, "Failed vec2 sat penetration");

    return out;
}

#endif
