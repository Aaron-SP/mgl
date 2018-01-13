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
#include <min/togg.h>
#include <min/tsound_buffer.h>
#include <min/twave.h>

int main()
{
    try
    {
        bool out = true;
        out = out && test_wave();
        out = out && test_ogg();
        out = out && test_sound_buffer();
        if (out)
        {
            std::cout << "Sound tests passed!" << std::endl;
            return 0;
        }
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    std::cout << "Sound tests failed!" << std::endl;
    return -1;
}
