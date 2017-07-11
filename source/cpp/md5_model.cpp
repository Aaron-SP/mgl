#include <min/aabbox.h>
#include <min/md5_model.h>
#include <min/oobbox.h>
#include <min/sphere.h>
#include <min/vec4.h>
template class min::md5_model<float, uint32_t, min::vec4, min::aabbox>;
template class min::md5_model<float, uint32_t, min::vec4, min::oobbox>;
template class min::md5_model<float, uint32_t, min::vec4, min::sphere>;