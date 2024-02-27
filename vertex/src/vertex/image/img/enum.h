#pragma once

namespace vx {
namespace img {

enum class image_filter
{
    NEAREST,
    LINEAR
};

enum class image_wrap
{
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
    REPEAT,
    MIRRORED_REPEAT,
    MIRROR_CLAMP_TO_EDGE
};

}
}