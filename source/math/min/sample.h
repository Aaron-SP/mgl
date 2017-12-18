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
#ifndef __SAMPLE__
#define __SAMPLE__

namespace min
{

template <typename T, template <typename> class vec>
class sample
{
  private:
    vec<T> _src;
    vec<T> _dst;
    T _t;
    T _weight;

  public:
    sample() : _t(0.0), _weight(1.0) {}
    sample(const vec<T> &src, const vec<T> &dst) : _src(src), _dst(dst), _t(0.0), _weight(1.0) {}
    sample(const vec<T> &src, const vec<T> &dst, const T weight) : _src(src), _dst(dst), _t(0.0), _weight(weight) {}
    bool done() const
    {
        return _t > 1.0;
    }
    const vec<T> &get_start() const
    {
        return _src;
    }
    const vec<T> &get_dest() const
    {
        return _dst;
    }
    vec<T> interpolate(const T dt)
    {
        // Adds dt to the current time and interpolates from _src to _dst
        // values of _t > 1.0 are valid
        _t += dt;

        // Calls interpolate which is the slower, more accurate interpolation mechanism of 'vec'
        return vec<T>::interpolate(_src, _dst, _t);
    }
    vec<T> weight_interpolate(const T dt)
    {
        // Adds dt to the current time and interpolates from _src to _dst
        // values of _t > 1.0 are valid
        _t += _weight * dt;

        // Calls interpolate which is the slower, more accurate interpolation mechanism of 'vec'
        return vec<T>::interpolate(_src, _dst, _t);
    }
    vec<T> lerp(const T dt)
    {
        // Adds dt to the current time and lerps from _src to _dst
        // values of _t > 1.0 are valid
        _t += dt;

        // Calls strictly lerp on the type of 'vec'
        return vec<T>::lerp(_src, _dst, _t);
    }
    vec<T> weight_lerp(const T dt)
    {
        // Adds dt to the current time and lerps from _src to _dst
        // values of _t > 1.0 are valid
        _t += _weight * dt;

        // Calls strictly lerp on the type of 'vec'
        return vec<T>::lerp(_src, _dst, _t);
    }
    void reset()
    {
        _t = 0.0;
    }
};
}

#endif
