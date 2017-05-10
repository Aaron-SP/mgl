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
#include <iostream>
#include <min/bmp.h>
#include <min/camera.h>
#include <min/emitter_buffer.h>
#include <min/loop_sync.h>
#include <min/program.h>
#include <min/settings.h>
#include <min/shader.h>
#include <min/texture_buffer.h>
#include <min/uniform_buffer.h>
#include <min/wavefront.h>
#include <min/window.h>
#include <string>

class particle_test
{
  private:
    // OpenGL pipeline
    min::window _win;
    min::shader _vertex;
    min::shader _fragment;
    min::program _prog;

    // Buffers for particle data and textures
    min::emitter_buffer<float, GL_FLOAT> _ebuffer;
    min::texture_buffer _tbuffer;
    GLuint _bmp_id;

    // Camera and uniform data
    min::camera<float> _cam;
    min::uniform_buffer<float> _ubuffer;

    // Force setting for toggling between radial and linear
    int _force_type;

  public:
    // Load window shaders and program
    particle_test()
        : _win("Example particle system", 720, 480, 3, 3),
          _vertex("data/shader/emitter.vertex", GL_VERTEX_SHADER),
          _fragment("data/shader/emitter.fragment", GL_FRAGMENT_SHADER),
          _prog(_vertex, _fragment),
          _ebuffer(min::vec3<float>(), 1000, 0.1, 5.0),
          _ubuffer(0, 1),
          _force_type(0)
    {
        // Set depth and cull settings
        min::settings::initialize();

        // Use the shader program to draw models
        _prog.use();

        // Set ability to close out of application by pressing 'Q'
        auto &keyboard = _win.get_keyboard();

        // Add Q key to watch
        keyboard.add(min::window::key_code::KEYQ);

        // Add ENTER key to watch
        keyboard.add(min::window::key_code::ENTER);

        // Register callback function for closing window
        keyboard.register_keydown(min::window::key_code::KEYQ, particle_test::close_window, (void *)&_win);

        // Register callback function for changing force type
        keyboard.register_keydown(min::window::key_code::ENTER, particle_test::toggle_force, (void *)this);
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
        std::cout << "particle_test: Shutdown called by user" << std::endl;
    }
    static void toggle_force(void *ptr, double step)
    {
        // Call back function for setting force
        // 'ptr' is passed in by us in constructor
        if (ptr)
        {
            // Cast to particle_test pointer and set force
            particle_test *test = reinterpret_cast<particle_test *>(ptr);
            test->set_force();
        }

        // Alert that we received the call back
        std::cout << "particle_test: Changing particle system force type" << std::endl;
    }
    void clear_background() const
    {
        // blue background
        const float color[] = {0.10f, 0.10f, 0.10f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, color);
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    bool is_closed() const
    {
        return _win.get_shutdown();
    }
    void load_emitter_texture()
    {
        // Load textures
        const min::bmp b = min::bmp("data/texture/stone.bmp");

        // Load texture buffer
        _bmp_id = _tbuffer.add_bmp_texture(b);

        // Lower the default gravity
        _ebuffer.set_gravity(min::vec3<float>(0.0, -5.0, 0.0));
        _ebuffer.set_speed(min::vec3<float>(0.0, 1.0, 0.0));

        // Load buffer with data
        _ebuffer.upload();
    }
    void load_camera_uniforms()
    {
        // Move and camera to -X, +Y and look at origin
        min::vec3<float> pos = min::vec3<float>(-1.0, 40.0, 0.0);
        min::vec3<float> look = min::vec3<float>(0.0, 0.0, 0.0);

        // Test perspective projection
        // Create camera, set location and look at
        _cam.set_position(pos);
        _cam.set_look_at(look);
        _cam.set_perspective();

        // Load the uniform buffer with program we will use
        _ubuffer.set_program(_prog);

        // Load projection matrix into uniform buffer
        _ubuffer.add_matrix(_cam.get_pv_matrix());

        // Load the buffer with data
        _ubuffer.update();
    }
    void draw()
    {
        // Bind VAO
        _ebuffer.bind();

        // Bind this texture for drawing
        _tbuffer.bind(_bmp_id, 0);

        // Draw the particles
        _ebuffer.draw();
    }
    void set_force()
    {
        // Iterate through the force profiles
        _force_type++;

        // Wrap around to start
        _force_type = _force_type % 3;

        // If it is a radial force, disable gravity
        if (_force_type == 0)
        {
            _ebuffer.set_gravity(min::vec3<float>(0.0, -5.0, 0.0));
            _ebuffer.attractor_clear();
        }
        else if (_force_type == 1)
        {
            _ebuffer.set_gravity(min::vec3<float>(0.0, 0.0, 0.0));
            _ebuffer.attractor_clear();
            _ebuffer.attractor_add(min::vec3<float>(0.0, 0.0, 0.0), 1.0);
        }
        else if (_force_type == 2)
        {
            _ebuffer.set_gravity(min::vec3<float>(0.0, 0.0, 0.0));
            _ebuffer.attractor_clear();
            _ebuffer.attractor_add(min::vec3<float>(10.0, 0.0, 15.0), 0.1);
            _ebuffer.attractor_add(min::vec3<float>(-10.0, 0.0, -15.0), 0.1);
        }
    }
    void set_title(const std::string &title)
    {
        _win.set_title(title);
    }
    void step(float dt)
    {
        // Update the particle positions
        _ebuffer.step(dt);

        // Upload data to GPU
        _ebuffer.upload();
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
    particle_test test;

    // Load model and textures from files
    test.load_emitter_texture();

    // Load the camera and fill uniform buffers with light and model matrix
    test.load_camera_uniforms();

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

            // Draw the model
            test.draw();

            // Update the window after draw command
            test.window_update();

            // Update the particles
            test.step(sync.sync());
        }

        // Calculate the number of 'average' frames per second
        double fps = sync.get_fps();

        // Update the window title with FPS count of last frame
        test.set_title("Example particle system: FPS: " + std::to_string(fps));
    }

    return 0;
}

int main()
{
    return test_render_loop();
}