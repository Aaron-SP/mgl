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
    mutable size_t _data_bytes;
    mutable size_t _element_bytes;
    std::vector<T> _data;
    std::vector<K> _element;
    std::vector<std::pair<size_t, size_t>> _data_index;
    std::vector<std::pair<size_t, size_t>> _element_index;
    GLuint _vao;
    GLuint _vbo;
    GLuint _ibo;

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
            const auto element_offset = _element.size();
            _element.resize(element_offset + element_size);

            // Insert new indices using index adjustment
            const size_t index_shift = _data.size() / width;
            for (size_t i = 0; i < element_size; i++)
            {
                // Index is from the start of this mesh
                _element[i + element_offset] = index_shift + m.index[i];
            }

            // Store the object dimensions in the index buffer
            _element_index.push_back(std::make_pair(element_size, element_offset));

            // Resize the vector to hold the new mesh data, 'data_size' is number of floats not bytes
            // 'data_offset' is the number of floats not bytes
            auto data_offset = _data.size();
            const size_t data_size = width * attr_size;
            _data.resize(data_offset + data_size, 0.0);

            // Store the data dimensions in the data index buffer
            _data_index.push_back(std::make_pair(data_size, data_offset));

            // Interlace the data in the buffer
            for (size_t i = 0; i < attr_size; i++, data_offset += width)
            {
                // Vertex specific element-wise copy
                vertex_type<T, K, FLOAT_TYPE>::copy(m, _data, data_offset, i);
            }
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
            const auto &e = _element_index[key];
            if (e.first != element_size)
            {
                throw std::runtime_error("vertex_buffer.swap_mesh(): can only swap mesh with the same exact size");
            }

            // Insert new indices using index adjustment
            const auto &d = _data_index[key];
            const size_t index_shift = d.second / width;
            for (size_t i = 0; i < element_size; i++)
            {
                // Index is from the start of this mesh
                _element[i + e.second] = index_shift + m.index[i];
            }

            // Verify vertex buffer dimensions are compatible
            const size_t data_size = width * attr_size;
            if (d.first != data_size)
            {
                throw std::runtime_error("vertex_buffer.swap_mesh(): can only swap mesh with the same exact size");
            }

            // Interlace the data in the buffer in place
            size_t data_offset = d.second;
            for (size_t i = 0; i < attr_size; i++, data_offset += width)
            {
                // Vertex specific element-wise copy
                vertex_type<T, K, FLOAT_TYPE>::copy(m, _data, data_offset, i);
            }
        }
        else
        {
            throw std::runtime_error("vertex_buffer.push_back_mesh(): vertex or index are of invalid length");
        }
    }
    bool reallocate_buffer() const
    {
        bool out = false;

        const size_t data_bytes = _data.size() * sizeof(T);
        const size_t element_bytes = _element.size() * sizeof(K);

        // Check if we need to reallocate the data buffer
        if (data_bytes > _data_bytes)
        {
            // Allocate twice the required size
            _data_bytes = 2 * data_bytes;

            // Bind the data buffer
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);

            // Allocate 2x needed data
            glBufferData(GL_ARRAY_BUFFER, _data_bytes, nullptr, vertex_type<T, K, FLOAT_TYPE>::buffer_type());

            // Send the entire dataset to GPU
            glBufferSubData(GL_ARRAY_BUFFER, 0, data_bytes, &_data[0]);

            out = true;
        }

        // Check if we need to reallocate the element buffer
        if (element_bytes > _element_bytes)
        {
            // Allocate twice the required size
            _element_bytes = 2 * element_bytes;

            // Bind the element buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

            // Allocate 2x needed data
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, _element_bytes, nullptr, vertex_type<T, K, FLOAT_TYPE>::buffer_type());

            // Send the entire dataset to GPU
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, element_bytes, &_element[0]);

            out = true;
        }

        return out;
    }
    void upload_all() const
    {
        if (_data.size() == 0 && _element.size() == 0)
        {
            // Fail silently
            return;
        }

        // Bind the buffer to hold data
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        // Send the data to the GPU, calculate data size in bytes
        _data_bytes = _data.size() * sizeof(T);
        glBufferData(GL_ARRAY_BUFFER, _data_bytes, &_data[0], vertex_type<T, K, FLOAT_TYPE>::buffer_type());

        // Bind the element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

        // Send the indices to the GPU, calculate index size in bytes
        _element_bytes = _element.size() * sizeof(K);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _element_bytes, &_element[0], vertex_type<T, K, FLOAT_TYPE>::buffer_type());
    }
    void upload(const size_t key) const
    {
        // Check if we have added a mesh
        if (_data_index.size() == 0 && _element_index.size() == 0)
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
        const auto &data = _data_index[key];
        const auto &element = _element_index[key];

        // Bind the buffer to hold data
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        // Send the data to the GPU, calculate data size in bytes
        const size_t data_size_bytes = data.first * sizeof(T);
        const size_t data_offset_bytes = data.second * sizeof(T);
        glBufferSubData(GL_ARRAY_BUFFER, data_offset_bytes, data_size_bytes, &_data[data.second]);

        // Bind the element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

        // Send the indices to the GPU, calculate index size in bytes
        const size_t element_size_bytes = element.first * sizeof(K);
        const size_t element_offset_bytes = element.second * sizeof(K);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, element_offset_bytes, element_size_bytes, &_element[element.second]);
    }
    void upload_last() const
    {
        if (_data_index.size() > 0)
        {
            const size_t back = _data_index.size() - 1;
            upload(back);
        }
    }

  public:
    vertex_buffer() : _data_bytes(0), _element_bytes(0)
    {
        // Generate the VAO for this vertex layout
        glGenVertexArrays(1, &_vao);

        // Bind the vao
        bind();

        // Generate the vertex buffer id
        glGenBuffers(1, &_vbo);

        // Generate the index buffer id
        glGenBuffers(1, &_ibo);

        // Bind the buffer to hold data
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        // vertex specific creation routine
        vertex_type<T, K, FLOAT_TYPE>::create();
    }
    ~vertex_buffer()
    {
        if (_ibo > 0)
        {
            // unbind the VBO and delete it
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glDeleteBuffers(1, &_ibo);
        }
        if (_vbo > 0)
        {
            // Bind the vao
            bind();

            // vertex specific destruction routine
            vertex_type<T, K, FLOAT_TYPE>::destroy();

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
    vertex_buffer(const vertex_buffer &sb) = delete;
    size_t add_mesh(const mesh<T, K> &m)
    {
        // Add mesh to the end of the data buffer
        push_back_mesh(m);

        // Return the mesh index
        return _element_index.size() - 1;
    }
    inline void bind() const
    {
        // Bind the VAO for this buffer
        // Do not call this function often as it is unneeded context switching
        glBindVertexArray(_vao);
    }
    inline void clear()
    {
        // Clears the data in this buffer, but data will remain on GPU until next upload is called
        _data.clear();
        _data_index.clear();
        _element.clear();
        _element_index.clear();

        // Reset buffer byte sizes
        _data_bytes = 0;
        _element_bytes = 0;
    }
    inline void draw(const GLenum mode, const size_t index) const
    {
        // Draw object at index 'n'
        const auto &n = _element_index[index];
        const size_t byte_offset = n.second * sizeof(K);
        glDrawElements(mode, n.first, INT_TYPE, (GLvoid *)byte_offset);
    }
    inline void draw(const GLenum mode, const size_t start, const size_t stop) const
    {
        // Draw object at index 'n'
        const auto &start_n = _element_index[start];
        const auto &stop_n = _element_index[stop];
        const size_t draw_size = stop_n.first + (stop_n.second - start_n.second);
        const size_t byte_offset = start_n.second * sizeof(K);
        glDrawElements(mode, draw_size, INT_TYPE, (GLvoid *)byte_offset);
    }
    inline void draw_all(const GLenum mode) const
    {
        // Draw all objects in the static buffer
        glDrawElements(mode, _element.size(), INT_TYPE, nullptr);
    }
    inline void draw_all_after(const GLenum mode, const size_t index) const
    {
        // Draw all objects after index 'n' to end
        const auto &n = _element_index[index];
        const size_t draw_offset = (n.first + n.second);
        const size_t draw_size = _element.size() - draw_offset;
        const size_t byte_offset = draw_offset * sizeof(K);
        glDrawElements(mode, draw_size, INT_TYPE, (GLvoid *)byte_offset);
    }
    inline void draw_many(const GLenum mode, const size_t index, const size_t count) const
    {
        // Draw 'count' times objects at index 'n'
        const auto &n = _element_index[index];
        const size_t byte_offset = n.second * sizeof(K);
        glDrawElementsInstanced(mode, n.first, INT_TYPE, (GLvoid *)byte_offset, count);
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
