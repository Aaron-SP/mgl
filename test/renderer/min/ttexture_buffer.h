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
#ifndef _MGL_TESTTEXTUREBUFFER_MGL_
#define _MGL_TESTTEXTUREBUFFER_MGL_

#include <min/bmp.h>
#include <min/camera.h>
#include <min/convert.h>
#include <min/dds.h>
#include <min/program.h>
#include <min/settings.h>
#include <min/shader.h>
#include <min/static_vertex.h>
#include <min/test.h>
#include <min/texture_buffer.h>
#include <min/vertex_buffer.h>
#include <min/window.h>
#include <stdexcept>
#include <string>

class texture_test
{
  private:
    // OpenGL pipeline
    min::window _win;
    min::shader _vertex;
    min::shader _fragment;
    min::program _prog;

    min::vertex_buffer<float, uint16_t, min::static_vertex, GL_FLOAT, GL_UNSIGNED_SHORT> _sbuffer;
    min::texture_buffer _tbuffer;
    GLuint _bmp_id;
    GLuint _dds_id;

    min::camera<float> _cam;

  public:
    texture_test() : _win("Test texture buffers", 720, 480, 3, 3),
                     _vertex("data/shader/texture.vertex", GL_VERTEX_SHADER),
                     _fragment("data/shader/texture.fragment", GL_FRAGMENT_SHADER),
                     _prog(_vertex, _fragment)
    {
        // Set depth and cull settings
        min::settings::initialize();

        // Show the window
        _win.show();

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
    void load_model_texture_data()
    {
        // load sphere model
        min::sphere<float, min::vec3> sph(min::vec3<float>(0.0, 0.0, 2.0), 1.0);
        min::mesh<float, uint16_t> sph_test = min::to_mesh<float, uint16_t>(sph);

        // load box model
        min::aabbox<float, min::vec3> box(min::vec3<float>(-0.0, -0.0, -2.0), min::vec3<float>(-1.0, -1.0, -1.0));
        min::mesh<float, uint16_t> box_test = min::to_mesh<float, uint16_t>(box);

        // Load textures
        const min::bmp b = min::bmp("data/texture/sphere_grad.bmp");
        const min::dds d = min::dds("data/texture/stone.dds");

        // Load texture buffer
        _bmp_id = _tbuffer.add_bmp_texture(b);
        _dds_id = _tbuffer.add_dds_texture(d);

        // Add mesh and update buffers
        _sbuffer.add_mesh(sph_test);
        _sbuffer.add_mesh(box_test);
        _sbuffer.upload();
    }
    void load_camera_data()
    {
        // Move and camera to +X and look at origin
        min::vec3<float> pos(5.0, 1.0, 1.0);
        min::vec3<float> look(0.0, 0.0, 0.0);

        // Test perspective projection
        // Create camera, set location and look at
        _cam.set_position(pos);
        _cam.set_look_at(look);
        _cam.set_perspective();

        // Get the projection-view matrix and update its uniform
        GLint location = glGetUniformLocation(_prog.id(), "pvm");
        if (location != -1)
        {
            glUniformMatrix4fv(location, 1, GL_FALSE, (float *)&_cam.get_pv_matrix());
        }
        else
        {
            throw std::runtime_error("Failed setting matrix uniform");
        }
    }
    void draw_one()
    {
        // Bind VAO
        _sbuffer.bind();

        // Draw the BMP object
        _tbuffer.bind(_bmp_id, 0);
        _sbuffer.draw(GL_TRIANGLES, 0);

        // Draw the DDS object
        _tbuffer.bind(_dds_id, 0);
        _sbuffer.draw(GL_TRIANGLES, 1);
    }
    void draw_two()
    {
        // Test orthographic projection
        // Move and camera to -Z and look at origin
        min::vec3<float> pos(-2.0, 1.0, -5.0);
        min::vec3<float> look(0.0, 0.0, 2.0);

        // Create camera, set zoom
        _cam.set_position(pos);
        _cam.set_look_at(look);
        _cam.get_frustum().zoom(0.05);
        _cam.set_orthographic();

        // Update matrix to orthographic mode
        GLint location = glGetUniformLocation(_prog.id(), "pvm");
        if (location != -1)
        {
            glUniformMatrix4fv(location, 1, GL_FALSE, (float *)&_cam.get_pv_matrix());
        }
        else
        {
            throw std::runtime_error("Failed setting matrix uniform");
        }

        // Color the background as we just cleared it
        clear_background();

        // Bind VAO, already bound but for completeness
        _sbuffer.bind();

        // Bind the bmp for drawing
        _tbuffer.bind(_bmp_id, 0);

        // Draw object one
        _sbuffer.draw(GL_TRIANGLES, 0);

        // Bind the dds for drawing
        _tbuffer.bind(_dds_id, 0);

        // Draw object two
        _sbuffer.draw(GL_TRIANGLES, 1);
    }
    void window_update()
    {
        // Update and swap buffers
        _win.update();
        _win.swap_buffers();
    }
};

bool test_texture_buffer()
{
    bool out = true;

    // Start test
    texture_test test;

    // Blue background
    test.clear_background();

    // Load box and sphere with two different textures
    test.load_model_texture_data();

    // Load perspective camera
    test.load_camera_data();

    // Draw two models in perspective
    test.draw_one();

    // Update the window
    test.window_update();

    std::string s = ask("Can you see textured shapes on the screen in perspective mode?");
    if (!s.compare("n"))
    {
        throw std::runtime_error("Failed texture_buffer perspective test");
    }

    // Draw two models in orthographic
    test.draw_two();

    // Update window
    test.window_update();

    s = ask("Does the depth look realistic in orthographic mode?");
    if (!s.compare("n"))
    {
        throw std::runtime_error("Failed texture_buffer orthographic test");
    }

    return out;
}

#endif
