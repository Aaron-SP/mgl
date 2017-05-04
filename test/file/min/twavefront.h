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
#ifndef __TESTWAVEFRONT__
#define __TESTWAVEFRONT__

#include <min/test.h>
#include <min/wavefront.h>
#include <stdexcept>

bool test_wavefront()
{
    bool out = true;

    // Local variables
    int v;
    int uv;
    int n;
    int i;
    min::vec4<double> v3;
    min::vec3<double> n3;
    min::vec2<double> uv2;
    min::wavefront<double, uint16_t> w("data/models/cube.obj");
    std::vector<min::mesh<double, uint16_t>> meshes = w.get_meshes();

    // Test parse file and check counts
    const min::mesh<double, uint16_t> &m1 = meshes[0];
    v = m1.vertex.size();
    uv = m1.uv.size();
    n = m1.normal.size();
    i = m1.index.size();
    out = out && compare(26, v);
    out = out && compare(26, uv);
    out = out && compare(26, n);
    out = out && compare(36, i);
    if (!out)
    {
        throw std::runtime_error("Failed wavefront mesh1 data parse");
    }

    // Test mesh 1 name
    const std::string &one = m1.get_name();
    out = out && compare(one, "Cube");
    if (!out)
    {
        throw std::runtime_error("Failed wavefront mesh1 name");
    }

    // Test parse file and check counts
    const min::mesh<double, uint16_t> &m2 = meshes[1];
    v = m2.vertex.size();
    uv = m2.uv.size();
    n = m2.normal.size();
    i = m2.index.size();
    out = out && compare(26, v);
    out = out && compare(26, uv);
    out = out && compare(26, n);
    out = out && compare(36, i);
    if (!out)
    {
        throw std::runtime_error("Failed wavefront mesh2 data parse");
    }

    // Test mesh 2 name
    const std::string &two = m2.get_name();
    out = out && compare(two, "Cube 2");
    if (!out)
    {
        throw std::runtime_error("Failed wavefront mesh2 name");
    }

    // Test vertex data access
    v3 = m1.vertex[4];
    out = out && compare(-1.0, v3.x(), 1E-4);
    out = out && compare(1.0, v3.y(), 1E-4);
    out = out && compare(-1.0, v3.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed wavefront data access vertex");
    }

    // Test texture coord data access
    uv2 = m1.uv[4];
    out = out && compare(0.3333, uv2.x(), 1E-4);
    out = out && compare(0.3333, uv2.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed wavefront data access uv");
    }

    // Test normal data access
    n3 = m1.normal[4];
    out = out && compare(0.0, n3.x(), 1E-4);
    out = out && compare(1.0, n3.y(), 1E-4);
    out = out && compare(0.0, n3.z(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed wavefront data access normal");
    }

    // Test index data access
    i = m1.index[2];
    out = out && compare(2, i);
    i = m1.index[3];
    out = out && compare(3, i);
    i = m1.index[4];
    out = out && compare(4, i);
    if (!out)
    {
        throw std::runtime_error("Failed wavefront data access index");
    }

    return out;
}

#endif