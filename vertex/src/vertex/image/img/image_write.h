#pragma once

#define VX_IMAGE_WRITE_IMPLIMENTATION

#ifdef VX_IMAGE_WRITE_IMPLIMENTATION

#include <vector>

#include "image_info.h"

namespace vx {
namespace img {

const char* get_image_write_error_message();

bool save_bmp(const char* path, const image_info& info, const byte_type* data, bool flip_vertically_on_write = false);
bool save_jpg(const char* path, const image_info& info, const byte_type* data, int quality = 75, bool flip_vertically_on_write = false);
bool save_png(const char* path, const image_info& info, const byte_type* data, bool flip_vertically_on_write = false);
bool save_tga(const char* path, const image_info& info, const byte_type* data, bool flip_vertically_on_write = false);

}
}

#endif