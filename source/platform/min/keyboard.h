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

#include <algorithm>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

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
    std::vector<T> _key_order;
    void *_override_data;
    void (*_override)(void *, T);
    bool _disable;

  public:
    keyboard() : _override_data(nullptr), _override(nullptr), _disable(false) {}
    void add(const T code)
    {
        // Add key to the key map
        _keys.insert(std::make_pair(code, key<K>()));
        _key_order.push_back(code);
    }
    void clear()
    {
        _keys.clear();
        _key_order.clear();
    }
    void disable()
    {
        _disable = true;
    }
    void enable()
    {
        _disable = false;
    }
    const std::vector<T> &get_active_keys() const
    {
        return _key_order;
    }
    const std::pair<T, key<K>> &get_key(const T code) const
    {
        const auto &i = _keys.find(code);
        if (i != _keys.end())
        {
            return i->second;
        }
        else
        {
            throw std::runtime_error("keyboard: keycode " + std::to_string(code) + " is not in the key map");
        }
    }
    bool is_down(const T code) const
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
    void key_down(const T code, const K step)
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
    void key_up(const T code, const K step)
    {
        if (_override)
        {
            _override(_override_data, code);
        }
        else if (!_disable)
        {
            // Lookup key in the map
            const auto &i = _keys.find(code);
            if (i != _keys.end())
            {
                i->second.up(step);
            }
        }
    }
    void register_keydown(const T code, void (*on_down)(void *, K), void *data)
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
    void register_keyup(const T code, void (*on_up)(void *, K), void *data)
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
    void register_keydown_per_frame(const T code, void (*on_down)(void *, K), void *data)
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
    void register_keyup_per_frame(const T code, void (*on_up)(void *, K), void *data)
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
    void register_override_keyup(void (*on_override)(void *, T), void *data)
    {
        _override_data = data;
        _override = on_override;
    }
    bool swap(const T one, const T two)
    {
        const bool in_set = _keys.find(two) != _keys.end();
        if (one != two && !in_set)
        {
            const auto &i = _keys.find(one);
            if (i != _keys.end())
            {
                // Register callbacks under new key
                _keys.insert(std::make_pair(two, i->second));

                // Remove old key code from the map
                _keys.erase(i);

                // Swap the vector key order
                const typename std::vector<T>::iterator j = std::find(_key_order.begin(), _key_order.end(), one);
                if (j != _key_order.end())
                {
                    *j = two;
                }
                else
                {
                    throw std::runtime_error("keyboard: keycode " + std::to_string(one) + " is not in the key order list");
                }

                // A swap happened
                return true;
            }
            else
            {
                throw std::runtime_error("keyboard: keycode " + std::to_string(one) + " is not in the key map");
            }
        }

        return false;
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
