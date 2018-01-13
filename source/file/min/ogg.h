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
#ifndef __OGG__
#define __OGG__

#include <cstring>
#include <min/mem_chunk.h>
#include <vector>
#include <vorbis/vorbisfile.h>

namespace min
{

// This fakes a file interface to read files from memory
struct fake_file
{
  public:
    char *_head;
    char *const _start;
    size_t _size;
    char *const _end;

    fake_file(char *const head, char *const start, const size_t size)
        : _head(head), _start(start), _size(size), _end(start + size) {}
};

size_t fake_read_ogg(void *const dest, const size_t byte_size, const size_t size_to_read, void *const fake)
{
    // Get the fake_file handle
    fake_file *const ff = reinterpret_cast<fake_file *const>(fake);

    // Size of the read
    const size_t read_length = byte_size * size_to_read;

    // Don't read past the end of the file
    char *read_end = ff->_head + read_length;

    // Calculate the size of the read, avoiding reading past EOF
    const size_t length = (read_end > ff->_end) ? ff->_end - ff->_head : read_length;

    // Copy the data
    std::memcpy(dest, ff->_head, length);

    // Increment the head
    ff->_head += length;

    // Return the read length
    return length;
}

int fake_seek_ogg(void *const fake, const ogg_int64_t to, const int type)
{
    // Get the fake_file handle
    fake_file *const ff = reinterpret_cast<fake_file *const>(fake);

    // Implement the seek operation in the fake file
    switch (type)
    {
    case SEEK_CUR:
        ff->_head += to;
        break;
    case SEEK_END:
        ff->_head = ff->_end - to;
        break;
    case SEEK_SET:
        ff->_head = ff->_start + to;
        break;
    default:
        return -1;
    }

    // If head is before the start of file
    if (ff->_head < ff->_start)
    {
        ff->_head = ff->_start;
    }

    // If head is past the end of file
    if (ff->_head > ff->_end)
    {
        ff->_head = ff->_end;
    }

    return 0;
}

int fake_close_ogg(void *const fake)
{
    // THE FILE IS FAKE!
    return 0;
}

long fake_tell_ogg(void *const fake)
{
    // Get the fake_file handle
    fake_file *const ff = reinterpret_cast<fake_file *const>(fake);

    // Return the location from the start of the file
    return (ff->_head - ff->_start);
}

class ogg
{
  private:
    uint16_t _num_channels;
    uint32_t _sample_rate;
    uint32_t _bits_per_sample;
    std::vector<uint8_t> _data;

    inline void load(const std::string _file)
    {
        std::ifstream file(_file, std::ios::in | std::ios::binary | std::ios::ate);
        if (file.is_open())
        {
            // Get the size of the file
            const auto size = file.tellg();

            // Adjust file pointer to beginning
            file.seekg(0, std::ios::beg);

            // Allocate space for new file
            std::vector<uint8_t> data(size);

            // Read bytes and close the file
            char *ptr = reinterpret_cast<char *>(data.data());
            file.read(ptr, size);

            // Close the file
            file.close();

            // Process the WAVE file
            load_little_endian_16<std::vector<uint8_t>>(data);
        }
        else
        {
            throw std::runtime_error("wave: Could not load file '" + _file + "'");
        }
    }
    template <class T>
    inline void load_little_endian_16(const T &data)
    {
        // Check that nothing funky is going on with char and uint8_t
        static_assert(sizeof(char) == 1, "Size of char is not 1");
        static_assert(sizeof(uint8_t) == 1, "Size of uint8_t is not 1");
        static_assert(std::is_same<std::uint8_t, unsigned char>::value,
                      "std::uint8_t must be implemented as unsigned char");

        // Tell OggVorbis that our file is in memory
        char *head = const_cast<char *>(reinterpret_cast<const char *>(&data[0]));
        fake_file ff(head, head, data.size());

        // Load an OggVorbis file
        OggVorbis_File ov_file;
        std::memset(&ov_file, 0, sizeof(OggVorbis_File));

        // Set the callbacks struct using the fake file interface
        ov_callbacks callbacks;
        callbacks.read_func = fake_read_ogg;
        callbacks.seek_func = fake_seek_ogg;
        callbacks.close_func = fake_close_ogg;
        callbacks.tell_func = fake_tell_ogg;

        // Open the file from memory
        const int ret = ov_open_callbacks((void *)&ff, &ov_file, nullptr, -1, callbacks);
        if (ret != 0)
        {
            throw std::runtime_error("ogg: Error at ov_open_callbacks");
        }

        // Get the info from the OggVorbis file
        const vorbis_info *const info = ov_info(&ov_file, -1);

        // Get the number of channels
        _num_channels = info->channels;

        // Get the sample rate
        _sample_rate = info->rate;

        // Local static buffer for copying data
        char copy[4096];
        uint8_t *from = reinterpret_cast<uint8_t *>(copy);

        // Assuming 16 bits per sample depth
        _bits_per_sample = 16;

        // Compressed data is ready to be decoded into data buffer
        const int endian = 0;
        const int depth = 2;
        const int sgned = 1;
        long bytes = 0;
        int bit_stream = 0;
        do
        {
            // Read up to a buffer's worth of decoded sound data
            // Sound samples must be 16 bit depth!
            bytes = ov_read(&ov_file, copy, 4096, endian, depth, sgned, &bit_stream);

            // Insert bytes into data buffer
            _data.insert(_data.end(), from, from + bytes);
        } while (bytes > 0);

        // Release the OggFile
        ov_clear(&ov_file);
    }

  public:
    ogg(const std::string &file)
    {
        load(file);
    }
    ogg(const mem_file &mem)
    {
        load_little_endian_16<mem_file>(mem);
    }
    void clear()
    {
        // Delete WAV data and reset WAV
        _data.clear();

        // Zero out fields so we dont try to use it later
        _num_channels = 0;
        _sample_rate = 0;
        _bits_per_sample = 0;
    }
    bool is_mono() const
    {
        return _num_channels == 1;
    }
    bool is_stereo() const
    {
        return _num_channels > 1;
    }
    const std::vector<uint8_t> &data() const
    {
        return _data;
    }
    uint32_t get_bits_per_sample() const
    {
        return _bits_per_sample;
    }
    size_t get_data_samples() const
    {
        // Calculate number of samples in data buffer
        return (_data.size() * 8) / _bits_per_sample;
    }
    uint32_t get_sample_rate() const
    {
        return _sample_rate;
    }
};
}
#endif
