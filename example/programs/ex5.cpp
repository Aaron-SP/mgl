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
#include <min/loop_sync.h>
#include <min/md5_mesh.h>
#include <min/md5_model.h>
#include <min/program.h>
#include <min/settings.h>
#include <min/shader.h>
#include <min/skeletal_vertex.h>
#include <min/text_buffer.h>
#include <min/texture_buffer.h>
#include <min/uniform_buffer.h>
#include <min/utility.h>
#include <min/vertex_buffer.h>
#include <min/window.h>
#include <string>

class md5_render_loop_test
{
  private:
    // OpenGL pipeline
    min::window _win;
    min::shader _vert_vertex;
    min::shader _vert_fragment;
    min::shader _text_vertex;
    min::shader _text_fragment;
    min::program _vert_prog;
    min::program _text_prog;

    // md5 model
    min::md5_model<float, uint32_t, min::vec4, min::aabbox> _md5_model;

    // Buffers for model data and textures
    min::vertex_buffer<float, uint32_t, min::skeletal_vertex, GL_FLOAT, GL_UNSIGNED_INT> _skbuffer;
    min::texture_buffer _texture_buffer;
    GLuint _bmp_id;

    // Create a text buffer for displaying FPS
    min::text_buffer _text_buffer;
    size_t _fps_id;
    size_t _idle_id;

    // Camera and uniform data
    min::camera<float> _cam;
    min::uniform_buffer<float> _ubuffer;
    size_t _proj_view_id;
    size_t _view_id;
    size_t _model_id;

    // Light properties for editing
    min::vec4<float> _light_color;
    min::vec4<float> _light_position;
    min::vec4<float> _light_power;
    size_t _light_id;

    // For animating md5 model bones
    std::vector<size_t> _bone_id;

    // Model matrix for rotating md5 model
    min::mat4<float> _model_matrix;

