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
#ifndef __TESTTRANSFORM2__
#define __TESTTRANSFORM2__

#include <min/test.h>
#include <min/tran2.h>
#include <stdexcept>

bool test_tran2()
{
    bool out = true;

    // Local variables
    min::vec2<double> v;
    min::vec2<double> v2;
    min::vec3<double> v3;
    min::tran2<double> tran;

    // Test identity translate matrix
    v = min::vec2<double>(1.0, 2.0);
    v3 = min::tran2<double>().transform(v);
    out = out && compare(1.0, v3.x(), 1E-4);
    out = out && compare(2.0, v3.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran2 identity translation");
    }

    // Test identity rotate (1.0, 1.0) -> (-1.0, 1.0)
    v = min::vec2<double>(1.0, 1.0);
    v3 = min::tran2<double>().rotate(min::mat2<double>(90.0)).transform(v);
    out = out && compare(-1.0, v3.x(), 1E-4);
    out = out && compare(1.0, v3.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran2 identity-rotation");
    }

    // Test translation (1.0, 2.0) -> (2.0, 4.0)
    v = min::vec2<double>(1.0, 2.0);
    v3 = min::tran2<double>(v).transform(v);
    out = out && compare(2.0, v3.x(), 1E-4);
    out = out && compare(4.0, v3.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran2 translate");
    }

    // Test multi translation (1.0, 1.0) -> (0.0, 0.0)
    v = min::vec2<double>(1.0, 1.0);
    v2 = min::vec2<double>(-1.0, -1.0);
    v3 = min::tran2<double>(v).translate(v2).translate(v2).transform(v);
    out = out && compare(0.0, v3.x(), 1E-4);
    out = out && compare(0.0, v3.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran2 translation-translation");
    }

    // Test rotation (1.0, 0.0) -> (0.0, 1.0)
    v = min::vec2<double>(1.0, 0.0);
    v3 = min::tran2<double>(90.0).transform(v);
    out = out && compare(0.0, v3.x(), 1E-4);
    out = out && compare(1.0, v3.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran2 rotate");
    }

    // Test multi rotation (1.0, 0.0) -> (-1.0, 0.0)
    v = min::vec2<double>(1.0, 0.0);
    v3 = min::tran2<double>(90.0).rotate(90.0).transform(v);
    out = out && compare(-1.0, v3.x(), 1E-4);
    out = out && compare(0.0, v3.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran2 rotate-rotate");
    }

    // Test rotate to translate (1.0, 1.0) -> (-1.0, 1.0) -> (0.0, 2.0)
    v = min::vec2<double>(1.0, 1.0);
    v3 = min::tran2<double>(90.0).translate(v).transform(v);
    out = out && compare(0.0, v3.x(), 1E-4);
    out = out && compare(2.0, v3.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran2 rotation-translation");
    }

    // Test translate to rotate (1.0, 1.0) -> (2.0, 2.0) -> (-2.0, 2.0)
    v = min::vec2<double>(1.0, 1.0);
    v3 = min::tran2<double>(v).rotate(90.0).transform(v);
    out = out && compare(-2.0, v3.x(), 1E-4);
    out = out && compare(2.0, v3.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran2 translation-rotation");
    }

    // Test orthogonal decomposition
    // (-2.0, 2.0)-> (2.0, 2.0) -> (1.0, 1.0)
    v = min::vec2<double>(-1.0, -1.0);
    tran = min::tran2<double>(90.0).transpose().translate(v);
    v = min::vec2<double>(-2.0, 2.0);
    v3 = tran.transform(v);
    out = out && compare(1.0, v3.x(), 1E-4);
    out = out && compare(1.0, v3.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran2 orthogonal decomposition");
    }

    // Test scale (2.0, 1.0) -> (4.0, 2.0)
    v = min::vec2<double>(2.0, 1.0);
    v3 = min::tran2<double>().scale(2.0, 2.0).transform(v);
    out = out && compare(4.0, v3.x(), 1E-4);
    out = out && compare(2.0, v3.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran2 scale");
    }

    // Test scale rotate translate (2.0, 1.0) -> (4.0, 2.0) -> (2.0, -4.0) -> (0.0, 0.0)
    v = min::vec2<double>(2.0, 1.0);
    v2 = min::vec2<double>(-2.0, 4.0);
    v3 = min::tran2<double>().scale(2.0, 2.0).rotate(-90.0).translate(v2).transform(v);
    out = out && compare(0.0, v3.x(), 1E-4);
    out = out && compare(0.0, v3.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran2 scale-rotate-translate");
    }

    // Test non-orthogonal inverse
    // Test scale rotate translate (0.0, 0.0) -> (2.0, -4.0) -> (4.0, 2.0) -> (2.0, 1.0)
    v = min::vec2<double>(0.0, 0.0);
    v2 = min::vec2<double>(-2.0, 4.0);
    tran = min::tran2<double>().scale(2.0, 2.0).rotate(-90.0).translate(v2).invert();
    v3 = tran.transform(v);
    out = out && compare(2.0, v3.x(), 1E-4);
    out = out && compare(1.0, v3.y(), 1E-4);
    if (!out)
    {
        throw std::runtime_error("Failed tran2 non-orthogonal inverse");
    }

    return out;
}

#endif
