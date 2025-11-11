#pragma once

#include "vertex/app/input/mouse.hpp"
#include "vertex/app/video/video.hpp"
#include "vertex/util/time.hpp"

namespace vx {
namespace app {

namespace video { class video_instance; }
namespace video { class window_instance; }

namespace mouse {

//=============================================================================
// forward declares
//=============================================================================

class mouse_instance;
class mouse_instance_impl;

class cursor_instance;
class cursor_instance_impl;

//=============================================================================
// cursor
//=============================================================================

class cursor_data
{
    cursor_id id;
};

//=============================================================================

class cursor_instance
{
public:

    cursor_instance();
    ~cursor_instance();

    cursor_instance(cursor_instance&&) noexcept;
    cursor_instance& operator=(cursor_instance&&) noexcept;

public:

    cursor_data data;
    std::unique_ptr<cursor_instance_impl> impl_ptr;
};

//=============================================================================
// mouse data
//=============================================================================

struct click_state
{
    time::time_point last_click_time;
    double motion_x;
    double motion_y;
    uint8_t click_count;
};

//=============================================================================

struct input_source_data
{
    mouse_id id;
    button button_state;
    std::vector<click_state> click_states;
};

class input_source
{
public:

    click_state* get_click_state(uint8_t button);

public:

    input_source_data data;
};

//=============================================================================

struct mouse_info
{
    mouse_id id;
    std::string name;
};

//=============================================================================

struct mouse_cursor_data
{
    std::vector<cursor_instance> cursors;
    cursor_id default_cursor = invalid_id;
    cursor_id current_cursor = invalid_id;
    bool visible = false;
};

//=============================================================================

VX_FLAGS_UT_DECLARE_BEGIN(integer_mode, uint8_t)
{
    none    = 0,
    motion  = VX_BIT(0),
    scroll  = VX_BIT(1)
}
VX_FLAGS_DECLARE_END(integer_mode)

//=============================================================================

struct mouse_data
{
    bool quitting = false;

    // Window focus
    video::window_id focus = invalid_id;

    // Position
    float x = 0.0f, y = 0.0f;           // current position in window space
    float last_x = 0.0f, last_y = 0.0f; // last recorded position
    float x_accu = 0.0f, y_accu = 0.0f; // accumulated subpixel motion
    bool has_position = false;          // true if position valid

    // Click motion thresholding
    double click_motion_x = 0.0;
    double click_motion_y = 0.0;

    // Double click settings
    time::time_point double_click_time;
    int double_click_radius = 0;

    // Relative mode
    bool relative_mode_enabled = false;         // is relative mode active
    bool relative_warp_motion = false;          // generate motion events on warp
    bool relative_hide_cursor = false;          // hide cursor while active
    bool relative_center = false;               // lock to window center

    // Warp enumlation
    bool warp_emulation_hint = false;           // hint for warp emulation
    bool warp_emulation_active = false;
    bool warp_emulation_prohibited = false;
    time::time_point last_center_warp_time;

    // Scaling
    bool normal_speed_scale_enabled = false;
    bool relative_speed_scale_enabled = false;
    bool relative_system_scale_enabled = false;
    float normal_speed_scale = 0.0f;
    float relative_speed_scale = 0.0f;

    // Capture settings
    bool auto_capture = false;        // allow OS auto capture
    bool capture_desired = false;     // app requested capture
    video::window_id capture_window = invalid_id;

    // Event translation toggles
    bool touch_mouse_events = false;  // touch -> mouse
    bool mouse_touch_events = false;  // mouse -> touch
    bool pen_mouse_events = false;    // pen -> mouse
    bool pen_touch_events = false;    // pen -> touch
    bool was_touch_mouse_events = false;
    bool added_mouse_touch_device = false;
    bool added_pen_touch_device = false;
    bool track_mouse_down = false;

    // Integer mode
    integer_mode integer_mode = integer_mode::none;
    float integer_mode_residual_motion_x = 0.0f;
    float integer_mode_residual_motion_y = 0.0f;
    float integer_mode_residual_scroll_x = 0.0f;
    float integer_mode_residual_scroll_y = 0.0f;

    // mice tracker
    std::vector<mouse_info> mice;

    // Input sources (physical devices)
    std::vector<input_source> sources;

    // Cursor
    mouse_cursor_data cursors;
    bool cursor_visible = true;
};

//=============================================================================
// mouse internal
//=============================================================================

class mouse_instance
{
public:

