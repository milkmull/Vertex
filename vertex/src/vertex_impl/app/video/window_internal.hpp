#pragma once

#include "vertex/config/flags.hpp"
#include "vertex/app/video/window.hpp"
#include "vertex/app/video/message_box.hpp"
#include "vertex/config/scoped_enum.hpp"
#include "vertex/pixel/pixel_format.hpp"
#include "vertex/pixel/surface.hpp"

namespace vx {
namespace app {
namespace video {

class video_instance;

//=============================================================================
// window flags
//=============================================================================

VX_FLAGS_UT_DECLARE_BEGIN(window_flags, uint32_t)
{
    none                = 0,

    fullscreen          = VX_BIT(0),
    minimized           = VX_BIT(1),
    maximized           = VX_BIT(2),

    borderless          = VX_BIT(3),
    resizable           = VX_BIT(4),
    topmost             = VX_BIT(5),
    transparent         = VX_BIT(6),

    hidden              = VX_BIT(7),
    occluded            = VX_BIT(8),

    not_focusable       = VX_BIT(9),
    input_focus         = VX_BIT(10),
    mouse_focus         = VX_BIT(11),

    mouse_grabbed       = VX_BIT(12),
    keyboard_grabbed    = VX_BIT(13),

    mouse_capture       = VX_BIT(14),
    mouse_relative_mode = VX_BIT(15),

    // flags to be set by the os on window creation
    create_flags        = (hidden | minimized | borderless | resizable | topmost),

    // flags to be restored when a hidden window is shown again
    pending_flags       = (maximized | minimized | fullscreen | keyboard_grabbed | mouse_grabbed)
}
VX_FLAGS_DECLARE_END(window_flags)

//=============================================================================
// window rect type
//=============================================================================

enum class window_rect_type
{
    input = 0,
    current,
    windowed,
    floating
};

//=============================================================================
// window data
//=============================================================================

// common format used by windows, macOS, X11, etc
using surface_argb = pixel::surface<pixel::pixel_format::argb_8888>;

struct window_data
{
    //=============================================================================
    // identity
    //=============================================================================

    window_id id = invalid_id;
    std::string title;

    //=============================================================================
    // geometry
    //=============================================================================

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

    math::recti safe_inset;
    math::vec2i last_pixel_size;
    math::vec2 display_scale;

    //=============================================================================
    // flags
    //=============================================================================

    window_flags flags = window_flags::none;

    // Flags representing window state changes (minimized, maximized, restored, fullscreen, etc.)
    // that should take effect once the window becomes visible again.
    // If the window is hidden when a state change is requested, the desired state is stored here
    // and applied when the window is shown.
    window_flags pending_flags = window_flags::none;

    //=============================================================================
    // fullscreen
    //=============================================================================

    // stores the requested mode inormation, can be used to look up the closes mode to the requested
    // configuration if the exact requestes mode does not exist
    display_mode requested_fullscreen_mode;
    // when the fullscreen flag is set, it is assumed that this value is valid
    display_mode current_fullscreen_mode;

    // Prevent running fullscreen update logic if a fullscreen operation has already been
    // performed by an event watcher in response to a window display change event. Watchers
    // are invoked when the event is posted—before the "on display change" function runs.
    // That function normally updates the fullscreen mode for the new display when a fullscreen
    // window is moved. If a watcher has already changed the display or fullscreen mode, we
    // skip the update to avoid overriding the new configuration.
    bool update_fullscreen_on_display_changed = false;

    bool fullscreen_exclusive = false;
    display_id last_fullscreen_exclusive_display_id = invalid_id;

    // the display that the window was last detected on
    display_id current_display_id = invalid_id;
    // the display that was initially requested to show the window
    display_id pending_display_id = invalid_id;

    //=============================================================================
    // appearence
    //=============================================================================

    float opacity = 1.0f;
    surface_argb icon;

    bool surface_valid = false;
    surface_argb shape_surface;

