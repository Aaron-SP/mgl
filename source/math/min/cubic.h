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
#ifndef _MGL_CUBIC_MGL_
#define _MGL_CUBIC_MGL_

namespace min
{

template <typename T, template <typename> class vec>
class bezier
{
  private:
    vec<T> _p0;
    vec<T> _p1;
    vec<T> _p2;
    vec<T> _p3;

  public:
    bezier() {}
    bezier(const vec<T> &p0, const vec<T> &p1, const vec<T> &p2, const vec<T> &p3)
        : _p0(p0), _p1(p1), _p2(p2), _p3(p3) {}

    // Assumes t is between domain [0.0f, 1.0f]
    inline vec<T> interpolate(const T t) const
    {
        // Precalculate time constants
        const T t2 = t * t;
        const T t3 = t2 * t;
        const T u = 1.0f - t;
        const T u2 = u * u;
        const T u3 = u2 * u;

        // Calculate blending functions
        const T b0 = u3;
        const T b1 = 3.0f * u2 * t;
        const T b2 = 3.0f * u * t2;
        const T b3 = t3;

        // return interpolated point on bezier curve
        return (_p0 * b0) + (_p1 * b1) + (_p2 * b2) + (_p3 * b3);
    }
    inline const min::vec3<float> &begin() const
    {
        return _p0;
    }
    inline const min::vec3<float> &end() const
    {
        return _p3;
    }
};

template <typename T, template <typename> class vec>
class bezier_deriv
{
  private:
    vec<T> _p0;
    vec<T> _p1;
    vec<T> _p2;
    vec<T> _p3;

  public:
    bezier_deriv() {}
    bezier_deriv(const vec<T> &p0, const vec<T> &p1, const vec<T> &p2, const vec<T> &p3)
        : _p0(p0), _p1(p1), _p2(p2), _p3(p3) {}

    // Assumes t is between domain [0.0f, 1.0f]
    inline vec<T> interpolate(const T t) const
    {
        // Precalculate time constants
        const T t2 = t * t;
        const T u = 1.0f - t;
        const T u2 = u * u;

        // Calculate blending functions
        const T b0 = -3.0f * u2;
        const T b1 = 3.0f - (12.0f * t) + (9.0f * t2);
        const T b2 = (6.0f * t) - (9.0f * t2);
        const T b3 = 3.0f * t2;

        // return interpolated point on bezier_deriv curve
        return (_p0 * b0) + (_p1 * b1) + (_p2 * b2) + (_p3 * b3);
    }
    inline const min::vec3<float> &begin() const
    {
        return _p0;
    }
    inline const min::vec3<float> &end() const
    {
        return _p3;
    }
};

template <typename T, template <typename> class vec>
class bspline
{
  private:
    vec<T> _p0;
    vec<T> _p1;
    vec<T> _p2;
    vec<T> _p3;

  public:
    bspline() {}
    // Points p1 & p2 define the curve endpoints; p0 and p3 are control points
    bspline(const vec<T> &p0, const vec<T> &p1, const vec<T> &p2, const vec<T> &p3)
        : _p0(p0), _p1(p1), _p2(p2), _p3(p3) {}

    // Assumes t is between domain [0.0f, 1.0f]
    inline vec<T> interpolate(const T t) const
    {
        // Precalculate time constants
        const T t2 = t * t;
        const T t3 = t2 * t;
        const T u = 1.0f - t;
        const T u3 = u * u * u;

        // Calculate blending functions
        const T b0 = u3;
        const T b1 = 4.0f - (6.0f * t2) + (3.0f * t3);
        const T b2 = 1.0f + (3.0f * t) + (3.0f * t2) - (3.0f * t3);
        const T b3 = t3;

        // return interpolated point on bspline curve
        return ((_p0 * b0) + (_p1 * b1) + (_p2 * b2) + (_p3 * b3)) * 0.166667;
    }
    inline const min::vec3<float> &begin() const
    {
        return _p1;
    }
    inline const min::vec3<float> &end() const
    {
        return _p2;
    }
};

template <typename T, template <typename> class vec>
class bspline_deriv
{
  private:
    vec<T> _p0;
    vec<T> _p1;
    vec<T> _p2;
    vec<T> _p3;

