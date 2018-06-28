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
#ifndef __WIN32WINDOW__
#define __WIN32WINDOW__

// Do not reorder the GLEW headers, clang-format will break compiles
#include <min/glew.h>

#include <min/wglew.h>

#include <gl\GL.h>

#include <cstdio>
#include <iostream>
#include <min/keyboard.h>
#include <stdexcept>
#include <string>
#include <utility>
#include <windows.h>
#include <windowsx.h>

// This pollutes the namespace
#undef far
#undef near

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
    return wglewIsSupported(string);
}

class win32_window
{
  public:
    // Virtual key codes for WIN32 platform
    using key_type = WPARAM;
    enum key_code : key_type
    {
        F1 = VK_F1,
        F2 = VK_F2,
        F3 = VK_F3,
        F4 = VK_F4,
        F5 = VK_F5,
        F6 = VK_F6,
        F7 = VK_F7,
        F8 = VK_F8,
        F9 = VK_F9,
        F10 = VK_F10,
        F11 = VK_F11,
        F12 = VK_F12,
        KEY0 = 0x30,
        KEY1 = 0x31,
        KEY2 = 0x32,
        KEY3 = 0x33,
        KEY4 = 0x34,
        KEY5 = 0x35,
        KEY6 = 0x36,
        KEY7 = 0x37,
        KEY8 = 0x38,
        KEY9 = 0x39,
        KEYA = 0x41,
        KEYB = 0x42,
        KEYC = 0x43,
        KEYD = 0x44,
        KEYE = 0x45,
        KEYF = 0x46,
        KEYG = 0x47,
        KEYH = 0x48,
        KEYI = 0x49,
        KEYJ = 0x4A,
        KEYK = 0x4B,
        KEYL = 0x4C,
        KEYM = 0x4D,
        KEYN = 0x4E,
        KEYO = 0x4F,
        KEYP = 0x50,
        KEYQ = 0x51,
        KEYR = 0x52,
        KEYS = 0x53,
        KEYT = 0x54,
        KEYU = 0x55,
        KEYV = 0x56,
        KEYW = 0x57,
        KEYX = 0x58,
        KEYY = 0x59,
        KEYZ = 0x5A,
        NUM0 = VK_NUMPAD0,
        NUM1 = VK_NUMPAD1,
        NUM2 = VK_NUMPAD2,
        NUM3 = VK_NUMPAD3,
        NUM4 = VK_NUMPAD4,
        NUM5 = VK_NUMPAD5,
        NUM6 = VK_NUMPAD6,
        NUM7 = VK_NUMPAD7,
        NUM8 = VK_NUMPAD8,
        NUM9 = VK_NUMPAD9,
        LSHIFT = VK_LSHIFT,
        RSHIFT = VK_RSHIFT,
        LCONTROL = VK_LCONTROL,
        RCONTROL = VK_RCONTROL,
        TAB = VK_TAB,
        ENTER = VK_RETURN,
        BACKSPACE = VK_BACK,
        SPACE = VK_SPACE,
        LALT = VK_LMENU,
        RALT = VK_RMENU,
        ESCAPE = VK_ESCAPE,
        APOSTROPHE = VK_OEM_3,
        QUOTE = VK_OEM_7,
        COMMA = VK_OEM_COMMA,
        PERIOD = VK_OEM_PERIOD,
        SEMICOLON = VK_OEM_1
    };

  private:
    uint_fast16_t _w;
    uint_fast16_t _h;
    int _major;
    int _minor;
    bool _shutdown;
    HWND _hwnd;
    HDC _hdc;
    HGLRC _hrc;
    HINSTANCE _hinst;
    keyboard<key_type, double> _keyboard;

