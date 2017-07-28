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
#ifndef __TESTUNIFORMBUFFER__
#define __TESTUNIFORMBUFFER__

#include <iostream>
#include <min/bmp.h>
#include <min/camera.h>
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
#include <stdexcept>
#include <string>

class uniform_test
{
  private:
    // OpenGL pipeline
    min::window _win;
    min::shader _vertex;
    min::shader _fragment;
    min::program _prog;

    // Buffers for model data and textures
    min::vertex_buffer<float, uint32_t, min::static_vertex, GL_FLOAT, GL_UNSIGNED_INT> _sbuffer;
    min::texture_buffer _tbuffer;
    GLuint _bmp_id;

    // Camera and uniform data
    min::camera<float> _cam;
    min::uniform_buffer<float> _ubuffer;
    size_t _model_id;

  public:
    // Load window shaders and program
    uniform_test()
        : _win("Test uniform buffers", 720, 480, 3, 3),
          _vertex("data/shader/light.vertex", GL_VERTEX_SHADER),
          _fragment("data/shader/light.fragment", GL_FRAGMENT_SHADER),
          _prog(_vertex, _fragment),
          _ubuffer(100, 100)
    {
        // Set depth and cull settings
        min::settings::initialize();

        // Use the shader program to draw models
        _prog.use();
    }
    void clear_background()
    {
        // blue background
        const float color[] = {0.690, 0.875f, 0.901f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, color);
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    void load_model_texture()
    {
        // Warn user we are opening a large model
        std::cout << "Opening a very large model: blender_suzanne.obj" << std::endl;

        // load blender-suzanne model
        min::wavefront<float, uint32_t> wave("data/models/blender_suzanne.obj");
        min::mesh<float, uint32_t> &suzanne = wave.get_meshes()[0];
        suzanne.scale_uv(10.0);
        suzanne.calculate_tangents();

        // Load textures
        const min::bmp b = min::bmp("data/texture/stone.bmp");

        // Query max texture size
        size_t size = _tbuffer.get_max_texture_size();
        std::cout << "Max texture size is: " << size << "x" << size << std::endl;

        // Load texture buffer
        _bmp_id = _tbuffer.add_bmp_texture(b);

        // Add mesh and update buffers
        _sbuffer.add_mesh(suzanne);

        // Load buffer with data
        _sbuffer.upload();
    }
    void load_camera_uniforms()
    {
        // Move and camera to -X and look at origin
        min::vec3<float> pos = min::vec3<float>(-5.0, 2.0, 0.0);
        min::vec3<float> look = min::vec3<float>(0.0, 0.0, 0.0);

        // Test perspective projection
        // Create camera, set location and look at
        _cam.set_position(pos);
        _cam.set_look_at(look);
        _cam.set_perspective();

        // Load the uniform buffer with program we will use
        _ubuffer.set_program(_prog);

        // Bind this uniform buffer for use
        _ubuffer.bind();

        size_t size = _ubuffer.get_max_buffer_size();
        std::cout << "Max uniform buffer size(bytes) is: " << size << std::endl;

        // Load light into uniform buffer
        min::vec4<float> light_color(1.0, 1.0, 1.0, 1.0);
        min::vec4<float> light_position(-2.0, 2.0, 0.0, 1.0);
        min::vec4<float> light_power(0.1, 6.0, 5.0, 1.0);
        _ubuffer.add_light(min::light<float>(light_color, light_position, light_power));

        // Load projection and view matrix into uniform buffer
        _ubuffer.add_matrix(_cam.get_pv_matrix());
        _ubuffer.add_matrix(_cam.get_v_matrix());

        // Get model ID for later use
        _model_id = _ubuffer.add_matrix(min::mat4<float>());

        // Load the buffer with data
        _ubuffer.update();
    }
    void draw_one()
    {
        // Bind VAO
        _sbuffer.bind();

        // Bind the bmp for drawing
        _tbuffer.bind(_bmp_id, 0);

        // Draw object
        _sbuffer.draw(GL_TRIANGLES, 0);
    }
    void draw_two()
    {
        // Test light with model translation in -Z (right)
        // Reset the model matrix in the uniform buffer
        min::mat4<float> model_matrix(min::vec3<float>(0.0, 0.0, -1.0));
        _ubuffer.set_matrix(model_matrix, _model_id);
        _ubuffer.update();

        // Bind VAO, already bound but for completeness
        _sbuffer.bind();

        // Bind the bmp for drawing
        _tbuffer.bind(_bmp_id, 0);

        // Draw object
        _sbuffer.draw(GL_TRIANGLES, 0);
    }
    void window_update()
    {
        // Update and swap buffers
        _win.update();
        _win.swap_buffers();
    }
};

bool test_uniform_buffer()
{
    bool out = true;

    // Load window shaders and program, enable shader program
    uniform_test test;

    // Clear the background color
    test.clear_background();

    // Load model and textures from files
    test.load_model_texture();

    // Load the camera and fill uniform buffers with light and model matrix
    test.load_camera_uniforms();

    // Draw the model at first position
    test.draw_one();

    // Update the window after draw command
    test.window_update();

    std::string s = ask("Can you see a white light on a stone monkey?");
    if (!s.compare("n"))
    {
        throw std::runtime_error("Failed uniform_buffer light test");
    }

    // Clear the screen and depth buffer for redrawing
    test.clear_background();

    // Draw the model at first position
    test.draw_two();

    // Update the window after draw command
    test.window_update();

    s = ask("Did the monkey move to the right and maintain light exposure on the left side?");
    if (!s.compare("n"))
    {
        throw std::runtime_error("Failed uniform_buffer light position change test");
    }

    return out;
}

#endif
