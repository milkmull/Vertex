#pragma once

#define VX_IMAGE_LOAD_IMPLIMENTATION
#ifdef VX_IMAGE_LOAD_IMPLIMENTATION

#include <vector>

#include "../image_info.h"
#include "../enum_error_code.h"

namespace vx {
namespace img {
namespace raw {

error_code get_file_info(const char* filename, image_info& info);

error_code load_image(const char* filename, image_info& info, std::vector<byte_type>& data, bool flip_vertically_on_load = false);
error_code load_image(const char* filename, image_info& info, image_format target_format, std::vector<byte_type>& data, bool flip_vertically_on_load = false);

}
}
}

#endif