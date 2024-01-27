#pragma once

namespace vx {
namespace img {

enum class image_error_code
{
    NONE = 0,
    ERROR,

    BAD_PATH,
    FAILED_TO_OPEN_FILE,
    OUT_OF_MEMORY,

    MAX_SIZE,
    UNSUPPORTED_CHANNEL_COUNT,
    UNSUPPORTED_BITDEPTH,

    UNSUPPORTED_FORMAT,
    UNSUPPORTED_FORMAT_CONVERSION,

    UNSUPPORTED_IMAGE_TYPE,

    BAD_JPEG,
    BAD_PNG,
    BAD_BMP,
    BAD_TGA,
    BAD_PSD,
    BAD_PIC,
    BAD_GIF,
    BAD_PNM
};

//
// 
//

}
}