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
#ifndef __TESTMD5ANIM__
#define __TESTMD5ANIM__

#include <min/test.h>
#include <min/md5_anim.h>
#include <stdexcept>

bool test_md5_anim()
{
    bool out = true;

    // Local variables
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
        throw std::runtime_error("Failed md5 anim node size");
    }

    // Test the base frame count
    out = out && compare(15, anim.get_transforms().size());
    out = out && compare(-0.9840, anim.get_transforms()[0].get_position().y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed md5 anim base frame size");
    }

    // Test the frame rate
    out = out && compare(24, anim.get_frame_rate());
    if (!out)
    {
        throw std::runtime_error("Failed md5 anim frame rate");
    }

    // Test the frame bounds count
    out = out && compare(32, anim.get_bounds().size());
    out = out && compare(-2.6032, anim.get_bounds()[0].get_min().y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed md5 anim bounds size");
    }

    // Test the frame bounds count
    out = out && compare(32, anim.get_frame_data().size());
    out = out && compare(-0.9925, anim.get_frame_data()[0].get_data()[4], 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed md5 anim frame data size");
    }

    // Test the frame count
    out = out && compare(32, anim.get_frames().size());
    if (!out)
    {
        throw std::runtime_error("Failed md5 anim frame size");
    }

    // Test interpolate for 0.5 seconds
    anim.step(0.5);

    return out;
}

#endif