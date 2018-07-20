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
#ifndef _MGL_BENCHWAVEFRONT_MGL_
#define _MGL_BENCHWAVEFRONT_MGL_

#include <chrono>
#include <iostream>
#include <min/wavefront.h>

double bench_wavefront()
{
    // Running wavefront test
    std::cout << std::endl
              << "wavefront: Opening a very large model: blender_suzanne.obj" << std::endl;

    // Start the time clock
    const auto start = std::chrono::high_resolution_clock::now();

    // load blender-suzanne model
    min::wavefront<float, uint32_t> wave("data/models/blender_suzanne.obj");
    min::mesh<float, uint32_t> &suzanne = wave.get_meshes()[0];
    suzanne.scale_uv(10.0);
    suzanne.calculate_tangents();

    // Calculate the difference between start and end
    const auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Print the execution time
    const double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "wavefront: OBJ mesh loaded in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}
#endif
