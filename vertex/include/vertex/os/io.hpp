#pragma once

namespace vx {
namespace os {

enum class stream
{
    in,
    out,
    err
};

VX_API size_t write_raw(stream s, const char* data, size_t size);
VX_API size_t read_raw(stream s, char* data, size_t size);

} // namespace os
} // namespace vx
