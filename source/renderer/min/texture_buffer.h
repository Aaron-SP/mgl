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
#ifndef __TEXTUREBUFFER__
#define __TEXTUREBUFFER__

#include <algorithm>
#include <cstdint>
#include <min/bmp.h>
#include <min/dds.h>
#include <min/program.h>
#include <min/window.h>
#include <stdexcept>

namespace min
{

class texture_buffer
{
  private:
    std::vector<GLuint> _ids;
    size_t _max_size;

    inline void check_extensions() const
    {
        const bool fbo = GLEW_ARB_framebuffer_object;
        const bool s3tc = GLEW_EXT_texture_compression_s3tc;
        const bool srgb = GLEW_EXT_texture_sRGB;

        // Check that we have the extensions we need
        if (!fbo || !s3tc || !srgb)
        {
            throw std::runtime_error("texture_buffer: minimum extensions not met");
        }
    }
    void check_texture_size(const uint32_t width, const uint32_t height)
    {
        // Check the texture size versus the maximum supported by hardware
        if (width > _max_size || height > _max_size)
        {
            std::string max_string = std::to_string(_max_size);
            throw std::runtime_error("texture_buffer: input texture is too large, your hardware maximum is " + max_string + "x" + max_string);
        }
    }
    std::vector<GLuint> generate_texture(const size_t n, const size_t mips)
    {
        // Generating N textures
        std::vector<GLuint> out(n, 0);

        // Generate new texture ID
        glGenTextures(n, &out[0]);

        // Bind this texture and set filtering parameters
        for (const auto &id : out)
        {
            glBindTexture(GL_TEXTURE_2D, id);

            // Set texture wrap settings
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // Set min/mag filter settings
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

            // Tell opengl how many mipmaps we will use
            if (mips > 1)
            {
                // The parameter is the maximum mip max 'index' not actual size
                // 'index' = size - 1
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mips - 1);
            }
        }

        return out;
    }

  public:
    texture_buffer() : _max_size(get_max_texture_size())
    {
        // Check that all needed extensions are present
        check_extensions();
    }
    ~texture_buffer()
    {
        auto size = _ids.size();
        if (size > 0)
        {
            // Delete all textures in the buffer
            glDeleteTextures(size, &_ids[0]);
        }

        // Check for opengl errors
        throw_gl_error();
    }
    texture_buffer(const texture_buffer &tb) = delete;
    GLuint add_bmp_texture(const bmp &b, const bool srgb = false)
    {
        // Extracted input image data
        const uint32_t width = b.get_width();
        const uint32_t height = b.get_height();
        const uint32_t pixel_size = b.get_pixel_size();
        const std::vector<uint8_t> &pixels = b.get_pixels();

        // Check the texture size vs the maximum size
        check_texture_size(width, height);

        // Generate 1 texture id
        auto id = generate_texture(1, 1);

        // Store this id in the internal id buffer
        _ids.push_back(id[0]);

        // Mandate 4 bytes per pixel, since we change this in text_buffer!
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

        // Check if 24 or 32 bit image
        if (pixel_size == 3)
        {
            // If image row is not aligned by 4 bytes throw error, this could cause texture distortion
            if (width % 4 != 0)
            {
                throw std::runtime_error("texture_buffer: BMP is not 4 byte aligned");
            }

            // If gamma correcting texture
            if (srgb)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, &pixels[0]);
            }
            else
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, &pixels[0]);
            }
        }
        else if (pixel_size == 4)
        {
            // If gamma correcting texture
            if (srgb)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, &pixels[0]);
            }
            else
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, &pixels[0]);
            }
        }
        else
        {
            throw std::runtime_error("texture_buffer: BMP format is not supported");
        }

        // Generate mip map levels for this texture
        glGenerateMipmap(GL_TEXTURE_2D);

        // Return the id for this texture
        return id[0];
    }
    GLuint add_dds_texture(const dds &d, const bool srgb = false)
    {
        // Extracted input image data
        const uint32_t width = d.get_width();
        const uint32_t height = d.get_height();
        const uint32_t mips = d.get_mips();
        const uint32_t format = d.get_format();
        const std::vector<uint8_t> &pixels = d.get_pixels();

        // Check the texture size vs the maximum size
        check_texture_size(width, height);

        // Generate 1 texture id
        const auto id = generate_texture(1, mips);

        // Store this id in the internal id buffer
        _ids.push_back(id[0]);

        // Mandate 4 byte row alignment, we change this in
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

        // Figure out the type of compression based on the DDS format
        GLenum type;
        if (format == dds::DXT1)
        {
            // If gamma correcting texture
            if (srgb)
            {
                type = GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
            }
            else
            {
                type = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
            }

            // If image row is not aligned by 4 bytes throw error, this could cause texture distortion
            if (width % 4 != 0)
            {
                throw std::runtime_error("texture_buffer: BMP is not 4 byte aligned");
            }
        }
        else if (format == dds::DXT3)
        {
            // If gamma correcting texture
            if (srgb)
            {
                type = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
            }
            else
            {
                type = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            }
        }
        else if (format == dds::DXT5)
        {
            // If gamma correcting texture
            if (srgb)
            {
                type = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
            }
            else
            {
                type = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            }
        }
        else
        {
            throw std::runtime_error("texture_buffer: DDS format is not supported");
        }

        // For all mip map levels
        // Determine the blocksize based on the DDS format
        const uint32_t block_size = (format == dds::DXT1) ? 8 : 16;
        uint32_t offset = 0;
        uint32_t w = width;
        uint32_t h = height;
        for (uint32_t i = 0; i < mips; i++)
        {
            // Calculate the mip map level size in bytes
            const uint32_t size = ((w + 3) / 4) * ((h + 3) / 4) * block_size;

            // Load the compressed mip map into the texture buffer
            glCompressedTexImage2D(GL_TEXTURE_2D, i, type, w, h, 0, size, &pixels[offset]);

            // Break out since we hit the end of the mip levels
            if ((w == 1 && h == 1) && (i != mips - 1))
            {
                // Throw error here if we are not at the last mip map
                throw std::runtime_error("texture_buffer: too many mip map levels are specified for this texture, hit 1x1 too early");
            }

            // Calculate offset, width and height for next level, accurate for non-power of two textures
            offset += size;
            w = std::max((unsigned)1, w / 2);
            h = std::max((unsigned)1, h / 2);
        }

        // Check if the mip map levels were not correctly calculated
        if (mips > 1 && w != 1 && h != 1)
        {
            throw std::runtime_error("texture_buffer: loaded mip map levels were not fully specified, 1x1 condition never reached");
        }
        else if (mips == 1)
        {
            // Check if the mips need to be generated
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else if (mips == 0)
        {
            // This is an invalid texture
            throw std::runtime_error("texture_buffer: compressed texture has a mip map level of zero, invalid state");
        }

        // Return the id for this texture
        return id[0];
    }
    inline void bind(const GLuint id, const size_t layer) const
    {
        // Activate the texture layer
        glActiveTexture(GL_TEXTURE0 + layer);

        // Bind texture to this texture target
        glBindTexture(GL_TEXTURE_2D, id);
    }
    inline size_t get_max_texture_size() const
    {
        // Get the maximum uniform buffer size
        GLint size;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);

        return (size_t)size;
    }
    inline void set_texture_uniform(const program &program, const std::string &name, const size_t layer) const
    {
        const GLint sampler_location = glGetUniformLocation(program.id(), name.c_str());
        if (sampler_location == -1)
        {
            throw std::runtime_error("texture_buffer: could not find uniform '" + name + "'");
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