    mouse_instance();
    ~mouse_instance();

    mouse_instance(const mouse_instance&) = delete;
    mouse_instance& operator=(const mouse_instance&) = delete;

    mouse_instance(mouse_instance&&) noexcept = delete;
    mouse_instance& operator=(mouse_instance&&) noexcept = delete;

public:

    //=============================================================================
    // lifetime
    //=============================================================================

    bool init(video::video_instance* owner);
    void quit();

    bool init_impl();
    void quit_impl();

    //=============================================================================
    // devices
    //=============================================================================

    size_t get_mouse_index(mouse_id id) const;
    const mouse_info* get_mouse(mouse_id id) const;
    mouse_info* get_mouse(mouse_id id);

    void add_mouse(mouse_id id, const char* name);
    void remove_mouse(mouse_id id);

    bool is_mouse(uint16_t vendor, uint16_t product) const { return true; }
    bool any_connected() const;
    std::vector<mouse_id> list_mice() const;
    const char* get_name(mouse_id id = default_mouse_id) const;

    input_source* get_input_source(mouse_id id, bool down, uint8_t button);

    //=============================================================================
    // focus
    //=============================================================================

    video::window_id get_focus() const;
    video::window_instance* get_focus_instance();

    void set_focus(video::window_id wid);
    bool update_focus(video::window_instance* w, float x, float y, button button_state, bool send_mouse_motion);

    //=============================================================================
    // state
    //=============================================================================

    button get_button_state(mouse_id id, bool include_touch) const;

    button get_state(float* x, float* y) const;
    button get_relative_state(float* x, float* y);
    button get_global_state(float* x, float* y) const;

    //=============================================================================
    // position
    //=============================================================================

    bool is_position_in_window(const video::window_instance* w, float x, float y);

    void warp_in_window(video::window_instance* w, float x, float y);
    void warp_in_window_internal(video::window_instance* w, float x, float y);

    bool warp_global(float x, float y);

    //=============================================================================
    // Relative Mode
    //=============================================================================

    void maybe_enable_warp_emulation(const video::window_instance* w, float x, float y);
    void disable_warp_emulation();

    bool set_relative_mode(bool enabled);
    bool get_relative_mode() const;
    bool update_relative_mode();

    //=============================================================================
    // Capture
    //=============================================================================

    bool set_capture(bool enabled) { return false; }
    bool update_capture(bool force_release) { return false; }

    void constrain_position(const video::window_instance* w, float* x, float* y) const;

    //=============================================================================
    //  events
    //=============================================================================

    void send_mouse_added(mouse_id id);
    void send_mouse_removed(mouse_id id);

    void send_motion(time::time_point t, video::window_instance* w, mouse_id id, bool relative, float x, float y);
    void send_motion_internal(time::time_point t, video::window_instance* w, mouse_id id, bool relative, float x, float y);

    void send_button(time::time_point t, video::window_instance* w, mouse_id id, uint8_t b, bool down);
    void send_button_internal(time::time_point t, video::window_instance* w, mouse_id id, uint8_t b, bool down, int clicks);

    void send_button_clicks(time::time_point t, video::window_instance* w, mouse_id id, uint8_t b, bool down, int clicks);
    void send_wheel(time::time_point t, const video::window_instance* w, mouse_id id, float x, float y, wheel_direction direction);

    //=============================================================================
    // cursor
    //=============================================================================

    // lifecycle
    cursor_id create_cursor(const pixel::bitmask& mask, const math::vec2i& hotspot);
    cursor_id create_color_cursor(const pixel::surface_rgba8& surf, const math::vec2i& hotspot);
    cursor_id create_system_cursor(cursor_shape id);

    // info
    cursor_shape get_shape(cursor_id id);
    math::vec2i get_hotspot(cursor_id id);

    // active cursor
    bool set_cursor(cursor_id id);
    cursor_id get_cursor();
    cursor_id get_default_cursor();
    void set_default_cursor(const cursor& cursor);
    cursor get_default_system_cursor();

    // visibility
    bool show_cursor();
    bool hide_cursor();
    bool cursor_visible();
    void redraw_cursor() {}

    //=============================================================================
    // Data
    //=============================================================================

    video::video_instance* video = nullptr;
    mouse_data data;
    std::unique_ptr<mouse_instance_impl> impl_ptr;
};

} // namespace mouse
} // namespace app
} // namespace vx