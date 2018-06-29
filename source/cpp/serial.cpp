#include <min/serial.h>

template float min::read_le<float>(const std::vector<uint8_t> &stream, size_t &next);
template float min::read_be<float>(const std::vector<uint8_t> &stream, size_t &next);
template void min::write_le<float>(std::vector<uint8_t> &stream, const float data);
template void min::write_be<float>(std::vector<uint8_t> &stream, const float data);
template void min::write_le<float>(std::vector<uint8_t> &stream, const float data, const size_t offset);
template void min::write_be<float>(std::vector<uint8_t> &stream, const float data, const size_t offset);
template std::vector<float> min::read_le_vector<float>(const std::vector<uint8_t> &stream, size_t &next);
template std::vector<float> min::read_be_vector<float>(const std::vector<uint8_t> &stream, size_t &next);
template void min::write_le_vector<float>(std::vector<uint8_t> &stream, const std::vector<float> &data);
template void min::write_be_vector<float>(std::vector<uint8_t> &stream, const std::vector<float> &data);
template min::vec2<float> min::read_le_vec2<float>(const std::vector<uint8_t> &stream, size_t &next);
template min::vec2<float> min::read_be_vec2<float>(const std::vector<uint8_t> &stream, size_t &next);
template min::vec3<float> min::read_le_vec3<float>(const std::vector<uint8_t> &stream, size_t &next);
template min::vec3<float> min::read_be_vec3<float>(const std::vector<uint8_t> &stream, size_t &next);
template min::vec4<float> min::read_le_vec4<float>(const std::vector<uint8_t> &stream, size_t &next);
template min::vec4<float> min::read_be_vec4<float>(const std::vector<uint8_t> &stream, size_t &next);
template void min::write_le_vec2<float>(std::vector<uint8_t> &stream, const min::vec2<float> &v);
template void min::write_be_vec2<float>(std::vector<uint8_t> &stream, const min::vec2<float> &v);
template void min::write_le_vec3<float>(std::vector<uint8_t> &stream, const min::vec3<float> &v);
template void min::write_be_vec3<float>(std::vector<uint8_t> &stream, const min::vec3<float> &v);
template void min::write_le_vec4<float>(std::vector<uint8_t> &stream, const min::vec4<float> &v);
template void min::write_be_vec4<float>(std::vector<uint8_t> &stream, const min::vec4<float> &v);
template std::vector<min::vec2<float>> min::read_le_vector_vec2<float>(const std::vector<uint8_t> &stream, size_t &next);
template std::vector<min::vec2<float>> min::read_be_vector_vec2<float>(const std::vector<uint8_t> &stream, size_t &next);
template std::vector<min::vec3<float>> min::read_le_vector_vec3<float>(const std::vector<uint8_t> &stream, size_t &next);
template std::vector<min::vec3<float>> min::read_be_vector_vec3<float>(const std::vector<uint8_t> &stream, size_t &next);
template std::vector<min::vec4<float>> min::read_le_vector_vec4<float>(const std::vector<uint8_t> &stream, size_t &next);
template std::vector<min::vec4<float>> min::read_be_vector_vec4<float>(const std::vector<uint8_t> &stream, size_t &next);
template void min::write_le_vector_vec2<float>(std::vector<uint8_t> &stream, const std::vector<vec2<float>> &data);
template void min::write_be_vector_vec2<float>(std::vector<uint8_t> &stream, const std::vector<vec2<float>> &data);
template void min::write_le_vector_vec3<float>(std::vector<uint8_t> &stream, const std::vector<vec3<float>> &data);
template void min::write_be_vector_vec3<float>(std::vector<uint8_t> &stream, const std::vector<vec3<float>> &data);
template void min::write_le_vector_vec4<float>(std::vector<uint8_t> &stream, const std::vector<vec4<float>> &data);
template void min::write_be_vector_vec4<float>(std::vector<uint8_t> &stream, const std::vector<vec4<float>> &data);