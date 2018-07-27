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
#ifndef _MGL_EVOLUTION_MGL_
#define _MGL_EVOLUTION_MGL_

#include <algorithm>
#include <functional>
#include <min/nn.h>
#include <utility>

namespace min
{

template <typename T, size_t IN, size_t OUT, template <typename, size_t, size_t> class net, unsigned _pool_size = 512, unsigned _species = 8, unsigned mutations = 1, unsigned _epoch = 60>
class evolution
{
  private:
    static constexpr T _inv_pool_size = 1.0 / _pool_size;
    static constexpr unsigned _species_size = _pool_size / _species;
    static constexpr unsigned _species_half_size = (_species_size / 2) - 1;
    static constexpr T _inv_species = 1.0 / _species;
    static constexpr T _inv_species_size = 1.0 / _species_size;
    static constexpr T _inv_sample_size = 1.0 / (_species_size - 1);
    static constexpr unsigned _mutation_rate = mutations * _pool_size;

  private:
    std::vector<std::vector<net<T, IN, OUT>>> _nets;
    std::vector<std::vector<T>> _scores;
    std::vector<std::vector<size_t>> _breed_stock;
    T _ave[_species];
    size_t _species_top[_species];
    std::pair<size_t, size_t> _top;
    T _average_top;
    T _average_fitness;
    bool _cataclysm;
    unsigned _mutations;
    net_rng<T> _rng;
    net<T, IN, OUT> _top_net;
    T _top_score;
    unsigned _year;

    void average_fitness_score()
    {
        // Initialize statistical variables
        _average_top = 0.0;
        _average_fitness = 0.0;
        for (size_t i = 0; i < _species; i++)
        {
            _ave[i] = 0.0;
        }

        // Calculate sum of scores per species
        for (size_t i = 0; i < _species; i++)
        {
            for (size_t j = 0; j < _species_size; j++)
            {
                _ave[i] += _scores[i][j];
            }
        }

        // Calculate average score per species
        for (size_t i = 0; i < _species; i++)
        {
            // Calculate total average
            _average_fitness += _ave[i];

            // Normalize species average
            _ave[i] *= _inv_species_size;
        }

        // Normalize total average
        _average_fitness *= _inv_pool_size;

        // Create breeding partners greater than average score
        for (size_t i = 0; i < _species; i++)
        {
            size_t k = 0;
            for (size_t j = 0; j < _species_size; j++)
            {
                if (_scores[i][j] > _ave[i])
                {
                    _breed_stock[i][k] = j;
                    k++;
                }
            }
        }

        // Find top performer in each species
        for (size_t i = 0; i < _species; i++)
        {
            T top_score = _scores[i][0];
            _species_top[i] = 0;
            for (size_t j = 1; j < _species_size; j++)
            {
                // Get top index for this species
                if (_scores[i][j] > top_score)
                {
                    top_score = _scores[i][j];
                    _species_top[i] = j;
                }
            }
        }

        // Calculate average top species score
        for (size_t i = 0; i < _species; i++)
        {
            const size_t j = _species_top[i];
            _average_top += _scores[i][j];
        }

        // Normalize average top species score
        _average_top *= _inv_species;

        // Record the top score
        for (size_t i = 0; i < _species; i++)
        {
            // Create the top breeder
            const size_t top_index = _species_top[i];
            _breed_stock[i][0] = top_index;

            // If best of species is better than global top
            const bool exceeds_top = _scores[i][top_index] > this->top_fitness();
            if (exceeds_top)
            {
                // New top found
                _top = std::make_pair(i, top_index);
            }
        }

        // Cache the top performing net for thread safety
        _cataclysm = (_year % _epoch) == 0;
        if (_cataclysm)
        {
            _top_net = top_net();
            _top_score = top_fitness();
        }
    }
    void evolve()
    {
        // Create index vector to sort 0 to N
        average_fitness_score();

        for (size_t i = 0; i < _species; i++)
        {
            // Kill off species if average species performance is below kill threshold
            if (_cataclysm && _ave[i] < _average_fitness)
            {
                // Reseed this species with top performer
                for (size_t j = 0; j < _species_size; j++)
                {
                    // Reset the score, and start life a new man
                    _scores[i][j] = _top_score;

                    // Let the top net structure take over this species
                    _nets[i][j] = _top_net;

                    // Try to improve a winning formula
                    _nets[i][j].mutate(_rng);
                }
            }
            // Breed species groups
            else
            {
                // Create breeding stock
                size_t alpha = 0; // Parent A
                size_t beta = 1;  // Parent B
                for (size_t j = 0; j < _species_size; j++)
                {
                    if (_scores[i][j] < _ave[i])
                    {
                        // Reset the score, and start life a new man
                        _scores[i][j] = _ave[i];

                        // Breed new workers
                        const size_t m = _breed_stock[i][alpha];
                        const size_t n = _breed_stock[i][beta];
                        _nets[i][j] = net<T, IN, OUT>::breed(_nets[i][m], _nets[i][n]);

                        // Increment parent B
                        beta++;

                        // (N^2 - N)/2 breeding pairs
                        if (beta > _species_half_size)
                        {
                            alpha++;
                            beta = alpha + 1;
                        }
                    }
                }
            }
        }

        // Calculate mutations from max fitness
        if (_average_fitness > 0.0)
        {
            const T one = 1.0;
            const T approx_max_fitness = std::max(one, std::abs(_average_top - _average_fitness));
            _mutations = (_mutation_rate / approx_max_fitness);
        }
        else
        {
            _mutations = _mutation_rate;
        }

        // Mutate random nets
        for (size_t i = 0; i < _mutations; i++)
        {
            // Safe non negative, see constructor
            const size_t j = _rng.random_int() % _species;
            const size_t k = _rng.random_int() % _species_size;

            // Validation set has better convergence when preserving top species
            if (k != _species_top[j])
            {
                _nets[j][k].mutate(_rng);
            }
        }

        // Increment year count
        _year++;
    }

