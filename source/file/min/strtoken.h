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
#ifndef __STRINGTOOLS__
#define __STRINGTOOLS__

#include <algorithm>
#include <cctype>
#include <functional>
#include <locale>
#include <vector>

namespace min
{

// trim from start
inline std::string &ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
inline std::string &rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
inline std::string &trim(std::string &s)
{
    return ltrim(rtrim(s));
}

// Split string on 'space' characters, 'space' is an abstract space separator define by ptr_fun
inline std::vector<std::string> split(const std::string &s, const std::pointer_to_unary_function<int, int> &f, const size_t hint)
{
    // Output string buffer
    std::vector<std::string> out;
    out.reserve(hint);
    size_t start = 0;

    // Find all 'spaces' and split up string
    auto size = s.size();
    for (size_t i = 0; i < size; i++)
    {
        if (f(s[i]) != 0)
        {
            // Skip all adjacent 'spaces'
            size_t j = i;
            while (j + 1 < size && f(s[j + 1]) != 0)
            {
                j++;
            }

            // Calculate the string position to character before first 'space'
            const size_t length = i - start;
            if (length != 0)
            {
                // Make substring
                out.push_back(s.substr(start, length));
            }

            // Skip over all adjacent 'spaces'
            start = j + 1;
        }
    }

    // Add remaining characters at end of string
    const size_t length = size - start;
    if (length != 0)
    {
        out.push_back(s.substr(start, length));
    }

    return out;
}

inline int isequal(int ch)
{
    if (ch == '=')
    {
        // Positive test
        return 1;
    }

    // Negative test
    return 0;
}

inline int isslash(int ch)
{
    if (ch == '/')
    {
        // Positive test
        return 1;
    }

    // Negative test
    return 0;
}

// This is effectively regex split on, "\\s+"
inline std::vector<std::string> split_space(const std::string &s, const size_t hint = 0)
{
    return split(s, std::ptr_fun<int, int>(std::isspace), hint);
}

// This is effectively regex split on, "\\/+"
inline std::vector<std::string> split_equal(const std::string &s, const size_t hint = 0)
{
    return split(s, std::ptr_fun<int, int>(isequal), hint);
}

// This is effectively regex split on, "=+"
inline std::vector<std::string> split_slash(const std::string &s, const size_t hint = 0)
{
    return split(s, std::ptr_fun<int, int>(isslash), hint);
}

inline std::vector<std::pair<size_t, size_t>> read_lines(const std::string &data, const size_t hint = 0)
{
    // Create output vector
    std::vector<std::pair<size_t, size_t>> out;
    out.reserve(hint);

    // Start locations within the data buffer for each line
    size_t start = 0;

    // For each character in the data
    auto size = data.size();
    for (size_t i = 0; i < size; i++)
    {
        if (data[i] == '\n' || (data[i] == '\r' && i != size - 1 && data[i + 1] == '\n'))
        {
            // Calculate the string position removing the \n, discard empty lines
            const size_t length = i - start;
            if (length != 0)
            {
                // Record line position numbers
                out.emplace_back(start, length);
            }

            // Reset start index, start == size if 'space' ends the string
            start = i + 1;
        }
    }

    return out;
}

inline std::vector<std::string> get_lines(const std::string &data, const std::vector<std::pair<size_t, size_t>> &lines, const unsigned count, size_t &i)
{
    std::vector<std::string> out;
    out.reserve(count);

    for (size_t j = 0; j < count; j++)
    {
        // Go to the next line
        i++;

        // read line and trim the line whitespace
        const auto &position = lines[i];
        std::string line = data.substr(position.first, position.second);
        min::trim(line);

        // skip empty line size in bytes
        if (line.size() == 0)
        {
            j--;
            continue;
        }

        // Save this line
        out.push_back(line);
    }

    // Check that we got the correct number of full lines
    if (out.size() != count)
    {
        throw std::runtime_error("get_lines: Read wrong number of lines");
    }

    return out;
}

// Convert string to lowercase
std::string to_lower(const std::string &s)
{
    // Transform string to lower case and make copy
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);

    return out;
}
}
#endif
