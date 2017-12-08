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
#ifndef __PROGRAM__
#define __PROGRAM__

#include <min/shader.h>
#include <min/window.h>
#include <string>
#include <vector>

namespace min
{

class program
{
  private:
    GLuint _id;

    inline void create_program()
    {
        // Create program ID
        _id = glCreateProgram();
        if (_id == 0)
        {
            throw std::runtime_error("program: Failed to create program ID.");
        }
    }
    inline void link_program()
    {
        // Link the program
        glLinkProgram(_id);

        // Check that the link was successful
        GLint status;
        glGetProgramiv(_id, GL_LINK_STATUS, &status);
        if (!status)
        {
            print_errors();
            throw std::runtime_error("program: Failed to link shader program.");
        }
    }
    void print_errors() const
    {
        GLint log_length = 0;
        if (glIsProgram(_id))
        {
            // Get the shader log length
            glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &log_length);

            // Check that error exists
            if (log_length > 0)
            {
                // Create space for the log
                std::string log(log_length + 1, '\0');

                // Get the log data, NULL means we do not need the length since we queried for it
                glGetProgramInfoLog(_id, log_length, nullptr, &log[0]);

                // Handle the error
                window_error(log);
            }
        }
        else
        {
            throw std::runtime_error("program: program id is invalid!");
        }
    }

  public:
    program(const shader &vertex, const shader &fragment)
    {
        // Create program id
        create_program();

        // Attach vertex and fragment shader
        glAttachShader(_id, vertex.id());
        glAttachShader(_id, fragment.id());

        // link program
        link_program();

        // Detach the shaders as we do not need them anymore
        glDetachShader(_id, vertex.id());
        glDetachShader(_id, fragment.id());
    }
    program(const std::vector<GLuint> &shaders)
    {
        // Create program id
        create_program();

        // Attach all shaders
        for (const auto &shader_id : shaders)
        {
            // Attach vertex and fragment shader
            glAttachShader(_id, shader_id);
        }

        // link program
        link_program();

        // Detach the shaders as we do not need them anymore
        for (const auto &shader_id : shaders)
        {
            glDetachShader(_id, shader_id);
        }
    }
    ~program()
    {
        if (_id > 0)
        {
            // Disable the current program
            glUseProgram(0);

            // Delete the program
            glDeleteProgram(_id);
        }

        // check for errors
        check_error();
    }
    program(const program &p) = delete;
    inline void use() const
    {
        // Enable the program for use
        glUseProgram(_id);
    }
    inline GLuint id() const
    {
        return _id;
    }
};
}

#endif
