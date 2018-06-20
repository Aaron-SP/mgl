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
#ifndef __WAVEFRONT__
#define __WAVEFRONT__

#include <array>
#include <fstream>
#include <limits>
#include <map>
#include <min/mem_chunk.h>
#include <min/mesh.h>
#include <min/strtoken.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// This class offers limited support for reading WaveFront .obj files
// The file will be parsed into a list of vertices, uv texture coordinates,
// normals and index buffers.

// Support is limited to fully triangulated meshes, that have uv and normals
// defined for each vertex

namespace min
{

template <typename T, typename K>
class wavefront
{
  private:
    std::vector<mesh<T, K>> _mesh;
    std::vector<vec4<T>> _v;
    std::vector<vec2<T>> _uv;
    std::vector<vec3<T>> _n;
    std::vector<K> _i;
    bool _invert;

    inline void flush()
    {
        // Check if we have a mesh to load
        if (_mesh.size() > 0)
        {
            // Get the last mesh and process the data
            process_mesh(_mesh.back());
        }
    }
    inline void load_file(const std::string _file)
    {
        std::ifstream file(_file, std::ios::in | std::ios::binary | std::ios::ate);
        if (file.is_open())
        {
            // Get the size of the file
            const auto size = file.tellg();

            // Adjust file pointer to beginning
            file.seekg(0, std::ios::beg);

            // Allocate space for new file
            std::string data(size, 0);

            // Read bytes and close the file
            file.read(&data[0], size);

            // Close the file
            file.close();

            // Process the DDS file
            load(data);
        }
        else
        {
            throw std::runtime_error("md5_mesh: Could not load file '" + _file + "'");
        }
    }
    inline void load(const std::string &data)
    {
        // Get locations of all lines in string buffer
        auto lines = min::read_lines(data);

        // Read line by line
        for (auto &position : lines)
        {
            // read line and trim the line whitespace
            std::string line = data.substr(position.first, position.second);
            min::trim(line);

            // skip empty line size in bytes
            if (line.size() == 0)
            {
                continue;
            }

            // if new object
            if (line.compare(0, 2, "o ") == 0)
            {
                process_object(line);
            }
            // If vertex coordinate
            else if (line.compare(0, 2, "v ") == 0)
            {
                process_vertex(line);
            }
            // if texture coordinate
            else if (line.compare(0, 2, "vt") == 0)
            {
                process_uv(line);
            }
            // if normal coordinate
            else if (line.compare(0, 2, "vn") == 0)
            {
                process_normal(line);
            }
            // if face coordinate
            else if (line.compare(0, 1, "f") == 0)
            {
                process_face(line);
            }
        }

        // Consume data in buffers
        flush();
    }
    inline void process_mesh(mesh<T, K> &mesh)
    {
        // Check attribute indices are multiple of three
        auto size = _i.size();
        if (size % 3 != 0)
        {
            throw std::runtime_error("wavefront: Face attribute indices not multiple of three, invalid format");
        }

        // Use a map to test for unique index combinations
        std::map<std::array<K, 3>, K> map;
        std::vector<std::array<K, 3>> attr;

        // Store all unique attribute nodes in the attribute vector on successful insert
        for (size_t i = 0, j = 0; i < size; i += 3)
        {
            // If inserted into map, copy the attribute to attribute vector
            std::array<K, 3> node = {_i[i], _i[i + 1], _i[i + 2]};
            auto it = map.insert(std::pair<std::array<K, 3>, K>(node, j));
            if (it.second)
            {
                // No duplicate so copy this node
                attr.push_back(node);
                j++;
            }

            // Set index for this node
            mesh.index.push_back(it.first->second);
        }

        // Process all attribute nodes, copy vertex, uv, and normal into buffer using indices
        for (const auto &node : attr)
        {
            // Check that indices are valid before copying, node[N] is index + 1
            if (node[0] > _v.size())
                throw std::runtime_error("wavefront: face index out of range, invalid format");
            if (node[1] > _uv.size())
                throw std::runtime_error("wavefront: face index out of range, invalid format");
            if (node[2] > _n.size())
                throw std::runtime_error("wavefront: face index out of range, invalid format");

            // Get attribute references
            const vec4<T> &v = _v[node[0] - 1];
            const vec2<T> &u = _uv[node[1] - 1];
            const vec3<T> &n = _n[node[2] - 1];

            // Copy attributes
            mesh.vertex.push_back(v);
            mesh.uv.push_back(u);
            mesh.normal.push_back(n);
        }

        // Check for integer overflow
        if (_i.size() > std::numeric_limits<K>::max())
        {
            throw std::runtime_error("wavefront: index integer overflow");
        }

        // Clear out buffers for next object
        _v.clear();
        _uv.clear();
        _n.clear();
        _i.clear();
    }
    inline void process_object(const std::string &line)
    {
        // Trim the mesh name
        std::string name = line.substr(2);
        min::trim(name);

        // Consume data in buffers
        flush();

        // Create new mesh to be processed
        _mesh.emplace_back(name);
    }
    inline void process_vertex(const std::string &line)
    {
        // Parse string to three numbers
        T x, y, z;
        std::istringstream s(line.substr(2));
        s >> x >> y >> z;

        // Check for errors
        if (s.fail())
        {
            throw std::runtime_error("wavefront: Invalid vertex line '" + line + "'");
        }

        // add vertex to list
        _v.emplace_back(x, y, z, 1.0);
    }
    inline void process_uv(const std::string &line)
    {
        // Parse string to two numbers
        std::istringstream s(line.substr(2));
        T u, v;
        s >> u >> v;

        // Check for errors
        if (s.fail())
        {
            throw std::runtime_error("wavefront: Invalid uv line '" + line + "'");
        }

        // Check inverted flag
        // add texture coordinate to list
        if (!_invert)
            _uv.emplace_back(u, v);
        else
            _uv.emplace_back(u, 1.0 - v);
    }
    inline void process_normal(const std::string &line)
    {
        // Parse string to three numbers
        T x, y, z;
        std::istringstream s(line.substr(2));
        s >> x >> y >> z;

        // Check for errors
        if (s.fail())
        {
            throw std::runtime_error("wavefront: Invalid normal line '" + line + "'");
        }

        // Add normals to list
        _n.emplace_back(x, y, z);
    }
    inline void process_face(const std::string &line)
    {
        // Trim the substring
        std::string sub = line.substr(2);
        min::trim(sub);

        // Split on space, verify three columns, "\\s+"
        auto columns = min::split_space(sub);
        if (columns.size() != 3)
        {
            throw std::runtime_error("wavefront: Faces must be triangulated, invalid format '" + sub + "'");
        }

        // For all columns
        for (const auto &c : columns)
        {
            // Split on forward slash, verify 3 rows, "\\/+"
            auto rows = min::split_slash(c);
            if (rows.size() != 3)
            {
                throw std::runtime_error("wavefront: Faces must be fully defined vertex/uv/normal, invalid format '" + c + "'");
            }

            // Add all indices to the index list
            for (const auto &r : rows)
            {
                int value = std::stoi(r);
                _i.push_back(value);
            }
        }
    }

  public:
    wavefront(const std::string &file, const bool invert = false) : _invert(invert)
    {
        load_file(file);
    }
    wavefront(const mem_file &mem, const bool invert = false) : _invert(invert)
    {
        load(mem.to_string());
    }
    const std::vector<mesh<T, K>> &get_meshes() const
    {
        return _mesh;
    }
    std::vector<mesh<T, K>> &get_meshes()
    {
        return _mesh;
    }
};
}

#endif
