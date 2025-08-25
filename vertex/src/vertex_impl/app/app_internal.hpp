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
// instance
///////////////////////////////////////////////////////////////////////////////

class app_instance
{
private:

    ///////////////////////////////////////////////////////////////////////////////
    // app data
    ///////////////////////////////////////////////////////////////////////////////

    struct app_data
    {
        owner_ptr<hint::hints_instance> hints;
        owner_ptr<event::events_instance> events;
        //owner_ptr<video::video_data> video_data;
    };

    app_data data;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // initialization
    ///////////////////////////////////////////////////////////////////////////////

    bool init(init_flag flags);
    bool is_init(init_flag flags) const;
    void quit(init_flag flags);

    bool can_shutdown() const;
    void shutdown();

    ///////////////////////////////////////////////////////////////////////////////
    // hints
    ///////////////////////////////////////////////////////////////////////////////

    bool init_hints();
    bool is_init_hints() const;
    void quit_hints();

    hint::hints_instance* get_hints_instance() { return data.hints.get(); }

    ///////////////////////////////////////////////////////////////////////////////
    // events
    ///////////////////////////////////////////////////////////////////////////////

    bool init_events();
    bool is_init_events() const;
    void quit_events();

    event::events_instance* get_events_instance() { return data.events.get(); }
};

extern owner_ptr<app_instance> s_app;

#define s_hints (s_app->get_hints_instance())
#define s_events (s_app->get_events_instance())
#define s_video_data (s_app->get_video())

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

#define VX_CHECK_APP_INIT(r) VX_CHECK_SUBSYSTEM_INIT(App, s_app, r)
#define VX_CHECK_APP_INIT_VOID() VX_CHECK_APP_INIT(void())

#define VX_CHECK_HINTS_SUBSYSTEM_INIT(r) VX_CHECK_SUBSYSTEM_INIT(Hints subsystem, s_hints, r)
#define VX_CHECK_HINTS_SUBSYSTEM_INIT_VOID() VX_CHECK_HINTS_SUBSYSTEM_INIT(void())

#define VX_CHECK_EVENT_SUBSYSTEM_INIT(r) VX_CHECK_SUBSYSTEM_INIT(Event subsystem, s_events, r)
#define VX_CHECK_EVENT_SUBSYSTEM_INIT_VOID() VX_CHECK_EVENT_SUBSYSTEM_INIT(void())

#define VX_CHECK_VIDEO_SUBSYSTEM_INIT(r) VX_CHECK_SUBSYSTEM_INIT(Video subsystem, s_video_data, r)
#define VX_CHECK_VIDEO_SUBSYSTEM_INIT_VOID() VX_CHECK_VIDEO_SUBSYSTEM_INIT(void())

} // namespace app
} // namespace vx