#include <min/convert.h>

// Finalize mesh function
template void min::finalize_mesh<float, uint16_t>(min::mesh<float, uint16_t> &m);

// Append mesh functions
template void min::append_mesh<float, uint16_t>(const min::sphere<float, min::vec3> &s, min::mesh<float, uint16_t> &m);
template void min::append_mesh<float, uint16_t>(const min::aabbox<float, min::vec3> &b, min::mesh<float, uint16_t> &m);

// To mesh functions
template min::mesh<float, uint16_t> min::to_mesh<float, uint16_t>(const min::sphere<float, min::vec3> &s);
template min::mesh<float, uint16_t> min::to_mesh<float, uint16_t>(const min::aabbox<float, min::vec3> &b);
