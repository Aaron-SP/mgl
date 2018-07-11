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
#ifndef __SPHERE__
#define __SPHERE__

#include <cmath>
#include <cstdio>
#include <min/stack_vector.h>
#include <utility>
#include <vector>

namespace min
{

template <typename T, template <typename> class vec>
class sphere
{
  private:
    vec<T> _center;
    T _radius;
    T _radius2;

    // Calculate an aabbox using the most separating axis
    // Center the sphere at this location
    inline void fit(const std::vector<vec<T>> &verts)
    {
        const auto size = verts.size();
        if (size >= 2)
        {
            // Find the greatest separation
            const std::pair<vec<T>, vec<T>> p = vec<T>::most_separating(verts);
            vec<T> min = p.first;
            vec<T> max = p.second;

            // Set the sphere properties from calculated points
            _center = (min + max) * 0.5;
            const vec<T> dxyz = max - _center;
            _radius2 = dxyz.dot(dxyz);
            _radius = std::sqrt(_radius2);
        }
        else if (size == 1)
        {
            _center = verts[0];
            _radius = _radius2 = 0.0;
        }
    }

  public:
    sphere() : _radius(0.0), _radius2(0.0) {}
    sphere(const vec<T> &c, const T r)
        : _center(c), _radius(r), _radius2(_radius * _radius) {}
    sphere(const vec<T> &c, const T r, const T tol)
        : _center(c), _radius(r + tol), _radius2(_radius * _radius) {}
    sphere(const vec<T> &min, const vec<T> &max)
    {
        // calculate center position of space
        _center = (min + max) * 0.5;

        // Calculate maximum distance from space center
        const vec<T> half_extent = (max - _center);

        // Calculate the square distance between center and extent
        _radius2 = half_extent.dot(half_extent);

        // Calculate the radius
        _radius = std::sqrt(_radius2);
    }
    sphere(const std::vector<vec<T>> &verts) : _center(), _radius()
    {
        // Find maximum separation between points
        fit(verts);

        // Extend sphere if points outside
        add(verts);
    }
    inline void add(const std::vector<vec<T>> &verts)
    {
        for (const auto &v : verts)
        {
            // If point is outside the sphere
            const vec<T> dxyz = v - _center;
            const T d2 = dxyz.dot(dxyz);
            if (d2 > _radius2)
            {
                const T d = std::sqrt(d2);
                const T update = (_radius + d) * 0.5;
                const T k = (update - _radius) / d;

                // Update the sphere radius and center
                _radius = update;
                _radius2 = _radius * _radius;
                _center += dxyz * k;
            }
        }
    }
    inline vec<T> closest_point(const vec<T> &p) const
    {
        const vec<T> direction = (p - _center).normalize();
        return (_center + direction * _radius);
    }
    inline const vec<T> &get_center() const
    {
        return _center;
    }
    inline vec<T> get_extent() const
    {
        // Calculate the circumference of the sphere
        const T factor = vec<T>::inverse_unit_length();
        return vec<T>().set_all(2.0 * _radius * factor);
    }
    inline vec<T> get_min() const
    {
        const T factor = vec<T>::inverse_unit_length();
        return _center - vec<T>().set_all(_radius * factor);
    }
    inline vec<T> get_max() const
    {
        const T factor = vec<T>::inverse_unit_length();
        return _center + vec<T>().set_all(_radius * factor);
    }
    inline T get_radius() const
    {
        return _radius;
    }
    inline T get_square_radius() const
    {
        return _radius2;
    }
    template <typename C>
    inline void grid(C &v, const size_t scale) const
    {
        // Calculate radius for this scale
        const T radius = _radius / scale;

        // Create the grid cells
        vec<T>::grid_center(v, get_min(), get_max(), scale, radius);
    }
    inline vec<T> normal(const vec<T> &p, T &length, const T tolerance) const
    {
        // Calculate normal direction vector
        const vec<T> normal = p - _center;

        // Length is initially zero
        length = 0.0;

        // Test for zero vector if p = center
        T mag2 = normal.dot(normal);
        if (mag2 < tolerance)
        {
            // If the two spheres have the same center use up vector
            return vec<T>::up();
        }

        // Calculate the length
        length = std::sqrt(mag2);

        // Normalize the collision normal vector
        return normal / length;
    }
    inline bool point_inside(const vec<T> &p) const
    {
        const vec<T> d = p - _center;
        return (d.dot(d) <= _radius2);
    }
    inline void set_position(const vec<T> &pos)
    {
        _center = pos;
    }
    inline T square_distance(const vec<T> &p) const
    {
        // Calculate the distance between p and closest point
        const vec<T> closest = closest_point(p);
        const vec<T> d = (p - closest);
        return d.dot(d);
    }
    inline T square_size() const
    {
        // Calculates the squared distance across the sphere extent
        return 4.0 * _radius2;
    }
    template <typename C>
    inline void subdivide(C &v) const
    {
        // Create the subdivided space
        vec<T>::subdivide_center(v, get_min(), get_max());
    }
};
}

#endif
