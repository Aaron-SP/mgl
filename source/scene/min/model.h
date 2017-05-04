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
#ifndef __MODEL__
#define __MODEL__

#include <min/vec2.h>
#include <min/vec3.h>
#include <min/vec4.h>
#include <min/mesh.h>
#include <vector>
#include <algorithm>

namespace min
{

template <typename T, typename K, template <typename> class vec, template <typename, template <typename> class> class bound>
class model
{
  protected:
    std::vector<mesh<T, K>> _mesh;
    std::vector<bound<T, vec>> _bounds;
    vec<T> _center;

    inline void calculate_bounds()
    {
        // Creating bounding volumes for all meshes
        for (auto &m : _mesh)
        {
            _bounds.emplace_back(m.vertex);
        }

        // Calculate the model center by averaging all mesh centers
        _center = vec<T>();
        for (size_t i = 0; i < _mesh.size(); i++)
        {
            _center += _bounds[i].get_center();
        }
        _center /= _mesh.size();
    }

  public:
    // This will steal data from provider
    model(std::vector<mesh<T, K>> &&mesh) : _mesh(std::move(mesh)), _center()
    {
        calculate_bounds();

        // calculate normals
        calculate_normals();

        // calculate tangents
        calculate_tangents();
    }
    model(const std::vector<mesh<T, K>> &mesh) : _mesh(mesh), _center()
    {
        calculate_bounds();

        // calculate normals
        calculate_normals();

        // calculate tangents
        calculate_tangents();
    }
    const vec<T> &center_model()
    {
        // Center all vertices in the model
        for (auto &m : _mesh)
        {
            // Center all meshes by substracting the center
            for (auto &vert : m.vertex)
            {
                vert -= _center;
            }
        }

        // Clear out the model bounding volumes
        _bounds.clear();

        // Recalculate the model bounding volumes
        calculate_bounds();

        return _center;
    }
    void calculate_normals()
    {
        for (auto &m : _mesh)
        {
            m.calculate_normals();
        }
    }
    void calculate_tangents()
    {
        for (auto &m : _mesh)
        {
            m.calculate_tangents();
        }
    }
    inline const vec<T> &get_center() const
    {
        return _center;
    }
    inline const std::vector<mesh<T, K>> &get_meshes() const
    {
        // Allows for const usage
        return _mesh;
    }
    inline std::vector<mesh<T, K>> &get_meshes()
    {
        // Allows for const usage
        return _mesh;
    }
};
}

#endif