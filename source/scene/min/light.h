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
#ifndef __LIGHT__
#define __LIGHT__

#include <min/vec4.h>

namespace min
{

template <typename T>
class light
{
  private:
    vec4<T> _color;
    vec4<T> _position;
    vec4<T> _power;

  public:
    light() {}
    light(const vec4<T> &color, const vec4<T> &position, const vec4<T> &power) : _color(color), _position(position), _power(power) {}
    inline const vec4<T> &get_color() const
    {
        return _color;
    }
    inline const vec4<T> &get_position() const
    {
        return _position;
    }
    inline const vec4<T> &get_power() const
    {
        return _power;
    }
    inline void set_color(const vec4<T> &color)
    {
        _color = color;
    }
    inline void set_position(const vec4<T> &p)
    {
        _position = p;
    }
    inline void set_power(const vec4<T> &power)
    {
        _power = power;
    }
};
}

#endif
