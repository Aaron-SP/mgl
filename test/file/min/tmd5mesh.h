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
#ifndef _MGL_TESTMD5MESH_MGL_
#define _MGL_TESTMD5MESH_MGL_

#include <min/md5_mesh.h>
#include <min/test.h>
#include <stdexcept>

bool test_md5_mesh()
{
    bool out = true;

    // Print size and alignment of class
    std::cout << "md5_joint_size: " << sizeof(min::md5_joint<float>) << std::endl;
    std::cout << "md5_joint_align: " << alignof(min::md5_joint<float>) << std::endl;
    std::cout << "md5_weight_size: " << sizeof(min::weight<float>) << std::endl;
    std::cout << "md5_weight_align: " << alignof(min::weight<float>) << std::endl;
    std::cout << "md5_vertex_weight_size: " << sizeof(min::vertex_weight<float>) << std::endl;
    std::cout << "md5_vertex_weight_align: " << alignof(min::vertex_weight<float>) << std::endl;
    std::cout << "md5_mesh_size: " << sizeof(min::md5_mesh<float, uint16_t>) << std::endl;
    std::cout << "md5_mesh_align: " << alignof(min::md5_mesh<float, uint16_t>) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "tmd5_joint.h: Testing alignment" << std::endl;
    out = out && test(sizeof(float) * 8, sizeof(min::md5_joint<float>), "Failed md5_joint sizeof");
    out = out && test(sizeof(float), alignof(min::md5_joint<float>), "Failed md5_joint alignof");
    out = out && test(sizeof(float) * 5, sizeof(min::weight<float>), "Failed md5_weight sizeof");
    out = out && test(sizeof(float), alignof(min::weight<float>), "Failed md5_weight alignof");
    out = out && test(sizeof(unsigned) * 2, sizeof(min::vertex_weight<float>), "Failed md5_vertex_weight sizeof");
    out = out && test(sizeof(unsigned), alignof(min::vertex_weight<float>), "Failed md5_vertex_weight alignof");
    out = out && test(sizeof(void *) * 12, sizeof(min::md5_mesh<float, uint16_t>), "Failed md5_mesh sizeof");
    out = out && test(sizeof(void *), alignof(min::md5_mesh<float, uint16_t>), "Failed md5_mesh alignof");
#endif

    // Low polygon box character mesh
    const min::md5_mesh<float, uint16_t> bob_md5 = min::md5_mesh<float, uint16_t>("data/models/bob.md5mesh");

    // Test the mesh count
    out = out && compare(1, bob_md5.get_meshes().size());
    out = out && compare(376, bob_md5.get_meshes()[0].vertex.size());
    out = out && compare(376, bob_md5.get_meshes()[0].uv.size());
    out = out && compare(570, bob_md5.get_meshes()[0].index.size());
    out = out && compare(376, bob_md5.get_meshes()[0].bone_index.size());
    out = out && compare(376, bob_md5.get_meshes()[0].bone_weight.size());
    if (!out)
    {
        throw std::runtime_error("Failed bob_md5 load mesh");
    }

    // Check bones and joint sizes
    out = out && compare(15, bob_md5.get_joints().size());
    if (!out)
    {
        throw std::runtime_error("Failed bob_md5 joint/bone sizes");
    }

    // Higher polygon mech warrior mesh
    const min::md5_mesh<float, uint16_t> mech_md5 = min::md5_mesh<float, uint16_t>("data/models/mech_warrior.md5mesh");

    // Test the mesh count
    out = out && compare(1, mech_md5.get_meshes().size());
    out = out && compare(1516, mech_md5.get_meshes()[0].vertex.size());
    out = out && compare(1516, mech_md5.get_meshes()[0].uv.size());
    out = out && compare(5856, mech_md5.get_meshes()[0].index.size());
    out = out && compare(1516, mech_md5.get_meshes()[0].bone_index.size());
    out = out && compare(1516, mech_md5.get_meshes()[0].bone_weight.size());
    if (!out)
    {
        throw std::runtime_error("Failed mech_md5 load mesh");
    }

    // Check bones and joint sizes
    out = out && compare(15, mech_md5.get_joints().size());
    if (!out)
    {
        throw std::runtime_error("Failed mech_md5 joint/bone sizes");
    }

    return out;
}

#endif
