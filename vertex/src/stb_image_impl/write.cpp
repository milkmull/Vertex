#include "vertex/image/raw/write.h"
#include "vertex/image/util/image_info_helpers.h"
#include "vertex/system/error.h"

#ifdef VX_IMAGE_WRITE_IMPLIMENTATION

///////////////////////////////////////////////////////////////////////////////
// stb_image
///////////////////////////////////////////////////////////////////////////////

#define STB_IMAGE_WRITE_IMPLEMENTATION

//#define STBI_MAX_DIMENSIONS VX_MAX_IMAGE_DIMENSIONS

VX_DISABLE_WARNING_PUSH()
VX_DISABLE_WARNING("-Wimplicit-fallthrough", 26819)
#include "stb_image/stb_image_write.h"
VX_DISABLE_WARNING_POP()

///////////////////////////////////////////////////////////////////////////////

namespace vx {
namespace img {
namespace raw {

///////////////////////////////////////////////////////////////////////////////
// write
///////////////////////////////////////////////////////////////////////////////

#define IMAGE_WRITE_ERROR(filename) VX_ERROR(error::error_code::FILE_ERROR) << "Failed to write image file " << filename
#define IMAGE_FORMAT_ERROR(err) VX_ERROR(error::error_code::UNSUPPORTED_FORMAT) << util::image_error_code_to_string(err)

bool write_bmp(const char* filename, const image_info& info, const byte_type* data, bool flip_vertically_on_write)
{
    const util::image_error_code err = util::get_image_info_error(info);
    if (err != util::image_error_code::NONE)
    {
        IMAGE_FORMAT_ERROR(err);
        return false;
    }

    const image_info info_8_bit = util::get_8_bit_info(info);

    stbi_flip_vertically_on_write(flip_vertically_on_write);
    const bool success = stbi_write_bmp(
        filename,
        static_cast<int>(info_8_bit.width),
        static_cast<int>(info_8_bit.height),
        static_cast<int>(info_8_bit.channels()),
        data
    );

    if (!success)
    {
        IMAGE_WRITE_ERROR(filename);
    }

    return success;
}

bool write_jpg(const char* filename, const image_info& info, const byte_type* data, int quality, bool flip_vertically_on_write)
{
    const util::image_error_code err = util::get_image_info_error(info);
    if (err != util::image_error_code::NONE)
    {
        IMAGE_FORMAT_ERROR(err);
        return false;
    }

    const image_info info_8_bit = util::get_8_bit_info(info);

    stbi_flip_vertically_on_write(flip_vertically_on_write);
    const bool success = stbi_write_jpg(
        filename,
        static_cast<int>(info_8_bit.width),
        static_cast<int>(info_8_bit.height),
        static_cast<int>(info_8_bit.channels()),
        data,
        quality
    );

    if (!success)
    {
        IMAGE_WRITE_ERROR(filename);
    }

    return success;
}

bool write_png(const char* filename, const image_info& info, const byte_type* data, bool flip_vertically_on_write)
{
    const util::image_error_code err = util::get_image_info_error(info);
    if (err != util::image_error_code::NONE)
    {
        IMAGE_FORMAT_ERROR(err);
        return false;
    }

    const image_info info_8_bit = util::get_8_bit_info(info);

    stbi_flip_vertically_on_write(flip_vertically_on_write);
    bool success = stbi_write_png(
        filename,
        static_cast<int>(info_8_bit.width),
        static_cast<int>(info_8_bit.height),
        static_cast<int>(info_8_bit.channels()),
        data,
        static_cast<int>(info_8_bit.stride())
    );

    if (!success)
    {
        IMAGE_WRITE_ERROR(filename);
    }

    return success;
}

bool write_tga(const char* filename, const image_info& info, const byte_type* data, bool flip_vertically_on_write)
{
    const util::image_error_code err = util::get_image_info_error(info);
    if (err != util::image_error_code::NONE)
    {
        IMAGE_FORMAT_ERROR(err);
        return false;
    }

    const image_info info_8_bit = util::get_8_bit_info(info);

    stbi_flip_vertically_on_write(flip_vertically_on_write);
    bool success = stbi_write_tga(
        filename,
        static_cast<int>(info_8_bit.width),
        static_cast<int>(info_8_bit.height),
        static_cast<int>(info_8_bit.channels()),
        data
    );

    if (!success)
    {
        IMAGE_WRITE_ERROR(filename);
    }

    return success;
}

}
}
}

#endif