#include "../image_load.h"

#include "vertex/detail/setup.h"

#ifdef VX_IMAGE_LOAD_IMPLIMENTATION

// =============== stb image ===============

#define STB_IMAGE_IMPLEMENTATION

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_ONLY_TGA

#define STBI_MAX_DIMENSIONS VX_MAX_IMAGE_SIZE

#define STBI_NO_FAILURE_STRINGS

VX_DISABLE_WARNING_PUSH()
VX_DISABLE_WARNING("-Wimplicit-fallthrough", 26819)
VX_DISABLE_WARNING("-Wconversion", 4244)
#include "stb_image/stb_image.h"
VX_DISABLE_WARNING_POP()

// =========================================

namespace vx {
namespace img {

static inline error_code stb_image_error_to_vx_error(int stb_image_error)
{
    switch (stb_image_error)
    {
        case STBI_ERROR_NONE:                       return error_code::NONE;
        case STBI_ERROR_INTERNAL_ERROR:             return error_code::ERROR;
        case STBI_ERROR_FILE_IO:                    return error_code::FILE_IO;
        case STBI_ERROR_OUT_OF_MEMORY:              return error_code::OUT_OF_MEMORY;
        case STBI_ERROR_MAX_SIZE:                   return error_code::MAX_SIZE;
        case STBI_ERROR_UNSUPPORTED_FORMAT:         return error_code::UNSUPPORTED_FORMAT;
        case STBI_ERROR_UNSUPPORTED_CONVERSION:     return error_code::UNSUPPORTED_CONVERSION;
        case STBI_ERROR_UNSUPPORTED_IMAGE_TYPE:     return error_code::UNSUPPORTED_IMAGE_TYPE;
        case STBI_ERROR_BAD_JPEG:                   return error_code::BAD_JPEG;
        case STBI_ERROR_BAD_PNG:                    return error_code::BAD_PNG;
        case STBI_ERROR_BAD_BMP:                    return error_code::BAD_BMP;
        case STBI_ERROR_BAD_TGA:                    return error_code::BAD_TGA;
        case STBI_ERROR_BAD_PSD:                    return error_code::BAD_PSD;
        case STBI_ERROR_BAD_PIC:                    return error_code::BAD_PIC;
        case STBI_ERROR_BAD_GIF:                    return error_code::BAD_GIF;
        case STBI_ERROR_BAD_HDR:                    return error_code::BAD_HDR;
        case STBI_ERROR_BAD_PNM:                    return error_code::BAD_PNM;
        default:                                    break;
    }

    return error_code::NONE;
}

// =============== load ===============

error_code load_image(const char* path, image_info& info, std::vector<byte_type>& data, bool flip_vertically_on_load)
{
    if (path == nullptr)
    {
        return error_code::FILE_IO;
    }

    int width, height, channels;
    data.clear();

    stbi_set_flip_vertically_on_load(flip_vertically_on_load);
    stbi_uc* raw = stbi_load(path, &width, &height, &channels, 0);

    if (raw == nullptr)
    {
        const int err = stbi_failure_reason();
        return stb_image_error_to_vx_error(err);
    }

    error_code err = check_image_size_limits(width, height, channels, 8);
    if (err != error_code::NONE)
    {
        return err;
    }

    info.width = width;
    info.height = height;
    info.format = choose_format(channels);
    data.assign(raw, raw + info.size());

    stbi_image_free(raw);

    return error_code::NONE;
}

}
}

#endif