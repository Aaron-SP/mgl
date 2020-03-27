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
#ifndef _MGL_SDL_WINDOW_MGL_
#define _MGL_SDL_WINDOW_MGL_

#if __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#define GL_COMPRESSED_SRGB_S3TC_DXT1_EXT 0x8C4C
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT 0x8C4D
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT 0x8C4E
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT 0x8C4F
#else
#include <glad/glad.h>
#endif
#include <SDL2/SDL.h>
#include <cstdio>
#include <iostream>
#include <min/keyboard.h>
#include <stdexcept>
#include <string>
#include <utility>

namespace min
{

inline bool check_gl_error()
{
    const GLenum error = glGetError();

    // Check for error and print to stdout
    const bool ret = error != GL_NO_ERROR;
    if (ret)
    {
        std::cout << "OpenGL Error: " << std::to_string(error) << std::endl;
    }

    return ret;
}

inline void throw_gl_error()
{
    const GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        throw std::runtime_error("GL ERROR! glGetError(): " + std::to_string(error));
    }
}

inline GLboolean is_supported(const char *string)
{
    return SDL_GL_ExtensionSupported(string);
}

class sdl_window
{
  public:
    // Virtual key codes for SDL platform
    using key_type = SDL_Keycode;
    enum key_code : key_type
    {
        F1 = SDLK_F1,
        F2 = SDLK_F2,
        F3 = SDLK_F3,
        F4 = SDLK_F4,
        F5 = SDLK_F5,
        F6 = SDLK_F6,
        F7 = SDLK_F7,
        F8 = SDLK_F8,
        F9 = SDLK_F9,
        F10 = SDLK_F10,
        F11 = SDLK_F11,
        F12 = SDLK_F12,
        KEY0 = SDLK_0,
        KEY1 = SDLK_1,
        KEY2 = SDLK_2,
        KEY3 = SDLK_3,
        KEY4 = SDLK_4,
        KEY5 = SDLK_5,
        KEY6 = SDLK_6,
        KEY7 = SDLK_7,
        KEY8 = SDLK_8,
        KEY9 = SDLK_9,
        KEYA = SDLK_a,
        KEYB = SDLK_b,
        KEYC = SDLK_c,
        KEYD = SDLK_d,
        KEYE = SDLK_e,
        KEYF = SDLK_f,
        KEYG = SDLK_g,
        KEYH = SDLK_h,
        KEYI = SDLK_i,
        KEYJ = SDLK_j,
        KEYK = SDLK_k,
        KEYL = SDLK_l,
        KEYM = SDLK_m,
        KEYN = SDLK_n,
        KEYO = SDLK_o,
        KEYP = SDLK_p,
        KEYQ = SDLK_q,
        KEYR = SDLK_r,
        KEYS = SDLK_s,
        KEYT = SDLK_t,
        KEYU = SDLK_u,
        KEYV = SDLK_v,
        KEYW = SDLK_w,
        KEYX = SDLK_x,
        KEYY = SDLK_y,
        KEYZ = SDLK_z,
        NUM0 = SDLK_KP_0,
        NUM1 = SDLK_KP_1,
        NUM2 = SDLK_KP_2,
        NUM3 = SDLK_KP_3,
        NUM4 = SDLK_KP_4,
        NUM5 = SDLK_KP_5,
        NUM6 = SDLK_KP_6,
        NUM7 = SDLK_KP_7,
        NUM8 = SDLK_KP_8,
        NUM9 = SDLK_KP_9,
        LSHIFT = SDLK_LSHIFT,
        RSHIFT = SDLK_RSHIFT,
        LCONTROL = SDLK_LCTRL,
        RCONTROL = SDLK_RCTRL,
        TAB = SDLK_TAB,
        ENTER = SDLK_RETURN,
        BACKSPACE = SDLK_BACKSPACE,
        SPACE = SDLK_SPACE,
        LALT = SDLK_LALT,
        RALT = SDLK_RALT,
        ESCAPE = SDLK_ESCAPE,
        APOSTROPHE = SDLK_BACKQUOTE,
        QUOTE = SDLK_QUOTE,
        COMMA = SDLK_COMMA,
        PERIOD = SDLK_PERIOD,
        SEMICOLON = SDLK_SEMICOLON
    };

