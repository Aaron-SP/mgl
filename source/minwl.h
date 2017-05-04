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
#include <min/settings.h>
#include <min/texture_compressor.h>
#include <min/window.h>
#include <min/loop_sync.h>
#include <min/shader.h>
#include <min/program.h>
#include <min/emitter_buffer.h>
#include <min/skeletal_vertex.h>
#include <min/static_vertex.h>
#include <min/text_buffer.h>
#include <min/texture_buffer.h>
#include <min/uniform_buffer.h>
#include <min/vertex_buffer.h>

template class min::emitter_buffer<float, GL_FLOAT>;
template class min::vertex_buffer<float, uint16_t, min::skeletal_vertex, GL_FLOAT, GL_UNSIGNED_SHORT>;
template class min::vertex_buffer<float, uint16_t, min::static_vertex, GL_FLOAT, GL_UNSIGNED_SHORT>;
template class min::uniform_buffer<float>;
