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
#include <iostream>
#include <min/bmd5.h>
#include <min/bphysics.h>
#include <min/bspatial.h>
#include <min/bwavefront.h>
#include <min/grid.h>
#include <min/tree.h>
#include <string>

/* Calculate the current flowing through the circuit
      /---=---\
     /----=----\
 Vin.-----=-----.Vout
     \----=----/
      \---=---/
I = V/R; 1/R = 1/R1 ... 1/Rn
I = V * 1/R 
*/

constexpr size_t N = 40000;

constexpr unsigned long float_aabb = 808207;
constexpr unsigned long float_sphere = 808185;
constexpr unsigned long double_both = 810661;

// Make various sphere world sizes
min::sphere<float, min::vec2> fsw2 = make_sphere<float, min::vec2>();
min::sphere<double, min::vec2> dsw2 = make_sphere<double, min::vec2>();
min::sphere<float, min::vec3> fsw3 = make_sphere<float, min::vec3>();
min::sphere<double, min::vec3> dsw3 = make_sphere<double, min::vec3>();

// Make various aabb world sizes
min::aabbox<float, min::vec2> fbw2 = make_box<float, min::vec2>();
min::aabbox<double, min::vec2> dbw2 = make_box<double, min::vec2>();
min::aabbox<float, min::vec3> fbw3 = make_box<float, min::vec3>();
min::aabbox<double, min::vec3> dbw3 = make_box<double, min::vec3>();

// Make spheres
std::vector<min::sphere<float, min::vec2>> fs2 = make_spheres<float, min::vec2>(N);
std::vector<min::sphere<double, min::vec2>> ds2 = make_spheres<double, min::vec2>(N);
std::vector<min::sphere<float, min::vec3>> fs3 = make_spheres<float, min::vec3>(N);
std::vector<min::sphere<double, min::vec3>> ds3 = make_spheres<double, min::vec3>(N);

// Make boxes
std::vector<min::aabbox<float, min::vec2>> fb2 = make_boxes<float, min::vec2>(N);
std::vector<min::aabbox<double, min::vec2>> db2 = make_boxes<double, min::vec2>(N);
std::vector<min::aabbox<float, min::vec3>> fb3 = make_boxes<float, min::vec3>(N);
std::vector<min::aabbox<double, min::vec3>> db3 = make_boxes<double, min::vec3>(N);

double tree(const size_t V)
{
    double iR = 0.0;

    // Run 2D benchmarks in single precision
    std::cout << std::endl
              << "Running in 2D tree tests single precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_aabb_aabb<float, float_aabb, min::vec2, min::tree>(V, fbw2, fb2);
    iR += 1.0 / bench_aabb_sphere<float, float_sphere, min::vec2, min::tree>(V, fbw2, fs2);
    iR += 1.0 / bench_sphere_aabb<float, float_aabb, min::vec2, min::tree>(V, fsw2, fb2);
    iR += 1.0 / bench_sphere_sphere<float, float_sphere, min::vec2, min::tree>(V, fsw2, fs2);

    // Run 2D benchmarks in double precision
    std::cout << std::endl
              << "Running in 2D tree tests double precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_aabb_aabb<double, double_both, min::vec2, min::tree>(V, dbw2, db2);
    iR += 1.0 / bench_aabb_sphere<double, double_both, min::vec2, min::tree>(V, dbw2, ds2);
    iR += 1.0 / bench_sphere_aabb<double, double_both, min::vec2, min::tree>(V, dsw2, db2);
    iR += 1.0 / bench_sphere_sphere<double, double_both, min::vec2, min::tree>(V, dsw2, ds2);

    // Run 3D benchmarks in single precision
    std::cout << std::endl
              << "Running in 3D tree tests single precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_aabb_aabb<float, float_aabb, min::vec3, min::tree>(V, fbw3, fb3);
    iR += 1.0 / bench_aabb_sphere<float, float_sphere, min::vec3, min::tree>(V, fbw3, fs3);
    iR += 1.0 / bench_sphere_aabb<float, float_aabb, min::vec3, min::tree>(V, fsw3, fb3);
    iR += 1.0 / bench_sphere_sphere<float, float_sphere, min::vec3, min::tree>(V, fsw3, fs3);

    // Run 3D benchmarks in double precision
    std::cout << std::endl
              << "Running in 3D tree tests double precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_aabb_aabb<double, double_both, min::vec3, min::tree>(V, dbw3, db3);
    iR += 1.0 / bench_aabb_sphere<double, double_both, min::vec3, min::tree>(V, dbw3, ds3);
    iR += 1.0 / bench_sphere_aabb<double, double_both, min::vec3, min::tree>(V, dsw3, db3);
    iR += 1.0 / bench_sphere_sphere<double, double_both, min::vec3, min::tree>(V, dsw3, ds3);

    return iR;
}

