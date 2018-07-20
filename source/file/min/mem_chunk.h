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
#ifndef _MGL_MEMORY_CHUNK_MGL_
#define _MGL_MEMORY_CHUNK_MGL_

#include <cstring>
#include <fstream>
#include <min/serial.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace min
{

class mem_chunk
{
  private:
    std::vector<uint8_t> _file_data;
    std::unordered_map<std::string, mem_file> _files;

    inline mem_file push_back_file(const std::string &file_name)
    {
        // Read bytes from file
        std::ifstream file(file_name, std::ios::in | std::ios::binary | std::ios::ate);
        if (file.is_open())
        {
            // Get the size of the file
            const std::streampos size = file.tellg();

            // Adjust file pointer to beginning
            file.seekg(0, std::ios::beg);

            // Allocate space for new file at end of file list
            const size_t end = _file_data.size();
            _file_data.resize(end + size);

            // Read bytes and close the file
            char *data = reinterpret_cast<char *>(&_file_data[end]);
            file.read(data, size);
            file.close();

            // Return the file position
            return mem_file(&_file_data, end, size);
        }
        else
        {
            throw std::runtime_error("mem_chunk: could not read file '" + file_name + "'");
        }
    }
    inline void load_memory_file(const std::string &file_name)
    {
        // Check that nothing funky is going on with char and uint8_t
        static_assert(sizeof(char) == 1, "Size of char is not 1");
        static_assert(sizeof(uint8_t) == 1, "Size of uint8_t is not 1");
        static_assert(std::is_same<std::uint8_t, unsigned char>::value,
                      "std::uint8_t must be implemented as unsigned char");

        // Read bytes from file
        std::ifstream file(file_name, std::ios::in | std::ios::binary | std::ios::ate);
        if (file.is_open())
        {
            // Get the size of the file
            std::streampos size = file.tellg();

            // Reserve space for the data
            std::vector<uint8_t> data;
            data.resize(size, 0);

            // Adjust file pointer to beginning
            file.seekg(0, std::ios::beg);

            // Read bytes and close the file
            char *ptr = reinterpret_cast<char *>(data.data());
            file.read(ptr, size);
            file.close();

            // Get the size of the entire file
            const size_t data_size = data.size();

            // Read the header from the data
            size_t next = 0;

            // Get the number of files in file data
            const uint32_t file_data_size = read_le<uint32_t>(data, next);

            // Allocate space for the data file list
            _file_data.resize(file_data_size);

            // Get the number of files in file list
            const uint32_t files = read_le<uint32_t>(data, next);

            // Read each file description
            uint32_t accum_file = 0;
            for (uint32_t i = 0; i < files; i++)
            {
                // Get the offset into data of file
                const uint32_t offset = read_le<uint32_t>(data, next);

                // Get the size of the file
                const uint32_t file_size = read_le<uint32_t>(data, next);

                // Accumulate the file sizes
                accum_file += file_size;

                // Read size of the file name
                const uint32_t str_size = read_le<uint32_t>(data, next);

                // Allocate space for file name
                std::string name(str_size, 0);

                // Read the file name
                for (uint32_t j = 0; j < str_size; j++)
                {
                    name[j] = read_le<char>(data, next);
                }

                // Add file into the file list
                _files.insert({name, mem_file(&_file_data, offset, file_size)});
            }

            // Check if the file content description matches the allocated declaration
            if (accum_file != file_data_size)
            {
                throw std::runtime_error("mem_chunk: corrupt header, file description does not match allocated declaration");
            }

            // Check if we are reading the correct section
            if (data_size != next + accum_file)
            {
                throw std::runtime_error("mem_chunk: corrupt header, torn file data section");
            }

            // Copy the file data section
            const size_t data_offset = next;
            for (size_t i = data_offset; i < data_size; i++)
            {
                _file_data[i - data_offset] = data[i];
            }
        }
        else
        {
            throw std::runtime_error("mem_chunk: could not read file '" + file_name + "'");
        }
    }
    inline void save_memory_file(const std::string &file_name) const
    {
        // Check that nothing funky is going on with char and uint8_t
        static_assert(sizeof(char) == 1, "Size of char is not 1");
        static_assert(sizeof(uint8_t) == 1, "Size of uint8_t is not 1");
        static_assert(std::is_same<std::uint8_t, unsigned char>::value,
                      "std::uint8_t must be implemented as unsigned char");

        // Save bytes to file
        std::ofstream file(file_name, std::ios::out | std::ios::binary);
        if (file.is_open())
        {
            // Generate header byte stream
            std::vector<uint8_t> header;

            // Write the size of the file data
            write_le<uint32_t>(header, _file_data.size());

            // Write the number of files in file list
            write_le<uint32_t>(header, _files.size());

            // Write the map contents to header
            for (auto &p : _files)
            {
                // Write the file offset to header
                write_le<uint32_t>(header, p.second.offset());

                // Write file size to header
                write_le<uint32_t>(header, p.second.size());

                // Write the size of the file name string
                const size_t str_size = p.first.size();
                write_le<uint32_t>(header, str_size);

                // Write each character in file name
                for (size_t i = 0; i < str_size; i++)
                {
                    write_le<char>(header, p.first[i]);
                }
            }

            // Write the header file
            const char *header_data = reinterpret_cast<const char *>(header.data());
            file.write(header_data, header.size());

            // Write the file data
            const char *file_data = reinterpret_cast<const char *>(_file_data.data());
            file.write(file_data, _file_data.size());

            // Close the file
            file.close();
        }
        else
        {
            throw std::runtime_error("mem_chunk: could not save file '" + file_name + "'");
        }
    }

  public:
    mem_chunk() {}
    mem_chunk(const std::string &file)
    {
        // Load the file list from a file
        load_memory_file(file);
    }
    inline void add_file(const std::string &file)
    {
        const auto &i = _files.find(file);
        if (i == _files.end())
        {
            // Push file onto end of file list
            const mem_file mf = push_back_file(file);

            // Insert the file name into file list
            _files.insert({file, mf});
        }
        else
        {
            throw std::runtime_error("mem_chunk: duplicate file " + file + " insert request");
        }
    }
    inline void clear()
    {
        // This function should deallocate all held memory
        std::vector<uint8_t>().swap(_file_data);
        std::unordered_map<std::string, mem_file>().swap(_files);
    }
    inline const mem_file &get_file(const std::string &key) const
    {
        // Lookup key in the map
        const auto &i = _files.find(key);
        if (i != _files.end())
        {
            return i->second;
        }
        else
        {
            throw std::runtime_error("mem_chunk: file " + key + " is not in the file list");
        }
    }
    inline mem_file &get_file(const std::string &key)
    {
        // Lookup key in the map
        const auto &i = _files.find(key);
        if (i != _files.end())
        {
            return i->second;
        }
        else
        {
            throw std::runtime_error("mem_chunk: file " + key + " is not in the file list");
        }
    }
    inline size_t size() const
    {
        return _files.size();
    }
    inline void write_memory_file(const std::string &file)
    {
        // Save the file list to a file
        save_memory_file(file);
    }
};
}
#endif
