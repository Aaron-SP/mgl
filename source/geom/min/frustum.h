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
#ifndef __FRUSTUM__
#define __FRUSTUM__

#include <cmath>
#include <min/mat4.h>
#include <min/plane.h>
#include <min/vec3.h>

namespace min
{

template <class T>
class frustum
{
  private:
    T _fov; // Screen dimensions
    T _ratio;
    T _zoom;
    vec3<T> _near; // Z planes
    vec3<T> _far;
    vec3<T> _center; // Orientation
    vec3<T> _right;
    mat4<T> _proj; // Matricies
    mat4<T> _view;
    plane<T, vec3> _plane[6]; // Planes
    bool _dirty;              // Needs updating

    // If the plane is facing in the negative direction then the excluding corner
    // is the maximum corner in the plane normal direction else use the minimum corner
    inline bool not_between_plane(const vec3<T> &min, const vec3<T> &max, const int i) const
    {
        // Use min corner for positive axis
        vec3<T> p = min;
        const plane<T, vec3> &pl = _plane[i];
        const vec3<T> &n = pl.get_normal();

        // Get the excluding corner of the range to the plane
        if (n.x() < 0.0)
            p.x(max.x());
        if (n.y() < 0.0)
            p.y(max.y());
        if (n.z() < 0.0)
            p.z(max.z());

        // If the excluding corner is outside the plane half space
        // it can't be between the frustum planes
        if (outside_plane(p, i, 0.0))
        {
            return true;
        }

        return false;
    }
    inline bool outside_plane(const vec3<T> &p, const int i, const T d) const
    {
        // distances are positive because planes point outward
        // a positive distance from plane means it is outside the half space
        return _plane[i].get_distance(p) > d;
    }
    inline void orthographic_frustum()
    {
        // This frustum is symmetric and thus is simplified from the generic equations
        const T r = _near.x();
        const T t = _near.y();
        const T near = _near.z();
        const T far = _far.z();

        // Create orthographic projection matrix
        _proj = mat4<T>(r, t, near, far);
    }
    inline void perspective_frustum()
    {
        // This frustum is symmetric and thus is simplified from the generic equations
        const T r = _near.x();
        const T t = _near.y();
        const T near = _near.z();
        const T far = _far.z();
        const T idz = 1.0 / (far - near);

        // Set symmetric matrix values
        T a = near / r;
        T b = 0.0;
        T c = 0.0;
        T d = 0.0;
        T e = 0.0;
        T f = near / t;
        T g = 0.0;
        T h = 0.0;
        T i = 0.0;
        T j = 0.0;
        T k = -(far + near) * idz;
        T l = (2.0 * far * near) * idz;
        T m = 0.0;
        T n = 0.0;
        T o = 1.0;
        T p = 0.0;

        _proj = mat4<T>(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p);
    }
    inline void update()
    {
        // tan(fov / 2) = top / near
        const T tang = std::tan(deg_to_rad2(_fov)) * _zoom;

        // Calculate near planes
        T ny = _near.z() * tang;
        T nx = ny * _ratio;

        // Calculate far planes
        T fy = _far.z() * tang;
        T fx = fy * _ratio;

        // Update the interval vectors
        _near.x(nx);
        _near.y(ny);
        _far.x(fx);
        _far.y(fy);
    }

