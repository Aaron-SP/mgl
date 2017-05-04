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
#ifndef __TESTMD5MODEL__
#define __TESTMD5MODEL__

#include <min/test.h>
#include <min/md5_mesh.h>
#include <min/md5_model.h>
#include <stdexcept>

bool test_md5_model()
{
    bool out = true;

    // Local variables
    min::md5_mesh<float, uint16_t> md5 = min::md5_mesh<float, uint16_t>("data/models/bob.md5mesh");
    min::md5_model<float, uint16_t, min::vec4, min::aabbox> model(std::move(md5));

    // Check for mesh steal
    out = out && compare(0, md5.get_meshes().size());
    if (!out)
    {
        throw std::runtime_error("Failed md5 model steal mesh");
    }

    // Test load animation
    model.load_animation("data/models/bob.md5anim");

    // Test step
    model.step(0.5);

    // Test the bone count
    out = out && compare(15, model.get_bones().size());
    if (!out)
    {
        throw std::runtime_error("Failed md5 model bone count");
    }

    return out;
}

#endif