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
#include <min/bmp.h>
#include <min/camera.h>
#include <min/convert.h>
#include <min/grid.h>
#include <min/loop_sync.h>
#include <min/md5_mesh.h>
#include <min/md5_model.h>
#include <min/oobbox.h>
#include <min/physics.h>
#include <min/program.h>
#include <min/settings.h>
#include <min/shader.h>
#include <min/skeletal_vertex.h>
#include <min/static_vertex.h>
#include <min/texture_buffer.h>
#include <min/uniform_buffer.h>
#include <min/vertex_buffer.h>
#include <min/wavefront.h>
#include <min/window.h>
#include <random>

class character
{
  private:
    min::shader _vertex;
    min::shader _fragment;
    min::program _prog;

    // md5 model
    min::md5_model<float, uint32_t, min::vec4, min::aabbox> _md5_model;

    // Buffers for model data and textures
    min::vertex_buffer<float, uint32_t, min::skeletal_vertex, GL_FLOAT, GL_UNSIGNED_INT> _skbuffer;
    min::texture_buffer _texture_buffer;
    GLuint _bmp_id;

    // Camera and uniform data
    min::uniform_buffer<float> _ubuffer;
    size_t _proj_view_id;
    size_t _view_id;
    size_t _model_id;

    // Light properties for rendering model
    min::vec4<float> _light_color;
    min::vec4<float> _light_position;
    min::vec4<float> _light_power;
    size_t _light_id;

    // For animating md5 model bones
    std::vector<size_t> _bone_id;

    // Bounding box for model
    min::mat4<float> _model_matrix;
    min::aabbox<float, min::vec3> _box;

    void load_model()
    {
        // Load animation
        _md5_model.load_animation("data/models/mech_warrior_walk.md5anim");

        // Setup the md5 mesh
        min::mesh<float, uint32_t> &md5 = _md5_model.get_meshes()[0];
        md5.calculate_normals();
        md5.calculate_tangents();

        // Get bounding box of first frame of animation
        _box = _md5_model.get_current_animation().get_bounds()[0];

        // Bind VAO
        _skbuffer.bind();

        // Add mesh and update buffers
        _skbuffer.add_mesh(md5);

        // Load vertex buffer with data
        _skbuffer.upload();
    }
    void load_textures()
    {
        // Load textures
        const min::bmp b = min::bmp("data/texture/mech_warrior.bmp");

        // Set the texture channel for this program, we need to do this here because we render textures on channel '0'
        // _prog will be in use by the end of this call
        _texture_buffer.set_texture_uniform(_prog, "in_texture", 1);

        // Load texture buffer
        _bmp_id = _texture_buffer.add_bmp_texture(b, true);
    }
    void load_uniforms()
    {
        // Load light into uniform buffer
        _light_id = _ubuffer.add_light(min::light<float>(_light_color, _light_position, _light_power));

        // Load projection and view matrix into uniform buffer
        _proj_view_id = _ubuffer.add_matrix(min::mat4<float>());
        _view_id = _ubuffer.add_matrix(min::mat4<float>());

        // Get model ID for later use
        _model_id = _ubuffer.add_matrix(_model_matrix);

        // Add bones matrices to uniform buffer
        for (const auto &bone : _md5_model.get_bones())
        {
            size_t bone_id = _ubuffer.add_matrix(bone);
            _bone_id.push_back(bone_id);
        }

        // Load the uniform buffer with the program we will use
        _ubuffer.set_program_lights(_prog);
        _ubuffer.set_program_matrix(_prog);
    }

  public:
    character() : _vertex("data/shader/md5.vertex", GL_VERTEX_SHADER),
                  _fragment("data/shader/md5.fragment", GL_FRAGMENT_SHADER),
                  _prog(_vertex, _fragment),
                  _md5_model(min::md5_mesh<float, uint32_t>("data/models/mech_warrior.md5mesh")),
                  _ubuffer(1, 100, 0),
                  _light_color(1.0, 1.0, 1.0, 1.0),
                  _light_position(0.0, 40.0, 0.0, 1.0),
                  _light_power(0.1, 1000.0, 10.0, 1.0)

