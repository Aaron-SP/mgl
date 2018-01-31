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
#ifndef __EMITTERBUFFER__
#define __EMITTERBUFFER__

#include <cmath>
#include <functional>
#include <min/vec3.h>
#include <min/window.h>
#include <random>
#include <stdexcept>
#include <utility>

// The particle system is governed by the combination of the following equations

// Equation 1: Newton's 2nd Law of Motion
// SUM(Forces) = mass * acceleration

// Equation 2: Acceleration, velocity, distance
// vo = vi + a * dt;
// do = di + vo * dt;

namespace min
{

template <typename T, GLenum FLOAT_TYPE>
class emitter_buffer
{
  private:
    // This is the size of each particle
    static constexpr size_t particle_size = sizeof(vec3<T>);

  private:
    GLuint _vao;
    GLuint _vbo;
    size_t _emit_count;
    size_t _emit_pool_offset;
    size_t _emit_pool_size;
    T _emit_freq;
    T _emit_accum;
    T _spawn_freq;
    T _spawn_accum;
    T _inv_mass;
    T _random;
    vec3<T> _rot_axis;
    vec3<T> _grav_force;
    vec3<T> _start_pos;
    vec3<T> _start_speed;
    vec3<T> _wind_force;
    std::uniform_real_distribution<T> _dist;
    std::mt19937 _rand;
    std::vector<vec3<T>> _position;
    std::vector<vec3<T>> _speed;
    std::vector<std::pair<vec3<T>, T>> _attractors;

    inline void check_extensions() const
    {
        const bool vao = GLEW_ARB_vertex_array_object;
        const bool vbo = GLEW_ARB_vertex_buffer_object;

        // Check that we have the extensions we need
        if (!vao || !vbo)
        {
            throw std::runtime_error("emitter_buffer: minimum extensions not met");
        }
    }
    inline vec3<T> compute_force(const vec3<T> &position, const vec3<T> &speed)
    {
        vec3<T> attract;

        // For each attraction, attraction force based off distance from attractor
        for (const auto &a : _attractors)
        {
            // Compute a tangential orbit around the Y axis,
            const vec3<T> dp = position - a.first;

            // Calculate the attraction force
            attract += (dp.cross(_rot_axis) - dp) * a.second;
        }

        // Compute the force from Newton's 2nd Law
        return attract + _grav_force + _wind_force;
    }
    inline void seed(const size_t start, const size_t len)
    {
        // Reset all particles to the start position
        const size_t size = start + len;
        for (size_t i = start; i < size; i++)
        {
            // Set particle position to start position
            _position[i] = _start_pos;

            // Set speed
            _speed[i] = _start_speed + random();
        }

        const size_t attrs = _attractors.size();
        if (attrs > 0)
        {
            for (size_t i = start; i < size; i++)
            {
                const size_t group = i % attrs;
                const T rand_interp = _dist(_rand) / _random;
                _position[i] = _start_pos + (_attractors[group].first - _start_pos) * rand_interp;
            }
        }
    }

  public:
    emitter_buffer(const vec3<T> &position, const size_t emit_count, const size_t emit_periods, const T emit_freq, const T spawn_freq, const T random)

