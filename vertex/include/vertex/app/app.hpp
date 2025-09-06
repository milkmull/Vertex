#pragma once

#include "vertex/config/language_config.hpp"

namespace vx {
namespace app {

enum init_flag
{
    NONE                = 0,

    INIT_VIDEO          = (1 << 0),
    INIT_AUDIO          = (1 << 1),
    INIT_CAMERA         = (1 << 2),

    INIT_EVENTS = INIT_VIDEO | INIT_AUDIO | INIT_CAMERA,
    INIT_EVERYTHING = INIT_VIDEO | INIT_AUDIO | INIT_CAMERA
};

VX_API bool init(init_flag flags);
VX_API bool is_init(init_flag flags);
VX_API void quit(init_flag flags = init_flag::INIT_EVERYTHING);

} // namespace app
} // namespace vx