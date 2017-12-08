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
#ifndef __TESTSAMPLE__
#define __TESTSAMPLE__

#include <min/quat.h>
#include <min/sample.h>
#include <min/test.h>
#include <min/vec2.h>
#include <min/vec3.h>
#include <min/vec4.h>
#include <stdexcept>

bool test_sample()
{
    bool out = true;

    // vec2 sample
    {
        // Local variables
        min::vec2<double> src;
        min::vec2<double> dst;
        min::vec2<double> p;
        min::sample<double, min::vec2> s;

        // Test lerp
        src = min::vec2<double>(2.0, 2.0);
        dst = min::vec2<double>(4.0, 4.0);
        s = min::sample<double, min::vec2>(src, dst);
        p = s.lerp(0.5);
        out = out && compare(3.0, p.x(), 1E-4);
        out = out && compare(3.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sample lerp");
        }

        // Test interpolate
        src = min::vec2<double>(2.0, 2.0);
        dst = min::vec2<double>(4.0, 4.0);
        s = min::sample<double, min::vec2>(src, dst);
        p = s.interpolate(0.5);
        out = out && compare(3.0, p.x(), 1E-4);
        out = out && compare(3.0, p.y(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec2 sample interpolate");
        }
    }

    // vec3 sample
    {
        // Local variables
        min::vec3<double> src;
        min::vec3<double> dst;
        min::vec3<double> p;
        min::sample<double, min::vec3> s;

        // Test lerp
        src = min::vec3<double>(2.0, 2.0, 2.0);
        dst = min::vec3<double>(4.0, 4.0, 4.0);
        s = min::sample<double, min::vec3>(src, dst);
        p = s.lerp(0.5);
        out = out && compare(3.0, p.x(), 1E-4);
        out = out && compare(3.0, p.y(), 1E-4);
        out = out && compare(3.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sample lerp");
        }

        // Test interpolate
        src = min::vec3<double>(2.0, 2.0, 2.0);
        dst = min::vec3<double>(4.0, 4.0, 4.0);
        s = min::sample<double, min::vec3>(src, dst);
        p = s.interpolate(0.5);
        out = out && compare(3.0, p.x(), 1E-4);
        out = out && compare(3.0, p.y(), 1E-4);
        out = out && compare(3.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec3 sample interpolate");
        }
    }

    // vec4 sample
    {
        // Local variables
        min::vec4<double> src;
        min::vec4<double> dst;
        min::vec4<double> p;
        min::sample<double, min::vec4> s;

        // Test lerp
        src = min::vec4<double>(2.0, 2.0, 2.0, 0.0);
        dst = min::vec4<double>(4.0, 4.0, 4.0, 0.0);
        s = min::sample<double, min::vec4>(src, dst);
        p = s.lerp(0.5);
        out = out && compare(3.0, p.x(), 1E-4);
        out = out && compare(3.0, p.y(), 1E-4);
        out = out && compare(3.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sample lerp");
        }

        // Test interpolate
        src = min::vec4<double>(2.0, 2.0, 2.0, 0.0);
        dst = min::vec4<double>(4.0, 4.0, 4.0, 0.0);
        s = min::sample<double, min::vec4>(src, dst);
        p = s.interpolate(0.5);
        out = out && compare(3.0, p.x(), 1E-4);
        out = out && compare(3.0, p.y(), 1E-4);
        out = out && compare(3.0, p.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed vec4 sample interpolate");
        }
    }

    // quat sample
    {
        // Local variables
        min::quat<double> src;
        min::quat<double> dst;
        min::quat<double> q;
        min::sample<double, min::quat> s;

        // Test lerp
        src = min::quat<double>(1.0, 1.0, 1.0);
        dst = min::quat<double>(-1.0, 1.0, -1.0);
        s = min::sample<double, min::quat>(src, dst);
        q = s.lerp(0.2);
        out = out && compare(0.606339, q.w(), 1E-4);
        out = out && compare(0.363803, q.x(), 1E-4);
        out = out && compare(0.606339, q.y(), 1E-4);
        out = out && compare(0.363803, q.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed quat sample lerp");
        }

        // Test lerp
        src = min::quat<double>(1.0, 1.0, 1.0);
        dst = min::quat<double>(-1.0, 1.0, -1.0);
        s = min::sample<double, min::quat>(src, dst);
        q = s.interpolate(0.2);
        out = out && compare(0.630037, q.w(), 1E-4);
        out = out && compare(0.32102, q.x(), 1E-4);
        out = out && compare(0.630037, q.y(), 1E-4);
        out = out && compare(0.32102, q.z(), 1E-4);
        if (!out)
        {
            throw std::runtime_error("Failed quat sample lerp");
        }
    }
    return out;
}

#endif
