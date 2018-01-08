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
#ifndef __TESTSOUND__
#define __TESTSOUND__

#include <min/sound_buffer.h>
#include <min/test.h>
#include <stdexcept>

bool test_sound_buffer()
{
    bool out = true;
    {
        // Load a sound buffer
        min::sound_buffer player;

        // Enumerate available devices
        player.enumerate_devices();

        // Load a WAVE file
        const min::wave sound = min::wave("data/sound/invention_no_1.wav");
        size_t wav = player.add_wave_pcm(sound);

        // Create a source
        size_t s = player.add_source();

        // Bind source to wave data
        player.bind(wav, s);

        // Play the sound file synchronously
        player.play_sync(s);
    }
    return out;
}

#endif
