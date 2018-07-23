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
#ifndef _MGL_TEMPLATE_MATH_MGL_
#define _MGL_TEMPLATE_MATH_MGL_

#include <min/aabbox.h>
#include <min/mat2.h>
#include <min/mat3.h>
#include <min/mat4.h>
#include <min/oobbox.h>
#include <min/quat.h>
#include <min/sphere.h>
#include <min/vec2.h>
#include <min/vec3.h>
#include <min/vec4.h>

namespace min
{

// These functions are only useful for cross-product return types within templates
template <typename T>
inline T align(const T v, const mat2<T> &m)
{
    // Since this is a 2D cross product, the vector in world space == vector in object space!
    return v;
}

template <typename T>
inline vec3<T> align(const vec3<T> &v, const quat<T> &q)
{
    // Convert the world space vector to object space
    return q.inverse().transform(v);
}

template <typename T>
inline vec4<T> align(const vec4<T> &v, const quat<T> &q)
{
    // Convert the world space vector to object space
    return q.inverse().transform(v);
}

template <typename T>
inline T transform(const T v, const mat2<T> &m)
{
    // Since this is a 2D cross product, the vector in world space == vector in object space!
    return v;
}

template <typename T>
inline vec3<T> transform(const vec3<T> &v, const quat<T> &q)
{
    // Convert the world space vector to object space
    return q.transform(v);
}

template <typename T>
inline vec4<T> transform(const vec4<T> &v, const quat<T> &q)
{
    // Convert the world space vector to object space
    return q.transform(v);
}

template <typename T>
inline vec2<T> cross(const T w, const vec2<T> &r)
{
    return r.orthogonal() * w;
}

template <typename T>
inline vec3<T> cross(const vec3<T> &w, const vec3<T> &r)
{
    return w.cross(r);
}

template <typename T>
inline vec4<T> cross(const vec4<T> &w, const vec4<T> &r)
{
    return w.cross(r);
}

template <typename T>
inline T dot(const T v1, const T v2)
{
    return v1 * v2;
}

template <typename T>
inline T dot(const vec3<T> &v1, const vec3<T> &v2)
{
    return v1.dot(v2);
}

template <typename T>
inline T dot(const vec4<T> &v1, const vec4<T> &v2)
{
    return v1.dot(v2);
}

template <typename T>
inline T inverse(const T x)
{
    return 1.0f / x;
}

template <typename T>
inline vec3<T> inverse(const vec3<T> &v)
{
    const T x = 1.0f / v.x();
    const T y = 1.0f / v.y();
    const T z = 1.0f / v.z();

    return vec3<T>(x, y, z);
}

template <typename T>
inline vec4<T> inverse(const vec4<T> &v)
{
    const T x = 1.0f / v.x();
    const T y = 1.0f / v.y();
    const T z = 1.0f / v.z();

    return vec4<T>(x, y, z, 1.0f);
}

// AABB
template <typename T>
inline T get_inertia(const aabbox<T, vec2> &box, const T mass)
{
    // Iz = (1/12) * (x^2 + y^2)
    const vec3<T> &b = box.get_extent();

    // return the local inertia
    return (b.x() * b.x() + b.y() * b.y()) * mass * 0.0833f;
}

template <typename T>
inline vec3<T> get_inertia(const aabbox<T, vec3> &box, const T mass)
{
    // Ix = (1/12) * (y^2 + z^2)
    // Iy = (1/12) * (x^2 + z^2)
    // Iz = (1/12) * (x^2 + y^2)
    const vec3<T> &b = box.get_extent();
    const T x2 = b.x() * b.x();
    const T y2 = b.y() * b.y();
    const T z2 = b.z() * b.z();

    // return the local inertia
    return vec3<T>(y2 + z2, x2 + z2, x2 + y2) * mass * 0.0833f;
}

template <typename T>
inline vec4<T> get_inertia(const aabbox<T, vec4> &box, const T mass)
{
    // Ix = (1/12) * (y^2 + z^2)
    // Iy = (1/12) * (x^2 + z^2)
    // Iz = (1/12) * (x^2 + y^2)
    const vec3<T> &b = box.get_extent();
    const T x2 = b.x() * b.x();
    const T y2 = b.y() * b.y();
    const T z2 = b.z() * b.z();

    // return the local inertia
    return vec3<T>(y2 + z2, x2 + z2, x2 + y2) * mass * 0.0833f;
}

// OOBB
template <typename T>
inline T get_inertia(const oobbox<T, vec2> &box, const T mass)
{
    // Iz = (1/12) * (x^2 + y^2)
    const vec3<T> &b = box.get_extent();

    // return the local inertia
    return (b.x() * b.x() + b.y() * b.y()) * mass * 0.0833f;
}

template <typename T>
inline vec3<T> get_inertia(const oobbox<T, vec3> &box, const T mass)
{
    // Ix = (1/12) * (y^2 + z^2)
    // Iy = (1/12) * (x^2 + z^2)
    // Iz = (1/12) * (x^2 + y^2)
    const vec3<T> &b = box.get_extent();
    const T x2 = b.x() * b.x();
    const T y2 = b.y() * b.y();
    const T z2 = b.z() * b.z();

    // return the local inertia
    return vec3<T>(y2 + z2, x2 + z2, x2 + y2) * mass * 0.0833f;
}

template <typename T>
inline vec4<T> get_inertia(const oobbox<T, vec4> &box, const T mass)
{
    // Ix = (1/12) * (y^2 + z^2)
    // Iy = (1/12) * (x^2 + z^2)
    // Iz = (1/12) * (x^2 + y^2)
    const vec3<T> &b = box.get_extent();
    const T x2 = b.x() * b.x();
    const T y2 = b.y() * b.y();
    const T z2 = b.z() * b.z();

    // return the local inertia
    return vec4<T>(y2 + z2, x2 + z2, x2 + y2, 1.0f) * mass * 0.0833f;
}

// SPHERE
template <typename T>
inline T get_inertia(const sphere<T, vec2> &s, const T mass)
{
    // Iz = m*r^2/2

    // return the local inertia
    return s.get_square_radius() * mass * 0.25f;
}

template <typename T>
inline vec3<T> get_inertia(const sphere<T, vec3> &s, const T mass)
{
    // Ixyz = (2.0f/5.0f)*m*r^2
    T inertia = s.get_square_radius() * mass * 0.4f;

    // return the local inertia
    return vec3<T>().set_all(inertia);
}

template <typename T>
inline vec4<T> get_inertia(const sphere<T, vec4> &s, const T mass)
{
    // Ixyz = (2.0f/5.0f)*m*r^2
    T inertia = s.get_square_radius() * mass * 0.4f;

    // return the local inertia
    return vec4<T>().set_all(inertia);
}

// Can't rotate an AABB reliably so the default implementation is empty
template <typename T>
inline void rotate(aabbox<T, vec2> &box, const mat2<T> &mat) {}

template <typename T>
inline void rotate(aabbox<T, vec3> &box, const quat<T> &q) {}

template <typename T>
inline void rotate(aabbox<T, vec4> &box, const quat<T> &q) {}

template <typename T>
inline void rotate(oobbox<T, vec2> &box, const mat2<T> &m)
{
    // Set shape rotation
    box.set_rotation(m);
}

template <typename T>
inline void rotate(oobbox<T, vec3> &box, const quat<T> &q)
{
    // Set shape rotation
    box.set_rotation(q);
}

template <typename T>
inline void rotate(oobbox<T, vec4> &box, const quat<T> &q)
{
    // Set shape rotation
    box.set_rotation(q);
}

// No need to rotate sphere shape so these are empty on purpose
template <typename T>
inline void rotate(sphere<T, vec2> &s, const mat2<T> &mat) {}

template <typename T>
inline void rotate(sphere<T, vec3> &s, const quat<T> &q) {}

template <typename T>
inline void rotate(sphere<T, vec4> &s, const quat<T> &q) {}
}

#endif
