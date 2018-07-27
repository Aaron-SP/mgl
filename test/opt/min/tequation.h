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
#ifndef _MGL_TESTEQUATION_MGL_
#define _MGL_TESTEQUATION_MGL_

#include <min/equation.h>
#include <min/numeric.h>
#include <min/test.h>
#include <min/vec.h>

double g1(const min::vector<double, 3> &x)
{
    return x[0] * x[0] + 2.0 * x[1] * x[1] + 2.0 * x[2] * x[2] + 15;
}

bool test_equation()
{
    bool out = true;

    // Backward Hessian
    {
        // Create equation array
        min::equation<double, 3, min::backward> eqs[1] = {g1};

        // Test solving for the local minimum of f1
        min::vector<double, 3> x0(10.0);
        min::vector<double, 3> x1;

        // Test min_fast
        double convergence = eqs[0].min_fast(x0, x1, 20, 1E-7);

        // Evaluate g1 at x1
        double y1 = g1(x1);

        // Test if found min at (0.0, 0.0, 0.0) at starting point (10.0, 10.0, 10.0)
        out = out && test(0.0, convergence, 1E-4, "Failed equation backward min_fast convex");
        out = out && test(15.0, y1, 1E-4, "Failed equation backward min_fast convex");
        out = out && test(0.0, x1[0], 1E-4, "Failed equation backward min_fast convex");
        out = out && test(0.0, x1[1], 1E-4, "Failed equation backward min_fast convex");
        out = out && test(0.0, x1[2], 1E-4, "Failed equation backward min_fast convex");

        // Test min
        convergence = eqs[0].min(x0, x1, 20, 1E-4);

        // Evaluate g1 at x1
        y1 = g1(x1);

        // Test if found min at (0.0, 0.0, 0.0) at starting point (10.0, 10.0, 10.0)
        out = out && test(0.0, convergence, 1E-4, "Failed equation backward min");
        out = out && test(15.0, y1, 1E-4, "Failed equation backward min");
        out = out && test(0.0, x1[0], 1E-4, "Failed equation backward min");
        out = out && test(0.0, x1[1], 1E-4, "Failed equation backward min");
        out = out && test(0.0, x1[2], 1E-4, "Failed equation backward min");

        // Test hessian calculation
        min::matrix<double, 3, 3> h = min::backward<double, 3>::hessian(eqs[0], x0, 1E-3);
        out = out && test(2.0, h.get(0, 0), 1E-4, "Failed equation backward hessian");
        out = out && test(0.0, h.get(0, 1), 1E-4, "Failed equation backward hessian");
        out = out && test(0.0, h.get(0, 2), 1E-4, "Failed equation backward hessian");
        out = out && test(0.0, h.get(1, 0), 1E-4, "Failed equation backward hessian");
        out = out && test(4.0, h.get(1, 1), 1E-4, "Failed equation backward hessian");
        out = out && test(0.0, h.get(1, 2), 1E-4, "Failed equation backward hessian");
        out = out && test(0.0, h.get(2, 0), 1E-4, "Failed equation backward hessian");
        out = out && test(0.0, h.get(2, 1), 1E-4, "Failed equation backward hessian");
        out = out && test(4.0, h.get(2, 2), 1E-4, "Failed equation backward hessian");
    }

    // Center Hessian
    { // Create equation array
        min::equation<double, 3, min::center> eqs[1] = {g1};

        // Test solving for the local minimum of f1
        min::vector<double, 3> x0(10.0);
        min::vector<double, 3> x1;

        // Test min_fast
        double convergence = eqs[0].min_fast(x0, x1, 20, 1E-7);

        // Evaluate g1 at x1
        double y1 = g1(x1);

        // Test if found min at (0.0, 0.0, 0.0) at starting point (10.0, 10.0, 10.0)
        out = out && test(0.0, convergence, 1E-4, "Failed equation center min_fast convex");
        out = out && test(15.0, y1, 1E-4, "Failed equation center min_fast convex");
        out = out && test(0.0, x1[0], 1E-4, "Failed equation center min_fast convex");
        out = out && test(0.0, x1[1], 1E-4, "Failed equation center min_fast convex");
        out = out && test(0.0, x1[2], 1E-4, "Failed equation center min_fast convex");

        // Test min
        convergence = eqs[0].min(x0, x1, 20, 1E-4);

        // Evaluate g1 at x1
        y1 = g1(x1);

        // Test if found min at (0.0, 0.0, 0.0) at starting point (10.0, 10.0, 10.0)
        out = out && test(0.0, convergence, 1E-4, "Failed equation center min");
        out = out && test(15.0, y1, 1E-4, "Failed equation center min");
        out = out && test(0.0, x1[0], 1E-4, "Failed equation center min");
        out = out && test(0.0, x1[1], 1E-4, "Failed equation center min");
        out = out && test(0.0, x1[2], 1E-4, "Failed equation center min");

        // Test hessian calculation
        min::matrix<double, 3, 3> h = min::center<double, 3>::hessian(eqs[0], x0, 1E-3);
        out = out && test(2.0, h.get(0, 0), 1E-4, "Failed equation center hessian");
        out = out && test(0.0, h.get(0, 1), 1E-4, "Failed equation center hessian");
        out = out && test(0.0, h.get(0, 2), 1E-4, "Failed equation center hessian");
        out = out && test(0.0, h.get(1, 0), 1E-4, "Failed equation center hessian");
        out = out && test(4.0, h.get(1, 1), 1E-4, "Failed equation center hessian");
        out = out && test(0.0, h.get(1, 2), 1E-4, "Failed equation center hessian");
        out = out && test(0.0, h.get(2, 0), 1E-4, "Failed equation center hessian");
        out = out && test(0.0, h.get(2, 1), 1E-4, "Failed equation center hessian");
        out = out && test(4.0, h.get(2, 2), 1E-4, "Failed equation center hessian");
    }

    // Forward Hessian
    {
        // Create equation array
        min::equation<double, 3, min::forward> eqs[1] = {g1};

        // Test solving for the local minimum of f1
        min::vector<double, 3> x0(10.0);
        min::vector<double, 3> x1;

        // Test min_fast
        double convergence = eqs[0].min_fast(x0, x1, 20, 1E-7);

        // Evaluate g1 at x1
        double y1 = g1(x1);

        // Test if found min at (0.0, 0.0, 0.0) at starting point (10.0, 10.0, 10.0)
        out = out && test(0.0, convergence, 1E-4, "Failed equation forward min_fast convex");
        out = out && test(15.0, y1, 1E-4, "Failed equation forward min_fast convex");
        out = out && test(0.0, x1[0], 1E-4, "Failed equation forward min_fast convex");
        out = out && test(0.0, x1[1], 1E-4, "Failed equation forward min_fast convex");
        out = out && test(0.0, x1[2], 1E-4, "Failed equation forward min_fast convex");

        // Test min
        convergence = eqs[0].min(x0, x1, 20, 1E-4);

        // evaluate g1 at x1
        y1 = g1(x1);

        // Test if found min at (0.0, 0.0, 0.0) at starting point (10.0, 10.0, 10.0)
        out = out && test(0.0, convergence, 1E-4, "Failed equation forward min");
        out = out && test(15.0, y1, 1E-4, "Failed equation forward min");
        out = out && test(0.0, x1[0], 1E-4, "Failed equation forward min");
        out = out && test(0.0, x1[1], 1E-4, "Failed equation forward min");
        out = out && test(0.0, x1[2], 1E-4, "Failed equation forward min");

        // Test hessian calculation
        min::matrix<double, 3, 3> h = min::forward<double, 3>::hessian(eqs[0], x0, 1E-3);
        out = out && test(2.0, h.get(0, 0), 1E-4, "Failed equation forward hessian");
        out = out && test(0.0, h.get(0, 1), 1E-4, "Failed equation forward hessian");
        out = out && test(0.0, h.get(0, 2), 1E-4, "Failed equation forward hessian");
        out = out && test(0.0, h.get(1, 0), 1E-4, "Failed equation forward hessian");
        out = out && test(4.0, h.get(1, 1), 1E-4, "Failed equation forward hessian");
        out = out && test(0.0, h.get(1, 2), 1E-4, "Failed equation forward hessian");
        out = out && test(0.0, h.get(2, 0), 1E-4, "Failed equation forward hessian");
        out = out && test(0.0, h.get(2, 1), 1E-4, "Failed equation forward hessian");
        out = out && test(4.0, h.get(2, 2), 1E-4, "Failed equation forward hessian");
    }

    return out;
}

#endif
