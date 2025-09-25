#pragma once

#include "vertex/app/app.hpp"
#include "vertex/app/owner_ptr.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace app {

///////////////////////////////////////////////////////////////////////////////
// subsystem instances
///////////////////////////////////////////////////////////////////////////////

namespace hint { struct hint_data; }
namespace event { struct event_data; }
namespace video { struct video_data; }
namespace audio { struct audio_data; }
namespace camera { struct camera_data; }

///////////////////////////////////////////////////////////////////////////////
// data
///////////////////////////////////////////////////////////////////////////////

struct app_data
{
    init_flag flags = init_flag::NONE;

    owner_ptr<hint::hint_data> hint_data_ptr;
    owner_ptr<event::event_data> event_data_ptr;
    owner_ptr<video::video_data> video_data_ptr;
};

///////////////////////////////////////////////////////////////////////////////
// instance
///////////////////////////////////////////////////////////////////////////////

class app_internal
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // initialization
    ///////////////////////////////////////////////////////////////////////////////

    static bool init_app();
    static void quit_app();

    static bool init_hints();
    static void quit_hints();

    static bool init_events();
    static void quit_events();

    static bool init_video();
    static void quit_video();

    static init_flag init(init_flag flags);
    static bool is_init(init_flag flags);
    static void quit(init_flag flags);

    static void shutdown();
};

///////////////////////////////////////////////////////////////////////////////
// helpers
///////////////////////////////////////////////////////////////////////////////

#define VX_SET_SUBSYSTEM_NOT_INIT_ERROR(name) err::set(err::UNSUPPORTED_OPERATION, #name " not initialized")

///////////////////////////////////////

#define VX_CHECK_SUBSYSTEM_INIT(name, ptr, r) \
do \
{ \
    if (!(ptr)) \
    { \
        VX_SET_SUBSYSTEM_NOT_INIT_ERROR(name); \
        return (r); \
    } \
} while (0)

///////////////////////////////////////

#define VX_CHECK_SUBSYSTEM_INIT_BACKEND(ptr, r) \
do \
{ \
    if (!(ptr)) \
    { \
        return (r); \
    } \
} while (0)

///////////////////////////////////////

#define VX_SAFE_CALL(ptr, expr) \
do \
{ \
    if ((ptr)) \
    { \
        expr; \
    } \
} while (0)

///////////////////////////////////////

extern owner_ptr<app_data> s_app_data_ptr;

#define VX_CHECK_APP_INIT(r) VX_CHECK_SUBSYSTEM_INIT(App, s_app_data_ptr, r)
#define VX_CHECK_APP_INIT_VOID() VX_CHECK_APP_INIT(void())

#define VX_CHECK_APP_INIT_BACKEND(r) VX_CHECK_SUBSYSTEM_INIT_BACKEND(s_app_data_ptr, r)
#define VX_CHECK_APP_INIT_BACKEND_VOID() VX_CHECK_APP_INIT_BACKEND(void())

#define VX_SAFE_APP_CALL(expr) VX_SAFE_CALL(s_app_data_ptr, expr)

///////////////////////////////////////

#define s_hint_data_ptr (s_app_data_ptr->hint_data_ptr)

#define VX_CHECK_HINTS_SUBSYSTEM_INIT(r) VX_CHECK_SUBSYSTEM_INIT(Hints subsystem, s_hint_data_ptr, r)
#define VX_CHECK_HINTS_SUBSYSTEM_INIT_VOID() VX_CHECK_HINTS_SUBSYSTEM_INIT(void())

#define VX_CHECK_HINTS_SUBSYSTEM_INIT_BACKEND(r) VX_CHECK_SUBSYSTEM_INIT_BACKEND(s_hint_data_ptr, r)
#define VX_CHECK_HINTS_SUBSYSTEM_INIT_BACKEND_VOID() VX_CHECK_HINTS_SUBSYSTEM_INIT_BACKEND(void())

#define VX_SAFE_HINTS_CALL(expr) VX_SAFE_CALL(s_hint_data_ptr, expr)

///////////////////////////////////////

#define s_event_data_ptr (s_app_data_ptr->event_data_ptr)

#define VX_CHECK_EVENT_SUBSYSTEM_INIT(r) VX_CHECK_SUBSYSTEM_INIT(Event subsystem, s_event_data_ptr, r)
#define VX_CHECK_EVENT_SUBSYSTEM_INIT_VOID() VX_CHECK_EVENT_SUBSYSTEM_INIT(void())

#define VX_CHECK_EVENT_SUBSYSTEM_INIT_BACKEND(r) VX_CHECK_SUBSYSTEM_INIT_BACKEND(s_event_data_ptr, r)
#define VX_CHECK_EVENT_SUBSYSTEM_INIT_BACKEND_VOID() VX_CHECK_EVENT_SUBSYSTEM_INIT_BACKEND(void())

#define VX_SAFE_EVENT_CALL(expr) VX_SAFE_CALL(s_event_data_ptr, expr)

///////////////////////////////////////

#define s_video_data_ptr (s_app_data_ptr->video_data_ptr)

#define VX_CHECK_VIDEO_SUBSYSTEM_INIT(r) VX_CHECK_SUBSYSTEM_INIT(Video subsystem, s_video_data_ptr, r)
#define VX_CHECK_VIDEO_SUBSYSTEM_INIT_VOID() VX_CHECK_VIDEO_SUBSYSTEM_INIT(void())

#define VX_CHECK_VIDEO_SUBSYSTEM_INIT_BACKEND(r) VX_CHECK_SUBSYSTEM_INIT_BACKEND(s_video_data_ptr, r)
#define VX_CHECK_VIDEO_SUBSYSTEM_INIT_BACKEND_VOID() VX_CHECK_VIDEO_SUBSYSTEM_INIT_BACKEND(void())

#define VX_SAFE_VIDEO_CALL(expr) VX_SAFE_CALL(s_video_data_ptr, expr)

} // namespace app
} // namespace vx