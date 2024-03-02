#pragma once

#include <vector>

#include "detail/base_types.h"

namespace vx {
namespace img {

class image;

std::vector<image> generate_mipmap(const image& img, size_type depth);
std::vector<image> generate_mipmap(const image& img);

}
}