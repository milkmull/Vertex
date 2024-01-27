#pragma once

#define VX_IMAGE_LOAD_IMPLIMENTATION
#ifdef VX_IMAGE_LOAD_IMPLIMENTATION

#include <vector>

#include "image_info.h"

namespace vx {
namespace img {

error_code load_image(const char* path, image_info& info, std::vector<byte_type>& data, bool flip_vertically_on_load = false);

}
}

#endif