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
#ifndef _MGL_NEURAL_NET_MGL_
#define _MGL_NEURAL_NET_MGL_

#include <chrono>
#include <cmath>
#include <random>
#include <vector>

namespace min
{
// Maps range to 0, 1 for use with nnet
template <typename T>
class mapper
{
  private:
    T _min;
    T _max;
    T _dx;
    T _inv_dx;

  public:
    mapper(const T min, const T max) : _min(min), _max(max), _dx(max - min), _inv_dx(1.0 / _dx) {}
    inline T map(const T val) const
    {
        return (val - _min) * _inv_dx;
    }
    inline T unmap(const T val) const
    {
        return (val * _dx) + _min;
    }
};

template <typename T>
class net_rng
{
  private:
    std::uniform_real_distribution<T> _mut_dist;
    std::uniform_real_distribution<T> _ran_dist;
    std::uniform_int_distribution<unsigned> _int_dist;
    std::mt19937 _rgen;

  public:
    net_rng()
        : _mut_dist(-10.0, 10.0),
          _ran_dist(-1.0, 1.0),
          _int_dist(0, 100),
          _rgen(std::chrono::high_resolution_clock::now().time_since_epoch().count())
    {
    }
    net_rng(const std::uniform_real_distribution<T> &mut_dist,
            const std::uniform_real_distribution<T> &ran_dist,
            const std::uniform_int_distribution<unsigned> &int_dist)
        : _mut_dist(mut_dist),
          _ran_dist(ran_dist),
          _int_dist(int_dist),
          _rgen(std::chrono::high_resolution_clock::now().time_since_epoch().count())
    {
    }
    inline T mutation()
    {
        return _mut_dist(_rgen);
    }
    inline T random()
    {
        return _ran_dist(_rgen);
    }
    inline unsigned random_int()
    {
        return _int_dist(_rgen);
    }
    inline void reseed()
    {
        _rgen.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    }
};
}
#endif
