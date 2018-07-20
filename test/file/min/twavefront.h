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
#ifndef _MGL_TESTWAVEFRONT_MGL_
#define _MGL_TESTWAVEFRONT_MGL_

#include <limits>
#include <min/test.h>
#include <min/wavefront.h>
#include <stdexcept>

bool test_wavefront()
{
    bool out = true;

    // Print size and alignment of class
    std::cout << "wavefront_size: " << sizeof(min::wavefront<double, uint16_t>) << std::endl;
    std::cout << "wavefront_align: " << alignof(min::wavefront<double, uint16_t>) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "twavefront.h: Testing alignment" << std::endl;
    out = out && test(sizeof(void *) * 16, sizeof(min::wavefront<double, uint16_t>), "Failed wavefront sizeof");
    out = out && test(sizeof(void *), alignof(min::wavefront<double, uint16_t>), "Failed wavefront alignof");
#endif

    // Test small wavefront file
    {
        // Local variables
        int v;
        int uv;
        int n;
        int i;
        int t;
        int bt;
        int bi;
        int bw;

        min::vec4<double> v3;
        min::vec3<double> n3;
        min::vec2<double> uv2;
        min::wavefront<double, uint16_t> w("data/models/cube.obj");
        std::vector<min::mesh<double, uint16_t>> meshes = w.get_meshes();

        // Test parse file and check counts
        min::mesh<double, uint16_t> &m1 = meshes[0];
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
            throw std::runtime_error("Failed wavefront cube mesh1 data parse");
        }

        // Test mesh 1 name
        const std::string &one = m1.get_name();
        out = out && compare(one, "Cube");
        if (!out)
        {
            throw std::runtime_error("Failed wavefront cube mesh1 name");
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
            throw std::runtime_error("Failed wavefront cube mesh2 data parse");
        }

        // Test mesh 2 name
        const std::string &two = m2.get_name();
        out = out && compare(two, "Cube 2");
        if (!out)
        {
            throw std::runtime_error("Failed wavefront cube mesh2 name");
        }

        // Test vertex data access
        v3 = m1.vertex[4];
        out = out && compare(-1.0, v3.x(), 1E-4);
        out = out && compare(1.0, v3.y(), 1E-4);
        out = out && compare(-1.0, v3.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed wavefront cube data access vertex");
        }

        // Test texture coord data access
        uv2 = m1.uv[4];
        out = out && compare(0.3333, uv2.x(), 1E-4);
        out = out && compare(0.3333, uv2.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed wavefront cube data access uv");
        }

        // Test normal data access
        n3 = m1.normal[4];
        out = out && compare(0.0, n3.x(), 1E-4);
        out = out && compare(1.0, n3.y(), 1E-4);
        out = out && compare(0.0, n3.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed wavefront cube data access normal");
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
            throw std::runtime_error("Failed wavefront cube data access index");
        }

        // Test merging this mesh with itself
        m1.merge(m2);
        v = m1.vertex.size();
        uv = m1.uv.size();
        n = m1.normal.size();
        t = m1.tangent.size();
        bt = m1.bitangent.size();
        i = m1.index.size();
        bi = m1.bone_index.size();
        bw = m1.bone_weight.size();
        out = out && compare(52, v);
        out = out && compare(52, uv);
        out = out && compare(52, n);
        out = out && compare(0, t);
        out = out && compare(0, bt);
        out = out && compare(72, i);
        out = out && compare(0, bi);
        out = out && compare(0, bw);
        if (!out)
        {
            throw std::runtime_error("Failed wavefront mesh merge");
        }

        // Test serializing the mesh
        std::vector<uint8_t> stream;
        m1.serialize(stream);
        min::mesh<double, uint16_t> m3("serialize");
        m3.deserialize(stream);
        v = m3.vertex.size();
        uv = m3.uv.size();
        n = m3.normal.size();
        t = m3.tangent.size();
        bt = m3.bitangent.size();
        i = m3.index.size();
        bi = m3.bone_index.size();
        bw = m3.bone_weight.size();
        out = out && compare(52, v);
        out = out && compare(52, uv);
        out = out && compare(52, n);
        out = out && compare(0, t);
        out = out && compare(0, bt);
        out = out && compare(72, i);
        out = out && compare(0, bi);
        out = out && compare(0, bw);
        out = out && compare(m1.vertex[13].x(), m3.vertex[13].x(), 1E-4);
        out = out && compare(m1.uv[9].y(), m3.uv[9].y(), 1E-4);
        out = out && compare(m1.normal[4].z(), m3.normal[4].z(), 1E-4);
        out = out && compare(m1.index[51], m3.index[51]);
        if (!out)
        {
            throw std::runtime_error("Failed wavefront mesh serialize");
        }

        // Test clearing the mesh
        m1.clear();
        v = m1.vertex.size();
        uv = m1.uv.size();
        n = m1.normal.size();
        i = m1.index.size();
        out = out && compare(0, v);
        out = out && compare(0, uv);
        out = out && compare(0, n);
        out = out && compare(0, i);
        if (!out)
        {
            throw std::runtime_error("Failed wavefront mesh clear");
        }
    }

    // Test large wavefront file
    {
        // Since we are using a BMESH, assert floating point compatibility
        static_assert(std::numeric_limits<float>::is_iec559, "IEEE 754 float required");
        static_assert(sizeof(float) == 4, "32 bit IEEE 754 float required");

        int v;
        int uv;
        int n;
        int i;
        min::vec4<float> v3;
        min::vec3<float> n3;
        min::vec2<float> uv2;
        min::wavefront<float, uint32_t> w("data/models/blender_suzanne.obj");
        std::vector<min::mesh<float, uint32_t>> meshes = w.get_meshes();

        // Test parse file and check counts
        const min::mesh<float, uint32_t> &m = meshes[0];
        v = m.vertex.size();
        uv = m.uv.size();
        n = m.normal.size();
        i = m.index.size();
        out = out && compare(188636, v);
        out = out && compare(188636, uv);
        out = out && compare(188636, n);
        out = out && compare(188928, i);
        if (!out)
        {
            throw std::runtime_error("Failed wavefront suzanne mesh data parse");
        }

        // Test mesh 1 name
        const std::string &one = m.get_name();
        out = out && compare(one, "Suzanne");
        if (!out)
        {
            throw std::runtime_error("Failed wavefront suzanne mesh name");
        }

        // Test vertex data access
        v3 = m.vertex[188633];
        out = out && compare(0.206892, v3.x(), 1E-6);
        out = out && compare(-0.115051, v3.y(), 1E-6);
        out = out && compare(-0.77239, v3.z(), 1E-5);
        if (!out)
        {
            throw std::runtime_error("Failed wavefront suzanne data access vertex");
        }

        // Test texture coord data access
        uv2 = m.uv[188633];
        out = out && compare(0.7709, uv2.x(), 1E-4);
        out = out && compare(0.7012, uv2.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed wavefront suzanne data access uv");
        }

        // Test normal data access
        n3 = m.normal[188633];
        out = out && compare(-0.0956, n3.x(), 1E-4);
        out = out && compare(-0.9954, n3.y(), 1E-4);
        out = out && compare(-0.0043, n3.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed wavefront suzanne data access normal");
        }

        // Test index data access
        i = m.index[188925];
        out = out && compare(188633, i);
        i = m.index[188926];
        out = out && compare(188634, i);
        i = m.index[188927];
        out = out && compare(188635, i);
        if (!out)
        {
            throw std::runtime_error("Failed wavefront suzanne data access index");
        }

        // Test serializing the mesh
        std::vector<uint8_t> stream;
        m.serialize(stream);
        min::mesh<float, uint32_t> m2("serialize");
        m2.deserialize(stream);
        v = m2.vertex.size();
        uv = m2.uv.size();
        n = m2.normal.size();
        i = m2.index.size();
        out = out && compare(188636, v);
        out = out && compare(188636, uv);
        out = out && compare(188636, n);
        out = out && compare(188928, i);
        if (!out)
        {
            throw std::runtime_error("Failed wavefront suzanne mesh serialize");
        }

        // Write the model to a file
        m.to_file("data/models/blender_suzanne.bmesh");

        // Read the model from a file
        m2.from_file("data/models/blender_suzanne.bmesh");
        v = m2.vertex.size();
        uv = m2.uv.size();
        n = m2.normal.size();
        i = m2.index.size();
        out = out && compare(188636, v);
        out = out && compare(188636, uv);
        out = out && compare(188636, n);
        out = out && compare(188928, i);
        if (!out)
        {
            throw std::runtime_error("Failed wavefront suzanne mesh serialize");
        }
    }

    return out;
}

#endif
