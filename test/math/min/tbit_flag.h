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
#ifndef __TESTBITFLAG__
#define __TESTBITFLAG__

#include <cstdint>
#include <min/bit_flag.h>
#include <min/test.h>
#include <stdexcept>

bool test_bit_flag()
{
    bool out = true;
    min::bit_flag<uint_fast16_t, uint_fast32_t> flags(256, 256);

    // Make sure 1/2 is off by default
    out = out && flags.get(1, 2) == false;
    if (!out)
    {
        throw std::runtime_error("Failed bit_flag get default value");
    }

    // Set 1/2 to be on
    flags.set_on(1, 2);
    out = out && flags.get(1, 2) == true;
    if (!out)
    {
        throw std::runtime_error("Failed bit_flag set_on 1,2");
    }

    // Set 3/4 to be on
    flags.set_on(3, 4);
    out = out && flags.get(3, 4) == true;
    if (!out)
    {
        throw std::runtime_error("Failed bit_flag set_on 3,4");
    }

    // Turn off 1/2 and make sure 3/4 is still set
    flags.set_off(1, 2);
    out = out && flags.get(1, 2) == false;
    out = out && flags.get(3, 4) == true;
    if (!out)
    {
        throw std::runtime_error("Failed bit_flag get 1,2");
    }

    return out;
}

#endif
