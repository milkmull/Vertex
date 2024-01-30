#include <algorithm>

#include "../image_write.h"
#include "vertex/detail/setup.h"
#include "../image_size_limit.h"

#ifdef VX_IMAGE_WRITE_IMPLIMENTATION

// =============== stb image ===============

#define STB_IMAGE_WRITE_IMPLEMENTATION

//#define STBI_MAX_DIMENSIONS VX_MAX_IMAGE_DIMENSIONS

VX_DISABLE_WARNING_PUSH()
VX_DISABLE_WARNING("-Wimplicit-fallthrough", 26819)
#include "stb_image/stb_image_write.h"
VX_DISABLE_WARNING_POP()

// =========================================

namespace vx {
namespace img {

// =============== write ===============

error_code write_bmp(const char* path, const image_info& info, const byte_type* data, bool flip_vertically_on_write)
{
    if (path == nullptr || data == nullptr)
    {
        return error_code::NULL_POINTER;
    }

    const error_code info_error = get_image_info_error(info);
    if (info_error != error_code::NONE)
    {
        return info_error;
    }

    const image_info info_8_bit = get_8_bit_info(info);

	stbi_flip_vertically_on_write(flip_vertically_on_write);
	const bool success = stbi_write_bmp(
		path,
		static_cast<int>(info_8_bit.width),
        static_cast<int>(info_8_bit.height),
        static_cast<int>(info_8_bit.channels()),
		data
	);

    return success ? error_code::NONE : error_code::FILE_IO;
}

error_code write_jpg(const char* path, const image_info& info, const byte_type* data, int quality, bool flip_vertically_on_write)
{
    if (path == nullptr || data == nullptr)
    {
        return error_code::NULL_POINTER;
    }

    const error_code info_error = get_image_info_error(info);
    if (info_error != error_code::NONE)
    {
        return info_error;
    }

    const image_info info_8_bit = get_8_bit_info(info);

	stbi_flip_vertically_on_write(flip_vertically_on_write);
	const bool success = stbi_write_jpg(
		path,
        static_cast<int>(info_8_bit.width),
        static_cast<int>(info_8_bit.height),
        static_cast<int>(info_8_bit.channels()),
		data,
        std::clamp(quality, 1, 100)
	);

    return success ? error_code::NONE : error_code::FILE_IO;
}

error_code write_png(const char* path, const image_info& info, const byte_type* data, bool flip_vertically_on_write)
{
    if (path == nullptr || data == nullptr)
    {
        return error_code::NULL_POINTER;
    }

    const error_code info_error = get_image_info_error(info);
    if (info_error != error_code::NONE)
    {
        return info_error;
    }

    const image_info info_8_bit = get_8_bit_info(info);

	stbi_flip_vertically_on_write(flip_vertically_on_write);
	bool success = stbi_write_png(
		path,
        static_cast<int>(info_8_bit.width),
        static_cast<int>(info_8_bit.height),
        static_cast<int>(info_8_bit.channels()),
		data,
		static_cast<int>(info_8_bit.stride())
	);

    return success ? error_code::NONE : error_code::FILE_IO;
}

error_code write_tga(const char* path, const image_info& info, const byte_type* data, bool flip_vertically_on_write)
{
    if (path == nullptr || data == nullptr)
    {
        return error_code::NULL_POINTER;
    }

    const error_code info_error = get_image_info_error(info);
    if (info_error != error_code::NONE)
    {
        return info_error;
    }

    const image_info info_8_bit = get_8_bit_info(info);

	stbi_flip_vertically_on_write(flip_vertically_on_write);
	bool success = stbi_write_tga(
		path,
        static_cast<int>(info_8_bit.width),
        static_cast<int>(info_8_bit.height),
        static_cast<int>(info_8_bit.channels()),
		data
	);

    return success ? error_code::NONE : error_code::FILE_IO;
}

}
}

#endif