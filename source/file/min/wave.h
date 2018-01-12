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
#ifndef __WAVE__
#define __WAVE__

#include <cstring>
#include <fstream>
#include <min/mem_chunk.h>
#include <string>
#include <vector>

// Reads a RIFF WAV File with two subchunks, 'fmt' and 'data'
// chunk_id (RIFF) - 4B - offset 0 == 'RIFF'
// chunk_size      - 4B - offset 4 == file_size - 8
// format (WAVE)   - 4B - offset 8 == 'WAVE'
// subchunk1_id    - 4B - offset 12 == "fmt "
// subchunk1_size  - 4B - offset 16 == 16 for PCM
// audio_format    - 2B - offset 20 == 1 (Linear Uncompressed PCM)
// num_channels    - 2B - offset 22 == Mono = 1, Stereo = 2
// sample_rate     - 4B - offset 24 == 44100 Hz (CD Quality)
// byte_rate       - 4B - offset 28 == sample_rate * num_channels * bytes_per_sample
// block_align     - 2B - offset 32 == num_channels * bytes_per_sample
// bits_per_sample - 2B - offset 34 == 'N' bits
// subchunk2_id    - 4B - offset 36 == "data"
// subchunk2_size  - 4B - offset 40 == data_size
// data            - ^^ - offset 44 == start of sound data

namespace min
{

class wave
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
            load<std::vector<uint8_t>>(data);
        }
        else
        {
            throw std::runtime_error("wave: Could not load file '" + _file + "'");
        }
    }
    template <class T>
    inline void load(const T &data)
    {
        // Check that nothing funky is going on with char and uint8_t
        static_assert(sizeof(char) == 1, "Size of char is not 1");
        static_assert(sizeof(uint8_t) == 1, "Size of uint8_t is not 1");
        static_assert(std::is_same<std::uint8_t, unsigned char>::value,
                      "std::uint8_t must be implemented as unsigned char");

        // Get the size of the file
        const size_t size = data.size();
        if (size < 44)
        {
            throw std::runtime_error("wave: File not large enough to be WAV file");
        }

        // Asserted that size is unsigned previously
        const size_t file_size = static_cast<size_t>(size);

        // Check if this is a RIFF file
        size_t offset = 0;
        const uint32_t chunk_id = min::read_le<uint32_t>(data, offset);
        if (chunk_id != 0x46464952)
        {
            throw std::runtime_error("wave: File not a RIFF file");
        }

        // Read the chunk size in little endian
        const uint32_t chunk_size = min::read_le<uint32_t>(data, offset);
        if (chunk_size != file_size - 8)
        {
            throw std::runtime_error("wave: Invalid chunk size");
        }

        // Check if this is a WAVE file
        const uint32_t format = min::read_le<uint32_t>(data, offset);
        if (format != 0x45564157)
        {
            throw std::runtime_error("wave: File not a WAVE file");
        }

        // Check the first subchunk ID
        const uint32_t subchunk1_id = min::read_le<uint32_t>(data, offset);
        if (subchunk1_id != 0x20746D66)
        {
            throw std::runtime_error("wave: Invalid subchunk1 ID");
        }

        // Check that the subchunk1 size equates to PCM
        const uint32_t subchunk1_size = min::read_le<uint32_t>(data, offset);
        if (subchunk1_size != 16)
        {
            throw std::runtime_error("wave: WAV not storing PCM data");
        }

        // Check that the audio format is linear PCM
        const uint16_t audio_format = min::read_le<uint16_t>(data, offset);
        if (audio_format != 1)
        {
            throw std::runtime_error("wave: WAV not storing linear quantized PCM data");
        }

        // Read the num_channels and sample rate
        _num_channels = min::read_le<uint16_t>(data, offset);
        _sample_rate = min::read_le<uint32_t>(data, offset);

        // Read the byte rate, block align, and bits per sample
        const uint32_t byte_rate = min::read_le<uint32_t>(data, offset);
        const uint16_t block_align = min::read_le<uint16_t>(data, offset);

        _bits_per_sample = min::read_le<uint16_t>(data, offset);

        // Store the bytes per sample
        const uint16_t bytes_per_sample = _bits_per_sample / 8;

        // Check that the byte rate is correct
        if (byte_rate != _sample_rate * _num_channels * bytes_per_sample)
        {
            throw std::runtime_error("wave: Incorrect byte rate specified");
        }

        // Check that the block align is correct
        if (block_align != _num_channels * bytes_per_sample)
        {
            throw std::runtime_error("wave: Incorrect block align specified");
        }

        // Read the next subchunk ID and size
        uint32_t subchunk2_id = min::read_le<uint32_t>(data, offset);
        uint32_t subchunk2_size = min::read_le<uint32_t>(data, offset);

        // Check the second subchunk ID
        while (subchunk2_id != 0x61746164)
        {
            // Check that we didn't read past the end of file
            if (offset >= file_size)
            {
                throw std::runtime_error("wave: Can't find data chunk ID in file");
            }

            // Skip over the chunk
            offset += subchunk2_size;

            // Read the chunk contents
            subchunk2_id = min::read_le<uint32_t>(data, offset);
            subchunk2_size = min::read_le<uint32_t>(data, offset);
        }

        // Check that the subchunk2 size is valid
        if (subchunk2_size > file_size - 44)
        {
            throw std::runtime_error("wave: not enough sound data found in file");
        }

        // Allocate data and write into data buffer
        _data.resize(subchunk2_size);
        for (size_t i = 0; i < subchunk2_size; i++)
        {
            _data[i] = data[i + offset];
        }
    }

  public:
    wave(const std::string &file)
    {
        load(file);
    }
    wave(const mem_file &mem)
    {
        load<mem_file>(mem);
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
