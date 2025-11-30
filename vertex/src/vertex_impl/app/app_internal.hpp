#pragma once

#include "vertex/app/app.hpp"
#include "vertex/pixel/surface.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace app {

//=============================================================================
// subsystem instances
//=============================================================================

namespace hint { class hints_instance; }
namespace event { class events_instance; }

#if defined(VX_APP_VIDEO_ENABLED)
namespace video { class video_instance; }
#endif // VX_APP_VIDEO_ENABLED


namespace audio { class audio_instance; }
namespace camera { class camera_instance; }

enum subsystem
{
    hints_index,
    events_index,
    video_index,
    audio_index,
    camera_index,

    subsystem_count
};

//=============================================================================
// data
//=============================================================================

struct app_data
{
    app_metadata metadata;

    std::unique_ptr<hint::hints_instance> hints_ptr;
    std::unique_ptr<event::events_instance> events_ptr;

#if defined(VX_APP_VIDEO_ENABLED)
    std::unique_ptr<video::video_instance> video_ptr;
#endif // VX_APP_VIDEO_ENABLED

    size_t ref_counts[subsystem_count] = {};
};

//=============================================================================
// instance
//=============================================================================

class app_instance
{
public:

    app_instance();
    ~app_instance();

    app_instance(const app_instance&) = delete;
    app_instance& operator=(const app_instance&) = delete;

    app_instance(app_instance&&) noexcept = delete;
    app_instance& operator=(app_instance&&) noexcept = delete;

public:

    //=============================================================================
    // initialization
    //=============================================================================

    bool init(const app_metadata& metadata = {});
    void quit();

    init_flags init_subsystem(init_flags flags);
    bool is_subsystem_init(init_flags flags) const;
    void quit_subsystem(init_flags flags);

    bool init_hints();
    bool is_hints_init() const;
    void quit_hints();

    bool init_events();
    bool is_events_init() const;
    void quit_events();

    bool init_video();
    bool is_video_init() const;
    void quit_video();

    //=============================================================================
    // metadata
    //=============================================================================

    void set_metadata(const app_metadata& metadata);
    const app_metadata& get_metadata() const;

    //=============================================================================
    // data
    //=============================================================================

    app_data data;
};

extern std::unique_ptr<app_instance> s_app;

//=============================================================================
// helpers
//=============================================================================

#define VX_SET_SUBSYSTEM_NOT_INIT_ERROR(name) err::set(err::unsupported_operation, #name " not initialized")
#define VX_SET_SUBSYSTEM_INIT_FAILED_ERROR(name) err::set(err::out_of_memory, "Failed to initialize " #name " subsystem")

///////////////////////////////////////

#define VX_SAFE_CALL(ptr, expr) \
do \
{ \
    if ((ptr)) \
    { \
        expr; \
    } \
} while (VX_NULL_WHILE_LOOP_CONDITION)

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
} while (VX_NULL_WHILE_LOOP_CONDITION)

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
} while (VX_NULL_WHILE_LOOP_CONDITION)

///////////////////////////////////////

#define VX_CHECK_SUBSYSTEM_INIT_BACKEND(name, ptr, r) \
do \
{ \
    if (!(ptr)) \
    { \
        return (r); \
    } \
} while (VX_NULL_WHILE_LOOP_CONDITION)

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

#define s_video_ptr (s_app_ptr->data.video_ptr)

#define VX_CHECK_VIDEO_SUBSYSTEM_INIT(r) VX_CHECK_SUBSYSTEM_INIT(Video subsystem, s_video_ptr, r)
#define VX_CHECK_VIDEO_SUBSYSTEM_INIT_VOID() VX_CHECK_VIDEO_SUBSYSTEM_INIT(void())

#define VX_CHECK_VIDEO_SUBSYSTEM_INIT_BACKEND(r) VX_CHECK_SUBSYSTEM_INIT_BACKEND(s_video_ptr, r)
#define VX_CHECK_VIDEO_SUBSYSTEM_INIT_BACKEND_VOID() VX_CHECK_VIDEO_SUBSYSTEM_INIT_BACKEND(void())

#define VX_SAFE_VIDEO_CALL(expr) VX_SAFE_CALL(s_video_ptr, expr)
#define VX_SET_VIDEO_SUBSYSTEM_INIT_FAILED_ERROR() VX_SET_SUBSYSTEM_INIT_FAILED_ERROR(Video)

///////////////////////////////////////

using argb_surface = pixel::surface<pixel::pixel_format::argb_8888>;

} // namespace app
} // namespace vx