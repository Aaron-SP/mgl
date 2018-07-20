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
#ifndef _MGL_TEST_PHYSICS_NT_MGL_
#define _MGL_TEST_PHYSICS_NT_MGL_

// DO NOT COPY THIS! THIS IS A HACK!
namespace test_nt
{
#undef _MGL_MATRIX2_MGL_
#undef _MGL_QUATERNION_MGL_
#undef _MGL_VECTOR2_MGL_
#undef _MGL_VECTOR3_MGL_
#undef _MGL_VECTOR4_MGL_
#undef _MGL_GRID_MGL_
#undef _MGL_BIT_FLAG_MGL_
#undef _MGL_RAY_MGL_
#undef _MGL_AABBOX_MGL_
#undef _MGL_MGL_STATIC_VECTOR_MGL_
#undef _MGL_MGL_STACK_VECTOR_MGL_
#undef _MGL_UTILITY_MGL_
#undef _MGL_UINT_SORT_MGL_
#undef _MGL_MGL_BI_MGL_
#undef _MGL_MGL_TRI_MGL_
#undef _MGL_INTERSECT_MGL_
#undef _MGL_TEMPLATE_MATH_MGL_
using namespace min;
#include <min/aabbox.h>
#include <min/grid.h>
#include <min/quat.h>
#include <min/vec2.h>
#include <min/vec3.h>
#include <min/vec4.h>

#include <min/physics_nt.h>
}
#include <min/test.h>
#include <stdexcept>

