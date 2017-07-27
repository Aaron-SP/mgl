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
#include <chrono>
#include <iostream>
#include <min/bmp.h>
#include <min/camera.h>
#include <min/convert.h>
#include <min/grid.h>
#include <min/loop_sync.h>
#include <min/oobbox.h>
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
    size_t _proj_view_id;
    size_t _view_id;

    // Physics simulation
    min::oobbox<float, min::vec3> _world;
    min::vec3<float> _gravity;
    min::physics<float, uint16_t, uint32_t, min::vec3, min::oobbox, min::oobbox, min::grid> _simulation;
    size_t _base_mesh;
    size_t _box_mesh;
    size_t _box_count;
    float _body_radius;

  public:
    // Load window shaders and program
    physics_test()
        : _win("Test oobb physics simulation", 720, 480, 3, 3),
          _vertex("data/shader/instance.vertex", GL_VERTEX_SHADER),
          _fragment("data/shader/instance.fragment", GL_FRAGMENT_SHADER),
          _prog(_vertex, _fragment),
          _ubuffer(10, 102),
          _world(min::vec3<float>(-115.0, 0.0, -115.0), min::vec3<float>(115.0, 115.0, 115.0)),
          _gravity(0.0, -10.0, 0.0),
          _simulation(_world, _gravity),
          _box_count(0),
          _body_radius(2.3)
    {
        // Set depth and cull settings
        min::settings::initialize();

        // Get the keyboard
        auto &keyboard = _win.get_keyboard();

        // Add keys to watch
        keyboard.add(min::window::key_code::KEYQ);
        keyboard.add(min::window::key_code::ENTER);

        // Register callback function for closing window by pressing 'Q'
        keyboard.register_keydown(min::window::key_code::KEYQ, physics_test::close_window, (void *)&_win);

        // Register callback function for launching physics projectile box by pressing 'ENTER'
        keyboard.register_keydown(min::window::key_code::ENTER, physics_test::launch, (void *)this);

        // Use the shader program to draw models
        _prog.use();

        // Set the collision elasticity of the physics simulation
        _simulation.set_elasticity(0.1);
    }
    void clear_background()
    {
        // black background
        const float color[] = {0.4, 0.4, 0.4, 1.0f};
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
    static void launch(void *ptr, double step)
    {
        // Call back function for launching box projectile
        // 'ptr' is passed in by us in constructor
        if (ptr)
        {
            // Cast to example class for loading a new box
            physics_test *test = reinterpret_cast<physics_test *>(ptr);

            // Launch the box
            test->load_launch_box();
        }
    }
    bool is_closed() const
    {
        return _win.get_shutdown();
    }
    void load_camera()
    {
        // Move and camera to +XZ and look at origin
        const min::vec3<float> pos = min::vec3<float>(100.0, 20.0, 100.0);
        const min::vec3<float> look = min::vec3<float>(0.0, 0.0, 0.0);

        // Create camera, set location and look at
        _cam.set_position(pos);
        _cam.set_look_at(look);
        auto &f = _cam.get_frustum();
        f.set_far(1000.0);
        _cam.set_perspective();
    }
    void load_meshes()
    {
        // load base which will act as the floor
        const min::aabbox<float, min::vec3> base_shape(min::vec3<float>(-115.0, -2.5, -115.0), min::vec3<float>(115.0, 2.5, 115.0));
        min::mesh<float, uint16_t> base_mesh = min::to_mesh<float, uint16_t>(base_shape);

        // Scale the uv texture for extra resolution
        base_mesh.scale_uv(50.0);

        // Add the base mesh to the vertex buffer
        _base_mesh = _sbuffer.add_mesh(base_mesh);

        // load oobbox model
        const min::aabbox<float, min::vec3> box_shape(min::vec3<float>(0.0, 0.0, 0.0) - _body_radius, min::vec3<float>(0.0, 0.0, 0.0) + _body_radius);
        const min::mesh<float, uint16_t> box_mesh = min::to_mesh<float, uint16_t>(box_shape);

        // Add mesh and update buffers
        _box_mesh = _sbuffer.add_mesh(box_mesh);

        // Upload both meshes to the GPU
        _sbuffer.upload();
    }
    void load_textures()
    {
        // Load textures
        const min::bmp b = min::bmp("data/texture/gimp.bmp");

        // Load texture buffer
        _bmp_id = _tbuffer.add_bmp_texture(b);
    }
    void load_launch_box()
    {
        // We can only launch 99 boxes
        if (_box_count < 99)
        {
            // Set x and Y value
            const min::vec3<float> position = _cam.get_position() + _cam.get_forward() * 10.0;

            // Create rigid body, make rigid body a little bigger to avoid small overlap
            const size_t box_id = _simulation.add_body(min::oobbox<float, min::vec3>(position - _body_radius, position + _body_radius), 10.0);

            // Get the rigid body and add linear velocity to the body
            min::body<float, min::vec3> &body = _simulation.get_body(box_id);
            body.set_linear_velocity(_cam.get_forward() * 10.0);
            body.set_angular_velocity(min::vec3<float>::up() * 1E2);

            // Set this body to be unrotatable
            body.set_no_rotate();

            // Set the model matrix
            const min::mat4<float> model(position);
            _model_id[_box_count + 1] = _ubuffer.add_matrix(model);

            // Increment the box projectile count
            _box_count++;

            // Alert that we launched a physics object
            std::cout << "physics_test: Launched box " << _box_count << std::endl;
        }
    }
    void load_physics_entities()
    {
        // Load the uniform buffer with program we will use
        _ubuffer.set_program(_prog);

        // Load light into uniform buffer
        min::vec4<float> light_color(1.0, 1.0, 1.0, 1.0);
        min::vec4<float> light_position(0.0, 20.0, 0.0, 1.0);
        min::vec4<float> light_power(0.5, 1.0, 1.0, 1.0);
        _ubuffer.add_light(min::light<float>(light_color, light_position, light_power));

        // Load projection and view matrix into uniform buffer
        _proj_view_id = _ubuffer.add_matrix(_cam.get_pv_matrix());
        _view_id = _ubuffer.add_matrix(_cam.get_v_matrix());

        // Create rigid body for base mesh which will act as floor, make mass very large!
        const size_t body_id = _simulation.add_body(min::oobbox<float, min::vec3>(min::vec3<float>(-115.0, -2.5, -115.0), min::vec3<float>(115.0, 2.5, 115.0)), 1E15);

        // Move body into place
        min::body<float, min::vec3> &body = _simulation.get_body(body_id);
        body.set_position(min::vec3<float>(0.0, 2.5, 0.0));

        // Set this body to be unmovable
        body.set_no_move();

        // Set this body to be unrotatable
        body.set_no_rotate();

        // Set the base mesh matrix with identity
        _model_id[0] = _ubuffer.add_matrix(min::mat4<float>());

        // Update the buffer with new transform matrices
        _ubuffer.update();
    }
    void set_title(const std::string &title)
    {
        _win.set_title(title);
    }
    void solve(const double frame_time, const double damping)
    {
        // Solve the simulation
        _simulation.solve(frame_time, damping);

        // Calculate the total energy of the system
        _simulation.get_total_energy();
    }

    void update_base()
    {
        // Get all rigid bodies in the scene
        const std::vector<min::body<float, min::vec3>> &bodies = _simulation.get_bodies();

        // Update the base matrix
        _ubuffer.set_matrix(min::mat4<float>(bodies[0].get_position(), bodies[0].get_rotation()), _model_id[0]);

        // Update the buffer with new transform matrices
        _ubuffer.update();
    }

    void update_bodies()
    {
        // Update the camera matrices
        _ubuffer.set_matrix(_cam.get_pv_matrix(), _proj_view_id);
        _ubuffer.set_matrix(_cam.get_v_matrix(), _view_id);

        // Get all rigid bodies in the scene
        const std::vector<min::body<float, min::vec3>> &bodies = _simulation.get_bodies();

        // Update base and box rigid body matrices
        for (size_t i = 0; i < _box_count; i++)
        {
            _ubuffer.set_matrix(min::mat4<float>(bodies[i + 1].get_position(), bodies[i + 1].get_rotation()), _model_id[i]);
        }

        // Update the buffer with new transform matrices
        _ubuffer.update();
    }
    void update_camera()
    {
        // Get the cursor coordinates
        auto c = _win.get_cursor();

        // Get the offset from screen center
        float sensitivity = 0.1;
        float x = sensitivity * (c.first - (_win.get_width() / 2));
        float y = sensitivity * (c.second - (_win.get_height() / 2));

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
        uint16_t h = _win.get_height();
        uint16_t w = _win.get_width();

        // Center cursor in middle of window
        _win.set_cursor(w / 2, h / 2);
    }
    void draw()
    {
        // Bind VAO, it actually is already bound and could be removed
        _sbuffer.bind();

        // Bind the bmp for drawing
        _tbuffer.bind(_bmp_id, 0);

        // Update the base model matrix
        update_base();

        // Draw 1 base mesh to act as the floor
        _sbuffer.draw_many(GL_TRIANGLES, _base_mesh, 1);

        // Update the box model matrices
        update_bodies();

        if (_box_count > 0)
        {
            // Draw N instances of object
            _sbuffer.draw_many(GL_TRIANGLES, _box_mesh, _box_count);
        }
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

    // Clear the background color
    test.clear_background();

    // Load model from files
    test.load_meshes();

    // Load textures from files
    test.load_textures();

    // Load the camera properties
    test.load_camera();

    // Load light and rigid body data
    test.load_physics_entities();

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
                test.solve(frame_time / 30.0, 10.0);
            }

            // Clear the background color
            test.clear_background();

            // Update the camera movement
            test.update_camera();

            // Draw the model
            test.draw();

            // Update the window after draw command
            test.window_update();

            // Calculate needed delay to hit target
            frame_time = sync.sync();
        }

        // Calculate the number of 'average' frames per second
        double fps = sync.get_fps();

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
