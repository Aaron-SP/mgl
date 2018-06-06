#include <min/serial_mem.h>

template float min::read_le<float>(const mem_file &stream, size_t &next);
template float min::read_be<float>(const mem_file &stream, size_t &next);
template std::vector<float> min::read_le_vector<float>(const mem_file &stream, size_t &next);
template std::vector<float> min::read_be_vector<float>(const mem_file &stream, size_t &next);
template min::vec2<float> min::read_le_vec2<float>(const mem_file &stream, size_t &next);
template min::vec2<float> min::read_be_vec2<float>(const mem_file &stream, size_t &next);
template min::vec3<float> min::read_le_vec3<float>(const mem_file &stream, size_t &next);
template min::vec3<float> min::read_be_vec3<float>(const mem_file &stream, size_t &next);
template min::vec4<float> min::read_le_vec4<float>(const mem_file &stream, size_t &next);
template min::vec4<float> min::read_be_vec4<float>(const mem_file &stream, size_t &next);
template std::vector<min::vec2<float>> min::read_le_vector_vec2<float>(const mem_file &stream, size_t &next);
template std::vector<min::vec2<float>> min::read_be_vector_vec2<float>(const mem_file &stream, size_t &next);
template std::vector<min::vec3<float>> min::read_le_vector_vec3<float>(const mem_file &stream, size_t &next);
template std::vector<min::vec3<float>> min::read_be_vector_vec3<float>(const mem_file &stream, size_t &next);
template std::vector<min::vec4<float>> min::read_le_vector_vec4<float>(const mem_file &stream, size_t &next);
template std::vector<min::vec4<float>> min::read_be_vector_vec4<float>(const mem_file &stream, size_t &next);