    //=============================================================================
    // lifecycle
    //=============================================================================

    bool initializing = false;
    bool destroying = false;
    bool hiding = false;
    bool restore_on_show = false;
    bool tiled = false;

    //=============================================================================
    // mouse
    //=============================================================================

    bool mouse_capture = false;
    math::recti mouse_rect;

    //=============================================================================
    // drag and drop
    //=============================================================================

    bool drag_and_drop_enabled = false;
    bool is_dropping = false;

    //=============================================================================
    // text input
    //=============================================================================

    keyboard::text_input_options text_input_options;
    bool text_input_active = false;
    int text_input_cursor;
    math::recti text_input_rect;
};

//=============================================================================
// window_instance_impl
//=============================================================================

class window_instance_impl;

//=============================================================================
// window instance
//=============================================================================

class window_instance
{
public:

    window_instance();
    ~window_instance();

    window_instance(window_instance&&) noexcept;
    window_instance& operator=(window_instance&&) noexcept;

public:

    //=============================================================================
    // creation
    //=============================================================================

    bool create(video_instance* owner, const window_config& config);
    bool recreate(window_flags flags);
    void finish_creation(window_flags new_flags, bool drag_and_drop);

    void begin_destroy();
    void destroy();

    void apply_flags(window_flags new_flags);

    //=============================================================================
    // surface
    //=============================================================================

    bool destroy_surface() { return true; }
    
    //=============================================================================
    // sync
    //=============================================================================
    
    void sync();
    void sync_if_required();
    
    //=============================================================================
    // title
    //=============================================================================
    
    const std::string& get_title() const;
    bool set_title(const std::string& title);
    
    //=============================================================================
    // position and size
    //=============================================================================
    
    bool set_resizable(bool resizable);
    bool is_resizable() const;
    
    bool get_position(int32_t* x, int32_t* y) const;
    bool set_position(int32_t x, int32_t y);
    
    bool get_size(int32_t* w, int32_t* h) const;
    bool set_size(int32_t w, int32_t h);
    
    math::vec2i get_center() const;
    math::recti get_rect() const;

    bool get_border_size(int32_t* left, int32_t* right, int32_t* bottom, int32_t* top) const;
    bool get_size_in_pixels(int32_t* w, int32_t* h) const;
    float get_pixel_density() const;
    
    bool get_min_size(int32_t* w, int32_t* h) const;
    bool get_max_size(int32_t* w, int32_t* h) const;
    
    bool set_min_size(int32_t w, int32_t h);
    bool set_max_size(int32_t w, int32_t h);
    
    float get_aspect_ratio() const;
    float get_locked_aspect_ratio() const;
    bool lock_aspect_ratio(float aspect_ratio);

    //=============================================================================
    // safe area
    //=============================================================================

    bool set_safe_area(const math::recti& area);
    math::recti get_safe_area() const;
    
    //=============================================================================
    // bordered
    //=============================================================================
    
    bool set_bordered(bool bordered);
    bool is_bordered() const;
    
    //=============================================================================
    // always on top
    //=============================================================================
    
    bool set_always_on_top(bool always_on_top);
    bool is_always_on_top() const;
    
    //=============================================================================
    // visibility
    //=============================================================================
    
    bool show();
    bool hide();
    bool is_visible() const;
    bool is_hidden() const;

    bool is_occluded() const;
    bool is_exposed() const;

    //=============================================================================
    // minimize / maximize / restore
    //=============================================================================
    
    bool minimize();
    bool is_minimized() const;
    
    bool maximize();
    bool is_maximized() const;
    
    bool restore();
    bool raise();

    //=============================================================================
    // system
    //=============================================================================
    
    bool flash(window_flash_op operation);

    //=============================================================================
    // fullscreen
    //=============================================================================

    display_id get_display(bool ignore_pending_display_id) const;
    
    bool is_fullscreen() const;
    bool is_fullscreen_visible() const;
    
