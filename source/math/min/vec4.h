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
#ifndef _MGL_VECTOR4_MGL_
#define _MGL_VECTOR4_MGL_

namespace min
{
template <typename T, template <typename> class vec>
class coord_sys;
}

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <min/coord_sys.h>
#include <min/stack_vector.h>
#include <min/tri.h>
#include <min/utility.h>
#include <utility>
#include <vector>

namespace min
{

template <typename T>
class vec4
{
  private:
    T _x;
    T _y;
    T _z;
    T _w;

    inline void float_assert()
    {
        static_assert(std::is_floating_point<T>::value, "vec4<T> must be a floating point type");
    }

  public:
    vec4() : _x(0.0), _y(0.0), _z(0.0), _w(1.0) { float_assert(); }
    vec4(const vec3<T> &v) : _x(v.x()), _y(v.y()), _z(v.z()), _w(1.0) { float_assert(); }
    vec4(const vec3<T> &v, T w) : _x(v.x()), _y(v.y()), _z(v.z()), _w(w) { float_assert(); }
    vec4(const T x, const T y, const T z, const T w) : _x(x), _y(y), _z(z), _w(w) { float_assert(); }
    inline T x() const
    {
        return _x;
    }
    inline T y() const
    {
        return _y;
    }
    inline T z() const
    {
        return _z;
    }
    inline T w() const
    {
        return _w;
    }
    inline void x(const T x)
    {
        _x = x;
    }
    inline void y(const T y)
    {
        _y = y;
    }
    inline void z(const T z)
    {
        _z = z;
    }
    inline void w(const T w)
    {
        _w = w;
    }
    inline vec4<T> &set_all(const T v)
    {
        _x = v;
        _y = v;
        _z = v;
        _w = 1.0;

        return *this;
    }
    inline vec3<T> xyz() const
    {
        vec3<T> temp(_x, _y, _z);
        return temp;
    }
    inline vec4<T> &abs()
    {
        _x = std::abs(_x);
        _y = std::abs(_y);
        _z = std::abs(_z);

        return *this;
    }
    inline bool any_zero_outside(const vec4<T> &p, const vec4<T> &min, const vec4<T> &max) const
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
        else if (std::abs(p.z()) <= var<T>::TOL_RAY)
        {
            if (_z < min.z() || _z > max.z())
                return true;
        }

