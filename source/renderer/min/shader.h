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
#ifndef __SHADER__
#define __SHADER__

#include <fstream>
#include <min/mem_chunk.h>
#include <min/window.h>
#include <string>

namespace min
{

class shader
{
  private:
    GLuint _id;
    GLenum _type;

    inline void load_file(const std::string path, const GLenum type)
    {
        std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
        if (file.is_open())
        {
            // Get the size of the file
            std::streampos one = 1;
            std::ifstream::pos_type size = file.tellg();

            // Adjust file pointer to beginning
            file.seekg(0, std::ios::beg);

            // Allocate space for new file
            std::string data(size + one, '\0');

            // Read bytes and close the file
            file.read(&data[0], size);

            // Close the file
            file.close();

            // Process the DDS file
            load(data, type);
        }
        else
        {
            throw std::runtime_error("shader: File '" + path + "' doesn't exist.");
        }
    }
    inline void load(const std::string &src, const GLenum type)
    {
        // Create a shader id
        _id = glCreateShader(_type);
        if (_id == 0)
        {
            throw std::runtime_error("shader: Failed to create shader ID.");
        }

        // This function can take multiple strings and concatenate them together for combining fragments
        // We only use one file for now, give source code to opengl
        // NULL means that string is null terminated
        const char *array_strings = &src[0];
        glShaderSource(_id, 1, &array_strings, nullptr);

        // Compile the shader
        glCompileShader(_id);

        // Check that the compile was successful
        GLint status = GL_FALSE;
        glGetShaderiv(_id, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            print_errors();
            throw std::runtime_error("shader: Failed to compile opengl shader.");
        }
    }
    void print_errors() const
    {
        GLint log_length = 0;
        if (glIsShader(_id))
        {
            // Get the shader log length
            glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &log_length);

            // Check that error exists
            if (log_length > 0)
            {
                // Create space for the log
                std::string log(log_length + 1, '\0');

                // Get the log data, NULL means we do not need the length since we queried for it
                glGetShaderInfoLog(_id, log_length, nullptr, &log[0]);

                // Handle the error
                window_error(log);
            }
        }
        else
        {
            throw std::runtime_error("shader: shader id is invalid!");
        }
    }

  public:
    shader(const std::string &path, const GLenum type) : _id(0), _type(type)
    {
        load_file(path, type);
    }
    shader(const mem_file &mem, const GLenum type) : _id(0), _type(type)
    {
        load(mem.to_string(), type);
    }
    ~shader()
    {
        if (_id > 0)
        {
            glDeleteShader(_id);
        }

        // check for errors
        check_internal_error();
    }
    shader(const shader &s) = delete;
    inline GLuint id() const
    {
        return _id;
    }
};
}

#endif
