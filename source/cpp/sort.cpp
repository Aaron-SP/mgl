#include <functional>
#include <min/sort.h>
template void min::uint_sort<uint32_t, std::function<uint32_t(const uint32_t)>>(std::vector<uint32_t> &, std::vector<uint32_t> &, std::function<uint32_t(const uint32_t)> &&);
