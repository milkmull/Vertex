#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/app/owner_ptr.hpp"

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

//VX_API init_flag init(init_flag flags);
//VX_API bool is_init(init_flag flags);
//VX_API void quit(init_flag flags = init_flag::INIT_EVERYTHING);
//VX_API void shutdown();

class application_impl;

class application
{
public:

    VX_API init_flag init(init_flag flags);
    VX_API bool is_init(init_flag flags);
    VX_API void quit(init_flag flags = init_flag::INIT_EVERYTHING);
    VX_API void shutdown();

private:

    owner_ptr<application_impl> m_impl;
};

} // namespace app
} // namespace vx