#pragma once

#include "vertex/std/io.hpp"

namespace vx {
namespace os {

size_t write_raw_impl(stream s, const char* data, size_t size, bool allow_error);
size_t read_raw_impl(stream s, char* data, size_t size);

} // namespace os
} // namespace vx
