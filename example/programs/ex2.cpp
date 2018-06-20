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
#include <algorithm>
#include <min/bmp.h>
#include <min/dds.h>
#include <min/strtoken.h>
#include <min/texture_compressor.h>
#include <min/window.h>
#include <stdexcept>
#include <string>

void write_file(const std::string &file_path, const min::dds &d)
{
    // Get the file byte vector
    std::vector<uint8_t> output = d.to_file();

    // Open output binary file
    std::ofstream file(file_path, std::ios::out | std::ios::binary);
    if (file.is_open())
    {
        // Write bytes to file
        file.write((char *)&output[0], output.size());

        // Close the file
        file.close();
    }
}

// Mode 1 = DXT1; Mode 3 = DXT3; Mode 5 = DXT5
void bmp_to_dds_compress(const std::string &input, const std::string &output, bool mips, const int mode)
{
    if (mode == 1 || mode == 3 || mode == 5)
    {
        // Need OpenGL pipeline to compress
        min::window win("Compressing texture", 720, 480, 3, 3);

        // Load input texture
        const min::bmp b = min::bmp(input);

        // Create texture compressor for compressing input
        min::texture_compressor compress;
        if (mips == false)
        {
            // Only generate one texture level
            compress.disable_mip_maps();
        }

        // Choose compression function
        if (mode == 1)
        {
            // DXT1 compression
            min::dds d = compress.compress_bmp_dds_dxt1(b);

            // Output to file
            write_file(output, d);
        }
        else if (mode == 3)
        {
            // DXT3 compression
            min::dds d = compress.compress_bmp_dds_dxt3(b);

            // Output to file
            write_file(output, d);
        }
        else if (mode == 5)
        {
            // DXT5 compression
            min::dds d = compress.compress_bmp_dds_dxt5(b);

            // Output to file
            write_file(output, d);
        }
    }
    else
    {
        throw std::runtime_error("compress: Invalid mode '" + std::to_string(mode) + "' passed to compress");
    }
}

void fatal_error()
{
    std::cout << "ERROR: Invalid arguments specified" << std::endl;
    std::cout << "correct command structure is: 'compress <uncompressed.bmp> -o <compressed.dds> -t <compression_type> -m <bool>'" << std::endl;
}

// Returns 1 on error, 0 on success
int main(int argc, char *argv[])
{
    try
    {
        // Check the argument count
        if (argc < 2)
        {
            fatal_error();
            return 1;
        }

        // Input file name <uncompressed.bmp>
        std::string input(argv[1]);
        std::string output("out.dds");
        std::string format("dxt1");
        std::string mip_flag("true");

        // Parse inputs on command line
        for (int i = 3; i < argc; i += 2)
        {
            // Get flag parameter
            std::string flag(argv[i - 1]);

            // Check for overriding each flag
            if (flag.compare("-o") == 0)
            {
                // Output file name <compressed.dds>
                output = std::string(argv[i]);
            }
            else if (flag.compare("-t") == 0)
            {
                // Convert to lowercase for string comparison
                format = std::string(min::to_lower(argv[i]));
            }
            else if (flag.compare("-m") == 0)
            {
                mip_flag = std::string(min::to_lower(argv[i]));
            }
            else
            {
                std::cout << "ERROR: Invalid flag: unknown flag '"
                          << flag << "', expected '-o', '-t' or 'm'" << std::endl;
                return 1;
            }
        }

        // Generate mip maps?
        bool mips = true;
        if (mip_flag.compare("false") == 0)
        {
            mips = false;
        }
        else if (mip_flag.compare("true") != 0)
        {
            std::cout << "ERROR: Invalid mip flag: Valid boolean flags for -m are 'true' and 'false', default value is 'true'" << std::endl;
            return 1;
        }

        // Choose the correct format
        if (format.compare("dxt1") == 0)
        {
            // Compressing to DXT1
            bmp_to_dds_compress(input, output, mips, 1);
        }
        else if (format.compare("dxt3") == 0)
        {
            // Compressing to DXT3
            bmp_to_dds_compress(input, output, mips, 3);
        }
        else if (format.compare("dxt5") == 0)
        {
            // Compressing to DXT5
            bmp_to_dds_compress(input, output, mips, 5);
        }
        else
        {
            std::cout << "ERROR: Invalid format: Valid formats are dxt1, dxt3, and dxt5 when using the -t flag" << std::endl;
            return 1;
        }
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
