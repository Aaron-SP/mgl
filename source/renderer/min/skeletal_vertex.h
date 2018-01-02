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
#ifndef __SKELETALVERTEX__
#define __SKELETALVERTEX__

#include <cstring>
#include <min/mesh.h>
#include <min/window.h>

namespace min
{

template <typename T, typename K, GLenum FLOAT_TYPE>
class skeletal_vertex
{
  private:
    // Turn the Struct of Array (SoA) data into Array of Structs (AoS)

    // These are the struct member sizes
    static constexpr size_t vertex_size = sizeof(vec4<T>);
    static constexpr size_t uv_size = sizeof(vec2<T>);
    static constexpr size_t normal_size = sizeof(vec3<T>);
    static constexpr size_t tangent_size = sizeof(vec3<T>);
    static constexpr size_t bitangent_size = sizeof(vec3<T>);
    static constexpr size_t bone_index_size = sizeof(vec4<T>);
    static constexpr size_t bone_weight_size = sizeof(vec4<T>);

    // These are the struct member offsets in floats, not bytes
    static constexpr size_t uv_off = vertex_size / sizeof(T);
    static constexpr size_t normal_off = uv_off + (uv_size / sizeof(T));
    static constexpr size_t tangent_off = normal_off + (normal_size / sizeof(T));
    static constexpr size_t bitangent_off = tangent_off + (tangent_size / sizeof(T));
    static constexpr size_t bone_index_off = bitangent_off + (bitangent_size / sizeof(T));
    static constexpr size_t bone_weight_off = bone_index_off + (bone_index_size / sizeof(T));

    // Compute the size of struct in bytes
    static constexpr size_t width_bytes = vertex_size + uv_size + normal_size + tangent_size + bitangent_size + bone_index_size + bone_weight_size;

    // Compute the size of struct in floats
    static constexpr size_t width_size = width_bytes / sizeof(T);

