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
#ifndef _MGL_TESTVECTOR4_MGL_
#define _MGL_TESTVECTOR4_MGL_

#include <min/test.h>
#include <min/vec4.h>

bool test_vec4()
{
    bool out = true;

    // Print size and alignment of class
    std::cout << "vec4_size: " << sizeof(min::vec4<float>) << std::endl;
    std::cout << "vec4_align: " << alignof(min::vec4<float>) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "tvec4.h: Testing alignment" << std::endl;
    out = out && test(sizeof(float) * 4, sizeof(min::vec4<float>), "Failed vec4 sizeof");
    out = out && test(sizeof(float), alignof(min::vec4<float>), "Failed vec4 alignof");
#endif

    // Local variables
    min::vec4<double> one;
    min::vec4<double> two;
    min::vec4<double> three;
    min::vec4<double> vmin;
    min::vec4<double> vmax;

    // Test arithmetic
    one = min::vec4<double>(3.0, 1.0, 2.0, 1.0);
    one += 2.0;
    one -= 1.0;
    one *= 3.0;
    one /= 2.0;

    two = min::vec4<double>(2.0, 3.0, 4.0, 1.0);
    three = min::vec4<double>(1.0, 2.0, 3.0, 1.0);
    one += two;
    one -= three;
    one *= two;
    one /= three;

    two = min::vec4<double>(1.0, 1.0, 1.0, 1.0);
    one = one + two;
    one = one - two;
    two = min::vec4<double>(2.0, 2.0, 2.0, 1.0);
    one = one * two;
    one = one / two;

    // Should be 14.0, 6.0, 7.333
    out = out && test(14.0, one.x(), 1E-4, "Failed vec4 arithmetic operation");
    out = out && test(6.0, one.y(), 1E-4, "Failed vec4 arithmetic operation");
    out = out && test(7.3333, one.z(), 1E-4, "Failed vec4 arithmetic operation");
    out = out && test(1.0, one.w(), 1E-4, "Failed vec4 arithmetic operation");

    // Test LERP
    one = min::vec4<double>(1.0, 2.0, 3.0, 1.0);
    two = min::vec4<double>(2.0, 4.0, 6.0, 1.0);
    three = min::vec4<double>::lerp(one, two, 0.5);

    // Should be 1.5, 3, 4.5
    out = out && test(1.5, three.x(), 1E-4, "Failed vec4 lerp operation");
    out = out && test(3.0, three.y(), 1E-4, "Failed vec4 lerp operation");
    out = out && test(4.5, three.z(), 1E-4, "Failed vec4 lerp operation");
    out = out && test(1.0, three.w(), 1E-4, "Failed vec4 lerp operation");

    // Test dot product; should be 21
    double dot = one.dot(three);
    out = out && test(21.0, dot, 1E-4, "Failed vec4 dot product operation");

    // Test cross product; should be Y x Z = X
    one = min::vec4<double>(0.0, 1.0, 0.0, 1.0);
    two = min::vec4<double>(0.0, 0.0, 1.0, 1.0);
    three = one.cross(two);
    out = out && test(1.0, three.x(), 1E-4, "Failed vec4 cross product X operation");
    out = out && test(0.0, three.y(), 1E-4, "Failed vec4 cross product X operation");
    out = out && test(0.0, three.z(), 1E-4, "Failed vec4 cross product X operation");

    // Test cross product; should be Z x X = Y
    one = min::vec4<double>(0.0, 0.0, 1.0, 1.0);
    two = min::vec4<double>(1.0, 0.0, 0.0, 1.0);
    three = one.cross(two);
    out = out && test(0.0, three.x(), 1E-4, "Failed vec4 cross product Y operation");
    out = out && test(1.0, three.y(), 1E-4, "Failed vec4 cross product Y operation");
    out = out && test(0.0, three.z(), 1E-4, "Failed vec4 cross product Y operation");

    // Test cross product; should be X x Y = Z
    one = min::vec4<double>(1.0, 0.0, 0.0, 1.0);
    two = min::vec4<double>(0.0, 1.0, 0.0, 1.0);
    three = one.cross(two);
    out = out && test(0.0, three.x(), 1E-4, "Failed vec4 cross product Z operation");
    out = out && test(0.0, three.y(), 1E-4, "Failed vec4 cross product Z operation");
    out = out && test(1.0, three.z(), 1E-4, "Failed vec4 cross product Z operation");

    // Test magnitude; should be 3.74
    one = min::vec4<double>(1.0, 2.0, 3.0, 1.0);
    double mag = one.magnitude();
    out = out && test(3.7416, mag, 1E-4, "Failed vec4 magnitude operation");

    // Test normalize
    one = min::vec4<double>(1.0, 2.0, 3.0, 1.0);
    one.normalize();
    out = out && test(0.2672, one.x(), 1E-4, "Failed vec4 normalize operation");
    out = out && test(0.5345, one.y(), 1E-4, "Failed vec4 normalize operation");
    out = out && test(0.8017, one.z(), 1E-4, "Failed vec4 normalize operation");
    out = out && test(1.0, one.w(), 1E-4, "Failed vec4 normalize operation");

    // Test normalize_unsafe
    one = min::vec4<double>(1.0, 2.0, 3.0, 1.0);
    one.normalize_unsafe();
    out = out && test(0.2672, one.x(), 1E-4, "Failed vec4 normalize_unsafe operation");
    out = out && test(0.5345, one.y(), 1E-4, "Failed vec4 normalize_unsafe operation");
    out = out && test(0.8017, one.z(), 1E-4, "Failed vec4 normalize_unsafe operation");
    out = out && test(1.0, one.w(), 1E-4, "Failed vec4 normalize_unsafe operation");

    // Test normalize_safe
    one = min::vec4<double>(0.0, 0.0, 0.0, 1.0);
    one.normalize_safe(min::vec4<double>::up());
    out = out && test(0.0, one.x(), 1E-4, "Failed vec4 normalize_safe operation");
    out = out && test(1.0, one.y(), 1E-4, "Failed vec4 normalize_safe operation");
    out = out && test(0.0, one.z(), 1E-4, "Failed vec4 normalize_safe operation");
    out = out && test(1.0, one.w(), 1E-4, "Failed vec4 normalize_safe operation");

    // Test normalize fallback
    one = min::vec4<double>(0.0, 0.0, 0.0, 1.0);
    one.normalize();
    out = out && test(0.0, one.x(), 1E-4, "Failed vec4 normalize fallback operation");
    out = out && test(0.0, one.y(), 1E-4, "Failed vec4 normalize fallback operation");
    out = out && test(0.0, one.z(), 1E-4, "Failed vec4 normalize fallback operation");
    out = out && test(1.0, one.w(), 1E-4, "Failed vec4 normalize fallback operation");

    // Test clamp
    one = min::vec4<double>(-1.0, -1.0, -2.0, 0.0);
    two = min::vec4<double>(-2.0, 4.0, -3.0, 0.0);
    three = min::vec4<double>(3.0, 3.0, 3.0, 0.0);
    two = two.clamp(one, three);
    out = out && test(-1.0, two.x(), 1E-4, "Failed vec4 clamp operation");
    out = out && test(3.0, two.y(), 1E-4, "Failed vec4 clamp operation");
    out = out && test(-2.0, two.z(), 1E-4, "Failed vec4 clamp operation");

    // Test clamp direction
    one = min::vec4<double>(-1.0, -1.0, -2.0, 0.0);
    two = min::vec4<double>(2.0, 4.0, -3.0, 0.0);
    three = min::vec4<double>(3.0, 3.0, 3.0, 0.0);
    two = two.clamp_direction(one, three);
    out = out && test(1.0, two.x(), 1E-4, "Failed vec4 clamp operation");
    out = out && test(-1.0, two.y(), 1E-4, "Failed vec4 clamp operation");
    out = out && test(-1.0, two.z(), 1E-4, "Failed vec4 clamp operation");

    // Test any_zero_outside
    one = min::vec4<double>(0.0, -1.0, 1.0, 1.0);
    two = min::vec4<double>(1.0, 1.0, 1.0, 1.0);
    vmin = min::vec4<double>(2.0, 2.0, 2.0, 1.0);
    vmax = min::vec4<double>(3.0, 3.0, 3.0, 1.0);
    out = out && test(true, two.any_zero_outside(one, vmin, vmax), "Failed vec4 any_zero_outside operation");

    // Test not any_zero_outside
    // Test any_zero_outside
    one = min::vec4<double>(0.0, -1.0, 1.0, 1.0);
    two = min::vec4<double>(2.5, 2.5, 2.5, 1.0);
    vmin = min::vec4<double>(2.0, 2.0, 2.0, 1.0);
    vmax = min::vec4<double>(3.0, 3.0, 3.0, 1.0);
    out = out && test(true, !two.any_zero_outside(one, vmin, vmax), "Failed vec4 not any_zero_outside operation");

    // Test inverse
    one = min::vec4<double>(-2.0, 2.0, 3.0, 0.0);
    one = one.inverse();
    out = out && test(-0.5, one.x(), 1E-4, "Failed vec4 inverse operation");
    out = out && test(0.5, one.y(), 1E-4, "Failed vec4 inverse operation");
    out = out && test(0.3333, one.z(), 1E-4, "Failed vec4 inverse operation");

    // Test max
    one = min::vec4<double>(-2.0, 2.0, 5.0, 0.0);
    double max = one.max();
    out = out && test(5.0, max, 1E-4, "Failed vec4 max operation");

    // Test min
    one = min::vec4<double>(-2.0, 2.0, -10.0, 0.0);
    double min = one.min();
    out = out && test(-10.0, min, 1E-4, "Failed vec4 min operation");

    // Test order
    one = min::vec4<double>(-1.0, -1.0, 5.0, 0.0);
    two = min::vec4<double>(-2.0, 4.0, 1.0, 0.0);
    min::vec4<double>::order(one, two);
    out = out && test(-2.0, one.x(), 1E-4, "Failed vec4 order operation");
    out = out && test(-1.0, one.y(), 1E-4, "Failed vec4 order operation");
    out = out && test(1.0, one.z(), 1E-4, "Failed vec4 order operation");
    out = out && test(-1.0, two.x(), 1E-4, "Failed vec4 order operation");
    out = out && test(4.0, two.y(), 1E-4, "Failed vec4 order operation");
    out = out && test(5.0, two.z(), 1E-4, "Failed vec4 order operation");

    // Test within
    one = min::vec4<double>(-1.0, -1.0, -1.0, 0.0);
    two = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
    three = min::vec4<double>(3.0, 3.0, 3.0, 0.0);
    min::vec4<double>::order(one, two);
    out = out && test(true, two.within(one, three), "Failed vec4 within operation");

    // Test greater than
    one = min::vec4<double>(-1.0, -1.0, -1.0, 0.0);
    two = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
    out = out && test(true, two > one, "Failed vec4 less than operation");

    // Test less than
    one = min::vec4<double>(-1.0, -1.0, -1.0, 0.0);
    two = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
    out = out && test(true, one < two, "Failed vec4 greater than operation");

    // Test greater than equal
    one = min::vec4<double>(-1.0, -1.0, -1.0, 0.0);
    two = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
    out = out && test(true, two >= one, "Failed vec4 greater than equal operation");

    // Test less than equal
    one = min::vec4<double>(-1.0, -1.0, -1.0, 0.0);
    two = min::vec4<double>(0.0, 0.0, 0.0, 0.0);
    out = out && test(true, one <= two, "Failed vec4 less than equal operation");

    // Test sub-divide
    one = min::vec4<double>(-1.0, -1.0, -1.0, 1.0);
    two = min::vec4<double>(1.0, 1.0, 1.0, 1.0);
    std::vector<std::pair<min::vec4<double>, min::vec4<double>>> sub;
    std::vector<std::pair<min::vec4<double>, double>> subc;
    min::vec4<double>::subdivide(sub, one, two);
    min::vec4<double>::subdivide_center(subc, one, two);

    // Test zero
    out = out && test(-1.0, sub[0].first.x(), 1E-4, "Failed vec4 subdivide 0");
    out = out && test(-1.0, sub[0].first.y(), 1E-4, "Failed vec4 subdivide 0");
    out = out && test(-1.0, sub[0].first.z(), 1E-4, "Failed vec4 subdivide 0");
    out = out && test(0.0, sub[0].second.x(), 1E-4, "Failed vec4 subdivide 0");
    out = out && test(0.0, sub[0].second.y(), 1E-4, "Failed vec4 subdivide 0");
    out = out && test(0.0, sub[0].second.z(), 1E-4, "Failed vec4 subdivide 0");

    // Test zero center
    out = out && test(-0.5, subc[0].first.x(), 1E-4, "Failed vec4 subdivide_center 0");
    out = out && test(-0.5, subc[0].first.y(), 1E-4, "Failed vec4 subdivide_center 0");
    out = out && test(-0.5, subc[0].first.z(), 1E-4, "Failed vec4 subdivide_center 0");

    // Test one
    out = out && test(-1.0, sub[1].first.x(), 1E-4, "Failed vec4 subdivide 1");
    out = out && test(-1.0, sub[1].first.y(), 1E-4, "Failed vec4 subdivide 1");
    out = out && test(0.0, sub[1].first.z(), 1E-4, "Failed vec4 subdivide 1");
    out = out && test(0.0, sub[1].second.x(), 1E-4, "Failed vec4 subdivide 1");
    out = out && test(0.0, sub[1].second.y(), 1E-4, "Failed vec4 subdivide 1");
    out = out && test(1.0, sub[1].second.z(), 1E-4, "Failed vec4 subdivide 1");

    // Test one center
    out = out && test(-0.5, subc[1].first.x(), 1E-4, "Failed vec4 subdivide_center 1");
    out = out && test(-0.5, subc[1].first.y(), 1E-4, "Failed vec4 subdivide_center 1");
    out = out && test(0.5, subc[1].first.z(), 1E-4, "Failed vec4 subdivide_center 1");

    // Test two
    out = out && test(-1.0, sub[2].first.x(), 1E-4, "Failed vec4 subdivide 2");
    out = out && test(0.0, sub[2].first.y(), 1E-4, "Failed vec4 subdivide 2");
    out = out && test(-1.0, sub[2].first.z(), 1E-4, "Failed vec4 subdivide 2");
    out = out && test(0.0, sub[2].second.x(), 1E-4, "Failed vec4 subdivide 2");
    out = out && test(1.0, sub[2].second.y(), 1E-4, "Failed vec4 subdivide 2");
    out = out && test(0.0, sub[2].second.z(), 1E-4, "Failed vec4 subdivide 2");

    // Test two center
    out = out && test(-0.5, subc[2].first.x(), 1E-4, "Failed vec4 subdivide_center 2");
    out = out && test(0.5, subc[2].first.y(), 1E-4, "Failed vec4 subdivide_center 2");
    out = out && test(-0.5, subc[2].first.z(), 1E-4, "Failed vec4 subdivide_center 2");

    // Test three
    out = out && test(-1.0, sub[3].first.x(), 1E-4, "Failed vec4 subdivide 3");
    out = out && test(0.0, sub[3].first.y(), 1E-4, "Failed vec4 subdivide 3");
    out = out && test(0.0, sub[3].first.z(), 1E-4, "Failed vec4 subdivide 3");
    out = out && test(0.0, sub[3].second.x(), 1E-4, "Failed vec4 subdivide 3");
    out = out && test(1.0, sub[3].second.y(), 1E-4, "Failed vec4 subdivide 3");
    out = out && test(1.0, sub[3].second.z(), 1E-4, "Failed vec4 subdivide 3");

    // Test three center
    out = out && test(-0.5, subc[3].first.x(), 1E-4, "Failed vec4 subdivide_center 3");
    out = out && test(0.5, subc[3].first.y(), 1E-4, "Failed vec4 subdivide_center 3");
    out = out && test(0.5, subc[3].first.z(), 1E-4, "Failed vec4 subdivide_center 3");

    // Test four
    out = out && test(0.0, sub[4].first.x(), 1E-4, "Failed vec4 subdivide 4");
    out = out && test(-1.0, sub[4].first.y(), 1E-4, "Failed vec4 subdivide 4");
    out = out && test(-1.0, sub[4].first.z(), 1E-4, "Failed vec4 subdivide 4");
    out = out && test(1.0, sub[4].second.x(), 1E-4, "Failed vec4 subdivide 4");
    out = out && test(0.0, sub[4].second.y(), 1E-4, "Failed vec4 subdivide 4");
    out = out && test(0.0, sub[4].second.z(), 1E-4, "Failed vec4 subdivide 4");

    // Test four center
    out = out && test(0.5, subc[4].first.x(), 1E-4, "Failed vec4 subdivide_center 4");
    out = out && test(-0.5, subc[4].first.y(), 1E-4, "Failed vec4 subdivide_center 4");
    out = out && test(-0.5, subc[4].first.z(), 1E-4, "Failed vec4 subdivide_center 4");

    // Test five
    out = out && test(0.0, sub[5].first.x(), 1E-4, "Failed vec4 subdivide 5");
    out = out && test(-1.0, sub[5].first.y(), 1E-4, "Failed vec4 subdivide 5");
    out = out && test(0.0, sub[5].first.z(), 1E-4, "Failed vec4 subdivide 5");
    out = out && test(1.0, sub[5].second.x(), 1E-4, "Failed vec4 subdivide 5");
    out = out && test(0.0, sub[5].second.y(), 1E-4, "Failed vec4 subdivide 5");
    out = out && test(1.0, sub[5].second.z(), 1E-4, "Failed vec4 subdivide 5");

    // Test five center
    out = out && test(0.5, subc[5].first.x(), 1E-4, "Failed vec4 subdivide_center 5");
    out = out && test(-0.5, subc[5].first.y(), 1E-4, "Failed vec4 subdivide_center 5");
    out = out && test(0.5, subc[5].first.z(), 1E-4, "Failed vec4 subdivide_center 5");

    // Test six
    out = out && test(0.0, sub[6].first.x(), 1E-4, "Failed vec4 subdivide 6");
    out = out && test(0.0, sub[6].first.y(), 1E-4, "Failed vec4 subdivide 6");
    out = out && test(-1.0, sub[6].first.z(), 1E-4, "Failed vec4 subdivide 6");
    out = out && test(1.0, sub[6].second.x(), 1E-4, "Failed vec4 subdivide 6");
    out = out && test(1.0, sub[6].second.y(), 1E-4, "Failed vec4 subdivide 6");
    out = out && test(0.0, sub[6].second.z(), 1E-4, "Failed vec4 subdivide 6");

    // Test six center
    out = out && test(0.5, subc[6].first.x(), 1E-4, "Failed vec4 subdivide_center 6");
    out = out && test(0.5, subc[6].first.y(), 1E-4, "Failed vec4 subdivide_center 6");
    out = out && test(-0.5, subc[6].first.z(), 1E-4, "Failed vec4 subdivide_center 6");

    // Test seven
    out = out && test(0.0, sub[7].first.x(), 1E-4, "Failed vec4 subdivide 7");
    out = out && test(0.0, sub[7].first.y(), 1E-4, "Failed vec4 subdivide 7");
    out = out && test(0.0, sub[7].first.z(), 1E-4, "Failed vec4 subdivide 7");
    out = out && test(1.0, sub[7].second.x(), 1E-4, "Failed vec4 subdivide 7");
    out = out && test(1.0, sub[7].second.y(), 1E-4, "Failed vec4 subdivide 7");
    out = out && test(1.0, sub[7].second.z(), 1E-4, "Failed vec4 subdivide 7");

    // Test seven center
    out = out && test(0.5, subc[7].first.x(), 1E-4, "Failed vec4 subdivide_center 7");
    out = out && test(0.5, subc[7].first.y(), 1E-4, "Failed vec4 subdivide_center 7");
    out = out && test(0.5, subc[7].first.z(), 1E-4, "Failed vec4 subdivide_center 7");

    // Test subdivide_ray 1
    min::vec4<double> origin(300.0, 100.0, 50.0, 1.0);
    min::vec4<double> direction = min::vec4<double>(-1.0, -1.0, -1.0, 1.0).normalize();
    min::vec4<double> inverse = direction.inverse();
    vmin = min::vec4<double>(-100000.0, -100000.0, -100000.0, 1.0);
    vmax = min::vec4<double>(100000.0, 100000.0, 100000.0, 1.0);
    min::stack_vector<uint_fast8_t, min::vec4<double>::sub_size()> keys;
    keys = min::vec4<double>::subdivide_ray(vmin, vmax, origin, direction, inverse);
    out = out && test(4, keys.size(), "Failed vec4 subdivide_ray 1");
    out = out && test(7, keys[0], "Failed vec4 subdivide_ray 1");
    out = out && test(6, keys[1], "Failed vec4 subdivide_ray 1");
    out = out && test(4, keys[2], "Failed vec4 subdivide_ray 1");
    out = out && test(0, keys[3], "Failed vec4 subdivide_ray 1");

    // Test subdivide_ray 2
    origin = min::vec4<double>(-300.0, -50.0, -100.0, 1.0);
    direction = min::vec4<double>(1.0, 1.0, 1.0, 1.0).normalize();
    inverse = direction.inverse();
    keys = min::vec4<double>::subdivide_ray(vmin, vmax, origin, direction, inverse);
    out = out && test(4, keys.size(), "Failed vec4 subdivide_ray 2");
    out = out && test(0, keys[0], "Failed vec4 subdivide_ray 2");
    out = out && test(2, keys[1], "Failed vec4 subdivide_ray 2");
    out = out && test(3, keys[2], "Failed vec4 subdivide_ray 2");
    out = out && test(7, keys[3], "Failed vec4 subdivide_ray 2");

    // Test subdivide_ray 3
    origin = min::vec4<double>(-300.0, -300.0, -300.0, 1.0);
    direction = min::vec4<double>(-1.0, -1.0, -1.0, 1.0).normalize();
    inverse = direction.inverse();
    keys = min::vec4<double>::subdivide_ray(vmin, vmax, origin, direction, inverse);
    out = out && test(8, keys.size(), "Failed vec4 subdivide_ray 3");
    out = out && test(7, keys[0], "Failed vec4 subdivide_ray 3");
    out = out && test(6, keys[1], "Failed vec4 subdivide_ray 3");
    out = out && test(3, keys[2], "Failed vec4 subdivide_ray 3");
    out = out && test(2, keys[3], "Failed vec4 subdivide_ray 3");
    out = out && test(5, keys[4], "Failed vec4 subdivide_ray 3");
    out = out && test(4, keys[5], "Failed vec4 subdivide_ray 3");
    out = out && test(1, keys[6], "Failed vec4 subdivide_ray 3");
    out = out && test(0, keys[7], "Failed vec4 subdivide_ray 3");

    // Test subdivide_ray 4
    origin = min::vec4<double>(-1E-7, 1E-7, 1E-7, 1.0);
    direction = min::vec4<double>(1.0, -1.0, -1.0, 1.0).normalize();
    inverse = direction.inverse();
    keys = min::vec4<double>::subdivide_ray(vmin, vmax, origin, direction, inverse);
    out = out && test(8, keys.size(), "Failed vec4 subdivide_ray 4");
    out = out && test(3, keys[0], "Failed vec4 subdivide_ray 4");
    out = out && test(7, keys[1], "Failed vec4 subdivide_ray 4");
    out = out && test(2, keys[2], "Failed vec4 subdivide_ray 4");
    out = out && test(6, keys[3], "Failed vec4 subdivide_ray 4");
    out = out && test(1, keys[4], "Failed vec4 subdivide_ray 4");
    out = out && test(5, keys[5], "Failed vec4 subdivide_ray 4");
    out = out && test(0, keys[6], "Failed vec4 subdivide_ray 4");
    out = out && test(4, keys[7], "Failed vec4 subdivide_ray 4");

    // Test subdivide_ray 5
    origin = min::vec4<double>(-99999.0, 99999.0, -99999.0, 1.0);
    direction = min::vec4<double>(0.0, -1.0, 0.0, 1.0).normalize();
    inverse = direction.inverse();
    keys = min::vec4<double>::subdivide_ray(vmin, vmax, origin, direction, inverse);
    out = out && test(2, keys.size(), "Failed vec4 subdivide_ray 5");
    out = out && test(2, keys[0], "Failed vec4 subdivide_ray 5");
    out = out && test(0, keys[1], "Failed vec4 subdivide_ray 5");

    // Test subdivide_overlap
    one = min::vec4<double>(-1.0, -1.0, -1.0, 1.0);
    two = min::vec4<double>(1.0, 1.0, 1.0, 1.0);
    three = min::vec4<double>(0.0, 0.0, 0.0, 1.0);
    min::stack_vector<uint_fast8_t, min::vec4<double>::sub_size()> sub_over;
    sub_over = min::vec4<double>::subdivide_overlap(one, two, three);
    out = out && test(0, sub_over[0], "Failed vec4 subdivide_overlap");
    out = out && test(1, sub_over[1], "Failed vec4 subdivide_overlap");
    out = out && test(2, sub_over[2], "Failed vec4 subdivide_overlap");
    out = out && test(3, sub_over[3], "Failed vec4 subdivide_overlap");
    out = out && test(4, sub_over[4], "Failed vec4 subdivide_overlap");
    out = out && test(5, sub_over[5], "Failed vec4 subdivide_overlap");
    out = out && test(6, sub_over[6], "Failed vec4 subdivide_overlap");
    out = out && test(7, sub_over[7], "Failed vec4 subdivide_overlap");

    // Test grid and grid center
    one = min::vec4<double>(-1.0, -1.0, -1.0, 1.0);
    two = min::vec4<double>(1.0, 1.0, 1.0, 1.0);
    std::vector<std::pair<min::vec4<double>, min::vec4<double>>> grid;
    std::vector<std::pair<min::vec4<double>, double>> gridc;
    min::vec4<double>::grid(grid, one, two, 2);
    min::vec4<double>::grid_center(gridc, one, two, 2, 1.732);

    // Test zero
    out = out && test(-1.0, grid[0].first.x(), 1E-4, "Failed vec4 grid 0");
    out = out && test(-1.0, grid[0].first.y(), 1E-4, "Failed vec4 grid 0");
    out = out && test(-1.0, grid[0].first.z(), 1E-4, "Failed vec4 grid 0");
    out = out && test(0.0, grid[0].second.x(), 1E-4, "Failed vec4 grid 0");
    out = out && test(0.0, grid[0].second.y(), 1E-4, "Failed vec4 grid 0");
    out = out && test(0.0, grid[0].second.z(), 1E-4, "Failed vec4 grid 0");

    // Test zero center
    out = out && test(-0.5, gridc[0].first.x(), 1E-4, "Failed vec4 grid_center 0");
    out = out && test(-0.5, gridc[0].first.y(), 1E-4, "Failed vec4 grid_center 0");
    out = out && test(-0.5, gridc[0].first.z(), 1E-4, "Failed vec4 grid_center 0");

    // Test one
    out = out && test(-1.0, grid[1].first.x(), 1E-4, "Failed vec4 grid 1");
    out = out && test(-1.0, grid[1].first.y(), 1E-4, "Failed vec4 grid 1");
    out = out && test(0.0, grid[1].first.z(), 1E-4, "Failed vec4 grid 1");
    out = out && test(0.0, grid[1].second.x(), 1E-4, "Failed vec4 grid 1");
    out = out && test(0.0, grid[1].second.y(), 1E-4, "Failed vec4 grid 1");
    out = out && test(1.0, grid[1].second.z(), 1E-4, "Failed vec4 grid 1");

    // Test one center
    out = out && test(-0.5, gridc[1].first.x(), 1E-4, "Failed vec4 grid_center 1");
    out = out && test(-0.5, gridc[1].first.y(), 1E-4, "Failed vec4 grid_center 1");
    out = out && test(0.5, gridc[1].first.z(), 1E-4, "Failed vec4 grid_center 1");

    // Test two
    out = out && test(-1.0, grid[2].first.x(), 1E-4, "Failed vec4 grid 2");
    out = out && test(0.0, grid[2].first.y(), 1E-4, "Failed vec4 grid 2");
    out = out && test(-1.0, grid[2].first.z(), 1E-4, "Failed vec4 grid 2");
    out = out && test(0.0, grid[2].second.x(), 1E-4, "Failed vec4 grid 2");
    out = out && test(1.0, grid[2].second.y(), 1E-4, "Failed vec4 grid 2");
    out = out && test(0.0, grid[2].second.z(), 1E-4, "Failed vec4 grid 2");

    // Test two center
    out = out && test(-0.5, gridc[2].first.x(), 1E-4, "Failed vec4 grid_center 2");
    out = out && test(0.5, gridc[2].first.y(), 1E-4, "Failed vec4 grid_center 2");
    out = out && test(-0.5, gridc[2].first.z(), 1E-4, "Failed vec4 grid_center 2");

    // Test three
    out = out && test(-1.0, grid[3].first.x(), 1E-4, "Failed vec4 grid 3");
    out = out && test(0.0, grid[3].first.y(), 1E-4, "Failed vec4 grid 3");
    out = out && test(0.0, grid[3].first.z(), 1E-4, "Failed vec4 grid 3");
    out = out && test(0.0, grid[3].second.x(), 1E-4, "Failed vec4 grid 3");
    out = out && test(1.0, grid[3].second.y(), 1E-4, "Failed vec4 grid 3");
    out = out && test(1.0, grid[3].second.z(), 1E-4, "Failed vec4 grid 3");

    // Test three center
    out = out && test(-0.5, gridc[3].first.x(), 1E-4, "Failed vec4 grid_center 3");
    out = out && test(0.5, gridc[3].first.y(), 1E-4, "Failed vec4 grid_center 3");
    out = out && test(0.5, gridc[3].first.z(), 1E-4, "Failed vec4 grid_center 3");

    // Test four
    out = out && test(0.0, grid[4].first.x(), 1E-4, "Failed vec4 grid 4");
    out = out && test(-1.0, grid[4].first.y(), 1E-4, "Failed vec4 grid 4");
    out = out && test(-1.0, grid[4].first.z(), 1E-4, "Failed vec4 grid 4");
    out = out && test(1.0, grid[4].second.x(), 1E-4, "Failed vec4 grid 4");
    out = out && test(0.0, grid[4].second.y(), 1E-4, "Failed vec4 grid 4");
    out = out && test(0.0, grid[4].second.z(), 1E-4, "Failed vec4 grid 4");

    // Test four center
    out = out && test(0.5, gridc[4].first.x(), 1E-4, "Failed vec4 grid_center 4");
    out = out && test(-0.5, gridc[4].first.y(), 1E-4, "Failed vec4 grid_center 4");
    out = out && test(-0.5, gridc[4].first.z(), 1E-4, "Failed vec4 grid_center 4");

    // Test five
    out = out && test(0.0, grid[5].first.x(), 1E-4, "Failed vec4 grid 5");
    out = out && test(-1.0, grid[5].first.y(), 1E-4, "Failed vec4 grid 5");
    out = out && test(0.0, grid[5].first.z(), 1E-4, "Failed vec4 grid 5");
    out = out && test(1.0, grid[5].second.x(), 1E-4, "Failed vec4 grid 5");
    out = out && test(0.0, grid[5].second.y(), 1E-4, "Failed vec4 grid 5");
    out = out && test(1.0, grid[5].second.z(), 1E-4, "Failed vec4 grid 5");

    // Test five center
    out = out && test(0.5, gridc[5].first.x(), 1E-4, "Failed vec4 grid_center 5");
    out = out && test(-0.5, gridc[5].first.y(), 1E-4, "Failed vec4 grid_center 5");
    out = out && test(0.5, gridc[5].first.z(), 1E-4, "Failed vec4 grid_center 5");

    // Test six
    out = out && test(0.0, grid[6].first.x(), 1E-4, "Failed vec4 grid 6");
    out = out && test(0.0, grid[6].first.y(), 1E-4, "Failed vec4 grid 6");
    out = out && test(-1.0, grid[6].first.z(), 1E-4, "Failed vec4 grid 6");
    out = out && test(1.0, grid[6].second.x(), 1E-4, "Failed vec4 grid 6");
    out = out && test(1.0, grid[6].second.y(), 1E-4, "Failed vec4 grid 6");
    out = out && test(0.0, grid[6].second.z(), 1E-4, "Failed vec4 grid 6");

    // Test six center
    out = out && test(0.5, gridc[6].first.x(), 1E-4, "Failed vec4 grid_center 6");
    out = out && test(0.5, gridc[6].first.y(), 1E-4, "Failed vec4 grid_center 6");
    out = out && test(-0.5, gridc[6].first.z(), 1E-4, "Failed vec4 grid_center 6");

    // Test seven
    out = out && test(0.0, grid[7].first.x(), 1E-4, "Failed vec4 grid 7");
    out = out && test(0.0, grid[7].first.y(), 1E-4, "Failed vec4 grid 7");
    out = out && test(0.0, grid[7].first.z(), 1E-4, "Failed vec4 grid 7");
    out = out && test(1.0, grid[7].second.x(), 1E-4, "Failed vec4 grid 7");
    out = out && test(1.0, grid[7].second.y(), 1E-4, "Failed vec4 grid 7");
    out = out && test(1.0, grid[7].second.z(), 1E-4, "Failed vec4 grid 7");

    // Test seven center
    out = out && test(0.5, gridc[7].first.x(), 1E-4, "Failed vec4 grid_center 7");
    out = out && test(0.5, gridc[7].first.y(), 1E-4, "Failed vec4 grid_center 7");
    out = out && test(0.5, gridc[7].first.z(), 1E-4, "Failed vec4 grid_center 7");

    // Test grid key 6
    three = min::vec4<double>(0.5, 0.5, -0.5, 1.0);
    size_t key = min::vec4<double>::grid_key(one, two, 2, three);
    out = out && test(6, key, "Failed vec4 grid key 6");

    // Test grid key 7
    three = min::vec4<double>(0.5, 0.5, 0.5, 1.0);
    key = min::vec4<double>::grid_key(one, two, 2, three);
    out = out && test(7, key, "Failed vec4 grid key 7");

    // Test grid_index
    min::vec4<double> cell_extent(1.0, 1.0, 1.0, 1.0);
    origin = min::vec4<double>(0.5, 0.5, 0.5, 1.0);
    direction = min::vec4<double>(0.0, 1.0, 0.0, 1.0);
    inverse = direction.inverse();
    vmin = min::vec4<double>(-100.0, -100.0, -100.0, 1.0);
    auto index = min::vec4<double>::grid_index(vmin, cell_extent, origin);

    out = out && test(100, index.x(), "Failed vec4 grid_index 1");
    out = out && test(100, index.y(), "Failed vec4 grid_index 1");
    out = out && test(100, index.z(), "Failed vec4 grid_index 1");

    // Test grid_index from key
    key = min::vec4<double>::grid_key(vmin, cell_extent, 200, origin);
    index = min::vec4<double>::grid_index(key, 200);
    out = out && test(100, index.x(), "Failed vec4 grid_index 2");
    out = out && test(100, index.y(), "Failed vec4 grid_index 2");
    out = out && test(100, index.z(), "Failed vec4 grid_index 2");

    // Test grid_ray
    auto t = min::vec4<double>::grid_ray(vmin, cell_extent, origin, direction, inverse);
    out = out && test(std::numeric_limits<double>::max(), std::get<0>(t), 1E-4, "Failed vec4 grid_ray");
    out = out && test(std::numeric_limits<double>::max(), std::get<1>(t), 1E-4, "Failed vec4 grid_ray");
    out = out && test(0.5, std::get<2>(t), 1E-4, "Failed vec4 grid_ray");
    out = out && test(1.0, std::get<3>(t), 1E-4, "Failed vec4 grid_ray");
    out = out && test(std::numeric_limits<double>::max(), std::get<4>(t), 1E-4, "Failed vec4 grid_ray");
    out = out && test(std::numeric_limits<double>::max(), std::get<5>(t), 1E-4, "Failed vec4 grid_ray");
    out = out && test(1, std::get<6>(t), "Failed vec4 grid_ray");
    out = out && test(1, std::get<7>(t), "Failed vec4 grid_ray");
    out = out && test(1, std::get<8>(t), "Failed vec4 grid_ray");

    // Test grid_ray_next
    bool flag = false;
    key = min::vec4<double>::grid_ray_next(index, t, flag, 200);
    out = out && test(4020300, key, "Failed vec4 grid_ray_next");

    // Test grid_ray on diagonal
    origin = min::vec4<double>(0.0, 0.0, 0.0, 1.0);
    direction = min::vec4<double>(1.0, 1.0, 1.0, 1.0);
    inverse = direction.inverse();
    index = min::vec4<double>::grid_index(vmin, cell_extent, origin);
    t = min::vec4<double>::grid_ray(vmin, cell_extent, origin, direction, inverse);
    out = out && test(1.0, std::get<0>(t), 1E-4, "Failed vec4 grid_ray diagonal");
    out = out && test(1.0, std::get<1>(t), 1E-4, "Failed vec4 grid_ray diagonal");
    out = out && test(1.0, std::get<2>(t), 1E-4, "Failed vec4 grid_ray diagonal");
    out = out && test(1.0, std::get<3>(t), 1E-4, "Failed vec4 grid_ray diagonal");
    out = out && test(1.0, std::get<4>(t), 1E-4, "Failed vec4 grid_ray diagonal");
    out = out && test(1.0, std::get<5>(t), 1E-4, "Failed vec4 grid_ray diagonal");
    out = out && test(1, std::get<6>(t), "Failed vec4 grid_ray diagonal");
    out = out && test(1, std::get<7>(t), "Failed vec4 grid_ray diagonal");
    out = out && test(1, std::get<8>(t), "Failed vec4 grid_ray diagonal");

    // Starting point x = 100, y = 100, z = 100
    flag = false;
    key = min::vec4<double>::grid_key(vmin, cell_extent, 200.0, origin);
    out = out && test(4020100, key, "Failed vec4 grid_ray_next diagonal 1");

    // point x = 101, y = 100, z = 100
    key = min::vec4<double>::grid_ray_next(index, t, flag, 200.0);
    out = out && test(4060100, key, "Failed vec4 grid_ray_next diagonal 2");

    // point x = 101, y = 101, z = 100
    key = min::vec4<double>::grid_ray_next(index, t, flag, 200.0);
    out = out && test(4060300, key, "Failed vec4 grid_ray_next diagonal 3");

    // point x = 101, y = 101, z = 101
    key = min::vec4<double>::grid_ray_next(index, t, flag, 200.0);
    out = out && test(4060301, key, "Failed vec4 grid_ray_next diagonal 4");

    // Test grid overlap
    one = min::vec4<double>(-10000.0, -10000.0, -10000.0, 1.0);
    two = min::vec4<double>(10000.0, 10000.0, 10000.0, 1.0);
    vmin = min::vec4<double>(-100.0, -100.0, -100.0, 1.0);
    vmax = min::vec4<double>(100.0, 100.0, 100.0, 1.0);
    min::stack_vector<size_t, min::vec4<double>::over_size()> over;
    over = min::vec4<double>::grid_overlap(one, (two - one) / 40, 40, vmin, vmax);
    out = out && test(8, over.size(), "Failed vec4 grid_overlap");

    // Test grid sat penetration aligned
    one = min::vec4<double>(10.0, 10.0, 10.0, 1.0);
    two = min::vec4<double>(8.0, 8.0, 8.0, 1.0);
    vmin = min::vec4<double>(3.0, 3.0, 3.0, 1.0);
    vmax = min::vec4<double>(3.0, 3.0, 3.0, 1.0);

    // center, half_extent, center, half_extent
    std::pair<min::vec4<double>, double> p = min::vec4<double>::project_sat_aligned_penetration(one, vmin, two, vmax, 1E-6);
    out = out && test(1.0, p.first.x(), 1E-4, "Failed vec4 sat penetration aligned");
    out = out && test(0.0, p.first.y(), 1E-4, "Failed vec4 sat penetration aligned");
    out = out && test(0.0, p.first.z(), 1E-4, "Failed vec4 sat penetration aligned");
    out = out && test(4.0, p.second, 1E-4, "Failed vec4 sat penetration aligned");

    // Test grid sat penetration
    one = min::vec4<double>(10.0, 10.1, 10.2, 1.0);
    two = min::vec4<double>(8.0, 8.0, 8.0, 1.0);
    vmin = min::vec4<double>(3.0, 3.0, 3.0, 1.0);
    vmax = min::vec4<double>(3.0, 3.0, 3.0, 1.0);

    // coord_sys, center, half_extent, coord_sys, center, half_extent
    p = min::vec4<double>::project_sat_penetration(min::vec4<double>::axes(), one, vmin, min::vec4<double>::axes(), two, vmax, 1E-6);
    out = out && test(0.0, p.first.x(), 1E-4, "Failed vec4 sat penetration");
    out = out && test(0.0, p.first.y(), 1E-4, "Failed vec4 sat penetration");
    out = out && test(1.0, p.first.z(), 1E-4, "Failed vec4 sat penetration");
    out = out && test(3.8, p.second, 1E-4, "Failed vec4 sat penetration");

    return out;
}

#endif
