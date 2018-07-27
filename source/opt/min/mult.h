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
#ifndef _MGL_MULTIPLY_MGL_
#define _MGL_MULTIPLY_MGL_

#include <min/mat.h>
#include <min/vec.h>
#include <type_traits>

namespace min
{

template <typename T, size_t R1, size_t C1, size_t R2, size_t C2>
inline matrix<T, R1, C2> multiply(const matrix<T, R1, C1> &m1, const matrix<T, R2, C2> &m2)
{
    // Assert that this matrix is square
    static_assert(std::is_same<std::integral_constant<size_t, C1>, std::integral_constant<size_t, R2>>::value, "matrix.multiply(): matrices are not compatible!");

    matrix<T, R1, C2> out(0.0);

    // Multiply matrices
    for (size_t i = 0; i < R1; i++)
    {
        for (size_t j = 0; j < C2; j++)
        {
            for (size_t k = 0; k < C1; k++)
            {
                // C1 == R2
                out.get(i, j) += m1.get(i, k) * m2.get(k, j);
            }
        }
    }

    return out;
}

template <typename T, size_t R, size_t C>
inline vector<T, R> multiply(const matrix<T, R, C> &m1, const vector<T, C> &m2)
{
    vector<T, R> out;

    // Multiply column vector by matrix
    for (size_t i = 0; i < R; i++)
    {
        for (size_t j = 0; j < C; j++)
        {
            out[i] += m1.get(i, j) * m2[j];
        }
    }

    // Return vector of R rows
    return out;
}
}

#endif
