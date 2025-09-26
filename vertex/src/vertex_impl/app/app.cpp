#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/hints/hints_internal.hpp"
#include "vertex_impl/app/event/event_internal.hpp"
#include "vertex_impl/app/video/video_internal.hpp"

namespace vx {
namespace app {

///////////////////////////////////////////////////////////////////////////////
// initialization
///////////////////////////////////////////////////////////////////////////////

static owner_ptr<app_instance> s_app;

////////////////////////////////////////

VX_API bool init()
{
    if (is_init())
    {
        return true;
    }

    s_app_ptr.reset(new app_instance);

    if (!s_app_ptr)
    {
        err::set(err::OUT_OF_MEMORY, "Failed to create application instance");
        return false;
    }

    if (!s_app_ptr->init())
    {
        quit();
        return false;
    }

    return true;
}

////////////////////////////////////////

VX_API bool is_init()
{
    return s_app_ptr != nullptr;
}

////////////////////////////////////////

VX_API void quit()
{
    if (is_init())
    {
        s_app_ptr->quit();
        s_app_ptr.reset();
    }
}

////////////////////////////////////////

VX_API init_flag init_subsystem(init_flag flags)
{
    if (!is_init() && !init())
    {
        return init_flag::NONE;
    }

    return s_app_ptr->init_subsystem(flags);
}

////////////////////////////////////////

VX_API bool is_subsystem_init(init_flag flags)
{
    VX_CHECK_APP_INIT(false);
    return s_app_ptr->is_subsystem_init(flags);
}

////////////////////////////////////////

VX_API void quit_subsystem(init_flag flags)
{
    VX_CHECK_APP_INIT_VOID();
    s_app_ptr->quit_subsystem(flags);
}

////////////////////////////////////////

bool app_instance::init()
{
    if (!init_hints())
    {
        quit();
        return false;
    }

    return true;
}

////////////////////////////////////////

void app_instance::quit()
{
    if (data.flags & INIT_VIDEO)
    {
        quit_video();
    }

    if (data.flags & INIT_EVENTS)
    {
        quit_events();
    }

    quit_hints();
}

////////////////////////////////////////

#define add_flag(dst, flag) dst = static_cast<init_flag>(dst | flag)
#define remove_flag(dst, flag) dst = static_cast<init_flag>(dst & ~flag)

init_flag app_instance::init_subsystem(init_flag flags)
{
    init_flag initialized = NONE;

    if (flags & INIT_EVENTS)
    {
        if (init_events())
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

bool app_instance::is_subsystem_init(init_flag flags) const
{
    return (data.flags & flags) == flags;
}

////////////////////////////////////////

void app_instance::quit_subsystem(init_flag flags)
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

bool app_instance::init_hints()
{
    data.hints_ptr.reset(new hint::hints_instance);
    if (!data.hints_ptr)
    {
        VX_SET_HINTS_SUBSYSTEM_INIT_FAILED_ERROR();
        return false;
    }

    if (!data.hints_ptr->init(this))
    {
        quit_hints();
        return false;
    }

    return true;
}

bool app_instance::is_hints_init() const
{
    return data.hints_ptr != nullptr;
}

void app_instance::quit_hints()
{
    if (data.hints_ptr)
    {
        data.hints_ptr->quit();
        data.hints_ptr.reset();
    }
}

////////////////////////////////////////

bool app_instance::init_events()
{
    data.events_ptr.reset(new event::events_instance);
    if (!data.events_ptr)
    {
        VX_SET_EVENTS_SUBSYSTEM_INIT_FAILED_ERROR();
        return false;
    }

    if (!data.events_ptr->init(this))
    {
        quit_events();
        return false;
    }

    add_flag(data.flags, INIT_EVENTS);
    ++data.events_ref_count;
    return true;
}

bool app_instance::is_events_init() const
{
    return data.events_ptr != nullptr;
}

void app_instance::quit_events()
{
    if (data.hints_ptr && (--data.events_ref_count == 0))
    {
        data.hints_ptr->quit();
        data.hints_ptr.reset();
        remove_flag(data.flags, INIT_EVENTS);
    }
}

////////////////////////////////////////

bool app_instance::init_video()
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

bool app_instance::is_video_init() const
{
    return false;
}

void app_instance::quit_video()
{
    video::video_internal::quit();
    remove_flag(data.flags, INIT_VIDEO);
    quit_events();
}

} // namespace app
} // namespace vx