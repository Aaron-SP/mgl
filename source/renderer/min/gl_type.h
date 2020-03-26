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
#ifndef _MGL_GL_TYPE_MGL_
#define _MGL_GL_TYPE_MGL_

#include <min/window.h>

namespace min
{
template <class Tenum>
constexpr inline GLenum FLOAT_TYPE();

template <>
constexpr inline GLenum FLOAT_TYPE<float>()
{
    return GL_FLOAT;
}

#if !__EMSCRIPTEN__
template <>
constexpr inline GLenum FLOAT_TYPE<double>()
{
    return GL_DOUBLE;
}
#endif

template <class Tenum>
constexpr inline GLenum INT_TYPE();

template <>
constexpr inline GLenum INT_TYPE<uint8_t>()
{
    return GL_UNSIGNED_BYTE;
}
template <>
constexpr inline GLenum INT_TYPE<uint16_t>()
{
    return GL_UNSIGNED_SHORT;
}
template <>
constexpr inline GLenum INT_TYPE<uint32_t>()
{
    return GL_UNSIGNED_INT;
}
}

#endif
