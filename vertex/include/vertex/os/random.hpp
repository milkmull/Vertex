#pragma once

#include <cstdint>

#include "vertex/config/language_config.hpp"

namespace vx {
namespace os {

VX_API bool get_entropy(uint8_t* bytes, size_t count);

} // namespace os
} // namespace vx