  public:
    bspline_deriv() {}
    // Points p1 & p2 define the curve endpoints; p0 and p3 are control points
    bspline_deriv(const vec<T> &p0, const vec<T> &p1, const vec<T> &p2, const vec<T> &p3)
        : _p0(p0), _p1(p1), _p2(p2), _p3(p3) {}

    // Assumes t is between domain [0.0f, 1.0f]
    inline vec<T> interpolate(const T t) const
    {
        // Precalculate time constants
        const T t2 = t * t;
        const T u = 1.0f - t;
        const T u2 = u * u;

        // Calculate blending functions
        const T b0 = -0.5f * u2;
        const T b1 = (-2.0f * t) + (1.5f * t2);
        const T b2 = 0.5f + t - (1.5f * t2);
        const T b3 = 0.5f * t2;

        // return interpolated point on bspline_deriv curve
        return ((_p0 * b0) + (_p1 * b1) + (_p2 * b2) + (_p3 * b3));
    }
    inline const min::vec3<float> &begin() const
    {
        return _p1;
    }
    inline const min::vec3<float> &end() const
    {
        return _p2;
    }
};

template <typename T, template <typename> class vec>
class hermite
{
  private:
    vec<T> _p0;
    vec<T> _p1;
    vec<T> _t0;
    vec<T> _t1;

  public:
    hermite() {}
    // Points p0 & p1 and tangents at those points t0, t1
    hermite(const vec<T> &p0, const vec<T> &p1, const vec<T> &t0, const vec<T> &t1)
        : _p0(p0), _p1(p1), _t0(t0), _t1(t1) {}

    // Assumes t is between domain [0.0f, 1.0f]
    inline vec<T> interpolate(const T t) const
    {
        // Precalculate time constants
        const T t2 = t * t;
        const T t3 = t2 * t;
        const T u = 1.0f - t;
        const T u2 = u * u;

        // Calculate blending functions
        const T b0 = 1.0f - (3.0f * t2) + (2.0f * t3);
        const T b1 = t2 * (3.0f - (2.0f * t));
        const T b2 = t * u2;
        const T b3 = t2 * u;

        // return interpolated point on hermite curve
        return (_p0 * b0) + (_p1 * b1) + (_t0 * b2) + (_t1 * b3);
    }
    inline const min::vec3<float> &begin() const
    {
        return _p0;
    }
    inline const min::vec3<float> &end() const
    {
        return _p1;
    }
};

template <typename T, template <typename> class vec>
class hermite_deriv
{
  private:
    vec<T> _p0;
    vec<T> _p1;
    vec<T> _t0;
    vec<T> _t1;

  public:
    hermite_deriv() {}
    // Points p0 & p1 and tangents at those points t0, t1
    hermite_deriv(const vec<T> &p0, const vec<T> &p1, const vec<T> &t0, const vec<T> &t1)
        : _p0(p0), _p1(p1), _t0(t0), _t1(t1) {}

    // Assumes t is between domain [0.0f, 1.0f]
    inline vec<T> interpolate(const T t) const
    {
        // Precalculate time constants
        const T t2 = t * t;
        const T u = 1.0f - t;

        // Calculate blending functions
        const T b0 = (-6.0f * t) + (6.0f * t2);
        const T b1 = 6.0f * t * u;
        const T b2 = 1.0f - (4.0f * t) + (3.0f * t2);
        const T b3 = (2.0f * t) - (3.0f * t2);

        // return interpolated point on hermite_deriv curve
        return (_p0 * b0) + (_p1 * b1) + (_t0 * b2) + (_t1 * b3);
    }
    inline const min::vec3<float> &begin() const
    {
        return _p0;
    }
    inline const min::vec3<float> &end() const
    {
        return _p1;
    }
};
}

#endif