  private:
    uint_fast16_t _w;
    uint_fast16_t _h;
    int _major;
    int _minor;
    bool _shutdown;
    SDL_Window *window;
    SDL_GLContext sdl_context;
    keyboard<key_type, double> _keyboard;

    // Callback functions
    void *_data;
    void (*_lclick_down)(void *, const uint_fast16_t width, const uint_fast16_t);
    void (*_lclick_up)(void *, const uint_fast16_t width, const uint_fast16_t);
    void (*_rclick_down)(void *, const uint_fast16_t width, const uint_fast16_t);
    void (*_rclick_up)(void *, const uint_fast16_t width, const uint_fast16_t);
    void (*_update)(void *, const uint_fast16_t width, const uint_fast16_t);

    // Window class string literal
    static constexpr const char *window_class = "mgl:sdl_window";

    inline void create_window(const std::string &title)
    {
        SDL_Init(SDL_INIT_VIDEO);

        // Request SDL window parameters
#if __EMSCRIPTEN__
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
#else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
#endif

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        // Create the window
        window = SDL_CreateWindow(
            title.c_str(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _w, _h,
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

        // Get the window size if different than asked for
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        _w = static_cast<uint_fast16_t>(w);
        _h = static_cast<uint_fast16_t>(h);

        // Check if window is created
        if (!window)
        {
            throw std::runtime_error("sdl_window: Failed to create SDL window!");
        }

        // Create GL context
        sdl_context = SDL_GL_CreateContext(window);
        if (!sdl_context)
        {
            throw std::runtime_error("sdl_window: Failed to create GL context!");
        }

#if !__EMSCRIPTEN__
        if (!gladLoadGL())
        {
            throw std::runtime_error("sdl_window: GLAD failed to load!");
        }
#endif

        // Print out the opengl version
        GLint major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        std::cout << "sdl_window: opening SDL opengl context version " << major << "." << minor << std::endl;
    }
    inline void on_lclick_down(const uint_fast16_t x, const uint_fast16_t y) const
    {
        // Call the lclick_down callback
        if (_lclick_down)
        {
            _lclick_down(_data, x, y);
        }
    }
    inline void on_lclick_up(const uint_fast16_t x, const uint_fast16_t y) const
    {
        // Call the lclick_up callback
        if (_lclick_up)
        {
            _lclick_up(_data, x, y);
        }
    }
    inline void on_rclick_down(const uint_fast16_t x, const uint_fast16_t y) const
    {
        // Call the rlick_down callback
        if (_rclick_down)
        {
            _rclick_down(_data, x, y);
        }
    }
    inline void on_rclick_up(const uint_fast16_t x, const uint_fast16_t y) const
    {
        // Call the rclick_up callback
        if (_rclick_up)
        {
            _rclick_up(_data, x, y);
        }
    }
    inline void on_resize(const uint_fast16_t width, const uint_fast16_t height)
    {
        _w = width;
        _h = height;

        // Reset opengl viewport
        glViewport(0, 0, _w, _h);

        // Call the update callback
        on_update(_w, _h);
    }
    inline void on_update(const uint_fast16_t width, const uint_fast16_t height) const
    {
        // Call the update callback
        if (_update)
        {
            _update(_data, width, height);
        }
    }

  public:
    sdl_window(const std::string &title, const uint_fast16_t width, const uint_fast16_t height, int major, int minor)
        : _w(width), _h(height), _major(major), _minor(minor), _shutdown(false),
          _lclick_down(nullptr), _lclick_up(nullptr), _rclick_down(nullptr), _rclick_up(nullptr), _update(nullptr)
    {
        // Create SDL window for certain opengl version
        create_window(title);
    }
    ~sdl_window()
    {
        SDL_GL_DeleteContext(sdl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    inline void display_cursor(const bool set) const
    {
        // Set if cursor is visible
        if (!set)
        {
            SDL_ShowCursor(SDL_DISABLE);
        }
        else
        {
            SDL_ShowCursor(SDL_ENABLE);
        }
    }
    inline void error_message(const std::string &error) const
    {
        std::cout << error << std::endl;
    }
    inline const char *get_context_string(const GLenum str) const
    {
        return reinterpret_cast<const char *>(glGetString(str));
    }
    inline std::pair<uint_fast16_t, uint_fast16_t> get_cursor() const
    {
        int x, y;
        SDL_GetMouseState(&x, &y);
        return std::make_pair<uint_fast16_t, uint_fast16_t>(x, y);
    }
    inline uint_fast16_t get_height() const
    {
        return _h;
    }
    inline keyboard<key_type, double> &get_keyboard()
    {
        return _keyboard;
    }
    inline const keyboard<key_type, double> &get_keyboard() const
    {
        return _keyboard;
    }
    inline bool get_shutdown() const
    {
        return _shutdown;
    }
    inline uint_fast16_t get_width() const
    {
        return _w;
    }
    inline void hide() const
    {
        SDL_HideWindow(window);
    }
    inline void maximize() const
    {
        // Maximize the window
        SDL_MaximizeWindow(window);
    }
    inline void register_lclick_down(void (*down)(void *, const uint_fast16_t x, const uint_fast16_t y))
    {
        // Register callback on lmouse down
        _lclick_down = down;
    }
    inline void register_lclick_up(void (*up)(void *, const uint_fast16_t x, const uint_fast16_t y))
    {
        // Register callback on lmouse up
        _lclick_up = up;
    }
    inline void register_rclick_down(void (*down)(void *, const uint_fast16_t x, const uint_fast16_t y))
    {
        // Register callback on rmouse down
        _rclick_down = down;
    }
    inline void register_rclick_up(void (*up)(void *, const uint_fast16_t x, const uint_fast16_t y))
    {
        // Register callback on rmouse up
        _rclick_up = up;
    }
    inline void register_data(void *ptr)
    {
        // Register callback data pointer
        _data = ptr;
    }
    inline void register_update(void (*update)(void *, const uint_fast16_t width, const uint_fast16_t height))
    {
        // Register callback on resize
        _update = update;
    }
    inline void resize(const uint_fast16_t width, const uint_fast16_t height) const
    {
        SDL_SetWindowSize(window, width, height);
    }
    inline void set_cursor(const uint_fast16_t x, const uint_fast16_t y) const
    {
        SDL_WarpMouseInWindow(window, x, y);
    }
    inline void set_shutdown()
    {
        _shutdown = true;
    }
    inline void set_title(const std::string &title)
    {
        // Set the window title
        SDL_SetWindowTitle(window, title.c_str());
    }
    inline void show() const
    {
        // Show the window
        SDL_ShowWindow(window);
    }
    inline void swap_buffers() const
    {
        SDL_GL_SwapWindow(window);
    }
    inline void update()
    {
        // Process events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_WINDOWEVENT:
            {
                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_RESIZED:
                    // Update the window size
                    on_resize(event.window.data1, event.window.data2);
                    break;
                }
                break;
            }
            case SDL_KEYDOWN:
            {
                // Forward key down events to the virtual keyboard
                _keyboard.key_down(event.key.keysym.sym, 0);
                break;
            }
            case SDL_KEYUP:
            {
                // Forward key down events to the virtual keyboard
                _keyboard.key_up(event.key.keysym.sym, 0);
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    on_lclick_down(event.button.x, _h - event.button.y);
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    on_rclick_down(event.button.x, _h - event.button.y);
                }
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    on_lclick_up(event.button.x, _h - event.button.y);
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    on_rclick_up(event.button.x, _h - event.button.y);
                }
                break;
            }
            case SDL_QUIT:
            {
                this->set_shutdown();
                break;
            }
            }
        }
    }
};
}

#endif
