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
#ifndef __MGL_STACK_VECTOR__
#define __MGL_STACK_VECTOR__

#include <cstddef>
#include <cstring>
#include <utility>

namespace min
{

template <typename T, size_t N>
class stack_vector
{
  private:
    T _vec[N];
    size_t _end;

    template <size_t S>
    inline void const_copy(const T (&arr)[S])
    {
        static_assert(S <= N, "stack_vector<T, N> operator= overflow");
        constexpr size_t bytes = S * sizeof(T);
        std::memcpy(_vec, arr, bytes);
    }

  public:
    stack_vector() : _end(0) {}
    template <size_t S>
    stack_vector(const T (&arr)[S]) : _end(S)
    {
        const_copy(arr);
    }
    template <size_t S>
    inline stack_vector<T, N> &operator=(const T (&arr)[S])
    {
        _end = S;
        const_copy(arr);
        return *this;
    }
    inline const T &operator[](const size_t index) const
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
    inline const T *begin() const
    {
        return &_vec[0];
    }
    inline T *begin()
    {
        return &_vec[0];
    }
    inline const T *data() const
    {
        return &_vec[0];
    }
    inline T *data()
    {
        return &_vec[0];
    }
    inline const T *end() const
    {
        return &_vec[_end];
    }
    inline T *end()
    {
        return &_vec[_end];
    }
    template <typename... Ts>
    inline void emplace_back(Ts &&... args)
    {
        _vec[_end++] = T(std::forward<Ts>(args)...);
    }
    inline void fill(const T value)
    {
        for (size_t i = 0; i < _end; i++)
        {
            _vec[i] = value;
        }
    }
    inline void push_back(const T &v)
    {
        _vec[_end++] = v;
    }
    inline size_t size() const
    {
        return _end;
    }
    inline void zero()
    {
        const size_t bytes = this->size() * sizeof(T);
        std::memset(&_vec[0], 0, bytes);
    }
};
}

#endif
