#pragma once

namespace vx {
namespace pixel {

enum class wrap_mode
{
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
    REPEAT,
    MIRRORED_REPEAT,
    MIRROR_CLAMP_TO_EDGE
};

}
}