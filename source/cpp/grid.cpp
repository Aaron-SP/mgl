#include <min/aabbox.h>
#include <min/grid.h>
#include <min/oobbox.h>
#include <min/sphere.h>
#include <min/vec2.h>
#include <min/vec3.h>
template class min::grid<float, uint16_t, uint32_t, min::vec2, min::aabbox, min::aabbox>;
template class min::grid<float, uint16_t, uint32_t, min::vec3, min::aabbox, min::aabbox>;
template class min::grid<float, uint16_t, uint32_t, min::vec2, min::aabbox, min::oobbox>;
template class min::grid<float, uint16_t, uint32_t, min::vec3, min::aabbox, min::oobbox>;
template class min::grid<float, uint16_t, uint32_t, min::vec2, min::aabbox, min::sphere>;
template class min::grid<float, uint16_t, uint32_t, min::vec3, min::aabbox, min::sphere>;
template class min::grid<float, uint16_t, uint32_t, min::vec2, min::oobbox, min::aabbox>;
template class min::grid<float, uint16_t, uint32_t, min::vec3, min::oobbox, min::aabbox>;
template class min::grid<float, uint16_t, uint32_t, min::vec2, min::oobbox, min::oobbox>;
template class min::grid<float, uint16_t, uint32_t, min::vec3, min::oobbox, min::oobbox>;
template class min::grid<float, uint16_t, uint32_t, min::vec2, min::oobbox, min::sphere>;
template class min::grid<float, uint16_t, uint32_t, min::vec3, min::oobbox, min::sphere>;
template class min::grid<float, uint16_t, uint32_t, min::vec2, min::sphere, min::aabbox>;
template class min::grid<float, uint16_t, uint32_t, min::vec3, min::sphere, min::aabbox>;
template class min::grid<float, uint16_t, uint32_t, min::vec2, min::sphere, min::oobbox>;
template class min::grid<float, uint16_t, uint32_t, min::vec3, min::sphere, min::oobbox>;
template class min::grid<float, uint16_t, uint32_t, min::vec2, min::sphere, min::sphere>;
template class min::grid<float, uint16_t, uint32_t, min::vec3, min::sphere, min::sphere>;
