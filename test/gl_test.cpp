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
#include <min/taabbgrid.h>
#include <min/taabbox.h>
#include <min/taabboxinter.h>
#include <min/taabbresolve.h>
#include <min/taabbtree.h>
#include <min/tbitflag.h>
#include <min/tbmp.h>
#include <min/tcamera.h>
#include <min/tdds.h>
#include <min/tfrustinter.h>
#include <min/tfrustum.h>
#include <min/tmat2.h>
#include <min/tmat3.h>
#include <min/tmat4.h>
#include <min/tmd5anim.h>
#include <min/tmd5mesh.h>
#include <min/tmd5model.h>
#include <min/tmodel.h>
#include <min/toobbox.h>
#include <min/toobboxinter.h>
#include <min/toobbresolve.h>
#include <min/tphysics.h>
#include <min/tplane.h>
#include <min/tquat.h>
#include <min/tray.h>
#include <min/tsample.h>
#include <min/tsphere.h>
#include <min/tsphgrid.h>
#include <min/tsphinter.h>
#include <min/tsphresolve.h>
#include <min/tsphtree.h>
#include <min/ttran2.h>
#include <min/ttran3.h>
#include <min/tuint_sort.h>
#include <min/tvec2.h>
#include <min/tvec3.h>
#include <min/tvec4.h>
#include <min/twavefront.h>

int main()
{
    try
    {
        bool out = true;
        out = out && test_vec2();
        out = out && test_vec3();
        out = out && test_vec4();
        out = out && test_quat();
        out = out && test_mat2();
        out = out && test_mat3();
        out = out && test_mat4();
        out = out && test_tran2();
        out = out && test_tran3();
        out = out && test_plane();
        out = out && test_ray();
        out = out && test_sphere();
        out = out && test_aabbox();
        out = out && test_oobbox();
        out = out && test_sphere_intersect();
        out = out && test_sphere_resolve();
        out = out && test_aabbox_intersect();
        out = out && test_aabb_resolve();
        out = out && test_oobbox_intersect();
        out = out && test_oobb_resolve();
        out = out && test_frustum();
        out = out && test_frustum_intersect();
        out = out && test_camera();
        out = out && test_sample();
        out = out && test_wavefront();
        out = out && test_model();
        out = out && test_bmp();
        out = out && test_dds();
        out = out && test_aabb_tree();
        out = out && test_sphere_tree();
        out = out && test_bit_flag();
        out = out && test_uint_sort();
        out = out && test_aabb_grid();
        out = out && test_sphere_grid();
        out = out && test_md5_anim();
        out = out && test_md5_mesh();
        out = out && test_md5_model();
        out = out && test_physics_aabb_grid();
        if (out)
        {
            std::cout << "Graphics tests passed!" << std::endl;
            return 0;
        }
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    std::cout << "Graphics tests failed!" << std::endl;
    return -1;
}