    // Callback functions
    void *_data;
    void (*_lclick_down)(void *, const uint_fast16_t width, const uint_fast16_t);
    void (*_lclick_up)(void *, const uint_fast16_t width, const uint_fast16_t);
    void (*_rclick_down)(void *, const uint_fast16_t width, const uint_fast16_t);
    void (*_rclick_up)(void *, const uint_fast16_t width, const uint_fast16_t);
    void (*_update)(void *, const uint_fast16_t width, const uint_fast16_t);

    // Window class string literal
    static constexpr const char *window_class = "minwl:win32_window";

    // Window event callback function
    inline static LRESULT CALLBACK window_callback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        // Pointer to 'this' window
        win32_window *window;

        switch (message)
        {
        case WM_CREATE:
        {
            // Cast the pointer passed in to window class
            CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT *>(lParam);

            // Class the parameter to window pointer
            window = reinterpret_cast<win32_window *>(pCreate->lpCreateParams);

            // Store the window pointer for later use
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
            break;
        }
        case WM_CLOSE:
        {
            // Get the window class
            window = reinterpret_cast<win32_window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

            // Signal that we are closing down
            window->set_shutdown();
            break;
        }
        case WM_DESTROY:
        {
            // Post quit message
            PostQuitMessage(0);
            break;
        }
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        {
            // Get the window class
            window = reinterpret_cast<win32_window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

            const UINT scan_code = (lParam & 0x00ff0000) >> 16;
            const int ext = (lParam & 0x01000000) != 0;

            switch (wParam)
            {
            case VK_SHIFT:
                window->_keyboard.key_down(MapVirtualKey(scan_code, MAPVK_VSC_TO_VK_EX), 0);
                break;
            case VK_CONTROL:
                if (ext)
                {
                    window->_keyboard.key_down(VK_RCONTROL, 0);
                }
                else
                {
                    window->_keyboard.key_down(VK_LCONTROL, 0);
                }
                break;
            case VK_MENU:
                if (ext)
                {
                    window->_keyboard.key_down(VK_RMENU, 0);
                }
                else
                {
                    window->_keyboard.key_down(VK_LMENU, 0);
                }
                break;
            default:
                window->_keyboard.key_down(wParam, 0);
            }
            break;
        }
        case WM_SYSKEYUP:
        case WM_KEYUP:
        {
            // Get the window class
            window = reinterpret_cast<win32_window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

            const UINT scan_code = (lParam & 0x00ff0000) >> 16;
            const int ext = (lParam & 0x01000000) != 0;

            switch (wParam)
            {
            case VK_SHIFT:
                window->_keyboard.key_up(MapVirtualKey(scan_code, MAPVK_VSC_TO_VK_EX), 0);
                break;
            case VK_CONTROL:
                if (ext)
                {
                    window->_keyboard.key_up(VK_RCONTROL, 0);
                }
                else
                {
                    window->_keyboard.key_up(VK_LCONTROL, 0);
                }
                break;
            case VK_MENU:
                if (ext)
                {
                    window->_keyboard.key_up(VK_RMENU, 0);
                }
                else
                {
                    window->_keyboard.key_up(VK_LMENU, 0);
                }
                break;
            default:
                window->_keyboard.key_up(wParam, 0);
            }
            break;
        }
        case WM_LBUTTONDOWN:
        {
            if (GetKeyState(VK_LBUTTON) & 0x80)
            {
                // Get the window class
                window = reinterpret_cast<win32_window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

                // Get the mouse properties
                const int x = GET_X_LPARAM(lParam);
                const int y = GET_Y_LPARAM(lParam);

                // Call the drag callback
                const uint_fast16_t h = window->get_height();
                window->on_lclick_down(x, h - y);
            }
            break;
        }
        case WM_LBUTTONUP:
        {
            // Get the window class
            window = reinterpret_cast<win32_window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

            // Get the mouse properties
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);

            // Call the click callback
            const uint_fast16_t h = window->get_height();
            window->on_lclick_up(x, h - y);
            break;
        }
        case WM_RBUTTONDOWN:
        {
            // Get the window class
            window = reinterpret_cast<win32_window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

            // Get the mouse properties
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);

