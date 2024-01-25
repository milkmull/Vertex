#include "../image_write.h"

#include "vertex/detail/setup.h"
#include "../image_size_limit.h"

#ifdef VX_IMAGE_WRITE_IMPLIMENTATION

// =============== stb image ===============

#define STB_IMAGE_WRITE_IMPLEMENTATION

#define STBI_MAX_DIMENSIONS VX_MAX_IMAGE_SIZE

VX_DISABLE_WARNING_PUSH()
VX_DISABLE_WARNING("-Wimplicit-fallthrough", 26819)
#include "stb_image/stb_image_write.h"
VX_DISABLE_WARNING_POP()

// =========================================

namespace vx {
namespace img {

// =============== error ===============

static const char* error_message = nullptr;

static void set_image_write_error_message(const char* msg)
{
    error_message = msg;
}

const char* get_image_write_error_message()
{
    return error_message;
}

// =============== write ===============

bool write_bmp(const char* path, const image_info& info, const byte_type* data, bool flip_vertically_on_write)
{
    assert(path != nullptr);
    assert(data != nullptr);

	stbi_flip_vertically_on_write(flip_vertically_on_write);
	const bool success = stbi_write_bmp(
		path,
		static_cast<int>(info.width),
        static_cast<int>(info.height),
        static_cast<int>(info.channels()),
		data
	);

	if (!success)
	{
        set_image_write_error_message("failed to write bmp");
	}

    return success;
}

bool write_jpg(const char* path, const image_info& info, const byte_type* data, int quality, bool flip_vertically_on_write)
{
    assert(path != nullptr);
    assert(data != nullptr);
    assert(1 <= quality && quality <= 100);

	stbi_flip_vertically_on_write(flip_vertically_on_write);
	const bool success = stbi_write_jpg(
		path,
        static_cast<int>(info.width),
        static_cast<int>(info.height),
        static_cast<int>(info.channels()),
		data,
        quality
	);

    if (!success)
    {
        set_image_write_error_message("failed to write jpg");
    }

    return success;
}

bool write_png(const char* path, const image_info& info, const byte_type* data, bool flip_vertically_on_write)
{
    assert(path != nullptr);
    assert(data != nullptr);

	stbi_flip_vertically_on_write(flip_vertically_on_write);
	bool success = stbi_write_png(
		path,
        static_cast<int>(info.width),
        static_cast<int>(info.height),
        static_cast<int>(info.channels()),
		data,
		static_cast<int>(info.width * info.pixel_size())
	);

    if (!success)
    {
        set_image_write_error_message("failed to write png");
    }

    return success;
}

bool write_tga(const char* path, const image_info& info, const byte_type* data, bool flip_vertically_on_write)
{
    assert(path != nullptr);
    assert(data != nullptr);

	stbi_flip_vertically_on_write(flip_vertically_on_write);
	bool success = stbi_write_tga(
		path,
        static_cast<int>(info.width),
        static_cast<int>(info.height),
        static_cast<int>(info.channels()),
		data
	);

    if (!success)
    {
        set_image_write_error_message("failed to write tga");
    }

    return success;
}

}
}

#endif