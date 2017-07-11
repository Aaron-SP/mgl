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
#ifndef __TESTMODEL__
#define __TESTMODEL__

#include <min/aabbox.h>
#include <min/model.h>
#include <min/test.h>
#include <min/vec4.h>
#include <min/wavefront.h>
#include <stdexcept>

bool test_model()
{
    bool out = true;

    // Local variables
    min::vec4<double> v;
    min::vec2<double> uv;
    min::vec3<double> n;
    min::vec3<double> t;
    min::vec3<double> b;
    min::vec3<double> p;
    min::wavefront<double, uint16_t> w("data/models/cube.obj");
    std::vector<min::mesh<double, uint16_t>> &meshes = w.get_meshes();
    std::vector<min::mesh<double, uint16_t>> const *cm = &w.get_meshes();
    min::model<double, uint16_t, min::vec4, min::aabbox> model(std::move(meshes));

    // Test data stealing
    out = out && (meshes.size() == 0);
    if (!out)
    {
        throw std::runtime_error("Failed model data steal");
    }

    // Test model vertex retrieve
    cm = &model.get_meshes();
    v = (*cm)[0].vertex[4];
    out = out && compare(-1.0, v.x(), 1E-4);
    out = out && compare(1.0, v.y(), 1E-4);
    out = out && compare(-1.0, v.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed model vertex retrieve");
    }

    // Test model uv retrieve
    cm = &model.get_meshes();
    uv = (*cm)[0].uv[4];
    out = out && compare(0.3333, uv.x(), 1E-4);
    out = out && compare(0.3333, uv.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed model uv retrieve");
    }

    // Test model normal retrieve
    cm = &model.get_meshes();
    n = (*cm)[0].normal[4];
    out = out && compare(0.0, n.x(), 1E-4);
    out = out && compare(1.0, n.y(), 1E-4);
    out = out && compare(0.0, n.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed model normal retrieve");
    }

    // Test tangent calculation : B x T = N
    model.calculate_tangents();
    t = (*cm)[0].tangent[4];
    b = (*cm)[0].bitangent[4];
    n = b.cross(t).normalize();
    out = out && compare(0.0, n.x(), 1E-4);
    out = out && compare(1.0, n.y(), 1E-4);
    out = out && compare(0.0, n.z(), 1E-4);

    // Check tangent vector
    out = out && compare(0.0, t.x(), 1E-4);
    out = out && compare(0.0, t.y(), 1E-4);
    out = out && compare(-1.0, t.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed model tangent orthogonality");
    }

    // Check tangent vector: N X B = T
    t = n.cross(b);
    out = out && compare(0.0, t.x(), 1E-4);
    out = out && compare(0.0, t.y(), 1E-4);
    out = out && compare(-1.0, t.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed model tangent-bitangent orthogonality");
    }

    // Check bitangent vector
    out = out && compare(1.0, b.x(), 1E-4);
    out = out && compare(0.0, b.y(), 1E-4);
    out = out && compare(0.0, b.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed model bitangent orthogonality");
    }

    // Check bitangent vector: T X N = B
    b = t.cross(n);
    out = out && compare(1.0, b.x(), 1E-4);
    out = out && compare(0.0, b.y(), 1E-4);
    out = out && compare(0.0, b.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed model bitangent-tangent orthogonality");
    }

    // Test normal calculation
    model.calculate_normals();
    n = (*cm)[0].normal[4];
    out = out && compare(0.0, n.x(), 1E-4);
    out = out && compare(1.0, n.y(), 1E-4);
    out = out && compare(0.0, n.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed model calculate_normals");
    }

    // Test get center
    p = model.get_center();
    out = out && compare(0.0, p.x(), 1E-4);
    out = out && compare(0.0, p.y(), 1E-4);
    out = out && compare(0.0, p.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed model get_center");
    }

    // Test center_model
    model.center_model();
    p = model.get_center();
    out = out && compare(0.0, p.x(), 1E-4);
    out = out && compare(0.0, p.y(), 1E-4);
    out = out && compare(0.0, p.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed model center_model");
    }

    return out;
}

#endif
