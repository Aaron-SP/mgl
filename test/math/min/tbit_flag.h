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
#ifndef _MGL_TESTBITFLAG_MGL_
#define _MGL_TESTBITFLAG_MGL_

#include <cstdint>
#include <min/bit_flag.h>
#include <min/test.h>

bool test_bit_flag()
{
    bool out = true;

    // Print size and alignment of class
    std::cout << "bitflag_size: " << sizeof(min::bit_flag<uint_fast16_t, uint_fast32_t>) << std::endl;
    std::cout << "bitflag_align: " << alignof(min::bit_flag<uint_fast16_t, uint_fast32_t>) << std::endl;

#ifdef MGL_TEST_ALIGN
    std::cout << "tbit_flag.h: Testing alignment" << std::endl;
    out = out && test(sizeof(void *) * 3, sizeof(min::bit_flag<uint_fast16_t, uint_fast32_t>), "Failed bitflag sizeof");
    out = out && test(sizeof(void *), alignof(min::bit_flag<uint_fast16_t, uint_fast32_t>), "Failed bitflag alignof");
#endif

    min::bit_flag<uint_fast16_t, uint_fast32_t> flags(256, 256);

    // Make sure 1/2 is off by default
    out = out && test(false, flags.get(1, 2), "Failed bit_flag get default value");

    // Set 1/2 to be on
    flags.set_on(1, 2);
    out = out && test(true, flags.get(1, 2), "Failed bit_flag set_on 1, 2");

    // Set 3/4 to be on
    flags.set_on(3, 4);
    out = out && test(true, flags.get(3, 4), "Failed bit_flag set_on 3, 4");

    // Turn off 1/2 and make sure 3/4 is still set
    flags.set_off(1, 2);
    out = out && test(false, flags.get(1, 2), "Failed bit_flag get 1, 2");
    out = out && test(true, flags.get(3, 4), "Failed bit_flag get 3, 4");

    return out;
}

#endif
