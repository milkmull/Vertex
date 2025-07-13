#pragma once

#include <string>

#include "vertex/image/image.hpp"

namespace vx {
namespace img {

bool load(const std::string& filename, image& img);

} // namespace img
} // namespace vx