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
#ifndef __VERTEXBUFFER__
#define __VERTEXBUFFER__

#include <cstdint>
#include <min/mesh.h>
#include <min/window.h>
#include <stdexcept>
#include <utility>

namespace min
{

template <typename T, typename K, template <typename, typename, GLenum> class vertex_type, GLenum FLOAT_TYPE, GLenum INT_TYPE>
class vertex_buffer
{
  private:
    mutable std::vector<size_t> _data_bytes;
    mutable std::vector<size_t> _element_bytes;
    std::vector<std::vector<T>> _data;
    std::vector<std::vector<K>> _element;
    std::vector<std::vector<std::pair<size_t, size_t>>> _data_index;
    std::vector<std::vector<std::pair<size_t, size_t>>> _element_index;
    GLuint _vao;
    std::vector<GLuint> _vbo;
    std::vector<GLuint> _ibo;
    mutable size_t _index;

    void push_back_mesh(const mesh<T, K> &m)
    {
        // Verify vertex attribute size
        const auto attr_size = m.vertex.size();
        const auto element_size = m.index.size();
        if (attr_size > 0 && element_size > 0)
        {
            // Check that the mesh is valid for this vertex type
            vertex_type<T, K, FLOAT_TYPE>::check(m);

            // Get the width of the vertex structure, in floats not bytes
            const size_t width = vertex_type<T, K, FLOAT_TYPE>::width();

            // Resize the vector to hold the new mesh elements, 'element_size' is number of indices not bytes
            // 'element_offset' is the number of indices not bytes
            const auto element_offset = _element[_index].size();
            _element[_index].resize(element_offset + element_size);

            // Insert new indices using index adjustment
            const size_t index_shift = _data[_index].size() / width;
            for (size_t i = 0; i < element_size; i++)
            {
                // Index is from the start of this mesh
                _element[_index][i + element_offset] = index_shift + m.index[i];
            }

            // Store the object dimensions in the index buffer
            _element_index[_index].push_back(std::make_pair(element_size, element_offset));

            // Resize the vector to hold the new mesh data, 'data_size' is number of floats not bytes
            // 'data_offset' is the number of floats not bytes
            auto data_offset = _data[_index].size();
            const size_t data_size = width * attr_size;
            _data[_index].resize(data_offset + data_size, 0.0);

            // Store the data dimensions in the data index buffer
            _data_index[_index].push_back(std::make_pair(data_size, data_offset));

            // Interlace the data in the buffer in place
            vertex_type<T, K, FLOAT_TYPE>::copy(_data[_index], m, data_offset);
        }
        else
        {
            throw std::runtime_error("vertex_buffer.push_back_mesh(): vertex or index are of invalid length");
        }
    }
    void swap_mesh(const mesh<T, K> &m, const size_t key)
    {
        // Verify vertex attribute size
        const auto attr_size = m.vertex.size();
        const auto element_size = m.index.size();
        if (attr_size > 0 && element_size > 0)
        {
            // Check that the mesh is valid for this vertex type
            vertex_type<T, K, FLOAT_TYPE>::check(m);

            // Get the width of the vertex structure, in floats not bytes
            const size_t width = vertex_type<T, K, FLOAT_TYPE>::width();

            // Verify index buffer dimensions are compatible
            const auto &e = _element_index[_index][key];
            if (e.first != element_size)
            {
                throw std::runtime_error("vertex_buffer.swap_mesh(): can only swap mesh with the same exact size");
            }

            // Insert new indices using index adjustment
            const auto &d = _data_index[_index][key];
            const size_t index_shift = d.second / width;
            for (size_t i = 0; i < element_size; i++)
            {
                // Index is from the start of this mesh
                _element[_index][i + e.second] = index_shift + m.index[i];
            }

            // Verify vertex buffer dimensions are compatible
            const size_t data_size = width * attr_size;
            if (d.first != data_size)
            {
                throw std::runtime_error("vertex_buffer.swap_mesh(): can only swap mesh with the same exact size");
            }

            // Interlace the data in the buffer in place
            const size_t data_offset = d.second;
            vertex_type<T, K, FLOAT_TYPE>::copy(_data[_index], m, data_offset);
        }
        else
        {
            throw std::runtime_error("vertex_buffer.swap_mesh(): vertex or index are of invalid length");
        }
    }
    bool reallocate_buffer() const
    {
        bool out = false;

        const size_t data_bytes = _data[_index].size() * sizeof(T);
        const size_t element_bytes = _element[_index].size() * sizeof(K);

        // Check if we need to reallocate the data buffer
        if (data_bytes > _data_bytes[_index])
        {
            // Allocate twice the required size
            _data_bytes[_index] = 2 * data_bytes;

            // Bind the data buffer
            glBindBuffer(GL_ARRAY_BUFFER, _vbo[_index]);

            // Allocate 2x needed data
            glBufferData(GL_ARRAY_BUFFER, _data_bytes[_index], nullptr, vertex_type<T, K, FLOAT_TYPE>::buffer_type());

            // Send the entire dataset to GPU
            glBufferSubData(GL_ARRAY_BUFFER, 0, data_bytes, &_data[_index][0]);

            out = true;
        }

        // Check if we need to reallocate the element buffer
        if (element_bytes > _element_bytes[_index])
        {
            // Allocate twice the required size
            _element_bytes[_index] = 2 * element_bytes;

            // Bind the element buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo[_index]);

            // Allocate 2x needed data
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, _element_bytes[_index], nullptr, vertex_type<T, K, FLOAT_TYPE>::buffer_type());

            // Send the entire dataset to GPU
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, element_bytes, &_element[_index][0]);

