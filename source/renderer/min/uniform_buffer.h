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
#ifndef __UNIFORMBUFFER__
#define __UNIFORMBUFFER__

#include <cstdint>
#include <cstring>
#include <limits>
#include <min/light.h>
#include <min/mat4.h>
#include <min/program.h>
#include <min/window.h>
#include <stdexcept>

namespace min
{

template <typename T>
class uniform_buffer
{
  private:
    // size of uint
    static constexpr size_t size_bytes = sizeof(int32_t);

    // size of light uniform buffer
    static constexpr size_t sizeof_light = sizeof(light<T>);

    // size of matrix uniform buffer
    static constexpr size_t sizeof_matrix = sizeof(mat4<T>);

  private:
    std::vector<light<T>> _lights;
    std::vector<mat4<T>> _matrix;
    GLuint _lbo;
    GLuint _mbo;
    uint32_t _max_lights;
    uint32_t _max_matrix;
    GLint _light_offsets[2];
    GLint _matrix_offsets[2];

    inline size_t get_light_bytes() const
    {
        return _max_lights * sizeof_light + size_bytes;
    }
    inline size_t get_matrix_bytes() const
    {
        return _max_matrix * sizeof_matrix + size_bytes;
    }
    void set_lights(const program &p) const
    {
        if (_max_lights > 0)
        {
            // Calculate the light block indices, 4-N byte alignment
            const GLchar *names[2] = {
                "light_block.lights[0].color",
                "light_block.size"};

            // Array to store the indices
            GLuint indices[2];

            // Get the indices from the program
            glGetUniformIndices(p.id(), 2, names, indices);

            // Check that the indices are valid
            if (indices[0] == GL_INVALID_INDEX || indices[1] == GL_INVALID_INDEX)
            {
                throw std::runtime_error("uniform_buffer: light uniform indices are invalid");
            }

            // Get the member offsets from the program
            GLint light_offsets[2];
            glGetActiveUniformsiv(p.id(), 2, indices, GL_UNIFORM_OFFSET, light_offsets);

            // Check the array offset, offset MUST BE ZERO
            if (light_offsets[0] != _light_offsets[0])
            {
                throw std::runtime_error("uniform_buffer: light_block.lights offset calculated by opengl is not std140");
            }

            // Check the size offset
            if (light_offsets[1] != _light_offsets[1])
            {
                throw std::runtime_error("uniform_buffer: light_block.size offset calculated by opengl is not std140");
            }

            const GLuint light_index = glGetUniformBlockIndex(p.id(), "light_block");

            // Set the block binding index = 0
            glUniformBlockBinding(p.id(), light_index, 0);
        }
    }
    void set_matrix(const program &p) const
    {
        if (_max_matrix > 0)
        {
            // Calculate the matrix block indices, 4-N byte alignment
            const GLchar *names[2] = {
                "matrix_block.matrix[0]",
                "matrix_block.size"};

            // Array to store the indices
            GLuint indices[2];
            GLint arr_strides[2];
            GLint matrix_strides[2];

            // Get the indices from the program
            glGetUniformIndices(p.id(), 2, names, indices);

            // Check that the indices are valid
            if (indices[0] == GL_INVALID_INDEX || indices[1] == GL_INVALID_INDEX)
            {
                throw std::runtime_error("uniform_buffer: matrix uniform indices are invalid");
            }

            // Get the member offsets from the program
            GLint matrix_offsets[2];
            glGetActiveUniformsiv(p.id(), 2, indices, GL_UNIFORM_OFFSET, matrix_offsets);

            // Get the matrix array stride from the program
            glGetActiveUniformsiv(p.id(), 2, indices, GL_UNIFORM_ARRAY_STRIDE, arr_strides);

            // Get the matrix stride from the program
            glGetActiveUniformsiv(p.id(), 2, indices, GL_UNIFORM_MATRIX_STRIDE, matrix_strides);

            // Check the array offset, offset MUST BE ZERO
            if (matrix_offsets[0] != _matrix_offsets[0])
            {
                throw std::runtime_error("uniform_buffer: matrix_block.matrix offset calculated by opengl is not std140");
            }

            // Check the size offset
            if (matrix_offsets[1] != _matrix_offsets[1])
            {
                throw std::runtime_error("uniform_buffer: matrix_block.size offset calculated by opengl is not std140");
            }

            // Check the array stride, stride MUST BE 64 for float 128 for double
            if (arr_strides[0] != sizeof(mat4<T>))
            {
                throw std::runtime_error("uniform_buffer: matrix_block.matrix array stride calculated by opengl is not std140");
            }

            // Check the matrix stride, stride MUST BE 16 for float 32 for double
            if (matrix_strides[0] != sizeof(vec4<T>))
            {
                throw std::runtime_error("uniform_buffer: matrix_block.matrix matrix stride calculated by opengl is not std140");
            }

            const GLuint matrix_index = glGetUniformBlockIndex(p.id(), "matrix_block");

            // Set the block binding index = 1
            glUniformBlockBinding(p.id(), matrix_index, 1);
        }
    }
    void update_light_buffer() const
    {
        // Check for empty matrix array and return
        if (_lights.size() == 0)
        {
            return;
        }

        // The size of the maximum light count and the size
        // (3 * vec4 = 48) * N + (1 * int = 4)
        const size_t light_bytes = get_light_bytes();
        std::vector<uint8_t> data(light_bytes, 0);

        // Add the lights to the light uniform array
        // We can use one copy here because the array is tightly packed
        size_t copy_bytes = _lights.size() * sizeof_light;
        std::memcpy(&data[_light_offsets[0]], &_lights[0], copy_bytes);

        // GLSL likes to use int vs uint in older drivers
        // We must check for overflow
        if (_lights.size() > std::numeric_limits<int32_t>::max())
        {
            throw std::runtime_error("uniform_buffer: light integer overflow");
        }

        // Set the size property
        int32_t size = (int32_t)_lights.size();
        std::memcpy(&data[_light_offsets[1]], &size, size_bytes);

        // Bind buffer and copy data into it
        glBindBuffer(GL_UNIFORM_BUFFER, _lbo);

        // Send the data to the GPU with light_bytes calculated
        glBufferData(GL_UNIFORM_BUFFER, light_bytes, &data[0], GL_DYNAMIC_DRAW);
    }
    void update_matrix_buffer() const
    {
        // Check for empty matrix array and return
        if (_matrix.size() == 0)
        {
            return;
        }

        // The size of the maximum matrix count and the size
        // (4 * vec4 = 64) * N + (1 * int = 4)
        const size_t matrix_bytes = get_matrix_bytes();
        std::vector<uint8_t> data(matrix_bytes, 0);

        // Add the matrices to the matrix uniform array
        // We can use one copy here because the array is tightly packed
        size_t copy_bytes = _matrix.size() * sizeof_matrix;
        std::memcpy(&data[_matrix_offsets[0]], &_matrix[0], copy_bytes);

        // GLSL likes to use int vs uint in older drivers
        // We must check for overflow
        if (_matrix.size() > std::numeric_limits<int32_t>::max())
        {
            throw std::runtime_error("uniform_buffer: matrix integer overflow");
        }

        // Set the size property
        int32_t size = (int32_t)_matrix.size();
        std::memcpy(&data[_matrix_offsets[1]], &size, size_bytes);

        // Bind buffer and copy data into it
        glBindBuffer(GL_UNIFORM_BUFFER, _mbo);

        // Send the data to the GPU with matrix_bytes calculated
        glBufferData(GL_UNIFORM_BUFFER, matrix_bytes, &data[0], GL_DYNAMIC_DRAW);
    }

