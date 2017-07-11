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
#ifndef __TESTTEXTURECOMPRESSOR__
#define __TESTTEXTURECOMPRESSOR__

#include <min/bmp.h>
#include <min/dds.h>
#include <min/test.h>
#include <min/texture_compressor.h>
#include <min/window.h>
#include <stdexcept>
#include <string>

bool test_texture_compressor()
{
    bool out = true;

    // Local variables
    min::window win("Test texture compressor", 720, 480, 3, 3);

    // Load textures
    const min::bmp b = min::bmp("data/texture/sphere_grad.bmp");

    min::texture_compressor compress;

    // Test DXT1 compression
    min::dds d = compress.compress_bmp_dds_dxt1(b);

    // Test DXT3 compression
    d = compress.compress_bmp_dds_dxt3(b);

    // Test DXT5 compression
    d = compress.compress_bmp_dds_dxt5(b);

    return out;
}

#endif
