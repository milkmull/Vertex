#pragma once

#include <string>

#include "vertex/image/image.hpp"

namespace vx {
namespace img {

bool write_bmp(const std::string& filename, const image& img);
bool write_jpg(const std::string& filename, const image& img, int quality = 75);
bool write_png(const std::string& filename, const image& img);

} // namespace img
} // namespace vx