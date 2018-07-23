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
#ifndef _MGL_CONVERT_MGL_
#define _MGL_CONVERT_MGL_

#include <array>
#include <initializer_list>
#include <min/aabbox.h>
#include <min/mesh.h>
#include <min/sphere.h>

namespace min
{

// Calculates normals and tangents
template <typename T, typename K>
inline void finalize_mesh(min::mesh<T, K> &m)
{
    // Calculate the normals
    m.calculate_normals();

    // Calculate the tangents
    m.calculate_tangents();
}

// Appends a sphere into a mesh for drawing
template <typename T, typename K>
inline void append_mesh(const sphere<T, vec3> &s, min::mesh<T, K> &m)
{
    // Calculate the index offset
    const K index_offset = static_cast<K>(m.vertex.size());

    // Calculate sphere vertices
    std::array<vec4<T>, 240> verts{
        vec4<T>(0, -0.707107, 0.707107, 1.0),
        vec4<T>(-0.270598, -0.92388, 0.270598, 1.0),
        vec4<T>(0, -0.92388, 0.382683, 1.0),
        vec4<T>(-0, -0, 1, 1.0),
        vec4<T>(-0.653281, -0.382684, 0.653282, 1.0),
        vec4<T>(0, -0.382684, 0.923879, 1.0),
        vec4<T>(0, 0.707107, 0.707107, 1.0),
        vec4<T>(-0.653281, 0.382683, 0.653282, 1.0),
        vec4<T>(0, 0.382683, 0.923879, 1.0),
        vec4<T>(0, 0.92388, 0.382683, 1.0),
        vec4<T>(0, 1, 0, 1.0),
        vec4<T>(-0.270598, 0.92388, 0.270598, 1.0),
        vec4<T>(0, -1, 0, 1.0),
        vec4<T>(0, -0.92388, 0.382683, 1.0),
        vec4<T>(-0.270598, -0.92388, 0.270598, 1.0),
        vec4<T>(0, -0.707107, 0.707107, 1.0),
        vec4<T>(-0.653281, -0.382684, 0.653282, 1.0),
        vec4<T>(-0.5, -0.707107, 0.5, 1.0),
        vec4<T>(0, 0.382683, 0.923879, 1.0),
        vec4<T>(-0.707107, -0, 0.707107, 1.0),
        vec4<T>(-0, -0, 1, 1.0),
        vec4<T>(0, 0.707107, 0.707107, 1.0),
        vec4<T>(-0.270598, 0.92388, 0.270598, 1.0),
        vec4<T>(-0.5, 0.707107, 0.5, 1.0),
        vec4<T>(-0.270598, 0.92388, 0.270598, 1.0),
        vec4<T>(0, 1, 0, 1.0),
        vec4<T>(-0.382683, 0.92388, 0, 1.0),
        vec4<T>(0, -1, 0, 1.0),
        vec4<T>(-0.270598, -0.92388, 0.270598, 1.0),
        vec4<T>(-0.382683, -0.92388, 0, 1.0),
        vec4<T>(-0.5, -0.707107, 0.5, 1.0),
        vec4<T>(-0.923879, -0.382684, 0, 1.0),
        vec4<T>(-0.707107, -0.707107, 0, 1.0),
        vec4<T>(-0.653281, 0.382683, 0.653282, 1.0),
        vec4<T>(-1, -0, 0, 1.0),
        vec4<T>(-0.707107, -0, 0.707107, 1.0),
        vec4<T>(-0.270598, 0.92388, 0.270598, 1.0),
        vec4<T>(-0.707107, 0.707107, 0, 1.0),
        vec4<T>(-0.5, 0.707107, 0.5, 1.0),
        vec4<T>(-0.270598, -0.92388, 0.270598, 1.0),
        vec4<T>(-0.707107, -0.707107, 0, 1.0),
        vec4<T>(-0.382683, -0.92388, 0, 1.0),
        vec4<T>(-0.707107, -0, 0.707107, 1.0),
        vec4<T>(-0.923879, -0.382684, 0, 1.0),
        vec4<T>(-0.653281, -0.382684, 0.653282, 1.0),
        vec4<T>(-0.5, 0.707107, 0.5, 1.0),
        vec4<T>(-0.923879, 0.382683, 0, 1.0),
        vec4<T>(-0.653281, 0.382683, 0.653282, 1.0),
        vec4<T>(-0.923879, -0.382684, 0, 1.0),
        vec4<T>(-0.5, -0.707107, -0.5, 1.0),
        vec4<T>(-0.707107, -0.707107, 0, 1.0),
        vec4<T>(-0.923879, 0.382683, 0, 1.0),
        vec4<T>(-0.707107, -0, -0.707106, 1.0),
        vec4<T>(-1, -0, 0, 1.0),
        vec4<T>(-0.382683, 0.92388, 0, 1.0),
        vec4<T>(-0.5, 0.707107, -0.5, 1.0),
        vec4<T>(-0.707107, 0.707107, 0, 1.0),
        vec4<T>(-0.707107, -0.707107, 0, 1.0),
        vec4<T>(-0.270598, -0.92388, -0.270598, 1.0),
        vec4<T>(-0.382683, -0.92388, 0, 1.0),
        vec4<T>(-1, -0, 0, 1.0),
        vec4<T>(-0.653281, -0.382684, -0.653281, 1.0),
        vec4<T>(-0.923879, -0.382684, 0, 1.0),
        vec4<T>(-0.923879, 0.382683, 0, 1.0),
        vec4<T>(-0.5, 0.707107, -0.5, 1.0),
        vec4<T>(-0.653281, 0.382683, -0.653281, 1.0),
        vec4<T>(-0.382683, 0.92388, 0, 1.0),
        vec4<T>(0, 1, 0, 1.0),
        vec4<T>(-0.270598, 0.92388, -0.270598, 1.0),
        vec4<T>(0, -1, 0, 1.0),
        vec4<T>(-0.382683, -0.92388, 0, 1.0),
        vec4<T>(-0.270598, -0.92388, -0.270598, 1.0),
        vec4<T>(-0.5, -0.707107, -0.5, 1.0),
        vec4<T>(-0, -0.92388, -0.382683, 1.0),
        vec4<T>(-0.270598, -0.92388, -0.270598, 1.0),
        vec4<T>(-0.707107, -0, -0.707106, 1.0),
        vec4<T>(-0, -0.382684, -0.923879, 1.0),
        vec4<T>(-0.653281, -0.382684, -0.653281, 1.0),
        vec4<T>(-0.5, 0.707107, -0.5, 1.0),
        vec4<T>(-0, 0.382683, -0.923879, 1.0),
        vec4<T>(-0.653281, 0.382683, -0.653281, 1.0),
        vec4<T>(-0.270598, 0.92388, -0.270598, 1.0),
        vec4<T>(0, 1, 0, 1.0),
        vec4<T>(-0, 0.92388, -0.382683, 1.0),
        vec4<T>(0, -1, 0, 1.0),
        vec4<T>(-0.270598, -0.92388, -0.270598, 1.0),
        vec4<T>(-0, -0.92388, -0.382683, 1.0),
        vec4<T>(-0.5, -0.707107, -0.5, 1.0),
        vec4<T>(-0, -0.382684, -0.923879, 1.0),
        vec4<T>(-0, -0.707107, -0.707106, 1.0),
        vec4<T>(-0.653281, 0.382683, -0.653281, 1.0),
        vec4<T>(-0, -0, -1, 1.0),
        vec4<T>(-0.707107, -0, -0.707106, 1.0),
        vec4<T>(-0.270598, 0.92388, -0.270598, 1.0),
        vec4<T>(-0, 0.707107, -0.707106, 1.0),
        vec4<T>(-0.5, 0.707107, -0.5, 1.0),
        vec4<T>(-0, -0, -1, 1.0),
        vec4<T>(0.653281, -0.382684, -0.653281, 1.0),
        vec4<T>(-0, -0.382684, -0.923879, 1.0),
        vec4<T>(-0, 0.707107, -0.707106, 1.0),
        vec4<T>(0.653281, 0.382683, -0.653281, 1.0),
        vec4<T>(-0, 0.382683, -0.923879, 1.0),
        vec4<T>(-0, 0.92388, -0.382683, 1.0),
        vec4<T>(0, 1, 0, 1.0),
        vec4<T>(0.270598, 0.92388, -0.270598, 1.0),
        vec4<T>(0, -1, 0, 1.0),
        vec4<T>(-0, -0.92388, -0.382683, 1.0),
        vec4<T>(0.270598, -0.92388, -0.270598, 1.0),
        vec4<T>(-0, -0.707107, -0.707106, 1.0),
        vec4<T>(0.653281, -0.382684, -0.653281, 1.0),
        vec4<T>(0.5, -0.707107, -0.5, 1.0),
        vec4<T>(-0, 0.382683, -0.923879, 1.0),
        vec4<T>(0.707107, -0, -0.707106, 1.0),
        vec4<T>(-0, -0, -1, 1.0),
        vec4<T>(-0, 0.92388, -0.382683, 1.0),
        vec4<T>(0.5, 0.707107, -0.5, 1.0),
        vec4<T>(-0, 0.707107, -0.707106, 1.0),
        vec4<T>(-0, -0.707107, -0.707106, 1.0),
        vec4<T>(0.270598, -0.92388, -0.270598, 1.0),
        vec4<T>(-0, -0.92388, -0.382683, 1.0),
        vec4<T>(0, -1, 0, 1.0),
        vec4<T>(0.270598, -0.92388, -0.270598, 1.0),
        vec4<T>(0.382683, -0.92388, 0, 1.0),
        vec4<T>(0.5, -0.707107, -0.5, 1.0),
        vec4<T>(0.923879, -0.382684, 0, 1.0),
        vec4<T>(0.707107, -0.707107, 0, 1.0),
        vec4<T>(0.707107, -0, -0.707106, 1.0),
        vec4<T>(0.923879, 0.382683, 0, 1.0),
        vec4<T>(1, -0, 0, 1.0),
        vec4<T>(0.270598, 0.92388, -0.270598, 1.0),
        vec4<T>(0.707107, 0.707107, 0, 1.0),
        vec4<T>(0.5, 0.707107, -0.5, 1.0),
        vec4<T>(0.5, -0.707107, -0.5, 1.0),
        vec4<T>(0.382683, -0.92388, 0, 1.0),
        vec4<T>(0.270598, -0.92388, -0.270598, 1.0),
        vec4<T>(0.707107, -0, -0.707106, 1.0),
        vec4<T>(0.923879, -0.382684, 0, 1.0),
        vec4<T>(0.653281, -0.382684, -0.653281, 1.0),
        vec4<T>(0.5, 0.707107, -0.5, 1.0),
        vec4<T>(0.923879, 0.382683, 0, 1.0),
        vec4<T>(0.653281, 0.382683, -0.653281, 1.0),
        vec4<T>(0.270598, 0.92388, -0.270598, 1.0),
        vec4<T>(0, 1, 0, 1.0),
        vec4<T>(0.382683, 0.92388, 0, 1.0),
        vec4<T>(0.923879, 0.382683, 0, 1.0),
        vec4<T>(0.707107, -0, 0.707107, 1.0),
        vec4<T>(1, -0, 0, 1.0),
        vec4<T>(0.382683, 0.92388, 0, 1.0),
        vec4<T>(0.5, 0.707107, 0.5, 1.0),
        vec4<T>(0.707107, 0.707107, 0, 1.0),
        vec4<T>(0.382683, -0.92388, 0, 1.0),
        vec4<T>(0.5, -0.707107, 0.5, 1.0),
        vec4<T>(0.270598, -0.92388, 0.270598, 1.0),
        vec4<T>(1, -0, 0, 1.0),
        vec4<T>(0.653281, -0.382684, 0.653281, 1.0),
        vec4<T>(0.923879, -0.382684, 0, 1.0),
        vec4<T>(0.707107, 0.707107, 0, 1.0),
        vec4<T>(0.653281, 0.382683, 0.653281, 1.0),
        vec4<T>(0.923879, 0.382683, 0, 1.0),
        vec4<T>(0.382683, 0.92388, 0, 1.0),
        vec4<T>(0, 1, 0, 1.0),
        vec4<T>(0.270598, 0.92388, 0.270598, 1.0),
        vec4<T>(0, -1, 0, 1.0),
        vec4<T>(0.382683, -0.92388, 0, 1.0),
        vec4<T>(0.270598, -0.92388, 0.270598, 1.0),
        vec4<T>(0.923879, -0.382684, 0, 1.0),
        vec4<T>(0.5, -0.707107, 0.5, 1.0),
        vec4<T>(0.707107, -0.707107, 0, 1.0),
        vec4<T>(0.270598, -0.92388, 0.270598, 1.0),
        vec4<T>(0, -0.707107, 0.707107, 1.0),
        vec4<T>(0, -0.92388, 0.382683, 1.0),
        vec4<T>(0.707107, -0, 0.707107, 1.0),
        vec4<T>(0, -0.382684, 0.923879, 1.0),
        vec4<T>(0.653281, -0.382684, 0.653281, 1.0),
        vec4<T>(0.5, 0.707107, 0.5, 1.0),
        vec4<T>(0, 0.382683, 0.923879, 1.0),
        vec4<T>(0.653281, 0.382683, 0.653281, 1.0),
        vec4<T>(0.270598, 0.92388, 0.270598, 1.0),
        vec4<T>(0, 1, 0, 1.0),
        vec4<T>(0, 0.92388, 0.382683, 1.0),
        vec4<T>(0, -1, 0, 1.0),
        vec4<T>(0.270598, -0.92388, 0.270598, 1.0),
        vec4<T>(0, -0.92388, 0.382683, 1.0),
        vec4<T>(0.653281, -0.382684, 0.653281, 1.0),
        vec4<T>(0, -0.707107, 0.707107, 1.0),
        vec4<T>(0.5, -0.707107, 0.5, 1.0),
        vec4<T>(0.653281, 0.382683, 0.653281, 1.0),
        vec4<T>(-0, -0, 1, 1.0),
        vec4<T>(0.707107, -0, 0.707107, 1.0),
        vec4<T>(0.270598, 0.92388, 0.270598, 1.0),
        vec4<T>(0, 0.707107, 0.707107, 1.0),
        vec4<T>(0.5, 0.707107, 0.5, 1.0),
        vec4<T>(-0.5, -0.707107, 0.5, 1.0),
        vec4<T>(-0.707107, -0, 0.707107, 1.0),
        vec4<T>(-0.5, 0.707107, 0.5, 1.0),
        vec4<T>(0, -0.382684, 0.923879, 1.0),
        vec4<T>(-0.653281, 0.382683, 0.653282, 1.0),
        vec4<T>(0, 0.92388, 0.382683, 1.0),
        vec4<T>(-0.653281, -0.382684, 0.653282, 1.0),
        vec4<T>(-0.923879, 0.382683, 0, 1.0),
        vec4<T>(-0.382683, 0.92388, 0, 1.0),
        vec4<T>(-0.5, -0.707107, 0.5, 1.0),
        vec4<T>(-1, -0, 0, 1.0),
        vec4<T>(-0.707107, 0.707107, 0, 1.0),
        vec4<T>(-0.653281, -0.382684, -0.653281, 1.0),
        vec4<T>(-0.653281, 0.382683, -0.653281, 1.0),
        vec4<T>(-0.270598, 0.92388, -0.270598, 1.0),
        vec4<T>(-0.5, -0.707107, -0.5, 1.0),
        vec4<T>(-0.707107, -0, -0.707106, 1.0),
        vec4<T>(-0.707107, 0.707107, 0, 1.0),
        vec4<T>(-0, -0.707107, -0.707106, 1.0),
        vec4<T>(-0, -0, -1, 1.0),
        vec4<T>(-0, 0.707107, -0.707106, 1.0),
        vec4<T>(-0.653281, -0.382684, -0.653281, 1.0),
        vec4<T>(-0, 0.382683, -0.923879, 1.0),
        vec4<T>(-0, 0.92388, -0.382683, 1.0),
        vec4<T>(0.707107, -0, -0.707106, 1.0),
        vec4<T>(0.5, 0.707107, -0.5, 1.0),
        vec4<T>(-0, -0.382684, -0.923879, 1.0),
        vec4<T>(0.653281, 0.382683, -0.653281, 1.0),
        vec4<T>(0.270598, 0.92388, -0.270598, 1.0),
        vec4<T>(0.5, -0.707107, -0.5, 1.0),
        vec4<T>(0.653281, -0.382684, -0.653281, 1.0),
        vec4<T>(0.653281, 0.382683, -0.653281, 1.0),
        vec4<T>(0.382683, 0.92388, 0, 1.0),
        vec4<T>(0.707107, -0.707107, 0, 1.0),
        vec4<T>(1, -0, 0, 1.0),
        vec4<T>(0.707107, 0.707107, 0, 1.0),
        vec4<T>(0.653281, 0.382683, 0.653281, 1.0),
        vec4<T>(0.270598, 0.92388, 0.270598, 1.0),
        vec4<T>(0.707107, -0.707107, 0, 1.0),
        vec4<T>(0.707107, -0, 0.707107, 1.0),
        vec4<T>(0.5, 0.707107, 0.5, 1.0),
        vec4<T>(0.653281, -0.382684, 0.653281, 1.0),
        vec4<T>(0.5, -0.707107, 0.5, 1.0),
        vec4<T>(-0, -0, 1, 1.0),
        vec4<T>(0, 0.707107, 0.707107, 1.0),
        vec4<T>(0, -0.382684, 0.923879, 1.0),
        vec4<T>(0, 0.382683, 0.923879, 1.0),
        vec4<T>(0, 0.92388, 0.382683, 1.0)};

    // Scale the sphere points by radius
    const T radius = s.get_radius();
    const vec3<T> center = s.get_center();
    for (auto &v : verts)
    {
        v *= radius;
        v += center;
    }

    // Append vertices
    m.vertex.insert(m.vertex.end(), verts.begin(), verts.end());

    // Append the uv coordinates
    m.uv.insert(m.uv.end(),
                std::initializer_list<vec2<T>>{
                    vec2<T>(0.2856, 0.1505),
                    vec2<T>(0.2954, 0.227),
                    vec2<T>(0.2658, 0.2016),
                    vec2<T>(0.3535, 0.0001),
                    vec2<T>(0.4097, 0.1799),
                    vec2<T>(0.3133, 0.0875),
                    vec2<T>(0.7952, 0.3456),
                    vec2<T>(0.9125, 0.3133),
                    vec2<T>(0.8201, 0.4097),
                    vec2<T>(0.773, 0.2954),
                    vec2<T>(0.75, 0.25),
                    vec2<T>(0.7984, 0.2658),
                    vec2<T>(0.25, 0.25),
                    vec2<T>(0.2658, 0.2016),
                    vec2<T>(0.2954, 0.227),
                    vec2<T>(0.2856, 0.1505),
                    vec2<T>(0.4097, 0.1799),
                    vec2<T>(0.3456, 0.2048),
                    vec2<T>(0.8201, 0.4097),
                    vec2<T>(0.9999, 0.3535),
                    vec2<T>(0.8535, 0.4999),
                    vec2<T>(0.7952, 0.3456),
                    vec2<T>(0.7984, 0.2658),
                    vec2<T>(0.8495, 0.2856),
                    vec2<T>(0.7984, 0.2658),
                    vec2<T>(0.75, 0.25),
                    vec2<T>(0.7954, 0.227),
                    vec2<T>(0.25, 0.25),
                    vec2<T>(0.2954, 0.227),
                    vec2<T>(0.2984, 0.2658),
                    vec2<T>(0.3456, 0.2048),
                    vec2<T>(0.4125, 0.3133),
                    vec2<T>(0.3495, 0.2856),
                    vec2<T>(0.9125, 0.3133),
                    vec2<T>(0.9999, 0.1465),
                    vec2<T>(0.9999, 0.3535),
                    vec2<T>(0.7984, 0.2658),
                    vec2<T>(0.8456, 0.2048),
                    vec2<T>(0.8495, 0.2856),
                    vec2<T>(0.2954, 0.227),
                    vec2<T>(0.3495, 0.2856),
                    vec2<T>(0.2984, 0.2658),
                    vec2<T>(0.4999, 0.1465),
                    vec2<T>(0.4125, 0.3133),
                    vec2<T>(0.4097, 0.1799),
                    vec2<T>(0.8495, 0.2856),
                    vec2<T>(0.9097, 0.1799),
                    vec2<T>(0.9125, 0.3133),
                    vec2<T>(0.4125, 0.3133),
                    vec2<T>(0.2952, 0.3456),
                    vec2<T>(0.3495, 0.2856),
                    vec2<T>(0.9097, 0.1799),
                    vec2<T>(0.8535, 0.0001),
                    vec2<T>(0.9999, 0.1465),
                    vec2<T>(0.7954, 0.227),
                    vec2<T>(0.7856, 0.1505),
                    vec2<T>(0.8456, 0.2048),
                    vec2<T>(0.3495, 0.2856),
                    vec2<T>(0.273, 0.2954),
                    vec2<T>(0.2984, 0.2658),
                    vec2<T>(0.4999, 0.3535),
                    vec2<T>(0.3201, 0.4097),
                    vec2<T>(0.4125, 0.3133),
                    vec2<T>(0.9097, 0.1799),
                    vec2<T>(0.7856, 0.1505),
                    vec2<T>(0.8133, 0.0875),
                    vec2<T>(0.7954, 0.227),
                    vec2<T>(0.75, 0.25),
                    vec2<T>(0.7658, 0.2016),
                    vec2<T>(0.25, 0.25),
                    vec2<T>(0.2984, 0.2658),
                    vec2<T>(0.273, 0.2954),
                    vec2<T>(0.2952, 0.3456),
                    vec2<T>(0.2342, 0.2984),
                    vec2<T>(0.273, 0.2954),
                    vec2<T>(0.3535, 0.4999),
                    vec2<T>(0.1867, 0.4125),
                    vec2<T>(0.3201, 0.4097),
                    vec2<T>(0.7856, 0.1505),
                    vec2<T>(0.6799, 0.0903),
                    vec2<T>(0.8133, 0.0875),
                    vec2<T>(0.7658, 0.2016),
                    vec2<T>(0.75, 0.25),
                    vec2<T>(0.727, 0.2046),
                    vec2<T>(0.25, 0.25),
                    vec2<T>(0.273, 0.2954),
                    vec2<T>(0.2342, 0.2984),
                    vec2<T>(0.2952, 0.3456),
                    vec2<T>(0.1867, 0.4125),
                    vec2<T>(0.2144, 0.3495),
                    vec2<T>(0.8133, 0.0875),
                    vec2<T>(0.6465, 0.0001),
                    vec2<T>(0.8535, 0.0001),
                    vec2<T>(0.7658, 0.2016),
                    vec2<T>(0.7048, 0.1544),
                    vec2<T>(0.7856, 0.1505),
                    vec2<T>(0.1465, 0.4999),
                    vec2<T>(0.0903, 0.3201),
                    vec2<T>(0.1867, 0.4125),
                    vec2<T>(0.7048, 0.1544),
                    vec2<T>(0.5875, 0.1867),
                    vec2<T>(0.6799, 0.0903),
                    vec2<T>(0.727, 0.2046),
                    vec2<T>(0.75, 0.25),
                    vec2<T>(0.7016, 0.2342),
                    vec2<T>(0.25, 0.25),
                    vec2<T>(0.2342, 0.2984),
                    vec2<T>(0.2046, 0.273),
                    vec2<T>(0.2144, 0.3495),
                    vec2<T>(0.0903, 0.3201),
                    vec2<T>(0.1544, 0.2952),
                    vec2<T>(0.6799, 0.0903),
                    vec2<T>(0.5001, 0.1465),
                    vec2<T>(0.6465, 0.0001),
                    vec2<T>(0.727, 0.2046),
                    vec2<T>(0.6505, 0.2144),
                    vec2<T>(0.7048, 0.1544),
                    vec2<T>(0.2144, 0.3495),
                    vec2<T>(0.2046, 0.273),
                    vec2<T>(0.2342, 0.2984),
                    vec2<T>(0.25, 0.25),
                    vec2<T>(0.2046, 0.273),
                    vec2<T>(0.2016, 0.2342),
                    vec2<T>(0.1544, 0.2952),
                    vec2<T>(0.0875, 0.1867),
                    vec2<T>(0.1505, 0.2144),
                    vec2<T>(0.5001, 0.1465),
                    vec2<T>(0.5903, 0.3201),
                    vec2<T>(0.5001, 0.3535),
                    vec2<T>(0.7016, 0.2342),
                    vec2<T>(0.6544, 0.2952),
                    vec2<T>(0.6505, 0.2144),
                    vec2<T>(0.1544, 0.2952),
                    vec2<T>(0.2016, 0.2342),
                    vec2<T>(0.2046, 0.273),
                    vec2<T>(0.0001, 0.3535),
                    vec2<T>(0.0875, 0.1867),
                    vec2<T>(0.0903, 0.3201),
                    vec2<T>(0.6505, 0.2144),
                    vec2<T>(0.5903, 0.3201),
                    vec2<T>(0.5875, 0.1867),
                    vec2<T>(0.7016, 0.2342),
                    vec2<T>(0.75, 0.25),
                    vec2<T>(0.7046, 0.273),
                    vec2<T>(0.5903, 0.3201),
                    vec2<T>(0.6465, 0.4999),
                    vec2<T>(0.5001, 0.3535),
                    vec2<T>(0.7046, 0.273),
                    vec2<T>(0.7144, 0.3495),
                    vec2<T>(0.6544, 0.2952),
                    vec2<T>(0.2016, 0.2342),
                    vec2<T>(0.2048, 0.1544),
                    vec2<T>(0.227, 0.2046),
                    vec2<T>(0.0001, 0.1465),
                    vec2<T>(0.1799, 0.0903),
                    vec2<T>(0.0875, 0.1867),
                    vec2<T>(0.6544, 0.2952),
                    vec2<T>(0.6867, 0.4125),
                    vec2<T>(0.5903, 0.3201),
                    vec2<T>(0.7046, 0.273),
                    vec2<T>(0.75, 0.25),
                    vec2<T>(0.7342, 0.2984),
                    vec2<T>(0.25, 0.25),
                    vec2<T>(0.2016, 0.2342),
                    vec2<T>(0.227, 0.2046),
                    vec2<T>(0.0875, 0.1867),
                    vec2<T>(0.2048, 0.1544),
                    vec2<T>(0.1505, 0.2144),
                    vec2<T>(0.227, 0.2046),
                    vec2<T>(0.2856, 0.1505),
                    vec2<T>(0.2658, 0.2016),
                    vec2<T>(0.1465, 0.0001),
                    vec2<T>(0.3133, 0.0875),
                    vec2<T>(0.1799, 0.0903),
                    vec2<T>(0.7144, 0.3495),
                    vec2<T>(0.8201, 0.4097),
                    vec2<T>(0.6867, 0.4125),
                    vec2<T>(0.7342, 0.2984),
                    vec2<T>(0.75, 0.25),
                    vec2<T>(0.773, 0.2954),
                    vec2<T>(0.25, 0.25),
                    vec2<T>(0.227, 0.2046),
                    vec2<T>(0.2658, 0.2016),
                    vec2<T>(0.1799, 0.0903),
                    vec2<T>(0.2856, 0.1505),
                    vec2<T>(0.2048, 0.1544),
                    vec2<T>(0.6867, 0.4125),
                    vec2<T>(0.8535, 0.4999),
                    vec2<T>(0.6465, 0.4999),
                    vec2<T>(0.7342, 0.2984),
                    vec2<T>(0.7952, 0.3456),
                    vec2<T>(0.7144, 0.3495),
                    vec2<T>(0.3456, 0.2048),
                    vec2<T>(0.4999, 0.1465),
                    vec2<T>(0.8495, 0.2856),
                    vec2<T>(0.3133, 0.0875),
                    vec2<T>(0.9125, 0.3133),
                    vec2<T>(0.773, 0.2954),
                    vec2<T>(0.4097, 0.1799),
                    vec2<T>(0.9097, 0.1799),
                    vec2<T>(0.7954, 0.227),
                    vec2<T>(0.3456, 0.2048),
                    vec2<T>(0.4999, 0.3535),
                    vec2<T>(0.8456, 0.2048),
                    vec2<T>(0.3201, 0.4097),
                    vec2<T>(0.8133, 0.0875),
                    vec2<T>(0.7658, 0.2016),
                    vec2<T>(0.2952, 0.3456),
                    vec2<T>(0.3535, 0.4999),
                    vec2<T>(0.8456, 0.2048),
                    vec2<T>(0.2144, 0.3495),
                    vec2<T>(0.1465, 0.4999),
                    vec2<T>(0.7048, 0.1544),
                    vec2<T>(0.3201, 0.4097),
                    vec2<T>(0.6799, 0.0903),
                    vec2<T>(0.727, 0.2046),
                    vec2<T>(0.0001, 0.3535),
                    vec2<T>(0.6505, 0.2144),
                    vec2<T>(0.1867, 0.4125),
                    vec2<T>(0.5875, 0.1867),
                    vec2<T>(0.7016, 0.2342),
                    vec2<T>(0.1544, 0.2952),
                    vec2<T>(0.0903, 0.3201),
                    vec2<T>(0.5875, 0.1867),
                    vec2<T>(0.7046, 0.273),
                    vec2<T>(0.1505, 0.2144),
                    vec2<T>(0.0001, 0.1465),
                    vec2<T>(0.6544, 0.2952),
                    vec2<T>(0.6867, 0.4125),
                    vec2<T>(0.7342, 0.2984),
                    vec2<T>(0.1505, 0.2144),
                    vec2<T>(0.1465, 0.0001),
                    vec2<T>(0.7144, 0.3495),
                    vec2<T>(0.1799, 0.0903),
                    vec2<T>(0.2048, 0.1544),
                    vec2<T>(0.3535, 0.0001),
                    vec2<T>(0.7952, 0.3456),
                    vec2<T>(0.3133, 0.0875),
                    vec2<T>(0.8201, 0.4097),
                    vec2<T>(0.773, 0.2954)});

    // Create indices
    std::array<K, 336> indices{
        0, 1, 2, 3, 4, 5,
        6, 7, 8, 9, 10, 11,
        12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29,
        30, 31, 32, 33, 34, 35,
        36, 37, 38, 39, 40, 41,
        42, 43, 44, 45, 46, 47,
        48, 49, 50, 51, 52, 53,
        54, 55, 56, 57, 58, 59,
        60, 61, 62, 63, 64, 65,
        66, 67, 68, 69, 70, 71,
        72, 73, 74, 75, 76, 77,
        78, 79, 80, 81, 82, 83,
        84, 85, 86, 87, 88, 89,
        90, 91, 92, 93, 94, 95,
        96, 97, 98, 99, 100, 101,
        102, 103, 104, 105, 106, 107,
        108, 109, 110, 111, 112, 113,
        114, 115, 116, 117, 118, 119,
        120, 121, 122, 123, 124, 125,
        126, 127, 128, 129, 130, 131,
        132, 133, 134, 135, 136, 137,
        138, 139, 140, 141, 142, 143,
        144, 145, 146, 147, 148, 149,
        150, 151, 152, 153, 154, 155,
        156, 157, 158, 159, 160, 161,
        162, 163, 164, 165, 166, 167,
        168, 169, 170, 171, 172, 173,
        174, 175, 176, 177, 178, 179,
        180, 181, 182, 183, 184, 185,
        186, 187, 188, 189, 190, 191,
        0, 192, 1, 3, 193, 4,
        6, 194, 7, 15, 195, 16,
        18, 196, 19, 21, 197, 22,
        30, 198, 31, 33, 199, 34,
        36, 200, 37, 39, 201, 40,
        42, 202, 43, 45, 203, 46,
        48, 204, 49, 51, 205, 52,
        54, 206, 55, 57, 207, 58,
        60, 208, 61, 63, 209, 64,
        72, 210, 73, 75, 211, 76,
        78, 212, 79, 87, 213, 88,
        90, 214, 91, 93, 215, 94,
        96, 216, 97, 99, 217, 100,
        108, 218, 109, 111, 219, 112,
        114, 220, 115, 117, 221, 118,
        123, 222, 124, 126, 223, 127,
        129, 224, 130, 132, 225, 133,
        135, 226, 136, 138, 227, 139,
        144, 228, 145, 147, 229, 148,
        150, 230, 151, 153, 231, 154,
        156, 232, 157, 165, 233, 166,
        168, 234, 169, 171, 235, 172,
        174, 236, 175, 183, 237, 184,
        186, 238, 187, 189, 239, 190};

    // Offset the uv for each box in the mesh
    if (index_offset > 0)
    {
        for (auto &i : indices)
        {
            i += index_offset;
        }
    }

    // Append indices
    m.index.insert(m.index.end(), indices.begin(), indices.end());
}

// Converts an aabbox into a mesh for drawing
template <typename T, typename K>
inline void append_mesh(const aabbox<T, vec3> &b, min::mesh<T, K> &m)
{
    // Get box dimensions
    const vec3<T> &min = b.get_min();
    const vec3<T> &max = b.get_max();
    const K index_offset = static_cast<K>(m.vertex.size());

    // Append vertices
    m.vertex.insert(m.vertex.end(),
                    std::initializer_list<vec4<T>>{
                        vec4<T>(min.x(), min.y(), min.z(), 1.0),
                        vec4<T>(max.x(), min.y(), max.z(), 1.0),
                        vec4<T>(min.x(), min.y(), max.z(), 1.0),
                        vec4<T>(max.x(), max.y(), max.z(), 1.0),
                        vec4<T>(min.x(), max.y(), min.z(), 1.0),
                        vec4<T>(min.x(), max.y(), max.z(), 1.0),
                        vec4<T>(min.x(), max.y(), max.z(), 1.0),
                        vec4<T>(min.x(), min.y(), min.z(), 1.0),
                        vec4<T>(min.x(), min.y(), max.z(), 1.0),
                        vec4<T>(min.x(), max.y(), min.z(), 1.0),
                        vec4<T>(max.x(), min.y(), min.z(), 1.0),
                        vec4<T>(min.x(), min.y(), min.z(), 1.0),
                        vec4<T>(max.x(), min.y(), min.z(), 1.0),
                        vec4<T>(max.x(), max.y(), max.z(), 1.0),
                        vec4<T>(max.x(), min.y(), max.z(), 1.0),
                        vec4<T>(min.x(), min.y(), max.z(), 1.0),
                        vec4<T>(max.x(), max.y(), max.z(), 1.0),
                        vec4<T>(min.x(), max.y(), max.z(), 1.0),
                        vec4<T>(max.x(), min.y(), min.z(), 1.0),
                        vec4<T>(max.x(), max.y(), min.z(), 1.0),
                        vec4<T>(min.x(), max.y(), min.z(), 1.0),
                        vec4<T>(max.x(), max.y(), min.z(), 1.0),
                        vec4<T>(max.x(), max.y(), min.z(), 1.0),
                        vec4<T>(max.x(), min.y(), max.z(), 1.0)});

    // Append uv coordinates
    m.uv.insert(m.uv.end(),
                std::initializer_list<vec2<T>>{
                    vec2<T>(1, 0),
                    vec2<T>(0, 1),
                    vec2<T>(0, 0),
                    vec2<T>(1, 0),
                    vec2<T>(0, 1),
                    vec2<T>(0, 0),
                    vec2<T>(1, 0),
                    vec2<T>(0, 1),
                    vec2<T>(0, 0),
                    vec2<T>(1, 0),
                    vec2<T>(0, 1),
                    vec2<T>(0, 0),
                    vec2<T>(0, 0),
                    vec2<T>(1, 1),
                    vec2<T>(0, 1),
                    vec2<T>(1, 0),
                    vec2<T>(0, 1),
                    vec2<T>(0, 0),
                    vec2<T>(1, 1),
                    vec2<T>(1, 1),
                    vec2<T>(1, 1),
                    vec2<T>(1, 1),
                    vec2<T>(1, 0),
                    vec2<T>(1, 1)});

    // Create indices
    std::array<K, 36> indices{
        0, 1, 2,
        3, 4, 5,
        6, 7, 8,
        9, 10, 11,
        12, 13, 14,
        15, 16, 17,
        0, 18, 1,
        3, 19, 4,
        6, 20, 7,
        9, 21, 10,
        12, 22, 13,
        15, 23, 16};

    // Offset the uv for each box in the mesh
    if (index_offset > 0)
    {
        for (auto &i : indices)
        {
            i += index_offset;
        }
    }

    // Append indices
    m.index.insert(m.index.end(), indices.begin(), indices.end());
}

// Converts a sphere into a mesh for drawing
template <typename T, typename K>
inline mesh<T, K> to_mesh(const sphere<T, vec3> &s)
{
    // Create a mesh to return
    min::mesh<T, K> m("sphere");

    // Append a sphere to the mesh
    append_mesh<T, K>(s, m);

    // Finalize the contents of the mesh based off sphere
    finalize_mesh<T, K>(m);

    // return mesh
    return m;
}

// Converts an aabbox into a mesh for drawing
template <typename T, typename K>
inline mesh<T, K> to_mesh(const aabbox<T, vec3> &b)
{
    // Create a mesh to return
    min::mesh<T, K> m("aabbox");

    // Append a box to the mesh
    append_mesh<T, K>(b, m);

    // Finalize the contents of the mesh based off box
    finalize_mesh<T, K>(m);

    // return mesh
    return m;
}
}

#endif
