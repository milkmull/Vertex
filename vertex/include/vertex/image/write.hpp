#pragma once

#include <string>

#include "vertex/image/image.hpp"

namespace vx {
namespace img {

VX_API bool write_bmp(const std::string& filename, const image& img);
VX_API bool write_jpg(const std::string& filename, const image& img, int quality = 75);
VX_API bool write_png(const std::string& filename, const image& img);

} // namespace img
} // namespace vx