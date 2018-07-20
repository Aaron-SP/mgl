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
#ifndef _MGL_TESTMD5MODEL_MGL_
#define _MGL_TESTMD5MODEL_MGL_

#include <min/md5_mesh.h>
#include <min/md5_model.h>
#include <min/test.h>
#include <stdexcept>

bool test_md5_model()
{
    bool out = true;

    // Low polygon box character mesh
    min::md5_mesh<float, uint16_t> box_md5 = min::md5_mesh<float, uint16_t>("data/models/bob.md5mesh");
    min::md5_model<float, uint16_t, min::vec4, min::aabbox> box_model(std::move(box_md5));

    // Check for mesh steal
    out = out && compare(0, box_md5.get_meshes().size());
    if (!out)
    {
        throw std::runtime_error("Failed md5 box model steal mesh");
    }

    // Test load animation
    box_model.load_animation("data/models/bob.md5anim");

    // Test step
    box_model.step(0.5);

    // Test the bone count
    out = out && compare(15, box_model.get_bones().size());
    if (!out)
    {
        throw std::runtime_error("Failed md5 box model bone count");
    }

    // Higher polygon mech warrior mesh
    min::md5_mesh<float, uint16_t> mech_md5 = min::md5_mesh<float, uint16_t>("data/models/mech_warrior.md5mesh");
    min::md5_model<float, uint16_t, min::vec4, min::aabbox> mech_model(std::move(mech_md5));

    // Check for mesh steal
    out = out && compare(0, mech_md5.get_meshes().size());
    if (!out)
    {
        throw std::runtime_error("Failed md5 mech model steal mesh");
    }

    // Test load animation
    mech_model.load_animation("data/models/mech_warrior_stand.md5anim");

    // Test step
    mech_model.step(0.5);

    // Test the bone count
    out = out && compare(15, mech_model.get_bones().size());
    if (!out)
    {
        throw std::runtime_error("Failed md5 mech model bone count");
    }

    // Calculate normals and tangents for the model
    min::mesh<float, uint16_t> &mech_mesh = mech_model.get_meshes()[0];
    mech_mesh.scale_uv(10.0);
    mech_mesh.calculate_normals();
    mech_mesh.calculate_tangents();

    return out;
}

#endif
