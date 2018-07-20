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
#ifndef _MGL_TESTMD5ANIM_MGL_
#define _MGL_TESTMD5ANIM_MGL_

#include <min/md5_anim.h>
#include <min/test.h>
#include <stdexcept>

bool test_md5_anim()
{
    bool out = true;

    std::cout << "md5_node_size: " << sizeof(min::md5_node) << std::endl;
    std::cout << "md5_node_align: " << alignof(min::md5_node) << std::endl;
    std::cout << "md5_transform_size: " << sizeof(min::md5_transform<float>) << std::endl;
    std::cout << "md5_transform_align: " << alignof(min::md5_transform<float>) << std::endl;
    std::cout << "md5_animated_node_size: " << sizeof(min::md5_animated_node<float>) << std::endl;
    std::cout << "md5_animated_node_align: " << alignof(min::md5_animated_node<float>) << std::endl;
    std::cout << "md5_frame_data_size: " << sizeof(min::md5_frame_data<float>) << std::endl;
    std::cout << "md5_frame_data_align: " << alignof(min::md5_frame_data<float>) << std::endl;
    std::cout << "md5_frame_size: " << sizeof(min::md5_frame<float>) << std::endl;
    std::cout << "md5_frame_align: " << alignof(min::md5_frame<float>) << std::endl;
    std::cout << "md5_anim_size: " << sizeof(min::md5_anim<float>) << std::endl;
    std::cout << "md5_anim_align: " << alignof(min::md5_anim<float>) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "tmd5anim.h: Testing alignment" << std::endl;
    out = out && test(sizeof(void *) * 6, sizeof(min::md5_node), "Failed md5_node sizeof");
    out = out && test(sizeof(void *), alignof(min::md5_node), "Failed md5_node alignof");
    out = out && test(sizeof(float) * 7, sizeof(min::md5_transform<float>), "Failed md5_transform sizeof");
    out = out && test(sizeof(float), alignof(min::md5_transform<float>), "Failed md5_transform alignof");
    out = out && test(sizeof(float) * 8, sizeof(min::md5_animated_node<float>), "Failed md5_animated_node sizeof");
    out = out && test(sizeof(float), alignof(min::md5_animated_node<float>), "Failed md5_animated_node alignof");
    out = out && test(sizeof(void *) * 4, sizeof(min::md5_frame_data<float>), "Failed md5_frame_data sizeof");
    out = out && test(sizeof(void *), alignof(min::md5_frame_data<float>), "Failed md5_frame_data alignof");
    out = out && test(sizeof(void *) * 6, sizeof(min::md5_frame<float>), "Failed md5_frame sizeof");
    out = out && test(sizeof(void *), alignof(min::md5_frame<float>), "Failed md5_frame alignof");
    out = out && test(sizeof(void *) * 20, sizeof(min::md5_anim<float>), "Failed md5_anim sizeof");
    out = out && test(sizeof(void *), alignof(min::md5_anim<float>), "Failed md5_anim alignof");
#endif

    // Low polygon box character animation
    const min::md5_anim<float> anim = min::md5_anim<float>("data/models/bob.md5anim");

    // Test the hierarchy node count
    out = out && compare(15, anim.get_nodes().size());
    out = out && compare("root", anim.get_nodes()[0].get_name());
    out = out && compare("l_thigh", anim.get_nodes()[1].get_name());
    out = out && compare("l_leg", anim.get_nodes()[2].get_name());
    out = out && compare("r_thigh", anim.get_nodes()[3].get_name());
    out = out && compare("r_leg", anim.get_nodes()[4].get_name());
    out = out && compare("spine1", anim.get_nodes()[5].get_name());
    out = out && compare("spine2", anim.get_nodes()[6].get_name());
    out = out && compare("neck", anim.get_nodes()[7].get_name());
    out = out && compare("head", anim.get_nodes()[8].get_name());
    out = out && compare("r_shoulder", anim.get_nodes()[9].get_name());
    out = out && compare("r_arm", anim.get_nodes()[10].get_name());
    out = out && compare("r_forearm", anim.get_nodes()[11].get_name());
    out = out && compare("l_shoulder", anim.get_nodes()[12].get_name());
    out = out && compare("l_arm", anim.get_nodes()[13].get_name());
    out = out && compare("l_forearm", anim.get_nodes()[14].get_name());
    if (!out)
    {
        throw std::runtime_error("Failed md5 box anim node size");
    }

    // Test the base frame count
    out = out && compare(15, anim.get_transforms().size());
    out = out && compare(-0.2397, anim.get_transforms()[0].get_position().y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed md5 box anim base frame size");
    }

    // Test the frame rate
    out = out && compare(24, anim.get_frame_rate());
    if (!out)
    {
        throw std::runtime_error("Failed md5 box anim frame rate");
    }

    // Test the frame bounds count
    out = out && compare(32, anim.get_bounds().size());
    out = out && compare(-2.7041, anim.get_bounds()[0].get_min().y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed md5 box anim bounds size");
    }

    // Test the frame bounds count
    out = out && compare(32, anim.get_frame_data().size());
    out = out && compare(-0.7018, anim.get_frame_data()[0].get_data()[4], 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed md5 box anim frame data size");
    }

    // Test the frame count
    out = out && compare(32, anim.get_frames().size());
    if (!out)
    {
        throw std::runtime_error("Failed md5 box anim frame size");
    }

    // Test interpolate for 0.5 seconds
    anim.step(0.5);

    // Higher polygon mech warrior mesh
    const min::md5_anim<float> mech_anim = min::md5_anim<float>("data/models/mech_warrior_stand.md5anim");

    // Test the hierarchy node count
    out = out && compare(15, mech_anim.get_nodes().size());
    out = out && compare("root", mech_anim.get_nodes()[0].get_name());
    out = out && compare("spine", mech_anim.get_nodes()[1].get_name());
    out = out && compare("head", mech_anim.get_nodes()[2].get_name());
    out = out && compare("left_shoulder", mech_anim.get_nodes()[3].get_name());
    out = out && compare("left_upper_arm", mech_anim.get_nodes()[4].get_name());
    out = out && compare("left_lower_arm", mech_anim.get_nodes()[5].get_name());
    out = out && compare("right_shoulder", mech_anim.get_nodes()[6].get_name());
    out = out && compare("right_upper_arm", mech_anim.get_nodes()[7].get_name());
    out = out && compare("right_lower_arm", mech_anim.get_nodes()[8].get_name());
    out = out && compare("left_thigh", mech_anim.get_nodes()[9].get_name());
    out = out && compare("left_calf", mech_anim.get_nodes()[10].get_name());
    out = out && compare("left_foot", mech_anim.get_nodes()[11].get_name());
    out = out && compare("right_thigh", mech_anim.get_nodes()[12].get_name());
    out = out && compare("right_calf", mech_anim.get_nodes()[13].get_name());
    out = out && compare("right_foot", mech_anim.get_nodes()[14].get_name());
    if (!out)
    {
        throw std::runtime_error("Failed md5 mech anim node size");
    }

    // Test the base frame count
    out = out && compare(15, mech_anim.get_transforms().size());
    out = out && compare(0.0014, mech_anim.get_transforms()[0].get_position().y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed md5 mech anim base frame size");
    }

    // Test the frame rate
    out = out && compare(60, mech_anim.get_frame_rate());
    if (!out)
    {
        throw std::runtime_error("Failed md5 mech anim frame rate");
    }

    // Test the frame bounds count
    out = out && compare(60, mech_anim.get_bounds().size());
    out = out && compare(-3.6695, mech_anim.get_bounds()[0].get_min().y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed md5 mech anim bounds size");
    }

    // Test the frame bounds count
    out = out && compare(60, mech_anim.get_frame_data().size());
    out = out && compare(0.5282, mech_anim.get_frame_data()[0].get_data()[4], 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed md5 mech anim frame data size");
    }

    // Test the frame count
    out = out && compare(60, mech_anim.get_frames().size());
    if (!out)
    {
        throw std::runtime_error("Failed md5 mech anim frame size");
    }

    // Test interpolate for 0.5 seconds
    mech_anim.step(0.5);

    return out;
}

#endif