  public:
    inline static void change_bind_buffer(const GLuint vbo)
    {
#ifdef MGL_VB43
        // No offset, standard stride, binding point 0
        glBindVertexBuffer(0, vbo, 0, width_bytes);
#else
        // Redundantly recreate the vertex attributes
        create_vertex_attributes();
#endif
    }
    inline static void create_vertex_attributes()
    {
#ifdef MGL_VB43
        // Specify the vertex attributes in location = 0, no offset
        glVertexAttribFormat(0, 4, FLOAT_TYPE, GL_FALSE, 0);
        // Specify the uv attributes in location = 1, offset is in bytes
        glVertexAttribFormat(1, 2, FLOAT_TYPE, GL_FALSE, uv_off * sizeof(T));
        // Specify the normal attributes in location = 2, offset is in bytes
        glVertexAttribFormat(2, 3, FLOAT_TYPE, GL_FALSE, normal_off * sizeof(T));
        // Specify the tangent attributes in location = 3, offset is in bytes
        glVertexAttribFormat(3, 3, FLOAT_TYPE, GL_FALSE, tangent_off * sizeof(T));
        // Specify the bitangent attributes in location = 4, offset is in bytes
        glVertexAttribFormat(4, 3, FLOAT_TYPE, GL_FALSE, bitangent_off * sizeof(T));
        // Specify the bone_index attributes in location = 5, offset is in bytes
        glVertexAttribFormat(5, 4, FLOAT_TYPE, GL_FALSE, bone_index_off * sizeof(T));
        // Specify the bone_weight attributes in location = 6, offset is in bytes
        glVertexAttribFormat(6, 4, FLOAT_TYPE, GL_FALSE, bone_weight_off * sizeof(T));
#else
        // Specify the vertex attributes in location = 0, no offset
        glVertexAttribPointer(0, 4, FLOAT_TYPE, GL_FALSE, width_bytes, nullptr);
        // Specify the uv attributes in location = 1, offset is in bytes
        glVertexAttribPointer(1, 2, FLOAT_TYPE, GL_FALSE, width_bytes, (GLvoid *)(uv_off * sizeof(T)));
        // Specify the normal attributes in location = 2, offset is in bytes
        glVertexAttribPointer(2, 3, FLOAT_TYPE, GL_FALSE, width_bytes, (GLvoid *)(normal_off * sizeof(T)));
        // Specify the tangent attributes in location = 3, offset is in bytes
        glVertexAttribPointer(3, 3, FLOAT_TYPE, GL_FALSE, width_bytes, (GLvoid *)(tangent_off * sizeof(T)));
        // Specify the bitangent attributes in location = 4, offset is in bytes
        glVertexAttribPointer(4, 3, FLOAT_TYPE, GL_FALSE, width_bytes, (GLvoid *)(bitangent_off * sizeof(T)));
        // Specify the bone_index attributes in location = 5, offset is in bytes
        glVertexAttribPointer(5, 4, FLOAT_TYPE, GL_FALSE, width_bytes, (GLvoid *)(bone_index_off * sizeof(T)));
        // Specify the bone_weight attributes in location = 6, offset is in bytes
        glVertexAttribPointer(6, 4, FLOAT_TYPE, GL_FALSE, width_bytes, (GLvoid *)(bone_weight_off * sizeof(T)));
#endif
    }
    inline static void create_buffer_binding(const GLuint vbo, const GLuint bind_point)
    {
#ifdef MGL_VB43
        //  Create the buffer binding point
        glVertexAttribBinding(0, bind_point);
        glVertexAttribBinding(1, bind_point);
        glVertexAttribBinding(2, bind_point);
        glVertexAttribBinding(3, bind_point);
        glVertexAttribBinding(4, bind_point);
        glVertexAttribBinding(5, bind_point);
        glVertexAttribBinding(6, bind_point);

        // No offset, standard stride, binding point 0
        glBindVertexBuffer(bind_point, vbo, 0, width_bytes);
#endif
    }
    inline static void create(const GLuint vbo)
    {
        // Enable the attributes
        enable_attributes();

        // Create the vertex attributes
        create_vertex_attributes();

#ifdef MGL_VB43
        // Create the buffer binding point
        create_buffer_binding(vbo, 0);
#endif
    }
    inline static void check(const mesh<T, K> &m)
    {
        // Verify normal, tangent and bitangent sizes
        const auto attr_size = m.vertex.size();
        if (m.uv.size() != attr_size || m.normal.size() != attr_size || m.tangent.size() != attr_size || m.bitangent.size() != attr_size)
        {
            throw std::runtime_error("skeletal_vertex: uv, normals or tangents invalid length");
        }

        // Verify bone_index and bone_weight sizes
        if (m.bone_index.size() != attr_size || m.bone_weight.size() != attr_size)
        {
            throw std::runtime_error("skeletal_vertex: bone_index, bone_weight invalid length");
        }
    }
    inline static void copy(std::vector<T> &data, const mesh<T, K> &m, const size_t mesh_offset)
    {
        const auto attr_size = m.vertex.size();
        for (size_t i = 0, j = mesh_offset; i < attr_size; i++, j += width_size)
        {
            // Copy the vertex data, 4 floats
            std::memcpy(&data[j], &m.vertex[i], vertex_size);

            // Copy the uv data, 2 floats, offset is in number of floats
            std::memcpy(&data[j + uv_off], &m.uv[i], uv_size);

            // Copy the normal data, 3 floats, offset is in number of floats
            std::memcpy(&data[j + normal_off], &m.normal[i], normal_size);

            // Copy the tangent data, 3 floats, offset is in number of floats
            std::memcpy(&data[j + tangent_off], &m.tangent[i], tangent_size);

            // Copy the bitangent data, 3 floats, offset is in number of floats
            std::memcpy(&data[j + bitangent_off], &m.bitangent[i], bitangent_size);

            // Copy the bitangent data, 4 floats, offset is in number of floats
            std::memcpy(&data[j + bone_index_off], &m.bone_index[i], bone_index_size);

            // Copy the bitangent data, 4 floats, offset is in number of floats
            std::memcpy(&data[j + bone_weight_off], &m.bone_weight[i], bone_weight_size);
        }
    }
    inline static void destroy()
    {
        // Disable the vertex attributes before destruction
        disable_attributes();
    }
    inline static void disable_attributes()
    {
        // Disable the vertex attributes
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
        glDisableVertexAttribArray(4);
        glDisableVertexAttribArray(5);
        glDisableVertexAttribArray(6);
    }
    inline static void enable_attributes()
    {
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glEnableVertexAttribArray(6);
    }
    inline static constexpr size_t width()
    {
        return width_size;
    }
    inline static constexpr GLenum buffer_type()
    {
        return GL_STATIC_DRAW;
    }
};
}

#endif
