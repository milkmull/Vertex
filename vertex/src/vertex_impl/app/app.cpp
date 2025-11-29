#include "vertex/config/version.hpp"
#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/hints/hints_internal.hpp"
#include "vertex_impl/app/event/event_internal.hpp"

#if defined(VX_APP_VIDEO_ENABLED)
#   include "vertex_impl/app/video/video_internal.hpp"
#endif // VX_APP_VIDEO_ENABLED

namespace vx {
namespace app {

///////////////////////////////////////////////////////////////////////////////
// initialization
///////////////////////////////////////////////////////////////////////////////

std::unique_ptr<app_instance> s_app;

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
        err::set(err::out_of_memory, "Failed to create application instance");
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

VX_API init_flags init_subsystem(init_flags flags)
{
    if (!is_init() && !init())
    {
        return init_flags::none;
    }

    return s_app_ptr->init_subsystem(flags);
}

////////////////////////////////////////

VX_API bool is_subsystem_init(init_flags flags)
{
    VX_CHECK_APP_INIT(false);
    return s_app_ptr->is_subsystem_init(flags);
}

////////////////////////////////////////

VX_API void quit_subsystem(init_flags flags)
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

#define add_flag(dst, flag) dst = static_cast<init_flags>(dst | flag)
#define remove_flag(dst, flag) dst = static_cast<init_flags>(dst & ~flag)

init_flags app_instance::init_subsystem(init_flags flags)
{
    init_flags initialized = init_flags::none;

    if (flags & init_flags::events)
    {
        if (init_events())
        {
            add_flag(initialized, init_flags::events);
        }
    }
    
    if (flags & init_flags::video)
    {
        if (init_video())
        {
            add_flag(initialized, init_flags::video);
        }
    }

    return initialized;
}

////////////////////////////////////////

bool app_instance::is_subsystem_init(init_flags flags) const
{
    init_flags initialized = init_flags::none;

    if (is_events_init())
    {
        add_flag(initialized, init_flags::events);
    }

    if (is_video_init())
    {
        add_flag(initialized, init_flags::video);
    }

    return (initialized & flags) == flags;
}

////////////////////////////////////////

void app_instance::quit_subsystem(init_flags flags)
{
    if (flags & init_flags::video)
    {
        quit_video();
    }

    if (flags & init_flags::events)
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

    ++data.ref_counts[hints_index];
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
        VX_ASSERT(data.ref_counts[hints_index] == 0);
        return;
    }

    --data.ref_counts[hints_index];
    if (data.ref_counts[hints_index] == 0)
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

    ++data.ref_counts[events_index];
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
        VX_ASSERT(data.ref_counts[events_index] == 0);
        return;
    }

    --data.ref_counts[events_index];
    if (data.ref_counts[events_index] == 0)
    {
        data.events_ptr.reset();
    }
}

////////////////////////////////////////

bool app_instance::init_video()
{
#if defined(VX_APP_VIDEO_ENABLED)

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

    ++data.ref_counts[video_index];
    return true;

#else

    VX_UNSUPPORTED("init_video()");
    return false;

#endif // VX_APP_VIDEO_ENABLED
}

bool app_instance::is_video_init() const
{
#if defined(VX_APP_VIDEO_ENABLED)
    return data.video_ptr != nullptr;
#else
    return false;
#endif // VX_APP_VIDEO_ENABLED
}

void app_instance::quit_video()
{
#if defined(VX_APP_VIDEO_ENABLED)

    if (!is_video_init())
    {
        VX_ASSERT(data.ref_counts[video_index] == 0);
        return;
    }

    --data.ref_counts[video_index];
    if (data.ref_counts[video_index] == 0)
    {
        data.video_ptr.reset();
        quit_events();
    }

#endif // VX_APP_VIDEO_ENABLED
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