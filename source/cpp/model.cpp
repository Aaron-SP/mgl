#include <min/aabbox.h>
#include <min/model.h>
#include <min/oobbox.h>
#include <min/sphere.h>
#include <min/vec4.h>
template class min::model<float, uint16_t, min::vec4, min::aabbox>;
template class min::model<float, uint16_t, min::vec4, min::oobbox>;
template class min::model<float, uint16_t, min::vec4, min::sphere>;