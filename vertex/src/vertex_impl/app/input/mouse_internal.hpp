#pragma once

#include "vertex/app/input/mouse.hpp"
#include "vertex/app/video/video.hpp"
#include "vertex/util/time.hpp"
#include "vertex_impl/app/app_internal.hpp"

namespace vx {
namespace app {

namespace video { class video_instance; }
namespace video { class window_instance; }

namespace mouse {

//=============================================================================
// forward declares
//=============================================================================

class mouse_manager;
class mouse_instance_impl;

class cursor_instance;
class cursor_instance_impl;

//=============================================================================
// cursor
//=============================================================================

struct cursor_data
{
    cursor_id id = invalid_id;
    cursor_shape shape = cursor_shape::default_;
    int hot_x = 0;
    int hot_y = 0;
};

//=============================================================================

class cursor_instance_impl;

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
// input source
//=============================================================================

// Per-button click tracking for timing and movement thresholds

struct click_state
{
    // Timestamp of the most recent click
    time::time_point last_click_time;
    // Motion accumulated since the last button
    // press, used for distinguishing taps vs. drags
    double motion_x = 0.0;
    double motion_y = 0.0;
    // Number of clicks detected in succession (e.g., double-click)
    uint8_t click_count = 0;
};

struct input_source_data
{
    // Logical mouse ID this device contributes to
    mouse_id id = invalid_id;
    // Current aggregate button state for this device
    button button_state = button::none;
    // Per-button tracked click_state entries
    std::vector<click_state> click_states;
};

struct input_source
{
    click_state* get_click_state(uint8_t button);
    input_source_data data;
};

//=============================================================================
// mouse info
//=============================================================================

struct mouse_info
{
    mouse_id id = invalid_id;
    std::string name;
};

//=============================================================================
// mouse data
//=============================================================================

// integer_mode: Controls rounding behavior for motion/scroll events,
// enabling integer-only event generation for systems or applications that
// require pixel/line quantization.

VX_FLAGS_UT_DECLARE_BEGIN(integer_mode, uint8_t)
{
    none    = 0,
    motion  = VX_BIT(0),
    scroll  = VX_BIT(1)
}
VX_FLAGS_DECLARE_END(integer_mode)

//=============================================================================

struct mouse_manager_data
{
    // Window focus
    video::window_id focus = invalid_id;

    // Position
    float x = 0.0f;
    float y = 0.0f;                     // current position in window space
    float last_x = 0.0f;
    float last_y = 0.0f;                // last recorded position
    float x_accu = 0.0f;
    float y_accu = 0.0f;                // accumulated subpixel motion
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
    bool auto_capture = false;                  // allow OS auto capture
    bool capture_desired = false;               // app requested capture
    video::window_id capture_window = invalid_id;

    // Event translation toggles
    bool touch_mouse_events = false;            // touch -> mouse
    bool mouse_touch_events = false;            // mouse -> touch
    bool pen_mouse_events = false;              // pen -> mouse
    bool pen_touch_events = false;              // pen -> touch
    bool was_touch_mouse_events = false;
    bool added_mouse_touch_device = false;
    bool added_pen_touch_device = false;
    bool track_mouse_down = false;

    // Integer mode
    integer_mode int_mode = integer_mode::none;
    float integer_mode_residual_motion_x = 0.0f;
    float integer_mode_residual_motion_y = 0.0f;
    float integer_mode_residual_scroll_x = 0.0f;
    float integer_mode_residual_scroll_y = 0.0f;

    // Devices
    std::vector<mouse_info> mice;             // logical mice

    // Each input source tracks per-button click state. Buttons are treated as
    // zero-indexed, and the click_state array grows on demand when a button index
    // is requested. This allows arbitrary button indices to be queried without
    // requiring a fixed-size button map.
    std::vector<input_source> sources;        // physical devices

    // Cursor
    std::vector<cursor_instance> cursors;
    id_generator cursor_id_generator;
    cursor_id default_cursor = invalid_id;
    cursor_id current_cursor = invalid_id;
    bool cursor_visible = false;
};

//=============================================================================
// mouse internal
//=============================================================================

class mouse_manager
{
public:

    mouse_manager();
    ~mouse_manager();

    mouse_manager(const mouse_manager&) = delete;
    mouse_manager& operator=(const mouse_manager&) = delete;

    mouse_manager(mouse_manager&&) noexcept = delete;
    mouse_manager& operator=(mouse_manager&&) noexcept = delete;

public:

    //=============================================================================
    // touch instance
    //=============================================================================

    bool init(video::video_instance* owner);
    void quit();

    //=============================================================================
    // devices
    //=============================================================================

    size_t get_mouse_index(mouse_id id) const;
    mouse_info* get_mouse(mouse_id id);
    const mouse_info* get_mouse(mouse_id id) const;

    void add_mouse(mouse_id id, const char* name);
    void remove_mouse(mouse_id id);

    static bool is_mouse(uint16_t, uint16_t) { return true; }
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
    bool update_focus(video::window_instance* w, float x, float y, bool send_mouse_motion);

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

    bool set_capture(bool enabled);
    bool update_capture(bool force_release);

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

    cursor_id add_cursor(cursor_instance& c);
    void remove_cursor(cursor_id id);

    const cursor_instance* get_cursor_instance(cursor_id id) const;

    cursor_id create_cursor(const pixel::mask_pair& mask, int hot_x, int hot_y);
    cursor_id create_color_cursor(const argb_surface& surf, int hot_x, int hot_y);
    cursor_id create_system_cursor(cursor_shape shape);

    cursor_id get_cursor() const;
    bool set_cursor(cursor_id cid);

    cursor_id get_default_cursor() const;
    void set_default_cursor(cursor_id cid);

    void create_dummy_cursor();
    cursor_shape get_default_system_cursor() const;

    // visibility
    bool show_cursor();
    bool hide_cursor();
    bool cursor_visible() const;
    void redraw_cursor();

    //=============================================================================
    // Data
    //=============================================================================

    video::video_instance* video = nullptr;
    mouse_manager_data data;
    std::unique_ptr<mouse_instance_impl> impl_ptr;
};

} // namespace mouse
} // namespace app
} // namespace vx