  public:
    // Load window shaders and program
    md5_render_loop_test()
        : _win("Example animated MD5 with dynamic text", 720, 480, 3, 3),
          _vert_vertex("data/shader/md5.vertex", GL_VERTEX_SHADER),
          _vert_fragment("data/shader/md5.fragment", GL_FRAGMENT_SHADER),
          _text_vertex("data/shader/text.vertex", GL_VERTEX_SHADER),
          _text_fragment("data/shader/text.fragment", GL_FRAGMENT_SHADER),
          _vert_prog(_vert_vertex, _vert_fragment),
          _text_prog(_text_vertex, _text_fragment),
          _md5_model(std::move(min::md5_mesh<float, uint32_t>("data/models/mech_warrior.md5mesh"))),
          _text_buffer("data/fonts/open_sans.ttf", 14),
          _ubuffer(1, 100),
          _light_color(1.0, 1.0, 1.0, 1.0),
          _light_position(-9.0, 10.0, 0.0, 1.0),
          _light_power(0.1, 200.0, 100.0, 1.0)
    {
        // Set depth, cull and blend settings
        min::settings::initialize();

        // Set ability to close out of application by pressing 'Q'
        auto &keyboard = _win.get_keyboard();

        // Add key to watch
        keyboard.add(min::window::key_code::KEYQ);

        // Register callback function for closing window
        keyboard.register_keydown(min::window::key_code::KEYQ, md5_render_loop_test::close_window, (void *)&_win);

        // Put cursor in center of window
        update_cursor();
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
        std::cout << "md5_render_loop_test: Shutdown called by user" << std::endl;
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
    void load_model()
    {
        // Warn user we are opening an md5 model
        std::cout << "Opening an md5 model: " << std::endl;

        // Load animation
        _md5_model.load_animation("data/models/mech_warrior.md5anim");

        // Setup the md5 mesh
        min::mesh<float, uint32_t> &md5 = _md5_model.get_meshes()[0];
        md5.calculate_normals();
        md5.calculate_tangents();

        // Bind VAO, it actually is already bound and could be removed
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
        // _vert_prog will be in use by the end of this call
        _texture_buffer.set_texture_uniform(_vert_prog, "in_texture", 0);

        // Load texture buffer
        _bmp_id = _texture_buffer.add_bmp_texture(b);

        // Load text
        // Set the texture channel for this program, we need to do this here because we render text on channel '1'
        // _text_prog will be in use by the end of this call
        _text_buffer.set_texture_uniform(_text_prog, "in_texture", 1);

        // Load the text buffer with data
        _fps_id = _text_buffer.add_text("FPS:", 10, 28);
        _idle_id = _text_buffer.add_text("FPS:", 10, 14);
    }
    void load_camera_uniforms()
    {
        // Move and camera to -X and look at origin
        min::vec3<float> pos = min::vec3<float>(-10.0, 10.0, 0.0);
        min::vec3<float> look = min::vec3<float>(0.0, 0.0, 0.0);

        // Test perspective projection
        // Create camera, set location and look at
        _cam.set_position(pos);
        _cam.set_look_at(look);
        _cam.set_perspective();

        // Load the uniform buffer with the program we will use
        _ubuffer.set_program(_vert_prog);

        // Load light into uniform buffer
        _light_id = _ubuffer.add_light(min::light<float>(_light_color, _light_position, _light_power));

        // Load projection and view matrix into uniform buffer
        _proj_view_id = _ubuffer.add_matrix(_cam.get_pv_matrix());
        _view_id = _ubuffer.add_matrix(_cam.get_v_matrix());

        // Set initial model orientation
        _model_matrix = min::mat4<float>(min::mat3<float>().set_rotation_x(-90.0));

        // Get model ID for later use
        _model_id = _ubuffer.add_matrix(_model_matrix);

        // Add bones matrices to uniform buffer
        for (const auto &bone : _md5_model.get_bones())
        {
            size_t bone_id = _ubuffer.add_matrix(bone);
            _bone_id.push_back(bone_id);
        }

        // Load the buffer with data
        _ubuffer.update();
    }
    void draw(double time_step)
    {
        // Rotate the model around the Z axis
        _model_matrix *= min::mat4<float>(min::mat3<float>().set_rotation_y(10.0 * time_step));

        // Update matrix uniforms
        _ubuffer.set_matrix(_cam.get_pv_matrix(), _proj_view_id);
        _ubuffer.set_matrix(_cam.get_v_matrix(), _view_id);
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

        // Update the matrix and light buffer
        _ubuffer.update();

        // Change program back to vertex shaders
        _vert_prog.use();

        // Change back to the vertex buffer
        _skbuffer.bind();

        // Bind this texture for drawing on channel '0'
        _texture_buffer.bind(_bmp_id, 0);

        // Draw md5 model
        _skbuffer.draw(GL_TRIANGLES, 0);

        // Change program to text shaders
        _text_prog.use();

        // Bind the text_buffer vao, and textures on channel '1'
        _text_buffer.bind(1);

        // Draw the FPS text
        _text_buffer.draw(_fps_id);

        // Draw the idle text
        _text_buffer.draw(_idle_id);
    }
    void set_title(const std::string &title)
    {
        _win.set_title(title);
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
    void update_text(const double fps, const double idle)
    {
        // Set the screen size
        _text_buffer.set_screen(720, 480);

        // Update the FPS text in buffer
        _text_buffer.set_text("FPS: " + std::to_string(fps), _fps_id);

        // Update the FPS text in buffer
        _text_buffer.set_text("IDLE(%): " + std::to_string(idle), _idle_id);

        // Upload the strings to the GPU
        _text_buffer.upload();
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
    md5_render_loop_test test;

    // Load model from files
    test.load_model();

    // Load textures from files
    test.load_textures();

    // Load the camera and fill uniform buffers with light and model matrix
    test.load_camera_uniforms();

    // Initialize FPS text
    test.update_text(60.0, 60.0);

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

            // Clear the background color
            test.clear_background();

            // Update the camera movement
            test.update_camera();

            // Draw the model
            test.draw(frame_time);

            // Update the window after draw command
            test.window_update();

            // Calculate needed delay to hit target
            frame_time = sync.sync();
        }

        // Calculate the number of 'average' frames per second
        const double fps = sync.get_fps();

        // Calculate the percentage of frame spent idle
        const double idle = sync.idle();

        // Update the FPS count in the window using a text buffer
        test.update_text(fps, idle);
    }

    return 0;
}

int main()
{
    return test_render_loop();
}