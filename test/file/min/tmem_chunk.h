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
#ifndef __TEST_MEM_CHUNK__
#define __TEST_MEM_CHUNK__

#include <min/mem_chunk.h>
#include <min/test.h>
#include <stdexcept>

bool test_mem_chunk()
{
    bool out = true;

    min::mem_chunk chunk1;

    // Add some files to the set
    chunk1.add_file("data/texture/art_cube.bmp");
    chunk1.add_file("data/texture/stone.dds");
    out = out && compare(2, chunk1.size());
    if (!out)
    {
        throw std::runtime_error("Failed mem_chunk1 load test");
    }

    // Write the mem_chunk to a file
    chunk1.write_memory_file("bin/mem_chunk_test");

    // Load that mem_chunk file into new set
    min::mem_chunk chunk2("bin/mem_chunk_test");
    out = out && compare(2, chunk2.size());
    if (!out)
    {
        throw std::runtime_error("Failed mem_chunk2 load test");
    }

    // Compare the data inside the BMP files
    {
        const min::mem_file bmp1 = chunk1.get_file("data/texture/art_cube.bmp");
        const min::mem_file bmp2 = chunk2.get_file("data/texture/art_cube.bmp");

        // Compare the data offsets
        out = out && compare(bmp1.offset(), bmp2.offset());
        if (!out)
        {
            throw std::runtime_error("Failed mem_chunk bmp offset");
        }

        // Compare the data sizes
        out = out && compare(bmp1.size(), bmp2.size());
        if (!out)
        {
            throw std::runtime_error("Failed mem_chunk bmp size");
        }

        // Compare the BMP files byte by byte
        const size_t bytes = bmp1.size();
        for (size_t i = 0; i < bytes; i++)
        {
            if (bmp1[i] != bmp2[i])
            {
                throw std::runtime_error("Failed mem_chunk bmp byte compare");
            }
        }
    }

    // Compare the data inside the DDS files
    {
        const min::mem_file dds1 = chunk1.get_file("data/texture/stone.dds");
        const min::mem_file dds2 = chunk2.get_file("data/texture/stone.dds");

        // Compare the data offsets
        out = out && compare(dds1.offset(), dds2.offset());
        if (!out)
        {
            throw std::runtime_error("Failed mem_chunk dds offset");
        }

        // Compare the data sizes
        out = out && compare(dds1.size(), dds2.size());
        if (!out)
        {
            throw std::runtime_error("Failed mem_chunk dds size");
        }

        // Compare the DDS files byte by byte
        const size_t bytes = dds1.size();
        for (size_t i = 0; i < bytes; i++)
        {
            if (dds1[i] != dds2[i])
            {
                throw std::runtime_error("Failed mem_chunk dds byte compare");
            }
        }
    }

    return out;
}

#endif
