#include "vertex/config/version.hpp"
#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/hints/hints_internal.hpp"
#include "vertex_impl/app/event/event_internal.hpp"
#include "vertex_impl/app/video/video_internal.hpp"

namespace vx {
namespace app {

///////////////////////////////////////////////////////////////////////////////
// initialization
///////////////////////////////////////////////////////////////////////////////

owner_ptr<app_instance> s_app;

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

app_instance::~app_instance()
{
    quit();
}

////////////////////////////////////////

bool app_instance::init(const app_metadata& metadata)
{
    if (!init_hints())
    {
        quit();
        return false;
    }

    set_metadata(metadata);
    return true;
}

////////////////////////////////////////

void app_instance::quit()
{
    quit_video();
    quit_events();
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
        if (init_video())
        {
            add_flag(initialized, INIT_VIDEO);
        }
    }

    return initialized;
}

////////////////////////////////////////

bool app_instance::is_subsystem_init(init_flag flags) const
{
    init_flag initialized = NONE;

    if (is_events_init())
    {
        add_flag(initialized, INIT_EVENTS);
    }

    if (is_video_init())
    {
        add_flag(initialized, INIT_VIDEO);
    }

    return (initialized & flags) == flags;
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
    if (!is_hints_init())
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
    }

    ++data.ref_counts[HINTS_SUBSYSTEM];
    return true;
}

bool app_instance::is_hints_init() const
{
    return data.hints_ptr != nullptr;
}

void app_instance::quit_hints()
{
    if (!is_hints_init())
    {
        VX_ASSERT(data.ref_counts[HINTS_SUBSYSTEM] == 0);
        return;
    }

    --data.ref_counts[HINTS_SUBSYSTEM];
    if (data.ref_counts[HINTS_SUBSYSTEM] == 0)
    {
        data.hints_ptr->quit();
        data.hints_ptr.reset();
    }
}

////////////////////////////////////////

bool app_instance::init_events()
{
    if (!is_events_init())
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
    }

    ++data.ref_counts[EVENTS_SUBSYSTEM];
    return true;
}

bool app_instance::is_events_init() const
{
    return data.events_ptr != nullptr;
}

void app_instance::quit_events()
{
    if (!is_events_init())
    {
        VX_ASSERT(data.ref_counts[EVENTS_SUBSYSTEM] == 0);
        return;
    }

    --data.ref_counts[EVENTS_SUBSYSTEM];
    if (data.ref_counts[EVENTS_SUBSYSTEM] == 0)
    {
        data.events_ptr->quit();
        data.events_ptr.reset();
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

    if (!is_video_init())
    {
        data.video_ptr.reset(new video::video_instance);

        if (!data.video_ptr)
        {
            VX_SET_VIDEO_SUBSYSTEM_INIT_FAILED_ERROR();
            return false;
        }

        if (!data.video_ptr->init(this))
        {
            quit_video();
            return false;
        }
    }

    ++data.ref_counts[VIDEO_SUBSYSTEM];
    return true;
}

bool app_instance::is_video_init() const
{
    return data.video_ptr != nullptr;
}

void app_instance::quit_video()
{
    if (!is_video_init())
    {
        VX_ASSERT(data.ref_counts[VIDEO_SUBSYSTEM] == 0);
        return;
    }

    --data.ref_counts[VIDEO_SUBSYSTEM];
    if (data.ref_counts[VIDEO_SUBSYSTEM] == 0)
    {
        quit_events();
        data.video_ptr->quit();
        data.video_ptr.reset();
    }
}

////////////////////////////////////////

VX_API bool get_metadata(app_metadata& metadata)
{
    VX_CHECK_APP_INIT(false);
    metadata = s_app_ptr->get_metadata();
    return true;
}

void app_instance::set_metadata(const app_metadata& metadata)
{
    data.metadata.name = metadata.name ? metadata.name : "Vertex_App";
    data.metadata.version = metadata.version ? metadata.version : VX_VERSION_STRING;
    data.metadata.id = metadata.id;
    data.metadata.creator = metadata.creator;
    data.metadata.url = metadata.url;
}

const app_metadata& app_instance::get_metadata() const
{
    return data.metadata;
}

} // namespace app
} // namespace vx