  public:
    uniform_buffer(const uint32_t light_size, const uint32_t matrix_size)
        : _max_lights(light_size), _max_matrix(matrix_size),
          _light_offsets{0, (GLint)(get_light_bytes() - size_bytes)},
          _matrix_offsets{0, (GLint)(get_matrix_bytes() - size_bytes)}
    {
        size_t max_size = get_max_buffer_size();
        if (max_size < get_light_bytes() || max_size < get_matrix_bytes())
        {
            throw std::runtime_error("uniform_buffer: max uniform buffer size is too small");
        }

        // create light uniform buffer
        glGenBuffers(1, &_lbo);

        // create matrix uniform buffer
        glGenBuffers(1, &_mbo);
    }
    ~uniform_buffer()
    {
        if (_lbo > 0)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            glDeleteBuffers(1, &_lbo);
        }

        if (_mbo > 0)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            glDeleteBuffers(1, &_mbo);
        }

        // Check for opengl errors
        check_internal_error();
    }
    uniform_buffer(const uniform_buffer &sb) = delete;
    inline size_t add_light(const light<T> &light)
    {
        // Check if the buffer is full yet
        if (_lights.size() == _max_lights)
        {
            throw std::runtime_error("uniform_buffer: light buffer is full, you can't add any more lights");
        }

        _lights.push_back(light);

        // Return light ID
        return _lights.size() - 1;
    }
    inline size_t add_matrix(const mat4<T> &mat)
    {
        // Check if the buffer is full yet
        if (_matrix.size() == _max_matrix)
        {
            throw std::runtime_error("uniform_buffer: matrix buffer is full, you can't add any more matrices");
        }

        _matrix.push_back(mat);

        // Return matrix ID
        return _matrix.size() - 1;
    }
    inline void bind() const
    {
        // Connect the binding index = 0 to the buffer
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _lbo);

        // Connect the binding index = 1 to the buffer
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, _mbo);
    }
    inline size_t get_max_buffer_size() const
    {
        // Get the maximum uniform buffer size
        GLint size;
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &size);

        return (size_t)size;
    }
    inline void set_light(const light<T> &light, const size_t id)
    {
        _lights[id] = light;
    }
    inline void set_matrix(const mat4<T> &mat, const size_t id)
    {
        _matrix[id] = mat;
    }
    inline void set_program(const program &p) const
    {
        // set the light buffer
        set_lights(p);

        // set the matrix buffer
        set_matrix(p);
    }
    inline void set_program_light_only(const program &p) const
    {
        // set the light buffer
        set_lights(p);
    }
    inline void set_program_matrix_only(const program &p) const
    {
        // set the matrix buffer
        set_matrix(p);
    }
    inline void update() const
    {
        // Update the light buffer
        update_light_buffer();

        // Update the matrix buffer
        update_matrix_buffer();
    }
    inline void update_lights() const
    {
        // Update the light buffer
        update_light_buffer();
    }
    inline void update_matrix() const
    {
        // Update the light buffer
        update_matrix_buffer();
    }
};
}

#endif