        return false;
    }
    inline constexpr static coord_sys<T, min::vec4> axes()
    {
        return coord_sys<T, min::vec4>(vec4<T>(1.0, 0.0, 0.0, 1.0), vec4<T>(0.0, 1.0, 0.0, 1.0), vec4<T>(0.0, 0.0, 1.0, 1.0));
    }
    inline vec4<T> &clamp(const vec4<T> &min, const vec4<T> &max)
    {
        min::clamp(_x, min.x(), max.x());
        min::clamp(_y, min.y(), max.y());
        min::clamp(_z, min.z(), max.z());

        return *this;
    }
    inline vec4<T> clamp_direction(const vec4<T> &min, const vec4<T> &max)
    {
        T x = min::clamp_direction(_x, min.x(), max.x());
        T y = min::clamp_direction(_y, min.y(), max.y());
        T z = min::clamp_direction(_z, min.z(), max.z());

        return vec4<T>(x, y, z, 1.0);
    }
    inline vec4 cross(const vec4<T> &A) const
    {
        T x = _y * A.z() - _z * A.y();
        T y = _z * A.x() - _x * A.z();
        T z = _x * A.y() - _y * A.x();
        return vec4<T>(x, y, z, 1.0);
    }
    inline vec4<T> cross_x() const
    {
        return vec4<T>(0, _z, -_y, 1.0);
    }
    inline vec4<T> cross_y() const
    {
        return vec4<T>(-_z, 0, _x, 1.0);
    }
    inline vec4<T> cross_z() const
    {
        return vec4<T>(_y, -_x, 0, 1.0);
    }
    inline T dot(const vec4<T> &A) const
    {
        return _x * A.x() + _y * A.y() + _z * A.z();
    }
    inline T dot_x() const
    {
        return _x;
    }
    inline T dot_y() const
    {
        return _y;
    }
    inline T dot_z() const
    {
        return _z;
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
    inline static void grid(C &out, const vec4<T> &min, const vec4<T> &max, const size_t scale)
    {
        // Clear out vector
        out.clear();
        out.reserve(out.size() + scale * scale * scale);

        // Calculate the grid dimensions
        const vec4<T> size = max - min;
        const vec4<T> extent = size / scale;
        const T dx = extent.x();
        const T dy = extent.y();
        const T dz = extent.z();

        // Start at the bottom left corner
        vec4<T> cell;

        // Across the X dim
        for (T x = min.x(); x < max.x(); x += dx)
        {
            // Set the cell x value
            cell.x(x);

            // Across the Y dim
            for (T y = min.y(); y < max.y(); y += dy)
            {
                // Set the y value
                cell.y(y);

                // Across the Z dim
                for (T z = min.z(); z < max.z(); z += dz)
                {
                    // Set the cell z value
                    cell.z(z);
                    out.emplace_back(cell, cell + extent);
                }
            }
        }
    }
    template <typename C>
    inline static void grid_center(C &out, const vec4<T> &min, const vec4<T> &max, const size_t scale, const T size)
    {
        // Create out vector
        out.clear();
        out.reserve(out.size() + scale * scale * scale);

        // Calculate the grid dimensions
        const vec4<T> extent = (max - min) / scale;
        const vec4<T> half_extent = extent * 0.5;
        const T dx = extent.x();
        const T dy = extent.y();
        const T dz = extent.z();

        // Start at the bottom left corner
        vec4<T> cell;

        // Across the X dim
        for (T x = min.x(); x < max.x(); x += dx)
        {
            // Set the cell x value
            cell.x(x);

            // Across the Y dim
            for (T y = min.y(); y < max.y(); y += dy)
            {
                // Set the y value
                cell.y(y);

                // Across the Z dim
                for (T z = min.z(); z < max.z(); z += dz)
                {
                    // Set the cell z value
                    cell.z(z);
                    out.emplace_back(cell + half_extent, size);
                }
            }
        }
    }
    inline static min::tri<size_t> grid_index(const vec4<T> &min, const vec4<T> &extent, const vec4<T> &point)
    {
        // Calculate the grid dimensions
        const T ex = extent.x();
        const T ey = extent.y();
        const T ez = extent.z();

        const size_t col = (point.x() - min.x()) / ex;
        const size_t row = (point.y() - min.y()) / ey;
        const size_t zin = (point.z() - min.z()) / ez;

        // Return the row / col of cell
        return min::tri<size_t>(col, row, zin);
    }
    inline static min::tri<size_t> grid_index(const size_t index, const size_t scale)
    {
        // Precalculate the square scale
        const size_t scale2 = scale * scale;

        // Calculate row, col and height
        const size_t col = index / scale2;
        const size_t offset = index - col * scale2;
        const size_t row = offset / scale;
        const size_t zin = offset - (row * scale);

        // return tuple
        return min::tri<size_t>(col, row, zin);
    }
    inline static size_t grid_key(const vec4<T> &min, const vec4<T> &extent, const size_t scale, const vec4<T> &point)
    {
        // Calculate the cell location
        const min::tri<size_t> index = grid_index(min, extent, point);

        // Get the row / col of cell
        const size_t col = index.x();
        const size_t row = index.y();
        const size_t zin = index.z();

        // Return the grid index key for accessing cell
        return col * scale * scale + row * scale + zin;
    }
    inline static size_t grid_key(const min::tri<size_t> &index, const size_t scale)
    {
        // Get the row / col of cell
        const size_t col = index.x();
        const size_t row = index.y();
        const size_t zin = index.z();

        // Return the grid index key for accessing cell
        return col * scale * scale + row * scale + zin;
    }
    inline static constexpr size_t over_size()
    {
        return 27;
    }
    inline static auto grid_overlap(const vec4<T> &min, const vec4<T> &extent, const size_t scale, const vec4<T> &b_min, const vec4<T> &b_max)
    {
        min::stack_vector<size_t, vec4<T>::over_size()> out;

        // Calculate the grid dimensions
        const T dx = extent.x();
        const T dy = extent.y();
        const T dz = extent.z();

        // Calculate the center cell
        const vec4<T> center = (b_min + b_max) * 0.5;

        // Center cell indices
        const min::tri<size_t> index = vec4<T>::grid_index(min, extent, center);
        const size_t x = index.x();
        const size_t y = index.y();
        const size_t z = index.z();
        const size_t scale2 = scale * scale;

        // Bounds of the center cell
        const T minx = min.x() + dx * x;
        const T miny = min.y() + dy * y;
        const T minz = min.z() + dz * z;
        const T maxx = minx + dx;
        const T maxy = miny + dy;
        const T maxz = minz + dz;

        // Calculate the neighboring indices
        const int nx = x - 1;
        const int px = x + 1;
        const int ny = y - 1;
        const int py = y + 1;
        const int nz = z - 1;
        const int pz = z + 1;

        // Calculate whether neighbors are outside the main grid
        const bool nxg = nx >= 0;
        const bool nyg = ny >= 0;
        const bool nzg = nz >= 0;
        const bool pxg = px < (long)scale;
        const bool pyg = py < (long)scale;
        const bool pzg = pz < (long)scale;
        const bool lx = b_min.x() < minx;
        const bool ly = b_min.y() < miny;
        const bool lz = b_min.z() < minz;
        const bool gx = b_max.x() >= maxx;
        const bool gy = b_max.y() >= maxy;
        const bool gz = b_max.z() >= maxz;
        const bool tny = ly && nyg;
        const bool tgy = gy && pyg;
        const bool tnz = lz && nzg;
        const bool tgz = gz && pzg;

        // -X
        if (lx && nxg)
        {
            // -Y
            if (tny)
            {
                if (tnz)
                    out.push_back(nx * scale2 + ny * scale + nz); // -Z
                out.push_back(nx * scale2 + ny * scale + z);      // Z
                if (tgz)
                    out.push_back(nx * scale2 + ny * scale + pz); // +Z
            }

            // Y
            if (tnz)
                out.push_back(nx * scale2 + y * scale + nz); // -Z
            out.push_back(nx * scale2 + y * scale + z);      // Z
            if (tgz)
                out.push_back(nx * scale2 + y * scale + pz); // +Z

            // +Y
            if (tgy)
            {
                if (tnz)
                    out.push_back(nx * scale2 + py * scale + nz); // -Z
                out.push_back(nx * scale2 + py * scale + z);      // Z
                if (tgz)
                    out.push_back(nx * scale2 + py * scale + pz); // +Z
            }
        }

        // X -Y
        if (tny)
        {
            if (tnz)
                out.push_back(x * scale2 + ny * scale + nz); // -Z
            out.push_back(x * scale2 + ny * scale + z);      // Z
            if (tgz)
                out.push_back(x * scale2 + ny * scale + pz); // +Z
        }

        // X Y
        if (tnz)
            out.push_back(x * scale2 + y * scale + nz); // -Z
        out.push_back(x * scale2 + y * scale + z);      // Z
        if (tgz)
            out.push_back(x * scale2 + y * scale + pz); // +Z

        // X +Y
        if (tgy)
        {
            if (tnz)
                out.push_back(x * scale2 + py * scale + nz); // -Z
            out.push_back(x * scale2 + py * scale + z);      // Z
            if (tgz)
                out.push_back(x * scale2 + py * scale + pz); // +Z
        }

        // +X
        if (gx && pxg)
        {
            // -Y
            if (tny)
            {
                if (tnz)
                    out.push_back(px * scale2 + ny * scale + nz); // -Z
                out.push_back(px * scale2 + ny * scale + z);      // Z
                if (tgz)
                    out.push_back(px * scale2 + ny * scale + pz); // +Z
            }

            // Y
            if (tnz)
                out.push_back(px * scale2 + y * scale + nz); // -Z
            out.push_back(px * scale2 + y * scale + z);      // Z
            if (tgz)
                out.push_back(px * scale2 + y * scale + pz); // +Z

            // +Y
            if (tgy)
            {
                if (tnz)
                    out.push_back(px * scale2 + py * scale + nz); // -Z
                out.push_back(px * scale2 + py * scale + z);      // Z
                if (tgz)
                    out.push_back(px * scale2 + py * scale + pz); // +Z
            }
        }

        return out;
    }
    inline static std::tuple<T, T, T, T, T, T, int_fast8_t, int_fast8_t, int_fast8_t> grid_ray(const vec4<T> &min, const vec4<T> &extent, const vec4<T> &origin, const vec4<T> &dir, const vec4<T> &inv_dir)
    {
        // Get the grid dimensions
        const T ex = extent.x();
        const T ey = extent.y();
        const T ez = extent.z();

        // Get the origin starting points
        const T x = origin.x();
        const T y = origin.y();
        const T z = origin.z();

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

        // Calculate distance to below ray origin
        const T minz = ez * std::floor((z + min.z()) / ez) - min.z();

        // Calculate distance to above ray origin
        const T maxz = minz + ez;

        // Test for ray parallel to Z axis
        T tz = std::numeric_limits<T>::max();
        T dtz = std::numeric_limits<T>::max();
        int_fast8_t drz = 1;
        if (std::abs(dir.z()) >= var<T>::TOL_RAY)
        {
            // Choose distance based on ray direction
            if (dir.z() < 0.0)
            {
                drz = -1;
                tz = (z - minz) * std::abs(inv_dir.z());
            }
            else
            {
                tz = (maxz - z) * std::abs(inv_dir.z());
            }

            // Compute the length of the cell along the ray
            dtz = ez * std::abs(inv_dir.z());
        }

        // return the ray tuple
        return std::make_tuple(tx, dtx, ty, dty, tz, dtz, drx, dry, drz);
    }
    inline static size_t grid_ray_next(min::tri<size_t> &index, std::tuple<T, T, T, T, T, T, int_fast8_t, int_fast8_t, int_fast8_t> &grid_ray, bool &flag, const size_t scale)
    {
        // Get the cell row / col
        size_t &col = index.x_ref();
        size_t &row = index.y_ref();
        size_t &zin = index.z_ref();

        // X
        T &tx = std::get<0>(grid_ray);
        const T &dtx = std::get<1>(grid_ray);

        // Y
        T &ty = std::get<2>(grid_ray);
        const T &dty = std::get<3>(grid_ray);

        // Z
        T &tz = std::get<4>(grid_ray);
        const T &dtz = std::get<5>(grid_ray);

        // Dir
        const int_fast8_t &drx = std::get<6>(grid_ray);
        const int_fast8_t &dry = std::get<7>(grid_ray);
        const int_fast8_t &drz = std::get<8>(grid_ray);

        // Should we move along the x, y, or z axis? Guarantee a valid return value.
        if (tx <= ty && tx <= tz)
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
        else if (ty <= tx && ty <= tz)
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
        else
        {
            if (!(zin == 0 && drz == -1) && !(zin == scale - 1 && drz == 1))
            {
                // Increment column == choose x
                zin += drz;
                tz += dtz;
            }
            else
            {
                flag = true;
            }
        }

        // Return the grid index key for accessing cell
        return col * scale * scale + row * scale + zin;
    }
    template <typename F>
    inline static void grid_range(const vec4<T> &min, const vec4<T> &extent, const size_t scale,
                                  const vec4<T> &over_min, const vec4<T> &over_max,
                                  const F &f)
    {
        // Assumes over_min and over_max are clamped to world edges!!
        // Get the key of min and max points for overlap
        const min::tri<size_t> i_min = vec4<T>::grid_index(min, extent, over_min);
        const min::tri<size_t> i_max = vec4<T>::grid_index(min, extent, over_max);

        // Get all cells in between points and get overlapping shapes
        for (size_t i = i_min.x(); i <= i_max.x(); i++)
        {
            for (size_t j = i_min.y(); j <= i_max.y(); j++)
            {
                for (size_t k = i_min.z(); k <= i_max.z(); k++)
                {
                    // Get the key for this index
                    const size_t key = vec4<T>::grid_key(min::tri<size_t>(i, j, k), scale);

                    // Callback function on f
                    f(key);
                }
            }
        }
    }
    inline static std::pair<vec4<T>, vec4<T>> extents(const std::vector<vec4<T>> &verts)
    {
        // Find the greatest extents in the collection of vec4<T>
        const auto size = verts.size();
        if (size > 1)
        {
            const vec4<T> &first = verts[0];
            T minx = first.x();
            T miny = first.y();
            T minz = first.z();
            T maxx = first.x();
            T maxy = first.y();
            T maxz = first.z();
            for (size_t i = 1; i < size; i++)
            {
                const vec4<T> &v = verts[i];
                extend<T>(v.x(), minx, maxx);
                extend<T>(v.y(), miny, maxy);
                extend<T>(v.z(), minz, maxz);
            }

            // Return the greatest extents
            return std::make_pair(vec4<T>(minx, miny, minz, 1.0), vec4<T>(maxx, maxy, maxz, 1.0));
        }

        return std::make_pair(vec4<T>(), vec4<T>());
    }
    inline bool inside(const vec4<T> &min, const vec4<T> &max) const
    {
        // Return true if this vector is inside the min and max vector range
        return (_x > min.x() + var<T>::TOL_REL && _x < max.x() - var<T>::TOL_REL && _y > min.y() + var<T>::TOL_REL && _y < max.y() - var<T>::TOL_REL && _z > min.z() + var<T>::TOL_REL && _z < max.z() - var<T>::TOL_REL);
    }
    inline vec4<T> inverse() const
    {
        const T x = safe_inverse<T>(_x);
        const T y = safe_inverse<T>(_y);
        const T z = safe_inverse<T>(_z);

        // return inverse
        return vec4<T>(x, y, z, 1.0);
    }
    inline static vec4<T> lerp(const vec4<T> &v0, const vec4<T> &v1, T t)
    {
        return (v0 + (v1 - v0) * (t));
    }
    inline static vec4<T> interpolate(const vec4<T> &v0, const vec4<T> &v1, T t)
    {
        return lerp(v0, v1, t);
    }
    inline T magnitude() const
    {
        return sqrt(_x * _x + _y * _y + _z * _z);
    }
    inline T max() const
    {
        return std::max(std::max(_x, _y), _z);
    }
    inline T min() const
    {
        return std::min(std::min(_x, _y), _z);
    }
    inline static std::pair<vec4<T>, vec4<T>> most_separating(const std::vector<vec4<T>> &verts)
    {
        // Find the two most separating points in the collection of vec4<T>
        const auto size = verts.size();
        if (size > 1)
        {
            size_t minx, maxx, miny, maxy, minz, maxz, min, max;
            minx = maxx = miny = maxy = minz = maxz = min = max = 0;
            for (size_t i = 0; i < size; i++)
            {
                const vec4<T> &v = verts[i];
                if (v.x() > verts[maxx].x())
                    maxx = i;
                if (v.x() < verts[minx].x())
                    minx = i;
                if (v.y() > verts[maxy].y())
                    maxy = i;
                if (v.y() < verts[miny].y())
                    miny = i;
                if (v.z() > verts[maxz].z())
                    maxz = i;
                if (v.z() < verts[minz].z())
                    minz = i;
            }

            // Calculate span of the extents
            const vec4<T> dx = verts[maxx] - verts[minx];
            const vec4<T> dy = verts[maxy] - verts[miny];
            const vec4<T> dz = verts[maxz] - verts[minz];

            // Calculated the squared euclidean distance
            const T dx2 = dx.dot(dx);
            const T dy2 = dy.dot(dy);
            const T dz2 = dz.dot(dz);

            // Calculate the max extents
            min = minx;
            max = maxx;
            if (dy2 > dx2 && dy2 > dz2)
            {
                min = miny;
                max = maxy;
            }
            if (dz2 > dx2 && dz2 > dy2)
            {
                min = minz;
                max = maxz;
            }

            // Return the two most separating verts
            return std::make_pair(verts[min], verts[max]);
        }

        return std::make_pair(vec4<T>(), vec4<T>());
    }
    inline static vec4<T> normal(const vec4<T> &a, const vec4<T> &b, const vec4<T> &c)
    {
        // Computes normal vector to three points
        // Normal is not normalized
        return (b - a).cross(c - a);
    }
    inline vec4<T> &normalize()
    {
        const T mag = magnitude();
        if (std::abs(mag) > var<T>::TOL_ZERO)
        {
            T inv_mag = 1.0 / mag;
            _x *= inv_mag;
            _y *= inv_mag;
            _z *= inv_mag;
        }
        else
        {
            _x = 0.0;
            _y = 0.0;
            _z = 0.0;
        }

        return *this;
    }
    inline vec4<T> &normalize_unsafe()
    {
        const T inv_mag = 1.0 / magnitude();
        _x *= inv_mag;
        _y *= inv_mag;
        _z *= inv_mag;

        return *this;
    }
    inline vec4<T> &normalize_safe(const vec4<T> &safe)
    {
        const T mag = magnitude();
        if (std::abs(mag) > var<T>::TOL_ZERO)
        {
            T inv_mag = 1.0 / mag;
            _x *= inv_mag;
            _y *= inv_mag;
            _z *= inv_mag;
        }
        else
        {
            _x = safe.x();
            _y = safe.y();
            _z = safe.z();
        }

        return *this;
    }
    inline static void order(vec4<T> &min, vec4<T> &max)
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
        if (min._z > max._z)
        {
            swap(min._z, max._z);
        }
    }
    inline vec4<T> orthogonal() const
    {
        const T x2 = _x * _x;
        const T y2 = _y * _y;
        const T z2 = _z * _z;
        const T ratio = x2 / (x2 + y2 + z2);

        // If parallel to x axis, switch to y axis
        if (ratio > 0.95)
        {
            return cross_y();
        }

        // Compute cross product around x axis
        return cross_x();
    }
    inline vec4<T> project_point(const coord_sys<T, min::vec4> &axis, const vec4<T> &extent)
    {
        // Project v onto local x axis
        T x = dot(axis.x());

        // Clamp d onto the box half extent
        min::clamp<T>(x, -extent.x(), extent.x());

        // Project v onto local y axis
        T y = dot(axis.y());

        // Clamp d onto the box half extent
        min::clamp<T>(y, -extent.y(), extent.y());

        // Project v onto local z axis
        T z = dot(axis.z());

        // Clamp d onto the box half extent
        min::clamp<T>(z, -extent.z(), extent.z());

        // Compute the point along this axis
        return (axis.x() * x) + (axis.y() * y) + (axis.z() * z);
    }
    inline T project_length(const coord_sys<T, min::vec4> &axis, const vec4<T> &extent)
    {
        // Project this onto local x axis
        const T x = dot(axis.x());

        // Clamp x onto the box half extent, else zero
        const T dx = clamp_value<T>(x, -extent.x(), x + extent.x(), extent.x(), x - extent.x());

        // Project this onto local y axis
        const T y = dot(axis.y());

        // Clamp y onto the box half extent, else 0
        const T dy = clamp_value<T>(y, -extent.y(), y + extent.y(), extent.y(), y - extent.y());

        // Project this onto local y axis
        const T z = dot(axis.z());

        // Clamp y onto the box half extent, else 0
        const T dz = clamp_value<T>(z, -extent.z(), z + extent.z(), extent.z(), z - extent.z());

        // Compute the square distance from this point
        return (dx * dx) + (dy * dy) + (dz * dz);
    }
    static inline bool project_sat(const coord_sys<T, min::vec4> &axis1, const vec4<T> &center1, const vec4<T> &extent1, const coord_sys<T, min::vec4> &axis2, const vec4<T> &center2, const vec4<T> &extent2)
    {
        // This performs the separating axis theorem for checking oobb-oobb intersections
        // For every axis test (C2-C1).dot(L) > (a.get_extent() + b.get_extent()).dot(L)
        // This means testing the difference between box centers, C1 & C2, along the separating axis L
        // With the addition of box extents along this same axis L
        // For 3D, there are 15 axes that need to be tested against...
        // 2x3=6 local box axes plus and 3x3=9 axes perpendicular to the 6 local box axes

        // Rotation matrix expressing A2 in A1's coordinate frame
        const T x1x2 = axis1.x().dot(axis2.x());
        const T x1y2 = axis1.x().dot(axis2.y());
        const T x1z2 = axis1.x().dot(axis2.z());
        const T y1x2 = axis1.y().dot(axis2.x());
        const T y1y2 = axis1.y().dot(axis2.y());
        const T y1z2 = axis1.y().dot(axis2.z());
        const T z1x2 = axis1.z().dot(axis2.x());
        const T z1y2 = axis1.z().dot(axis2.y());
        const T z1z2 = axis1.z().dot(axis2.z());

        const T abs_x1x2 = std::abs(x1x2) + var<T>::TOL_REL;
        const T abs_x1y2 = std::abs(x1y2) + var<T>::TOL_REL;
        const T abs_x1z2 = std::abs(x1z2) + var<T>::TOL_REL;
        const T abs_y1x2 = std::abs(y1x2) + var<T>::TOL_REL;
        const T abs_y1y2 = std::abs(y1y2) + var<T>::TOL_REL;
        const T abs_y1z2 = std::abs(y1z2) + var<T>::TOL_REL;
        const T abs_z1x2 = std::abs(z1x2) + var<T>::TOL_REL;
        const T abs_z1y2 = std::abs(z1y2) + var<T>::TOL_REL;
        const T abs_z1z2 = std::abs(z1z2) + var<T>::TOL_REL;

        // Bring translation into A1's coordinate frame
        const vec4<T> d = center2 - center1;
        const vec4<T> t = vec4<T>(d.dot(axis1.x()), d.dot(axis1.y()), d.dot(axis1.z()), 1.0);

        // Test L = A1.x(); d1 and d2 is the length of extents along L
        T dL1 = extent1.x();
        T dL2 = extent2.x() * abs_x1x2 + extent2.y() * abs_x1y2 + extent2.z() * abs_x1z2;
        if (std::abs(t.x()) > dL1 + dL2)
            return false;

        // Test L = A1.y(); d1 and d2 is the length of extents along L
        dL1 = extent1.y();
        dL2 = extent2.x() * abs_y1x2 + extent2.y() * abs_y1y2 + extent2.z() * abs_y1z2;
        if (std::abs(t.y()) > dL1 + dL2)
            return false;

        // Test L = A1.z(); d1 and d2 is the length of extents along L
        dL1 = extent1.z();
        dL2 = extent2.x() * abs_z1x2 + extent2.y() * abs_z1y2 + extent2.z() * abs_z1z2;
        if (std::abs(t.z()) > dL1 + dL2)
            return false;

        // Test L = A2.x(); d1 and d2 is the length of extents along L
        dL1 = extent1.x() * abs_x1x2 + extent1.y() * abs_y1x2 + extent1.z() * abs_z1x2;
        dL2 = extent2.x();
        if (std::abs(t.x() * x1x2 + t.y() * y1x2 + t.z() * z1x2) > dL1 + dL2)
            return false;

        // Test L = A2.y(); d1 and d2 is the length of extents along L
        dL1 = extent1.x() * abs_x1y2 + extent1.y() * abs_y1y2 + extent1.z() * abs_z1y2;
        dL2 = extent2.y();
        if (std::abs(t.x() * x1y2 + t.y() * y1y2 + t.z() * z1y2) > dL1 + dL2)
            return false;

        // Test L = A2.z(); d1 and d2 is the length of extents along L
        dL1 = extent1.x() * abs_x1z2 + extent1.y() * abs_y1z2 + extent1.z() * abs_z1z2;
        dL2 = extent2.z();
        if (std::abs(t.x() * x1z2 + t.y() * y1z2 + t.z() * z1z2) > dL1 + dL2)
            return false;

        // Test axis L = A1.x() X A2.x()
        dL1 = extent1.y() * abs_z1x2 + extent1.z() * abs_y1x2;
        dL2 = extent2.y() * abs_x1z2 + extent2.z() * abs_x1y2;
        if (std::abs(t.z() * y1x2 - t.y() * z1x2) > dL1 + dL2)
            return false;

        // Test axis L = A1.x() X A2.y()
        dL1 = extent1.y() * abs_z1y2 + extent1.z() * abs_y1y2;
        dL2 = extent2.x() * abs_x1z2 + extent2.z() * abs_x1x2;
        if (std::abs(t.z() * y1y2 - t.y() * z1y2) > dL1 + dL2)
            return false;

        // Test axis L = A1.x() X A2.z()
        dL1 = extent1.y() * abs_z1z2 + extent1.z() * abs_y1z2;
        dL2 = extent2.x() * abs_x1y2 + extent2.y() * abs_x1x2;
        if (std::abs(t.z() * y1z2 - t.y() * z1z2) > dL1 + dL2)
            return false;

        // Test axis L = A1.y() X A2.x()
        dL1 = extent1.x() * abs_z1x2 + extent1.z() * abs_x1x2;
        dL2 = extent2.y() * abs_y1z2 + extent2.z() * abs_y1y2;
        if (std::abs(t.x() * z1x2 - t.z() * x1x2) > dL1 + dL2)
            return false;

        // Test axis L = A1.y() X A2.y()
        dL1 = extent1.x() * abs_z1y2 + extent1.z() * abs_x1y2;
        dL2 = extent2.x() * abs_y1z2 + extent2.z() * abs_y1x2;
        if (std::abs(t.x() * z1y2 - t.z() * x1y2) > dL1 + dL2)
            return false;

        // Test axis L = A1.y() X A2.z()
        dL1 = extent1.x() * abs_z1z2 + extent1.z() * abs_x1z2;
        dL2 = extent2.x() * abs_y1y2 + extent2.y() * abs_y1x2;
        if (std::abs(t.x() * z1z2 - t.z() * x1z2) > dL1 + dL2)
            return false;

        // Test axis L = A1.z() X A2.x()
        dL1 = extent1.x() * abs_y1x2 + extent1.y() * abs_x1x2;
        dL2 = extent2.y() * abs_z1z2 + extent2.z() * abs_z1y2;
        if (std::abs(t.y() * x1x2 - t.x() * y1x2) > dL1 + dL2)
            return false;

        // Test axis L = A1.z() X A2.y()
        dL1 = extent1.x() * abs_y1y2 + extent1.y() * abs_x1y2;
        dL2 = extent2.x() * abs_z1z2 + extent2.z() * abs_z1x2;
        if (std::abs(t.y() * x1y2 - t.x() * y1y2) > dL1 + dL2)
            return false;

        // Test axis L = A1.z() X A2.z()
        dL1 = extent1.x() * abs_y1z2 + extent1.y() * abs_x1z2;
        dL2 = extent2.x() * abs_z1y2 + extent2.y() * abs_z1x2;
        if (std::abs(t.y() * x1z2 - t.x() * y1z2) > dL1 + dL2)
            return false;

        return true;
    }
    static inline std::pair<vec4<T>, T> project_sat_penetration(
        const coord_sys<T, min::vec4> &axis1, const vec4<T> &center1, const vec4<T> &extent1,
        const coord_sys<T, min::vec4> &axis2, const vec4<T> &center2, const vec4<T> &extent2, const T tolerance)
    {
        // This performs the separating axis theorem for checking oobb-oobb intersection penetration
        // For every axis, penetration = (a.get_extent() + b.get_extent()).dot(L) - (C2-C1).dot(L)
        // This means testing the difference between box centers, C1 & C2, along the separating axis L
        // With the addition of box extents along this same axis L
        // For 3D, there are 15 axes that need to be tested against...
        // 2x3=6 local box axes plus and 3x3=9 axes perpendicular to the 6 local box axes

        // Rotation matrix expressing A2 in A1's coordinate frame
        const T x1x2 = axis1.x().dot(axis2.x());
        const T x1y2 = axis1.x().dot(axis2.y());
        const T x1z2 = axis1.x().dot(axis2.z());
        const T y1x2 = axis1.y().dot(axis2.x());
        const T y1y2 = axis1.y().dot(axis2.y());
        const T y1z2 = axis1.y().dot(axis2.z());
        const T z1x2 = axis1.z().dot(axis2.x());
        const T z1y2 = axis1.z().dot(axis2.y());
        const T z1z2 = axis1.z().dot(axis2.z());

        const T abs_x1x2 = std::abs(x1x2) + tolerance;
        const T abs_x1y2 = std::abs(x1y2) + tolerance;
        const T abs_x1z2 = std::abs(x1z2) + tolerance;
        const T abs_y1x2 = std::abs(y1x2) + tolerance;
        const T abs_y1y2 = std::abs(y1y2) + tolerance;
        const T abs_y1z2 = std::abs(y1z2) + tolerance;
        const T abs_z1x2 = std::abs(z1x2) + tolerance;
        const T abs_z1y2 = std::abs(z1y2) + tolerance;
        const T abs_z1z2 = std::abs(z1z2) + tolerance;

        // Bring translation into A1's coordinate frame
        const vec4<T> d = center2 - center1;
        const vec4<T> t = vec4<T>(d.dot(axis1.x()), d.dot(axis1.y()), d.dot(axis1.z()), 1.0);

        // Store axis and penetration depths
        vec4<T> axes[15];
        T penetration[15];

        // Test L = A1.x(); d1 and d2 is the length of extents along L
        T dL1 = extent1.x();
        T dL2 = extent2.x() * abs_x1x2 + extent2.y() * abs_x1y2 + extent2.z() * abs_x1z2;
        axes[0] = axis1.x();
        penetration[0] = (dL1 + dL2) - std::abs(t.x());

        // Test L = A1.y(); d1 and d2 is the length of extents along L
        dL1 = extent1.y();
        dL2 = extent2.x() * abs_y1x2 + extent2.y() * abs_y1y2 + extent2.z() * abs_y1z2;
        axes[1] = axis1.y();
        penetration[1] = (dL1 + dL2) - std::abs(t.y());

        // Test L = A1.z(); d1 and d2 is the length of extents along L
        dL1 = extent1.z();
        dL2 = extent2.x() * abs_z1x2 + extent2.y() * abs_z1y2 + extent2.z() * abs_z1z2;
        axes[2] = axis1.z();
        penetration[2] = (dL1 + dL2) - std::abs(t.z());

        // Test L = A2.x(); d1 and d2 is the length of extents along L
        dL1 = extent1.x() * abs_x1x2 + extent1.y() * abs_y1x2 + extent1.z() * abs_z1x2;
        dL2 = extent2.x();
        axes[3] = axis2.x();
        penetration[3] = (dL1 + dL2) - std::abs(t.x() * x1x2 + t.y() * y1x2 + t.z() * z1x2);

        // Test L = A2.y(); d1 and d2 is the length of extents along L
        dL1 = extent1.x() * abs_x1y2 + extent1.y() * abs_y1y2 + extent1.z() * abs_z1y2;
        dL2 = extent2.y();
        axes[4] = axis2.y();
        penetration[4] = (dL1 + dL2) - std::abs(t.x() * x1y2 + t.y() * y1y2 + t.z() * z1y2);

        // Test L = A2.z(); d1 and d2 is the length of extents along L
        dL1 = extent1.x() * abs_x1z2 + extent1.y() * abs_y1z2 + extent1.z() * abs_z1z2;
        dL2 = extent2.z();
        axes[5] = axis2.z();
        penetration[5] = (dL1 + dL2) - std::abs(t.x() * x1z2 + t.y() * y1z2 + t.z() * z1z2);

        // Test axis L = A1.x() X A2.x()
        dL1 = extent1.y() * abs_z1x2 + extent1.z() * abs_y1x2;
        dL2 = extent2.y() * abs_x1z2 + extent2.z() * abs_x1y2;
        axes[6] = axis1.x().cross(axis2.x());
        penetration[6] = (dL1 + dL2) - std::abs(t.z() * y1x2 - t.y() * z1x2);

        // Test axis L = A1.x() X A2.y()
        dL1 = extent1.y() * abs_z1y2 + extent1.z() * abs_y1y2;
        dL2 = extent2.x() * abs_x1z2 + extent2.z() * abs_x1x2;
        axes[7] = axis1.x().cross(axis2.y());
        penetration[7] = (dL1 + dL2) - std::abs(t.z() * y1y2 - t.y() * z1y2);

        // Test axis L = A1.x() X A2.z()
        dL1 = extent1.y() * abs_z1z2 + extent1.z() * abs_y1z2;
        dL2 = extent2.x() * abs_x1y2 + extent2.y() * abs_x1x2;
        axes[8] = axis1.x().cross(axis2.z());
        penetration[8] = (dL1 + dL2) - std::abs(t.z() * y1z2 - t.y() * z1z2);

        // Test axis L = A1.y() X A2.x()
        dL1 = extent1.x() * abs_z1x2 + extent1.z() * abs_x1x2;
        dL2 = extent2.y() * abs_y1z2 + extent2.z() * abs_y1y2;
        axes[9] = axis1.y().cross(axis2.x());
        penetration[9] = (dL1 + dL2) - std::abs(t.x() * z1x2 - t.z() * x1x2);

        // Test axis L = A1.y() X A2.y()
        dL1 = extent1.x() * abs_z1y2 + extent1.z() * abs_x1y2;
        dL2 = extent2.x() * abs_y1z2 + extent2.z() * abs_y1x2;
        axes[10] = axis1.y().cross(axis2.y());
        penetration[10] = (dL1 + dL2) - std::abs(t.x() * z1y2 - t.z() * x1y2);

        // Test axis L = A1.y() X A2.z()
        dL1 = extent1.x() * abs_z1z2 + extent1.z() * abs_x1z2;
        dL2 = extent2.x() * abs_y1y2 + extent2.y() * abs_y1x2;
        axes[11] = axis1.y().cross(axis2.z());
        penetration[11] = (dL1 + dL2) - std::abs(t.x() * z1z2 - t.z() * x1z2);

        // Test axis L = A1.z() X A2.x()
        dL1 = extent1.x() * abs_y1x2 + extent1.y() * abs_x1x2;
        dL2 = extent2.y() * abs_z1z2 + extent2.z() * abs_z1y2;
        axes[12] = axis1.z().cross(axis2.x());
        penetration[12] = (dL1 + dL2) - std::abs(t.y() * x1x2 - t.x() * y1x2);

        // Test axis L = A1.z() X A2.y()
        dL1 = extent1.x() * abs_y1y2 + extent1.y() * abs_x1y2;
        dL2 = extent2.x() * abs_z1z2 + extent2.z() * abs_z1x2;
        axes[13] = axis1.z().cross(axis2.y());
        penetration[13] = (dL1 + dL2) - std::abs(t.y() * x1y2 - t.x() * y1y2);

        // Test axis L = A1.z() X A2.z()
        dL1 = extent1.x() * abs_y1z2 + extent1.y() * abs_x1z2;
        dL2 = extent2.x() * abs_z1y2 + extent2.y() * abs_z1x2;
        axes[14] = axis1.z().cross(axis2.z());
        penetration[14] = (dL1 + dL2) - std::abs(t.y() * x1z2 - t.x() * y1z2);

        // normal default up vector return and zero penetration
        vec4<T> normal = vec4<T>::up();
        T overlap = 0.0;

        // Find the minimum, non-zero penetration index
        T min = std::numeric_limits<T>::max();
        int index = -1;
        for (int i = 0; i < 15; i++)
        {
            // Prune all parallel normal vectors and non-penetrating depths
            const T mag2 = axes[i].dot(axes[i]);
            if ((mag2 > tolerance) && (penetration[i] > tolerance) && (penetration[i] < (min + tolerance)))
            {
                min = penetration[i];
                index = i;
            }
        }

        // check if we found an intersection penetration
        if (index != -1)
        {
            // Calculate the sign of normal towards body1 and scale normal
            const vec4<T> sign = (center1 - center2).sign();
            normal = axes[index].abs() * sign;
            overlap = min;
        }

        // return normal vector and minimum penentration
        return std::make_pair(normal, overlap);
    }
    static inline std::pair<vec4<T>, T> project_sat_aligned_penetration(
        const vec4<T> &center1, const vec4<T> &extent1,
        const vec4<T> &center2, const vec4<T> &extent2, const T tolerance)
    {
        // This performs the separating axis theorem for checking aabb-aabb intersection penetration
        // For every axis, penetration = (a.get_extent() + b.get_extent()).dot(L) - (C2-C1).dot(L) on aligned axises
        // This means testing the difference between box centers, C1 & C2, along the separating axis L
        // With the addition of box extents along this same axis L
        // For 3D, there are 3 axes that need to be tested against...
        // 3 local box axes

        // Rotation matrix expressing A2 in A1's coordinate frame
        const T abs_x1x2 = 1.0 + tolerance;
        const T abs_x1y2 = tolerance;
        const T abs_x1z2 = tolerance;
        const T abs_y1x2 = tolerance;
        const T abs_y1y2 = 1.0 + tolerance;
        const T abs_y1z2 = tolerance;
        const T abs_z1x2 = tolerance;
        const T abs_z1y2 = tolerance;
        const T abs_z1z2 = 1.0 + tolerance;

        // Bring translation into A1's coordinate frame
        const vec4<T> t = vec4<T>(center2 - center1);

        // Store axis and penetration depths
        const vec4<T> axes[3] = {vec4<T>(1.0, 0.0, 0.0, 1.0), vec4<T>(0.0, 1.0, 0.0, 1.0), vec4<T>(0.0, 0.0, 1.0, 1.0)};
        T penetration[6];

        // Test L = A1.x(); d1 and d2 is the length of extents along L
        T dL1 = extent1.x();
        T dL2 = extent2.x() * abs_x1x2 + extent2.y() * abs_x1y2 + extent2.z() * abs_x1z2;
        penetration[0] = (dL1 + dL2) - std::abs(t.x());

        // Test L = A1.y(); d1 and d2 is the length of extents along L
        dL1 = extent1.y();
        dL2 = extent2.x() * abs_y1x2 + extent2.y() * abs_y1y2 + extent2.z() * abs_y1z2;
        penetration[1] = (dL1 + dL2) - std::abs(t.y());

        // Test L = A1.z(); d1 and d2 is the length of extents along L
        dL1 = extent1.z();
        dL2 = extent2.x() * abs_z1x2 + extent2.y() * abs_z1y2 + extent2.z() * abs_z1z2;
        penetration[2] = (dL1 + dL2) - std::abs(t.z());

        // Test L = A2.x(); d1 and d2 is the length of extents along L
        dL1 = extent1.x() * abs_x1x2 + extent1.y() * abs_y1x2 + extent1.z() * abs_z1x2;
        dL2 = extent2.x();
        penetration[3] = (dL1 + dL2) - std::abs(t.x());

        // Test L = A2.y(); d1 and d2 is the length of extents along L
        dL1 = extent1.x() * abs_x1y2 + extent1.y() * abs_y1y2 + extent1.z() * abs_z1y2;
        dL2 = extent2.y();
        penetration[4] = (dL1 + dL2) - std::abs(t.y());

        // Test L = A2.z(); d1 and d2 is the length of extents along L
        dL1 = extent1.x() * abs_x1z2 + extent1.y() * abs_y1z2 + extent1.z() * abs_z1z2;
        dL2 = extent2.z();
        penetration[5] = (dL1 + dL2) - std::abs(t.z());

        // normal default up vector return and zero penetration
        vec4<T> normal = vec4<T>::up();
        T overlap = 0.0;

        // Find the minimum, non-zero penetration index
        T min = std::numeric_limits<T>::max();
        int index = -1;
        for (int i = 0; i < 6; i++)
        {
            // Prune all parallel normal vectors and non-penetrating depths
            if (penetration[i] > tolerance && penetration[i] < min)
            {
                min = penetration[i];
                index = i;
            }
        }

        // check if we found an intersection penetration
        if (index != -1)
        {
            // Calculate the sign of normal towards body1 and scale normal
            const vec4<T> sign = (center1 - center2).sign();
            normal = axes[index % 3] * sign;
            overlap = min;
        }

        // return normal vector and minimum penentration
        return std::make_pair(normal, overlap);
    }

