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
#ifndef __BENCHMD5__
#define __BENCHMD5__

#include <chrono>
#include <min/md5_anim.h>
#include <min/md5_mesh.h>
#include <min/md5_model.h>

double bench_md5()
{
    // Running wavefront test
    std::cout << std::endl
              << "md5: Opening a very large model: mech_warrior.md5mesh" << std::endl;

    // Start the time clock
    const auto start = std::chrono::high_resolution_clock::now();

    // Load md5 model
    min::md5_model<float, uint32_t, min::vec4, min::aabbox> md5_model(std::move(min::md5_mesh<float, uint32_t>("data/models/mech_warrior.md5mesh")));

    // Load animation
    md5_model.load_animation("data/models/mech_warrior_stand.md5anim");

    // Calculate tangents
    min::mesh<float, uint32_t> &md5 = md5_model.get_meshes()[0];
    md5.scale_uv(10.0);
    md5.calculate_normals();
    md5.calculate_tangents();

    // Calculate the difference between start and end
    const auto dtime = std::chrono::high_resolution_clock::now() - start;

    // Print the execution time
    const double out = std::chrono::duration<double, std::milli>(dtime).count();
    std::cout << "md5: MD5 mesh loaded in: " << out << " ms" << std::endl;

    // Calculate cost of calculation (milliseconds)
    return out;
}
#endif
