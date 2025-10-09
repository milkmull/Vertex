#pragma once

#include "vertex/config/language_config.hpp"

namespace vx {
namespace app {

enum init_flag
{
    NONE                    = 0,

    INIT_EVENTS             = (1 << 0),
    INIT_VIDEO              = (1 << 1),
    INIT_AUDIO              = (1 << 2),
    INIT_CAMERA             = (1 << 3),

    INIT_EVERYTHING         = INIT_EVENTS | INIT_VIDEO | INIT_AUDIO | INIT_CAMERA,
    INIT_REQUIRES_EVENTS    = INIT_EVENTS | INIT_VIDEO | INIT_AUDIO | INIT_CAMERA
};

VX_API bool init();
VX_API bool is_init();
VX_API void quit();

VX_API init_flag init_subsystem(init_flag flags);
VX_API bool is_subsystem_init(init_flag flags);
VX_API void quit_subsystem(init_flag flags);

struct app_metadata
{
    const char* name = nullptr;
    const char* version = nullptr;
    const char* id = nullptr;
    const char* creator = nullptr;
    const char* url = nullptr;
};

VX_API bool get_metadata(app_metadata& metadata);
VX_API void set_metadata(const app_metadata& metadata);

} // namespace app
} // namespace vx