  public:
    frustum() : _fov(45.0), _ratio(1.33), _zoom(1.0), _near(0.0, 0.0, 0.1), _far(0.0, 0.0, 200.0), _dirty(true) {}
    frustum(const T ratio, const T fov, const T near, const T far) : _fov(fov), _ratio(ratio), _zoom(1.0), _near(0.0, 0.0, near), _far(0.0, 0.0, far), _dirty(true) {}
    inline bool between(const vec3<T> &min, const vec3<T> &max) const
    {
        if (not_between_plane(min, max, 0))
            return false;
        if (not_between_plane(min, max, 1))
            return false;
        if (not_between_plane(min, max, 2))
            return false;
        if (not_between_plane(min, max, 3))
            return false;
        if (not_between_plane(min, max, 4))
            return false;
        if (not_between_plane(min, max, 5))
            return false;

        return true;
    }
    inline vec3<T> closest_point(const vec3<T> &p) const
    {
        // Returns the closest point on the frustum to p

        // Calculate distances to each plane
        T min = std::abs(_plane[0].get_distance(p));
        T pd1 = std::abs(_plane[1].get_distance(p));
        T pd2 = std::abs(_plane[2].get_distance(p));
        T pd3 = std::abs(_plane[3].get_distance(p));
        T pd4 = std::abs(_plane[4].get_distance(p));
        T pd5 = std::abs(_plane[5].get_distance(p));

        // Get the smallest distance
        int index = 0;
        if (pd1 < min)
        {
            min = pd1;
            index = 1;
        }
        if (pd2 < min)
        {
            min = pd2;
            index = 2;
        }
        if (pd3 < min)
        {
            min = pd3;
            index = 3;
        }
        if (pd4 < min)
        {
            min = pd4;
            index = 4;
        }
        if (pd5 < min)
        {
            min = pd5;
            index = 5;
        }

        // Calculate the closest point on this frustum
        return _plane[index].get_point(p, min);
    }
    inline const vec3<T> &get_center() const
    {
        return _center;
    }
    inline const vec3<T> &get_right() const
    {
        return _right;
    }
    inline const mat4<T> &orthographic()
    {
        // Update the frustum dimensions if dirty
        if (_dirty)
        {
            // Update the frustum properties
            update();

            // Create the orthographic frustum
            orthographic_frustum();

            // No longer dirty
            _dirty = false;
        }

        // Return the view matrix
        return _proj;
    }
    inline const mat4<T> &perspective()
    {
        // Update the frustum dimensions if dirty
        if (_dirty)
        {
            // Update the frustum properties
            update();

            // Create the perspective frustum
            perspective_frustum();

            // No longer dirty
            _dirty = false;
        }

        // Return the view matrix
        return _proj;
    }
    inline const mat4<T> &look_at(const vec3<T> &eye, const vec3<T> &forward, vec3<T> &up)
    {
        // right: up x forward - left handed coordinates
        _right = up.cross(forward);
        _right.y(0.0);
        _right.normalize_unsafe();

        // up: = forward x right - left handed coordinates
        // up is recalculated for stabilization
        up = forward.cross(_right);

        // near corners: top left, top right, bottom left, bottom right
        const vec3<T> near = eye + forward * _near.z();
        vec3<T> tl = near + up * _near.y() - _right * _near.x();
        vec3<T> tr = near + up * _near.y() + _right * _near.x();
        vec3<T> bl = near - up * _near.y() - _right * _near.x();
        vec3<T> br = near - up * _near.y() + _right * _near.x();

        // far corners: top left, top right, bottom left, bottom right
        const vec3<T> far = eye + forward * _far.z();
        vec3<T> ftl = far + up * _far.y() - _right * _far.x();
        vec3<T> ftr = far + up * _far.y() + _right * _far.x();
        vec3<T> fbl = far - up * _far.y() - _right * _far.x();
        vec3<T> fbr = far - up * _far.y() + _right * _far.x();

        // planes: top, bottom, left: all normals point inside
        _plane[0] = plane<T, vec3>(tr, tl, ftl);
        _plane[1] = plane<T, vec3>(bl, br, fbr);
        _plane[2] = plane<T, vec3>(tl, bl, fbl);

        // planes: right, near, far: all normals point inside
        _plane[3] = plane<T, vec3>(br, tr, fbr);
        _plane[4] = plane<T, vec3>(tl, tr, br);
        _plane[5] = plane<T, vec3>(ftr, ftl, fbl);

        // construct the lookat matrix
        _view = mat4<T>(_right, up, forward, eye);

        // Update the frustum center
        _center = (far + near) * 0.5;

        return _view;
    }
    inline void make_dirty()
    {
        _dirty = true;
    }
    inline bool point_inside(const vec3<T> &p) const
    {
        return point_within(p, 0.0);
    }
    inline bool point_within(const vec3<T> &p, const T d) const
    {
        if (outside_plane(p, 0, d))
            return false;
        if (outside_plane(p, 1, d))
            return false;
        if (outside_plane(p, 2, d))
            return false;
        if (outside_plane(p, 3, d))
            return false;
        if (outside_plane(p, 4, d))
            return false;
        if (outside_plane(p, 5, d))
            return false;

        return true;
    }
    inline void set_aspect_ratio(const T x, const T y)
    {
        _ratio = x / y;
        _dirty = true;
    }
    inline void set_fov(const T fov)
    {
        _fov = fov;
        _dirty = true;
    }
    inline void set_near(const T near)
    {
        _near.z(near);
        _dirty = true;
    }
    inline void set_far(const T far)
    {
        _far.z(far);
        _dirty = true;
    }
    inline void zoom(const T zoom)
    {
        _zoom = 1.0 / zoom;
        _dirty = true;
    }
};
}

#endif
