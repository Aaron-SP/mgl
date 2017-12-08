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
#ifndef __SETTINGS__
#define __SETTINGS__

#include <min/window.h>
#include <string>

namespace min
{

class settings
{
  public:
    static void initialize()
    {
        // Depth settings
        glEnable(GL_DEPTH_TEST);
        glClearDepth(0.0);
        glClear(GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_GEQUAL);

        // Face culling settings
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);

        // Allow setting point size in vertex shader for particles
        glEnable(GL_PROGRAM_POINT_SIZE);

        // Enable blending for rendering text
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
};
}

#endif
