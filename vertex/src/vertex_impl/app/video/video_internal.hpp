#pragma once

#include "vertex/system/error.hpp"
#include "vertex/app/video/video.hpp"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// helpers
///////////////////////////////////////////////////////////////////////////////

#define VX_SET_VIDEO_NOT_INIT_ERROR() err::set(err::UNSUPPORTED_OPERATION, "Video subsystem has not been initialized")

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
// video data
///////////////////////////////////////////////////////////////////////////////

struct video_data
{
    bool is_init = false;

    std::vector<std::unique_ptr<display>> displays;
    bool setting_display_mode = false;
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
    // impl helpers
    ///////////////////////////////////////////////////////////////////////////////

    // Call the private constructor of a video type
    template <typename T>
    static T construct() { return T{}; }

    template <typename T>
    static auto create_impl() { return std::make_unique<T>(); }

    // Access private implementation details of a video type
    template <typename T>
    static auto* get_impl(T& x) { return x.m_impl.get(); }

    // Access private implementation details of a video type
    template <typename T>
    static const auto* get_impl(const T& x) { return x.m_impl.get(); }

    ///////////////////////////////////////////////////////////////////////////////
    // display events
    ///////////////////////////////////////////////////////////////////////////////

    static bool post_display_added(const display* d);
    static void on_display_added();

    static bool post_display_removed(const display* d);
    static bool post_display_moved(const display* d);
    static bool post_display_orientation_changed(display* d, display_orientation orientation);
    static bool post_display_desktop_mode_changed(display* d, const display_mode& mode);
    static bool post_display_current_mode_changed(display* d, const display_mode& mode);
    static bool post_display_content_scale_changed(display* d, const math::vec2& content_scale);

};

using manager = _priv::video_internal;

} // namespace video
} // namespace app
} // namespace vx