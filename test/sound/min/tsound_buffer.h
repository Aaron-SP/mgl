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
#ifndef _MGL_TESTSOUND_MGL_
#define _MGL_TESTSOUND_MGL_

#include <iostream>
#include <min/ogg.h>
#include <min/sound_buffer.h>
#include <min/test.h>
#include <min/wave.h>
#include <stdexcept>

bool test_sound_buffer()
{
    bool out = true;
    {
        // Load a WAV file
        {
            // Alert what file we are playing
            std::cout << "Playing 'invention_no_1.wav' WAVE file" << std::endl;

            // Load a sound buffer
            min::sound_buffer player;

            // Enumerate available devices
            player.enumerate_devices();

            // Load a WAVE file
            min::wave wav = min::wave("data/sound/invention_no_1.wav");
            size_t w = player.add_wave_pcm(wav);

            // Clear the sound data
            wav.clear();

            // Create a source
            size_t s = player.add_source();

            // Bind source to wave data
            player.bind(w, s);

            // Play the sound file synchronously
            player.play_sync(s);

            // See if we got any errors
            out = out && !min::check_al_error();
            if (!out)
            {
                throw std::runtime_error("Failed sound buffer test");
            }
        }

        // Load a OGG file
        {
            // Alert what file we are playing
            std::cout << "Playing 'invention_no_1.ogg' OGG file" << std::endl;

            // Load a sound buffer
            min::sound_buffer player;

            // Enumerate available devices
            player.enumerate_devices();

            // Load a WAVE file
            min::ogg ogg = min::ogg("data/sound/invention_no_1.ogg");
            size_t o = player.add_ogg_pcm(ogg);

            // Clear the sound data
            ogg.clear();

            // Create a source
            size_t s = player.add_source();

            // Bind source to wave data
            player.bind(o, s);

            // Play the sound file synchronously
            player.play_sync(s);

            // See if we got any errors
            out = out && !min::check_al_error();
            if (!out)
            {
                throw std::runtime_error("Failed sound buffer test");
            }
        }
    }

    return out;
}

#endif
