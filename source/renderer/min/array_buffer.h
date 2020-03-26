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
#ifndef _MGL_ARRAY_BUFFER_MGL_
#define _MGL_ARRAY_BUFFER_MGL_

#include <cstdint>
#include <min/gl_type.h>
#include <min/mesh.h>
#include <min/window.h>
#include <stdexcept>
#include <utility>

namespace min
{

template <typename T, typename K, template <typename, typename, GLenum> class vertex_type>
class array_buffer
{
  private:
    std::vector<std::vector<T>> _data;
    std::vector<std::vector<std::pair<size_t, size_t>>> _data_index;
    GLuint _vao;
    std::vector<GLuint> _vbo;
    mutable size_t _index;

    inline void check_extensions() const
    {
#if !__EMSCRIPTEN__
        const bool vao = GLEW_ARB_vertex_array_object;
        const bool vbo = GLEW_ARB_vertex_buffer_object;
        const bool inst = GLEW_ARB_draw_instanced;
        const bool eal = GLEW_ARB_explicit_attrib_location;

        // Check that we have the extensions we need
        if (!vao || !vbo || !inst || !eal)
        {
            throw std::runtime_error("array_buffer: minimum extensions not met");
        }

#ifdef MGL_VB43
        // Check extensions
        const bool ab = GLEW_ARB_vertex_attrib_binding;
        if (!ab)
        {
            throw std::runtime_error("array_buffer: minimum extensions not met");
        }
#endif
#endif
    }
    inline void push_back_mesh(const mesh<T, K> &m)
    {
        // Verify vertex attribute size
        const size_t attr_size = m.vertex.size();
        if (attr_size > 0)
        {
            // Check that the mesh is valid for this vertex type
            vertex_type<T, K, FLOAT_TYPE<T>()>::check(m);

            // Get the width of the vertex structure, in floats not bytes
            const size_t width = vertex_type<T, K, FLOAT_TYPE<T>()>::width();

            // Resize the vector to hold the new mesh data, 'float_size' is number of floats not bytes
            // 'float_offset' is the number of floats not bytes
            const size_t float_offset = _data[_index].size();
            const size_t float_size = width * attr_size;
            _data[_index].resize(float_offset + float_size, 0.0);

            // Calculate the vertex offset
            size_t attr_offset = 0;
            if (_data_index[_index].size() > 0)
            {
                const auto &p = _data_index[_index].back();
                attr_offset = p.second + p.first;
            }

            // Store the data dimensions in the data index buffer
            _data_index[_index].push_back(std::make_pair(attr_size, attr_offset));

            // Interlace the data in the buffer in place
            vertex_type<T, K, FLOAT_TYPE<T>()>::copy(_data[_index], m, float_offset);
        }
        else
        {
            throw std::runtime_error("array_buffer.push_back_mesh(): vertex or index are of invalid length");
        }
    }
    inline void upload_all() const
    {
        if (_data[_index].size() == 0)
        {
            // Fail silently
            return;
        }

        // Bind the buffer to hold data
        glBindBuffer(GL_ARRAY_BUFFER, _vbo[_index]);

        // Send the data to the GPU, calculate data size in bytes
        const size_t data_bytes = _data[_index].size() * sizeof(T);
        glBufferData(GL_ARRAY_BUFFER, data_bytes, &_data[_index][0], vertex_type<T, K, FLOAT_TYPE<T>()>::buffer_type());
    }

  public:
    array_buffer(const size_t size = 1)
        : _data(size),
          _data_index(size),
          _vbo(size),
          _index(0)
    {
        // Check for input errors
        if (size == 0)
        {
            throw std::runtime_error("array_buffer: can't allocate zero dimensional vertex_buffer, check constructor");
        }

        // Check that all needed extensions are present
        check_extensions();

        // Generate the VAO for this vertex layout
        glGenVertexArrays(1, &_vao);

        // Bind the vao
        bind();

        // Generate the vertex buffer id
        glGenBuffers(size, _vbo.data());

        // Bind the array buffer to hold data
        glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);

