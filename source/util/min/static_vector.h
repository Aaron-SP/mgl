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
#ifndef __MGL_STATIC_VECTOR__
#define __MGL_STATIC_VECTOR__

#include <cstddef>

namespace min
{

template <typename T>
class static_vector
{
  private:
    T *_beg;
    T *_end;

    inline void alloc(const size_t size)
    {
        _beg = new T[size];
        _end = _beg + size;
    }
    inline void const_copy(const static_vector<T> &sv)
    {
        const size_t size = sv.size();
        for (size_t i = 0; i < size; i++)
        {
            _beg[i] = sv._beg[i];
        }
    }
    inline void dealloc()
    {
        if (_beg)
        {
            delete[] _beg;
            _beg = nullptr;
            _end = nullptr;
        }
    }

  public:
    static_vector()
        : _beg(nullptr), _end(nullptr) {}
    static_vector(const size_t size)
        : _beg(nullptr), _end(nullptr)
    {
        alloc(size);
    }
    ~static_vector()
    {
        dealloc();
    }
    static_vector(const static_vector<T> &sv)
        : _beg(nullptr), _end(nullptr)
    {
        const size_t size = sv.size();
        if (size > 0)
        {
            alloc(size);
            const_copy(sv);
        }
    }
    static_vector(static_vector<T> &&sv) : _beg(sv._beg), _end(sv._end)
    {
        sv._beg = nullptr;
        sv._end = nullptr;
    }
    inline static_vector<T> &operator=(const static_vector<T> &sv)
    {
        if (this != &sv)
        {
            const size_t size = sv.size();
            if (size > 0)
            {
                resize(size);
                const_copy(sv);
            }
        }
    }
    template <size_t N>
    inline static_vector<T> &operator=(const T (&arr)[N])
    {
        // Set the array values
        resize(N);
        for (size_t i = 0; i < N; i++)
        {
            _beg[i] = arr[i];
        }

        return *this;
    }
    inline static_vector<T> &operator=(static_vector<T> &&sv)
    {
        _beg = sv._beg;
        _end = sv._end;
        sv._beg = nullptr;
        sv._end = nullptr;

        return *this;
    }
    inline const T &operator[](const size_t index) const
    {
        return _beg[index];
    }
    inline T &operator[](const size_t index)
    {
        return _beg[index];
    }
    inline const T *begin() const
    {
        return _beg;
    }
    inline const T *data() const
    {
        return _beg;
    }
    inline T *data()
    {
        return _beg;
    }
    inline const T *end() const
    {
        return _end;
    }
    inline void resize(const size_t size)
    {
        // Only allocate if need to grow
        if (size > this->size())
        {
            dealloc();
            alloc(size);
        }
        else
        {
            _end = _beg + size;
        }
    }
    inline size_t size() const
    {
        return _end - _beg;
    }
};
}

#endif
