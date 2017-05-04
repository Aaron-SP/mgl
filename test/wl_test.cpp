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
#include <min/twindow.h>
#include <min/ttexture_buffer.h>
#include <min/tuniform_buffer.h>
#include <min/ttexture_compressor.h>
#include <min/tstatic_instance.h>
#include <min/ttessellation.h>
#include <iostream>

int main()
{
    try
    {
        bool out = true;
        out = out && test_window();
        out = out && test_texture_buffer();
        out = out && test_uniform_buffer();
        out = out && test_texture_compressor();
        out = out && test_static_instance();
        out = out && test_tessellation();
        if (out)
        {
            std::cout << "Window tests passed!" << std::endl;
            return 0;
        }
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    std::cout << "Window tests failed!" << std::endl;
    return -1;
}