    const display_mode& get_fullscreen_mode() const;
    bool set_fullscreen_mode(const display_mode* mode);  

    bool set_fullscreen(bool fullscreen);

    enum class fullscreen_op
    {
        leave = 0,
        enter = 1,
        update
    };

    enum class fullscreen_result
    {
        failed,
        succeeded,
        pending
    };
    
    bool update_fullscreen_mode(fullscreen_op fullscreen, bool commit);

    //=============================================================================
    // icon
    //=============================================================================
    
    bool set_icon(const surface_argb& surf);
    const surface_argb& get_icon() const;

    //=============================================================================
    // appearence
    //=============================================================================

    bool set_shape(const surface_argb& shape);
    const surface_argb& get_shape();
    bool update_shape(bool force);

    float get_opacity() const;
    bool set_opacity(float opacity);

    std::vector<uint8_t> get_icc_profile() const;
    pixel::pixel_format get_pixel_format() const;

    //=============================================================================
    // grab
    //=============================================================================

    bool get_mouse_grab() const;
    bool get_keyboard_grab() const;

    bool set_mouse_grab(bool grabbed);
    bool set_keyboard_grab(bool grabbed);

    void clear_grab();
    void update_grab();

    //=============================================================================
    // focus
    //=============================================================================

    bool set_focusable(bool focusable);
    bool has_mouse_focus() const;
    bool has_keyboard_focus() const;
    bool should_minimize_on_focus_loss() const;

    //=============================================================================
    // mouse
    //=============================================================================

    math::recti get_mouse_rect() const;
    bool set_mouse_rect(const math::recti& rect);

    //=============================================================================
    // drag and drop
    //=============================================================================

    bool toggle_drag_and_drop(bool enabled);

    //=============================================================================
    // text input
    //=============================================================================

    bool start_text_input();
    bool start_text_input(const keyboard::text_input_options* options);
    bool text_input_active() const;
    void stop_text_input();

    bool clear_composition();

    bool set_text_input_area(const math::recti& rect, int cursor);
    bool get_text_input_area(math::recti* rect, int* cursor) const;
    
    //=============================================================================
    // events
    //=============================================================================

    void send_wakeup_event() const;
    
    bool send_shown();
    void on_shown();
    
    bool send_hidden();
    void on_hidden();

    bool send_occluded();
    void on_occluded() {}

    bool send_exposed();
    void on_exposed() {}
    
    bool send_moved(int32_t x, int32_t y);
    void on_moved();
    
    bool send_resized(int32_t w, int32_t h);
    void on_resized();

    void check_pixel_size_changed();
    bool send_pixel_size_changed(int32_t w, int32_t h);
    void on_pixel_size_changed();

    void check_display_scale_changed();
    bool send_display_scale_changed();
    void on_display_scale_changed() {}

    bool send_safe_area_changed(const math::recti& area);
    void on_safe_area_changed() {}
    
    bool send_minimized();
    void on_minimized();
    
    bool send_maximized();
    void on_maximized() {}
    
    bool send_restored();
    void on_restored();
    
    bool send_enter_fullscreen();
    void on_enter_fullscreen() {}

    bool send_leave_fullscreen();
    void on_leave_fullscreen() {}
    
    bool send_gained_focus();
    void on_gained_focus();

    bool send_lost_focus();
    void on_lost_focus();

    bool send_mouse_enter();
    void on_mouse_enter();

    bool send_mouse_leave();
    void on_mouse_leave() {}
    
    void check_display_changed();
    bool send_display_changed(display_id d);
    void on_display_changed(display_id d);
    
    bool send_close_requested();
    void on_close_requested();

    bool send_destroyed();
    void on_destroyed() {}

    //=============================================================================
    // data
    //=============================================================================

    video_instance* video = nullptr;
    window_data data;
    std::unique_ptr<window_instance_impl> impl_ptr;
};

} // namespace video
} // namespace app
} // namespace vx