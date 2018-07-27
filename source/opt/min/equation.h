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
#ifndef _MGL_EQUATION_MGL_
#define _MGL_EQUATION_MGL_

#include <min/mat.h>
#include <min/vec.h>

namespace min
{

// typedef for array of function pointers
template <typename T, size_t N>
using function = T (*)(const vector<T, N> &);

template <typename T, size_t N, template <typename, size_t> class numeric>
class equation
{
  private:
    function<T, N> _f;

  public:
    equation() : _f(nullptr) {}
    equation(const function<T, N> f) : _f(f) {}
    inline T operator()(const vector<T, N> &x) const
    {
        return _f(x);
    }
    inline matrix<T, N, N> hessian(const vector<T, N> &x0, const T dx)
    {
        return numeric<T, N>::hessian(*this, x0, dx);
    }
    // Find local minimum of function
    // This function uses steepest descent backtracking to adjust the step size between iterations
    // While: f(x + t*dx) > f(x) + alpha*t*grad(x)*dx
    //      t = B*t
    // where B = 0.75, alpha = 0.5, dx = -grad(x)
    // This equation assumes the function is *strongly convex* and may not apply for all functions
    // If this function doesn't work, it is recommended to calculate the Hessian for quadratic convergence
    inline T min_fast(const vector<T, N> &x0, vector<T, N> &x1, const size_t iterations, const T tolerance) const
    {
        // Start searching for all equations = 0
        x1 = x0;

        // Calculate the convergence criteria
        T convergence = 1.0;

        // Backtracking parameters
        const T alpha = 0.5;
        const T B = 0.75;

        // Search for up to _max_iterations
        for (size_t i = 0; i < iterations; i++)
        {
            // Calculate the gradient of the function
            const vector<T, N> grad = numeric<T, N>::gradient(*this, x1, tolerance);

            // Calculate the convergence criteria
            const T d = convergence = grad.square_magnitude();

            // Perform backtracking
            T t = 1.0;
            while ((*this)(x1 - grad * t) > ((*this)(x1) - (d * alpha * t)))
            {
                t *= B;
            }

            // Step to next iteration
            x1 -= grad * t;

            // Determine if we have converged
            if (convergence < tolerance)
            {
                return convergence;
            }
        }

        // Return the sum square of the function gradient, should be close to zero at solution
        return convergence;
    }

    // Find local minimum of function
    // This function calculates the hessian matrix for the equation
    // This method can be quite slow for higher dimensions
    // If equation is strongly convex, use min_fast instead as it converges exponentially with faster iteration times.
    // However min_fast will perform more iterations on average
    inline T min(const vector<T, N> &x0, vector<T, N> &x1, const size_t iterations, const T tolerance) const
    {
        // Start searching for minimum of equation
        x1 = x0;

        // Calculate the convergence criteria
        T convergence = 0.0;

        // Search for up to _max_iterations
        for (size_t i = 0; i < iterations; i++)
        {
            // Calculate the jacobian matrix at x1
            const matrix<T, N, N> hess = numeric<T, N>::hessian(*this, x1, tolerance);

            // Evaluate the system of equations at x1
            const vector<T, N> grad = numeric<T, N>::gradient(*this, x1, tolerance);

            // Calculate the convergence criteria
            convergence = grad.square_magnitude();

            // Calculate the next step of iteration
            const vector<T, N> step = hess.ludecomp(grad);

            // Step to next itertation
            x1 -= step;

            // Determine if we have converged
            if (convergence < tolerance)
            {
                return convergence;
            }
        }

        // Return the sum square of the x values, should be close to zero at solution
        return convergence;
    }
};
}
#endif
