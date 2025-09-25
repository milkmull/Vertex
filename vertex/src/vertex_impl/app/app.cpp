#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/hints/hints_internal.hpp"
#include "vertex_impl/app/event/event_internal.hpp"
#include "vertex_impl/app/video/video_internal.hpp"

namespace vx {
namespace app {

///////////////////////////////////////////////////////////////////////////////
// initialization
///////////////////////////////////////////////////////////////////////////////

static owner_ptr<app_data> s_app_data_ptr;

#define data (*s_app_data_ptr)

////////////////////////////////////////

#define add_flag(dst, flag) dst = static_cast<init_flag>(dst | flag)
#define remove_flag(dst, flag) dst = static_cast<init_flag>(dst & ~flag)

VX_API init_flag init(init_flag flags)
{
    return app_internal::init(flags);
}

init_flag app_internal::init(init_flag flags)
{
    init_flag initialized = NONE;

    if (!init_app())
    {
        return initialized;
    }

    if (flags & INIT_EVENTS)
    {
        if (init_events(true))
        {
            add_flag(initialized, INIT_EVENTS);
        }
    }

    if (flags & INIT_VIDEO)
    {
        if (!init_video())
        {
            add_flag(initialized, INIT_VIDEO);
        }
    }

    return initialized;
}

////////////////////////////////////////

VX_API bool is_init(init_flag flags)
{
    if (!s_app_data_ptr)
    {
        return false;
    }

    return app_internal::is_init(flags);
}

bool app_internal::is_init(init_flag flags)
{
    return (data.flags & flags) == flags;
}

////////////////////////////////////////

VX_API void quit(init_flag flags)
{
    if (!s_app_data_ptr)
    {
        return;
    }

    app_internal::quit(flags);
}

void app_internal::quit(init_flag flags)
{
    if (flags & INIT_VIDEO)
    {
        quit_video();
    }

    if (flags & INIT_EVENTS)
    {
        quit_events();
    }
}

////////////////////////////////////////

VX_API void shutdown()
{
    app_internal::shutdown();
}

void app_internal::shutdown()
{
    quit(init_flag::INIT_EVERYTHING);
    VX_ASSERT(data.flags == NONE);
    s_app_data_ptr.reset();
}

////////////////////////////////////////

bool app_internal::init_app()
{
    if (s_app_data_ptr)
    {
        return true;
    }

    s_app_data_ptr.reset(new app_data);

    if (!s_app_data_ptr)
    {
        quit_app();
        return false;
    }

    if (!init_hints())
    {
        quit_app();
        return false;
    }

    return true;
}

void app_internal::quit_app()
{
    quit_hints();

    if (s_app_data_ptr)
    {
        s_app_data_ptr.reset();
    }
}

////////////////////////////////////////

bool app_internal::init_hints()
{
    if (!hint::hint_internal::init())
    {
        quit_hints();
        return false;
    }

    return true;
}

void app_internal::quit_hints()
{
    hint::hint_internal::quit();
}

////////////////////////////////////////

bool app_internal::init_events()
{
    if (!event::event_internal::init())
    {
        return false;
    }

    add_flag(data.flags, INIT_EVENTS);
    return true;
}

void app_internal::quit_events()
{
    event::event_internal::quit();
    remove_flag(data.flags, INIT_EVENTS);
}

////////////////////////////////////////

bool app_internal::init_video()
{
    // video subsystem requires events
    if (!init_events())
    {
        return false;
    }

    if (!video::video_internal::init())
    {
        quit_video();
        return false;
    }

    add_flag(data.flags, INIT_VIDEO);
    return true;
}

void app_internal::quit_video()
{
    video::video_internal::quit();
    remove_flag(data.flags, INIT_VIDEO);
    quit_events();
}

} // namespace app
} // namespace vx