#pragma once

#include "vertex/app/video/video.hpp"
#include "vertex/app/video/window.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// helpers
///////////////////////////////////////////////////////////////////////////////

#define VX_SET_VIDEO_NOT_INIT_ERROR() err::set(err::UNSUPPORTED_OPERATION, "Video subsystem not initialized")

///////////////////////////////////////

#define VX_CHECK_VIDEO_INIT(x) \
do \
{ \
    if (!is_init()) \
    { \
        VX_SET_VIDEO_NOT_INIT_ERROR(); \
        return (x); \
    } \
} while (0)

///////////////////////////////////////

#define VX_CHECK_VIDEO_INIT_VOID() VX_CHECK_VIDEO_INIT(void())

///////////////////////////////////////

#define VX_CHECK_IMPL() VX_ASSERT(m_impl)

///////////////////////////////////////////////////////////////////////////////
// video capabilities
///////////////////////////////////////////////////////////////////////////////

enum capabilities
{
    NONE                            = 0,
    MODE_SWITCHING_EMULATED         = (1 << 0),
    SENDS_FULLSCREEN_DIMENSIONS     = (1 << 1),
    IS_FULLSCREEN_ONLY              = (1 << 2),
    SENDS_DISPLAY_CHANGES           = (1 << 3),
    DISABLE_FULLSCREEN_MOUSE_WARP   = (1 << 4),
    SENDS_HDR_CHANGES               = (1 << 5)
};

///////////////////////////////////////////////////////////////////////////////
// video data
///////////////////////////////////////////////////////////////////////////////

struct video_data
{
    bool is_init = false;

    std::vector<owner_ptr<display>> displays;
    bool setting_display_mode = false;

    std::vector<owner_ptr<window>> windows;
};

extern video_data s_video_data;

///////////////////////////////////////////////////////////////////////////////
// video_internal
///////////////////////////////////////////////////////////////////////////////

// Internal access helper for the `video` module.
// This class provides trusted internal access to private members and types,
// without exposing implementation details to users of the public API.

class _priv::video_internal
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // video
    ///////////////////////////////////////////////////////////////////////////////

    static bool init();
    static bool is_init();
    static void quit();

    ///////////////////////////////////////////////////////////////////////////////
    // dpi
    ///////////////////////////////////////////////////////////////////////////////

    static process_dpi_awareness get_dpi_awareness();

    ///////////////////////////////////////////////////////////////////////////////
    // system theme
    ///////////////////////////////////////////////////////////////////////////////

    static system_theme get_system_theme();

    ///////////////////////////////////////////////////////////////////////////////
    // display mode
    ///////////////////////////////////////////////////////////////////////////////

    static void clear_display_mode_display_id(display_mode& dm);

    ///////////////////////////////////////////////////////////////////////////////
    // displays
    ///////////////////////////////////////////////////////////////////////////////

    static void update_displays();

    static display* get_display(device_id id);
    static display* get_primary_display();

    static size_t display_count();
    static display* enum_displays(size_t i);

    static display* get_display_for_point(const math::vec2i& p);
    static display* get_display_for_rect(const math::recti& rect);
    static display* get_display_for_window(const window& w);

    static math::recti get_desktop_area();
    static device_id get_display_id(const display& d);

    static void init_display_modes(const display& d);

    ///////////////////////////////////////////////////////////////////////////////
    // windows
    ///////////////////////////////////////////////////////////////////////////////

    static window* create_window(const window_config& config);
    static void destroy_window(window& w);
    static void destroy_windows();

    static window* get_window(device_id id);
    static device_id get_window_id(const window& w);

    static size_t window_count();
    static window* enum_windows(size_t i);

    static void check_window_display_changed(window& w);

    ///////////////////////////////////////////////////////////////////////////////
    // fullscreen helpers
    ///////////////////////////////////////////////////////////////////////////////

    static device_id get_display_fullscreen_window_id(const display& d);
    static void set_display_fullscreen_window_id(display& d, device_id id);
    static void clear_display_fullscreen_window_id(display& d);
    static display* find_display_with_fullscreen_window(const window& w);

    ///////////////////////////////////////////////////////////////////////////////
    // event handlers
    ///////////////////////////////////////////////////////////////////////////////

    static bool post_display_added(const display& d);
    static void on_display_added();

    static bool post_display_removed(const display& d);
    static bool post_display_moved(const display& d);
    static bool post_display_orientation_changed(display& d, display_orientation orientation);
    static bool post_display_desktop_mode_changed(display& d, const display_mode& mode);
    static bool post_display_current_mode_changed(display& d, const display_mode& mode);
    static bool post_display_content_scale_changed(display& d, const math::vec2& content_scale);

    ///////////////////////////////////////////////////////////////////////////////
    // impl helpers
    ///////////////////////////////////////////////////////////////////////////////

    template <typename T>
    static T* create() { return new T; }

    template <typename T>
    static T construct() { return T{}; }

    template <typename T>
    static auto create_impl() { return std::make_unique<T>(); }

    template <typename T>
    static auto* get_impl(T& x) { return x.m_impl.get(); }

    template <typename T>
    static const auto* get_impl(const T& x) { return x.m_impl.get(); }
};

using video_internal = _priv::video_internal;

} // namespace video
} // namespace app
} // namespace vx