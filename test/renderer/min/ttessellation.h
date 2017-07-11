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
#ifndef __TESTTESSELLATION__
#define __TESTTESSELLATION__

#include <chrono>
#include <min/bmp.h>
#include <min/camera.h>
#include <min/convert.h>
#include <min/program.h>
#include <min/settings.h>
#include <min/shader.h>
#include <min/static_vertex.h>
#include <min/test.h>
#include <min/texture_buffer.h>
#include <min/uniform_buffer.h>
#include <min/vertex_buffer.h>
#include <min/wavefront.h>
#include <min/window.h>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

class tessellation_test
{
  private:
    // OpenGL pipeline
    min::window _win;
    min::shader _vertex;
    min::shader _tcs;
    min::shader _tes;
    min::shader _fragment;
    std::vector<GLuint> _shaders;
    min::program _prog;

    // Buffers for height map
    min::texture_buffer _tbuffer;
    GLuint _hmap_id;
    GLuint _tex_id;

    // Buffers for matrices
    min::vertex_buffer<float, uint32_t, min::static_vertex, GL_FLOAT, GL_UNSIGNED_INT> _sbuffer;
    min::uniform_buffer<float> _ubuffer;

    // Camera and uniform data
    min::camera<float> _cam;

  public:
    // Load window shaders and program
    tessellation_test()
        : _win("Test tessellation", 720, 480, 4, 0),
          _vertex("data/shader/height_map.vertex", GL_VERTEX_SHADER),
          _tcs("data/shader/height_map.tcs", GL_TESS_CONTROL_SHADER),
          _tes("data/shader/height_map.tes", GL_TESS_EVALUATION_SHADER),
          _fragment("data/shader/height_map.fragment", GL_FRAGMENT_SHADER),
          _shaders({_vertex.id(), _tcs.id(), _tes.id(), _fragment.id()}),
          _prog(_shaders),
          _ubuffer(0, 2)
    {
        // Set depth and cull settings
        min::settings::initialize();

        // Use the shader program to draw models
        _prog.use();
    }
    void clear_background()
    {
        // black background
        const float color[] = {0.1, 0.1, 0.1, 1.0f};
        glClearBufferfv(GL_COLOR, 0, color);
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    void load_texture()
    {
        // Load textures
        const min::bmp height_map = min::bmp("data/texture/height_map.bmp");
        const min::bmp texture = min::bmp("data/texture/stone.bmp");

        // Load texture buffer for height map and the texture
        _hmap_id = _tbuffer.add_bmp_texture(height_map);
        _tex_id = _tbuffer.add_bmp_texture(texture);
    }
    void load_camera_uniforms()
    {
        // Move and camera to +Y and look at origin
        min::vec3<float> pos = min::vec3<float>(0.0, 5.0, 30.0);
        min::vec3<float> look = min::vec3<float>(0.0, 5.0, -1.0);

        // Test perspective projection
        // Create camera, set location and look at
        _cam.set_position(pos);
        _cam.set_look_at(look);
        _cam.set_perspective();

        // Load the uniform buffer with program we will use
        _ubuffer.set_program(_prog);

        // Load projection and view matrix into uniform buffer
        _ubuffer.add_matrix(_cam.get_pv_matrix());
        _ubuffer.add_matrix(_cam.get_v_matrix());

        // Load the buffer with data
        _ubuffer.update();
    }
    void draw()
    {
        // Bind VAO, we won't add to the static buffer
        // But we need a VAO bound to draw anything
        _sbuffer.bind();

        // Draw the hmap on binding point 0
        // Draw the texture on binding point 1
        _tbuffer.bind(_hmap_id, 0);
        _tbuffer.bind(_tex_id, 1);

        // 4 vertex patches
        glPatchParameteri(GL_PATCH_VERTICES, 4);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Draw the 4 vertices in vertex shader 4096 times for 64 x 64 grid
        glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
    }
    void window_update()
    {
        // Update and swap buffers
        _win.update();
        _win.swap_buffers();
    }
};

bool test_tessellation()
{
    bool out = true;

    // Load window shaders and program, enable shader program
    tessellation_test test;

    // Clear the background color
    test.clear_background();

    // Load heightmap from files
    test.load_texture();

    // Load the camera and fill uniform buffers with light and model matrix
    test.load_camera_uniforms();

    // Draw the model at first position
    test.draw();

    // Update the window after draw command
    test.window_update();

    std::string s = ask("Can you see generated terrain?");
    if (!s.compare("n"))
    {
        throw std::runtime_error("Failed tessellation test");
    }

    return out;
}

#endif
