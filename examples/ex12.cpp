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
#include <min/aabbox.h>
#include <min/bmp.h>
#include <min/camera.h>
#include <min/intersect.h>
#include <min/loop_sync.h>
#include <min/program.h>
#include <min/ray.h>
#include <min/settings.h>
#include <min/shader.h>
#include <min/sphere.h>
#include <min/texture_buffer.h>
#include <min/tree.h>
#include <min/ui_vertex.h>
#include <min/vertex_buffer.h>
#include <min/window.h>
#include <string>

class ray_trace_test
{
  private:
    // OpenGL window
    min::window _win;

    // OpenGL pipeline
    min::shader _vertex;
    min::shader _fragment;
    min::program _prog;

    // Buffers for model data and textures
    min::vertex_buffer<float, uint32_t, min::ui_vertex> _sbuffer;
    min::texture_buffer _tbuffer;
    GLuint _img_id;

    // For casting rays
    min::aabbox<float, min::vec3> _world;
    min::vec3<float> _gravity;
    min::tree<float, uint_fast16_t, uint_fast32_t, min::vec3, min::aabbox, min::sphere> _tree;

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
        keyboard.register_keydown(min::window::key_code::KEYQ, ray_trace_test::close_window, (void *)&_win);
    }
    inline void load_image()
    {
        // Get the window size
        const uint_fast16_t win_width = _win.get_width();
        const uint_fast16_t win_height = _win.get_height();

        // Load BMP image
        const uint_fast16_t img_width = 512;
        const uint_fast16_t img_height = 512;
        min::bmp b = min::bmp(img_width, img_height, 3);

        // Ray trace the image
        ray_trace(b, img_width, img_height);

        // Load texture buffer
        _img_id = _tbuffer.add_bmp_texture(b);

        // Load textures
        uint_fast16_t width = 0;
        uint_fast16_t height = 0;
        if (win_width <= win_height)
        {
            // Calculate aspect ratio of image
            const float aspect_ratio = img_height / static_cast<float>(img_width);
            width = (win_width < img_width) ? win_width : img_width;
            height = static_cast<uint_fast16_t>(std::floor(aspect_ratio * width));
        }
        else
        {
            // Calculate aspect ratio of image
            const float aspect_ratio = img_width / static_cast<float>(img_height);
            height = (win_height < img_height) ? win_height : img_height;
            width = static_cast<uint_fast16_t>(std::floor(aspect_ratio * height));
        }

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
    inline void ray_trace(min::bmp &b, const uint_fast16_t width, const uint_fast16_t height)
    {
        // Create spheres
        std::vector<min::sphere<float, min::vec3>> spheres;
        spheres.reserve(10);
        spheres.emplace_back(min::vec3<float>(0.0, 0.0, 10.0), 2.0f);
        spheres.emplace_back(min::vec3<float>(-2.5, -2.5, 10.0), 1.0f);
        spheres.emplace_back(min::vec3<float>(-2.5, 2.5, 10.0), 1.0f);
        spheres.emplace_back(min::vec3<float>(2.5, -2.5, 10.0), 1.0f);
        spheres.emplace_back(min::vec3<float>(2.5, 2.5, 10.0), 1.0f);

        // Add spheres to the tree
        _tree.insert(spheres);

        // Create screen in 3D space
        const float x_slope = 2.0f / width;
        const float y_slope = 2.0f / height;

        // Create ray origin
        const min::vec3<float> origin;
        min::vec3<float> point;
        float min_d = 100.0f;
        float max_d = 0.0f;

        // Iterate over Y values
        for (uint32_t i = 0; i < height; i++)
        {
            // Calculate Y
            const float y_coord = -1.0f + (y_slope * i);
            const uint32_t row = i * height;

            // Iterate over X values
            for (uint32_t j = 0; j < width; j++)
            {
                // Calculate X
                const float x_coord = -1.0f + (x_slope * j);

                // Create a ray through the pixel
                const min::ray<float, min::vec3> r(origin, min::vec3<float>(x_coord, y_coord, 2.0));

                // Access the pixel index
                const uint32_t pixel_index = row + j;

                // Get the intersections with ray
                const auto inter = _tree.get_collisions(r);

                // If we hit the sphere
                if (inter.size() > 0)
                {
                    // Calculate distance
                    const float d = (inter[0].second - origin).magnitude();
                    min_d = std::min(min_d, d);
                    max_d = std::max(max_d, d);

                    // Calculate distance between [0, 1.0]
                    constexpr float denom = 11.0f - 8.0f;
                    const float dist_scale = (d - 8.0f) / denom;

                    // Calculate pixel color
                    constexpr float contrast = 4.0f;
                    const float color = 255.0f / (dist_scale * contrast + 1.0f);
                    const uint8_t value = static_cast<uint8_t>(color);

                    // Write pixel value
                    b.set(pixel_index, value, value, value);
                }
                else
                {
                    // Write black pixel
                    b.set(pixel_index, 0, 0, 0);
                }
            }
        }

        // Alert distance range for scaling pixel values
        std::cout << "Min Distance: " << min_d << std::endl;
        std::cout << "Max Distance: " << max_d << std::endl;
    }

  public:
    // Load window shaders and program
    ray_trace_test()
        : _win("Sphere Ray Tracer", 720, 480, 3, 3),
          _vertex("data/shader/ui.vertex", GL_VERTEX_SHADER),
          _fragment("data/shader/ui.fragment", GL_FRAGMENT_SHADER),
          _prog(_vertex, _fragment),
          _world(min::vec3<float>(-2000.0, -2000.0, -2000.0), min::vec3<float>(2000.0, 2000.0, 2000.0)),
          _gravity(0.0, -10.0, 0.0),
          _tree(_world)
    {
        // Set depth and cull settings
        min::settings::initialize();

        // Load the keyboard callbacks and settings
        load_keyboard();

        // Load image texture and vertices
        load_image();

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
        std::cout << "ray_trace_test: Shutdown called by user" << std::endl;
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

int load_ray_tracer()
{
    // Load window shaders and program, enable shader program
    ray_trace_test test;

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
        return load_ray_tracer();
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}
