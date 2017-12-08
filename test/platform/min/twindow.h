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
#ifndef __TESTWINDOW__
#define __TESTWINDOW__

#include <iostream>
#include <min/test.h>
#include <min/window.h>
#include <stdexcept>

bool key_press_test = false;

void test_Q(void *ptr, double step)
{
    key_press_test = true;
}

bool test_window()
{
    bool out = true;

    // Local variables
    min::window win("Test Window", 720, 480, 3, 3);
    std::pair<uint16_t, uint16_t> cursor;

    // Test the window dimensions
    out = out && compare(720, win.get_width());
    out = out && compare(480, win.get_height());
    if (!out)
    {
        throw std::runtime_error("Failed window constructor");
    }

    // Test set/get cursor
    win.set_cursor(360, 240);
    cursor = win.get_cursor();
    out = out && compare(360, cursor.first);
    out = out && compare(240, cursor.second);
    if (!out)
    {
        throw std::runtime_error("Failed window get/set cursor");
    }

    // Test resize
    win.resize(500, 500);
    win.update();
    out = out && compare(500, win.get_width());
    out = out && compare(500, win.get_height());
    if (!out)
    {
        // This is annoying if running on linux with a window manager
        // This isn't considered a failure just a warning
        out = true;
        std::cout << "Warning! Failed window resize, are you using a window manager?" << std::endl;
    }

    // Test display cursor on/off
    win.display_cursor(false);
    win.display_cursor(true);

    // Test swap buffers
    win.swap_buffers();

    // Test key events
    auto &keyb = win.get_keyboard();

    // Add key to watch
    keyb.add(min::window::key_code::KEYQ);

    // Register callback function
    keyb.register_keydown(min::window::key_code::KEYQ, test_Q, nullptr);

    // Ask the user to perform key detection
    std::string s = ask("Can you press the Q button on your keyboard with the window active, and then type (y)?");

    // Test update window message queue
    win.update();
    out = out && key_press_test;
    if (!out)
    {
        throw std::runtime_error("Failed window key press test");
    }

    return out;
}

#endif
