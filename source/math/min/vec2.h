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
#ifndef __VECTOR2__
#define __VECTOR2__

// Forward declarations
namespace min
{
template <typename T, template <typename> class vec>
class coord_sys;
}

#include <algorithm>
#include <cmath>
#include <min/coord_sys.h>
#include <min/utility.h>
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

  public:
    vec2() : _x(0), _y(0) {}
    vec2(const T x, const T y) : _x(x), _y(y) {}
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
    inline bool any_zero() const
    {
        return (std::abs(_x) <= 1E-6) || (std::abs(_y) <= 1E-6);
    }
    inline constexpr static coord_sys<T, vec2> axes()
    {
        return coord_sys<T, vec2>(vec2<T>(1.0, 0.0), vec2<T>(0.0, 1.0));
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
    inline static std::vector<std::pair<vec2<T>, vec2<T>>> grid(const vec2<T> &min, const vec2<T> &max, const size_t scale)
    {
        // Create out vector
        std::vector<std::pair<vec2<T>, vec2<T>>> out;
        out.reserve(scale * scale);

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
                out.emplace_back(std::make_pair(cell, cell + extent));
            }
        }

        // return the compute grid
        return out;
    }
    inline static std::vector<std::pair<vec2<T>, T>> grid_center(const vec2<T> &min, const vec2<T> &max, const size_t scale, const T size)
    {
        // Create out vector
        std::vector<std::pair<vec2<T>, T>> out;
        out.reserve(scale * scale);

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
                out.emplace_back(std::make_pair(cell + half_extent, size));
            }
        }

        // return the compute grid
        return out;
    }
    inline static size_t grid_key(const vec2<T> &min, const vec2<T> &extent, const size_t scale, const vec2<T> &point)
    {
        // Calculate the grid dimensions
        const T dx = extent.x();
        const T dy = extent.y();

        const size_t row = (point.x() - min.x()) / dx;
        const size_t col = (point.y() - min.y()) / dy;

        return row * scale + col;
    }
    inline static std::vector<size_t> grid_overlap(const vec2<T> &min, const vec2<T> &extent, const size_t scale, const vec2<T> &b_min, const vec2<T> &b_max)
    {
        // Create the output vector
        std::vector<size_t> out;
        out.reserve(9);

        // Calculate the grid dimensions
        const T dx = extent.x();
        const T dy = extent.y();

        // Calculate the center cell
        const vec2<T> center = (b_min + b_max) * 0.5;

        // Test for early out
        if (center.x() < min.x() || center.y() < min.y())
        {
            return out;
        }

        // Center cell indices
        const size_t x = (center.x() - min.x()) / dx;
        const size_t y = (center.y() - min.y()) / dy;

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
        bool nxg = nx >= 0;
        bool pxg = px < (long)scale;
        bool nyg = ny >= 0;
        bool pyg = py < (long)scale;
        bool lx = b_min.x() < minx;
        bool ly = b_min.y() < miny;
        bool gx = b_max.x() >= maxx;
        bool gy = b_max.y() >= maxy;
        bool tny = ly && nyg;
        bool tgy = gy && pyg;

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
    inline bool inside(const vec2<T> &min, const vec2<T> &max) const
    {
        // Return true if this vector is inside the min and max vector range
        return (_x > min.x() && _x < max.x() && _y > min.y() && _y < max.y());
    }
    inline vec2<T> inverse() const
    {
        return vec2<T>(1.0 / _x, 1.0 / _y);
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
        T mag = 1.0 / magnitude();
        _x *= mag;
        _y *= mag;

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
    inline vec2<T> project_point(const coord_sys<T, vec2> &axis, const vec2<T> &extent)
    {
        // Project this onto local x axis
        T x = this->dot(axis.x());

        // Clamp x onto the box half extent, else x
        min::clamp<T>(x, -extent.x(), extent.x());

        // Project this onto local y axis
        T y = this->dot(axis.y());

        // Clamp y onto the box half extent, else y
        min::clamp<T>(y, -extent.y(), extent.y());

        // Compute the point along this axis
        return (axis.x() * x) + (axis.y() * y);
    }
    inline T project_length(const coord_sys<T, vec2> &axis, const vec2<T> &extent)
    {
        // Project this onto local x axis
        const T x = this->dot(axis.x());

        // Clamp x onto the box half extent, else zero
        const T dx = clamp_value<T>(x, -extent.x(), x + extent.x(), extent.x(), x - extent.x());

        // Project this onto local y axis
        const T y = this->dot(axis.y());

        // Clamp y onto the box half extent, else 0
        const T dy = clamp_value<T>(y, -extent.y(), y + extent.y(), extent.y(), y - extent.y());

        // Compute the square distance from this point
        return (dx * dx) + (dy * dy);
    }
    static inline bool project_sat(const coord_sys<T, vec2> &axis1, const vec2<T> &center1, const vec2<T> &extent1, const coord_sys<T, vec2> &axis2, const vec2<T> &center2, const vec2<T> &extent2)
    {
        // This performs the separating axis theorem for checking oobb-oobb intersections
        // For every axis test (C2-C1).dot(axis_n) > (a.get_extent() + b.get_extent()).dot(axis_n)
        // This means testing the difference between box centers, C1 & C2, along the separating axis L
        // With the addition of box extents along this same axis L
        // For 2D, there are 4 axes that need to be tested against...
        // 2*2 = 4 local box axes

        // Rotation matrix expressing A2 in A1's coordinate frame
        // Even though dot product is always > 0, if the dot product is zero, 0 > -0 may skew results
        const T xx = std::abs(axis1.x().dot(axis2.x()));
        const T xy = std::abs(axis1.x().dot(axis2.y()));
        const T yx = std::abs(axis1.y().dot(axis2.x()));
        const T yy = std::abs(axis1.y().dot(axis2.y()));

        // Bring translation into A1's coordinate frame
        const vec2<T> d = center2 - center1;
        const vec2<T> t = vec2<T>(d.dot(axis1.x()), d.dot(axis1.y())).abs();

        // Test L = A1.x(); d1 and d2 is the length of extents along L
        T dL1 = extent1.x();
        T dL2 = extent2.x() * xx + extent2.y() * xy;
        if (t.x() > dL1 + dL2)
            return false;

        // Test L = A1.y(); d1 and d2 is the length of extents along L
        dL1 = extent1.y();
        dL2 = extent2.x() * yx + extent2.y() * yy;
        if (t.y() > dL1 + dL2)
            return false;

        // Test L = A2.x(); d1 and d2 is the length of extents along L
        dL1 = extent1.x() * xx + extent1.y() * yx;
        dL2 = extent2.x();
        if (t.x() * xx + t.y() * yx > dL1 + dL2)
            return false;

        // Test L = A2.y(); d1 and d2 is the length of extents along L
        dL1 = extent1.x() * xy + extent1.y() * yy;
        dL2 = extent2.y();
        if (t.x() * xy + t.y() * yy > dL1 + dL2)
            return false;

        return true;
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
        T xr = (point.x() - min.x()) / (max.x() - min.x());
        T yr = (point.y() - min.y()) / (max.y() - min.y());

        return vec2<T>(xr, yr);
    }
    inline uint8_t subdivide_key(const T middle)
    {
        T x = _x;
        T y = _y;

        // Get the x portion of key
        uint8_t key = 0;
        if (x > middle)
        {
            // Set the least significant bit
            key |= 0x1;
            x -= middle;
            _x = x;
        }

        // Get the y portion of key
        key <<= 1;
        if (y > middle)
        {
            // Set the least significant bit
            key |= 0x1;
            y -= middle;
            _y = y;
        }

        return key;
    }
    inline static std::vector<std::pair<vec2<T>, vec2<T>>> subdivide(const vec2<T> &min, const vec2<T> &max)
    {
        std::vector<std::pair<vec2<T>, vec2<T>>> out;
        out.reserve(4);

        // Half extent of vector space
        vec2<T> h = (max - min) * 0.5;

        // Center of the vector space
        vec2<T> c = (max + min) * 0.5;

        // Positions
        T cx_hx = c.x() - h.x();
        T cy_hy = c.y() - h.y();

        T cxhx = c.x() + h.x();
        T cyhy = c.y() + h.y();

        // Octant 0
        vec2<T> min0 = vec2<T>(cx_hx, cy_hy);
        vec2<T> max0 = vec2<T>(c.x(), c.y());

        // Octant 1
        vec2<T> min1 = vec2<T>(cx_hx, c.y());
        vec2<T> max1 = vec2<T>(c.x(), cyhy);

        // Octant 2
        vec2<T> min2 = vec2<T>(c.x(), cy_hy);
        vec2<T> max2 = vec2<T>(cxhx, c.y());

        // Octant 3
        vec2<T> min3 = vec2<T>(c.x(), c.y());
        vec2<T> max3 = vec2<T>(cxhx, cyhy);

        // Add sub spaces to out vector
        out.push_back(std::make_pair(min0, max0));
        out.push_back(std::make_pair(min1, max1));
        out.push_back(std::make_pair(min2, max2));
        out.push_back(std::make_pair(min3, max3));

        return out;
    }
    inline static std::vector<std::pair<vec2<T>, T>> subdivide_center(const vec2<T> &min, const vec2<T> &max, const T size)
    {
        std::vector<std::pair<vec2<T>, T>> out;
        out.reserve(4);

        // quarter extent of vector space
        vec2<T> h = (max - min) * 0.25;

        // Center of the vector space
        vec2<T> c = (max + min) * 0.5;

        // Positions
        T cx_hx = c.x() - h.x();
        T cy_hy = c.y() - h.y();

        T cxhx = c.x() + h.x();
        T cyhy = c.y() + h.y();

        // Octant 0
        vec2<T> c0 = vec2<T>(cx_hx, cy_hy);

        // Octant 1
        vec2<T> c1 = vec2<T>(cx_hx, cyhy);

        // Octant 2
        vec2<T> c2 = vec2<T>(cxhx, cy_hy);

        // Octant 3
        vec2<T> c3 = vec2<T>(cxhx, cyhy);

        // Add sub spaces to out vector
        out.push_back(std::make_pair(c0, size));
        out.push_back(std::make_pair(c1, size));
        out.push_back(std::make_pair(c2, size));
        out.push_back(std::make_pair(c3, size));

        return out;
    }
    inline static std::vector<uint8_t> sub_overlap(const vec2<T> &min, const vec2<T> &max, const vec2<T> &center)
    {
        std::vector<uint8_t> out;
        out.reserve(4);

        bool minx = min.x() < center.x();
        bool maxx = max.x() > center.x();
        bool miny = min.y() < center.y();
        bool maxy = max.y() > center.y();

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
        return (_x >= min.x() && _x <= max.x() && _y >= min.y() && _y <= max.y());
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
