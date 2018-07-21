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
#include <iostream>
#include <limits>
#include <min/bmp.h>
#include <min/camera.h>
#include <min/loop_sync.h>
#include <min/program.h>
#include <min/settings.h>
#include <min/shader.h>
#include <min/static_vertex.h>
#include <min/texture_buffer.h>
#include <min/uniform_buffer.h>
#include <min/utility.h>
#include <min/vertex_buffer.h>
#include <min/window.h>
#include <string>

class render_loop_test
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
    size_t _proj_view_id;
    size_t _view_id;
    size_t _model_id;

    // Model matrix for rotating suzanne
    min::mat4<float> _model_matrix;

    void load_camera()
    {
        // Move and camera to -X and look at origin
        const min::vec3<float> pos = min::vec3<float>(-5.0, 2.0, 0.0);
        const min::vec3<float> look = min::vec3<float>(0.0, 0.0, 0.0);

        // Test perspective projection
        // Create camera, set location and look at
        _cam.set(pos, look, min::vec3<float>::up());
        _cam.set_perspective();
    }
    void load_keyboard()
    {
        // Set ability to close out of application by pressing 'Q'
        auto &keyboard = _win.get_keyboard();

        // Add key to watch
        keyboard.add(min::window::key_code::KEYQ);

        // Register callback function for closing window
        keyboard.register_keydown(min::window::key_code::KEYQ, render_loop_test::close_window, (void *)&_win);
    }
    void load_model_texture()
    {
        // Warn user we are opening a large model
        std::cout << "Opening a very large model: blender_suzanne.bmesh" << std::endl;

        // Since we are using a BMESH, assert floating point compatibility
        static_assert(std::numeric_limits<float>::is_iec559, "IEEE 754 float required");
        static_assert(sizeof(float) == 4, "32 bit IEEE 754 float required");

        // load blender-suzanne model
        min::mesh<float, uint32_t> suzanne("suzanne");
        suzanne.from_file("data/models/blender_suzanne.bmesh");
        suzanne.scale_uv(10.0);
        suzanne.calculate_tangents();

        // Load textures
        const min::bmp b = min::bmp("data/texture/stone.bmp");

        // Query max texture size
        const size_t size = _tbuffer.get_max_texture_size();
        std::cout << "Max texture size is: " << size << "x" << size << std::endl;

        // Load texture buffer
        _bmp_id = _tbuffer.add_bmp_texture(b);

        // Add mesh and update buffers
        _sbuffer.add_mesh(suzanne);

        // Load buffer with data
        _sbuffer.upload();
    }
    void load_uniforms()
    {
        const size_t size = _ubuffer.get_max_buffer_size();
        std::cout << "Max uniform buffer size(bytes) is: " << size << std::endl;

        // Load light into uniform buffer
        const min::vec4<float> light_color(1.0, 1.0, 1.0, 1.0);
        const min::vec4<float> light_position(-2.0, 2.0, 0.0, 1.0);
        const min::vec4<float> light_power(0.1, 6.0, 5.0, 1.0);
        _ubuffer.add_light(min::light<float>(light_color, light_position, light_power));

        // Load projection and view matrix into uniform buffer
        _proj_view_id = _ubuffer.add_matrix(_cam.get_pv_matrix());
        _view_id = _ubuffer.add_matrix(_cam.get_v_matrix());

        // Get model ID for later use
        _model_id = _ubuffer.add_matrix(min::mat4<float>());

        // Load the uniform buffer with program we will use
        _ubuffer.set_program_lights(_prog);
        _ubuffer.set_program_matrix(_prog);

        // Bind this uniform buffer for use
        _ubuffer.bind();

        // Load the buffer with data
        _ubuffer.update();
    }

  public:
    // Load window shaders and program
    render_loop_test()
        : _win("Example render loop with first person camera", 720, 480, 3, 3),
          _vertex("data/shader/light.vertex", GL_VERTEX_SHADER),
          _fragment("data/shader/light.fragment", GL_FRAGMENT_SHADER),
          _prog(_vertex, _fragment),
          _ubuffer(100, 100, 0)
    {
        // Set depth and cull settings
        min::settings::initialize();

        // Load the camera
        load_camera();

        // Load the keyboard callbacks and settings
        load_keyboard();

        // Load model and textures from files
        load_model_texture();

        // Load uniform buffers with light and model matrix
        load_uniforms();

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
        std::cout << "render_loop_test: Shutdown called by user" << std::endl;
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
    void draw()
    {
        // Bind VAO
        _sbuffer.bind();

        // Bind this texture for drawing
        _tbuffer.bind(_bmp_id, 0);

        // Rotate the model around the Z axis
        _model_matrix *= min::mat4<float>(min::mat2<float>(2.0));

        // Update matrix uniforms
        _ubuffer.set_matrix(_cam.get_pv_matrix(), _proj_view_id);
        _ubuffer.set_matrix(_cam.get_v_matrix(), _view_id);
        _ubuffer.set_matrix(_model_matrix, _model_id);

        // Update the data in the uniform buffer
        _ubuffer.update();

        // Use the shader program to draw models
        _prog.use();

        // Draw blender-suzanne
        _sbuffer.draw(GL_TRIANGLES, 0);
    }
    void set_title(const std::string &title)
    {
        _win.set_title(title);
    }
    void update_camera()
    {
        // Get the cursor coordinates
        const auto c = _win.get_cursor();

        // Get the offset from screen center
        const uint_fast16_t w2 = _win.get_width() / 2;
        const uint_fast16_t h2 = _win.get_height() / 2;
        const int_fast16_t dx = c.first - w2;
        const int_fast16_t dy = c.second - h2;
        constexpr float sensitivity = 0.25;
        float x = dx * sensitivity;
        float y = dy * sensitivity;

        // If the mouse coordinates moved at all
        if (std::abs(x) > 1E-3 || std::abs(y) > 1E-3)
        {
            // Limit maximum jump
            min::clamp<float>(x, -2.0, 2.0);
            min::clamp<float>(y, -2.0, 2.0);

            // Adjust the camera by the offset from screen center
            _cam.move_look_at(x, y);

            // Move the cursor back
            update_cursor();
        }
    }
    void update_cursor()
    {
        // Get the screen dimensions
        const uint_fast16_t h = _win.get_height();
        const uint_fast16_t w = _win.get_width();

        // Center cursor in middle of window
        _win.set_cursor(w / 2, h / 2);
    }
    void window_update()
    {
        // Update and swap buffers
        _win.update();
        _win.swap_buffers();
    }
};

int test_render_loop()
{
    // Load window shaders and program, enable shader program
    render_loop_test test;

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

            // Update the camera movement
            test.update_camera();

            // Draw the model
            test.draw();

            // Update the window after draw command
            test.window_update();

            // Calculate needed delay to hit target
            sync.sync();
        }

        // Calculate the number of 'average' frames per second
        const double fps = sync.get_fps();

        // Update the window title with FPS count of last frame
        test.set_title("Example render loop with first person camera: FPS: " + std::to_string(fps));
    }

    return 0;
}

int main()
{
    try
    {
        return test_render_loop();
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}