    // Subdividing vector space into 2^3 spaces using binary key location codes for index (xyz)
    // Most significant bit of (x - xmin)/(xmax - xmin), (y - ymin)/(ymax - ymin), (z - zmin)/(zmax - zmin)
    // Yields the key location code if MSB 0 = -, MSB 1 = +
    // Example position 3 = 011 = left, up, forward
    //
    //     /-----/-----/
    //    /  3  /  7  /
    //   /-----/-----/
    //  /  2  /  6  /
    // /-----/-----/
    //     /-----/-----/
    //    /  1  /  5  /
    //   /-----/-----/
    //  /  0  /  4  /
    // /-----/-----/
    inline static vec4<T> ratio(const vec4<T> &min, const vec4<T> &max, const vec4<T> &point)
    {
        // Calculates the ratio of the point between min and max [0.0, 1.0]
        const T xr = (point.x() - min.x()) / (max.x() - min.x());
        const T yr = (point.y() - min.y()) / (max.y() - min.y());
        const T zr = (point.z() - min.z()) / (max.z() - min.z());

        return vec4<T>(xr, yr, zr, 1.0);
    }
    inline vec4<T> sign() const
    {
        // Get the sign of the vector
        const T x = sgn<T>(_x);
        const T y = sgn<T>(_y);
        const T z = sgn<T>(_z);

        return vec4<T>(x, y, z, 1.0);
    }
    inline uint_fast8_t subdivide_key(const T middle)
    {
        T x = _x;
        T y = _y;
        T z = _z;

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

        // Get the z portion of key
        key <<= 1;
        if (z >= middle)
        {
            // Set the least significant bit
            key |= 0x1;
            z -= middle;
            _z = z;
        }

        return key;
    }
    inline uint_fast8_t subdivide_key(const vec4<T> &center) const
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

