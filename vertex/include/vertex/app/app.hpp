#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/config/flags.hpp"

namespace vx {
namespace app {

VX_FLAGS_DECLARE_BEGIN(init_flags)
{
    none                = 0,

    events              = VX_BIT(0),
    video               = VX_BIT(1),
    audio               = VX_BIT(2),
    camera              = VX_BIT(3),

    everything         = events | video | audio | camera,
    requires_events    = events | video | audio | camera
}
VX_FLAGS_DECLARE_END(init_flags)

VX_API bool init();
VX_API bool is_init();
VX_API void quit();

VX_API init_flags init_subsystem(init_flags flags);
VX_API bool is_subsystem_init(init_flags flags);
VX_API void quit_subsystem(init_flags flags);

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