double grid(const size_t V)
{
    double iR = 0.0;

    // Run 2D benchmarks in single precision
    std::cout << std::endl
              << "Running in 2D grid tests single precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_aabb_aabb<float, float_aabb, min::vec2, min::grid>(V, fbw2, fb2);
    iR += 1.0 / bench_aabb_sphere<float, float_sphere, min::vec2, min::grid>(V, fbw2, fs2);
    iR += 1.0 / bench_sphere_aabb<float, float_aabb, min::vec2, min::grid>(V, fsw2, fb2);
    iR += 1.0 / bench_sphere_sphere<float, float_sphere, min::vec2, min::grid>(V, fsw2, fs2);

    // Run 2D benchmarks in double precision
    std::cout << std::endl
              << "Running in 2D grid tests double precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_aabb_aabb<double, double_both, min::vec2, min::grid>(V, dbw2, db2);
    iR += 1.0 / bench_aabb_sphere<double, double_both, min::vec2, min::grid>(V, dbw2, ds2);
    iR += 1.0 / bench_sphere_aabb<double, double_both, min::vec2, min::grid>(V, dsw2, db2);
    iR += 1.0 / bench_sphere_sphere<double, double_both, min::vec2, min::grid>(V, dsw2, ds2);

    // Run 3D benchmarks in single precision
    std::cout << std::endl
              << "Running in 3D grid tests single precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_aabb_aabb<float, float_aabb, min::vec3, min::grid>(V, fbw3, fb3);
    iR += 1.0 / bench_aabb_sphere<float, float_sphere, min::vec3, min::grid>(V, fbw3, fs3);
    iR += 1.0 / bench_sphere_aabb<float, float_aabb, min::vec3, min::grid>(V, fsw3, fb3);
    iR += 1.0 / bench_sphere_sphere<float, float_sphere, min::vec3, min::grid>(V, fsw3, fs3);

    // Run 3D benchmarks in double precision
    std::cout << std::endl
              << "Running in 3D grid tests double precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_aabb_aabb<double, double_both, min::vec3, min::grid>(V, dbw3, db3);
    iR += 1.0 / bench_aabb_sphere<double, double_both, min::vec3, min::grid>(V, dbw3, ds3);
    iR += 1.0 / bench_sphere_aabb<double, double_both, min::vec3, min::grid>(V, dsw3, db3);
    iR += 1.0 / bench_sphere_sphere<double, double_both, min::vec3, min::grid>(V, dsw3, ds3);

    return iR;
}

