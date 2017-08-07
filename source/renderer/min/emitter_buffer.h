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
#ifndef __EMITTERBUFFER__
#define __EMITTERBUFFER__

#include <cmath>
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
    GLuint _vao; // Buffer properties
    GLuint _vbo;
    T _emit_freq;
    T _emit_accum; // Solver properties
    T _spawn_freq;
    T _spawn_accum;
    size_t _emit_count;
    size_t _emit_pool_offset;
    size_t _emit_pool_size;
    T _inv_mass; // Particle properties
    T _random;
    vec3<T> _grav_force;
    vec3<T> _start_pos;
    vec3<T> _start_speed;
    vec3<T> _wind_force;
    std::uniform_real_distribution<T> _dist; // Randomness
    std::mt19937 _rand;
    std::vector<vec3<T>> _particles; // Particle buffers
    std::vector<vec3<T>> _speed;
    std::vector<std::pair<vec3<T>, T>> _attractors; // Gravity attractors

    inline vec3<T> compute_force(const vec3<T> &position, const vec3<T> &speed)
    {
        vec3<T> attract;

        // For each attraction, attraction force based off distance from attractor
        for (const auto &a : _attractors)
        {
            // Compute a tangential orbit around the Y axis,
            const vec3<T> dp = position - a.first;

            // Calculate the attraction force
            attract += (dp.cross_y() - dp) * a.second;
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
            _particles[i] = _start_pos;
        }

        const size_t attrs = _attractors.size();
        if (attrs > 0)
        {
            for (size_t i = start; i < size; i++)
            {
                size_t group = i % attrs;
                _particles[i] = _start_pos + _attractors[group].first;
            }
        }

        // Reset all particle speeds
        for (size_t i = start; i < size; i++)
        {
            auto &speed = _speed[i];

            // Compute a random speed modifier for dispersion of particles
            T randx = _dist(_rand);
            T randy = _dist(_rand);
            T randz = _dist(_rand);

            // Calculate random speed
            vec3<T> rand_speed(randx, randy, randz);

            // Set speed
            speed = _start_speed + rand_speed;
        }
    }

  public:
    emitter_buffer(const vec3<T> &position, const size_t emit_count, const size_t emit_periods, const T emit_freq, const T spawn_freq, const T life)

        : _emit_count(emit_count), _emit_pool_offset(0), _emit_pool_size(0),
          _emit_freq(emit_freq), _emit_accum(0.0), _spawn_freq(spawn_freq), _spawn_accum(0.0),
          _inv_mass(2.0), _random(10.0),
          _grav_force(0.0, -9.8, 0.0), _start_pos(position), _start_speed(0.0, 10.0, 0.0), _dist(-_random, _random),
          _particles(emit_count * emit_periods), _speed(_particles.size())
    {
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
        seed(0, _particles.size());
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
        check_error();
    }
    emitter_buffer(const emitter_buffer &sb) = delete;
    inline void attractor_add(const vec3<T> &center, const T power)
    {
        _attractors.push_back(std::make_pair(center, power));
    }
    inline void attractor_clear()
    {
        _attractors.clear();
    }
    inline void bind() const
    {
        // Bind the VAO for this buffer
        // Do not call this function often as it is unneeded context switching
        glBindVertexArray(_vao);
    }
    inline void draw() const
    {
        // Draw all objects in the static buffer
        glDrawArrays(GL_POINTS, 0, _emit_pool_size);
    }
    inline void reset()
    {
        // Initialize the simulation
        seed(0, _particles.size());

        // Reset the accumulated time
        _emit_accum = 0.0;
        _spawn_accum = 0.0;
    }
    inline void resize(const size_t n)
    {
        _particles.resize(n);
    }
    inline void set_mass(const T mass)
    {
        _inv_mass = 1.0 / mass;
    }
    inline void set_gravity(const vec3<T> &grav)
    {
        _grav_force = grav;
    }
    inline void set_random(const T rand)
    {
        _dist = std::uniform_real_distribution<T>(-rand, rand);
    }
    inline void set_position(const vec3<T> &position)
    {
        _start_pos = position;
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
            if (_emit_pool_size != _particles.size())
            {
                // Emit more particles
                _emit_pool_size += _emit_count;
            }
        }

        // If pool is full
        if (_emit_pool_size == _particles.size())
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
            auto &position = _particles[i];
            auto &speed = _speed[i];

            // Compute the force based on particle system settings
            vec3<T> force = compute_force(position, speed);

            // Calculate the acceleration of the particle
            vec3<T> accel = force * _inv_mass;

            // Update the particle velocity
            speed += accel * dt;

            // Update the particles position
            position += speed * dt;
        }
    }
    inline void upload() const
    {
        // Bind the buffer to hold data
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        // Send the data to the GPU, calculate data size in bytes
        size_t data_bytes = _emit_pool_size * particle_size;
        glBufferData(GL_ARRAY_BUFFER, data_bytes, &_particles[0], GL_DYNAMIC_DRAW);
    }
};
}

#endif
