#pragma once

namespace vx {
namespace img {

enum class error_code
{
    NONE = 0,
    ERROR,
    NULL_POINTER,

    FILE_IO,
    OUT_OF_MEMORY,

    MAX_SIZE,

    UNSUPPORTED_FORMAT,
    UNSUPPORTED_CONVERSION,
    UNSUPPORTED_IMAGE_TYPE,

    UNSUPPORTED_CHANNEL_COUNT,
    UNSUPPORTED_BITDEPTH,

    BAD_JPEG,
    BAD_PNG,
    BAD_BMP,
    BAD_TGA,
    BAD_PSD,
    BAD_PIC,
    BAD_GIF,
    BAD_HDR,
    BAD_PNM
};

}
}