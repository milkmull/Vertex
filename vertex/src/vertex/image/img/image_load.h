#pragma once

#define VX_IMAGE_LOAD_IMPLIMENTATION
#ifdef VX_IMAGE_LOAD_IMPLIMENTATION

#include <vector>

#include "image_info.h"

namespace vx {
namespace img {

const char* get_image_load_error_message();

bool load_image(const char* path, image_info& specs, std::vector<byte_type>& data, bool flip_vertically_on_load = false);

}
}

#endif