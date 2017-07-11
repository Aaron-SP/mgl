#include <min/convert.h>

template min::mesh<float, uint16_t> min::to_mesh<float, uint16_t>(const min::sphere<float, min::vec3> &s);
template min::mesh<float, uint16_t> min::to_mesh<float, uint16_t>(const min::aabbox<float, min::vec3> &b);