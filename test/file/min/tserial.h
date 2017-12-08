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
#ifndef __TESTSERIALIZE__
#define __TESTSERIALIZE__

#include <min/serial.h>
#include <min/test.h>
#include <stdexcept>

bool test_serial()
{
    bool out = true;

    // Serialize int
    {
        // Test serializing int
        size_t next = 0;
        std::vector<uint8_t> stream;
        int a = -19567;
        int b;

        // Write little endian data to stream
        min::write_le<int>(stream, a);

        // Read little endian data from stream
        b = min::read_le<int>(stream, next);
        out = out && compare(-19567, b);
        if (!out)
        {
            throw std::runtime_error("Failed serializing little endian signed integer");
        }

        // Test serializing int big endian
        a = -2105678;

        // Write little endian data to stream
        min::write_be<int>(stream, a);

        // Read little endian data from stream
        b = min::read_be<int>(stream, next);
        out = out && compare(-2105678, b);
        if (!out)
        {
            throw std::runtime_error("Failed serializing big endian signed integer");
        }
    }

    // Serialize unsigned
    {
        // Test serializing unsigned
        size_t next = 0;
        std::vector<uint8_t> stream;
        unsigned a = 19567;
        unsigned b;

        // Write little endian data to stream
        min::write_le<unsigned>(stream, a);

        // Read little endian data from stream
        b = min::read_le<unsigned>(stream, next);
        out = out && compare(19567, b);
        if (!out)
        {
            throw std::runtime_error("Failed serializing little endian unsigned integer");
        }

        // Test serializing unsigned big endian
        a = 2105678;

        // Write little endian data to stream
        min::write_be<unsigned>(stream, a);

        // Read little endian data from stream
        b = min::read_be<unsigned>(stream, next);
        out = out && compare(2105678, b);
        if (!out)
        {
            throw std::runtime_error("Failed serializing big endian unsigned integer");
        }
    }

    // Serialize float
    {
        // Test serializing float
        size_t next = 0;
        std::vector<uint8_t> stream;
        float a = 19567.53;
        float b;

        // Write little endian data to stream
        min::write_le<float>(stream, a);

        // Read little endian data from stream, notice floating point inaccuracy
        b = min::read_le<float>(stream, next);
        out = out && compare(19567.5293, b, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed serializing little endian float");
        }

        // Test serializing float big endian
        a = 2105678.35;

        // Write big endian data to stream
        min::write_be<float>(stream, a);

        // Read big endian data from stream, notice floating point inaccuracy
        b = min::read_be<float>(stream, next);
        out = out && compare(2105678.25, b, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed serializing big endian signed float");
        }
    }

    // Serialize double
    {
        // Test serializing double
        size_t next = 0;
        std::vector<uint8_t> stream;
        double a = 19567.545;
        double b;

        // Write little endian data to stream
        min::write_le<double>(stream, a);

        // Read little endian data from stream
        b = min::read_le<double>(stream, next);
        out = out && compare(19567.5450, b, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed serializing little endian double");
        }

        // Test serializing double big endian
        a = 2105678.3510;

        // Write big endian data to stream
        min::write_be<double>(stream, a);

        // Read big endian data from stream
        b = min::read_be<double>(stream, next);
        out = out && compare(2105678.3510, b, 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed serializing big endian signed double");
        }
    }

    // Serialize int vector
    {
        // Test serializing int little endian
        size_t next = 0;
        std::vector<uint8_t> stream;
        std::vector<int> a = {1, -34567, 1902365, -42};
        std::vector<int> b;

        // Write little endian data to stream
        min::write_le_vector<int>(stream, a);

        // Read little endian data from stream
        b = min::read_le_vector<int>(stream, next);
        out = out && compare(1, b[0]);
        out = out && compare(-34567, b[1]);
        out = out && compare(1902365, b[2]);
        out = out && compare(-42, b[3]);
        if (!out)
        {
            throw std::runtime_error("Failed serializing little endian vector of signed integer");
        }

        // Test serializing int big endian
        a = {42, -34561, 1028427, -1, 1};

        // Write big endian data to stream
        min::write_be_vector<int>(stream, a);

        // Read big endian data from stream
        b = min::read_be_vector<int>(stream, next);
        out = out && compare(42, b[0]);
        out = out && compare(-34561, b[1]);
        out = out && compare(1028427, b[2]);
        out = out && compare(-1, b[3]);
        out = out && compare(1, b[4]);
        if (!out)
        {
            throw std::runtime_error("Failed serializing big endian vector of signed integer");
        }
    }

    // Serialize vec2<float> vector
    {
        // Test serializing int little endian
        size_t next = 0;
        std::vector<uint8_t> stream;
        std::vector<min::vec2<float>> a = {min::vec2<float>(1.0, 0.1), min::vec2<float>(-3.0, -4.1)};
        std::vector<min::vec2<float>> b;

        // Write little endian data to stream
        min::write_le_vector_vec2<float>(stream, a);

        // Read little endian data from stream
        b = min::read_le_vector_vec2<float>(stream, next);
        out = out && compare(1.0, b[0].x(), 1E-4);
        out = out && compare(0.1, b[0].y(), 1E-4);
        out = out && compare(-3.0, b[1].x(), 1E-4);
        out = out && compare(-4.1, b[1].y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed serializing little endian vector of vec2<float>");
        }

        // Test serializing vec2<float> big endian
        a = {min::vec2<float>(2.0, 0.3), min::vec2<float>(-7.0, -8.1)};

        // Write big endian data to stream
        min::write_be_vector_vec2<float>(stream, a);

        // Read big endian data from stream
        b = min::read_be_vector_vec2<float>(stream, next);
        out = out && compare(2.0, b[0].x(), 1E-4);
        out = out && compare(0.3, b[0].y(), 1E-4);
        out = out && compare(-7.0, b[1].x(), 1E-4);
        out = out && compare(-8.1, b[1].y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed serializing big endian vector of vec2<float>");
        }
    }

    // Serialize vec3<float> vector
    {
        // Test serializing int little endian
        size_t next = 0;
        std::vector<uint8_t> stream;
        std::vector<min::vec3<float>> a = {min::vec3<float>(1.0, 0.1, 3.2), min::vec3<float>(-3.0, -4.1, 7.2)};
        std::vector<min::vec3<float>> b;

        // Write little endian data to stream
        min::write_le_vector_vec3<float>(stream, a);

        // Read little endian data from stream
        b = min::read_le_vector_vec3<float>(stream, next);
        out = out && compare(1.0, b[0].x(), 1E-4);
        out = out && compare(0.1, b[0].y(), 1E-4);
        out = out && compare(3.2, b[0].z(), 1E-4);
        out = out && compare(-3.0, b[1].x(), 1E-4);
        out = out && compare(-4.1, b[1].y(), 1E-4);
        out = out && compare(7.2, b[1].z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed serializing little endian vector of vec3<float>");
        }

        // Test serializing vec3<float> big endian
        a = {min::vec3<float>(2.0, 0.3, -2.4), min::vec3<float>(-7.0, -8.1, 1.2)};

        // Write big endian data to stream
        min::write_be_vector_vec3<float>(stream, a);

        // Read big endian data from stream
        b = min::read_be_vector_vec3<float>(stream, next);
        out = out && compare(2.0, b[0].x(), 1E-4);
        out = out && compare(0.3, b[0].y(), 1E-4);
        out = out && compare(-2.4, b[0].z(), 1E-4);
        out = out && compare(-7.0, b[1].x(), 1E-4);
        out = out && compare(-8.1, b[1].y(), 1E-4);
        out = out && compare(1.2, b[1].z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed serializing big endian vector of vec3<float>");
        }
    }

    // Serialize vec4<float> vector
    {
        // Test serializing int little endian
        size_t next = 0;
        std::vector<uint8_t> stream;
        std::vector<min::vec4<float>> a = {min::vec4<float>(1.0, 0.1, 3.2, 1.0), min::vec4<float>(-3.0, -4.1, 7.2, 1.0)};
        std::vector<min::vec4<float>> b;

        // Write little endian data to stream
        min::write_le_vector_vec4<float>(stream, a);

        // Read little endian data from stream
        b = min::read_le_vector_vec4<float>(stream, next);
        out = out && compare(1.0, b[0].x(), 1E-4);
        out = out && compare(0.1, b[0].y(), 1E-4);
        out = out && compare(3.2, b[0].z(), 1E-4);
        out = out && compare(1.0, b[0].w(), 1E-4);
        out = out && compare(-3.0, b[1].x(), 1E-4);
        out = out && compare(-4.1, b[1].y(), 1E-4);
        out = out && compare(7.2, b[1].z(), 1E-4);
        out = out && compare(1.0, b[1].w(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed serializing little endian vector of vec4<float>");
        }

        // Test serializing vec4<float> big endian
        a = {min::vec4<float>(2.0, 0.3, -2.4, 1.0), min::vec4<float>(-7.0, -8.1, 1.2, 1.0)};

        // Write big endian data to stream
        min::write_be_vector_vec4<float>(stream, a);

        // Read big endian data from stream
        b = min::read_be_vector_vec4<float>(stream, next);
        out = out && compare(2.0, b[0].x(), 1E-4);
        out = out && compare(0.3, b[0].y(), 1E-4);
        out = out && compare(-2.4, b[0].z(), 1E-4);
        out = out && compare(1.0, b[0].w(), 1E-4);
        out = out && compare(-7.0, b[1].x(), 1E-4);
        out = out && compare(-8.1, b[1].y(), 1E-4);
        out = out && compare(1.2, b[1].z(), 1E-4);
        out = out && compare(1.0, b[1].w(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed serializing big endian vector of vec4<float>");
        }
    }

    return out;
}

#endif
