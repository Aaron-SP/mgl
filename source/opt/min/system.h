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
#ifndef _MGL_SYSTEM_MGL_
#define _MGL_SYSTEM_MGL_

#include <min/equation.h>
#include <min/numeric.h>
#include <min/vec.h>

namespace min
{

template <typename T, size_t N, template <typename, size_t> class numeric>
class system
{
  private:
    equation<T, N, numeric> _system[N];
    size_t _max_iterations;
    T _tolerance;

  public:
    system(const equation<T, N, numeric> eqs[N])
        : _max_iterations(100), _tolerance(1E-4)
    {
        // Copy all functions
        for (size_t i = 0; i < N; i++)
        {
            _system[i] = eqs[i];
        }
    }
    inline matrix<T, N, N> jacobian(const vector<T, N> &x, const T dx) const
    {
        // Return jacobian matrix of system
        return numeric<T, N>::jacobian(_system, x, dx);
    }
    inline vector<T, N> evaluate(const vector<T, N> &x) const
    {
        vector<T, N> out;

        // Evaluate all functions
        for (size_t i = 0; i < N; i++)
        {
            out[i] = _system[i](x);
        }

        return out;
    }
    // Uses Newton's Method to find roots of the system of equations
    inline T zero(const vector<T, N> &x0, vector<T, N> &x1) const
    {
        // Start searching for all equations = 0
        x1 = x0;

        // Calculate the convergence criteria
        T convergence = 0.0;

        // Search for up to _max_iterations
        for (size_t i = 0; i < _max_iterations; i++)
        {
            // Calculate the jacobian matrix at x1
            const matrix<T, N, N> jac = this->jacobian(x1, _tolerance);

            // Evaluate the system of equations at x1
            const vector<T, N> y = this->evaluate(x1);

            // Calculate the convergence criteria
            convergence = y.square_magnitude();

            // Calculate the next step of iteration
            const vector<T, N> step = jac.ludecomp(y);

            // Step to next itertation
            x1 -= step;

            // Determine if we have converged
            if (convergence < _tolerance)
            {
                return convergence;
            }
        }

        // Return the sums square of the x values, should be close to zero at solution
        return convergence;
    }
};
}

#endif
