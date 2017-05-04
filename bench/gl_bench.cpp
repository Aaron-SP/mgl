/* Copyright [2013-2016] [Aaron Springstroh, Minimal Graphics Library]

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
#include <min/baabbgrid.h>
#include <min/baabbtree.h>
#include <min/bsphgrid.h>
#include <min/bsphtree.h>
#include <min/bwavefront.h>
#include <min/bmd5.h>
#include <string>
#include <iostream>

/* Calculate the current flowing through the circuit
      /---=---\
     /----=----\
 Vin.-----=-----.Vout
     \----=----/
      \---=---/
I = V/R; 1/R = 1/R1 ... 1/Rn
I = V * 1/R 
*/

const unsigned long float_aabb = 8047844;
const unsigned long float_sphere = 8047834;
const unsigned long double_both = 8085492;

double tree2D(size_t V)
{
    double iR = 0.0;

    // Running 2D tests
    std::cout << "Running 2D tree tests" << std::endl
              << std::endl;

    // Run benchmarks in single precision
    std::cout << "Running in 2D tree tests single precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_aabb_aabb_tree<float, float_aabb, min::vec2>(V);
    iR += 1.0 / bench_aabb_sphere_tree<float, float_sphere, min::vec2>(V);
    iR += 1.0 / bench_sphere_aabb_tree<float, float_aabb, min::vec2>(V);
    iR += 1.0 / bench_sphere_sphere_tree<float, float_sphere, min::vec2>(V);

    // Run benchmarks in double precision
    std::cout << std::endl
              << "Running in 2D tree tests double precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_aabb_aabb_tree<double, double_both, min::vec2>(V);
    iR += 1.0 / bench_aabb_sphere_tree<double, double_both, min::vec2>(V);
    iR += 1.0 / bench_sphere_aabb_tree<double, double_both, min::vec2>(V);
    iR += 1.0 / bench_sphere_sphere_tree<double, double_both, min::vec2>(V);

    return iR;
}

double tree3D(size_t V)
{
    double iR = 0.0;

    // Running 3D tests
    std::cout << "Running 3D tree tests" << std::endl
              << std::endl;

    // Run benchmarks in single precision
    std::cout << "Running in 3D tree tests single precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_aabb_aabb_tree<float, float_aabb, min::vec3>(V);
    iR += 1.0 / bench_aabb_sphere_tree<float, float_sphere, min::vec3>(V);
    iR += 1.0 / bench_sphere_aabb_tree<float, float_aabb, min::vec3>(V);
    iR += 1.0 / bench_sphere_sphere_tree<float, float_sphere, min::vec3>(V);

    // Run benchmarks in double precision
    std::cout << std::endl
              << "Running in 3D tree tests double precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_aabb_aabb_tree<double, double_both, min::vec3>(V);
    iR += 1.0 / bench_aabb_sphere_tree<double, double_both, min::vec3>(V);
    iR += 1.0 / bench_sphere_aabb_tree<double, double_both, min::vec3>(V);
    iR += 1.0 / bench_sphere_sphere_tree<double, double_both, min::vec3>(V);

    return iR;
}

double grid2D(size_t V)
{
    double iR = 0.0;

    // Running 2D tests
    std::cout << "Running 2D grid tests" << std::endl
              << std::endl;

    // Run benchmarks in single precision
    std::cout << "Running in 2D grid tests single precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_aabb_aabb_grid<float, float_aabb, min::vec2>(V);
    iR += 1.0 / bench_aabb_sphere_grid<float, float_sphere, min::vec2>(V);
    iR += 1.0 / bench_sphere_aabb_grid<float, float_aabb, min::vec2>(V);
    iR += 1.0 / bench_sphere_sphere_grid<float, float_sphere, min::vec2>(V);

    // Run benchmarks in double precision
    std::cout << std::endl
              << "Running in 2D grid tests double precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_aabb_aabb_grid<double, double_both, min::vec2>(V);
    iR += 1.0 / bench_aabb_sphere_grid<double, double_both, min::vec2>(V);
    iR += 1.0 / bench_sphere_aabb_grid<double, double_both, min::vec2>(V);
    iR += 1.0 / bench_sphere_sphere_grid<double, double_both, min::vec2>(V);

    return iR;
}

double grid3D(size_t V)
{
    // 3D tests
    double iR = 0.0;

    // Running 3D tests
    std::cout << "Running 3D grid tests" << std::endl
              << std::endl;

    // Run benchmarks in single precision
    std::cout << "Running in 3D grid tests single precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_aabb_aabb_grid<float, float_aabb, min::vec3>(V);
    iR += 1.0 / bench_aabb_sphere_grid<float, float_sphere, min::vec3>(V);
    iR += 1.0 / bench_sphere_aabb_grid<float, float_aabb, min::vec3>(V);
    iR += 1.0 / bench_sphere_sphere_grid<float, float_sphere, min::vec3>(V);

    // Run benchmarks in double precision
    std::cout << std::endl
              << "Running in 3D grid tests double precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_aabb_aabb_grid<double, double_both, min::vec3>(V);
    iR += 1.0 / bench_aabb_sphere_grid<double, double_both, min::vec3>(V);
    iR += 1.0 / bench_sphere_aabb_grid<double, double_both, min::vec3>(V);
    iR += 1.0 / bench_sphere_sphere_grid<double, double_both, min::vec3>(V);

    return iR;
}

int main(int argc, char *argv[])
{
    try
    {
        // Check the argument count
        if (argc > 1)
        {
            std::string input(argv[1]);
            if (input.compare("--verbose") != 0)
            {
                std::cout << "Unknown flag '" << argv[1] << "' expected '--verbose'" << std::endl;
                return 2;
            }
        }
        else
        {
            // Disable logging to cout
            std::cout.setstate(std::ios_base::failbit);
        }

        const size_t V = 40000;
        double I = 0.0;
        double iR = 0.0;

        // Test tree2D
        iR = tree2D(V);
        I += V * iR;

        // Test tree3D
        iR = tree3D(V);
        I += V * iR;

        // Test grid2D
        iR = grid2D(V);
        I += V * iR;

        // Test grid3D
        iR = grid3D(V);
        I += V * iR;

        // Test load wavefront
        iR = bench_wavefront();
        I += 1.0 / iR;

        // Test load md5
        iR = bench_md5();
        I += 1.0 / iR;

        // Enable logging to cout
        std::cout.clear();

        // print the performance score
        std::cout << "Graphics score is: " << I << std::endl;
        return 0;
    }
    catch (std::exception &ex)
    {
        // Enable logging to cout
        std::cout.clear();

        // print the exception
        std::cout << ex.what() << std::endl;
        std::cout << "Benchmark tests failed!" << std::endl;
    }

    return 1;
}