#pragma once

namespace vx {
namespace img {

enum class image_filter
{
    NEAREST,
    LINEAR,
    CUBIC
};

enum class image_wrap
{
    REPEAT,
    MIRRORED_REPEAT,
    EDGE_CLAMP,
    BORDER_CLAMP,
};

}
}