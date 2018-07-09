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
#ifndef __STACK_VECTOR__
#define __STACK_VECTOR__

#include <cstddef>

namespace min
{

template <typename T, size_t N>
class stack_vector
{
  private:
    T _vec[N];
    size_t _end;

  public:
    stack_vector() : _end(0) {}
    stack_vector(const T (&arr)[N]) : _end(N)
    {
        for (size_t i = 0; i < N; i++)
        {
            _vec[i] = arr[i];
        }
    }
    inline stack_vector<T, N> &operator=(const T (&arr)[N])
    {
        // Set the array values
        for (size_t i = 0; i < N; i++)
        {
            _vec[i] = arr[i];
        }

        // Set the end of array
        _end = N;

        return *this;
    }
    inline T operator[](const size_t index) const
    {
        return _vec[index];
    }
    inline T &operator[](const size_t index)
    {
        return _vec[index];
    }
    inline void clear()
    {
        _end = 0;
    }
    const T *begin() const
    {
        return &_vec[0];
    }
    const T *end() const
    {
        return &_vec[_end];
    }
    inline void push_back(const T &v)
    {
        _vec[_end++] = v;
    }
    inline size_t size() const
    {
        return _end;
    }
};
}

#endif
