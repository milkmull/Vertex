#pragma once

#include <cstdint>

namespace vx {
namespace os {

// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/rand-s?view=msvc-170
// https://github.com/microsoft/STL/blob/25dc2b72b6f594326253cf3a05543f7a5750f802/stl/src/xrngdev.cpp#L10

bool get_entropy_impl(uint8_t* bytes, size_t count);

} // namespace os
} // namespace vx