        // Get the z portion of key
        key <<= 1;
        if (_z >= center.z())
        {
            // Set the least significant bit
            key |= 0x1;
        }

        return key;
    }
    inline static constexpr size_t sub_size()
    {
        return 8;
    }
    template <typename C>
    inline static auto subdivide(C &out, const vec4<T> &min, const vec4<T> &max)
    {
        // Clear out vector
        out.clear();
        out.reserve(out.size() + vec4<T>::sub_size());

        // Center of the vector space
        const vec4<T> c = (max + min) * 0.5;

        // Octant 0
        const vec4<T> min0 = vec4<T>(min.x(), min.y(), min.z(), 1.0);
        const vec4<T> max0 = vec4<T>(c.x(), c.y(), c.z(), 1.0);

        // Octant 1
        const vec4<T> min1 = vec4<T>(min.x(), min.y(), c.z(), 1.0);
        const vec4<T> max1 = vec4<T>(c.x(), c.y(), max.z(), 1.0);

        // Octant 2
        const vec4<T> min2 = vec4<T>(min.x(), c.y(), min.z(), 1.0);
        const vec4<T> max2 = vec4<T>(c.x(), max.y(), c.z(), 1.0);

        // Octant 3
        const vec4<T> min3 = vec4<T>(min.x(), c.y(), c.z(), 1.0);
        const vec4<T> max3 = vec4<T>(c.x(), max.y(), max.z(), 1.0);

        // Octant 4
        const vec4<T> min4 = vec4<T>(c.x(), min.y(), min.z(), 1.0);
        const vec4<T> max4 = vec4<T>(max.x(), c.y(), c.z(), 1.0);

        // Octant 5
        const vec4<T> min5 = vec4<T>(c.x(), min.y(), c.z(), 1.0);
        const vec4<T> max5 = vec4<T>(max.x(), c.y(), max.z(), 1.0);

        // Octant 6
        const vec4<T> min6 = vec4<T>(c.x(), c.y(), min.z(), 1.0);
        const vec4<T> max6 = vec4<T>(max.x(), max.y(), c.z(), 1.0);

        // Octant 7
        const vec4<T> min7 = vec4<T>(c.x(), c.y(), c.z(), 1.0);
        const vec4<T> max7 = vec4<T>(max.x(), max.y(), max.z(), 1.0);

        // Add sub spaces to out vector
        out.emplace_back(min0, max0);
        out.emplace_back(min1, max1);
        out.emplace_back(min2, max2);
        out.emplace_back(min3, max3);
        out.emplace_back(min4, max4);
        out.emplace_back(min5, max5);
        out.emplace_back(min6, max6);
        out.emplace_back(min7, max7);
    }
    template <typename C>
    inline static auto subdivide_center(C &out, const vec4<T> &min, const vec4<T> &max)
    {
        // Clear out vector
        out.clear();
        out.reserve(out.size() + vec4<T>::sub_size());

        // Quarter extent of vector space
        const vec4<T> h = ((max - min) * 0.25) + var<T>::TOL_REL;

        // Center of the vector space
        const vec4<T> c = (max + min) * 0.5;

        // Positions
        const T cx_hx = c.x() - h.x();
        const T cy_hy = c.y() - h.y();
        const T cz_hz = c.z() - h.z();

        const T cxhx = c.x() + h.x();
        const T cyhy = c.y() + h.y();
        const T czhz = c.z() + h.z();

        // Octant 0
        const vec4<T> c0 = vec4<T>(cx_hx, cy_hy, cz_hz, 1.0);

        // Octant 1
        const vec4<T> c1 = vec4<T>(cx_hx, cy_hy, czhz, 1.0);

        // Octant 2
        const vec4<T> c2 = vec4<T>(cx_hx, cyhy, cz_hz, 1.0);

        // Octant 3
        const vec4<T> c3 = vec4<T>(cx_hx, cyhy, czhz, 1.0);

        // Octant 4
        const vec4<T> c4 = vec4<T>(cxhx, cy_hy, cz_hz, 1.0);

        // Octant 5
        const vec4<T> c5 = vec4<T>(cxhx, cy_hy, czhz, 1.0);

        // Octant 6
        const vec4<T> c6 = vec4<T>(cxhx, cyhy, cz_hz, 1.0);

        // Octant 7
        const vec4<T> c7 = vec4<T>(cxhx, cyhy, czhz, 1.0);

        // Calculate the square distance between center and extent
        const T radius2 = h.dot(h);

        // Calculate the radius
        const T radius = std::sqrt(radius2);

        // Add sub spaces to out vector
        out.emplace_back(c0, radius);
        out.emplace_back(c1, radius);
        out.emplace_back(c2, radius);
        out.emplace_back(c3, radius);
        out.emplace_back(c4, radius);
        out.emplace_back(c5, radius);
        out.emplace_back(c6, radius);
        out.emplace_back(c7, radius);
    }
    // Plane n·x - c = 0
    // Ray x = P + td
    // If intersecting n · (P + td) - c = 0; x >= 0.0
    // n · P + n · td - c = 0
    // t = (c - n · P) / (n · d)
    // Each axis is axis aligned so we can simplify to, where nx = ny = 1
    // tx(yz-plane) = (cx - nx · Px) / (nx · dx)
    // ty(xz-plane) = (cy - ny · Py) / (ny · dy)
    // tz(xy-plane) = (cz - nz · Pz) / (nz · dz)
    inline static auto subdivide_ray(const vec4<T> &min, const vec4<T> &max, const vec4<T> &origin, const vec4<T> &dir, const vec4<T> &inv_dir)
    {
        min::stack_vector<uint_fast8_t, vec4<T>::sub_size()> out;

        // Ray can't intersect the slab if ray is parallel to axis
        if (origin.any_zero_outside(dir, min, max))
        {
            return out;
        }

        // Center of the vector space
        const vec4<T> center = (max + min) * 0.5;

        // Calculate ray intersections among all axes
        const vec4<T> t = (center - origin) * inv_dir;

        // YZ intersection types
        const T pyz_y = origin.y() + (t.x() * dir.y());
        const T pyz_z = origin.z() + (t.x() * dir.z());
        const bool yz_front = t.x() >= 0.0;
        const bool ymin_zmin_out = yz_front && (pyz_y < center.y()) && (pyz_z < center.z());
        const bool ymax_zmin_out = yz_front && (pyz_y >= center.y()) && (pyz_z < center.z());
        const bool ymin_zmax_out = yz_front && (pyz_y < center.y()) && (pyz_z >= center.z());
        const bool ymax_zmax_out = yz_front && (pyz_y >= center.y()) && (pyz_z >= center.z());
        const bool ymin_zmin = (pyz_y >= min.y()) && (pyz_z >= min.z());
        const bool ymax_zmin = (pyz_y <= max.y()) && (pyz_z >= min.z());
        const bool ymin_zmax = (pyz_y >= min.y()) && (pyz_z <= max.z());
        const bool ymax_zmax = (pyz_y <= max.y()) && (pyz_z <= max.z());

        // XZ intersection types
        const T pxz_x = origin.x() + (t.y() * dir.x());
        const T pxz_z = origin.z() + (t.y() * dir.z());
        const bool xz_front = t.y() >= 0.0;
        const bool xmin_zmin_out = xz_front && (pxz_x < center.x()) && (pxz_z < center.z());
        const bool xmax_zmin_out = xz_front && (pxz_x >= center.x()) && (pxz_z < center.z());
        const bool xmin_zmax_out = xz_front && (pxz_x < center.x()) && (pxz_z >= center.z());
        const bool xmax_zmax_out = xz_front && (pxz_x >= center.x()) && (pxz_z >= center.z());
        const bool xmin_zmin = (pxz_x >= min.x()) && (pxz_z >= min.z());
        const bool xmax_zmin = (pxz_x <= max.x()) && (pxz_z >= min.z());
        const bool xmin_zmax = (pxz_x >= min.x()) && (pxz_z <= max.z());
        const bool xmax_zmax = (pxz_x <= max.x()) && (pxz_z <= max.z());

        // XY intersection types
        const T pxy_x = origin.x() + (t.z() * dir.x());
        const T pxy_y = origin.y() + (t.z() * dir.y());
        const bool xy_front = t.z() >= 0.0;
        const bool xmin_ymin_out = xy_front && (pxy_x < center.x()) && (pxy_y < center.y());
        const bool xmax_ymin_out = xy_front && (pxy_x >= center.x()) && (pxy_y < center.y());
        const bool xmin_ymax_out = xy_front && (pxy_x < center.x()) && (pxy_y >= center.y());
        const bool xmax_ymax_out = xy_front && (pxy_x >= center.x()) && (pxy_y >= center.y());
        const bool xmin_ymin = (pxy_x >= min.x()) && (pxy_y >= min.y());
        const bool xmax_ymin = (pxy_x <= max.x()) && (pxy_y >= min.y());
        const bool xmin_ymax = (pxy_x >= min.x()) && (pxy_y <= max.y());
        const bool xmax_ymax = (pxy_x <= max.x()) && (pxy_y <= max.y());

        // Less than
        const bool xly = yz_front && (!xz_front || (t.x() < t.y()));
        const bool xlz = yz_front && (!xy_front || (t.x() < t.z()));
        const bool ylx = xz_front && (!yz_front || (t.y() < t.x()));
        const bool ylz = xz_front && (!xy_front || (t.y() < t.z()));
        const bool zlx = xy_front && (!yz_front || (t.z() < t.x()));
        const bool zly = xy_front && (!xz_front || (t.z() < t.y()));

        // Prefer point inside versus first plane intersection
        const bool yz_inside = ymin_zmin && ymax_zmax;
        const bool xz_inside = xmin_zmin && xmax_zmax;
        const bool xy_inside = xmin_ymin && xmax_ymax;

        // Special case, only one cell is intersected in this case
        const bool all_outside = !yz_inside && !xz_inside && !xy_inside;
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
                vec4<T> near = (min - origin) * inv_dir;
                vec4<T> far = (max - origin) * inv_dir;

                // Order to get the nearer intersection points
                vec4<T>::order(near, far);

                // Get the farthest entry into the slab
                const T tmin = near.max();

                // Get the nearest exit from a slab
                const T tmax = far.min();

                // If tmin are >= 0.0 and nearest exit > farthest entry we have an intersection
                if (tmax >= tmin && tmin >= 0.0)
                {
                    // Find the octant the the origin is in
                    const vec4<T> point = (origin + (dir * tmin));

                    // Get the key from octant
                    const uint_fast8_t key = point.subdivide_key(center);
                    out.push_back(key);
                }
            }

            // Early return
            return out;
        }

        // Calculate first axis intersection
        const bool yz_ = xly && xlz && yz_inside;
        const bool yz_xz = ylz;
        const bool yz_xy = zly;
        const bool xz_ = ylx && ylz && xz_inside;
        const bool xz_yz = xlz;
        const bool xz_xy = zlx;
        const bool xy_ = zlx && zly && xy_inside;
        const bool xy_xz = ylx;
        const bool xy_yz = xly;
        if (yz_)
        {
            if (ymin_zmin_out)
            {
                if (dir.x() < 0.0)
                {
                    if (ymin_zmin)
                    {
                        out.push_back(4);
                    }
                    out.push_back(0);

                    if (yz_xz && xmin_zmin)
                    {
                        out.push_back(2);
                        if (xmin_ymax)
                        {
                            out.push_back(3);
                        }
                    }
                    else if (yz_xy && xmin_ymin)
                    {
                        out.push_back(1);
                        if (xmin_zmax)
                        {
                            out.push_back(3);
                        }
                    }
                }
                else
                {
                    if (ymin_zmin)
                    {
                        out.push_back(0);
                    }
                    out.push_back(4);

                    if (yz_xz && xmax_zmin)
                    {
                        out.push_back(6);
                        if (xmax_ymax)
                        {
                            out.push_back(7);
                        }
                    }
                    else if (yz_xy && xmax_ymin)
                    {
                        out.push_back(5);
                        if (xmax_zmax)
                        {
                            out.push_back(7);
                        }
                    }
                }
            }
            else if (ymax_zmin_out)
            {
                if (dir.x() < 0.0)
                {
                    if (ymax_zmin)
                    {
                        out.push_back(6);
                    }
                    out.push_back(2);

                    if (yz_xz && xmin_zmin)
                    {
                        out.push_back(0);
                        if (xmin_ymin)
                        {
                            out.push_back(1);
                        }
                    }
                    else if (yz_xy && xmin_ymax)
                    {
                        out.push_back(3);
                        if (xmin_zmax)
                        {
                            out.push_back(1);
                        }
                    }
                }
                else
                {
                    if (ymax_zmin)
                    {
                        out.push_back(2);
                    }
                    out.push_back(6);

                    if (yz_xz && xmax_zmin)
                    {
                        out.push_back(4);
                        if (xmax_ymin)
                        {
                            out.push_back(5);
                        }
                    }
                    else if (yz_xy && xmax_ymax)
                    {
                        out.push_back(7);
                        if (xmax_zmax)
                        {
                            out.push_back(5);
                        }
                    }
                }
            }
            else if (ymin_zmax_out)
            {
                if (dir.x() < 0.0)
                {
                    if (ymin_zmax)
                    {
                        out.push_back(5);
                    }
                    out.push_back(1);

                    if (yz_xz && xmin_zmax)
                    {
                        out.push_back(3);
                        if (xmin_ymax)
                        {
                            out.push_back(2);
                        }
                    }
                    else if (yz_xy && xmin_ymin)
                    {
                        out.push_back(0);
                        if (xmin_zmin)
                        {
                            out.push_back(2);
                        }
                    }
                }
                else
                {
                    if (ymin_zmax)
                    {
                        out.push_back(1);
                    }
                    out.push_back(5);

                    if (yz_xz && xmax_zmax)
                    {
                        out.push_back(7);
                        if (xmax_ymax)
                        {
                            out.push_back(6);
                        }
                    }
                    else if (yz_xy && xmax_ymin)
                    {
                        out.push_back(4);
                        if (xmax_zmin)
                        {
                            out.push_back(6);
                        }
                    }
                }
            }
            else if (ymax_zmax_out)
            {
                if (dir.x() < 0.0)
                {
                    if (ymax_zmax)
                    {
                        out.push_back(7);
                    }
                    out.push_back(3);

                    if (yz_xz && xmin_zmax)
                    {
                        out.push_back(1);
                        if (xmin_ymin)
                        {
                            out.push_back(0);
                        }
                    }
                    else if (yz_xy && xmin_ymax)
                    {
                        out.push_back(2);
                        if (xmin_zmin)
                        {
                            out.push_back(0);
                        }
                    }
                }
                else
                {
                    if (ymax_zmax)
                    {
                        out.push_back(3);
                    }
                    out.push_back(7);

                    if (yz_xz && xmax_zmax)
                    {
                        out.push_back(5);
                        if (xmax_ymin)
                        {
                            out.push_back(4);
                        }
                    }
                    else if (yz_xy && xmax_ymax)
                    {
                        out.push_back(6);
                        if (xmax_zmin)
                        {
                            out.push_back(4);
                        }
                    }
                }
            }
        }
        else if (xz_)
        {
            if (xmin_zmin_out)
            {
                if (dir.y() < 0.0)
                {
                    if (xmin_zmin)
                    {
                        out.push_back(2);
                    }
                    out.push_back(0);

                    if (xz_yz && ymin_zmin)
                    {
                        out.push_back(4);
                        if (xmax_ymin)
                        {
                            out.push_back(5);
                        }
                    }
                    else if (xz_xy && xmin_ymin)
                    {
                        out.push_back(1);
                        if (ymin_zmax)
                        {
                            out.push_back(5);
                        }
                    }
                }
                else
                {
                    if (xmin_zmin)
                    {
                        out.push_back(0);
                    }
                    out.push_back(2);

                    if (xz_yz && ymax_zmin)
                    {
                        out.push_back(6);
                        if (xmax_ymax)
                        {
                            out.push_back(7);
                        }
                    }
                    else if (xz_xy && xmin_ymax)
                    {
                        out.push_back(3);
                        if (ymax_zmax)
                        {
                            out.push_back(7);
                        }
                    }
                }
            }
            else if (xmax_zmin_out)
            {
                if (dir.y() < 0.0)
                {
                    if (xmax_zmin)
                    {
                        out.push_back(6);
                    }
                    out.push_back(4);

                    if (xz_yz && ymin_zmin)
                    {
                        out.push_back(0);
                        if (xmin_ymin)
                        {
                            out.push_back(1);
                        }
                    }
                    else if (xz_xy && xmax_ymin)
                    {
                        out.push_back(5);
                        if (ymin_zmax)
                        {
                            out.push_back(1);
                        }
                    }
                }
                else
                {
                    if (xmax_zmin)
                    {
                        out.push_back(4);
                    }
                    out.push_back(6);

                    if (xz_yz && ymax_zmin)
                    {
                        out.push_back(2);
                        if (xmin_ymax)
                        {
                            out.push_back(3);
                        }
                    }
                    else if (xz_xy && xmax_ymax)
                    {
                        out.push_back(7);
                        if (ymax_zmax)
                        {
                            out.push_back(3);
                        }
                    }
                }
            }
            else if (xmin_zmax_out)
            {
                if (dir.y() < 0.0)
                {
                    if (xmin_zmax)
                    {
                        out.push_back(3);
                    }
                    out.push_back(1);

                    if (xz_yz && ymin_zmax)
                    {
                        out.push_back(5);
                        if (xmax_ymin)
                        {
                            out.push_back(4);
                        }
                    }
                    else if (xz_xy && xmin_ymin)
                    {
                        out.push_back(0);
                        if (ymin_zmin)
                        {
                            out.push_back(4);
                        }
                    }
                }
                else
                {
                    if (xmin_zmax)
                    {
                        out.push_back(1);
                    }
                    out.push_back(3);

                    if (xz_yz && ymax_zmax)
                    {
                        out.push_back(7);
                        if (xmax_ymax)
                        {
                            out.push_back(6);
                        }
                    }
                    else if (xz_xy && xmin_ymax)
                    {
                        out.push_back(2);
                        if (ymax_zmin)
                        {
                            out.push_back(6);
                        }
                    }
                }
            }
            else if (xmax_zmax_out)
            {
                if (dir.y() < 0.0)
                {
                    if (xmax_zmax)
                    {
                        out.push_back(7);
                    }
                    out.push_back(5);

                    if (xz_yz && ymin_zmax)
                    {
                        out.push_back(1);
                        if (xmin_ymin)
                        {
                            out.push_back(0);
                        }
                    }
                    else if (xz_xy && xmax_ymin)
                    {
                        out.push_back(4);
                        if (ymin_zmin)
                        {
                            out.push_back(0);
                        }
                    }
                }
                else
                {
                    if (xmax_zmax)
                    {
                        out.push_back(5);
                    }
                    out.push_back(7);

                    if (xz_yz && ymax_zmax)
                    {
                        out.push_back(3);
                        if (xmin_ymax)
                        {
                            out.push_back(2);
                        }
                    }
                    else if (xz_xy && xmax_ymax)
                    {
                        out.push_back(6);
                        if (ymax_zmin)
                        {
                            out.push_back(2);
                        }
                    }
                }
            }
        }
        else if (xy_)
        {
            if (xmin_ymin_out)
            {
                if (dir.z() < 0.0)
                {
                    if (xmin_ymin)
                    {
                        out.push_back(1);
                    }
                    out.push_back(0);

                    if (xy_xz && xmin_zmin)
                    {
                        out.push_back(2);
                        if (ymax_zmin)
                        {
                            out.push_back(6);
                        }
                    }
                    else if (xy_yz && ymin_zmin)
                    {
                        out.push_back(4);
                        if (xmax_zmin)
                        {
                            out.push_back(6);
                        }
                    }
                }
                else
                {
                    if (xmin_ymin)
                    {
                        out.push_back(0);
                    }
                    out.push_back(1);

                    if (xy_xz && xmin_zmax)
                    {
                        out.push_back(3);
                        if (ymax_zmax)
                        {
                            out.push_back(7);
                        }
                    }
                    else if (xy_yz && ymin_zmax)
                    {
                        out.push_back(5);
                        if (xmax_zmax)
                        {
                            out.push_back(7);
                        }
                    }
                }
            }
            else if (xmax_ymin_out)
            {
                if (dir.z() < 0.0)
                {
                    if (xmax_ymin)
                    {
                        out.push_back(5);
                    }
                    out.push_back(4);

                    if (xy_xz && xmax_zmin)
                    {
                        out.push_back(6);
                        if (ymax_zmin)
                        {
                            out.push_back(2);
                        }
                    }
                    else if (xy_yz && ymin_zmin)
                    {
                        out.push_back(0);
                        if (xmin_zmin)
                        {
                            out.push_back(2);
                        }
                    }
                }
                else
                {
                    if (xmax_ymin)
                    {
                        out.push_back(4);
                    }
                    out.push_back(5);

                    if (xy_xz && xmax_zmax)
                    {
                        out.push_back(7);
                        if (ymax_zmax)
                        {
                            out.push_back(3);
                        }
                    }
                    else if (xy_yz && ymin_zmax)
                    {
                        out.push_back(1);
                        if (xmin_zmax)
                        {
                            out.push_back(3);
                        }
                    }
                }
            }
            else if (xmin_ymax_out)
            {
                if (dir.z() < 0.0)
                {
                    if (xmin_ymax)
                    {
                        out.push_back(3);
                    }
                    out.push_back(2);

                    if (xy_xz && xmin_zmin)
                    {
                        out.push_back(0);
                        if (ymin_zmin)
                        {
                            out.push_back(4);
                        }
                    }
                    else if (xy_yz && ymax_zmin)
                    {
                        out.push_back(6);
                        if (xmax_zmin)
                        {
                            out.push_back(4);
                        }
                    }
                }
                else
                {
                    if (xmin_ymax)
                    {
                        out.push_back(2);
                    }
                    out.push_back(3);

                    if (xy_xz && xmin_zmax)
                    {
                        out.push_back(1);
                        if (ymin_zmax)
                        {
                            out.push_back(5);
                        }
                    }
                    else if (xy_yz && ymax_zmax)
                    {
                        out.push_back(7);
                        if (xmax_zmax)
                        {
                            out.push_back(5);
                        }
                    }
                }
            }
            else if (xmax_ymax_out)
            {
                if (dir.z() < 0.0)
                {
                    if (xmax_ymax)
                    {
                        out.push_back(7);
                    }
                    out.push_back(6);

                    if (xy_xz && xmax_zmin)
                    {
                        out.push_back(4);
                        if (ymin_zmin)
                        {
                            out.push_back(0);
                        }
                    }
                    else if (xy_yz && ymax_zmin)
                    {
                        out.push_back(2);
                        if (xmin_zmin)
                        {
                            out.push_back(0);
                        }
                    }
                }
                else
                {
                    if (xmax_ymax)
                    {
                        out.push_back(6);
                    }
                    out.push_back(7);

                    if (xy_xz && xmax_zmax)
                    {
                        out.push_back(5);
                        if (ymin_zmax)
                        {
                            out.push_back(1);
                        }
                    }
                    else if (xy_yz && ymax_zmax)
                    {
                        out.push_back(3);
                        if (xmin_zmax)
                        {
                            out.push_back(1);
                        }
                    }
                }
            }
        }
        else
        {
            if (dir.x() <= 0.0 && dir.y() <= 0.0 && dir.z() <= 0.0)
            {
                const uint_fast8_t temp[vec4<T>::sub_size()] = {7, 6, 3, 2, 5, 4, 1, 0};
                out = temp;
            }
            else if (dir.x() <= 0.0 && dir.y() <= 0.0 && dir.z() > 0.0)
            {
                const uint_fast8_t temp[vec4<T>::sub_size()] = {6, 2, 7, 3, 4, 0, 5, 1};
                out = temp;
            }
            else if (dir.x() <= 0.0 && dir.y() > 0.0 && dir.z() <= 0.0)
            {
                const uint_fast8_t temp[vec4<T>::sub_size()] = {5, 4, 1, 0, 7, 6, 3, 2};
                out = temp;
            }
            else if (dir.x() <= 0.0 && dir.y() > 0.0 && dir.z() > 0.0)
            {
                const uint_fast8_t temp[vec4<T>::sub_size()] = {4, 0, 5, 1, 6, 2, 7, 3};
                out = temp;
            }
            else if (dir.x() > 0.0 && dir.y() <= 0.0 && dir.z() <= 0.0)
            {
                const uint_fast8_t temp[vec4<T>::sub_size()] = {3, 7, 2, 6, 1, 5, 0, 4};
                out = temp;
            }
            else if (dir.x() > 0.0 && dir.y() <= 0.0 && dir.z() > 0.0)
            {
                const uint_fast8_t temp[vec4<T>::sub_size()] = {2, 3, 6, 7, 0, 1, 4, 5};
                out = temp;
            }
            else if (dir.x() > 0.0 && dir.y() > 0.0 && dir.z() <= 0.0)
            {
                const uint_fast8_t temp[vec4<T>::sub_size()] = {1, 5, 0, 4, 3, 7, 2, 6};
                out = temp;
            }
            else if (dir.x() > 0.0 && dir.y() > 0.0 && dir.z() > 0.0)
            {
                const uint_fast8_t temp[vec4<T>::sub_size()] = {0, 1, 4, 5, 2, 3, 6, 7};
                out = temp;
            }
        }

        return out;
    }
    inline static auto subdivide_overlap(const vec4<T> &min, const vec4<T> &max, const vec4<T> &center)
    {
        min::stack_vector<uint_fast8_t, vec4<T>::sub_size()> out;

        const bool minx = min.x() <= center.x();
        const bool miny = min.y() <= center.y();
        const bool minz = min.z() <= center.z();
        const bool maxx = max.x() >= center.x();
        const bool maxy = max.y() >= center.y();
        const bool maxz = max.z() >= center.z();

        // If overlapping 0,1,2,3 cells
        if (minx)
        {
            // If overlapping 0,1
            if (miny)
            {
                // If overlapping 0
                if (minz)
                {
                    out.push_back(0);
                    // If overlapping 1
                    if (maxz)
                    {
                        out.push_back(1);
                    }
                }
                else
                {
                    // Must be overlapping 1
                    out.push_back(1);
                }

                // If overlapping 2,3
                if (maxy)
                {
                    if (minz)
                    {
                        out.push_back(2);
                        // If overlapping 3
                        if (maxz)
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
            }
            else
            {
                if (minz)
                {
                    out.push_back(2);
                    // If overlapping 3
                    if (maxz)
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
        }

        // If overlapping 4,5,6,7 cells
        if (maxx)
        {
            // If overlapping 4,5
            if (miny)
            {
                // If overlapping 4
                if (minz)
                {
                    out.push_back(4);
                    // If overlapping 5
                    if (maxz)
                    {
                        out.push_back(5);
                    }
                }
                else
                {
                    // Must be overlapping 1
                    out.push_back(5);
                }

                // If overlapping 6,7
                if (maxy)
                {
                    if (minz)
                    {
                        out.push_back(6);
                        // If overlapping 7
                        if (maxz)
                        {
                            out.push_back(7);
                        }
                    }
                    else
                    {
                        // Must be overlapping 7
                        out.push_back(7);
                    }
                }
            }
            else
            {
                if (minz)
                {
                    out.push_back(6);
                    // If overlapping 7
                    if (maxz)
                    {
                        out.push_back(7);
                    }
                }
                else
                {
                    // Must be overlapping 7
                    out.push_back(7);
                }
            }
        }

        return out;
    }
    inline constexpr static T unit_length()
    {
        return var<T>::SQRT3;
    }
    inline constexpr static T inverse_unit_length()
    {
        return var<T>::INV_SQRT3;
    }
    inline constexpr static vec4<T> up()
    {
        return vec4<T>(0.0, 1.0, 0.0, 1.0);
    }
    inline bool within(const vec4<T> &min, const vec4<T> &max) const
    {
        // Return true if this vector is within the min and max vector range
        return (_x >= min.x() + var<T>::TOL_REL && _x <= max.x() - var<T>::TOL_REL && _y >= min.y() + var<T>::TOL_REL && _y <= max.y() - var<T>::TOL_REL && _z >= min.z() + var<T>::TOL_REL && _z <= max.z() - var<T>::TOL_REL);
    }
    inline vec4<T> &operator+=(const T a)
    {
        _x += a;
        _y += a;
        _z += a;
        return *this;
    }
    inline vec4<T> &operator+=(const vec4<T> &A)
    {
        _x += A.x();
        _y += A.y();
        _z += A.z();
        return *this;
    }
    inline vec4<T> &operator-=(const T a)
    {
        _x -= a;
        _y -= a;
        _z -= a;
        return *this;
    }
    inline vec4<T> &operator-=(const vec4<T> &A)
    {
        _x -= A.x();
        _y -= A.y();
        _z -= A.z();
        return *this;
    }
    inline vec4<T> &operator*=(const T a)
    {
        _x *= a;
        _y *= a;
        _z *= a;
        return *this;
    }
    inline vec4<T> &operator*=(const vec4<T> &A)
    {
        _x *= A.x();
        _y *= A.y();
        _z *= A.z();
        return *this;
    }
    inline vec4<T> &operator/=(const T a)
    {
        _x /= a;
        _y /= a;
        _z /= a;
        return *this;
    }
    inline vec4<T> &operator/=(const vec4<T> &A)
    {
        _x /= A.x();
        _y /= A.y();
        _z /= A.z();
        return *this;
    }
    inline vec4<T> operator+(const T a) const
    {
        vec4<T> temp = *this;
        temp += a;
        return temp;
    }
    inline vec4<T> operator+(const vec4<T> &A) const
    {
        vec4<T> temp = *this;
        temp += A;
        return temp;
    }
    inline vec4<T> operator-(const T a) const
    {
        vec4<T> temp = *this;
        temp -= a;
        return temp;
    }
    inline vec4<T> operator-(const vec4<T> &A) const
    {
        vec4<T> temp = *this;
        temp -= A;
        return temp;
    }
    inline vec4<T> operator*(const T a) const
    {
        vec4<T> temp = *this;
        temp *= a;
        return temp;
    }
    inline vec4<T> operator*(const vec4<T> &A) const
    {
        vec4<T> temp = *this;
        temp *= A;
        return temp;
    }
    inline vec4<T> operator/(const T a) const
    {
        vec4<T> temp = *this;
        temp /= a;
        return temp;
    }
    inline vec4<T> operator/(const vec4<T> &A) const
    {
        vec4<T> temp = *this;
        temp /= A;
        return temp;
    }
    inline bool operator>(const vec4<T> &A) const
    {
        return _x > A._x && _y > A._y && _z > A._z;
    }
    inline bool operator>=(const vec4<T> &A) const
    {
        return _x >= A._x && _y >= A._y && _z >= A._z;
    }
    inline bool operator<(const vec4<T> &A) const
    {
        return _x < A._x && _y < A._y && _z < A._z;
    }
    inline bool operator<=(const vec4<T> &A) const
    {
        return _x <= A._x && _y <= A._y && _z <= A._z;
    }
};
}

#endif
