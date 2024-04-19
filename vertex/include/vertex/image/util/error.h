#pragma once

namespace vx {
namespace img {
namespace util {

enum class image_error_code : int
{
    NONE = 0,

    UNSUPPORTED_IMAGE_FORMAT,

    MAX_SIZE,
    UNSUPPORTED_CHANNEL_COUNT,
    UNSUPPORTED_BITDEPTH,

    REINTERPRETATION_ERROR
};

inline constexpr const char* image_error_code_to_string(image_error_code code)
{
    switch (code)
    {
        case image_error_code::NONE:                        return "none";
        case image_error_code::UNSUPPORTED_IMAGE_FORMAT:    return "unknown image format";
        case image_error_code::MAX_SIZE:                    return "max image size";
        case image_error_code::UNSUPPORTED_CHANNEL_COUNT:   return "unsupported image channel count";
        case image_error_code::UNSUPPORTED_BITDEPTH:        return "unsupported image bit depth";
        case image_error_code::REINTERPRETATION_ERROR:      return "failed to interpret image as desired format";

        default:                                            break;
    }

    return nullptr;
}

}
}
}