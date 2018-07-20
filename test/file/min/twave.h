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
#ifndef _MGL_TESTWAVE_MGL_
#define _MGL_TESTWAVE_MGL_

#include <min/test.h>
#include <min/wave.h>
#include <stdexcept>

bool test_wave()
{
    bool out = true;

    // Load invention wav file
    {
        const min::wave sound = min::wave("data/sound/invention_no_1.wav");

        // File should not be mono
        out = out && !sound.is_mono();
        if (!out)
        {
            throw std::runtime_error("Failed wave file not is_mono");
        }

        // File should be stereo
        out = out && sound.is_stereo();
        if (!out)
        {
            throw std::runtime_error("Failed wave file is_stereo");
        }

        // Test WAVE data bits per sample
        out = out && compare(16, sound.get_bits_per_sample());
        if (!out)
        {
            throw std::runtime_error("Failed wave bits per sample");
        }

        // Test size of the WAVE data
        out = out && compare(1360896, sound.data().size());
        if (!out)
        {
            throw std::runtime_error("Failed wave data size");
        }

        // Test WAVE data sample rate
        out = out && compare(44100, sound.get_sample_rate());
        if (!out)
        {
            throw std::runtime_error("Failed wave sample rate");
        }

        // Test WAVE data number of samples
        out = out && compare(680448, sound.get_data_samples());
        if (!out)
        {
            throw std::runtime_error("Failed wave number of data samples");
        }
    }

    return out;
}

#endif
