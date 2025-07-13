#pragma once

#include "vertex/image/defs.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace img {

struct image_info
{
    size_t width, height;
    pixel::pixel_format format;
};

///////////////////////////////////////////////////////////////////////////////
// error handling
///////////////////////////////////////////////////////////////////////////////

enum class error_code : int
{
    NONE = 0,
    UNSUPPORTED_PIXEL_FORMAT,
    MAX_SIZE,
    UNSUPPORTED_CHANNEL_COUNT,
    UNSUPPORTED_BITDEPTH
};

inline const char* error_code_to_string(error_code code)
{
    switch (code)
    {
        case error_code::NONE:                          return "none";
        case error_code::UNSUPPORTED_PIXEL_FORMAT:      return "unknown pixel format";
        case error_code::MAX_SIZE:                      return "max image size";
        case error_code::UNSUPPORTED_CHANNEL_COUNT:     return "unsupported image channel count";
        case error_code::UNSUPPORTED_BITDEPTH:          return "unsupported image bit depth";

        default:                                        break;
    }

    return nullptr;
}

inline error_code get_image_info_error(const image_info& info) noexcept
{
    if (info.format == pixel::pixel_format::UNKNOWN)
    {
        return error_code::UNSUPPORTED_PIXEL_FORMAT;
    }

    if (info.width > IMAGE_SIZE_LIMIT_MAX_DIMENSIONS || info.height > IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)
    {
        return error_code::MAX_SIZE;
    }

    return error_code::NONE;
}

} // namespace img
} // namespace vx