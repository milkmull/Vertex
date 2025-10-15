#pragma once

#include "vertex/app/video/window.hpp"
#include "vertex/app/owner_ptr.hpp"
#include "vertex/pixel/surface.hpp"

namespace vx {
namespace app {
namespace video {

class video_instance;

///////////////////////////////////////////////////////////////////////////////
// helper defs
///////////////////////////////////////////////////////////////////////////////

struct window_flags
{
    using type = uint64_t;

    enum : type
    {
        NONE = 0,

        FULLSCREEN = (1 << 0),
        MINIMIZED = (1 << 1),
        MAXIMIZED = (1 << 2),

        BORDERLESS = (1 << 3),
        RESIZABLE = (1 << 4),
        TOPMOST = (1 << 5),

        HIDDEN = (1 << 6),
        FOCUSSED = (1 << 7),

        MOUSE_GRABBED = (1 << 8),
        MOUSE_CAPTURE = (1 << 9),

        KEYBOARD_GRABBED = (1 << 10),

        // flags to be set by the os on window creation
        CREATE_FLAGS = (HIDDEN | MINIMIZED | BORDERLESS | RESIZABLE | TOPMOST),

        // flags to be restored when a hidden window is shown again
        PENDING_FLAGS = (MAXIMIZED | MINIMIZED | FULLSCREEN | KEYBOARD_GRABBED | MOUSE_GRABBED)
    };
};

enum window_rect_type
{
    INPUT = 0,
    CURRENT,
    WINDOWED,
    FLOATING
};

///////////////////////////////////////////////////////////////////////////////
// window data
///////////////////////////////////////////////////////////////////////////////

struct window_data
{
    window_id id = INVALID_ID;

    std::string title;
    math::vec2i position;
    math::vec2i size;

    math::vec2i min_size, max_size;
    float locked_aspect = 0.0f;

    // Position and size of the window_instance in any windowed state.
    // This includes maximized or tiled states, but excludes fullscreen.
    // This is the position and size that the window_instance should return to
    // when leaving fullscreen state.
    math::recti windowed_rect;

    // Position and size of the window_instance in a normal "floating" state
    // (not maximized, fullscreen, or tiled). This is the position and
    // size that the window_instance should return to when restored.
    math::recti floating_rect;

    // The most recently requested position and size of the window_instance.
    // This is a temporary holding value used after the front end requests
    // a move or resize, but before the backend has applied the change.
    // Once the backend confirms the new position and size, this rect is
    // replaced by the actual window_instance geometry.
    math::recti pending_rect;

    //  True if the frontend requested a move that has not yet been confirmed.
    //  When true, GetPosition() should return pending_rect.x/y, because
    //  the system may never explicitly confirm the new position.
    //  (Some window_instance managers do not reliably send move notifications.)
    //
    //  Why only for position:
    //  - Position requests are best effort. Some systems ignore or adjust
    //    them silently. Without exposing the pending value, the app would
    //    have no way to know what it asked for.
    bool last_position_pending = false;

    //  True if the frontend requested a resize that has not yet been confirmed.
    //  Unlike position, GetSize() should not return the pending size. It should
    //  continue to return the true, confirmed size from the system.
    //
    //  Why the difference from position:
    //  - Rendering backends (GL, Vulkan, D3D) require the true size of the
    //    drawable surface. Returning a pending size here would desync the
    //    rendering pipeline and cause invalid buffers or crashes.
    //  - All major systems (Win32, X11, Wayland, Cocoa) send explicit resize
    //    notifications, so we can trust the authoritative size will arrive.
    //
    //  Summary:
    //  - Pending position may be returned, since confirmation is unreliable.
    //  - Pending size is never returned, since confirmation is guaranteed
    //    and rendering correctness depends on the real value.
    bool last_size_pending = false;

    typename window_flags::type flags = window_flags::NONE;

    // Flags representing window state changes (minimized, maximized, restored, fullscreen, etc.)
    // that should take effect once the window becomes visible again.
    // If the window is hidden when a state change is requested, the desired state is stored here
    // and applied when the window is shown.
    typename window_flags::type pending_flags = window_flags::NONE;

    // stores the requested mode inormation, can be used to look up the closes mode to the requested
    // configuration if the exact requestes mode does not exist
    display_mode requested_fullscreen_mode;
    bool requested_fullscreen_mode_set = false;
    // when the fullscreen flag is set, it is assumed that this value is valid
    display_mode current_fullscreen_mode;

    bool update_fullscreen_on_display_changed = false;

    bool fullscreen_exclusive = false;
    display_id last_fullscreen_exclusive_display_id = INVALID_ID;

    // The id of the display that the window was last detected on
    display_id last_display_id = INVALID_ID;
    // Stores the display that was initially requested to show the window
    display_id pending_display_id = INVALID_ID;

    float display_scale = 1.0f;
    float opacity = 1.0f;

    bool initializing = false;
    bool destroying = false;

    bool hiding = false;
    bool restore_on_show = false;

    bool tiled = false;

    bool mouse_capture = false;
};

///////////////////////////////////////////////////////////////////////////////
// window_instance_impl
///////////////////////////////////////////////////////////////////////////////

class window_instance_impl;

struct window_instance_impl_deleter
{
    void operator()(window_instance_impl* ptr) const noexcept;
};

///////////////////////////////////////////////////////////////////////////////
// window instance
///////////////////////////////////////////////////////////////////////////////

class window_instance
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // creation
    ///////////////////////////////////////////////////////////////////////////////

