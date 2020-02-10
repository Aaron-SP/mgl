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
#ifndef _MGL_SHADOW_BUFFER_MGL_
#define _MGL_SHADOW_BUFFER_MGL_

#include <min/frustum.h>
#include <min/gl_type.h>
#include <min/mat4.h>
#include <min/program.h>
#include <min/window.h>

namespace min
{

class shadow_buffer
{
  private:
    GLuint _id;
    GLuint _depth;
    size_t _width;
    size_t _height;
    mat4<float> _pv;
    mat4<float> _v;
    mat4<float> _scale_bias;
    mat4<float> _shadow;
    float _v_width;
    float _v_height;
    float _v_near;
    float _v_far;

    inline void check_extensions() const
    {
        const bool fbo = GLEW_ARB_framebuffer_object;

        // Check that we have the extensions we need
        if (!fbo)
        {
            throw std::runtime_error("shadow_buffer: minimum extensions not met");
        }
    }
    inline void set_light_view(const vec3<float> &eye, const vec3<float> &look, const vec3<float> &up)
    {
        // forward: look - eye
        const vec3<float> forward = (look - eye).normalize_unsafe();

        // Check for forward vector parallel to up vector
        vec3<float> right;
        if (std::abs(forward.dot(up)) > var<float>::TOL_PONE)
        {
            // Construct the view matrix with special axis, right == x
            right = vec3<float>(1.0, 0.0, 0.0);
        }
        else
        {
            // right: _up x forward - left handed coordinates
            right = up.cross(forward).normalize_unsafe();
        }

        // up: = forward x right - left handed coordinates
        // up is recalculated for stabilization
        const vec3<float> _up = forward.cross(right);

        // construct the view matrix
        _v = mat4<float>(right, _up, forward, eye);

        // Construct the projection matrix
        const mat4<float> proj(_v_width, _v_height, _v_near, _v_far);

        // Update the projection-view matrix
        _pv = _v * proj;

        // Update the shadow matrix
        _shadow = _pv * _scale_bias;
    }

  public:
    shadow_buffer(const size_t width, const size_t height)
        : _width(width), _height(height),
          _scale_bias(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0),
          _v_width(2), _v_height(2), _v_near(0.1), _v_far(200.0)

    {
        // Check that all needed extensions are present
        check_extensions();

        // Generate the shadow frame buffer
        glGenFramebuffers(1, &_id);

        // Bind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, _id);

        // Generate texture to hold shadows
        glGenTextures(1, &_depth);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D, _depth);

        // Allocate texture space
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, _width, _height, 0, GL_DEPTH_COMPONENT, FLOAT_TYPE<float>(), 0);

        // To prevent artifacts when sampling texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_GEQUAL);

        // Attach this texture to the framebuffer
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depth, 0);

        // Check that the frame buffer is valid
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            // Delete the texture buffer
            glDeleteTextures(1, &_depth);

            // Delete the frame buffer
            glDeleteFramebuffers(1, &_id);

            // Throw exception after cleaning up
            throw std::runtime_error("shadow_buffer: Failed framebuffer status check.");
        }

        // Do not draw to the color buffer
        glDrawBuffer(GL_NONE);

        // Switch back to the default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    ~shadow_buffer()
    {
        // Delete the texture buffer
        glDeleteTextures(1, &_depth);

        // Delete the frame buffer
        glDeleteFramebuffers(1, &_id);

        // Check for opengl errors
        throw_gl_error();
    }
    shadow_buffer(const shadow_buffer &sb) = delete;
    inline void bind() const
    {
        // Bind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, _id);

        // Adjust the viewport size
        glViewport(0, 0, _width, _height);
    }
    inline void bind_default(const size_t width, const size_t height) const
    {
        // Bind default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Adjust the viewport size
        glViewport(0, 0, width, height);
    }
    inline void bind_shadow_texture(const size_t layer) const
    {
        // Activate the texture layer
        glActiveTexture(GL_TEXTURE0 + layer);

        // Bind depth texture to this texture target
        glBindTexture(GL_TEXTURE_2D, _depth);
    }
    inline const mat4<float> &get_pv_matrix() const
    {
        // Return the proj-view matrix in opengl format
        return _pv;
    }
    inline const mat4<float> &get_shadow_matrix() const
    {
        return _shadow;
    }
    inline const mat4<float> &get_v_matrix() const
    {
        // Return the view matrix in opengl format
        return _v;
    }
    inline void set_light(const vec3<float> &light_position, const vec3<float> &light_look)
    {
        // Set the view matrix for this light
        set_light_view(light_position, light_look, vec3<float>::up());
    }
    inline void set_view_width(const float width)
    {
        _v_width = width;
    }
    inline void set_view_height(const float height)
    {
        _v_height = height;
    }
    inline void set_view_near(const float near)
    {
        _v_near = near;
    }
    inline void set_view_far(const float far)
    {
        _v_far = far;
    }
    inline void set_texture_uniform(const program &program, const std::string &name, const size_t layer) const
    {
        const GLint sampler_location = glGetUniformLocation(program.id(), name.c_str());
        if (sampler_location == -1)
        {
            throw std::runtime_error("shadow_buffer: could not find uniform '" + name + "'");
        }

        // Use the program to set the uniform value
        program.use();

        // Set the sampler active texture
        glUniform1i(sampler_location, layer);

        // Check for opengl errors
        throw_gl_error();
    }
};
}

#endif