double physics2D(const size_t V)
{
    double iR = 0.0;

    // Run benchmarks in single precision
    std::cout << std::endl
              << "Running in 2D physics tree tests single precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_physics_aabb_aabb<float, min::vec2, min::tree>(V, fbw2, fb2);
    iR += 1.0 / bench_physics_aabb_sphere<float, min::vec2, min::tree>(V, fbw2, fs2);
    iR += 1.0 / bench_physics_sphere_aabb<float, min::vec2, min::tree>(V, fsw2, fb2);
    iR += 1.0 / bench_physics_sphere_sphere<float, min::vec2, min::tree>(V, fsw2, fs2);

    // Run benchmarks in single precision
    std::cout << std::endl
              << "Running in 2D physics tree tests double precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_physics_aabb_aabb<double, min::vec2, min::tree>(V, dbw2, db2);
    iR += 1.0 / bench_physics_aabb_sphere<double, min::vec2, min::tree>(V, dbw2, ds2);
    iR += 1.0 / bench_physics_sphere_aabb<double, min::vec2, min::tree>(V, dsw2, db2);
    iR += 1.0 / bench_physics_sphere_sphere<double, min::vec2, min::tree>(V, dsw2, ds2);

    // Run benchmarks in single precision
    std::cout << std::endl
              << "Running in 2D physics grid tests single precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_physics_aabb_aabb<float, min::vec2, min::grid>(V, fbw2, fb2);
    iR += 1.0 / bench_physics_aabb_sphere<float, min::vec2, min::grid>(V, fbw2, fs2);
    iR += 1.0 / bench_physics_sphere_aabb<float, min::vec2, min::grid>(V, fsw2, fb2);
    iR += 1.0 / bench_physics_sphere_sphere<float, min::vec2, min::grid>(V, fsw2, fs2);

    // Run benchmarks in double precision
    std::cout << std::endl
              << "Running in 2D physics grid tests double precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_physics_aabb_aabb<double, min::vec2, min::grid>(V, dbw2, db2);
    iR += 1.0 / bench_physics_aabb_sphere<double, min::vec2, min::grid>(V, dbw2, ds2);
    iR += 1.0 / bench_physics_sphere_aabb<double, min::vec2, min::grid>(V, dsw2, db2);
    iR += 1.0 / bench_physics_sphere_sphere<double, min::vec2, min::grid>(V, dsw2, ds2);

    return iR;
}

double physics3D(const size_t V)
{
    double iR = 0.0;

    // Run benchmarks in single precision
    std::cout << std::endl
              << "Running in 3D physics tree tests single precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_physics_aabb_aabb<float, min::vec3, min::tree>(V, fbw3, fb3);
    iR += 1.0 / bench_physics_aabb_sphere<float, min::vec3, min::tree>(V, fbw3, fs3);
    iR += 1.0 / bench_physics_sphere_aabb<float, min::vec3, min::tree>(V, fsw3, fb3);
    iR += 1.0 / bench_physics_sphere_sphere<float, min::vec3, min::tree>(V, fsw3, fs3);

    // Run benchmarks in double precision
    std::cout << std::endl
              << "Running in 3D physics tree tests double precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_physics_aabb_aabb<double, min::vec3, min::tree>(V, dbw3, db3);
    iR += 1.0 / bench_physics_aabb_sphere<double, min::vec3, min::tree>(V, dbw3, ds3);
    iR += 1.0 / bench_physics_sphere_aabb<double, min::vec3, min::tree>(V, dsw3, db3);
    iR += 1.0 / bench_physics_sphere_sphere<double, min::vec3, min::tree>(V, dsw3, ds3);

    // Run benchmarks in single precision
    std::cout << std::endl
              << "Running in 3D physics grid tests single precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_physics_aabb_aabb<float, min::vec3, min::grid>(V, fbw3, fb3);
    iR += 1.0 / bench_physics_aabb_sphere<float, min::vec3, min::grid>(V, fbw3, fs3);
    iR += 1.0 / bench_physics_sphere_aabb<float, min::vec3, min::grid>(V, fsw3, fb3);
    iR += 1.0 / bench_physics_sphere_sphere<float, min::vec3, min::grid>(V, fsw3, fs3);

    // Run benchmarks in double precision
    std::cout << std::endl
              << "Running in 3D physics grid tests double precision mode" << std::endl
              << std::endl;

    iR += 1.0 / bench_physics_aabb_aabb<double, min::vec3, min::grid>(V, dbw3, db3);
    iR += 1.0 / bench_physics_aabb_sphere<double, min::vec3, min::grid>(V, dbw3, ds3);
    iR += 1.0 / bench_physics_sphere_aabb<double, min::vec3, min::grid>(V, dsw3, db3);
    iR += 1.0 / bench_physics_sphere_sphere<double, min::vec3, min::grid>(V, dsw3, ds3);

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

        const size_t V = N;
        double I = 0.0;
        double iR = 0.0;

        // Test tree
        iR = tree(V);
        I += V * iR;

        // Test grid
        iR = grid(V);
        I += V * iR;

        // Test physics2D
        iR = physics2D(V);
        I += V * iR;

        // Test physics3D
        iR = physics3D(V);
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