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
#ifndef __STREAMVERTEX__
#define __STREAMVERTEX__

#include <min/static_vertex.h>

namespace min
{

template <typename T, typename K, GLenum FLOAT_TYPE>
class stream_vertex : public static_vertex<T, K, FLOAT_TYPE>
{
  public:
    inline static constexpr GLenum buffer_type()
    {
        return GL_STREAM_DRAW;
    }
};
}

#endif
