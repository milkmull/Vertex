#pragma once

#define VX_IMAGE_WRITE_IMPLIMENTATION
#ifdef VX_IMAGE_WRITE_IMPLIMENTATION

#include <vector>

#include "image_info.h"

namespace vx {
namespace img {

error_code write_bmp(const char* filename, const image_info& info, const byte_type* data, bool flip_vertically_on_write = false);
error_code write_jpg(const char* filename, const image_info& info, const byte_type* data, int quality = 75, bool flip_vertically_on_write = false);
error_code write_png(const char* filename, const image_info& info, const byte_type* data, bool flip_vertically_on_write = false);
error_code write_tga(const char* filename, const image_info& info, const byte_type* data, bool flip_vertically_on_write = false);

}
}

#endif