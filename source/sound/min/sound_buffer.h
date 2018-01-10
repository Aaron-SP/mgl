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
#ifndef __SOUND_BUFFER__
#define __SOUND_BUFFER__

#include <AL/al.h>
#include <AL/alc.h>
#include <cstring>
#include <iostream>
#include <min/wave.h>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace min
{

bool check_al_error()
{
    const ALCenum error = alGetError();
    return error != AL_NO_ERROR;
}

class sound_buffer
{
  private:
    ALCdevice *_device;
    ALCcontext *_context;
    std::vector<ALuint> _buffers;
    std::vector<ALuint> _sources;
    min::vec3<float> _listener;

    inline static ALenum al_format(const min::wave &wave)
    {
        const bool stereo = wave.is_stereo();
        const uint32_t bits = wave.get_bits_per_sample();

        // Select the ALenum for this data
        switch (bits)
        {
        case 16:
            if (stereo)
            {
                return AL_FORMAT_STEREO16;
            }
            return AL_FORMAT_MONO16;
        case 8:
            if (stereo)
            {
                return AL_FORMAT_STEREO8;
            }
            return AL_FORMAT_MONO8;
        default:
            throw std::runtime_error("openal: Unsupported wave data found");
        }
    }
    inline void check_internal_error() const
    {
        const ALCenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            throw std::runtime_error("openal: Error: " + std::to_string(error));
        }
    }
    inline void clear_error() const
    {
        ALCenum error = alGetError();
        while (error != AL_NO_ERROR)
        {
            error = alGetError();
        }
    }
    inline void create_openal_context()
    {
        // Get the default device specifier
        const ALCchar *default_device = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
        std::cout << "Creating sound buffer device on: " << default_device << std::endl;

        // Clear the error stack
        clear_error();

        // Create the default device
        _device = alcOpenDevice(default_device);
        if (_device == nullptr)
        {
            throw std::runtime_error("openal: Could get default alc device");
        }

        // Create the OpenAL context on the default device
        _context = alcCreateContext(_device, nullptr);
        if (_context == nullptr)
        {
            throw std::runtime_error("openal: Could get default alc context");
        }

        // Make the context current
        const ALCboolean current = alcMakeContextCurrent(_context);
        if (!current)
        {
            throw std::runtime_error("openal: Could not make context current");
        }

        // Check for any errors
        check_internal_error();
    }

    inline void shutdown()
    {
        // Delete sources
        for (auto s : _sources)
        {
            alDeleteSources(1, &s);
        }

        // Delete buffers
        for (auto b : _buffers)
        {
            alDeleteBuffers(1, &b);
        }

        // Release the current context
        const ALCboolean current = alcMakeContextCurrent(nullptr);
        if (!current)
        {
            throw std::runtime_error("openal: Could not release current context");
        }

        // Destory the context
        alcDestroyContext(_context);

        // Close the device
        const bool closed = alcCloseDevice(_device);
        if (!closed)
        {
            throw std::runtime_error("openal: Could not close device");
        }

        // Check for any errors
        check_internal_error();
    }

  public:
    sound_buffer()
    {
        static_assert(std::is_same<float, ALfloat>::value,
                      "ALfloat must be implemented as float");

        // Create the OpenAL context
        create_openal_context();
    }
    ~sound_buffer()
    {
        shutdown();
    }
    inline void enumerate_devices() const
    {
        // Enumerate available devices
        const ALboolean enum_ext = alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT");
        if (enum_ext)
        {
            // List all available audio devices
            const ALCchar *list = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);

            // Alert user what we are printing
            std::cout << "Listing available play devices: " << std::endl;

            // Devices are separated by null character, two null's terminates the list
            if (list != nullptr)
            {
                // Print out the list
                while (list[0] != '\0' && list[1] != '\0')
                {
                    // Print device list
                    std::cout << "   - " << list << std::endl;

                    // Find the end of this string
                    const size_t length = std::strlen(list);

                    // Increment to next string
                    list += (length + 1);
                }
            }
        }
    }
    inline size_t add_source()
    {
        // Create a new source
        _sources.emplace_back();

        // Get the source ID and create
        ALuint &source = _sources.back();
        alGenSources(1, &source);

        // Return the index for this source
        return _sources.size() - 1;
    }
    inline size_t add_wave_pcm(const min::wave &wave)
    {
        // Allocate space for a buffer
        _buffers.emplace_back();

        // Generate a buffer for this sound
        ALuint &buffer = _buffers.back();
        alGenBuffers(1, &buffer);

        // Get the al_format for this data
        const ALenum format = al_format(wave);

        // Copy wave data into buffer
        const ALvoid *const data = reinterpret_cast<const ALvoid *const>(wave.data().data());
        const ALsizei size = wave.data().size();
        const ALsizei freq = wave.get_sample_rate();
        alBufferData(buffer, format, data, size, freq);

        // Return the index for this data
        return _buffers.size() - 1;
    }
    inline void bind(const size_t buffer, const size_t source) const
    {
        // Bind source to buffer
        alSourcei(_sources[source], AL_BUFFER, _buffers[buffer]);
    }
    inline bool is_playing(const size_t source) const
    {
        ALint state;
        alGetSourcei(_sources[source], AL_SOURCE_STATE, &state);

        return state == AL_PLAYING;
    }
    inline void play_async(const size_t source) const
    {
        // This call is asynch!!
        const ALuint &s = _sources[source];
        alSourcePlay(s);
    }
    inline void stop_async(const size_t source) const
    {
        // This call is asynch!!
        const ALuint &s = _sources[source];
        alSourceStop(s);
    }
    inline void play_sync(const size_t source) const
    {
        // This call is asynch so we need to poll
        const ALuint &s = _sources[source];
        alSourcePlay(s);

        // Get the source state
        while (is_playing(source))
        {
            // Wait for 1 second
            std::this_thread::sleep_for(std::chrono::duration<double>(1.0));
        }
    }
    inline void set_listener_position(const min::vec3<float> &p)
    {
        // Cache the listener position
        _listener = p;

        // Update the listener position
        const ALfloat *const data = reinterpret_cast<const ALfloat *const>(&p);
        alListenerfv(AL_POSITION, data);
    }
    inline void set_listener_velocity(const min::vec3<float> &v) const
    {
        const ALfloat *const data = reinterpret_cast<const ALfloat *const>(&v);
        alListenerfv(AL_VELOCITY, data);
    }
    inline void set_listener_orientation(const min::vec3<float> &at, const min::vec3<float> &up) const
    {
        ALfloat orien[6] = {at.x(), at.y(), at.z(), up.x(), up.y(), up.z()};
        alListenerfv(AL_ORIENTATION, orien);
    }
    inline void set_source_gain(const size_t source, const float gain) const
    {
        alSourcef(_sources[source], AL_GAIN, gain);
    }
    inline void set_source_loop(const size_t source, const bool flag) const
    {
        alSourcei(_sources[source], AL_LOOPING, flag);
    }
    inline void set_source_position(const size_t source, const min::vec3<float> &p) const
    {
        const ALfloat *const data = reinterpret_cast<const ALfloat *const>(&p);
        alSourcefv(_sources[source], AL_POSITION, data);
    }
    inline void set_source_at_listener(const size_t source) const
    {
        const ALfloat *const data = reinterpret_cast<const ALfloat *const>(&_listener);
        alSourcefv(_sources[source], AL_POSITION, data);
    }
    inline void set_source_velocity(const size_t source, const min::vec3<float> &v) const
    {
        const ALfloat *const data = reinterpret_cast<const ALfloat *const>(&v);
        alSourcefv(_sources[source], AL_VELOCITY, data);
    }
};
}

#endif