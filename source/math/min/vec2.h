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
#ifndef _MGL_VECTOR2_MGL_
#define _MGL_VECTOR2_MGL_

namespace min
{
template <typename T, template <typename> class vec>
class coord_sys;
}

#include <algorithm>
#include <cmath>
#include <limits>
#include <min/bi.h>
#include <min/coord_sys.h>
#include <min/stack_vector.h>
#include <min/utility.h>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace min
{

template <typename T>
class vec2
{
  private:
    T _x;
    T _y;

    inline void float_assert()
    {
        static_assert(std::is_floating_point<T>::value, "vec2<T> must be a floating point type");
    }

  public:
    vec2() : _x(0.0), _y(0.0) { float_assert(); }
    vec2(const T x, const T y) : _x(x), _y(y) { float_assert(); }
    inline T x() const
    {
        return _x;
    }
    inline T y() const
    {
        return _y;
    }
    inline void x(const T x)
    {
        _x = x;
    }
    inline void y(const T y)
    {
        _y = y;
    }
    inline vec2<T> &set_all(const T v)
    {
        _x = v;
        _y = v;

        return *this;
    }
    inline vec2<T> &abs()
    {
        _x = std::abs(_x);
        _y = std::abs(_y);

        return *this;
    }
    inline bool any_zero_outside(const vec2<T> &p, const vec2<T> &min, const vec2<T> &max) const
    {
        // If p is zero and this is outside min and max return true else false
        if (std::abs(p.x()) <= var<T>::TOL_RAY)
        {
            if (_x < min.x() || _x > max.x())
                return true;
        }
        else if (std::abs(p.y()) <= var<T>::TOL_RAY)
        {
            if (_y < min.y() || _y > max.y())
                return true;
        }

        return false;
    }
    inline constexpr static coord_sys<T, min::vec2> axes()
    {
        return coord_sys<T, min::vec2>(vec2<T>(1.0, 0.0), vec2<T>(0.0, 1.0));
    }
    inline vec2<T> &clamp(const vec2<T> &min, const vec2<T> &max)
    {
        min::clamp(_x, min.x(), max.x());
        min::clamp(_y, min.y(), max.y());

        return *this;
    }
    inline vec2<T> clamp_direction(const vec2<T> &min, const vec2<T> &max)
    {
        T x = min::clamp_direction(_x, min.x(), max.x());
        T y = min::clamp_direction(_y, min.y(), max.y());

        return vec2<T>(x, y);
    }
    inline T cross(const vec2<T> &A) const
    {
        return _x * A.y() - _y * A.x();
    }
    inline T dot(const vec2<T> &A) const
    {
        return _x * A.x() + _y * A.y();
    }
    inline static std::pair<vec2<T>, vec2<T>> extents(const std::vector<vec2<T>> &verts)
    {
        // Find the greatest extents in the collection of vec2<T>
        const auto size = verts.size();
        if (size > 1)
        {
            const vec2<T> &first = verts[0];
            T minx = first.x();
            T miny = first.y();
            T maxx = first.x();
            T maxy = first.y();
            for (size_t i = 1; i < size; i++)
            {
                const vec2<T> &v = verts[i];
                extend<T>(v.x(), minx, maxx);
                extend<T>(v.y(), miny, maxy);
            }

            // Return the greatest extents
            return std::make_pair(vec2<T>(minx, miny), vec2<T>(maxx, maxy));
        }

        return std::make_pair(vec2<T>(), vec2<T>());
    }
    // Creating N^2 grid from min corner to max corner
    // Grid iterates in the Y than X dimension to reflect tree data layout
    // ^     /-----/-----/
    // |    /  1  /  3  /
    // c   /-----/-----/
    // o  /  0  /  2  /
    // l /-----/-----/
    //   row --->
    template <typename C>
    inline static void grid(C &out, const vec2<T> &min, const vec2<T> &max, const size_t scale)
    {
        // Clear the vector
        out.clear();
        out.reserve(out.size() + scale * scale);

        // Calculate the grid dimensions
        const vec2<T> size = max - min;
        const vec2<T> extent = size / scale;
        const T dx = extent.x();
        const T dy = extent.y();

        // Start at the bottom left corner
        vec2<T> cell;

        // Across the X dim
        for (T x = min.x(); x < max.x(); x += dx)
        {
            // Set the cell x value
            cell.x(x);

            // Across the Y dim
            for (T y = min.y(); y < max.y(); y += dy)
            {
                // Set the cell y value
                cell.y(y);
                out.emplace_back(cell, cell + extent);
            }
        }
    }
    template <typename C>
    inline static void grid_center(C &out, const vec2<T> &min, const vec2<T> &max, const size_t scale, const T size)
    {
        // Clear out vector
        out.clear();
        out.reserve(out.size() + scale * scale);

        // Calculate the grid dimensions
        const vec2<T> extent = (max - min) / scale;
        const vec2<T> half_extent = extent * 0.5;
        const T dx = extent.x();
        const T dy = extent.y();

        // Start at the bottom left corner
        vec2<T> cell;

        // Across the X dim
        for (T x = min.x(); x < max.x(); x += dx)
        {
            // Set the cell x value
            cell.x(x);

            // Across the Y dim
            for (T y = min.y(); y < max.y(); y += dy)
            {
                // Set the cell y value
                cell.y(y);
                out.emplace_back(cell + half_extent, size);
            }
        }
    }
    inline static min::bi<size_t> grid_index(const vec2<T> &min, const vec2<T> &extent, const vec2<T> &point)
    {
        // Calculate the grid dimensions
        const T ex = extent.x();
        const T ey = extent.y();

        // Get the row / col of cell
        const size_t col = (point.x() - min.x()) / ex;
        const size_t row = (point.y() - min.y()) / ey;

        // Return the row / col of cell
        return min::bi<size_t>(col, row);
    }
    inline static min::bi<size_t> grid_index(const size_t index, const size_t scale)
    {
        const size_t col = index / scale;
        const size_t row = index - (col * scale);

        // return tuple
        return min::bi<size_t>(col, row);
    }
    inline static size_t grid_key(const vec2<T> &min, const vec2<T> &extent, const size_t scale, const vec2<T> &point)
    {
        // Calculate the cell location
        const min::bi<size_t> index = grid_index(min, extent, point);

        // Get the row / col of cell
        const size_t col = index.x();
        const size_t row = index.y();

        // Return the grid index key for accessing cell
        return col * scale + row;
    }
    inline static size_t grid_key(const min::bi<size_t> &index, const size_t scale)
    {
        // Get the row / col of cell
        const size_t col = index.x();
        const size_t row = index.y();

        // Return the grid index key for accessing cell
        return col * scale + row;
    }
    inline static constexpr size_t over_size()
    {
        return 9;
    }
    inline static auto grid_overlap(const vec2<T> &min, const vec2<T> &extent, const size_t scale, const vec2<T> &b_min, const vec2<T> &b_max)
    {
        min::stack_vector<size_t, vec2<T>::over_size()> out;

        // Calculate the grid dimensions
        const T dx = extent.x();
        const T dy = extent.y();

        // Calculate the center cell
        const vec2<T> center = (b_min + b_max) * 0.5;

        // Center cell indices
        const min::bi<size_t> index = vec2<T>::grid_index(min, extent, center);
        const size_t x = index.x();
        const size_t y = index.y();

        // Bounds of the center cell
        const T minx = min.x() + dx * x;
        const T miny = min.y() + dy * y;
        const T maxx = minx + dx;
        const T maxy = miny + dy;

        // Calculate the neighboring indices
        const int nx = x - 1;
        const int px = x + 1;
        const int ny = y - 1;
        const int py = y + 1;

        // Calculate whether neighbors are outside the main grid
        const bool nxg = nx >= 0;
        const bool pxg = px < (long)scale;
        const bool nyg = ny >= 0;
        const bool pyg = py < (long)scale;
        const bool lx = b_min.x() < minx;
        const bool ly = b_min.y() < miny;
        const bool gx = b_max.x() >= maxx;
        const bool gy = b_max.y() >= maxy;
        const bool tny = ly && nyg;
        const bool tgy = gy && pyg;

        // -X
        if (lx && nxg)
        {
            if (tny)
                out.push_back(nx * scale + ny); // -Y
            out.push_back(nx * scale + y);      // Y
            if (tgy)
                out.push_back(nx * scale + py); // +Y
        }

        // X
        if (tny)
            out.push_back(x * scale + ny); // -Y
        out.push_back(x * scale + y);      // Y
        if (tgy)
            out.push_back(x * scale + py); // +Y

        // +X
        if (gx && pxg)
        {
            if (tny)
                out.push_back(px * scale + ny); // -Y
            out.push_back(px * scale + y);      // Y
            if (tgy)
                out.push_back(px * scale + py); // + Y
        }

        return out;
    }
    inline static std::tuple<T, T, T, T, int_fast8_t, int_fast8_t> grid_ray(const vec2<T> &min, const vec2<T> &extent, const vec2<T> &origin, const vec2<T> &dir, const vec2<T> &inv_dir)
    {
        // Get the grid dimensions
        const T ex = extent.x();
        const T ey = extent.y();

        // Get the origin starting points
        const T x = origin.x();
        const T y = origin.y();

        // Calculate distance to left of ray origin
        const T minx = ex * std::floor((x + min.x()) / ex) - min.x();

        // Calculate distance to right of ray origin
        const T maxx = minx + ex;

        // Test for ray parallel to X axis
        T tx = std::numeric_limits<T>::max();
        T dtx = std::numeric_limits<T>::max();
        int_fast8_t drx = 1;
        if (std::abs(dir.x()) >= var<T>::TOL_RAY)
        {
            // Choose distance based on ray direction
            if (dir.x() < 0.0)
            {
                drx = -1;
                tx = (x - minx) * std::abs(inv_dir.x());
            }
            else
            {
                tx = (maxx - x) * std::abs(inv_dir.x());
            }

            // Compute the length of the cell along the ray
            dtx = ex * std::abs(inv_dir.x());
        }

        // Calculate distance to below ray origin
        const T miny = ey * std::floor((y + min.y()) / ey) - min.y();

        // Calculate distance to above ray origin
        const T maxy = miny + ey;

        // Test for ray parallel to Y axis
        T ty = std::numeric_limits<T>::max();
        T dty = std::numeric_limits<T>::max();
        int_fast8_t dry = 1;
        if (std::abs(dir.y()) >= var<T>::TOL_RAY)
        {
            // Choose distance based on ray direction
            if (dir.y() < 0.0)
            {
                dry = -1;
                ty = (y - miny) * std::abs(inv_dir.y());
            }
            else
            {
                ty = (maxy - y) * std::abs(inv_dir.y());
            }

            // Compute the length of the cell along the ray
            dty = ey * std::abs(inv_dir.y());
        }

        // return the ray tuple
        return std::make_tuple(tx, dtx, ty, dty, drx, dry);
    }
    inline static size_t grid_ray_next(min::bi<size_t> &index, std::tuple<T, T, T, T, int_fast8_t, int_fast8_t> &grid_ray, bool &flag, const size_t scale)
    {
        // Get the cell row / col
        size_t &col = index.x_ref();
        size_t &row = index.y_ref();

        // X
        T &tx = std::get<0>(grid_ray);
        const T &dtx = std::get<1>(grid_ray);

        // Y
        T &ty = std::get<2>(grid_ray);
        const T &dty = std::get<3>(grid_ray);

        // Dir
        const int_fast8_t &drx = std::get<4>(grid_ray);
        const int_fast8_t &dry = std::get<5>(grid_ray);

        // Should we move along the x, y, or z axis? Guarantee a valid return value.
        if (tx <= ty)
        {
            if (!(col == 0 && drx == -1) && !(col == scale - 1 && drx == 1))
            {
                // Increment column == choose x
                col += drx;
                tx += dtx;
            }
            else
            {
                flag = true;
            }
        }
        else
        {
            if (!(row == 0 && dry == -1) && !(row == scale - 1 && dry == 1))
            {
                // Increment row == choose y
                row += dry;
                ty += dty;
            }
            else
            {
                flag = true;
            }
        }

        // Return the grid index key for accessing cell
        return col * scale + row;
    }
    template <typename F>
    inline static void grid_range(const vec2<T> &min, const vec2<T> &extent, const size_t scale,
                                  const vec2<T> &over_min, const vec2<T> &over_max,
                                  const F &f)
    {
        // Assumes over_min and over_max are clamped to world edges!!
        // Get the key of min and max points for overlap
        const min::bi<size_t> i_min = vec2<T>::grid_index(min, extent, over_min);
        const min::bi<size_t> i_max = vec2<T>::grid_index(min, extent, over_max);

        // Get all cells in between points and get overlapping shapes
        for (size_t i = i_min.x(); i <= i_max.x(); i++)
        {
            for (size_t j = i_min.y(); j <= i_max.y(); j++)
            {
                // Get the key for this index
                const size_t key = vec2<T>::grid_key(min::bi<size_t>(i, j), scale);

                // Callback function on f
                f(key);
            }
        }
    }
    inline bool inside(const vec2<T> &min, const vec2<T> &max) const
    {
        // Return true if this vector is inside the min and max vector range
        return (_x > min.x() + var<T>::TOL_REL && _x < max.x() - var<T>::TOL_REL && _y > min.y() + var<T>::TOL_REL && _y < max.y() - var<T>::TOL_REL);
    }
    inline vec2<T> inverse() const
    {
        const T x = safe_inverse<T>(_x);
        const T y = safe_inverse<T>(_y);

        // return inverse
        return vec2<T>(x, y);
    }
    inline static vec2<T> lerp(const vec2<T> &v0, const vec2<T> &v1, T t)
    {
        return (v0 + (v1 - v0) * (t));
    }
    inline static vec2<T> interpolate(const vec2<T> &v0, const vec2<T> &v1, T t)
    {
        return lerp(v0, v1, t);
    }
    inline T magnitude() const
    {
        return std::sqrt(_x * _x + _y * _y);
    }
    inline T max() const
    {
        return std::max(_x, _y);
    }
    inline T min() const
    {
        return std::min(_x, _y);
    }
    inline static std::pair<vec2<T>, vec2<T>> most_separating(const std::vector<vec2<T>> &verts)
    {
        // Find the two most separating points in the collection of vec2<T>
        const auto size = verts.size();
        if (size > 1)
        {
            size_t minx, maxx, miny, maxy, min, max;
            minx = maxx = miny = maxy = min = max = 0;
            for (size_t i = 0; i < size; i++)
            {
                const vec2<T> &v = verts[i];
                if (v.x() > verts[maxx].x())
                    maxx = i;
                if (v.x() < verts[minx].x())
                    minx = i;
                if (v.y() > verts[maxy].y())
                    maxy = i;
                if (v.y() < verts[miny].y())
                    miny = i;
            }

            // Calculate span of the extents
            const vec2<T> dx = verts[maxx] - verts[minx];
            const vec2<T> dy = verts[maxy] - verts[miny];

            // Calculated the squared euclidean distance
            const T dx2 = dx.dot(dx);
            const T dy2 = dy.dot(dy);

            // Calculate the max extents
            min = minx;
            max = maxx;
            if (dy2 > dx2)
            {
                min = miny;
                max = maxy;
            }

            // Return the two most separating verts
            return std::make_pair(verts[min], verts[max]);
        }

        return std::make_pair(vec2<T>(), vec2<T>());
    }
    inline static vec2<T> normal(const vec2<T> &a, const vec2<T> &b, const vec2<T> &c)
    {
        // Computes normal vector to two points, third argument is ignored
        // Normal is not normalized
        return (b - a).orthogonal();
    }
    inline vec2<T> &normalize()
    {
        const T mag = magnitude();
        if (std::abs(mag) > var<T>::TOL_ZERO)
        {
            T inv_mag = 1.0 / mag;
            _x *= inv_mag;
            _y *= inv_mag;
        }
        else
        {
            _x = 0.0;
            _y = 0.0;
        }

        return *this;
    }
    inline vec2<T> &normalize_unsafe()
    {
        const T inv_mag = 1.0 / magnitude();
        _x *= inv_mag;
        _y *= inv_mag;

        return *this;
    }
    inline vec2<T> &normalize_safe(const vec2<T> &safe)
    {
        const T mag = magnitude();
        if (std::abs(mag) > var<T>::TOL_ZERO)
        {
            T inv_mag = 1.0 / mag;
            _x *= inv_mag;
            _y *= inv_mag;
        }
        else
        {
            _x = safe.x();
            _y = safe.y();
        }

        return *this;
    }
    inline static void order(vec2<T> &min, vec2<T> &max)
    {
        // Order the components into min and max
        if (min._x > max._x)
        {
            swap(min._x, max._x);
        }
        if (min._y > max._y)
        {
            swap(min._y, max._y);
        }
    }
    inline vec2<T> orthogonal() const
    {
        // Compute the orthogonal vector to A
        return vec2<T>(_y, -_x);
    }
    inline vec2<T> project_point(const coord_sys<T, min::vec2> &axis, const vec2<T> &extent)
    {
        // Project this onto local x axis
        T x = dot(axis.x());

        // Clamp x onto the box half extent, else x
        min::clamp<T>(x, -extent.x(), extent.x());

        // Project this onto local y axis
        T y = dot(axis.y());

        // Clamp y onto the box half extent, else y
        min::clamp<T>(y, -extent.y(), extent.y());

        // Compute the point along this axis
        return (axis.x() * x) + (axis.y() * y);
    }
    inline T project_length(const coord_sys<T, min::vec2> &axis, const vec2<T> &extent)
    {
        // Project this onto local x axis
        const T x = dot(axis.x());

        // Clamp x onto the box half extent, else zero
        const T dx = clamp_value<T>(x, -extent.x(), x + extent.x(), extent.x(), x - extent.x());

        // Project this onto local y axis
        const T y = dot(axis.y());

        // Clamp y onto the box half extent, else 0
        const T dy = clamp_value<T>(y, -extent.y(), y + extent.y(), extent.y(), y - extent.y());

        // Compute the square distance from this point
        return (dx * dx) + (dy * dy);
    }
    static inline bool project_sat(const coord_sys<T, min::vec2> &axis1, const vec2<T> &center1, const vec2<T> &extent1, const coord_sys<T, min::vec2> &axis2, const vec2<T> &center2, const vec2<T> &extent2)
    {
        // This performs the separating axis theorem for checking oobb-oobb intersections
        // For every axis test (C2-C1).dot(L) > (a.get_extent() + b.get_extent()).dot(L)
        // This means testing the difference between box centers, C1 & C2, along the separating axis L
        // With the addition of box extents along this same axis L
        // For 2D, there are 4 axes that need to be tested against...
        // 2*2 = 4 local box axes

        // Rotation matrix expressing A2 in A1's coordinate frame
        const T x1x2 = axis1.x().dot(axis2.x());
        const T x1y2 = axis1.x().dot(axis2.y());
        const T y1x2 = axis1.y().dot(axis2.x());
        const T y1y2 = axis1.y().dot(axis2.y());

        const T abs_x1x2 = std::abs(x1x2) + var<T>::TOL_REL;
        const T abs_x1y2 = std::abs(x1y2) + var<T>::TOL_REL;
        const T abs_y1x2 = std::abs(y1x2) + var<T>::TOL_REL;
        const T abs_y1y2 = std::abs(y1y2) + var<T>::TOL_REL;

        // Bring translation into A1's coordinate frame
        const vec2<T> d = center2 - center1;
        const vec2<T> t = vec2<T>(d.dot(axis1.x()), d.dot(axis1.y()));

        // Test L = A1.x(); d1 and d2 is the length of extents along L
        T dL1 = extent1.x();
        T dL2 = extent2.x() * abs_x1x2 + extent2.y() * abs_x1y2;
        if (std::abs(t.x()) > dL1 + dL2)
            return false;

        // Test L = A1.y(); d1 and d2 is the length of extents along L
        dL1 = extent1.y();
        dL2 = extent2.x() * abs_y1x2 + extent2.y() * abs_y1y2;
        if (std::abs(t.y()) > dL1 + dL2)
            return false;

        // Test L = A2.x(); d1 and d2 is the length of extents along L
        dL1 = extent1.x() * abs_x1x2 + extent1.y() * abs_y1x2;
        dL2 = extent2.x();
        if (std::abs(t.x() * x1x2 + t.y() * y1x2) > dL1 + dL2)
            return false;

        // Test L = A2.y(); d1 and d2 is the length of extents along L
        dL1 = extent1.x() * abs_x1y2 + extent1.y() * abs_y1y2;
        dL2 = extent2.y();
        if (std::abs(t.x() * x1y2 + t.y() * y1y2) > dL1 + dL2)
            return false;

        return true;
    }

    static inline std::pair<vec2<T>, T> project_sat_penetration(
        const coord_sys<T, min::vec2> &axis1, const vec2<T> &center1, const vec2<T> &extent1,
        const coord_sys<T, min::vec2> &axis2, const vec2<T> &center2, const vec2<T> &extent2, const T tolerance)
    {
        // This performs the separating axis theorem for checking oobb-oobb intersection penetration
        // For every axis, penetration = (a.get_extent() + b.get_extent()).dot(L) - (C2-C1).dot(L)
        // This means testing the difference between box centers, C1 & C2, along the separating axis L
        // With the addition of box extents along this same axis L
        // For 2D, there are 4 axes that need to be computed...
        // 2*2 = 4 local box axes

        // Rotation matrix expressing A2 in A1's coordinate frame
        const T x1x2 = axis1.x().dot(axis2.x());
        const T x1y2 = axis1.x().dot(axis2.y());
        const T y1x2 = axis1.y().dot(axis2.x());
        const T y1y2 = axis1.y().dot(axis2.y());

        const T abs_x1x2 = std::abs(x1x2) + tolerance;
        const T abs_x1y2 = std::abs(x1y2) + tolerance;
        const T abs_y1x2 = std::abs(y1x2) + tolerance;
        const T abs_y1y2 = std::abs(y1y2) + tolerance;

        // Bring translation into A1's coordinate frame
        const vec2<T> d = center2 - center1;
        const vec2<T> t = vec2<T>(d.dot(axis1.x()), d.dot(axis1.y()));

        // Store axis and penetration depths
        vec2<T> axes[4];
        T penetration[4];

        // Test L = A1.x(); d1 and d2 is the length of extents along L
        T dL1 = extent1.x();
        T dL2 = extent2.x() * abs_x1x2 + extent2.y() * abs_x1y2;
        axes[0] = axis1.x();
        penetration[0] = (dL1 + dL2) - std::abs(t.x());

        // Test L = A1.y(); d1 and d2 is the length of extents along L
        dL1 = extent1.y();
        dL2 = extent2.x() * abs_y1x2 + extent2.y() * abs_y1y2;
        axes[1] = axis1.y();
        penetration[1] = (dL1 + dL2) - std::abs(t.y());

        // Test L = A2.x(); d1 and d2 is the length of extents along L
        dL1 = extent1.x() * abs_x1x2 + extent1.y() * abs_y1x2;
        dL2 = extent2.x();
        axes[2] = axis2.x();
        penetration[2] = (dL1 + dL2) - std::abs(t.x() * x1x2 + t.y() * y1x2);

        // Test L = A2.y(); d1 and d2 is the length of extents along L
        dL1 = extent1.x() * abs_x1y2 + extent1.y() * abs_y1y2;
        dL2 = extent2.y();
        axes[3] = axis2.y();
        penetration[3] = (dL1 + dL2) - std::abs(t.x() * x1y2 + t.y() * y1y2);

        // normal default up vector return and zero penetration
        vec2<T> normal = vec2<T>::up();
        T overlap = 0.0;

        // Find the minimum, non-zero penetration index
        T min = std::numeric_limits<T>::max();
        int index = -1;
        for (int i = 0; i < 4; i++)
        {
            // Prune all parallel normal vectors and non-penetrating depths
            const T mag2 = axes[i].dot(axes[i]);
            if (mag2 > tolerance && penetration[i] > tolerance && penetration[i] < min)
            {
                min = penetration[i];
                index = i;
            }
        }

        // check if we found an intersection penetration
        if (index != -1)
        {
            // Calculate the sign of normal towards body1 and scale normal
            const vec2<T> sign = (center1 - center2).sign();
            normal = axes[index].abs() * sign;
            overlap = min;
        }

        // return normal vector and minimum penentration
        return std::make_pair(normal, overlap);
    }
    static inline std::pair<vec2<T>, T> project_sat_aligned_penetration(
        const vec2<T> &center1, const vec2<T> &extent1,
        const vec2<T> &center2, const vec2<T> &extent2, const T tolerance)
    {
        // This performs the separating axis theorem for checking aabb-aabb intersection penetration
        // For every axis, penetration = (a.get_extent() + b.get_extent()).dot(L) - (C2-C1).dot(L) on aligned axises
        // This means testing the difference between box centers, C1 & C2, along the separating axis L
        // With the addition of box extents along this same axis L
        // For 2D, there are 2 axes that need to be computed...
        // 2 local box axes

        const vec2<T> d = center1 - center2;
        const vec2<T> t = vec2<T>(d).abs();

        // Test L = A1.x() = A2.x(); d1 and d2 is the length of extents along L
        // Test L = A1.y() = A2.y(); d1 and d2 is the length of extents along L
        const vec2<T> dL = (extent1 + extent2 + tolerance) - t;

        // Store axis and penetration depths
        const vec2<T> axes[2] = {vec2<T>(1.0, 0.0), vec2<T>(0.0, 1.0)};
        const T penetration[2] = {dL.x(), dL.y()};

        // normal default up vector return and zero penetration
        vec2<T> normal = vec2<T>::up();
        T overlap = 0.0;

        T min = std::numeric_limits<T>::max();
        int index = -1;
        if (penetration[0] > tolerance)
        {
            // Find the minimum, non-zero penetration index
            if (penetration[1] > tolerance && penetration[1] < penetration[0])
            {
                min = penetration[1];
                index = 1;
            }
            else
            {
                min = penetration[0];
                index = 0;
            }
        }
        else if (penetration[1] > tolerance)
        {
            min = penetration[1];
            index = 1;
        }

        // check if we found an intersection penetration
        if (index != -1)
        {
            // Calculate the sign of normal towards body1 and scale normal
            const vec2<T> sign = d.sign();
            normal = axes[index] * sign;
            overlap = min;
        }

        // return normal vector and minimum penentration
        return std::make_pair(normal, overlap);
    }
    // Subdividing vector space into 2^2 spaces using binary key location codes for index (xy)
    // Most significant bit of (x - xmin)/(xmax - xmin), (y - ymin)/(ymax - ymin)
    // Yields the key location code if MSB 0 = -, MSB 1 = +
    // Example position 2 = 01 = left, up
    //
    //     /-----/-----/
    //    /  1  /  3  /
    //   /-----/-----/
    //  /  0  /  2  /
    // /-----/-----/
    inline static vec2<T> ratio(const vec2<T> &min, const vec2<T> &max, const vec2<T> &point)
    {
        const T xr = (point.x() - min.x()) / (max.x() - min.x());
        const T yr = (point.y() - min.y()) / (max.y() - min.y());

        return vec2<T>(xr, yr);
    }
    inline vec2<T> sign() const
    {
        // Get the sign of the vector
        const T x = sgn<T>(_x);
        const T y = sgn<T>(_y);

        return vec2<T>(x, y);
    }
    inline uint_fast8_t subdivide_key(const T middle)
    {
        T x = _x;
        T y = _y;

        // Get the x portion of key
        uint_fast8_t key = 0;
        if (x >= middle)
        {
            // Set the least significant bit
            key |= 0x1;
            x -= middle;
            _x = x;
        }

        // Get the y portion of key
        key <<= 1;
        if (y >= middle)
        {
            // Set the least significant bit
            key |= 0x1;
            y -= middle;
            _y = y;
        }

        return key;
    }
    inline uint_fast8_t subdivide_key(const vec2<T> &center) const
    {
        // Get the x portion of key
        uint_fast8_t key = 0;
        if (_x >= center.x())
        {
            // Set the least significant bit
            key |= 0x1;
        }

        // Get the y portion of key
        key <<= 1;
        if (_y >= center.y())
        {
            // Set the least significant bit
            key |= 0x1;
        }

        return key;
    }
    inline static constexpr size_t sub_size()
    {
        return 4;
    }
    template <typename C>
    inline static auto subdivide(C &out, const vec2<T> &min, const vec2<T> &max)
    {
        // Clear out vector
        out.clear();
        out.reserve(out.size() + vec2<T>::sub_size());

        // Center of the vector space
        const vec2<T> c = (max + min) * 0.5;

        // Octant 0
        const vec2<T> min0 = vec2<T>(min.x(), min.y());
        const vec2<T> max0 = vec2<T>(c.x(), c.y());

        // Octant 1
        const vec2<T> min1 = vec2<T>(min.x(), c.y());
        const vec2<T> max1 = vec2<T>(c.x(), max.y());

        // Octant 2
        const vec2<T> min2 = vec2<T>(c.x(), min.y());
        const vec2<T> max2 = vec2<T>(max.x(), c.y());

        // Octant 3
        const vec2<T> min3 = vec2<T>(c.x(), c.y());
        const vec2<T> max3 = vec2<T>(max.x(), max.y());

        // Add sub spaces to out vector
        out.emplace_back(min0, max0);
        out.emplace_back(min1, max1);
        out.emplace_back(min2, max2);
        out.emplace_back(min3, max3);
    }
    template <typename C>
    inline static auto subdivide_center(C &out, const vec2<T> &min, const vec2<T> &max)
    {
        // Clear out vector
        out.clear();
        out.reserve(out.size() + vec2<T>::sub_size());

        // quarter extent of vector space
        const vec2<T> h = ((max - min) * 0.25) + var<T>::TOL_REL;

        // Center of the vector space
        const vec2<T> c = (max + min) * 0.5;

        // Positions
        const T cx_hx = c.x() - h.x();
        const T cy_hy = c.y() - h.y();

        const T cxhx = c.x() + h.x();
        const T cyhy = c.y() + h.y();

        // Octant 0
        const vec2<T> c0 = vec2<T>(cx_hx, cy_hy);

        // Octant 1
        const vec2<T> c1 = vec2<T>(cx_hx, cyhy);

        // Octant 2
        const vec2<T> c2 = vec2<T>(cxhx, cy_hy);

        // Octant 3
        const vec2<T> c3 = vec2<T>(cxhx, cyhy);

        // Calculate the square distance between center and extent
        const T radius2 = h.dot(h);

        // Calculate the radius
        const T radius = std::sqrt(radius2);

        // Add sub spaces to out vector
        out.emplace_back(c0, radius);
        out.emplace_back(c1, radius);
        out.emplace_back(c2, radius);
        out.emplace_back(c3, radius);
    }
    // Plane n·x - c = 0
    // Ray x = P + td
    // If intersecting n · (P + td) - c = 0; x >= 0.0
    // n · P + n · td - c = 0
    // t = (c - n · P) / (n · d)
    // Each axis is axis aligned so we can simplify to, where nx = ny = 1
    // tx(y-axis) = (cx - nx · Px) / (nx · dx)
    // ty(x-axis) = (cy - ny · Py) / (ny · dy)
    inline static auto subdivide_ray(const vec2<T> &min, const vec2<T> &max, const vec2<T> &origin, const vec2<T> &dir, const vec2<T> &inv_dir)
    {
        min::stack_vector<uint_fast8_t, vec2<T>::sub_size()> out;

        // Ray can't intersect the slab if ray is parallel to axis
        if (origin.any_zero_outside(dir, min, max))
        {
            return out;
        }

        // Center of the vector space
        const vec2<T> center = (max + min) * 0.5;

        // Calculate ray intersections among all axes
        const vec2<T> t = (center - origin) * inv_dir;

        // X intersection types
        const bool x_front = t.y() >= 0.0;
        const T px = origin.x() + (t.y() * dir.x());
        const bool xmin_out = x_front && (px < center.x());
        const bool xmax_out = x_front && (px >= center.x());
        const bool xmin = px >= min.x();
        const bool xmax = px <= max.x();

        // Y intersection types
        const bool y_front = t.x() >= 0.0;
        const T py = origin.y() + (t.x() * dir.y());
        const bool ymin_out = y_front && (py < center.y());
        const bool ymax_out = y_front && (py >= center.y());
        const bool ymin = py >= min.y();
        const bool ymax = py <= max.y();

        // Calculate first axis intersection
        const bool xly = x_front && (!y_front || (t.y() < t.x()));
        const bool ylx = y_front && (!x_front || (t.x() < t.y()));

        // Prefer point inside versus first plane intersection
        const bool x_inside = xmin && xmax;
        const bool y_inside = ymin && ymax;

        // Special case, only one cell is intersected in this case
        const bool all_outside = !x_inside && !y_inside;
        if (all_outside)
        {
            if (origin.within(min, max))
            {
                // Get the key from octant
                const uint_fast8_t key = origin.subdivide_key(center);
                out.push_back(key);
            }
            else
            {
                // Calculate the intersection with near and far plane
                vec2<T> near = (min - origin) * inv_dir;
                vec2<T> far = (max - origin) * inv_dir;

                // Order to get the nearer intersection points
                vec2<T>::order(near, far);

                // Get the farthest entry into the slab
                const T tmin = near.max();

                // Get the nearest exit from a slab
                const T tmax = far.min();

                // If tmin are >= 0.0 and nearest exit > farthest entry we have an intersection
                if (tmax >= tmin && tmin >= 0.0)
                {
                    // Find the octant the the origin is in
                    const vec2<T> point = (origin + (dir * tmin));

                    // Get the key from octant
                    const uint_fast8_t key = point.subdivide_key(center);
                    out.push_back(key);
                }
            }

            // Early return
            return out;
        }

        const bool x_ = xly && x_inside;
        const bool y_ = ylx && y_inside;
        if (x_)
        {
            if (xmin_out)
            {
                if (dir.y() < 0.0)
                {
                    if (xmin)
                    {
                        out.push_back(1);
                    }
                    out.push_back(0);
                    if (ymin)
                    {
                        out.push_back(2);
                    }
                }
                else
                {
                    if (xmin)
                    {
                        out.push_back(0);
                    }
                    out.push_back(1);
                    if (ymax)
                    {
                        out.push_back(3);
                    }
                }
            }
            else if (xmax_out)
            {
                if (dir.y() < 0.0)
                {
                    if (xmax)
                    {
                        out.push_back(3);
                    }
                    out.push_back(2);
                    if (ymin)
                    {
                        out.push_back(0);
                    }
                }
                else
                {
                    if (xmax)
                    {
                        out.push_back(2);
                    }
                    out.push_back(3);
                    if (ymax)
                    {
                        out.push_back(1);
                    }
                }
            }
        }
        else if (y_)
        {
            if (ymin_out)
            {
                if (dir.x() < 0.0)
                {
                    if (ymin)
                    {
                        out.push_back(2);
                    }
                    out.push_back(0);
                    if (xmin)
                    {
                        out.push_back(1);
                    }
                }
                else
                {
                    if (ymin)
                    {
                        out.push_back(0);
                    }
                    out.push_back(2);
                    if (xmax)
                    {
                        out.push_back(3);
                    }
                }
            }
            else if (ymax_out)
            {
                if (dir.x() < 0.0)
                {
                    if (ymax)
                    {
                        out.push_back(3);
                    }
                    out.push_back(1);
                    if (xmin)
                    {
                        out.push_back(0);
                    }
                }
                else
                {
                    if (ymax)
                    {
                        out.push_back(1);
                    }
                    out.push_back(3);
                    if (xmax)
                    {
                        out.push_back(2);
                    }
                }
            }
        }
        else
        {
            if (dir.x() <= 0.0 && dir.y() <= 0.0)
            {
                const uint_fast8_t temp[vec2<T>::sub_size()] = {3, 2, 1, 0};
                out = temp;
            }
            else if (dir.x() > 0.0 && dir.y() <= 0.0)
            {
                const uint_fast8_t temp[vec2<T>::sub_size()] = {1, 3, 0, 2};
                out = temp;
            }
            else if (dir.x() <= 0.0 && dir.y() > 0.0)
            {
                const uint_fast8_t temp[vec2<T>::sub_size()] = {2, 0, 3, 1};
                out = temp;
            }
            else if (dir.x() > 0.0 && dir.y() > 0.0)
            {
                const uint_fast8_t temp[vec2<T>::sub_size()] = {0, 1, 2, 3};
                out = temp;
            }
        }

        return out;
    }
    inline static auto subdivide_overlap(const vec2<T> &min, const vec2<T> &max, const vec2<T> &center)
    {
        min::stack_vector<uint_fast8_t, vec2<T>::sub_size()> out;

        const bool minx = min.x() <= center.x();
        const bool miny = min.y() <= center.y();
        const bool maxx = max.x() >= center.x();
        const bool maxy = max.y() >= center.y();

        // If overlapping 0-1 cells
        if (minx)
        {
            // If overlapping 0
            if (miny)
            {
                out.push_back(0);
                // If overlapping 1
                if (maxy)
                {
                    out.push_back(1);
                }
            }
            else
            {
                // Must be overlapping 1
                out.push_back(1);
            }
        }

        // If overlapping 2-3 cells
        if (maxx)
        {
            // If overlapping 2
            if (miny)
            {
                out.push_back(2);
                // If overlapping 3
                if (maxy)
                {
                    out.push_back(3);
                }
            }
            else
            {
                // Must be overlapping 3
                out.push_back(3);
            }
        }

        return out;
    }
    inline constexpr static T unit_length()
    {
        return var<T>::SQRT2;
    }
    inline constexpr static T inverse_unit_length()
    {
        return var<T>::INV_SQRT2;
    }
    inline constexpr static vec2<T> up()
    {
        return vec2<T>(0.0, 1.0);
    }
    inline bool within(const vec2<T> &min, const vec2<T> &max) const
    {
        // Return true if this vector is within the min and max vector range
        return (_x >= min.x() + var<T>::TOL_REL && _x <= max.x() - var<T>::TOL_REL && _y >= min.y() + var<T>::TOL_REL && _y <= max.y() - var<T>::TOL_REL);
    }
    inline vec2<T> &operator+=(const T a)
    {
        _x += a;
        _y += a;
        return *this;
    }
    inline vec2<T> &operator+=(const vec2<T> &A)
    {
        _x += A.x();
        _y += A.y();
        return *this;
    }
    inline vec2<T> &operator-=(const T a)
    {
        _x -= a;
        _y -= a;
        return *this;
    }
    inline vec2<T> &operator-=(const vec2<T> &A)
    {
        _x -= A.x();
        _y -= A.y();
        return *this;
    }
    inline vec2<T> &operator*=(const T a)
    {
        _x *= a;
        _y *= a;
        return *this;
    }
    inline vec2<T> &operator*=(const vec2<T> &A)
    {
        _x *= A.x();
        _y *= A.y();
        return *this;
    }
    inline vec2<T> &operator/=(const T a)
    {
        _x /= a;
        _y /= a;
        return *this;
    }
    inline vec2<T> &operator/=(const vec2<T> &A)
    {
        _x /= A.x();
        _y /= A.y();
        return *this;
    }
    inline vec2<T> operator+(const T a) const
    {
        vec2<T> temp = *this;
        temp += a;
        return temp;
    }
    inline vec2<T> operator+(const vec2<T> &A) const
    {
        vec2<T> temp = *this;
        temp += A;
        return temp;
    }
    inline vec2<T> operator-(const T a) const
    {
        vec2<T> temp = *this;
        temp -= a;
        return temp;
    }
    inline vec2<T> operator-(const vec2<T> &A) const
    {
        vec2<T> temp = *this;
        temp -= A;
        return temp;
    }
    inline vec2<T> operator*(const T a) const
    {
        vec2<T> temp = *this;
        temp *= a;
        return temp;
    }
    inline vec2<T> operator*(const vec2<T> &A) const
    {
        vec2<T> temp = *this;
        temp *= A;
        return temp;
    }
    inline vec2<T> operator/(const T a) const
    {
        vec2<T> temp = *this;
        temp /= a;
        return temp;
    }
    inline vec2<T> operator/(const vec2<T> &A) const
    {
        vec2<T> temp = *this;
        temp /= A;
        return temp;
    }
    inline bool operator>(const vec2<T> &A) const
    {
        return _x > A._x && _y > A._y;
    }
    inline bool operator>=(const vec2<T> &A) const
    {
        return _x >= A._x && _y >= A._y;
    }
    inline bool operator<(const vec2<T> &A) const
    {
        return _x < A._x && _y < A._y;
    }
    inline bool operator<=(const vec2<T> &A) const
    {
        return _x <= A._x && _y <= A._y;
    }
};
}

#endif
