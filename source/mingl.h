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
// file
#include <min/bmp.h>
#include <min/dds.h>
#include <min/md5_anim.h>
#include <min/md5_mesh.h>
#include <min/wavefront.h>

// geom
#include <min/aabbox.h>
#include <min/convert.h>
#include <min/frustum.h>
#include <min/intersect.h>
#include <min/mesh.h>
#include <min/plane.h>
#include <min/ray.h>
#include <min/sphere.h>

// math
#include <min/mat2.h>
#include <min/mat3.h>
#include <min/mat4.h>
#include <min/quat.h>
#include <min/sample.h>
#include <min/tran2.h>
#include <min/tran3.h>
#include <min/vec2.h>
#include <min/vec3.h>
#include <min/vec4.h>

// scene
#include <min/camera.h>
#include <min/grid.h>
#include <min/light.h>
#include <min/md5_model.h>
#include <min/model.h>
#include <min/tree.h>

// file
template class min::md5_anim<float>;
template class min::md5_mesh<float, uint16_t>;
template class min::wavefront<float, uint16_t>;

// geom
template class min::aabbox<float, min::vec2>;
template class min::aabbox<float, min::vec3>;
template class min::frustum<float>;
template class min::plane<float, min::vec2>;
template class min::plane<float, min::vec3>;
template class min::ray<float, min::vec2>;
template class min::ray<float, min::vec3>;
template class min::sphere<float, min::vec2>;
template class min::sphere<float, min::vec3>;
template class min::mesh<float, uint16_t>;
template min::mesh<float, uint16_t> min::to_mesh<float, uint16_t>(const min::sphere<float, min::vec3> &s);
template min::mesh<float, uint16_t> min::to_mesh<float, uint16_t>(const min::aabbox<float, min::vec3> &b);

// math
template class min::mat2<float>;
template class min::mat3<float>;
template class min::mat4<float>;
template class min::quat<float>;
template class min::sample<float, min::vec2>;
template class min::sample<float, min::vec3>;
template class min::sample<float, min::vec4>;
template class min::sample<float, min::quat>;
template class min::tran2<float>;
template class min::tran3<float>;
template class min::vec2<float>;
template class min::vec3<float>;
template class min::vec4<float>;

// intersection 2D
template bool min::intersect<float>(const sphere<float, min::vec2> &s, const ray<float, min::vec2> &ray, min::vec2<float> &p);
template bool min::intersect<float>(const aabbox<float, min::vec2> &box, const ray<float, min::vec2> &ray, min::vec2<float> &p);
template bool min::intersect<float>(const sphere<float, min::vec2> &s1, const sphere<float, min::vec2> &s2);
template bool min::intersect<float>(const sphere<float, min::vec2> &s1, const sphere<float, min::vec2> &s2, min::vec2<float> &p);
template bool min::intersect<float>(const sphere<float, min::vec2> &s, const aabbox<float, min::vec2> &box);
template bool min::intersect<float>(const aabbox<float, min::vec2> &box, const sphere<float, min::vec2> &s);
template bool min::intersect<float>(const sphere<float, min::vec2> &s, const aabbox<float, min::vec2> &box, min::vec2<float> &p);
template bool min::intersect<float>(const aabbox<float, min::vec2> &box, const sphere<float, min::vec2> &s, min::vec2<float> &p);
template bool min::intersect<float>(const aabbox<float, min::vec2> &box1, const aabbox<float, min::vec2> &box2);
template bool min::intersect<float>(const aabbox<float, min::vec2> &box1, const aabbox<float, min::vec2> &box2, min::vec2<float> &p);

// intersection 3D
template bool min::intersect<float>(const sphere<float, min::vec3> &s, const ray<float, min::vec3> &ray, min::vec3<float> &p);
template bool min::intersect<float>(const aabbox<float, min::vec3> &box, const ray<float, min::vec3> &ray, min::vec3<float> &p);
template bool min::intersect<float>(const sphere<float, min::vec3> &s1, const sphere<float, min::vec3> &s2);
template bool min::intersect<float>(const sphere<float, min::vec3> &s1, const sphere<float, min::vec3> &s2, min::vec3<float> &p);
template bool min::intersect<float>(const sphere<float, min::vec3> &s, const aabbox<float, min::vec3> &box);
template bool min::intersect<float>(const aabbox<float, min::vec3> &box, const sphere<float, min::vec3> &s);
template bool min::intersect<float>(const sphere<float, min::vec3> &s, const aabbox<float, min::vec3> &box, min::vec3<float> &p);
template bool min::intersect<float>(const aabbox<float, min::vec3> &box, const sphere<float, min::vec3> &s, min::vec3<float> &p);
template bool min::intersect<float>(const aabbox<float, min::vec3> &box1, const aabbox<float, min::vec3> &box2);
template bool min::intersect<float>(const aabbox<float, min::vec3> &box1, const aabbox<float, min::vec3> &box2, min::vec3<float> &p);

// intersection frustum
template bool min::intersect<float>(const frustum<float> &f, const sphere<float, min::vec3> &s);
template bool min::intersect<float>(const frustum<float> &f, const sphere<float, min::vec3> &s, min::vec3<float> &p);
template bool min::intersect<float>(const frustum<float> &f, const aabbox<float, min::vec3> &box);
template bool min::intersect<float>(const frustum<float> &f, const aabbox<float, min::vec3> &box, min::vec3<float> &p);

// scene
template class min::camera<float>;
template class min::grid<float, uint16_t, uint32_t, min::vec2, min::aabbox, min::aabbox>;
template class min::grid<float, uint16_t, uint32_t, min::vec3, min::aabbox, min::aabbox>;
template class min::grid<float, uint16_t, uint32_t, min::vec2, min::sphere, min::aabbox>;
template class min::grid<float, uint16_t, uint32_t, min::vec3, min::sphere, min::aabbox>;
template class min::grid<float, uint16_t, uint32_t, min::vec2, min::aabbox, min::sphere>;
template class min::grid<float, uint16_t, uint32_t, min::vec3, min::aabbox, min::sphere>;
template class min::grid<float, uint16_t, uint32_t, min::vec2, min::sphere, min::sphere>;
template class min::grid<float, uint16_t, uint32_t, min::vec3, min::sphere, min::sphere>;
template class min::light<float>;
template class min::model<float, uint16_t, min::vec4, min::aabbox>;
template class min::model<float, uint16_t, min::vec4, min::sphere>;
template class min::md5_model<float, uint32_t, min::vec4, min::aabbox>;
template class min::md5_model<float, uint32_t, min::vec4, min::sphere>;
template class min::tree<float, uint16_t, uint32_t, min::vec2, min::aabbox, min::aabbox>;
template class min::tree<float, uint16_t, uint32_t, min::vec3, min::aabbox, min::aabbox>;
template class min::tree<float, uint16_t, uint32_t, min::vec2, min::sphere, min::aabbox>;
template class min::tree<float, uint16_t, uint32_t, min::vec3, min::sphere, min::aabbox>;
template class min::tree<float, uint16_t, uint32_t, min::vec2, min::aabbox, min::sphere>;
template class min::tree<float, uint16_t, uint32_t, min::vec3, min::aabbox, min::sphere>;
template class min::tree<float, uint16_t, uint32_t, min::vec2, min::sphere, min::sphere>;
template class min::tree<float, uint16_t, uint32_t, min::vec3, min::sphere, min::sphere>;
