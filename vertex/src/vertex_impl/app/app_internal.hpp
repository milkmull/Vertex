#pragma once

#include "vertex/app/app.hpp"
#include "vertex/app/owner_ptr.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace app {

///////////////////////////////////////////////////////////////////////////////
// subsystem instances
///////////////////////////////////////////////////////////////////////////////

namespace hint { struct hints_instance; }
namespace event { struct events_instance; }
namespace video { struct video_data; }
namespace audio { struct audio_data; }
namespace camera { struct camera_data; }

///////////////////////////////////////////////////////////////////////////////
// data
///////////////////////////////////////////////////////////////////////////////

struct app_data
{
    init_flag flags = init_flag::NONE;

    owner_ptr<hint::hints_instance> hints_ptr;
    owner_ptr<event::events_instance> events_ptr;
    size_t events_ref_count = 0;
    //owner_ptr<video::video_data> video_data_ptr;
};

///////////////////////////////////////////////////////////////////////////////
// instance
///////////////////////////////////////////////////////////////////////////////

class app_instance
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // initialization
    ///////////////////////////////////////////////////////////////////////////////

    bool init();
    void quit();

    init_flag init_subsystem(init_flag flags);
    bool is_subsystem_init(init_flag flags) const;
    void quit_subsystem(init_flag flags);

    bool init_hints();
    bool is_hints_init() const;
    void quit_hints();

    bool init_events();
    bool is_events_init() const;
    void quit_events();

    bool init_video();
    bool is_video_init() const;
    void quit_video();

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    app_data data;
};

extern owner_ptr<app_instance> s_app;

///////////////////////////////////////////////////////////////////////////////
// helpers
///////////////////////////////////////////////////////////////////////////////

#define VX_SET_SUBSYSTEM_NOT_INIT_ERROR(name) err::set(err::UNSUPPORTED_OPERATION, #name " not initialized")
#define VX_SET_SUBSYSTEM_INIT_FAILED_ERROR(name) err::set(err::OUT_OF_MEMORY, "Failed to initialize " #name " subsystem")

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

#define s_app_ptr (s_app)

#define VX_CHECK_APP_INIT(r) \
do \
{ \
    if (!(s_app_ptr)) \
    { \
        VX_SET_SUBSYSTEM_NOT_INIT_ERROR(App); \
        return (r); \
    } \
} while (0)

#define VX_CHECK_APP_INIT_VOID() VX_CHECK_APP_INIT(void())
#define VX_SAFE_APP_CALL(expr) VX_SAFE_CALL(s_app_ptr, expr)

///////////////////////////////////////

#define VX_CHECK_SUBSYSTEM_INIT(name, ptr, r) \
do \
{ \
    VX_CHECK_APP_INIT(r); \
    if (!(ptr)) \
    { \
        VX_SET_SUBSYSTEM_NOT_INIT_ERROR(name); \
        return (r); \
    } \
} while (0)

///////////////////////////////////////

#define VX_CHECK_SUBSYSTEM_INIT_BACKEND(name, ptr, r) \
do \
{ \
    if (!(ptr)) \
    { \
        return (r); \
    } \
} while (0)

///////////////////////////////////////

#define s_hints_ptr (s_app_ptr->data.hints_ptr)

#define VX_CHECK_HINTS_SUBSYSTEM_INIT(r) VX_CHECK_SUBSYSTEM_INIT(Hints subsystem, s_hints_ptr, r)
#define VX_CHECK_HINTS_SUBSYSTEM_INIT_VOID() VX_CHECK_HINTS_SUBSYSTEM_INIT(void())

#define VX_CHECK_HINTS_SUBSYSTEM_INIT_BACKEND(r) VX_CHECK_SUBSYSTEM_INIT_BACKEND(s_hints_ptr, r)
#define VX_CHECK_HINTS_SUBSYSTEM_INIT_BACKEND_VOID() VX_CHECK_HINTS_SUBSYSTEM_INIT_BACKEND(void())

#define VX_SAFE_HINTS_CALL(expr) VX_SAFE_CALL(s_hints_ptr, expr)
#define VX_SET_HINTS_SUBSYSTEM_INIT_FAILED_ERROR() VX_SET_SUBSYSTEM_INIT_FAILED_ERROR(Hints)

///////////////////////////////////////

#define s_events_ptr (s_app_ptr->data.events_ptr)

#define VX_CHECK_EVENTS_SUBSYSTEM_INIT(r) VX_CHECK_SUBSYSTEM_INIT(Events subsystem, s_events_ptr, r)
#define VX_CHECK_EVENTS_SUBSYSTEM_INIT_VOID() VX_CHECK_EVENTS_SUBSYSTEM_INIT(void())

#define VX_CHECK_EVENTS_SUBSYSTEM_INIT_BACKEND(r) VX_CHECK_SUBSYSTEM_INIT_BACKEND(s_events_ptr, r)
#define VX_CHECK_EVENTS_SUBSYSTEM_INIT_BACKEND_VOID() VX_CHECK_EVENTS_SUBSYSTEM_INIT_BACKEND(void())

#define VX_SAFE_EVENTS_CALL(expr) VX_SAFE_CALL(s_events_ptr, expr)
#define VX_SET_EVENTS_SUBSYSTEM_INIT_FAILED_ERROR() VX_SET_SUBSYSTEM_INIT_FAILED_ERROR(Events)

///////////////////////////////////////

//#define s_video_data_ptr (s_app->video_data_ptr)
//
//#define VX_CHECK_VIDEO_SUBSYSTEM_INIT(r) VX_CHECK_SUBSYSTEM_INIT(Video subsystem, s_video_data_ptr, r)
//#define VX_CHECK_VIDEO_SUBSYSTEM_INIT_VOID() VX_CHECK_VIDEO_SUBSYSTEM_INIT(void())
//
//#define VX_CHECK_VIDEO_SUBSYSTEM_INIT_BACKEND(r) VX_CHECK_SUBSYSTEM_INIT_BACKEND(s_video_data_ptr, r)
//#define VX_CHECK_VIDEO_SUBSYSTEM_INIT_BACKEND_VOID() VX_CHECK_VIDEO_SUBSYSTEM_INIT_BACKEND(void())
//
//#define VX_SAFE_VIDEO_CALL(expr) VX_SAFE_CALL(s_video_data_ptr, expr)

} // namespace app
} // namespace vx