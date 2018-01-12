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
#ifndef __MD5_MESH__
#define __MD5_MESH__

#include <array>
#include <min/mat4.h>
#include <min/md5_anim.h>
#include <min/mem_chunk.h>
#include <min/mesh.h>
#include <min/quat.h>
#include <min/strtoken.h>
#include <min/vec3.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// weights and vertex_weights are only needed to compute the bind pose vertex
// after this they can be safely thrown away
// this is why they no longer reside as a mesh property.

namespace min
{

template <class T>
class md5_joint
{
  private:
    std::string _name;
    int _id;
    vec3<T> _position;
    quat<T> _rotation;

  public:
    md5_joint(const std::string &name, const int id, const vec3<T> &p, const quat<T> &r)
        : _name(name), _id(id), _position(p), _rotation(r) {}

    int get_id() const
    {
        return _id;
    }
    const vec3<T> &get_position() const
    {
        return _position;
    }
    const quat<T> &get_rotation() const
    {
        return _rotation;
    }
};

template <class T>
class weight
{
  private:
    unsigned _id;
    T _bias;
    vec3<T> _position;

  public:
    weight(const unsigned id, const T bias, const vec3<T> &p)
        : _id(id), _bias(bias), _position(p) {}

    unsigned get_id() const
    {
        return _id;
    }
    T get_bias() const
    {
        return _bias;
    }
    const vec3<T> &get_position() const
    {
        return _position;
    }
};

template <class T>
class vertex_weight
{
  private:
    unsigned _start;
    unsigned _count;

  public:
    vertex_weight(const unsigned start, const unsigned count)
        : _start(start), _count(count) {}

    unsigned get_start() const
    {
        return _start;
    }
    unsigned get_count() const
    {
        return _count;
    }
};

template <typename T, typename K>
class md5_mesh
{
  private:
    std::vector<mesh<T, K>> _mesh;
    std::vector<md5_joint<T>> _joints;
    std::vector<weight<T>> _weights;
    std::vector<vertex_weight<T>> _vertex_weights;

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
        const auto lines = tools::read_lines(data);

