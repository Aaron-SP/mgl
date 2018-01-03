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
#ifndef __TESTSTATICINSTANCE__
#define __TESTSTATICINSTANCE__

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

class instance_test
{
  private:
    // OpenGL pipeline
    min::window _win;
    min::shader _vertex;
    min::shader _fragment;
    min::program _prog;

    // Buffers for model data and textures
    min::vertex_buffer<float, uint16_t, min::static_vertex, GL_FLOAT, GL_UNSIGNED_SHORT> _sbuffer;
    min::texture_buffer _tbuffer;
    GLuint _bmp_id;

    // Camera and uniform data
    min::camera<float> _cam;
    min::uniform_buffer<float> _ubuffer;
    size_t _model_id[100];

  public:
    // Load window shaders and program
    instance_test()
        : _win("Test static instances", 720, 480, 3, 3),
          _vertex("data/shader/instance.vertex", GL_VERTEX_SHADER),
          _fragment("data/shader/instance.fragment", GL_FRAGMENT_SHADER),
          _prog(_vertex, _fragment),
          _ubuffer(10, 102)
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
    void load_model_texture()
    {
        // load sphere model
        min::sphere<float, min::vec3> sph(min::vec3<float>(0.0, 0.0, 0.0), 0.5);
        min::mesh<float, uint16_t> sph_test = min::to_mesh<float, uint16_t>(sph);
        sph_test.scale_uv(4.0);

        // Load textures
        const min::bmp b = min::bmp("data/texture/stone.bmp");

        // Load texture buffer
        _bmp_id = _tbuffer.add_bmp_texture(b);

        // Add mesh and update buffers
        _sbuffer.add_mesh(sph_test);
        _sbuffer.upload();
    }
    void load_camera_uniforms()
    {
        // Move and camera to +Y and look at origin
        min::vec3<float> pos = min::vec3<float>(0.0, 0.0, 20.0);
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

        // Load light into uniform buffer
        min::vec4<float> light_color(1.0, 1.0, 1.0, 1.0);
        min::vec4<float> light_position(0.0, 0.0, 20.0, 1.0);
        min::vec4<float> light_power(0.1, 10.0, 1.0, 1.0);
        _ubuffer.add_light(min::light<float>(light_color, light_position, light_power));

        // Load projection and view matrix into uniform buffer
        _ubuffer.add_matrix(_cam.get_pv_matrix());
        _ubuffer.add_matrix(_cam.get_v_matrix());

        // Generate random positions in a circle
        // The objects will be between radius
        float radius = 5.0;
        std::uniform_real_distribution<float> position(-radius, radius);
        std::uniform_real_distribution<float> offset(0.0, 0.8 * radius);
        // Mersenne Twister: Good quality random number generator
        std::mt19937 rng;
        // Initialize with fixed seed
        rng.seed(101129);

        // Radius of circle
        float radius2 = radius * radius;
        bool flag = true;
        for (size_t i = 0; i < 100; i++)
        {
            // Generate 100 instances of sphere, with random position and rotation around circle
            float x = position(rng);
            float y = std::sqrt(radius2 - x * x) - offset(rng);
            if (flag)
            {
                y *= -1.0;
            }
            flag = !flag;

            // Set x and Y value
            min::vec3<float> t(x, y, 0);

            // Create model matrix from translation away from origin
            min::mat3<float> rot = min::mat3<float>().set_rotation_x(x * 40.0) * min::mat3<float>().set_rotation_y(y * 40.0);
            min::mat4<float> model = min::mat4<float>(t, rot);

            // Set the model matrix
            _model_id[i] = _ubuffer.add_matrix(model);
        }

        // Load the buffer with data
        _ubuffer.update();
    }
    void draw()
    {
        // Bind VAO, it actually is already bound and could be removed
        _sbuffer.bind();

        // Bind the bmp for drawing
        _tbuffer.bind(_bmp_id, 0);

        // Draw 100 instances of object
        _sbuffer.draw_many(GL_TRIANGLES, 0, 100);
    }
    void window_update()
    {
        // Update and swap buffers
        _win.update();
        _win.swap_buffers();
    }
};

bool test_static_instance()
{
    bool out = true;

    // Load window shaders and program, enable shader program
    instance_test test;

    // Clear the background color
    test.clear_background();

    // Load model and textures from files
    test.load_model_texture();

    // Load the camera and fill uniform buffers with light and model matrix
    test.load_camera_uniforms();

    // Draw the model at first position
    test.draw();

    // Update the window after draw command
    test.window_update();

    std::string s = ask("Can you see the instanced spheres?");
    if (!s.compare("n"))
    {
        throw std::runtime_error("Failed static_instance sphere test");
    }

    return out;
}

#endif