        // vertex specific creation routine
        vertex_type<T, K, FLOAT_TYPE<T>()>::create(_vbo[0]);
    }
    ~array_buffer()
    {
        if (_vbo.size() > 0)
        {
            // Bind the vao
            bind();

            // Vertex specific destruction routine
            vertex_type<T, K, FLOAT_TYPE<T>()>::destroy();

            // Unbind the VBO and delete it
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDeleteBuffers(_vbo.size(), _vbo.data());
        }
        if (_vao > 0)
        {
            // Unbind the VAO and delete it
            glBindVertexArray(0);
            glDeleteVertexArrays(1, &_vao);
        }

        // Check for opengl errors
        throw_gl_error();
    }
    array_buffer(const array_buffer &sb) = delete;
    inline size_t add_mesh(const mesh<T, K> &m)
    {
        // Add mesh to the end of the data buffer
        push_back_mesh(m);

        // Return the mesh index
        return _data_index[_index].size() - 1;
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
    inline void bind_buffer(const size_t index) const
    {
        // This function heavily modifies VAO state
        _index = index;

        // Bind the array buffer to hold data
        glBindBuffer(GL_ARRAY_BUFFER, _vbo[_index]);

        // vertex specific creation routine
        vertex_type<T, K, FLOAT_TYPE<T>()>::change_bind_buffer(_vbo[_index]);
    }
    inline void clear()
    {
        // Clears the data in this buffer, but data will remain on GPU until next upload is called
        _data[_index].clear();
        _data_index[_index].clear();
    }
    inline void draw(const GLenum mode, const size_t index) const
    {
        // Draw object at index
        const auto &p = _data_index[_index][index];
        const size_t offset = p.second;
        const size_t size = p.first;

        // Draw the specific vertices from index
        glDrawArrays(mode, offset, size);
    }
    inline void draw(const GLenum mode, const size_t from, const size_t to) const
    {
        // Draw object at index 'from' to index 'to'
        const auto &start = _data_index[_index][from];
        const auto &stop = _data_index[_index][to];

        // Calculate
        const size_t offset = start.second;
        const size_t size = stop.first + (stop.second - start.second);

        // Draw all of the vertices from 'start' to index 'stop'
        glDrawArrays(mode, offset, size);
    }
    inline void draw_all(const GLenum mode) const
    {
        // Draw all objects in the static buffer
        const size_t draw_size = _data_index[_index].size();
        if (draw_size > 0)
        {
            const auto &p = _data_index[_index].back();
            const size_t size = p.second + p.first;

            // Draw all of the vertices in one pass
            glDrawArrays(mode, 0, size);
        }
    }
    inline void draw_many(const GLenum mode, const size_t index, const size_t primcount) const
    {
        // Intel drivers don't like zero instanced draw calls!
        if (primcount > 0)
        {
            const auto &p = _data_index[_index][index];
            const size_t offset = p.second;
            const size_t size = p.first;

            // Draw the specific vertices from index 'primcount' times
            glDrawArraysInstanced(mode, offset, size, primcount);
        }
    }
    inline void reserve(const size_t vertex, const size_t meshes)
    {
        // Get the width of the vertex structure, in floats not bytes
        const size_t width = vertex_type<T, K, FLOAT_TYPE<T>()>::width();

        // Reserve memory for vertex buffer
        _data[_index].reserve(vertex * width);

        // Reserve memory for mesh indices
        _data_index[_index].reserve(meshes);
    }
    inline void set_buffer(const size_t index) const
    {
        // Set the active buffer
        _index = index;
    }
    inline void upload() const
    {
        // Upload all the data in the buffer
        // Buffer will tightly fit the data, no extra allocation
        upload_all();
    }
};
}

#endif
