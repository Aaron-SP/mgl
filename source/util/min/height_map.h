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
#ifndef _MGL_HEIGHT_MAP_MGL_
#define _MGL_HEIGHT_MAP_MGL_

#include <chrono>
#include <cmath>
#include <min/static_vector.h>
#include <random>
#include <stdexcept>

namespace min
{

template <typename T>
class height_map
{
  private:
    const size_t _size;
    min::static_vector<T> _map;
    std::uniform_real_distribution<T> _dist;
    const T _lower;
    const T _upper;

    inline void gaussian_blur_5x5()
    {
        // Gaussian blur kernel, 5x5
        // Sigma = 1
        // exp(-(x * x + y * y) / 2), normalized on [-2, 2] for x and y
        const T kernel[5] = {0.05449, 0.24420, 0.40262, 0.24420, 0.05449};

        // Image length and width
        const size_t end2 = _size - 2;

        // Create a copy of the image to populate map edges
        min::static_vector<T> copy(_map.size());
        copy.fill(0.0f);

        // X Dimensional blur
        {
            for (size_t i = 0; i < 2; i++)
            {
                for (size_t j = 0; j < _size; j++)
                {
                    // Summing convolution
                    for (size_t k = 0; k < 5; k++)
                    {
                        const size_t index = key(i + k, j);
                        copy[key(i, j)] += _map[index] * kernel[k];
                    }
                }
            }

            for (size_t i = 2; i < end2; i++)
            {
                // Starting position
                const size_t i2 = i - 2;
                for (size_t j = 0; j < _size; j++)
                {
                    // Summing convolution
                    for (size_t k = 0; k < 5; k++)
                    {
                        const size_t index = key(i2 + k, j);
                        copy[key(i, j)] += _map[index] * kernel[k];
                    }
                }
            }

            for (size_t i = end2; i < _size; i++)
            {
                // Starting position
                const size_t i4 = i - 4;
                for (size_t j = 0; j < _size; j++)
                {
                    // Summing convolution
                    for (size_t k = 0; k < 5; k++)
                    {
                        const size_t index = key(i4 + k, j);
                        copy[key(i, j)] += _map[index] * kernel[k];
                    }
                }
            }
        }

        // Y Dimensional blur
        {
            for (size_t i = 0; i < _size; i++)
            {
                for (size_t j = 0; j < 2; j++)
                {
                    // Reset the map value
                    _map[key(i, j)] = 0.0f;

                    // Summing convolution
                    for (size_t k = 0; k < 5; k++)
                    {
                        const size_t index = key(i, j + k);
                        _map[key(i, j)] += copy[index] * kernel[k];
                    }
                }
                for (size_t j = end2; j < _size; j++)
                {
                    // Starting position
                    const size_t j4 = j - 4;

                    // Reset the map value
                    _map[key(i, j)] = 0.0f;

                    // Summing convolution
                    for (size_t k = 0; k < 5; k++)
                    {
                        const size_t index = key(i, j4 + k);
                        _map[key(i, j)] += copy[index] * kernel[k];
                    }
                }
            }

            for (size_t i = 0; i < _size; i++)
            {
                for (size_t j = 2; j < end2; j++)
                {
                    // Starting position
                    const size_t j2 = j - 2;

                    // Reset the map value
                    _map[key(i, j)] = 0.0f;

                    // Summing convolution
                    for (size_t k = 0; k < 5; k++)
                    {
                        const size_t index = key(i, j2 + k);
                        _map[key(i, j)] += copy[index] * kernel[k];
                    }
                }
            }
        }
    }
    inline void generate(std::mt19937 &gen)
    {
        // Generate start indexes
        const size_t end = _size - 1;

        // Generate corner keys
        const size_t ll = key(0, 0);
        const size_t lr = key(end, 0);
        const size_t ul = key(0, end);
        const size_t ur = key(end, end);

        // Generate random values at corners
        _map[ll] = _dist(gen);
        _map[lr] = _dist(gen);
        _map[ul] = _dist(gen);
        _map[ur] = _dist(gen);

        // Recursively diamond square
        const size_t mid_point = end / 2;
        diamond_square(gen, mid_point, mid_point, mid_point, 1);
    }
    inline size_t key(const size_t x, const size_t y) const
    {
        return _size * x + y;
    }
    inline void diamond_square(std::mt19937 &gen, const size_t x, const size_t y, const size_t length, const size_t level)
    {
        // Generate the corner points
        const size_t nx = x - length;
        const size_t px = x + length;
        const size_t ny = y - length;
        const size_t py = y + length;

        // Generate diamond keys
        const size_t ll = key(nx, ny);
        const size_t lr = key(px, ny);
        const size_t ul = key(nx, py);
        const size_t ur = key(px, py);

        // Generate average value at center
        const size_t center = key(x, y);
        _map[center] = _dist(gen) + (_map[ll] + _map[ul] + _map[lr] + _map[ur]) / 4.0f;

        // Generate square keys
        const size_t l = key(nx, y);
        const size_t r = key(px, y);
        const size_t d = key(x, ny);
        const size_t u = key(x, py);

        // Generate random values at corners
        _map[l] = _dist(gen) + (_map[ll] + _map[ul] + _map[center]) / 3.0f;
        _map[r] = _dist(gen) + (_map[lr] + _map[ur] + _map[center]) / 3.0f;
        _map[d] = _dist(gen) + (_map[ll] + _map[lr] + _map[center]) / 3.0f;
        _map[u] = _dist(gen) + (_map[ul] + _map[ur] + _map[center]) / 3.0f;

        // Recursively call this function
        const size_t half = length / 2;
        if (half > 0)
        {
            // Recalculate distribution
            _dist = std::uniform_real_distribution<T>(_lower / level, _upper / level);

            // Calculate indices
            const size_t nnx = x - half;
            const size_t npx = x + half;
            const size_t nny = y - half;
            const size_t npy = y + half;
            const size_t next = level + 1;

            // Recurse into lower
            diamond_square(gen, nnx, nny, half, next); // LL
            diamond_square(gen, npx, nny, half, next); // LR
            diamond_square(gen, nnx, npy, half, next); // UL
            diamond_square(gen, npx, npy, half, next); // UR
        }
    }
    inline static size_t pow2(const size_t level)
    {
        return 1 << level;
    }

  public:
    height_map(std::mt19937 &gen, const size_t level, const T lower, const T upper)
        : _size(pow2(level) + 1), _map(_size * _size),
          _dist(lower, upper),
          _lower(lower), _upper(upper)

    {
        // Map size must be odd, and greater than one
        if (level == 0)
        {
            throw std::runtime_error("height_map: level must greater than zero");
        }

        // Generate the random height map
        generate(gen);
    }
    inline void gauss_blur_5x5()
    {
        // Gaussian 5x5 kernel
        gaussian_blur_5x5();
    }
    inline const T get(const size_t x, const size_t y) const
    {
        return _map[key(x, y)];
    }
    inline size_t size() const
    {
        return _size;
    }
};
}

#endif
