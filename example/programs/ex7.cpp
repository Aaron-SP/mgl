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
#include <min/aabbox.h>
#include <min/bmp.h>
#include <min/camera.h>
#include <min/convert.h>
#include <min/grid.h>
#include <min/loop_sync.h>
#include <min/physics.h>
#include <min/program.h>
#include <min/settings.h>
#include <min/shader.h>
#include <min/static_vertex.h>
#include <min/texture_buffer.h>
#include <min/uniform_buffer.h>
#include <min/vertex_buffer.h>
#include <min/wavefront.h>
#include <min/window.h>
#include <random>

class physics_test
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

    // Physics simulation
    min::aabbox<float, min::vec3> _world;
    min::vec3<float> _gravity;
    min::physics<float, uint_fast16_t, uint_fast32_t, min::vec3, min::aabbox, min::aabbox, min::grid> _simulation;
    float _body_radius;

    void load_camera()
    {
        // Move and camera to +Z and look at origin
        const min::vec3<float> pos = min::vec3<float>(0.0, 0.0, 300.0);
        const min::vec3<float> look = min::vec3<float>(0.0, 0.0, 0.0);

        // Create camera, set location and look at
        _cam.set(pos, look, min::vec3<float>::up());
        auto &f = _cam.get_frustum();
        f.set_far(500.0);
        _cam.set_perspective();
    }
    void load_keyboard()
    {
        // Set ability to close out of application by pressing 'Q'
        auto &keyboard = _win.get_keyboard();

        // Add key to watch
        keyboard.add(min::window::key_code::KEYQ);

        // Register callback function for closing window
        keyboard.register_keydown(min::window::key_code::KEYQ, physics_test::close_window, (void *)&_win);
    }

    void load_model_texture()
    {
        // load aabbox model
        const min::aabbox<float, min::vec3> shape(min::vec3<float>(0.0, 0.0, 0.0) - _body_radius, min::vec3<float>(0.0, 0.0, 0.0) + _body_radius);
        const min::mesh<float, uint16_t> box_mesh = min::to_mesh<float, uint16_t>(shape);

        // Load textures
        const min::bmp b = min::bmp("data/texture/stone.bmp");

        // Load texture buffer
        _bmp_id = _tbuffer.add_bmp_texture(b);

        // Add mesh and update buffers
        _sbuffer.add_mesh(box_mesh);
        _sbuffer.upload();
    }
    void load_uniforms()
    {
        // Load the uniform buffer with program we will use
        _ubuffer.set_program_lights(_prog);
        _ubuffer.set_program_matrix(_prog);

        // Bind this uniform buffer for use
        _ubuffer.bind();

        // Load light into uniform buffer
        const min::vec4<float> light_color(1.0, 1.0, 1.0, 1.0);
        const min::vec4<float> light_position(0.0, 0.0, 300.0, 1.0);
        const min::vec4<float> light_power(0.25, 300.0, 1.0, 1.0);
        _ubuffer.add_light(min::light<float>(light_color, light_position, light_power));

        // Load projection and view matrix into uniform buffer
        _ubuffer.add_matrix(_cam.get_pv_matrix());
        _ubuffer.add_matrix(_cam.get_v_matrix());

        // Generate random positions in a circle
        // The objects will be between radius
        const float radius = 5.0;
        std::uniform_real_distribution<float> position(-radius, radius);
        std::uniform_real_distribution<float> offset(0.0, 0.8 * radius);
        // Mersenne Twister: Good quality random number generator
        std::mt19937 rng;
        // Initialize with fixed seed
        rng.seed(101129);

        // Creates aabb's
        const float radius2 = radius * radius;
        bool flag = true;
        for (size_t i = 0; i < 100; i++)
        {
            // Generate 100 instances of aabbox, with random position and rotation around circle
            const float x = position(rng);
            float y = std::sqrt(radius2 - x * x) - offset(rng);
            if (flag)
            {
                y *= -1.0;
            }
            flag = !flag;

            // Set x and Y value
            const min::vec3<float> translation(x, y, 0);

            // Create rigid body, make rigid body a little bigger to avoid small overlap
            const size_t body = _simulation.add_body(min::aabbox<float, min::vec3>(translation - _body_radius - 0.1, translation + _body_radius + 0.1), 100.0);

            // We do this to prevent AABB simulation from getting stuck since normals can only be aligned to axises
            _simulation.get_body(body).set_linear_velocity(translation);
            const min::mat4<float> model = min::mat4<float>(translation);

            // Set the model matrix
            _model_id[i] = _ubuffer.add_matrix(model);
        }

        // Load the buffer with data
        _ubuffer.update();
    }

  public:
    // Load window shaders and program
    physics_test()
        : _win("Test aabb physics simulation", 720, 480, 3, 3),
          _vertex("data/shader/instance.vertex", GL_VERTEX_SHADER),
          _fragment("data/shader/instance.fragment", GL_FRAGMENT_SHADER),
          _prog(_vertex, _fragment),
          _ubuffer(10, 102, 0),
          _world(min::vec3<float>(-115, -115, -115), min::vec3<float>(115, 115, 115)),
          _gravity(0.0, -10.0, 0.0),
          _simulation(_world, _gravity),
          _body_radius(2.3)
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
    void clear_background()
    {
        // black background
        const float color[] = {0.1, 0.1, 0.1, 1.0f};
        glClearBufferfv(GL_COLOR, 0, color);
        glClear(GL_DEPTH_BUFFER_BIT);
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
        std::cout << "physics_test: Shutdown called by user" << std::endl;
    }
    bool is_closed() const
    {
        return _win.get_shutdown();
    }
    void set_title(const std::string &title)
    {
        _win.set_title(title);
    }
    void solve(const double frame_time, const double damping)
    {
        // Solve the simulation
        _simulation.solve(frame_time, damping);

        _simulation.get_total_energy();
    }
    void update_bodies()
    {
        // Get all rigid bodies
        const std::vector<min::body<float, min::vec3>> &bodies = _simulation.get_bodies();

        // Update rigid bodies transform matrix
        for (size_t i = 0; i < 100; i++)
        {
            _ubuffer.set_matrix(min::mat4<float>(bodies[i].get_position(), bodies[i].get_rotation()), _model_id[i]);
        }

        // Update the buffer with new transform
        _ubuffer.update();
    }
    void draw()
    {
        // Bind VAO, it actually is already bound and could be removed
        _sbuffer.bind();

        // Bind the bmp for drawing
        _tbuffer.bind(_bmp_id, 0);

        // Use the shader program to draw models
        _prog.use();

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

int test_render_loop()
{
    // Load window shaders and program, enable shader program
    physics_test test;

    // Setup controller to run at 60 frames per second
    const int frames = 60;
    min::loop_sync sync(frames);

    // User can close with Q or use window manager
    double frame_time = 0.0;
    while (!test.is_closed())
    {
        for (int i = 0; i < frames; i++)
        {
            // Start synchronizing the loop
            sync.start();

            // Update rigid bodies in simulation
            for (int i = 0; i < 30; i++)
            {
                test.solve(frame_time / 30.0, 0.01);
            }

            // Update the bodies buffer information
            test.update_bodies();

            // Clear the background color
            test.clear_background();

            // Draw the model
            test.draw();

            // Update the window after draw command
            test.window_update();

            // Calculate needed delay to hit target
            frame_time = sync.sync();
        }

        // Calculate the number of 'average' frames per second
        const double fps = sync.get_fps();

        // Update the window title with FPS count of last frame
        test.set_title("Test oobb physics simulation: FPS: " + std::to_string(fps));
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
