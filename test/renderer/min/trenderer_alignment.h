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
#ifndef _MGL_TEST_ARRAY_BUFFER_MGL_
#define _MGL_TEST_ARRAY_BUFFER_MGL_

#include <min/array_buffer.h>
#include <min/emitter_buffer.h>
#include <min/loop_sync.h>
#include <min/program.h>
#include <min/shader.h>
#include <min/shadow_buffer.h>
#include <min/static_vertex.h>
#include <min/test.h>
#include <min/text_buffer.h>
#include <min/texture_buffer.h>
#include <min/uniform_buffer.h>
#include <min/vertex_buffer.h>
#include <stdexcept>

typedef min::array_buffer<float, uint16_t, min::static_vertex> array_buffer;
typedef min::emitter_buffer<float> emitter_buffer;
typedef min::uniform_buffer<float> uniform_buffer;
typedef min::vertex_buffer<float, uint16_t, min::static_vertex> vertex_buffer;

bool test_renderer_align()
{
    bool out = true;

    // Print size and alignment of array_buffer
    std::cout << "array_buffer_size: " << sizeof(array_buffer) << std::endl;
    std::cout << "array_buffer_align: " << alignof(array_buffer) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "trenderer_alignment.h: Testing array_buffer alignment" << std::endl;
    out = out && test(sizeof(void *) * 11, sizeof(array_buffer), "Failed array_buffer sizeof");
    out = out && test(sizeof(void *), alignof(array_buffer), "Failed array_buffer alignof");
#endif

    // Print size and alignment of emitter_buffer
    std::cout << "emitter_buffer_size: " << sizeof(emitter_buffer) << std::endl;
    std::cout << "emitter_buffer_align: " << alignof(emitter_buffer) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "trenderer_alignment.h: Testing emitter_buffer alignment" << std::endl;
    out = out && test(sizeof(void *) * 25, sizeof(emitter_buffer), "Failed emitter_buffer sizeof");
    out = out && test(sizeof(void *), alignof(emitter_buffer), "Failed emitter_buffer alignof");
#endif

    // Print size and alignment of loop_sync
    std::cout << "loop_sync_size: " << sizeof(min::loop_sync) << std::endl;
    std::cout << "loop_sync_align: " << alignof(min::loop_sync) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "trenderer_alignment.h: Testing loop_sync alignment" << std::endl;
    out = out && test(sizeof(void *) * 44, sizeof(min::loop_sync), "Failed loop_sync sizeof");
    out = out && test(sizeof(void *), alignof(min::loop_sync), "Failed loop_sync alignof");
#endif

    // Print size and alignment of program
    std::cout << "program_size: " << sizeof(min::program) << std::endl;
    std::cout << "program_align: " << alignof(min::program) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "trenderer_alignment.h: Testing program alignment" << std::endl;
    out = out && test(sizeof(GLuint), sizeof(min::program), "Failed program sizeof");
    out = out && test(sizeof(GLuint), alignof(min::program), "Failed program alignof");
#endif

    // Print size and alignment of shader
    std::cout << "shader_size: " << sizeof(min::shader) << std::endl;
    std::cout << "shader_align: " << alignof(min::shader) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "trenderer_alignment.h: Testing shader alignment" << std::endl;
    out = out && test(sizeof(GLuint) * 2, sizeof(min::shader), "Failed shader sizeof");
    out = out && test(sizeof(GLuint), alignof(min::shader), "Failed shader alignof");
#endif

    // Print size and alignment of shadow_buffer
    std::cout << "shadow_buffer_size: " << sizeof(min::shadow_buffer) << std::endl;
    std::cout << "shadow_buffer_align: " << alignof(min::shadow_buffer) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "trenderer_alignment.h: Testing shadow_buffer alignment" << std::endl;
    out = out && test(sizeof(void *) * 37, sizeof(min::shadow_buffer), "Failed shadow_buffer sizeof");
    out = out && test(sizeof(void *), alignof(min::shadow_buffer), "Failed shadow_buffer alignof");
#endif

    // Print size and alignment of text_buffer
    std::cout << "text_buffer_size: " << sizeof(min::text_buffer) << std::endl;
    std::cout << "text_buffer_align: " << alignof(min::text_buffer) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "trenderer_alignment.h: Testing text_buffer alignment" << std::endl;
    out = out && test(sizeof(void *) * 1037, sizeof(min::text_buffer), "Failed text_buffer sizeof");
    out = out && test(sizeof(void *), alignof(min::text_buffer), "Failed text_buffer alignof");
#endif

    // Print size and alignment of texture_buffer
    std::cout << "texture_buffer_size: " << sizeof(min::texture_buffer) << std::endl;
    std::cout << "texture_buffer_align: " << alignof(min::texture_buffer) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "trenderer_alignment.h: Testing texture_buffer alignment" << std::endl;
    out = out && test(sizeof(void *) * 4, sizeof(min::texture_buffer), "Failed texture_buffer sizeof");
    out = out && test(sizeof(void *), alignof(min::texture_buffer), "Failed texture_buffer alignof");
#endif

    // Print size and alignment of uniform_buffer
    std::cout << "uniform_buffer_size: " << sizeof(min::uniform_buffer<float>) << std::endl;
    std::cout << "uniform_buffer_align: " << alignof(min::uniform_buffer<float>) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "trenderer_alignment.h: Testing uniform_buffer alignment" << std::endl;
    out = out && test(sizeof(void *) * 15, sizeof(min::uniform_buffer<float>), "Failed uniform_buffer sizeof");
    out = out && test(sizeof(void *), alignof(min::uniform_buffer<float>), "Failed uniform_buffer alignof");
#endif

    // Print size and alignment of vertex_buffer
    std::cout << "vertex_buffer_size: " << sizeof(vertex_buffer) << std::endl;
    std::cout << "vertex_buffer_align: " << alignof(vertex_buffer) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "trenderer_alignment.h: Testing vertex_buffer alignment" << std::endl;
    out = out && test(sizeof(void *) * 26, sizeof(vertex_buffer), "Failed vertex_buffer sizeof");
    out = out && test(sizeof(void *), alignof(vertex_buffer), "Failed vertex_buffer alignof");
#endif

    // return status
    return out;
}

#endif
