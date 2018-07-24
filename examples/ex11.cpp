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
#include <algorithm>
#include <chrono>
#include <iostream>
#include <min/bmp.h>
#include <min/camera.h>
#include <min/dds.h>
#include <min/loop_sync.h>
#include <min/program.h>
#include <min/settings.h>
#include <min/shader.h>
#include <min/texture_buffer.h>
#include <min/ui_vertex.h>
#include <min/vertex_buffer.h>
#include <min/window.h>
#include <string>

class image_view_test
{
  private:
    // OpenGL window
    min::window _win;

    // OpenGL pipeline
    min::shader _vertex;
    min::shader _fragment;
    min::program _prog;

    // Buffers for model data and textures
    min::vertex_buffer<float, uint32_t, min::ui_vertex, GL_FLOAT, GL_UNSIGNED_INT> _sbuffer;
    min::texture_buffer _tbuffer;
    GLuint _img_id;

    inline float x_coord(const float x)
    {
        return (2.0f / _win.get_width()) * x - 1.0f;
    }
    inline float y_coord(const float y)
    {
        return (2.0f / _win.get_height()) * y - 1.0f;
    }
    inline void load_keyboard()
    {
        // Set ability to close out of application by pressing 'Q'
        auto &keyboard = _win.get_keyboard();

        // Add key to watch
        keyboard.add(min::window::key_code::KEYQ);

        // Register callback function for closing window
        keyboard.register_keydown(min::window::key_code::KEYQ, image_view_test::close_window, (void *)&_win);
    }
    inline void load_image(const char *file_name)
    {
        // Get the window size
        const uint_fast16_t win_width = _win.get_width();
        const uint_fast16_t win_height = _win.get_height();

        uint_fast16_t img_width = 0;
        uint_fast16_t img_height = 0;
        std::string file(file_name);
        std::string ext = file.substr(file.find_last_of(".") + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == "dds")
        {
            // Load DDS image
            const min::dds d = min::dds(file);
            img_width = d.get_width();
            img_height = d.get_height();

            // Load texture buffer
            _img_id = _tbuffer.add_dds_texture(d);
        }
        else
        {
            // Load BMP image
            const min::bmp b = min::bmp(file);
            img_width = b.get_width();
            img_height = b.get_height();

            // Load texture buffer
            _img_id = _tbuffer.add_bmp_texture(b);
        }

        // Calculate aspect ratio of image
        const float aspect_ratio = img_height / static_cast<float>(img_width);

        // Load textures
        const uint_fast16_t width = (win_width < img_width) ? win_width : img_width;
        const uint_fast16_t height = static_cast<uint_fast16_t>(std::floor(aspect_ratio * width));

        // Calculate centered image extent
        const uint_fast16_t win_w2 = win_width / 2;
        const uint_fast16_t win_h2 = win_height / 2;
        const uint_fast16_t w2 = width / 2;
        const uint_fast16_t h2 = height / 2;
        const float x_min = static_cast<float>(win_w2 - w2);
        const float x_max = static_cast<float>(win_w2 + w2);
        const float y_min = static_cast<float>(win_h2 - h2);
        const float y_max = static_cast<float>(win_h2 + h2);

        // Triangles must be in CW
        min::mesh<float, uint32_t> rect("image");

        // Bottom Left
        rect.vertex.push_back(min::vec4<float>(x_coord(x_min), y_coord(y_min), 0.0, 1.0));
        rect.uv.push_back(min::vec2<float>(0.0, 0.0));

        // Top left
        rect.vertex.push_back(min::vec4<float>(x_coord(x_min), y_coord(y_max), 0.0, 1.0));
        rect.uv.push_back(min::vec2<float>(0.0, 1.0));

        // Bottom right
        rect.vertex.push_back(min::vec4<float>(x_coord(x_max), y_coord(y_min), 0.0, 1.0));
        rect.uv.push_back(min::vec2<float>(1.0, 0.0));

        // Bottom right
        rect.vertex.push_back(min::vec4<float>(x_coord(x_max), y_coord(y_min), 0.0, 1.0));
        rect.uv.push_back(min::vec2<float>(1.0, 0.0));

        // Top left
        rect.vertex.push_back(min::vec4<float>(x_coord(x_min), y_coord(y_max), 0.0, 1.0));
        rect.uv.push_back(min::vec2<float>(0.0, 1.0));

        // Top right
        rect.vertex.push_back(min::vec4<float>(x_coord(x_max), y_coord(y_max), 0.0, 1.0));
        rect.uv.push_back(min::vec2<float>(1.0, 1.0));

        // Generate indices
        for (uint32_t i = 0; i < 6; i++)
        {
            rect.index.push_back(i);
        }

        // Add mesh and update buffers
        _sbuffer.add_mesh(rect);

        // Load buffer with data
        _sbuffer.upload();
    }

  public:
    // Load window shaders and program
    image_view_test(const char *file)
        : _win("MGL Image Viewer", 720, 480, 3, 3),
          _vertex("data/shader/ui.vertex", GL_VERTEX_SHADER),
          _fragment("data/shader/ui.fragment", GL_FRAGMENT_SHADER),
          _prog(_vertex, _fragment)
    {
        // Set depth and cull settings
        min::settings::initialize();

        // Load the keyboard callbacks and settings
        load_keyboard();

        // Load image texture and vertices
        load_image(file);

        // Show the window
        _win.show();
    }
    inline static void close_window(void *ptr, double step)
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
        std::cout << "image_view_test: Shutdown called by user" << std::endl;
    }
    inline void clear_background() const
    {
        // blue background
        const float color[] = {0.1, 0.1, 0.1, 1.0f};
        glClearBufferfv(GL_COLOR, 0, color);
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    inline bool is_closed() const
    {
        return _win.get_shutdown();
    }
    inline void draw() const
    {
        // Bind VAO
        _sbuffer.bind();

        // Bind this texture for drawing
        _tbuffer.bind(_img_id, 0);

        // Use the shader program to draw models
        _prog.use();

        // Draw all triangles in buffer
        _sbuffer.draw_all(GL_TRIANGLES);
    }
    inline void window_update()
    {
        // Update and swap buffers
        _win.update();
        _win.swap_buffers();
    }
};

int load_image_file(const char *file)
{
    // Alert user of loaded file
    std::cout << "image_view_test: Opening file '" << file << "' " << std::endl;

    // Load window shaders and program, enable shader program
    image_view_test test(file);

    // Setup controller to run at 60 frames per second
    const int frames = 60;
    min::loop_sync sync(frames);

    // User can close with Q or use window manager
    while (!test.is_closed())
    {
        for (int i = 0; i < frames; i++)
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

int main(int argc, char *argv[])
{
    try
    {
        // See if we are passing an input file
        if (argc > 1)
        {
            return load_image_file(argv[1]);
        }

        return load_image_file("data/texture/winter_moon.dds");
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}
