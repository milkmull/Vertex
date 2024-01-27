#include "../image_load.h"

#include "vertex/detail/setup.h"

#ifdef VX_IMAGE_LOAD_IMPLIMENTATION

// =============== stb image ===============

#define STB_IMAGE_IMPLEMENTATION

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_ONLY_TGA

#define STBI_MAX_DIMENSIONS VX_MAX_IMAGE_DIMENSIONS

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
    info.width = info.height = 0;
    info.format = image_format::UNKNOWN;
    data.clear();

    if (path == nullptr)
    {
        return error_code::NULL_POINTER;
    }

    int width, height, channels;

    stbi_set_flip_vertically_on_load(flip_vertically_on_load);
    stbi_uc* raw = stbi_load(path, &width, &height, &channels, 0);

    // If stb image fails to load the file, grab the failure reason and return.
    if (raw == nullptr)
    {
        const int err = stbi_failure_reason();
        return stb_image_error_to_vx_error(err);
    }

    info.width = static_cast<size_type>(width);
    info.height = static_cast<size_type>(height);
    info.format = choose_format(static_cast<size_type>(channels));

    const error_code err = info.get_error();

    // Ideally we should never fail here. If for some reason we get an unsupported
    // number of channels from stb_image, the info.format will be unknown. If that
    // happens, the size of the image will be set but the data will be empty.
    if (err == error_code::NONE)
    {
        data.assign(raw, raw + info.size());
    }

    stbi_image_free(raw);

    return err;
}

}
}

#endif