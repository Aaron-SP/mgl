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
#ifndef __INTERSECT__
#define __INTERSECT__

#include <min/sphere.h>
#include <min/ray.h>
#include <min/aabbox.h>
#include <min/frustum.h>
#include <cmath>

namespace min
{

// Solves the quadratic equation for sphere ray intersection
// ray: o + t*dir, sphere center: s.c; (ray-sphere).(ray-sphere) = r^2
// d = o - s.c
// (o + t*dir - s.c)·(o + t*dir - s.c) = r^2
// (d + t*dir)·(d + t*dir) = r^2
// (dir·dir)t^2 + 2*(d·dir)t + d·d - r^2 = 0

// Thus if (dir.dir) == 1.0 ...
// t^2 + 2(d·dir)t + (d·d) − r^2 = 0
// t = -b +/- sqrt(b^2 -c)
// b = d·dir
// c = d·d - r2
template <typename T, template <typename> class vec>
bool intersect(const sphere<T, vec> &s, const ray<T, vec> &ray, vec<T> &p)
{
    // If the ray origin is inside the sphere it intersects
    const vec<T> &o = ray.get_origin();
    const vec<T> &d = o - s.get_center();
    const T r2 = s.get_square_radius();
    const T c = d.dot(d) - r2;
    if (c <= 0.0)
    {
        // Ray is inside sphere so return origin
        p = o;
        return true;
    }

    // If the angle between the ray direction
    // and the vector between ray origin and sphere center
    // is positive, then the ray is pointing away from the sphere
    const vec<T> &dir = ray.get_direction();
    const T b = d.dot(dir);
    if (b > 0.0)
    {
        return false;
    }

    // discriminant b^2 - c must be positive for a real solution
    const T disc = b * b - c;
    if (disc < 0.0)
    {
        return false;
    }

    // Calculate intersection point
    const T t = -b - std::sqrt(disc);
    p = ray.interpolate(t);

    return true;
}

// Solves the algebraic equation of a ray intersecting N-planes, where N is the dim of vec
// ray: o + t*dir = x
// plane: n · x - d = 0;
// substitude: n · (o + t*dir) - d = 0;
// <tx, ty, tz> = <(d − o · nx)/(dir · nx), (d − o · ny)/(dir · ny), (d − o · nz)/(dir · nz)>
// Since the box is an aabb, the dot product can be simplified
// <tx, ty, tz> = (d - o) / dir

template <typename T, template <typename> class vec>
bool intersect(const aabbox<T, vec> &box, const ray<T, vec> &ray, vec<T> &p)
{
    const vec<T> &o = ray.get_origin();
    const vec<T> &dir = ray.get_direction();
    const vec<T> &inv = ray.get_inverse();
    const vec<T> &min = box.get_min();
    const vec<T> &max = box.get_max();

    // If parallel to an axis and not in slab
    if (dir.any_zero())
    {
        if (!o.within(min, max))
        {
            return false;
        }
    }

    // Calculate the intersection with near and far plane
    vec<T> near = (min - o) * inv;
    vec<T> far = (max - o) * inv;

    // Order to get the nearer intersection points
    vec<T>::order(near, far);

    // Get the farthest entry into the slab
    const T tmin = near.max();

    // Get the nearest exit from a slab
    const T tmax = far.min();

    // if tmin are > 0.0 and nearest exit > farthest entry we have an intersection
    if (tmax > tmin && tmin > 0.0)
    {
        // Calculate the intersection point
        p = o + (dir * tmin);
        return true;
    }

    return false;
}

// tests if s1 overlaps with s2 by comparing the squared distance between centers
// with the squared sum of radiuses
template <typename T, template <typename> class vec>
inline bool intersect(const sphere<T, vec> &s1, const sphere<T, vec> &s2)
{
    vec<T> d = s1.get_center() - s2.get_center();
    T d2 = d.dot(d);
    T radius_sum = s1.get_radius() + s2.get_radius();
    return (d2 <= radius_sum * radius_sum);
}

// Calculates the closest point on s2 to s1's center
// tests if this point is inside s1
template <typename T, template <typename> class vec>
inline bool intersect(const sphere<T, vec> &s1, const sphere<T, vec> &s2, vec<T> &p)
{
    p = s2.closest_point(s1.get_center());
    return s1.point_inside(p);
}

// Calculates the closest point on box to s's center
// tests if this point is inside s
template <typename T, template <typename> class vec>
inline bool intersect(const sphere<T, vec> &s, const aabbox<T, vec> &box)
{
    vec<T> p = box.closest_point(s.get_center());
    return s.point_inside(p);
}

// Calculates the closest point on box to s's center
// tests if this point is inside s
template <typename T, template <typename> class vec>
inline bool intersect(const aabbox<T, vec> &box, const sphere<T, vec> &s)
{
    return intersect(s, box);
}

// Calculates the closest point on box to s's center
// tests if this point is inside s
template <typename T, template <typename> class vec>
inline bool intersect(const sphere<T, vec> &s, const aabbox<T, vec> &box, vec<T> &p)
{
    p = box.closest_point(s.get_center());
    return s.point_inside(p);
}

// Calculates the closest point on box to s's center
// tests if this point is inside s
template <typename T, template <typename> class vec>
inline bool intersect(const aabbox<T, vec> &box, const sphere<T, vec> &s, vec<T> &p)
{
    return intersect(s, box, p);
}

// tests if box1 intersects with box2
template <typename T, template <typename> class vec>
inline bool intersect(const aabbox<T, vec> &box1, const aabbox<T, vec> &box2)
{
    return box1.get_min() <= box2.get_max() && box1.get_max() >= box2.get_min();
}

// Calculates the closest point on box2 to box1's center
// tests if box1 intersects with box2
template <typename T, template <typename> class vec>
inline bool intersect(const aabbox<T, vec> &box1, const aabbox<T, vec> &box2, vec<T> &p)
{
    p = box2.closest_point(box1.get_center());
    return intersect(box1, box2);
}

// Tests if the sphere intersects the frustum
template <typename T>
inline bool intersect(const frustum<T> &f, const sphere<T, vec3> &s)
{
    return f.point_within(s.get_center(), s.get_radius());
}

// Calculates the closest point on sphere to frustum center
// Tests if the closest point is inside the frustum
template <typename T>
inline bool intersect(const frustum<T> &f, const sphere<T, vec3> &s, vec3<T> &p)
{
    p = s.closest_point(f.get_center());
    return f.point_inside(p);
}

// Tests if the frustum intersects the aabb
// For each plane in frustum determine if the box axis intersects the frustum plane
template <typename T>
inline bool intersect(const frustum<T> &f, const aabbox<T, vec3> &box)
{
    const vec3<T> &min = box.get_min();
    const vec3<T> &max = box.get_max();

    return f.between(min, max);
}

// Calculates the closest point on aabb to frustum center
// Tests if the frustum intersects the aabb
// For each plane in frustum determine if the box axis intersects the frustum plane
template <typename T>
inline bool intersect(const frustum<T> &f, const aabbox<T, vec3> &box, vec3<T> &p)
{
    p = box.closest_point(f.get_center());
    return f.point_inside(p);
}
}

#endif