  public:
    evolution(const net<double, IN, OUT> &seed)
        : _nets(_species, std::vector<net<T, IN, OUT>>(_species_size)),
          _scores(_species, std::vector<T>(_species_size, 0.0)),
          _breed_stock(_species, std::vector<size_t>(_species_size, 0)),
          _average_top(0.0), _average_fitness(0.0),
          _cataclysm(false), _mutations(0),
          _rng(std::uniform_real_distribution<T>(-2.0, 2.0),
               std::uniform_real_distribution<T>(-2.0, 2.0),
               std::uniform_int_distribution<unsigned>(0, _pool_size - 1)),
          _top_score(0.0), _year(0)
    {
        // Assert we are not stupid
        static_assert(_species_size < _pool_size, "Species size can't be larger than the pool size");

        // Assert we are not stupid
        static_assert(_pool_size % _species == 0, "Speciation must perfectly divide the pool size");

        // Initialize seeded nets
        for (size_t i = 0; i < _species; i++)
        {
            for (size_t j = 0; j < _species_size; j++)
            {
                // Initialize networks
                _nets[i][j] = seed;
            }
        }
    }
    inline T average_fitness() const
    {
        return _average_fitness;
    }
    inline const net<T, IN, OUT> &top_net() const
    {
        return _nets[_top.first][_top.second];
    }
    inline T top_fitness() const
    {
        // Return top fitness
        return _scores[_top.first][_top.second];
    }
    T evolve(const std::function<T(const net<T, IN, OUT> &)> &fitness)
    {
        for (size_t i = 0; i < _species; i++)
        {
            for (size_t j = 0; j < _species_size; j++)
            {
                // Run fitness calculation
                this->_scores[i][j] = fitness(this->_nets[i][j]);
            }
        }

        // Evolve the pool
        evolve();

        // return average fitness
        return _average_fitness;
    }
};
}

#endif
