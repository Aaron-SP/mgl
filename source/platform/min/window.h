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
#ifndef __WINDOW__
#define __WINDOW__

#if defined(_WIN32)

#include <min/win32_window.h>

namespace min
{
using window = min::win32_window;
}

#elif __linux__

#include <min/x_window.h>

namespace min
{
using window = min::x_window;
}

#endif

#endif
