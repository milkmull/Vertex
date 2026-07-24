#include "vertex_impl/os/_platform/platform_io.hpp"

namespace vx {
namespace os {

size_t write_raw(stream s, const char* data, size_t size)
{
    return write_raw_impl(s, data, size);
}

size_t read_raw(stream s, char* data, size_t size)
{
    return read_raw_impl(s, data, size);
}

} // namespace os
} // namespace vx