        // Read line by line
        unsigned joints, meshes, mesh_counter;
        joints = meshes = mesh_counter = 0;
        const size_t size = lines.size();
        for (size_t i = 0; i < size; i++)
        {
            // read line and trim the line whitespace
            const auto &position = lines[i];
            std::string line = data.substr(position.first, position.second);
            tools::trim(line);

            // skip empty line size in bytes
            if (line.size() == 0)
            {
                continue;
            }

            // Read the property of this line
            std::string field;
            std::istringstream s(line);
            s >> field;

            // Execute action based on property
            if (tools::to_lower(field).compare("md5version") == 0)
            {
                // Parse the version number and check against target
                int version;
                s >> version;

                // Expected version 10
                if (version != 10)
                {
                    throw std::runtime_error("md5_mesh: Version number unsupported, got '" + std::to_string(version) + "' expected '10'");
                }
            }
            else if (tools::to_lower(field).compare("commandline") == 0)
            {
                // Skip these command parameters, as they are unused
                continue;
            }
            else if (tools::to_lower(field).compare("numjoints") == 0)
            {
                // Reserve space for the joints and bones
                s >> joints;
                if (joints > 0)
                {
                    _joints.reserve(joints);
                }
                else
                {
                    throw std::runtime_error("md5_mesh: no joints specified in file");
                }
            }
            else if (tools::to_lower(field).compare("nummeshes") == 0)
            {
                // Get number of meshes in file
                s >> meshes;
            }
            else if (tools::to_lower(field).compare("joints") == 0)
            {
                // Parse the next 'joints' lines in the file
                const std::vector<std::string> j_lines = tools::get_lines(data, lines, joints, i);

                // process the hierarchy data
                process_joints(j_lines);
            }
            else if (tools::to_lower(field).compare("mesh") == 0)
            {
                // Create a new mesh
                _mesh.emplace_back(std::to_string(++mesh_counter));
                std::istringstream ss;

                {
                    // Scan lines looking for 'numverts'
                    while (tools::to_lower(field).compare("numverts") != 0)
                    {
                        std::string line = tools::get_lines(data, lines, 1, i)[0];
                        ss.str(line);
                        ss.clear();

                        // Parse the first token
                        ss >> field;

                        // Check for error
                        if (ss.fail())
                        {
                            throw std::runtime_error("md5_mesh: couldn't find 'numverts'");
                        }
                    }

                    // Get the number of verts
                    unsigned verts;
                    ss >> verts;

                    // Check for error
                    if (ss.fail())
                    {
                        throw std::runtime_error("md5_mesh: bad vertex count");
                    }

                    // Parse the next 'verts' lines in the file
                    const std::vector<std::string> v_lines = tools::get_lines(data, lines, verts, i);

                    // process the hierarchy data
                    process_vertices(v_lines);
                }

                {
                    // Scan lines looking for 'numtris'
                    while (tools::to_lower(field).compare("numtris") != 0)
                    {
                        std::string line = tools::get_lines(data, lines, 1, i)[0];
                        ss.str(line);
                        ss.clear();

                        // Parse the first token
                        ss >> field;

                        // Check for error
                        if (ss.fail())
                        {
                            throw std::runtime_error("md5_mesh: couldn't find 'numtris'");
                        }
                    }

                    // Get the number of triangles
                    unsigned triangles;
                    ss >> triangles;

                    // Check for error
                    if (ss.fail())
                    {
                        throw std::runtime_error("md5_mesh: bad triangle count");
                    }

                    // Parse the next 'triangles' lines in the file
                    const std::vector<std::string> t_lines = tools::get_lines(data, lines, triangles, i);

                    // process the hierarchy data
                    process_triangles(t_lines);
                }

                {
                    // Scan lines looking for 'numweights'
                    while (tools::to_lower(field).compare("numweights") != 0)
                    {
                        std::string line = tools::get_lines(data, lines, 1, i)[0];
                        ss.str(line);
                        ss.clear();

                        // Parse the first token
                        ss >> field;

                        // Check for error
                        if (ss.fail())
                        {
                            throw std::runtime_error("md5_mesh: couldn't find 'numweights'");
                        }
                    }

                    // Get the number of weights
                    unsigned weights;
                    ss >> weights;

                    // Check for error
                    if (ss.fail())
                    {
                        throw std::runtime_error("md5_mesh: bad weight count");
                    }

                    // Parse the next 'weights' lines in the file
                    const std::vector<std::string> w_lines = tools::get_lines(data, lines, weights, i);

                    // process the hierarchy data
                    process_weights(w_lines);
                }

                // Process mesh
                process_mesh();
            }
            else if (field.compare("}") != 0)
            {
                // There is another command that we do not know about
                throw std::runtime_error("md5_mesh: unknown property '" + field + "'");
            }

            // Check for errors
            if (s.fail())
            {
                throw std::runtime_error("md5_mesh: Invalid line in file '" + line + "'");
            }
        }
    }
    inline void process_joints(const std::vector<std::string> &lines)
    {
        if (lines.size() > 0)
        {
            _joints.reserve(lines.size());
            for (const auto &line : lines)
            {
                std::string name, junk;
                int id;
                T x, y, z;
                T qx, qy, qz;

                // Parse the items on the line
                std::istringstream s(line);

                s >> name;
                s >> id;

                // Skip the '('
                s >> junk;

                // Read the translation
                s >> x;
                s >> y;
                s >> z;

                // Skip the ')' and '('
                s >> junk;
                s >> junk;

                // Read the rotation
                s >> qx;
                s >> qy;
                s >> qz;

                // Remove quotes from string name
                name.erase(std::remove(name.begin(), name.end(), '\"'), name.end());

                // Calculate the quaternion and w component, this normalizes the quaternion
                // We need to negate rotation angle here since we use left-handed coordinates
                quat<T> rotation(0, -qx, -qy, -qz);
                rotation.calculate_w();

                // Add joint to the joint list
                _joints.emplace_back(name, id, vec3<T>(x, y, z), rotation);

                // Check for errors
                if (s.fail())
                {
                    throw std::runtime_error("md5_mesh: invalid joint line: '" + line + "'");
                }
            }
        }
        else
        {
            throw std::runtime_error("md5_mesh: no joints specified in file");
        }
    }
    inline void process_vertices(const std::vector<std::string> &lines)
    {
        if (lines.size() > 0)
        {
            // Get the last mesh for processing and reserve space
            mesh<T, K> &m = _mesh.back();
            _vertex_weights.reserve(lines.size());

            for (const auto &line : lines)
            {
                std::string junk;
                T x, y;
                unsigned start, count;

                // Parse the items on the line
                std::istringstream s(line);

                // Throw away junk
                s >> junk;
                s >> junk;
                s >> junk;

                // Read UV coordinates
                s >> x;
                s >> y;

                // Store this UV in the mesh, reverse the uv.y coordinate to match opengl
                m.uv.emplace_back(x, 1.0 - y);

                // Throw away ')'
                s >> junk;

                // Read vertex weights
                s >> start;
                s >> count;

                // Store the vertex weights in the mesh
                _vertex_weights.emplace_back(start, count);

                // Check for errors
                if (s.fail())
                {
                    throw std::runtime_error("md5_mesh: invalid vertex line: '" + line + "'");
                }
            }
        }
        else
        {
            throw std::runtime_error("md5_mesh: no vertexs specified in file");
        }
    }
    inline void process_triangles(const std::vector<std::string> &lines)
    {
        if (lines.size() > 0)
        {
            // Get the last mesh for processing and reserve space
            mesh<T, K> &m = _mesh.back();
            m.index.reserve(lines.size());

            for (const auto &line : lines)
            {
                std::string junk;
                K i1, i2, i3;

                // Parse the items on the line
                std::istringstream s(line);

                // Throw away junk
                s >> junk;
                s >> junk;

                // Read triangle indices
                s >> i1;
                s >> i2;
                s >> i3;

                // Store the triangle indices in the mesh, MD5 is CCW so reverse the winding order to be CW here
                m.index.push_back(i3);
                m.index.push_back(i2);
                m.index.push_back(i1);

                // Check for errors
                if (s.fail())
                {
                    throw std::runtime_error("md5_mesh: invalid triangle line: '" + line + "'");
                }
            }
        }
        else
        {
            throw std::runtime_error("md5_mesh: no triangles specified in file");
        }
    }
    inline void process_weights(const std::vector<std::string> &lines)
    {
        if (lines.size() > 0)
        {
            // Get the last mesh for processing
            _weights.reserve(lines.size());

            for (const auto &line : lines)
            {
                std::string junk;
                unsigned id;
                T bias, x, y, z;

                // Parse the items on the line
                std::istringstream s(line);

                // Throw away junk
                s >> junk;
                s >> junk;

                // Read id and bias
                s >> id;
                s >> bias;
                s >> junk;

                // Read position
                s >> x;
                s >> y;
                s >> z;

                // Store the weight in the mesh
                _weights.emplace_back(id, bias, vec3<T>(x, y, z));

                // Check for errors
                if (s.fail())
                {
                    throw std::runtime_error("md5_mesh: invalid weights line: '" + line + "'");
                }
            }
        }
        else
        {
            throw std::runtime_error("md5_mesh: no weights specified in file");
        }
    }
    inline void process_mesh()
    {
        // Get a reference to the last mesh
        mesh<T, K> &m = _mesh.back();

        // For all vertex weights
        for (const auto &vw : _vertex_weights)
        {
            // Calculate vertex, bone weight, and bone index
            vec3<T> vertex;
            std::array<T, 4> b_weight = {0.0, 0.0, 0.0, 0.0};
            std::array<T, 4> b_index = {0.0, 0.0, 0.0, 0.0};

            // Check the vertex weight count
            const unsigned count = vw.get_count();
            if (count > 4)
            {
                throw std::runtime_error("md5_mesh: Maximum weight per vertex constraint surpassed.");
            }

            // Check for vertex weight overflow
            const unsigned start = vw.get_start();
            if (start + count > _weights.size())
            {
                throw std::runtime_error("md5_mesh: weight index overflow.");
            }

            // For all vertex weights for this vertex
            for (unsigned i = 0; i < count; i++)
            {
                // Get the weight and joint to calculate the vertex
                const weight<T> &weight = _weights[start + i];

                // Check for joint overflow
                const unsigned id = weight.get_id();
                if (id >= _joints.size())
                {
                    throw std::runtime_error("md5_mesh: joint index overflow.");
                }
                const md5_joint<T> &joint = _joints[id];

                // Rotate the weight position by the joint rotation
                const vec3<T> rotated = joint.get_rotation().transform(weight.get_position());

                // Transform the vertex for each weight
                vertex += (joint.get_position() + rotated) * weight.get_bias();

                // Record the weight index and the weight bias
                b_index[i] = weight.get_id();
                b_weight[i] = weight.get_bias();
            }

            // Record vertex parameters
            m.vertex.push_back(vertex);
            m.bone_index.emplace_back(b_index[0], b_index[1], b_index[2], b_index[3]);
            m.bone_weight.emplace_back(b_weight[0], b_weight[1], b_weight[2], b_weight[3]);
        }

        // Clear the vertex_weights and weight vectors as they are no longer needed
        _vertex_weights.clear();
        _weights.clear();
    }

  public:
    md5_mesh(const std::string &file)
    {
        load_file(file);
    }
    md5_mesh(const mem_file &mem)
    {
        load(mem.to_string());
    }
    const std::vector<md5_joint<T>> &get_joints() const
    {
        return _joints;
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
