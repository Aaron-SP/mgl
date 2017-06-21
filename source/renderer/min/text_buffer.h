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
#ifndef __TEXTBUFFER__
#define __TEXTBUFFER__

#include <min/window.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace min
{

class text_buffer
{
  private:
    class char_data
    {
      public:
        float adv_x;    // advance.x
        float adv_y;    // advance.y
        float height;   // bitmap.rows;
        float width;    // bitmap.width;
        float left;     // bitmap_left;
        float top;      // bitmap_top;
        float offset_x; // x offset of glyph in texture coordinates
        float offset_y; // y offset of glyph in texture coordinates
    } _chars[256];
    unsigned _w;
    unsigned _h;
    GLuint _tid;
    GLuint _vao;
    GLuint _vbo;
    std::vector<std::string> _text;
    std::vector<std::pair<float, float>> _location;
    mutable std::vector<vec4<float>> _data;
    mutable std::vector<std::pair<size_t, size_t>> _data_index;
    size_t _char_count;
    float _screen_x;
    float _screen_y;

    inline void bind_vao() const
    {
        // Bind the VAO for this buffer
        // Do not call this function often as it is unneeded context switching
        glBindVertexArray(_vao);
    }
    void create_vertex_buffer()
    {
        // Generate the VAO for text layout
        glGenVertexArrays(1, &_vao);

        // Bind the vao
        bind_vao();

        // Generate the vertex buffer id
        glGenBuffers(1, &_vbo);

        // Bind the buffer to hold data
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        // Specify the vertex attributes in location = 0, no offset, tighly packed
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
    }

    void create_texture_buffer(const unsigned width, const unsigned height)
    {
        // Generate texture buffer
        glGenTextures(1, &_tid);
        glBindTexture(GL_TEXTURE_2D, _tid);

        // Allow 1 byte per pixel, as OpenGL requires 4 bytes per pixel usually
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // To prevent artifacts when sampling texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Check for bad width or height values
        if (width == 0 || height == 0)
        {
            throw std::runtime_error("text_buffer: invalid texture size calculated: ");
        }

        // Allocate memory for texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    }

    void calculate_texture_dimensions(const FT_Face &face)
    {
        // Get a reference to the current face glyph
        FT_GlyphSlot glyph_slot = face->glyph;
        unsigned accumw = 0;
        unsigned maxh = 0;

        // For all characters
        for (unsigned i = 0; i < 256; i++)
        {
            // Load character from int id
            if (FT_Load_Char(face, i, FT_LOAD_RENDER))
            {
                throw std::runtime_error("text_buffer: could not load TTF face id: " + std::to_string(i));
            }

            // get the character data
            char_data &c = _chars[i];

            // the horizontal distance (in 1/64th pixels) from the origin to the origin of the next glyph.
            c.adv_x = glyph_slot->advance.x >> 6;
            c.adv_y = glyph_slot->advance.y >> 6;

            // image dimensions and bitmap buffer
            c.width = glyph_slot->bitmap.width;
            c.height = glyph_slot->bitmap.rows;
            c.left = glyph_slot->bitmap_left;
            c.top = glyph_slot->bitmap_top;

            // Create a new row if we exceed 1024 pixels in X
            if (accumw + c.width + 1 >= 1024)
            {
                // Calculate the width and height for this row, width transcends all rows
                _w = std::max(_w, accumw);
                _h += maxh;

                // Reset width and max height for the next row
                accumw = 0;
                maxh = 0;
            }

            // Accumulate width and max height
            accumw += c.width + 1;
            maxh = std::max(maxh, (unsigned)c.height);
        }

        // Record the width of the image
        _w = std::max(_w, accumw);

        // Add current row to the image height, starts at 0 so we need to add to get real height
        _h += maxh;
    }

    void upload_character_glyphs(const FT_Face &face)
    {
        // Get a reference to the current face glyph
        FT_GlyphSlot glyph_slot = face->glyph;

        unsigned maxh = 0;
        unsigned offset_w = 0;
        unsigned offset_h = 0;

        // For all characters upload generated images to OpenGL
        for (unsigned i = 0; i < 256; i++)
        {
            // Load character from int id, we must loop through a second time
            // The bitmap.buffer ptr is only valid until the next ft_load_char is called
            if (FT_Load_Char(face, i, FT_LOAD_RENDER))
            {
                throw std::runtime_error("text_buffer: could not load TTF face id: " + std::to_string(i));
            }

            // get the character data
            char_data &c = _chars[i];

            // Create rows if we hit 1024 pixels in X
            if (offset_w + c.width + 1 >= 1024)
            {
                offset_h += maxh;
                offset_w = 0;
                maxh = 0;
            }

            // Upload part of the image to the opengl texture
            glTexSubImage2D(GL_TEXTURE_2D, 0, offset_w, offset_h, c.width, c.height, GL_RED, GL_UNSIGNED_BYTE, glyph_slot->bitmap.buffer);

            // calculate the offset in texture coordinates
            c.offset_x = offset_w / (float)_w;
            c.offset_y = offset_h / (float)_h;

            // increment the total width of the image
            offset_w += c.width + 1;
            maxh = std::max(maxh, (unsigned)c.height);
        }
    }

    void create_texture_atlas(const FT_Face &face)
    {
        // Calculate dimensions of the texture atlas
        calculate_texture_dimensions(face);

        // allocate texture buffer for storing glyphs
        create_texture_buffer(_w, _h);

        // Upload all glyphs into the texture buffer
        upload_character_glyphs(face);
    }
    unsigned process_text(const std::string &text, const float pos_x, const float pos_y, const float screen_x, const float screen_y) const
    {
        // Create conversion to screen coordinates using screen size
        const float scale_x = 2.0 / screen_x;
        const float scale_y = 2.0 / screen_y;

        // Convert x & y to screen coordinates, start from bottom left corner
        float x = pos_x * scale_x - 1.0;
        float y = pos_y * scale_y - 1.0;

        // Starting triangle offset for this string
        const unsigned start = _data.size();

        // For all characters in string
        for (const auto &ch : text)
        {
            // get the character data
            const unsigned index = ch;
            const char_data &c = _chars[index];

            // Get glyph dimensions, in screen coordinates
            const float x_left = x + (c.left * scale_x);
            const float y_top = y + (c.top * scale_y);
            const float w = c.width * scale_x;
            const float h = c.height * scale_y;
            const float y_bot = y_top - h;
            const float x_right = x_left + w;

            // Skip glyphs that have no pixels
            if (w == 0 || h == 0)
                continue;

            // Create triangles, 2 for each character, data format: (x_pos, y_pos, uv_x, uv_y)
            // We flip the y-uv coordinates here because of OpenGL (0,0) == bottom, left
            const float uv_bot = c.offset_y + (c.height / (float)_h);
            const float uv_top = c.offset_y;
            const float uv_left = c.offset_x;
            const float uv_right = c.offset_x + (c.width / (float)_w);

            // Triangles must be in CW
            _data.emplace_back(x_left, y_bot, uv_left, uv_bot);
            _data.emplace_back(x_left, y_top, uv_left, uv_top);
            _data.emplace_back(x_right, y_bot, uv_right, uv_bot);

            _data.emplace_back(x_right, y_bot, uv_right, uv_bot);
            _data.emplace_back(x_left, y_top, uv_left, uv_top);
            _data.emplace_back(x_right, y_top, uv_right, uv_top);

            // Advance the cursor to the start of the next character
            x += c.adv_x * scale_x;
            y += c.adv_y * scale_y;
        }

        // Return the number of triangles added to buffer for this string
        return _data.size() - start;
    }

  public:
    text_buffer(const std::string &file, const int font_height) : _w(0), _h(0), _char_count(0), _screen_x(0.0), _screen_y(0.0)
    {
        // Initialize the FreeType library
        FT_Library lib;
        if (FT_Init_FreeType(&lib))
        {
            throw std::runtime_error("text_buffer: could not init freeType library");
        }

        // Load TTF font from file
        FT_Face face;
        if (FT_New_Face(lib, file.c_str(), 0, &face))
        {
            throw std::runtime_error("text_buffer: failed to load font");
        }

        // Set the font size, width is dynamically calculated based on font height
        FT_Set_Pixel_Sizes(face, 0, font_height);

        // create the vertex buffer for storing quads
        create_vertex_buffer();

        // Create texture atlas
        create_texture_atlas(face);

        // Free the font face
        FT_Done_Face(face);

        // Free the freetype library
        FT_Done_FreeType(lib);
    }
    ~text_buffer()
    {
        if (_tid > 0)
        {
            // Delete the texture
            glDeleteTextures(1, &_tid);
        }
        if (_vbo > 0)
        {
            // Bind the vao
            bind_vao();

            // disable the vertex attrib
            glDisableVertexAttribArray(0);

            // unbind the VBO and delete it
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDeleteBuffers(1, &_vbo);
        }
        if (_vao > 0)
        {
            // Unbind the VAO and delete it
            glBindVertexArray(0);
            glDeleteVertexArrays(1, &_vao);
        }

        // Check for opengl errors
        check_error();
    }
    inline size_t add_text(const std::string &text, const float x, const float y)
    {
        // Record the total character count
        _char_count += text.size();

        // Store the text
        _text.push_back(text);

        // Store the location
        _location.push_back(std::make_pair(x, y));

        // return the string index
        return _text.size() - 1;
    }
    inline void bind(const size_t layer) const
    {
        // Bind the VAO for this buffer
        // Do not call this function often as it is unneeded context switching
        glBindVertexArray(_vao);

        // Activate the texture layer
        glActiveTexture(GL_TEXTURE0 + layer);

        // Bind texture to this texture target
        glBindTexture(GL_TEXTURE_2D, _tid);
    }
    inline void clear()
    {
        // Clears the data in this buffer, but data will remain on GPU until next upload is called

        // Clear cached mutable data
        _data.clear();
        _data_index.clear();

        // Clear the string and index buffer
        _text.clear();
        _location.clear();

        // Reset char count
        _char_count = 0;
    }
    inline void draw(const size_t index) const
    {
        // Check if we have text to draw
        if (_data_index.size() > 0)
        {
            // Get the draw index
            const auto &n = _data_index[index];

            // Draw the specific string from index
            glDrawArrays(GL_TRIANGLES, n.first, n.second);
        }
    }
    inline void draw_all() const
    {
        // Check if we have text to draw
        if (_data_index.size() > 0)
        {
            // Calculate total buffer size
            const auto &n = _data_index.back();
            const size_t size = n.first + n.second;

            // Draw all of the text in one pass
            glDrawArrays(GL_TRIANGLES, 0, size);
        }
    }
    inline void set_texture_uniform(const program &program, const std::string &name, const size_t layer) const
    {
        GLint sampler_location = glGetUniformLocation(program.id(), name.c_str());
        if (sampler_location == -1)
        {
            throw std::runtime_error("text_buffer: could not find uniform '" + name + "'");
        }

        // Use the program to set the uniform value
        program.use();

        // Set the sampler active texture
        glUniform1i(sampler_location, layer);

        // Check for opengl errors
        check_error();
    }
    inline void set_screen(const float screen_x, const float screen_y)
    {
        _screen_x = screen_x;
        _screen_y = screen_y;
    }
    inline void set_text(const std::string &text, const size_t index)
    {
        // Calculate the change in character count
        const long diff = (long)(text.size() - _text[index].size());

        // Update character count
        _char_count += diff;

        // Update the text
        _text[index] = text;
    }
    inline void set_text(const std::string &text, const size_t index, const float x, const float y)
    {
        // Calculate the change in character count
        const long diff = (long)(text.size() - _text[index].size());

        // Update character count
        _char_count += diff;

        // Update the text
        _text[index] = text;

        // Update the location
        _location[index] = std::make_pair(x, y);
    }
    void upload() const
    {
        // Get the number of strings to render
        const size_t size = _text.size();

        // Do nothing if no data to upload
        if (size == 0)
        {
            // Fail silently
            return;
        }

        // Reserve space for the text, 2 triangles for each character
        _data.reserve(6 * _char_count);

        // Clear and reserve space for index parameters
        _data_index.clear();
        _data_index.reserve(size);

        size_t offset = 0;
        for (size_t i = 0; i < size; i++)
        {
            // Draw text at location and scale
            const auto loc = _location[i];

            // count = number of triangles (char * 6) added to buffer
            const size_t count = process_text(_text[i], loc.first, loc.second, _screen_x, _screen_y);

            // Calculate the index parameters
            _data_index.push_back(std::make_pair(offset, count));

            // Calculate the next offset
            offset += count;
        }

        // Bind the text buffer to hold data
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        // Upload data to the GPU
        const size_t text_bytes = _data.size() * sizeof(vec4<float>);
        glBufferData(GL_ARRAY_BUFFER, text_bytes, &_data[0], GL_DYNAMIC_DRAW);

        // Check that the expected character count did not overflow
        if (_data.size() > 6 * _char_count)
        {
            throw std::runtime_error("text_buffer: invalid character count");
        }

        // Data is on the GPU, so we throw this away
        _data.clear();
    }
};
}

#endif