    {
        // Load md5 model
        load_model();

        // Load md5 model textures
        load_textures();

        // Load the md5 uniforms
        load_uniforms();
    }
    void draw(min::camera<float> &cam, const double time_step)
    {
        // Update matrix uniforms
        _ubuffer.set_matrix(cam.get_pv_matrix(), _proj_view_id);
        _ubuffer.set_matrix(cam.get_v_matrix(), _view_id);
        _ubuffer.set_matrix(_model_matrix, _model_id);

        // Update the md5 animation
        _md5_model.step(time_step);

        // update model bones matrices
        const auto &bones = _md5_model.get_bones();
        const size_t size = bones.size();
        for (size_t i = 0; i < size; i++)
        {
            _ubuffer.set_matrix(bones[i], _bone_id[i]);
        }

        // Bind this uniform buffer for use
        _ubuffer.bind();

        // Update the matrix and light buffer
        _ubuffer.update();

        // Bind VAO
        _skbuffer.bind();

        // Bind this texture for drawing on channel '0'
        _texture_buffer.bind(_bmp_id, 1);

        // Change program back to md5 shaders
        _prog.use();

        // Draw md5 model
        _skbuffer.draw(GL_TRIANGLES, 0);
    }
    const min::aabbox<float, min::vec3> &get_box() const
    {
        return _box;
    }
    void set_model_matrix(const min::mat4<float> &m)
    {
        _model_matrix = m;
    }
    void set_animation_count(const unsigned count)
    {
        // Trigger animation
        _md5_model.get_current_animation().set_loop_count(count);
    }
};

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
    GLuint _base_id;
    GLuint _box_id;

    // Camera and uniform data
    min::camera<float> _cam;
    min::uniform_buffer<float> _ubuffer;
    size_t _model_id[100];
    size_t _proj_view_id;
    size_t _view_id;

    // Physics simulation
    min::oobbox<float, min::vec3> _world;
    min::vec3<float> _gravity;
    min::physics<float, uint_fast16_t, uint_fast32_t, min::vec3, min::oobbox, min::oobbox, min::grid> _simulation;
    size_t _base_mesh;
    size_t _box_mesh;
    size_t _box_count;
    float _body_radius;

    // MD5 character
    character _md5_char;
    min::vec3<float> _forward;

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
        f.set_fov(90.0);
        _cam.set_perspective();
    }
    void load_keyboard()
    {
        // Get the keyboard
        auto &keyboard = _win.get_keyboard();

        // Add keys to watch
        keyboard.add(min::window::key_code::KEYQ);
        keyboard.add(min::window::key_code::ENTER);
        keyboard.add(min::window::key_code::KEYW);

        // Register callback function for closing window by pressing 'Q'
        keyboard.register_keydown(min::window::key_code::KEYQ, physics_test::close_window, (void *)&_win);

        // Register callback function for launching physics projectile box by pressing 'ENTER'
        keyboard.register_keydown(min::window::key_code::ENTER, physics_test::launch, (void *)this);

        // Register callback function W
        keyboard.register_keydown_per_frame(min::window::key_code::KEYW, physics_test::forward, (void *)this);
    }
    void load_meshes()
    {
        // load base which will act as the floor
        const min::aabbox<float, min::vec3> base_shape(min::vec3<float>(-115.0, -2.5, -115.0), min::vec3<float>(115.0, 2.5, 115.0));
        min::mesh<float, uint16_t> base_mesh = min::to_mesh<float, uint16_t>(base_shape);

        // Scale the uv texture for extra resolution
        base_mesh.scale_uv(50.0);

        // Bind VAO
        _sbuffer.bind();

        // Add the base mesh to the vertex buffer
        _base_mesh = _sbuffer.add_mesh(base_mesh);

        // load oobbox cube model
        min::wavefront<float, uint16_t> wave("data/models/art_cube.obj");
        std::vector<min::mesh<float, uint16_t>> meshes = wave.get_meshes();
        min::mesh<float, uint16_t> &box_mesh = meshes[0];
        box_mesh.calculate_tangents();

        // Add mesh and update buffers
        _box_mesh = _sbuffer.add_mesh(box_mesh);

        // Upload both meshes to the GPU
        _sbuffer.upload();
    }
    void load_launch_box()
    {
        // We can only launch 99 boxes
        if (_box_count < 99)
        {
            // Set x and Y value
            const min::vec3<float> position = _cam.get_position() + _cam.get_forward() * 15.0;

            // Create rigid body, make rigid body a little bigger to avoid small overlap
            const size_t box_id = _simulation.add_body(min::oobbox<float, min::vec3>(position - _body_radius, position + _body_radius), 10.0);

            // Get the rigid body and add linear velocity to the body
            min::body<float, min::vec3> &body = _simulation.get_body(box_id);
            body.set_linear_velocity(_cam.get_forward() * 10.0);
            body.set_angular_velocity(min::vec3<float>::up() * 100.0);

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
    void load_textures()
    {
        // Load textures
        const min::bmp base = min::bmp("data/texture/stone.bmp");
        const min::bmp box = min::bmp("data/texture/art_cube.bmp");

        // Load texture buffer
        _base_id = _tbuffer.add_bmp_texture(base, true);
        _box_id = _tbuffer.add_bmp_texture(box, true);
    }
    void load_physics_entities()
    {
        // Load light into uniform buffer
        min::vec4<float> light_color(1.0, 0.75, 0.0, 1.0);
        min::vec4<float> light_position(0.0, 20.0, 0.0, 1.0);
        min::vec4<float> light_power(0.1, 100.0, 1.0, 1.0);
        _ubuffer.add_light(min::light<float>(light_color, light_position, light_power));

        // Load projection and view matrix into uniform buffer
        _proj_view_id = _ubuffer.add_matrix(_cam.get_pv_matrix());
        _view_id = _ubuffer.add_matrix(_cam.get_v_matrix());

        // Set the collision elasticity of the physics simulation
        _simulation.set_elasticity(0.1);

        // Create rigid body for base mesh which will act as floor, make mass very large!
        const size_t body1_id = _simulation.add_body(min::oobbox<float, min::vec3>(min::vec3<float>(-115.0, -2.5, -115.0), min::vec3<float>(115.0, 2.5, 115.0)), 1E15);

        // Move box body into place
        min::body<float, min::vec3> &body1 = _simulation.get_body(body1_id);
        body1.set_position(min::vec3<float>(0.0, 2.5, 0.0));

        // Set this body to be unmovable
        body1.set_no_move();

        // Set this body to be unrotatable
        body1.set_no_rotate();

        // Set the base mesh matrix with identity
        _model_id[0] = _ubuffer.add_matrix(min::mat4<float>());

        // Add md5 model rigid body into simulation
        min::aabbox<float, min::vec3> md5_box = _md5_char.get_box();
        const size_t body2_id = _simulation.add_body(min::oobbox<float, min::vec3>(md5_box.get_min(), md5_box.get_max()), 100.0);

        // Move md5 body into place, and set it standing upright, move to start position
        min::body<float, min::vec3> &body2 = _simulation.get_body(body2_id);
        body2.set_rotation(min::quat<float>(min::vec3<float>(1.0, 0.0, 0.0), -90.0));
        body2.set_position(min::vec3<float>(90.0, 20.0, 90.0));

        // Set this body to be unrotatable
        body2.set_no_rotate();

        // Load the uniform buffer with program we will use
        _ubuffer.set_program_lights(_prog);
        _ubuffer.set_program_matrix(_prog);
    }
    void update_cursor()
    {
        // Get the screen dimensions
        uint_fast16_t h = _win.get_height();
        uint_fast16_t w = _win.get_width();

        // Center cursor in middle of window
        _win.set_cursor(w / 2, h / 2);
    }
    void update_base()
    {
        // Get all rigid bodies in the scene
        const std::vector<min::body<float, min::vec3>> &bodies = _simulation.get_bodies();

        // Update the base matrix
        _ubuffer.set_matrix(min::mat4<float>(bodies[0].get_position(), bodies[0].get_rotation()), _model_id[0]);

        // Bind this uniform buffer for use
        _ubuffer.bind();

        // Update the buffer with new transform matrices
        _ubuffer.update();
    }
    void update_instances()
    {
        // Update the camera matrices
        _ubuffer.set_matrix(_cam.get_pv_matrix(), _proj_view_id);
        _ubuffer.set_matrix(_cam.get_v_matrix(), _view_id);

        // Get all rigid bodies in the scene
        const std::vector<min::body<float, min::vec3>> &bodies = _simulation.get_bodies();

        // Update base and box rigid body matrices
        for (size_t i = 0; i < _box_count; i++)
        {
            _ubuffer.set_matrix(min::mat4<float>(bodies[i + 2].get_position(), bodies[i + 2].get_rotation()), _model_id[i]);
        }

        // Bind this uniform buffer for use
        _ubuffer.bind();

        // Update the buffer with new transform matrices
        _ubuffer.update();
    }
    void update_md5_mesh(min::camera<float> &cam)
    {
        // Calculate the forward vector in the XZ plane
        _forward = cam.get_forward();
        _forward.y(0.0);
        _forward.normalize();

        // Calculate the angle between -X, which model is pointing at at t=0
        float angle = min::rad_to_deg<float>(std::acos(_forward.dot(min::vec3<float>(-1.0, 0.0, 0.0))));
        if (_forward.z() < 0.0)
        {
            angle *= -1.0;
        }

        // Transform the md5 model rotation around Y axis by angle
        min::quat<float> rotation(min::vec3<float>::up(), angle);

        // Get all rigid bodies in the scene
        std::vector<min::body<float, min::vec3>> &bodies = _simulation.get_bodies();

        // Rotate md5 body by camera rotation
        min::quat<float> rotx(min::vec3<float>(1.0, 0.0, 0.0), -90.0);
        bodies[1].set_rotation(rotation * rotx);

        // Update the md5 model matrix
        min::mat4<float> model(bodies[1].get_position(), bodies[1].get_rotation());
        _md5_char.set_model_matrix(model);

        // Set camera on top of md5 mesh
        min::vec3<float> offset(10.0, 0.0, 10.0);
        cam.set_position(model.transform(offset));
    }
    void walk_forward()
    {
        // Get all rigid bodies in the scene
        std::vector<min::body<float, min::vec3>> &bodies = _simulation.get_bodies();

        // Add linear velocity to body in forward direction
        bodies[1].set_linear_velocity(_forward * 10.0);

        // Trigger animation for one cycle
        _md5_char.set_animation_count(1);
    }

  public:
    // Load window shaders and program
    physics_test()
        : _win("Test md5 physics simulation", 720, 480, 3, 3),
          _vertex("data/shader/instance.vertex", GL_VERTEX_SHADER),
          _fragment("data/shader/instance.fragment", GL_FRAGMENT_SHADER),
          _prog(_vertex, _fragment),
          _ubuffer(10, 102, 0),
          _world(min::vec3<float>(-115.0, 0.0, -115.0), min::vec3<float>(115.0, 115.0, 115.0)),
          _gravity(0.0, -10.0, 0.0),
          _simulation(_world, _gravity),
          _box_count(0),
          _body_radius(1.0)
    {
        // Set depth and cull settings
        min::settings::initialize();

        // Enable gamma correction
        min::settings::enable_gamma_correction();

        // Load the keyboard callbacks and settings
        load_keyboard();

        // Load model from files
        load_meshes();

        // Load textures from files
        load_textures();

        // Load the camera properties
        load_camera();

        // Load light and rigid body data
        load_physics_entities();
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
    static void forward(void *ptr, double step)
    {
        // Call back function for walking md5 mesh forward
        // 'ptr' is passed in by us in constructor
        if (ptr)
        {
            // Cast to example class for walking forward
            physics_test *test = reinterpret_cast<physics_test *>(ptr);

            // Walk
            test->walk_forward();
        }
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

        // Calculate the total energy of the system
        _simulation.get_total_energy();
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
    void draw(const double dt)
    {
        // Update the base and md5 model matrix
        update_base();

        // Bind VAO
        _sbuffer.bind();

        // Change program back to instance shaders
        _prog.use();

        // Bind the bmp for drawing
        _tbuffer.bind(_base_id, 0);

        // Draw 1 base mesh to act as the floor
        _sbuffer.draw_many(GL_TRIANGLES, _base_mesh, 1);

        // Update the box model matrices
        update_instances();

        // Bind the bmp for drawing
        _tbuffer.bind(_box_id, 0);

        // Draw N instances of object if we had added them to scene
        if (_box_count > 0)
        {
            _sbuffer.draw_many(GL_TRIANGLES, _box_mesh, _box_count);
        }

        // Update the md5 model if camera moved
        update_md5_mesh(_cam);

        // Draw the md5 character model
        _md5_char.draw(_cam, dt);
    }
    void window_update(const double dt)
    {
        // Update the keyboard
        auto &keyboard = _win.get_keyboard();
        keyboard.update(dt);

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
                test.solve(frame_time / 30.0, 10.0);
            }

            // Clear the background color
            test.clear_background();

            // Update the camera movement
            test.update_camera();

            // Draw the model
            test.draw(frame_time);

            // Update the window after draw command
            test.window_update(frame_time);

            // Calculate needed delay to hit target
            frame_time = sync.sync();
        }

        // Calculate the number of 'average' frames per second
        double fps = sync.get_fps();

        // Update the window title with FPS count of last frame
        test.set_title("Test md5 physics simulation: FPS: " + std::to_string(fps));
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