    bool create(video_instance* owner, const window_config& config);
    void finish_creation(typename window_flags::type new_flags);
    void destroy();

    bool validate() const;
    void apply_flags(typename window_flags::type new_flags);
    
    ///////////////////////////////////////////////////////////////////////////////
    // sync
    ///////////////////////////////////////////////////////////////////////////////
    
    void sync();
    
    ///////////////////////////////////////////////////////////////////////////////
    // title
    ///////////////////////////////////////////////////////////////////////////////
    
    const std::string& get_title() const;
    bool set_title(const std::string& title);
    
    ///////////////////////////////////////////////////////////////////////////////
    // position and size
    ///////////////////////////////////////////////////////////////////////////////
    
    bool set_resizable(bool resizable);
    bool is_resizable() const;
    
    bool get_position(int* x, int* y) const;
    bool set_position(int x, int y);
    
    bool get_size(int* w, int* h) const;
    bool set_size(int w, int h);
    
    math::vec2i get_center() const;
    math::recti get_rect() const;

    bool get_border_size(int* left, int* right, int* bottom, int* top) const;
    bool get_window_size_in_pixels(int* w, int* h);
    
    bool get_min_size(int* w, int* h) const;
    bool get_max_size(int* w, int* h) const;
    
    bool set_min_size(int w, int h);
    bool set_max_size(int w, int h);
    
    float get_aspect_ratio() const;
    float get_locked_aspect_ratio() const;
    bool lock_aspect_ratio(float aspect_ratio);
    
    ///////////////////////////////////////////////////////////////////////////////
    // bordered
    ///////////////////////////////////////////////////////////////////////////////
    
    bool set_bordered(bool bordered);
    bool is_bordered() const;
    
    ///////////////////////////////////////////////////////////////////////////////
    // always on top
    ///////////////////////////////////////////////////////////////////////////////
    
    bool set_always_on_top(bool always_on_top);
    bool is_always_on_top() const;
    
    ///////////////////////////////////////////////////////////////////////////////
    // window_instance operators
    ///////////////////////////////////////////////////////////////////////////////
    
    bool show();
    bool hide();
    bool is_visible() const;
    bool is_hidden() const;
    
    bool minimize();
    bool is_minimized() const;
    
    bool maximize();
    bool is_maximized() const;
    
    bool restore();
    
    bool flash(window_flash_op operation);
    
    ///////////////////////////////////////////////////////////////////////////////
    // fullscreen
    ///////////////////////////////////////////////////////////////////////////////
    
    bool is_fullscreen() const;
    bool is_fullscreen_visible() const;
    
    const display_mode& get_fullscreen_mode() const;
    bool set_fullscreen_mode(const display_mode* mode);  

    bool set_fullscreen(bool fullscreen);

    struct fullscreen_op
    {
        using type = int;

        enum : type
        {
            LEAVE = 0,
            ENTER = 1,
            UPDATE
        };
    };

    enum class fullscreen_result
    {
        FAILED,
        SUCCEEDED,
        PENDING
    };
    
    bool update_fullscreen_mode(typename fullscreen_op::type fullscreen, bool commit);

    ///////////////////////////////////////////////////////////////////////////////
    // icon
    ///////////////////////////////////////////////////////////////////////////////
    
    bool set_icon(const pixel::surface_rgba8& surf);
    void clear_icon();

    ///////////////////////////////////////////////////////////////////////////////
    // grab
    ///////////////////////////////////////////////////////////////////////////////

    bool set_mouse_grab(bool grabbed);
    bool set_keyboard_grab(bool grabbed);

    void clear_grab();
    void update_grab();

    ///////////////////////////////////////////////////////////////////////////////
    // focus
    ///////////////////////////////////////////////////////////////////////////////

    bool has_mouse_focus() const;
    bool has_keyboard_focus() const;

    ///////////////////////////////////////////////////////////////////////////////
    // mouse
    ///////////////////////////////////////////////////////////////////////////////

    void set_mouse_capture(bool capture);
    bool mouse_capture_enabled() const;
    
    ///////////////////////////////////////////////////////////////////////////////
    // events
    ///////////////////////////////////////////////////////////////////////////////
    
    bool post_window_shown();
    void on_window_shown();
    
    bool post_window_hidden();
    void on_window_hidden();
    
    bool post_window_moved(int32_t x, int32_t y);
    void on_window_moved();
    
    bool post_window_resized(int32_t w, int32_t h);
    void on_window_resized();
    
    bool post_window_minimized();
    void on_window_minimized();
    
    bool post_window_maximized();
    void on_window_maximized();
    
    bool post_window_restored();
    void on_window_restored();
    
    bool post_window_enter_fullscreen();
    bool post_window_leave_fullscreen();
    
    //bool post_window_gained_focus();
    //bool post_window_lost_focus();
    //bool post_window_mouse_enter();
    //bool post_window_mouse_leave();
    
    bool post_window_display_changed(const display& d);
    void on_window_display_changed(const display& d);
    
    //bool post_window_display_scale_changed();
    
    bool post_window_close_requested();
    bool post_window_destroyed();

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    video_instance* video = nullptr;
    window_data data;
    owner_ptr<window_instance_impl, window_instance_impl_deleter> impl_ptr;
};

} // namespace video
} // namespace app
} // namespace vx