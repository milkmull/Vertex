#include "vertex/detail/setup.h"

#include "../image_load.h"
#include "../image_size_limit.h"

#ifdef VX_IMAGE_LOAD_IMPLIMENTATION

// =============== stb image ===============

#define STB_IMAGE_IMPLEMENTATION

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_ONLY_TGA

//#define STBI_MAX_DIMENSIONS VX_MAX_IMAGE_DIMENSIONS

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

static error_code get_info(const char* filename, image_info& info)
{
    assert(filename != nullptr);

    info.width = info.height = 0;
    info.format = image_format::UNKNOWN;

    int width, height, channels;
    const int ok = stbi_info(filename, &width, &height, &channels);

    if (ok == 0)
    {
        const int err = stbi_failure_reason();
        return stb_image_error_to_vx_error(err);
    }

    info = image_info
    {
        static_cast<size_type>(width),
        static_cast<size_type>(height),
        choose_format(static_cast<size_type>(channels))
    };

    return error_code::NONE;
}

static error_code load_image_internal(
    const char* filename,
    image_info& info,
    image_format target_format,
    std::vector<byte_type>& data,
    bool flip_vertically_on_load
)
{
    assert(filename != nullptr);

    data.clear();

    error_code err = get_info(filename, info);
    if (err != error_code::NONE)
    {
        return err;
    }

    if (target_format != image_format::UNKNOWN && !reinterpret_info(info, target_format))
    {
        return error_code::UNSUPPORTED_CONVERSION;
    }

    err = get_image_info_error(info);
    if (err != error_code::NONE)
    {
        return err;
    }

    int width, height, channels;
    stbi_set_flip_vertically_on_load(flip_vertically_on_load);
    stbi_uc* raw = stbi_load(filename, &width, &height, &channels, 0);

    // If stb image fails to load the file, grab the failure reason and return.
    if (raw == nullptr)
    {
        const int stb_err = stbi_failure_reason();
        return stb_image_error_to_vx_error(stb_err);
    }

    data.assign(raw, raw + info.size());
    stbi_image_free(raw);

    return err;
}

error_code load_image(const char* filename, image_info& info, std::vector<byte_type>& data, bool flip_vertically_on_load)
{
    return load_image_internal(filename, info, image_format::UNKNOWN, data, flip_vertically_on_load);
}

error_code load_image(const char* filename, image_info& info, image_format target_format, std::vector<byte_type>& data, bool flip_vertically_on_load)
{
    return load_image_internal(filename, info, target_format, data, flip_vertically_on_load);
}

}
}

#endif