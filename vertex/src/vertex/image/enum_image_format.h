#pragma once

namespace vx {
namespace img {

enum class image_format
{
    UNKNOWN = 0,

    R8 = 1,
    RG8 = 2,
    RGB8 = 3,
    RGBA8 = 4,

    R32F = 5,
    RG32F = 6,
    RGB32F = 7,
    RGBA32F = 8
};

}
}