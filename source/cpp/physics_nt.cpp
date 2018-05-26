#include <min/aabbox.h>
#include <min/grid.h>
#include <min/oobbox.h>
#include <min/physics_nt.h>
#include <min/sphere.h>
#include <min/vec2.h>
#include <min/vec3.h>
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec2, min::aabbox, min::aabbox, min::grid>;
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec3, min::aabbox, min::aabbox, min::grid>;
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec2, min::aabbox, min::oobbox, min::grid>;
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec3, min::aabbox, min::oobbox, min::grid>;
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec2, min::aabbox, min::sphere, min::grid>;
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec3, min::aabbox, min::sphere, min::grid>;
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec2, min::oobbox, min::aabbox, min::grid>;
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec3, min::oobbox, min::aabbox, min::grid>;
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec2, min::oobbox, min::oobbox, min::grid>;
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec3, min::oobbox, min::oobbox, min::grid>;
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec2, min::oobbox, min::sphere, min::grid>;
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec3, min::oobbox, min::sphere, min::grid>;
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec2, min::sphere, min::aabbox, min::grid>;
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec3, min::sphere, min::aabbox, min::grid>;
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec2, min::sphere, min::oobbox, min::grid>;
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec3, min::sphere, min::oobbox, min::grid>;
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec2, min::sphere, min::sphere, min::grid>;
template class min::physics<double, uint_fast16_t, uint_fast32_t, min::vec3, min::sphere, min::sphere, min::grid>;