        : _emit_count(emit_count), _emit_pool_offset(0), _emit_pool_size(0),
          _emit_freq(emit_freq), _emit_accum(0.0), _spawn_freq(spawn_freq), _spawn_accum(0.0),
          _inv_mass(2.0), _random(random),
          _rot_axis(vec3<T>::up()), _grav_force(0.0, -9.8, 0.0), _start_pos(position), _start_speed(0.0, 10.0, 0.0),
          _dist(-_random, _random), _position(emit_count * emit_periods), _speed(_position.size())
    {
        // Check that all needed extensions are present
        check_extensions();

        // Generate the VAO for this vertex layout
        glGenVertexArrays(1, &_vao);

        // Bind the vao
        bind();

        // Generate the vertex buffer id
        glGenBuffers(1, &_vbo);

        // Bind the buffer to hold data
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        // Specify the vertex attributes in location = 0, tightly packed
        glVertexAttribPointer(0, 3, FLOAT_TYPE, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        // Seed the generator
        _rand.seed(453178);

        // Initialize the simulation
        seed(0, _position.size());
    }
    ~emitter_buffer()
    {
        if (_vbo > 0)
        {
            // Bind the vao
            bind();

            // Bind the buffer for disabling attribs
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);

            // Disable the vertex attributes
            glDisableVertexAttribArray(0);

            // unbind the VBO and delete it
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDeleteBuffers(1, &_vbo);
        }
        if (_vao > 0)
        {
            // Unbind the VAO and delete it
            glBindVertexArray(0);
            glDeleteVertexArrays(1, &_vao);
        }

        // Check for opengl errors
        check_internal_error();
    }
    emitter_buffer(const emitter_buffer &sb) = delete;
    inline size_t attractor_add(const vec3<T> &center, const T power)
    {
        // Create a new attractor
        _attractors.push_back(std::make_pair(center, power));

        // Return attractor index
        return _attractors.size() - 1;
    }
    inline void attractor_clear()
    {
        _attractors.clear();
    }
    inline void set_attractor(const vec3<T> &center, const T power, const size_t index)
    {
        // Update attractor properties
        _attractors[index].first = center;
        _attractors[index].second = power;
    }
    inline void bind() const
    {
        // Bind the VAO for this buffer
        // Do not call this function often as it is unneeded context switching
        glBindVertexArray(_vao);
    }
    inline void unbind() const
    {
        glBindVertexArray(0);
    }
    inline void draw() const
    {
        // Draw all objects in the static buffer
        glDrawArrays(GL_POINTS, 0, _emit_pool_size);
    }
    inline vec3<T> random()
    {
        // Compute a random speed modifier for dispersion of particles
        const T randx = _dist(_rand);
        const T randy = _dist(_rand);
        const T randz = _dist(_rand);

        // Calculate random speed
        return vec3<T>(randx, randy, randz);
    }
    inline void reset()
    {
        // Initialize the simulation
        seed(0, _position.size());

        // Reset the accumulated time
        _emit_pool_size = 0;
        _emit_pool_offset = 0;
        _emit_accum = 0.0;
        _spawn_accum = 0.0;
    }
    inline void resize(const size_t n)
    {
        _position.resize(n);
        _speed.resize(n);
    }
    inline void set_mass(const T mass)
    {
        _inv_mass = 1.0 / mass;
    }
    inline void set_gravity(const vec3<T> &grav)
    {
        _grav_force = grav;
    }
    inline void set_position(const vec3<T> &position)
    {
        _start_pos = position;
    }
    inline void set_random(const T lower, const T upper)
    {
        _dist = std::uniform_real_distribution<T>(lower, upper);
    }
    inline void set_rotation_axis(const min::vec3<T> &axis)
    {
        _rot_axis = axis;
    }
    inline void set_speed(const vec3<T> &speed)
    {
        _start_speed = speed;
    }
    inline void set_wind(const vec3<T> &wind)
    {
        _wind_force = wind;
    }
    void step(const T dt)
    {
        // Accumulate this time step
        _emit_accum += dt;

        // Check if we need to grow pool size
        if (_emit_accum >= _emit_freq)
        {
            // Decrement emit accumulation
            _emit_accum -= _emit_freq;

            // Emit more particles if the buffer is not full
            if (_emit_pool_size != _position.size())
            {
                // Emit more particles
                _emit_pool_size += _emit_count;
            }
        }

        // If pool is full
        if (_emit_pool_size == _position.size())
        {
            // Accumulate spawn timer
            _spawn_accum += dt;

            // Check if pool is full and time to respawn
            if (_spawn_accum >= _spawn_freq)
            {
                // Reseed from offset to offset + _emit_count
                seed(_emit_pool_offset, _emit_count);

                // Increment the offset
                _emit_pool_offset += _emit_count;

                // If we hit the end of the pool reset offset = 0
                if (_emit_pool_offset == _emit_pool_size)
                {
                    _emit_pool_offset = 0;
                }

                // Add more life to particles
                _spawn_accum -= _spawn_freq;
            }
        }

        // Update all particles
        for (size_t i = 0; i < _emit_pool_size; i++)
        {
            // Get particle positon and speed
            vec3<T> &position = _position[i];
            vec3<T> &speed = _speed[i];

            // Compute the force based on particle system settings
            const vec3<T> force = compute_force(position, speed);

            // Calculate the acceleration of the particle
            const vec3<T> accel = force * _inv_mass;

            // Update the particle velocity
            speed += accel * dt;

            // Update the particles position
            position += speed * dt;
        }
    }
    inline void set(const std::function<void(vec3<T> &p, vec3<T> &s, const float inv_mass)> &f)
    {
        // Expand buffer
        _emit_pool_size = _position.size();

        // Update all particles
        for (size_t i = 0; i < _emit_pool_size; i++)
        {
            // Call custom function
            f(_position[i], _speed[i], _inv_mass);
        }
    }
    inline void upload() const
    {
        // Bind the buffer to hold data
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        // Send the data to the GPU, calculate data size in bytes
        const size_t data_bytes = _emit_pool_size * particle_size;
        glBufferData(GL_ARRAY_BUFFER, data_bytes, &_position[0], GL_DYNAMIC_DRAW);
    }
};
}

#endif
