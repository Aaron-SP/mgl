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
#ifndef __BENCH_BINARY_MESH__
#define __BENCH_BINARY_MESH__

#include <chrono>
#include <min/mesh.h>

double bench_bmesh()
{
    // Running bmesh test
    std::cout << std::endl
              << "bmesh: Opening a very large binary model: blender_suzanne.bmesh" << std::endl;

    // Start the time clock
    const auto start = std::chrono::high_resolution_clock::now();

    // Since we are using a BMESH, assert floating point compatibility
    static_assert(std::numeric_limits<float>::is_iec559, "IEEE 754 float required");
    static_assert(sizeof(float) == 4, "32 bit IEEE 754 float required");

    // load blender-suzanne model
    min::mesh<float, uint32_t> suzanne("suzanne");
    suzanne.from_file("data/models/blender_suzanne.bmesh");
    suzanne.scale_uv(10.0);
    suzanne.calculate_tangents();

    // Calculate the difference between start and end
    const auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Print the execution time
    const double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "bmesh: OBJ mesh loaded in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}
#endif