            out = true;
        }

        return out;
    }
    void upload_all() const
    {
        if (_data[_index].size() == 0 && _element[_index].size() == 0)
        {
            // Fail silently
            return;
        }

        // Bind the buffer to hold data
        glBindBuffer(GL_ARRAY_BUFFER, _vbo[_index]);

        // Send the data to the GPU, calculate data size in bytes
        _data_bytes[_index] = _data[_index].size() * sizeof(T);
        glBufferData(GL_ARRAY_BUFFER, _data_bytes[_index], &_data[_index][0], vertex_type<T, K, FLOAT_TYPE>::buffer_type());

        // Bind the element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo[_index]);

        // Send the indices to the GPU, calculate index size in bytes
        _element_bytes[_index] = _element[_index].size() * sizeof(K);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _element_bytes[_index], &_element[_index][0], vertex_type<T, K, FLOAT_TYPE>::buffer_type());
    }
    void upload(const size_t key) const
    {
        // Check if we have added a mesh
        if (_data_index[_index].size() == 0 && _element_index[_index].size() == 0)
        {
            // Fail silently
            return;
        }

        // If we need to reallocate the VBO, the entire data is sent so no need to continue
        if (reallocate_buffer())
        {
            return;
        }

        // Get the data indices for the last mesh in the data buffer
        const auto &data = _data_index[_index][key];
        const auto &element = _element_index[_index][key];

        // Bind the buffer to hold data
        glBindBuffer(GL_ARRAY_BUFFER, _vbo[_index]);

        // Send the data to the GPU, calculate data size in bytes
        const size_t data_size_bytes = data.first * sizeof(T);
        const size_t data_offset_bytes = data.second * sizeof(T);
        glBufferSubData(GL_ARRAY_BUFFER, data_offset_bytes, data_size_bytes, &_data[_index][data.second]);

        // Bind the element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo[_index]);

        // Send the indices to the GPU, calculate index size in bytes
        const size_t element_size_bytes = element.first * sizeof(K);
        const size_t element_offset_bytes = element.second * sizeof(K);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, element_offset_bytes, element_size_bytes, &_element[_index][element.second]);
    }
    void upload_last() const
    {
        if (_data_index[_index].size() > 0)
        {
            const size_t back = _data_index[_index].size() - 1;
            upload(back);
        }
    }

  public:
    vertex_buffer(const size_t size = 1)
        : _data_bytes(size, 0), _element_bytes(size, 0),
          _data(size), _element(size),
          _data_index(size), _element_index(size),
          _vbo(size), _ibo(size),
          _index(0)
    {
        // Check for input errors
        if (size == 0)
        {
            throw std::runtime_error("vertex_buffer: can't allocate zero dimensional vertex_buffer, check constructor");
        }

        // Generate the VAO for this vertex layout
        glGenVertexArrays(1, &_vao);

        // Bind the vao
        bind();

        // Generate the vertex buffer id
        glGenBuffers(size, _vbo.data());

        // Generate the index buffer id
        glGenBuffers(size, _ibo.data());

        // Bind the array buffer to hold data
        glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);

        // Bind the element buffer to hold data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo[0]);

        // vertex specific creation routine
        vertex_type<T, K, FLOAT_TYPE>::create(_vbo[0]);
    }
    ~vertex_buffer()
    {
        if (_ibo.size() > 0)
        {
            // Unbind the VBO and delete it
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glDeleteBuffers(_ibo.size(), _ibo.data());
        }
        if (_vbo.size() > 0)
        {
            // Bind the vao
            bind();

            // Vertex specific destruction routine
            vertex_type<T, K, FLOAT_TYPE>::destroy();

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
        check_internal_error();
    }
    vertex_buffer(const vertex_buffer &sb) = delete;
    size_t add_mesh(const mesh<T, K> &m)
    {
        // Add mesh to the end of the data buffer
        push_back_mesh(m);

        // Return the mesh index
        return _element_index[_index].size() - 1;
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

        // Bind the VAO for this buffer
        glBindVertexArray(_vao);

        // Bind the array buffer to hold data
        glBindBuffer(GL_ARRAY_BUFFER, _vbo[_index]);

        // Bind the element buffer to hold data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo[_index]);

        // vertex specific creation routine
        vertex_type<T, K, FLOAT_TYPE>::change_bind_buffer(_vbo[_index]);
    }
    inline void clear()
    {
        // Clears the data in this buffer, but data will remain on GPU until next upload is called
        _data[_index].clear();
        _data_index[_index].clear();
        _element[_index].clear();
        _element_index[_index].clear();

        // Reset buffer byte sizes
        _data_bytes[_index] = 0;
        _element_bytes[_index] = 0;
    }
    inline void draw(const GLenum mode, const size_t index) const
    {
        // Draw object at index 'n'
        const auto &n = _element_index[_index][index];
        const size_t byte_offset = n.second * sizeof(K);
        glDrawElements(mode, n.first, INT_TYPE, (GLvoid *)byte_offset);
    }
    inline void draw(const GLenum mode, const size_t start, const size_t stop) const
    {
        // Draw object at index 'n'
        const auto &start_n = _element_index[_index][start];
        const auto &stop_n = _element_index[_index][stop];
        const size_t draw_size = stop_n.first + (stop_n.second - start_n.second);
        const size_t byte_offset = start_n.second * sizeof(K);
        glDrawElements(mode, draw_size, INT_TYPE, (GLvoid *)byte_offset);
    }
    inline void draw_all(const GLenum mode) const
    {
        // Draw all objects in the static buffer
        glDrawElements(mode, _element[_index].size(), INT_TYPE, nullptr);
    }
    inline void draw_all_after(const GLenum mode, const size_t index) const
    {
        // Draw all objects after index 'n' to end
        const auto &n = _element_index[_index][index];
        const size_t draw_offset = (n.first + n.second);
        const size_t draw_size = _element[_index].size() - draw_offset;
        const size_t byte_offset = draw_offset * sizeof(K);
        glDrawElements(mode, draw_size, INT_TYPE, (GLvoid *)byte_offset);
    }
    inline void draw_many(const GLenum mode, const size_t index, const size_t count) const
    {
        // Intel drivers don't like zero instanced draw calls!
        if (count > 0)
        {
            // Draw 'count' times objects at index 'n'
            const auto &n = _element_index[_index][index];
            const size_t byte_offset = n.second * sizeof(K);
            glDrawElementsInstanced(mode, n.first, INT_TYPE, (GLvoid *)byte_offset, count);
        }
    }
    inline void reserve(const size_t vertex, const size_t index, const size_t meshes)
    {
        // Get the width of the vertex structure, in floats not bytes
        const size_t width = vertex_type<T, K, FLOAT_TYPE>::width();

        // Reserve memory for vertex buffer
        _data[_index].reserve(vertex * width);

        // Reserve memory for element buffer
        _element[_index].reserve(index);

        // Reserve memory for mesh indices
        _data_index[_index].reserve(meshes);
        _element_index[_index].reserve(meshes);
    }
    inline void set_buffer(const size_t index) const
    {
        // Set the active buffer
        _index = index;
    }
    void upload() const
    {
        // Upload all the data in the buffer
        // Buffer will tightly fit the data, no extra allocation
        upload_all();
    }
    void upload_mesh(const mesh<T, K> &m)
    {
        // This function allows dynamic resizing of the VBO
        // Buffer will resize to twice the needed amount

        // Add mesh to the end of the data buffer
        push_back_mesh(m);

        // Upload the last mesh in the buffer
        upload_last();
    }
    void upload_mesh(const mesh<T, K> &m, const size_t index)
    {
        // Update the mesh in 'index' slot
        swap_mesh(m, index);

        // Upload only part of the total vertex buffer
        upload(index);
    }
};
}

#endif
