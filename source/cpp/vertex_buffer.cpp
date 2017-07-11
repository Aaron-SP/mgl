#include <min/skeletal_vertex.h>
#include <min/static_vertex.h>
#include <min/vertex_buffer.h>
template class min::vertex_buffer<float, uint16_t, min::skeletal_vertex, GL_FLOAT, GL_UNSIGNED_SHORT>;
template class min::vertex_buffer<float, uint16_t, min::static_vertex, GL_FLOAT, GL_UNSIGNED_SHORT>;