bool test_physics_nt_aabb_grid()
{
    bool out = true;

    // vec2 grid simulation
    {
        // Print size and alignment of class
        std::cout << "body_nt_vec2_size: " << sizeof(test_nt::min::body<float, test_nt::min::vec2>) << std::endl;
        std::cout << "body_nt_vec2_align: " << alignof(test_nt::min::body<float, test_nt::min::vec2>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "tphysics_nt.h: Testing alignment" << std::endl;
        out = out && test(sizeof(void *) * 13, sizeof(test_nt::min::body<float, test_nt::min::vec2>), "Failed body_nt vec2 sizeof");
        out = out && test(sizeof(void *), alignof(test_nt::min::body<float, test_nt::min::vec2>), "Failed body_nt vec2 alignof");
#endif

        // Local variables
        const test_nt::min::vec2<double> minW(-10.0, -10.0);
        const test_nt::min::vec2<double> maxW(10.0, 10.0);
        const test_nt::min::aabbox<double, test_nt::min::vec2> world(minW, maxW);
        const test_nt::min::vec2<double> gravity(0.0, -10.0);
        test_nt::min::physics<double, uint_fast16_t, uint_fast32_t, test_nt::min::vec2, test_nt::min::aabbox, test_nt::min::aabbox, test_nt::min::grid> simulation(world, gravity);

        // Add rigid bodies to the simulation
        const test_nt::min::aabbox<double, test_nt::min::vec2> box1(test_nt::min::vec2<double>(1.0, 1.0), test_nt::min::vec2<double>(2.0, 2.0));
        const test_nt::min::aabbox<double, test_nt::min::vec2> box2(test_nt::min::vec2<double>(1.0, 3.0), test_nt::min::vec2<double>(2.0, 4.0));
        const size_t body1_id = simulation.add_body(box1, 100.0);
        const size_t body2_id = simulation.add_body(box2, 100.0);

        // Body1 should counter gravity and body2 should fall on body1
        const test_nt::min::vec2<double> up_force(0.0, 1000.0);
        test_nt::min::body<double, test_nt::min::vec2> &body1 = simulation.get_body(body1_id);
        test_nt::min::body<double, test_nt::min::vec2> &body2 = simulation.get_body(body2_id);
        body1.add_force(up_force);

        // Solve the simulation
        simulation.solve(0.1, 0.01);

        // Test body1 position didn't move
        const test_nt::min::vec2<double> &p1 = body1.get_position();
        out = out && compare(1.5, p1.x(), 1E-4);
        out = out && compare(1.5, p1.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed physics_nt vec2 position body 1");
        }

        // Test body2 position falls from 3.5 to 3.4; df = at^2; -10*(0.1 * 0.1) = -0.1
        const test_nt::min::vec2<double> &p2 = body2.get_position();
        out = out && compare(1.5, p2.x(), 1E-4);
        out = out && compare(3.4, p2.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed physics_nt vec2 position body2");
        }

        // Solve the simulation for intersection at t = 0.3162s; 0.41s
        body1.add_force(up_force);
        simulation.solve(0.1, 0.01);
        body1.add_force(up_force);
        simulation.solve(0.1, 0.01);
        body1.add_force(up_force);
        simulation.solve(0.11, 0.01);

        const test_nt::min::vec2<double> &v1 = body1.get_linear_velocity();
        const test_nt::min::vec2<double> &v2 = body2.get_linear_velocity();

        // Test velocity before collision
        out = out && compare(0.0, v1.x(), 1E-4);
        out = out && compare(0.0, v1.y(), 1E-4);
        out = out && compare(0.0, v2.x(), 1E-4);
        out = out && compare(-4.100, v2.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed physics_nt vec2 velocity before collision");
        }

        // The two boxes are touching after this time, so we don't need a force to prop up body1
        simulation.solve(0.001, 0.01);
        out = out && compare(0.0, v1.x(), 1E-4);
        out = out && compare(-4.1100, v1.y(), 1E-4);
        out = out && compare(0.0, v2.x(), 1E-4);
        out = out && compare(-0.0100, v2.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed physics_nt vec2 velocity collision1");
        }

        // Advance the simulation to test contact resolution
        simulation.solve(0.001, 0.01);

        out = out && compare(0.0, v1.x(), 1E-4);
        out = out && compare(-4.1200, v1.y(), 1E-4);
        out = out && compare(0.0, v2.x(), 1E-4);
        out = out && compare(-0.0200, v2.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed physics_nt vec2 velocity collision2");
        }
    }

    // vec3 grid simulation
    {
        // Print size and alignment of class
        std::cout << "body_nt_vec3_size: " << sizeof(test_nt::min::body<float, test_nt::min::vec3>) << std::endl;
        std::cout << "body_nt_vec3_align: " << alignof(test_nt::min::body<float, test_nt::min::vec3>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "tphysics_nt.h: Testing alignment" << std::endl;
        out = out && test(sizeof(void *) * 16, sizeof(test_nt::min::body<float, test_nt::min::vec3>), "Failed body_nt vec3 sizeof");
        out = out && test(sizeof(void *), alignof(test_nt::min::body<float, test_nt::min::vec3>), "Failed body_nt vec3 alignof");
#endif

        // Local variables
        const test_nt::min::vec3<double> minW(-10.0, -10.0, -10.0);
        const test_nt::min::vec3<double> maxW(10.0, 10.0, 10.0);
        const test_nt::min::aabbox<double, test_nt::min::vec3> world(minW, maxW);
        const test_nt::min::vec3<double> gravity(0.0, -10.0, 0.0);
        test_nt::min::physics<double, uint_fast16_t, uint_fast32_t, test_nt::min::vec3, test_nt::min::aabbox, test_nt::min::aabbox, test_nt::min::grid> simulation(world, gravity);

        // Add rigid bodies to the simulation
        const test_nt::min::aabbox<double, test_nt::min::vec3> box1(test_nt::min::vec3<double>(1.0, 1.0, 1.0), test_nt::min::vec3<double>(2.0, 2.0, 2.0));
        const test_nt::min::aabbox<double, test_nt::min::vec3> box2(test_nt::min::vec3<double>(1.0, 3.0, 1.0), test_nt::min::vec3<double>(2.0, 4.0, 2.0));
        const size_t body1_id = simulation.add_body(box1, 100.0);
        const size_t body2_id = simulation.add_body(box2, 100.0);

        // Body1 should counter gravity and body2 should fall on body1
        const test_nt::min::vec3<double> up_force(0.0, 1000.0, 0.0);
        test_nt::min::body<double, test_nt::min::vec3> &body1 = simulation.get_body(body1_id);
        test_nt::min::body<double, test_nt::min::vec3> &body2 = simulation.get_body(body2_id);
        body1.add_force(up_force);

        // Solve the simulation
        simulation.solve(0.1, 0.01);

        // Test body1 position didn't move
        const test_nt::min::vec3<double> &p1 = body1.get_position();
        out = out && compare(1.5, p1.x(), 1E-4);
        out = out && compare(1.5, p1.y(), 1E-4);
        out = out && compare(1.5, p1.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed physics_nt vec3 position body 1");
        }

        // Test body2 position falls from 3.5 to 3.4; df = at^2/t; -10*(0.1 * 0.1) = -0.1
        const test_nt::min::vec3<double> &p2 = body2.get_position();
        out = out && compare(1.5, p2.x(), 1E-4);
        out = out && compare(3.4, p2.y(), 1E-4);
        out = out && compare(1.5, p2.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed physics_nt vec3 position body2");
        }

        // Solve the simulation for intersection at t = 0.3162s; 0.41s
        body1.add_force(up_force);
        simulation.solve(0.1, 0.01);
        body1.add_force(up_force);
        simulation.solve(0.1, 0.01);
        body1.add_force(up_force);
        simulation.solve(0.11, 0.01);

        const test_nt::min::vec3<double> &v1 = body1.get_linear_velocity();
        const test_nt::min::vec3<double> &v2 = body2.get_linear_velocity();

        // Test velocity before collision
        out = out && compare(0.0, v1.x(), 1E-4);
        out = out && compare(0.0, v1.y(), 1E-4);
        out = out && compare(0.0, v1.z(), 1E-4);
        out = out && compare(0.0, v2.x(), 1E-4);
        out = out && compare(-4.100, v2.y(), 1E-4);
        out = out && compare(0.0, v2.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed physics_nt vec2 velocity before collision");
        }

        // The two boxes are touching after this time, so we don't need a force to prop up body1
        simulation.solve(0.001, 0.01);
        out = out && compare(0.0, v1.x(), 1E-4);
        out = out && compare(-4.1100, v1.y(), 1E-4);
        out = out && compare(0.0, v1.z(), 1E-4);
        out = out && compare(0.0, v2.x(), 1E-4);
        out = out && compare(-0.0100, v2.y(), 1E-4);
        out = out && compare(0.0, v2.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed physics_nt vec3 velocity collision1");
        }

        // Advance the simulation to test contact resolution
        simulation.solve(0.001, 0.01);
        out = out && compare(0.0, v1.x(), 1E-4);
        out = out && compare(-4.1200, v1.y(), 1E-4);
        out = out && compare(0.0, v1.z(), 1E-4);
        out = out && compare(0.0, v2.x(), 1E-4);
        out = out && compare(-0.0200, v2.y(), 1E-4);
        out = out && compare(0.0, v2.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed physics_nt vec3 velocity collision2");
        }
    }

    // vec4 grid simulation
    {
        // Print size and alignment of class
        std::cout << "body_nt_vec4_size: " << sizeof(test_nt::min::body<float, test_nt::min::vec4>) << std::endl;
        std::cout << "body_nt_vec4_align: " << alignof(test_nt::min::body<float, test_nt::min::vec4>) << std::endl;

#ifdef MGL_TEST_ALIGN
        std::cout << "tphysics_nt.h: Testing alignment" << std::endl;
        out = out && test(sizeof(void *) * 18, sizeof(test_nt::min::body<float, test_nt::min::vec4>), "Failed body_nt vec4 sizeof");
        out = out && test(sizeof(void *), alignof(test_nt::min::body<float, test_nt::min::vec4>), "Failed body_nt vec4 alignof");
#endif

        // Local variables
        const test_nt::min::vec4<double> minW(-10.0, -10.0, -10.0, 1.0);
        const test_nt::min::vec4<double> maxW(10.0, 10.0, 10.0, 1.0);
        const test_nt::min::aabbox<double, test_nt::min::vec4> world(minW, maxW);
        const test_nt::min::vec4<double> gravity(0.0, -10.0, 0.0, 1.0);
        test_nt::min::physics<double, uint_fast16_t, uint_fast32_t, test_nt::min::vec4, test_nt::min::aabbox, test_nt::min::aabbox, test_nt::min::grid> simulation(world, gravity);

        // Add rigid bodies to the simulation
        const test_nt::min::aabbox<double, test_nt::min::vec4> box1(test_nt::min::vec4<double>(1.0, 1.0, 1.0, 1.0), test_nt::min::vec4<double>(2.0, 2.0, 2.0, 1.0));
        const test_nt::min::aabbox<double, test_nt::min::vec4> box2(test_nt::min::vec4<double>(1.0, 3.0, 1.0, 1.0), test_nt::min::vec4<double>(2.0, 4.0, 2.0, 1.0));
        const size_t body1_id = simulation.add_body(box1, 100.0);
        const size_t body2_id = simulation.add_body(box2, 100.0);

        // Body1 should counter gravity and body2 should fall on body1
        const test_nt::min::vec4<double> up_force(0.0, 1000.0, 0.0, 1.0);
        test_nt::min::body<double, test_nt::min::vec4> &body1 = simulation.get_body(body1_id);
        test_nt::min::body<double, test_nt::min::vec4> &body2 = simulation.get_body(body2_id);
        body1.add_force(up_force);

        // Solve the simulation
        simulation.solve(0.1, 0.01);

        // Test body1 position didn't move
        const test_nt::min::vec4<double> &p1 = body1.get_position();
        out = out && compare(1.5, p1.x(), 1E-4);
        out = out && compare(1.5, p1.y(), 1E-4);
        out = out && compare(1.5, p1.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed physics_nt vec4 position body 1");
        }

        // Test body2 position falls from 3.5 to 3.4; df = at^2/t; -10*(0.1 * 0.1) = -0.1
        const test_nt::min::vec4<double> &p2 = body2.get_position();
        out = out && compare(1.5, p2.x(), 1E-4);
        out = out && compare(3.4, p2.y(), 1E-4);
        out = out && compare(1.5, p2.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed physics_nt vec4 position body2");
        }

        // Solve the simulation for intersection at t = 0.3162s; 0.41s
        body1.add_force(up_force);
        simulation.solve(0.1, 0.01);
        body1.add_force(up_force);
        simulation.solve(0.1, 0.01);
        body1.add_force(up_force);
        simulation.solve(0.11, 0.01);

        const test_nt::min::vec4<double> &v1 = body1.get_linear_velocity();
        const test_nt::min::vec4<double> &v2 = body2.get_linear_velocity();

        // Test velocity before collision
        out = out && compare(0.0, v1.x(), 1E-4);
        out = out && compare(0.0, v1.y(), 1E-4);
        out = out && compare(0.0, v1.z(), 1E-4);
        out = out && compare(0.0, v2.x(), 1E-4);
        out = out && compare(-4.100, v2.y(), 1E-4);
        out = out && compare(0.0, v2.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed physics_nt vec2 velocity before collision");
        }

        // The two boxes are touching after this time, so we don't need a force to prop up body1
        simulation.solve(0.001, 0.01);
        out = out && compare(0.0, v1.x(), 1E-4);
        out = out && compare(-4.1100, v1.y(), 1E-4);
        out = out && compare(0.0, v1.z(), 1E-4);
        out = out && compare(0.0, v2.x(), 1E-4);
        out = out && compare(-0.0100, v2.y(), 1E-4);
        out = out && compare(0.0, v2.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed physics_nt vec4 velocity collision1");
        }

        // Advance the simulation to test contact resolution
        simulation.solve(0.001, 0.01);
        out = out && compare(0.0, v1.x(), 1E-4);
        out = out && compare(-4.1200, v1.y(), 1E-4);
        out = out && compare(0.0, v1.z(), 1E-4);
        out = out && compare(0.0, v2.x(), 1E-4);
        out = out && compare(-0.0200, v2.y(), 1E-4);
        out = out && compare(0.0, v2.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed physics_nt vec4 velocity collision2");
        }
    }

    return out;
}

#endif
