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
#ifndef _MGL_TESTUTIL_MGL_
#define _MGL_TESTUTIL_MGL_

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <utility>

std::string ask(const std::string &q)
{
    // Ask a question to the tester
    std::string s;
    std::cout << q << std::endl;
    std::cout << "answer(y/n) >> ";
    std::cin >> s;

    // Transform answer to lowercase
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    // Return the answer
    return s;
}

template <typename T>
struct identity
{
    typedef T type;
};
template <typename T>
using identity_t = typename identity<T>::type;

template <typename T>
bool compare(const T one, const identity_t<T> two)
{
    return one == two;
}
template <typename T>
bool test(const T one, const identity_t<T> two, const char *fail)
{
    const bool out = compare<T>(one, two);
    if (!out)
    {
        throw std::runtime_error(fail);
    }
    return out;
}
template <typename T>
bool compare(const T one, const identity_t<T> two, const identity_t<T> threshold)
{
    return std::abs(one - two) <= threshold;
}
template <typename T>
bool test(const T one, const identity_t<T> two, const identity_t<T> tol, const char *fail)
{
    const bool out = compare(one, two, tol);
    if (!out)
    {
        throw std::runtime_error(fail);
    }
    return out;
}

bool compare(const std::string &s1, const std::string &s2)
{
    // Compare string
    return s1 == s2;
}

#endif
