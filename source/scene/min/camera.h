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
#ifndef __CAMERA__
#define __CAMERA__

#include <cmath>
#include <min/frustum.h>
#include <min/mat4.h>
#include <min/quat.h>
#include <min/vec3.h>

namespace min
{

template <typename T>
class camera
{
  private:
    vec3<T> _p;
    vec3<T> _look;
    frustum<T> _f;
    mat4<T> _pv;
    mat4<T> _v;
    bool _dirty;
    bool _proj_ortho;
    void update()
    {
        if (_dirty)
        {
            // Get the projection matrix
            mat4<T> const *proj = nullptr;
            if (_proj_ortho)
                proj = &_f.orthographic();
            else
                proj = &_f.perspective();

            // Update the view matrix
            _v = _f.look_at(_p, _look, vec3<T>::up());

            // Update the projection-view matrix
            // (A*B)^T = (B^T*A^T)
            // A*B = ((A*B)^T)^T = ((B^T*A^T)^T)
            _pv = (*proj).transpose_multiply(_v);

            // No longer dirty
            _dirty = false;
        }
    }

  public:
    camera() : _look(0.0, 0.0, 1.0), _dirty(true), _proj_ortho(true) {}
    inline const vec3<T> &get_forward() const
    {
        return _f.get_forward();
    }
    inline const vec3<T> &get_right() const
    {
        return _f.get_right();
    }
    inline const vec3<T> &get_up() const
    {
        return _f.get_up();
    }
    inline frustum<T> &get_frustum()
    {
        return _f;
    }
    inline const frustum<T> &get_frustum() const
    {
        return _f;
    }
    inline const vec3<T> &get_look_at() const
    {
        return _look;
    }
    inline const mat4<T> &get_pv_matrix()
    {
        // If the camera needs to be updated
        if (_dirty)
        {
            update();
        }

        // Return the proj-view matrix in opengl format
        return _pv;
    }
    inline const mat4<T> &get_v_matrix()
    {
        // If the camera needs to be updated
        if (_dirty)
        {
            update();
        }

        // Return the view matrix in opengl format
        return _v;
    }
    inline const vec3<T> &get_position() const
    {
        return _p;
    }
    inline void make_dirty()
    {
        _dirty = true;
    }
    inline void move_look_at(const T x, const T y)
    {
        // Calculate the direction camera is facing, centered at origin
        vec3<T> direction = _look - _p;

        // Get the right axis
        const vec3<T> &right = _f.get_right();

        // Rotate the camera by the move.y around the right
        quat<T> rotation(right, y);

        // Rotate the camera by the move.x around the global up
        rotation *= quat<T>(vec3<T>::up(), x);

        // Transform the direction
        direction = rotation.transform(direction);

        // Convert to world space and look at point
        set_look_at(_p + direction);
    }
    vec3<T> project_point(const T s)
    {
        return _p + _f.get_forward() * s;
    }
    inline void set_look_at(const vec3<T> &look)
    {
        _look = look;

        // Camera has moved
        _dirty = true;
    }
    inline void set_orthographic()
    {
        _proj_ortho = true;

        // Clear out the frustum proj matrix
        _f.make_dirty();

        // Projection matrix has changed
        _dirty = true;
    }
    inline void set_perspective()
    {
        _proj_ortho = false;

        // Clear out the frustum proj matrix
        _f.make_dirty();

        // Projection matrix has changed
        _dirty = true;
    }
    inline void set_position(const vec3<T> &p)
    {
        // Change lookat based on updated position
        _look += p - _p;

        // Update position
        _p = p;

        // Camera has moved
        _dirty = true;
    }
};
}

#endif
