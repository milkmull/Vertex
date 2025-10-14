#pragma once

#include "vertex/app/input/mouse.hpp"
#include "vertex/app/video/video.hpp"
#include "vertex/app/owner_ptr.hpp"
#include "vertex/util/time.hpp"

namespace vx {
namespace app {

namespace video { class video_instance; }

namespace mouse {

///////////////////////////////////////////////////////////////////////////////
// cursor
///////////////////////////////////////////////////////////////////////////////

class cursor_instance_impl;

struct cursor_instance_impl_deleter
{
    void operator()(cursor_instance_impl* ptr) const noexcept;
};

class cursor_data
{
    cursor_id id;
};

class cursor_instance
{
public:

    cursor_data data;
    owner_ptr<cursor_instance_impl, cursor_instance_impl_deleter> impl_ptr;
};

///////////////////////////////////////////////////////////////////////////////
// mouse data
///////////////////////////////////////////////////////////////////////////////

struct click_state
{
    time::time_point last_click_time;
    double motion_x;
    double motion_y;
    uint8_t click_count;
};

struct input_source
{
    mouse_id id;
    buttons button_state;
    // probably can put a cap on this in the future
    std::vector<click_state> click_states;
};

class mouse_info
{
public:

    mouse_id id;
    std::string name;
};

struct relative_mode_state
{
    bool enabled = false;                     // is relative mode active
    bool warp_motion = false;                 // generate motion events on warp
    bool hide_cursor = false;                 // hide cursor while active
    bool center = false;                      // lock to window center
    bool warp_emulation_hint = false;         // hint for warp emulation
    bool warp_emulation_active = false;
    bool warp_emulation_prohibited = false;
    time::time_point last_warp_time;
    time::time_point last_center_warp_time;
};

struct scale_state
{
    bool normal_enabled = false;
    float normal_scale = 0.0f;
    bool relative_enabled = false;
    float relative_scale = 0.0f;
    bool system_relative_enabled = false;
};

struct mouse_cursor_data
{
    std::vector<cursor_instance> cursors;
    cursor_id default_cursor = INVALID_ID;
    cursor_id current_cursor = INVALID_ID;
    bool visible = false;
};

struct mouse_data
{
    bool quitting = false;

    // Window focus
    video::window_id focus = INVALID_ID;

    // Position
    float x = 0.0f, y = 0.0f;           // current position in window space
    float last_x = 0.0f, last_y = 0.0f; // previous frame position
    float x_accu = 0.0f, y_accu = 0.0f; // accumulated subpixel motion
    bool has_position = false;          // true if position valid

    // Scroll
    float residual_scroll_x = 0.0f;
    float residual_scroll_y = 0.0f;

    // Click motion thresholding
    double click_motion_x = 0.0;
    double click_motion_y = 0.0;
    time::time_point double_click_time;
    int double_click_radius = 0;

    // mice tracker
    std::vector<mouse_info> mice;

    // Input sources (physical devices)
    std::vector<input_source> sources;

    // Modes & settings
    relative_mode_state relative_mode;
    scale_state scaling;

    // Capture settings
    bool auto_capture = false;        // allow OS auto capture
    bool capture_desired = false;     // app requested capture
    video::window* capture_window = nullptr;

    // Event translation toggles
    bool touch_mouse_events = false;  // touch -> mouse
    bool mouse_touch_events = false;  // mouse -> touch
    bool pen_mouse_events = false;    // pen -> mouse
    bool pen_touch_events = false;    // pen -> touch
    bool was_touch_mouse_events = false;
    bool added_mouse_touch_device = false;
    bool added_pen_touch_device = false;

    mouse_cursor_data cursors;
};

///////////////////////////////////////////////////////////////////////////////
// mouse internal
///////////////////////////////////////////////////////////////////////////////

class mouse_instance
{
public:

    bool init(video::video_instance* owner);
    void quit();

    //-------------------------------------------------------------------------
    // Device Management
    //-------------------------------------------------------------------------

    size_t get_mouse_index(mouse_id id) const;
    const mouse_info* get_mouse(mouse_id id) const;
    mouse_info* get_mouse(mouse_id id);