            // Call the click callback
            const uint_fast16_t h = window->get_height();
            window->on_rclick_down(x, h - y);
            break;
        }
        case WM_RBUTTONUP:
        {
            // Get the window class
            window = reinterpret_cast<win32_window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

            // Get the mouse properties
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);

            // Call the click callback
            const uint_fast16_t h = window->get_height();
            window->on_rclick_up(x, h - y);
            break;
        }
        case WM_PAINT:
        {
            // Paint the window
            PAINTSTRUCT ps;

            // Begin painting
            BeginPaint(hwnd, &ps);

            // End painting
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_SIZE:
        {
            // Called during window resize

            // Get the window class
            window = reinterpret_cast<win32_window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

            // Get the screen properties
            const uint_fast16_t w = LOWORD(lParam);
            const uint_fast16_t h = HIWORD(lParam);

            // Call the resize callback
            window->on_resize(w, h);

            break;
        }
        case WM_SYSCOMMAND:
        {
            // Disable the syscommand menu focus when pressing down 'ALT + something'
            // lParam holds the ASCII key pressed with ALT
            if (wParam == SC_KEYMENU)
            {
                return 0;
            }

            // Default window procedure is called
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
        default:
        {
            // Default window procedure is called
            return DefWindowProc(hwnd, message, wParam, lParam);
            break;
        }
        }
        return 0;
    }
    inline static std::pair<uint_fast16_t, uint_fast16_t> calculate_window_size(const uint_fast16_t width, const uint_fast16_t height)
    {
        // Calculate the size of window to get desired client area
        RECT wr = {0, 0, width, height};
        AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

        return std::make_pair<uint_fast16_t, uint_fast16_t>(wr.right - wr.left, wr.bottom - wr.top);
    }
    inline void init_glew(PIXELFORMATDESCRIPTOR &pfd)
    {
        // This creates a dummy context using the old Win32 opengl interface to load GLEW
        // This is used to load opengl extensions for 3.2+ opengl rendering contexts

        // Choose pixel format
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.iLayerType = PFD_MAIN_PLANE;

        // Choose the appropriate pixel format
        int pixel_format = ChoosePixelFormat(_hdc, &pfd);
        if (!pixel_format)
        {
            throw std::runtime_error("win32_window: Could not choose pixel format.");
        }

        // Set choosen pixel format
        if (!SetPixelFormat(_hdc, pixel_format, &pfd))
        {
            throw std::runtime_error("win32_window: Could not set pixel format, error: " + std::to_string(GetLastError()));
        }

        // Create dummy opengl context for querying opengl extensions
        HGLRC dummy = wglCreateContext(_hdc);
        if (!wglMakeCurrent(_hdc, dummy))
        {
            throw std::runtime_error("win32_window: Could not bind dummy rendering context");
        }

        // Load GLEW opengl extensions
        GLenum error = glewInit();
        if (error != GLEW_OK)
        {
            throw std::runtime_error("win32_window: GLEW error on glewInit().");
        }

        // Disable the current rendering context
        if (!wglMakeCurrent(_hdc, 0))
        {
            throw std::runtime_error("win32_window: Could not disable dummy rendering context");
        }

        // Deletes a specified OpenGL rendering context
        if (!wglDeleteContext(dummy))
        {
            throw std::runtime_error("win32_window: Could not delete dummy rendering context");
        }
    }

    inline void create_opengl_context(const PIXELFORMATDESCRIPTOR &pfd, int major, int minor)
    {
        // Query for 3.3 opengl context, ask for a forward compatible profile (disables deprecated functions)
        int attrib[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, major,
            WGL_CONTEXT_MINOR_VERSION_ARB, minor,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            0, 0}; // End

        // Check if opengl extensions are available
        if (wglewIsSupported("WGL_ARB_create_context") == GL_TRUE && wglewIsSupported("WGL_ARB_pixel_format") == GL_TRUE)
        {
            // Adjust the pixel format using newer opengl method
            const int pixel_attrib[] =
                {
                    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                    WGL_COLOR_BITS_ARB, 32,
                    WGL_DEPTH_BITS_ARB, 24,
                    //WGL_STENCIL_BITS_ARB, 8,
                    0}; // End

            // Query for available pixel formats
            int pixel_format;
            UINT numFormats;
            wglChoosePixelFormatARB(_hdc, pixel_attrib, NULL, 1, &pixel_format, &numFormats);

            // Set choosen pixel format
            if (!SetPixelFormat(_hdc, pixel_format, &pfd))
            {
                throw std::runtime_error("win32_window: Could not set pixel format, error: " + std::to_string(GetLastError()));
            }

            // Create the rendering context 3.2+
            _hrc = wglCreateContextAttribsARB(_hdc, NULL, attrib);

            // Make context current
            if (!wglMakeCurrent(_hdc, _hrc))
            {
                throw std::runtime_error("win32_window: Could not bind rendering context");
            }

            // Load GLEW opengl extensions
            GLenum error = glewInit();
            if (error != GLEW_OK)
            {
                throw std::runtime_error("win32_window: GLEW error on glewInit().");
            }
        }
        else
        {
            throw std::runtime_error("win32_window: Could not create 3.2+ opengl context.");
        }
    }

    inline void create_window(const std::string &title)
    {
        // Get the handle of the EXE
        _hinst = GetModuleHandle(NULL);

        // Create window struct for window creation
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wcex.lpfnWndProc = win32_window::window_callback;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = _hinst;
        wcex.hIcon = NULL;
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = window_class;
        wcex.hIconSm = NULL;

        // Register the window class
        if (!RegisterClassEx(&wcex))
        {
            throw std::runtime_error("win32_window: RegisterClassEx failed.");
        }

        // Calculate the size of window to get desired client area
        auto size = calculate_window_size(_w, _h);

        // Create window and check if it is good
        _hwnd = CreateWindow(window_class, title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, size.first, size.second, NULL, NULL, _hinst, this);
        if (!_hwnd)
        {
            throw std::runtime_error("win32_window: Could not create window.");
        }

        // Show the window, returns bool whether window was visible, disregard return value
        ShowWindow(_hwnd, SW_SHOW);

        // Update the window
        if (!UpdateWindow(_hwnd))
        {
            throw std::runtime_error("win32_window: Could not update window.");
        }

        // Create window device context
        _hdc = GetDC(_hwnd);
        if (_hdc == NULL)
        {
            throw std::runtime_error("win32_window: Could not get device context.");
        }

        // Create dummy opengl context to load opengl extensions through GLEW
        PIXELFORMATDESCRIPTOR pfd;
        memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

        // Initialize GLEW
        init_glew(pfd);

        // Create the real opengl context
        create_opengl_context(pfd, _major, _minor);

        // Print out the opengl version
        GLint major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        if (major < _major || (major == _major && minor < _minor))
        {
            throw std::runtime_error("win32_window: Could not create " + std::to_string(_major) + "." + std::to_string(_minor) + " opengl context.");
        }
        std::cout << "win32_window: opening win32 opengl context version " << major << "." << minor << std::endl;
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
    win32_window(const std::string &title, const uint_fast16_t width, const uint_fast16_t height, int major, int minor)
        : _w(width), _h(height), _major(major), _minor(minor),
          _shutdown(false),
          _lclick_down(nullptr), _lclick_up(nullptr), _rclick_down(nullptr), _rclick_up(nullptr), _update(nullptr)
    {
        // Create WIN32 window for certain opengl version
        create_window(title);
    }
    ~win32_window()
    {
        if (_hrc)
        {
            // Disable the current rendering context
            if (!wglMakeCurrent(_hdc, 0))
            {
                std::cout << "win32_window: Could not disable rendering context" << std::endl;
            }

            // Deletes a specified OpenGL rendering context
            if (!wglDeleteContext(_hrc))
            {
                std::cout << "win32_window: Could not delete rendering context" << std::endl;
            }
        }
        // Releases a device context
        if (_hdc && !ReleaseDC(_hwnd, _hdc))
        {
            std::cout << "win32_window: Could not release device context" << std::endl;
        }
        // Destroy window
        if (_hwnd && !DestroyWindow(_hwnd))
        {
            std::cout << "win32_window: Could not destroy window" << std::endl;
        }

        // Unregister window class so it can be reused
        if (_hinst)
        {
            UnregisterClass(window_class, _hinst);
        }
    }
    inline void display_cursor(const bool set) const
    {
        // Set if cursor is visible
        // The cursor is displayed only if the display count is greater than or equal to 0
        int count;
        if (set)
        {
            // Set count to zero
            count = ShowCursor(true);
            while (count < 0)
            {
                count = ShowCursor(true);
            }
        }
        else
        {
            // Set count to -1
            count = ShowCursor(false);
            while (count >= 0)
            {
                count = ShowCursor(false);
            }
        }
    }
    inline void error_message(const std::string &error) const
    {
        MessageBox(_hwnd, error.c_str(), "Window Error", MB_OK);
    }
    inline const char *get_context_string(const GLenum str) const
    {
        return reinterpret_cast<const char *>(glGetString(str));
    }
    inline std::pair<uint_fast16_t, uint_fast16_t> get_cursor() const
    {
        POINT p;
        if (!GetCursorPos(&p))
        {
            // We might not be able to get cursor due to shutdown event
            if (!_shutdown)
            {
                throw std::runtime_error("win32_window: failed getting global mouse coordinates.");
            }
        }
        else
        {
            if (!ScreenToClient(_hwnd, &p))
            {
                // We might not be able to get cursor due to shutdown event
                if (!_shutdown)
                {
                    throw std::runtime_error("win32_window: failed getting local mouse coordinates.");
                }
            }
        }

        return std::make_pair<uint_fast16_t, uint_fast16_t>(p.x, p.y);
    }
    inline keyboard<key_type, double> &get_keyboard()
    {
        return _keyboard;
    }
    inline const keyboard<key_type, double> &get_keyboard() const
    {
        return _keyboard;
    }
    inline uint_fast16_t get_height() const
    {
        return _h;
    }
    inline bool get_shutdown() const
    {
        return _shutdown;
    }
    inline uint_fast16_t get_width() const
    {
        return _w;
    }
    inline void maximize() const
    {
        // Maximize the window
        ShowWindow(_hwnd, SW_MAXIMIZE);

        // Return value isn't used,
        // If window was previously hidden zero
        // If window was previously visible nonzero
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
        // Calculate the window size to get desired area
        auto size = calculate_window_size(width, height);

        // Resize the window
        SetWindowPos(_hwnd, 0, 0, 0, size.first, size.second, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    }
    inline void set_cursor(const uint_fast16_t x, const uint_fast16_t y) const
    {
        // Set the cursor on this window
        POINT p;
        p.x = x;
        p.y = y;
        ClientToScreen(_hwnd, &p);
        SetCursorPos(p.x, p.y);
    }
    inline void set_shutdown()
    {
        _shutdown = true;
    }
    inline void set_title(const std::string &title)
    {
        // Set the window title, check for errors
        if (!SetWindowText(_hwnd, title.c_str()))
        {
            throw std::runtime_error("win32_window: failed setting window title");
        }
    }
    inline void swap_buffers() const
    {
        // Swaps the double buffer
        SwapBuffers(_hdc);
        UpdateWindow(_hwnd);
    }
    inline void update()
    {
        // Processes the message queue for this window
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            // If we are shutting down, flag shutdown
            if (msg.message == WM_QUIT)
            {
                this->set_shutdown();
            }
        }
    }
};
}

#endif
