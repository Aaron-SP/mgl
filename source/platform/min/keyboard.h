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
#ifndef __KEYBOARD__
#define __KEYBOARD__

#include <stdexcept>
#include <string>
#include <unordered_map>

namespace min
{

template <typename K>
class key
{
  private:
    bool _is_down;
    void *_down_data;
    void *_down_data_pf;
    void *_up_data;
    void *_up_data_pf;
    void (*_key_down)(void *, K);
    void (*_key_down_pf)(void *, K);
    void (*_key_up)(void *, K);
    void (*_key_up_pf)(void *, K);

  public:
    key() : _is_down(false),
            _down_data(nullptr), _down_data_pf(nullptr),
            _up_data(nullptr), _up_data_pf(nullptr),
            _key_down(nullptr), _key_down_pf(nullptr),
            _key_up(nullptr), _key_up_pf(nullptr) {}

    void down(K step)
    {
        // Call the callback function, if not down previously and not per frame
        if (_key_down && !_is_down)
            _key_down(_down_data, step);

        _is_down = true;
    }
    void up(K step)
    {
        // Call the callback function, if not up previously and not per frame
        if (_key_up && _is_down)
            _key_up(_up_data, step);

        _is_down = false;
    }
    bool is_down() const
    {
        return _is_down;
    }
    void set_down(void (*on_down)(void *, K), void *data)
    {
        _down_data = data;
        _key_down = on_down;
    }
    void set_up(void (*on_up)(void *, K), void *data)
    {
        _up_data = data;
        _key_up = on_up;
    }
    void set_down_per_frame(void (*on_down)(void *, K), void *data)
    {
        _down_data_pf = data;
        _key_down_pf = on_down;
    }
    void set_up_per_frame(void (*on_up)(void *, K), void *data)
    {
        _up_data_pf = data;
        _key_up_pf = on_up;
    }
    void update(const K step) const
    {
        if (_key_down_pf && _is_down)
        {
            // Call down_per_frame callback function
            _key_down_pf(_down_data_pf, step);
        }
        else if (_key_up_pf && !_is_down)
        {
            // Call up_per_frame callback function
            _key_up_pf(_up_data_pf, step);
        }
    }
};

template <typename T, typename K>
class keyboard
{
  private:
    std::unordered_map<T, key<K>> _keys;
    bool _disable;

  public:
    keyboard() : _disable(false) {}
    void add(const T &code)
    {
        // Add key to the key map
        _keys.insert(std::make_pair(code, key<K>()));
    }
    void clear()
    {
        _keys.clear();
    }
    void disable()
    {
        _disable = true;
    }
    void enable()
    {
        _disable = false;
    }
    bool is_down(const T &code) const
    {
        // Lookup key in the map
        const auto &i = _keys.find(code);
        if (i != _keys.end())
        {
            return i->second.is_down();
        }

        // No key in the map
        return false;
    }
    void key_down(const T &code, const K step)
    {
        if (!_disable)
        {
            // Lookup key in the map
            const auto &i = _keys.find(code);
            if (i != _keys.end())
            {
                i->second.down(step);
            }
        }
    }
    void key_up(const T &code, const K step)
    {
        if (!_disable)
        {
            // Lookup key in the map
            const auto &i = _keys.find(code);
            if (i != _keys.end())
            {
                i->second.up(step);
            }
        }
    }
    void register_keydown(const T &code, void (*on_down)(void *, K), void *data)
    {
        // Lookup key in the map
        const auto &i = _keys.find(code);
        if (i != _keys.end())
        {
            i->second.set_down(on_down, data);
        }
        else
        {
            throw std::runtime_error("keyboard: keycode " + std::to_string(code) + " is not in the key map");
        }
    }
    void register_keyup(const T &code, void (*on_up)(void *, K), void *data)
    {
        // Lookup key in the map
        const auto &i = _keys.find(code);
        if (i != _keys.end())
        {
            i->second.set_up(on_up, data);
        }
        else
        {
            throw std::runtime_error("keyboard: keycode " + std::to_string(code) + " is not in the key map");
        }
    }
    void register_keydown_per_frame(const T &code, void (*on_down)(void *, K), void *data)
    {
        // Lookup key in the map
        const auto &i = _keys.find(code);
        if (i != _keys.end())
        {
            i->second.set_down_per_frame(on_down, data);
        }
        else
        {
            throw std::runtime_error("keyboard: keycode " + std::to_string(code) + " is not in the key map");
        }
    }
    void register_keyup_per_frame(const T &code, void (*on_up)(void *, K), void *data)
    {
        // Lookup key in the map
        const auto &i = _keys.find(code);
        if (i != _keys.end())
        {
            i->second.set_up_per_frame(on_up, data);
        }
        else
        {
            throw std::runtime_error("keyboard: keycode " + std::to_string(code) + " is not in the key map");
        }
    }
    void update(const K step)
    {
        if (!_disable)
        {
            // Update all keys per frame
            for (const auto &k : _keys)
            {
                k.second.update(step);
            }
        }
        else
        {
            // Set all keys up if down
            for (auto &k : _keys)
            {
                if (k.second.is_down())
                {
                    k.second.up(step);
                }
            }
        }
    }
};
}

#endif
