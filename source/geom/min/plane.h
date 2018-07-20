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
#ifndef _MGL_PLANE_MGL_
#define _MGL_PLANE_MGL_

// This class solves the following plane equation:
// n · (x - x0) = d = 0;
// n · x - n · x0 = d = 0;
// n · x - c = d = 0;

namespace min
{

template <typename T, template <typename> class vec>
class plane
{
  private:
    vec<T> _normal;
    T _constant;

  public:
    // Default plane == xz plane
    plane() : _normal(vec<T>::up()), _constant(0.0) {}
    plane(const vec<T> &a, const vec<T> &b)
    {
        // 2D only
        // Calculate the unit normal vector by calculating orthonal vector to ba
        _normal = (b - a).orthogonal().normalize();
        _constant = _normal.dot(a);
    }
    plane(const vec<T> &a, const vec<T> &b, const vec<T> &c)
    {
        // 3D+
        // Calculate the unit normal vector by calculating ba x ca
        _normal = vec<T>::normal(a, b, c).normalize();
        _constant = _normal.dot(a);
    }
    inline vec<T> get_closest_point(const vec<T> &point) const
    {
        // Calculate the distance between point and the plane
        const T d = get_distance(point);

        // Calculate the point that is d distance away from plane
        return get_point(point, d);
    }
    inline T get_distance(const vec<T> &point) const
    {
        // Calculate the distance from the plane
        // distance = 0 means you are on the plane
        return (_normal.dot(point) - _constant);
    }
    inline const vec<T> &get_normal() const
    {
        return _normal;
    }
    inline vec<T> get_point(const vec<T> &point, const T d) const
    {
        // Return the point distance d away from p
        return (point - _normal * d);
    }
};
}

#endif
