#pragma once

#include "vertex/app/input/mouse.hpp"
#include "vertex/app/owner_ptr.hpp"
#include "vertex/util/time.hpp"

namespace vx {
namespace app {
namespace mouse {

///////////////////////////////////////////////////////////////////////////////
// cursor
///////////////////////////////////////////////////////////////////////////////

class cursor_internal;

struct cursor
{
    cursor_id id;
    owner_ptr<cursor_internal> internal;
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
    button button_state;
    // probably can put a cap on this in the future
    std::vector<click_state> click_states;
};

struct relative_mode_state
{
    bool enabled;                     // is relative mode active
    bool warp_motion;                 // generate motion events on warp
    bool hide_cursor;                 // hide cursor while active
    bool center;                      // lock to window center
    bool warp_emulation_hint;         // hint for warp emulation
    bool warp_emulation_active;
    bool warp_emulation_prohibited;
    time::time_point last_center_warp_time;
};

struct scale_state
{
    bool normal_enabled;
    float normal_scale;
    bool relative_enabled;
    float relative_scale;
    bool system_relative_enabled;
};

struct cursor_data
{
    std::vector<cursor> cursors;
    cursor_id default_cursor;
    cursor_id current_cursor;
    bool visible;
};

struct mouse_data
{
    // Window focus
    video::window* focus = nullptr;

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

    cursor_data cursors;
};

///////////////////////////////////////////////////////////////////////////////
// mouse internal
///////////////////////////////////////////////////////////////////////////////

class mouse_internal
{
public:

    static bool init();
    static bool is_init();
    static void quit();

    //-------------------------------------------------------------------------
    // Device Management
    //-------------------------------------------------------------------------

    static void add_mouse(mouse_id id, const char* name, bool send_event);      ///< Register a new mouse
    static void remove_mouse(mouse_id id, bool send_event);                     ///< Remove a mouse

    static bool is_mouse(uint16_t vendor, uint16_t product);                    ///< Identify device by VID/PID
    static bool any_connected();                                                ///< True if any mouse is connected
    static std::vector<mouse_id> list_ids();                                    ///< List connected mouse IDs
    static const char* get_name(mouse_id id = DEFAULT_MOUSE_ID);                ///< Get mouse name

    //-------------------------------------------------------------------------
    // Focus (window association)
    //-------------------------------------------------------------------------

    static const video::window* get_focus();         ///< Window currently receiving mouse focus
    static void set_focus(const video::window* w);   ///< Set the active focus window

    //-------------------------------------------------------------------------
    // State (buttons + position)
    //-------------------------------------------------------------------------

    static mouse_state get_state();            ///< State relative to focus window
    static mouse_state get_global_state();     ///< State in global desktop coordinates
    static mouse_state get_relative_state();   ///< Relative motion since last call

    //-------------------------------------------------------------------------
    // Position control
    //-------------------------------------------------------------------------

    static void set_position_in_window(const video::window* w, const math::vec2& position); ///< Move mouse inside a window
    static void set_position_global(const math::vec2& position);                            ///< Move mouse in global space

    //-------------------------------------------------------------------------
    // Relative Mode
    //-------------------------------------------------------------------------

    static bool set_relative_mode(const video::window* w, bool enabled); ///< Enable/disable relative mode for a window
    static bool relative_mode_enabled(const video::window* w);           ///< Check if relative mode is active
    static void update_relative_mouse_mode();                           ///< Internal update of relative mode state
    static void disable_mouse_warp_emulation();                         ///< Disable warp emulation workaround

    //-------------------------------------------------------------------------
    // Capture
    //-------------------------------------------------------------------------

    static bool set_capture(bool enabled);                  ///< Enable/disable capture
    static bool update_mouse_capture(bool force_release);   ///< Refresh capture (force release if needed)

    //-------------------------------------------------------------------------
    // Event dispatch (internal)
    //-------------------------------------------------------------------------

    static void send_mouse_motion(const video::window* w, mouse_id id, bool relative, float x, float y);
    static void send_mouse_button(const video::window* w, mouse_id id, button b, bool down);
    static void send_mouse_button_clicks(const video::window* w, mouse_id id, button b, bool down, int clicks);
    static void send_mouse_wheel(const video::window* w, mouse_id id, float x, float y, wheel_direction direction);
    static void perform_warp_mouse_in_window(const video::window* w, float x, float y, bool ignore_relative_mode);

    //-------------------------------------------------------------------------
    // Relative Mode
    //-------------------------------------------------------------------------

    static bool set_relative_mouse_mode(bool enabled);
    static bool get_relative_mouse_mode();
    static void update_relative_mouse_mode();
    static void disable_mouse_warp_emulation();

    //-------------------------------------------------------------------------
    // Cursor Management
    //-------------------------------------------------------------------------

    // -- Creation & Destruction
    static cursor_id create_cursor(const pixel::bitmask& mask, const math::vec2i& hotspot);
    static cursor_id create_color_cursor(const pixel::surface_rgba8& surf, const math::vec2i& hotspot);
    static cursor_id create_system_cursor(cursor_shape id);

    // -- Cursor Info
    static cursor_shape get_shape(cursor_id id);
    static math::vec2i get_hotspot(cursor_id id);

    // -- Active cursor
    static bool set_cursor(cursor_id id);
    static cursor_id get_cursor();
    static cursor_id get_default_cursor();
    static void set_default_cursor(const cursor& cursor);
    static cursor get_default_system_cursor();

    // -- Visibility
    static bool show_cursor();
    static bool hide_cursor();
    static bool cursor_visible();
    static void redraw_cursor();
};

} // namespace mouse
} // namespace app
} // namespace vx