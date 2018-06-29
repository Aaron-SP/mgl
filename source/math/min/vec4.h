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
#ifndef __VECTOR4__
#define __VECTOR4__

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
        if (std::abs(p.x()) <= var<T>::TOL_REL)
        {
            if (_x < min.x() || _x > max.x())
                return true;
        }
        else if (std::abs(p.y()) <= var<T>::TOL_REL)
        {
            if (_y < min.y() || _y > max.y())
                return true;
        }
        else if (std::abs(p.z()) <= var<T>::TOL_REL)
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
    inline static std::vector<std::pair<vec4<T>, vec4<T>>> grid(const vec4<T> &min, const vec4<T> &max, const size_t scale)
    {
        // Create out vector
        std::vector<std::pair<vec4<T>, vec4<T>>> out;
        out.reserve(scale * scale * scale);

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
                    out.emplace_back(std::make_pair(cell, cell + extent));
                }
            }
        }

        // return the compute grid
        return out;
    }
    inline static std::vector<std::pair<vec4<T>, T>> grid_center(const vec4<T> &min, const vec4<T> &max, const size_t scale, const T size)
    {
        // Create out vector
        std::vector<std::pair<vec4<T>, T>> out;
        out.reserve(scale * scale * scale);

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
                    out.emplace_back(std::make_pair(cell + half_extent, size));
                }
            }
        }

        // return the compute grid
        return out;
    }
    inline static std::tuple<size_t, size_t, size_t> grid_index(const vec4<T> &min, const vec4<T> &extent, const vec4<T> &point)
    {
        // Calculate the grid dimensions
        const T ex = extent.x();
        const T ey = extent.y();
        const T ez = extent.z();

        const size_t col = (point.x() - min.x()) / ex;
        const size_t row = (point.y() - min.y()) / ey;
        const size_t zin = (point.z() - min.z()) / ez;

        // Return the row / col of cell
        return std::make_tuple(col, row, zin);
    }
    inline static std::tuple<size_t, size_t, size_t> grid_index(const size_t index, const size_t scale)
    {
        // Precalculate the square scale
        const size_t scale2 = scale * scale;

        // Calculate row, col and height
        const size_t col = index / scale2;
        const size_t offset = index - col * scale2;
        const size_t row = offset / scale;
        const size_t zin = offset - (row * scale);

        // return tuple
        return std::make_tuple(col, row, zin);
    }
    inline static size_t grid_key(const vec4<T> &min, const vec4<T> &extent, const size_t scale, const vec4<T> &point)
    {
        // Calculate the cell location
        const std::tuple<size_t, size_t, size_t> index = grid_index(min, extent, point);

        // Get the row / col of cell
        const size_t col = std::get<0>(index);
        const size_t row = std::get<1>(index);
        const size_t zin = std::get<2>(index);

        // Return the grid index key for accessing cell
        return col * scale * scale + row * scale + zin;
    }
    inline static size_t grid_key(const std::tuple<size_t, size_t, size_t> &index, const size_t scale)
    {
        // Get the row / col of cell
        const size_t col = std::get<0>(index);
        const size_t row = std::get<1>(index);
        const size_t zin = std::get<2>(index);

        // Return the grid index key for accessing cell
        return col * scale * scale + row * scale + zin;
    }
    inline static void grid_overlap(std::vector<size_t> &out, const vec4<T> &min, const vec4<T> &extent, const size_t scale, const vec4<T> &b_min, const vec4<T> &b_max)
    {
        // Create the output vector
        out.clear();
        out.reserve(27);

        // Calculate the grid dimensions
        const T dx = extent.x();
        const T dy = extent.y();
        const T dz = extent.z();

        // Calculate the center cell
        const vec4<T> center = (b_min + b_max) * 0.5;

        // Center cell indices
        const std::tuple<size_t, size_t, size_t> t = vec4<T>::grid_index(min, extent, center);
        const size_t x = std::get<0>(t);
        const size_t y = std::get<1>(t);
        const size_t z = std::get<2>(t);
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
    }
    inline static std::tuple<int, T, T, int, T, T, int, T, T> grid_ray(const vec4<T> &extent, const vec4<T> &origin, const vec4<T> &dir, const vec4<T> &inv_dir)
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
        const T minx = ex * std::floor(x / ex);

        // Calculate distance to right of ray origin
        const T maxx = minx + ex;

        // Test for ray parallel to X axis
        T tx = std::numeric_limits<T>::max();
        T dtx = std::numeric_limits<T>::max();
        int drx = 1;
        if (std::abs(dir.x()) >= var<T>::TOL_ZERO)
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
        const T miny = ey * std::floor(y / ey);

        // Calculate distance to above ray origin
        const T maxy = miny + ey;

        // Test for ray parallel to Y axis
        T ty = std::numeric_limits<T>::max();
        T dty = std::numeric_limits<T>::max();
        int dry = 1;
        if (std::abs(dir.y()) >= var<T>::TOL_ZERO)
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
        const T minz = ez * std::floor(z / ez);

        // Calculate distance to above ray origin
        const T maxz = minz + ez;

        // Test for ray parallel to Z axis
        T tz = std::numeric_limits<T>::max();
        T dtz = std::numeric_limits<T>::max();
        int drz = 1;
        if (std::abs(dir.z()) >= var<T>::TOL_ZERO)
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
        return std::make_tuple(drx, tx, dtx, dry, ty, dty, drz, tz, dtz);
    }
    inline static size_t grid_ray_next(std::tuple<size_t, size_t, size_t> &index, std::tuple<int, T, T, int, T, T, int, T, T> &grid_ray, bool &flag, const size_t scale)
    {
        // Get the cell row / col
        size_t &col = std::get<0>(index);
        size_t &row = std::get<1>(index);
        size_t &zin = std::get<2>(index);

        // X
        const int &drx = std::get<0>(grid_ray);
        T &tx = std::get<1>(grid_ray);
        const T &dtx = std::get<2>(grid_ray);

        // Y
        const int &dry = std::get<3>(grid_ray);
        T &ty = std::get<4>(grid_ray);
        const T &dty = std::get<5>(grid_ray);

        // Z
        const int &drz = std::get<6>(grid_ray);
        T &tz = std::get<7>(grid_ray);
        const T &dtz = std::get<8>(grid_ray);

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
        const std::tuple<size_t, size_t, size_t> t_min = vec4<T>::grid_index(min, extent, over_min);
        const std::tuple<size_t, size_t, size_t> t_max = vec4<T>::grid_index(min, extent, over_max);

        // Get all cells in between points and get overlapping shapes
        for (size_t i = std::get<0>(t_min); i <= std::get<0>(t_max); i++)
        {
            for (size_t j = std::get<1>(t_min); j <= std::get<1>(t_max); j++)
            {
                for (size_t k = std::get<2>(t_min); k <= std::get<2>(t_max); k++)
                {
                    // Get the key for this index
                    const size_t key = vec4<T>::grid_key(std::make_tuple(i, j, k), scale);

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
        T x = this->dot(axis.x());

        // Clamp d onto the box half extent
        min::clamp<T>(x, -extent.x(), extent.x());

        // Project v onto local y axis
        T y = this->dot(axis.y());

        // Clamp d onto the box half extent
        min::clamp<T>(y, -extent.y(), extent.y());

        // Project v onto local z axis
        T z = this->dot(axis.z());

        // Clamp d onto the box half extent
        min::clamp<T>(z, -extent.z(), extent.z());

        // Compute the point along this axis
        return (axis.x() * x) + (axis.y() * y) + (axis.z() * z);
    }
    inline T project_length(const coord_sys<T, min::vec4> &axis, const vec4<T> &extent)
    {
        // Project this onto local x axis
        const T x = this->dot(axis.x());

        // Clamp x onto the box half extent, else zero
        const T dx = clamp_value<T>(x, -extent.x(), x + extent.x(), extent.x(), x - extent.x());

        // Project this onto local y axis
        const T y = this->dot(axis.y());

        // Clamp y onto the box half extent, else 0
        const T dy = clamp_value<T>(y, -extent.y(), y + extent.y(), extent.y(), y - extent.y());

        // Project this onto local y axis
        const T z = this->dot(axis.z());

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

        // Get the z portion of key
        key <<= 1;
        if (z > middle)
        {
            // Set the least significant bit
            key |= 0x1;
            z -= middle;
            _z = z;
        }

        return key;
    }
    inline static std::vector<std::pair<vec4<T>, vec4<T>>> subdivide(const vec4<T> &min, const vec4<T> &max)
    {
        std::vector<std::pair<vec4<T>, vec4<T>>> out;
        out.reserve(8);

        // Half extent of vector space
        const vec4<T> h = (max - min) * 0.5;

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
        const vec4<T> min0 = vec4<T>(cx_hx, cy_hy, cz_hz, 1.0);
        const vec4<T> max0 = vec4<T>(c.x(), c.y(), c.z(), 1.0);

        // Octant 1
        const vec4<T> min1 = vec4<T>(cx_hx, cy_hy, c.z(), 1.0);
        const vec4<T> max1 = vec4<T>(c.x(), c.y(), czhz, 1.0);

        // Octant 2
        const vec4<T> min2 = vec4<T>(cx_hx, c.y(), cz_hz, 1.0);
        const vec4<T> max2 = vec4<T>(c.x(), cyhy, c.z(), 1.0);

        // Octant 3
        const vec4<T> min3 = vec4<T>(cx_hx, c.y(), c.z(), 1.0);
        const vec4<T> max3 = vec4<T>(c.x(), cyhy, czhz, 1.0);

        // Octant 4
        const vec4<T> min4 = vec4<T>(c.x(), cy_hy, cz_hz, 1.0);
        const vec4<T> max4 = vec4<T>(cxhx, c.y(), c.z(), 1.0);

        // Octant 5
        const vec4<T> min5 = vec4<T>(c.x(), cy_hy, c.z(), 1.0);
        const vec4<T> max5 = vec4<T>(cxhx, c.y(), czhz, 1.0);

        // Octant 6
        const vec4<T> min6 = vec4<T>(c.x(), c.y(), cz_hz, 1.0);
        const vec4<T> max6 = vec4<T>(cxhx, cyhy, c.z(), 1.0);

        // Octant 7
        const vec4<T> min7 = vec4<T>(c.x(), c.y(), c.z(), 1.0);
        const vec4<T> max7 = vec4<T>(cxhx, cyhy, czhz, 1.0);

        // Add sub spaces to out vector
        out.push_back(std::make_pair(min0, max0));
        out.push_back(std::make_pair(min1, max1));
        out.push_back(std::make_pair(min2, max2));
        out.push_back(std::make_pair(min3, max3));
        out.push_back(std::make_pair(min4, max4));
        out.push_back(std::make_pair(min5, max5));
        out.push_back(std::make_pair(min6, max6));
        out.push_back(std::make_pair(min7, max7));

        return out;
    }
    inline static std::vector<std::pair<vec4<T>, T>> subdivide_center(const vec4<T> &min, const vec4<T> &max, const T size)
    {
        std::vector<std::pair<vec4<T>, T>> out;
        out.reserve(8);

        // Quarter extent of vector space
        const vec4<T> h = (max - min) * 0.25;

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

        // Add sub spaces to out vector
        out.push_back(std::make_pair(c0, size));
        out.push_back(std::make_pair(c1, size));
        out.push_back(std::make_pair(c2, size));
        out.push_back(std::make_pair(c3, size));
        out.push_back(std::make_pair(c4, size));
        out.push_back(std::make_pair(c5, size));
        out.push_back(std::make_pair(c6, size));
        out.push_back(std::make_pair(c7, size));

        return out;
    }
    // Plane n·x - c = 0
    // Ray x = P + td
    // If intersecting n · (P + td) - c = 0; x > 0.0
    // n · P + n · td - c = 0
    // t = (c - n · P) / (n · d)
    // Each axis is axis aligned so we can simplify to, where nx = ny = 1
    // tx = (cx - nx · Px) / (nx · dx)
    // ty = (cy - ny · Py) / (ny · dy)
    inline static void subdivide_ray(std::vector<size_t> &out, const vec4<T> &min, const vec4<T> &max, const vec4<T> &origin, const vec4<T> &dir, const vec4<T> &inv_dir)
    {
        // Reserve space for output
        out.clear();
        out.reserve(4);

        // Temporaries for holding the quadrants across intersecting plane, flag signals if we need to push_back
        size_t f, s;
        bool flag = false;

        // half extent of vector space
        const vec4<T> h = (max - min) * 0.5;

        // Center of the vector space
        const vec4<T> c = (max + min) * 0.5;

        // Calculate ray intersections among all axes
        const vec4<T> t = (c - origin) * inv_dir;
        const vec4<T> t_abs = vec4<T>(t).abs();

        const auto x_lamda = [&origin, &dir, &t, &c, &h, &f, &s, &flag]() {
            // Clear the flag
            flag = false;

            // Check that we are not parallel to y-axis
            if (t.x() >= 0.0 && std::abs(dir.x()) >= var<T>::TOL_ZERO)
            {
                // Calculate octant ranges
                const T cy_hy = c.y() - h.y();
                const T cyhy = c.y() + h.y();
                const T cz_hz = c.z() - h.z();
                const T czhz = c.z() + h.z();

                // Find y value at c.x() of intersection
                const T py = origin.y() + t.x() * dir.y();

                // Find z value at c.x() of intersection
                const T pz = origin.z() + t.x() * dir.z();

                // Check if we are crossing between 0-4 along y-axis
                if ((py > cy_hy && py < c.y()) && (pz > cz_hz && pz < c.z()))
                {
                    if (dir.x() < 0.0)
                    {
                        f = 4;
                        s = 0;
                        flag = true;
                    }
                    else
                    {
                        f = 0;
                        s = 4;
                        flag = true;
                    }
                }
                // Check if we are crossing between 1-5 along y-axis
                else if ((py >= c.y() && py < cyhy) && (pz > cz_hz && pz < c.z()))
                {
                    if (dir.x() < 0.0)
                    {
                        f = 5;
                        s = 1;
                        flag = true;
                    }
                    else
                    {
                        f = 1;
                        s = 5;
                        flag = true;
                    }
                }
                // Check if we are crossing between 2-6 along y-axis
                else if ((py > cy_hy && py < c.y()) && (pz >= c.z() && pz < czhz))
                {
                    if (dir.x() < 0.0)
                    {
                        f = 6;
                        s = 2;
                        flag = true;
                    }
                    else
                    {
                        f = 2;
                        s = 6;
                        flag = true;
                    }
                }
                // Check if we are crossing between 3-7 along y-axis
                else if ((py >= c.y() && py < cyhy) && (pz >= c.z() && pz < czhz))
                {
                    if (dir.x() < 0.0)
                    {
                        f = 7;
                        s = 3;
                        flag = true;
                    }
                    else
                    {
                        f = 3;
                        s = 7;
                        flag = true;
                    }
                }
            }
        };

        const auto y_lamda = [&origin, &dir, &t, &c, &h, &f, &s, &flag]() {
            // Clear the flag
            flag = false;

            // Check that we are not parallel to x-axis
            if (t.y() >= 0.0 && std::abs(dir.y()) >= var<T>::TOL_ZERO)
            {
                // Calculate octant ranges
                const T cx_hx = c.x() - h.x();
                const T cxhx = c.x() + h.x();
                const T cz_hz = c.z() - h.z();
                const T czhz = c.z() + h.z();

                // Find x value at c.y() of intersection
                const T px = origin.x() + t.y() * dir.x();

                // Find z value at c.y() of intersection
                const T pz = origin.z() + t.y() * dir.z();

                // Check if we are crossing between 0-1 along x-axis
                if ((px > cx_hx && px < c.x()) && (pz > cz_hz && pz < c.z()))
                {
                    if (dir.y() < 0.0)
                    {
                        f = 2;
                        s = 0;
                        flag = true;
                    }
                    else
                    {
                        f = 0;
                        s = 2;
                        flag = true;
                    }
                }
                // Check if we are crossing between 2-3 along x-axis
                else if ((px >= c.x() && px < cxhx) && (pz > cz_hz && pz < c.z()))
                {
                    if (dir.y() < 0.0)
                    {
                        f = 6;
                        s = 4;
                        flag = true;
                    }
                    else
                    {
                        f = 4;
                        s = 6;
                        flag = true;
                    }
                }
                // Check if we are crossing between 0-1 along x-axis
                else if ((px > cx_hx && px < c.x()) && (pz >= c.z() && pz < czhz))
                {
                    if (dir.y() < 0.0)
                    {
                        f = 3;
                        s = 1;
                        flag = true;
                    }
                    else
                    {
                        f = 1;
                        s = 3;
                        flag = true;
                    }
                }
                // Check if we are crossing between 2-3 along x-axis
                else if ((px >= c.x() && px < cxhx) && (pz >= c.z() && pz < czhz))
                {
                    if (dir.y() < 0.0)
                    {
                        f = 7;
                        s = 5;
                        flag = true;
                    }
                    else
                    {
                        f = 5;
                        s = 7;
                        flag = true;
                    }
                }
            }
        };

        const auto z_lamda = [&origin, &dir, &t, &c, &h, &f, &s, &flag]() {
            // Clear the flag
            flag = false;

            // Check that we are not parallel to x-axis
            if (t.z() >= 0.0 && std::abs(dir.z()) >= var<T>::TOL_ZERO)
            {
                // Calculate octant ranges
                const T cx_hx = c.x() - h.x();
                const T cxhx = c.x() + h.x();
                const T cy_hy = c.y() - h.y();
                const T cyhy = c.y() + h.y();

                // Find x value at c.z() of intersection
                const T px = origin.x() + t.z() * dir.x();

                // Find y value at c.z() of intersection
                const T py = origin.y() + t.z() * dir.y();

                // Check if we are crossing between 0-1 along x-axis
                if ((px > cx_hx && px < c.x()) && (py > cy_hy && py < c.y()))
                {
                    if (dir.z() < 0.0)
                    {
                        f = 1;
                        s = 0;
                        flag = true;
                    }
                    else
                    {
                        f = 0;
                        s = 1;
                        flag = true;
                    }
                }
                // Check if we are crossing between 2-3 along x-axis
                else if ((px >= c.x() && px < cxhx) && (py > cy_hy && py < c.y()))
                {
                    if (dir.z() < 0.0)
                    {
                        f = 5;
                        s = 4;
                        flag = true;
                    }
                    else
                    {
                        f = 4;
                        s = 5;
                        flag = true;
                    }
                }
                // Check if we are crossing between 0-1 along x-axis
                else if ((px > cx_hx && px < c.x()) && (py >= c.y() && py < cyhy))
                {
                    if (dir.z() < 0.0)
                    {
                        f = 3;
                        s = 2;
                        flag = true;
                    }
                    else
                    {
                        f = 2;
                        s = 3;
                        flag = true;
                    }
                }
                // Check if we are crossing between 2-3 along x-axis
                else if ((px >= c.x() && px < cxhx) && (py >= c.y() && py < cyhy))
                {
                    if (dir.z() < 0.0)
                    {
                        f = 7;
                        s = 6;
                        flag = true;
                    }
                    else
                    {
                        f = 6;
                        s = 7;
                        flag = true;
                    }
                }
            }
        };

        // Less than
        const bool xly = t_abs.x() < t_abs.y();
        const bool xlz = t_abs.x() < t_abs.z();
        const bool ylx = t_abs.y() < t_abs.x();
        const bool ylz = t_abs.y() < t_abs.z();
        const bool zlx = t_abs.z() < t_abs.x();
        const bool zly = t_abs.z() < t_abs.y();

        // Greater than
        const bool xgy = t_abs.x() > t_abs.y();
        const bool xgz = t_abs.x() > t_abs.z();
        const bool ygx = t_abs.y() > t_abs.x();
        const bool ygz = t_abs.y() > t_abs.z();
        const bool zgy = t_abs.z() > t_abs.y();
        const bool zgx = t_abs.z() > t_abs.x();

        //x is valid and x is first
        if (xly && xlz)
        {
            x_lamda();
            if (flag)
            {
                out.push_back(f);
                out.push_back(s);
            }
        }
        // y is first
        else if (ylx && ylz)
        {
            y_lamda();
            if (flag)
            {
                out.push_back(f);
                out.push_back(s);
            }
        }
        // z is first
        else if (zlx && zly)
        {
            z_lamda();
            if (flag)
            {
                out.push_back(f);
                out.push_back(s);
            }
        }

        // x is middle, zxy
        if (xlz && xgy)
        {
            x_lamda();
            if (flag)
            {
                out.push_back(s);
            }
        }
        // x is middle, yxz
        else if (xly && xgz)
        {
            x_lamda();
            if (flag)
            {
                out.push_back(s);
            }
        }
        // y is middle, zyx
        else if (ylz && ygx)
        {
            y_lamda();
            if (flag)
            {
                out.push_back(s);
            }
        }
        // y is middle, xyz
        else if (ylx && ygz)
        {
            y_lamda();
            if (flag)
            {
                out.push_back(s);
            }
        }
        // z is middle, yzx
        else if (zly && zgx)
        {
            z_lamda();
            if (flag)
            {
                out.push_back(s);
            }
        }
        // z is middle, xzy
        else if (zlx && zgy)
        {
            z_lamda();
            if (flag)
            {
                out.push_back(s);
            }
        }

        // x is valid and x is last
        if (t_abs.x() > t_abs.y() && t_abs.x() > t_abs.z())
        {
            x_lamda();
            if (flag)
            {
                out.push_back(s);
            }
        }
        // y is valid and y is last
        else if (t_abs.y() > t_abs.x() && t_abs.y() > t_abs.z())
        {
            y_lamda();
            if (flag)
            {
                out.push_back(s);
            }
        }
        // z is valid and z is last
        else if (t_abs.z() > t_abs.x() && t_abs.z() > t_abs.y())
        {
            z_lamda();
            if (flag)
            {
                out.push_back(s);
            }
        }
        // t.x() == t.y() == t.z() == 0.0
        else if (t_abs.x() < var<T>::TOL_ZERO && t_abs.y() < var<T>::TOL_ZERO && t_abs.z() < var<T>::TOL_ZERO)
        {
            out = {0, 1, 2, 3, 4, 5, 6, 7};
        }

        // If we didn't hit any planes, test if ray origin is within the cell
        if (out.size() == 0 && origin.within(min, max))
        {
            // Find the octant the the origin is in
            vec4<T> enter = vec4<T>(origin).clamp(min, max);

            // Calculate ratio between 0.0 and 1.0
            vec4<T> ratio = vec4<T>::ratio(min, max, enter);

            // Get the key from octant
            const uint_fast8_t key = ratio.subdivide_key(0.5);
            out.push_back(key);
        }
    }
    inline static void sub_overlap(std::vector<uint_fast8_t> &out, const vec4<T> &min, const vec4<T> &max, const vec4<T> &center)
    {
        // Reserve space for output
        out.clear();
        out.reserve(8);

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
