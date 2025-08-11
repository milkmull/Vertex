#pragma once

#include <cstdint>
#include <cstddef>

namespace vx {
namespace os {

bool get_entropy_impl(uint8_t* bytes, size_t count);

} // namespace os
} // namespace vx