    void add_mouse(mouse_id id, const char* name);      ///< Register a new mouse
    void remove_mouse(mouse_id id);                     ///< Remove a mouse

    bool is_mouse(uint16_t vendor, uint16_t product) const { return true; }       ///< Identify device by VID/PID
    bool any_connected() const;                                                   ///< True if any mouse is connected
    std::vector<mouse_id> list_mice() const;                                       ///< List connected mouse IDs
    const char* get_name(mouse_id id = DEFAULT_MOUSE_ID) const;                   ///< Get mouse name

    //-------------------------------------------------------------------------
    // Focus (window association)
    //-------------------------------------------------------------------------

    video::window_id get_focus() const;             ///< Window currently receiving mouse focus
    void set_focus(video::window_id w);       ///< Set the active focus window

    //-------------------------------------------------------------------------
    // State (buttons + position)
    //-------------------------------------------------------------------------

    buttons get_button_state(mouse_id id, bool include_touch) const;

    buttons get_state(float* x, float* y) const;            ///< State relative to focus window
    buttons get_relative_state(float* x, float* y);   ///< Relative motion since last call
    buttons get_global_state(float* x, float* y) const;     ///< State in global desktop coordinates

    //-------------------------------------------------------------------------
    // Position control
    //-------------------------------------------------------------------------

    void set_position_in_window(video::window_id w, const math::vec2& position); ///< Move mouse inside a window
    void set_position_global(const math::vec2& position);                            ///< Move mouse in global space

    //-------------------------------------------------------------------------
    // Relative Mode
    //-------------------------------------------------------------------------

    bool set_relative_mode(video::window_id w, bool enabled); ///< Enable/disable relative mode for a window
    bool relative_mode_enabled(video::window_id w);           ///< Check if relative mode is active
    void update_relative_mouse_mode();                           ///< Internal update of relative mode state
    void disable_mouse_warp_emulation();                         ///< Disable warp emulation workaround

    //-------------------------------------------------------------------------
    // Capture
    //-------------------------------------------------------------------------

    bool set_capture(bool enabled);                  ///< Enable/disable capture
    bool update_mouse_capture(bool force_release);   ///< Refresh capture (force release if needed)

    //-------------------------------------------------------------------------
    // Event dispatch (internal)
    //-------------------------------------------------------------------------

    void send_mouse_added(mouse_id id) {}
    void send_mouse_removed(mouse_id id) {}
    void send_mouse_motion(const video::window* w, mouse_id id, bool relative, float x, float y);
    void send_mouse_button(const video::window* w, mouse_id id, buttons b, bool down);
    void send_mouse_button_clicks(const video::window* w, mouse_id id, buttons b, bool down, int clicks);
    void send_mouse_wheel(const video::window* w, mouse_id id, float x, float y, wheel_direction direction);
    void perform_warp_mouse_in_window(const video::window* w, float x, float y, bool ignore_relative_mode);

    //-------------------------------------------------------------------------
    // Cursor Management
    //-------------------------------------------------------------------------

    // -- Creation & Destruction
    cursor_id create_cursor(const pixel::bitmask& mask, const math::vec2i& hotspot);
    cursor_id create_color_cursor(const pixel::surface_rgba8& surf, const math::vec2i& hotspot);
    cursor_id create_system_cursor(cursor_shape id);

    // -- Cursor Info
    cursor_shape get_shape(cursor_id id);
    math::vec2i get_hotspot(cursor_id id);

    // -- Active cursor
    bool set_cursor(cursor_id id);
    cursor_id get_cursor();
    cursor_id get_default_cursor();
    void set_default_cursor(const cursor& cursor);
    cursor get_default_system_cursor();

    // -- Visibility
    bool show_cursor();
    bool hide_cursor();
    bool cursor_visible();
    void redraw_cursor() {}

    //-------------------------------------------------------------------------
    // Data
    //-------------------------------------------------------------------------

    video::video_instance* video = nullptr;
    mouse_data data;
};

} // namespace mouse
} // namespace app
} // namespace vx