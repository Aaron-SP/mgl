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
#include <chrono>
#include <iostream>
#include <min/camera.h>
#include <min/loop_sync.h>
#include <min/program.h>
#include <min/settings.h>
#include <min/shader.h>
#include <min/static_vertex.h>
#include <min/vertex_buffer.h>
#include <min/window.h>
#include <string>

class screen_draw_test
{
  private:
    // OpenGL pipeline
    min::window _win;
    min::shader _vertex;
    min::shader _fragment;
    min::program _prog;

    // Buffers for model data and textures
    min::vertex_buffer<float, uint32_t, min::static_vertex, GL_FLOAT, GL_UNSIGNED_INT> _sbuffer;

    void load_keyboard()
    {
        // Set ability to close out of application by pressing 'Q'
        auto &keyboard = _win.get_keyboard();

        // Add key to watch
        keyboard.add(min::window::key_code::KEYQ);

        // Register callback function for closing window
        keyboard.register_keydown(min::window::key_code::KEYQ, screen_draw_test::close_window, (void *)&_win);

        // Register callback function for mouse click
        _win.register_data((void *)this);
        _win.register_lclick_up(screen_draw_test::on_click);
    }
    void load_vertex_buffer()
    {
        // Initialize the mesh buffer with one point
        min::mesh<float, uint32_t> point("Dynamic VBO");
        point.vertex.push_back(min::vec4<float>(0, 0, 0, 1));

        // We only use the vertex the extra parameters are required but unused
        point.uv.push_back(min::vec2<float>(0, 0));
        point.normal.push_back(min::vec3<float>::up());
        point.tangent.push_back(min::vec3<float>::up());
        point.bitangent.push_back(min::vec3<float>::up());

        // Since very mesh is one point, the model index is always zero!
        point.index.push_back(0);

        // Add mesh and update buffers
        _sbuffer.add_mesh(point);

        // Load buffer with data
        _sbuffer.upload();
    }

  public:
    // Load window shaders and program
    screen_draw_test()
        : _win("Test user input: Click 3x to add triangles", 720, 480, 3, 3),
          _vertex("data/shader/color.vertex", GL_VERTEX_SHADER),
          _fragment("data/shader/color.fragment", GL_FRAGMENT_SHADER),
          _prog(_vertex, _fragment)
    {
        // Set depth and cull settings
        min::settings::initialize();

        // Load the keyboard callbacks and settings
        load_keyboard();

        // Load static buffer for drawing points
        load_vertex_buffer();

        // Show the window
        _win.show();
    }
    static void close_window(void *ptr, double step)
    {
        // Call back function for closing window
        // 'ptr' is passed in by us in constructor
        if (ptr)
        {
            // Cast to window pointer type and call shut down on window
            min::window *win = reinterpret_cast<min::window *>(ptr);
            win->set_shutdown();
        }

        // Alert that we received the call back
        std::cout << "screen_draw_test: Shutdown called by user" << std::endl;
    }
    static void on_click(void *ptr, const uint_fast16_t x, const uint_fast16_t y)
    {
        // Call back function for closing window
        // 'ptr' is passed in by us in constructor
        if (ptr)
        {
            // Cast to window pointer type and call shut down on window
            screen_draw_test *test = reinterpret_cast<screen_draw_test *>(ptr);
            test->add_geometry(x, y);
        }
    }
    void add_geometry(const uint_fast16_t x, const uint_fast16_t y)
    {
        // Convert width and height to coordinates
        const float x_screen = (2.0 / _win.get_width()) * x - 1.0;
        const float y_screen = (2.0 / _win.get_height()) * y - 1.0;

        // Initialize the mesh buffer with one point
        min::mesh<float, uint32_t> point("add_geometry");
        point.vertex.push_back(min::vec4<float>(x_screen, y_screen, 0, 1));

        // We only use the vertex the extra parameters are required but unused
        point.uv.push_back(min::vec2<float>());
        point.normal.push_back(min::vec3<float>());
        point.tangent.push_back(min::vec3<float>());
        point.bitangent.push_back(min::vec3<float>());

        // Since very mesh is one point, the model index is always zero!
        point.index.push_back(0);

        // Update the GPU buffer by adding this point
        _sbuffer.upload_mesh(point);

        std::cout << "Added point at x: " + std::to_string(x_screen) + " y: " + std::to_string(y_screen) << std::endl;
    }
    void clear_background() const
    {
        // blue background
        const float color[] = {0.690, 0.875f, 0.901f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, color);
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    bool is_closed() const
    {
        return _win.get_shutdown();
    }
    void draw() const
    {
        // Bind VAO
        _sbuffer.bind();

        // Set point size to 40 pixels
        glPointSize(40.0);

        // Use the shader program to draw models
        _prog.use();

        // Draw all triangles in buffer
        _sbuffer.draw_all(GL_TRIANGLES);
    }
    void window_update()
    {
        // Update and swap buffers
        _win.update();
        _win.swap_buffers();
    }
};

int test_screen_draw()
{
    // Load window shaders and program, enable shader program
    screen_draw_test test;

    // Setup controller to run at 60 frames per second
    const int frames = 60;
    min::loop_sync sync(frames);

    // User can close with Q or use window manager
    while (!test.is_closed())
    {
        for (int i = 0; i < 60; i++)
        {
            // Start synchronizing the loop
            sync.start();

            // Clear the background color
            test.clear_background();

            // Draw the model at first position
            test.draw();

            // Update the window after draw command
            test.window_update();

            // Calculate needed delay to hit target
            sync.sync();
        }
    }

    return 0;
}

int main()
{
    try
    {
        return test_screen_draw();
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}
