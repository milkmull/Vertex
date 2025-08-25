#pragma once

#include "vertex/pixel/mask.hpp"

namespace vx {
namespace app {
namespace mouse {

///////////////////////////////////////////////////////////////////////////////
// mouse
///////////////////////////////////////////////////////////////////////////////

namespace _priv { class mouse_internal; }
using mouse_id = size_t;

///////////////////////////////////////////////////////////////////////////////
// mouse state
///////////////////////////////////////////////////////////////////////////////

enum mouse_button
{
    NONE            = 0,

    BUTTON_LEFT     = (1 << 1),
    BUTTON_RIGHT    = (1 << 2),
    BUTTON_MIDDLE   = (1 << 3),

    BUTTON_EXTRA_1  = (1 << 4),
    BUTTON_EXTRA_2  = (1 << 5)
};

enum mouse_wheel_direction
{
    VERTICAL,
    HORIZONTAL
};

struct mouse_state
{
    mouse_button button_flags;
    float x, y;

    inline bool left_button_down() const noexcept { return button_flags & mouse_button::BUTTON_LEFT; }
    inline bool right_button_down() const noexcept { return button_flags & mouse_button::BUTTON_RIGHT; }
    inline bool middle_button_down() const noexcept { return button_flags & mouse_button::BUTTON_MIDDLE; }
    inline bool extra_1_button_down() const noexcept { return button_flags & mouse_button::BUTTON_EXTRA_1; }
    inline bool extra_2_button_down() const noexcept { return button_flags & mouse_button::BUTTON_EXTRA_2; }
};

///////////////////////////////////////////////////////////////////////////////
// mouse
///////////////////////////////////////////////////////////////////////////////

VX_API bool has_mouse();
VX_API std::vector<mouse_id> list_mice_ids();
VX_API const char* get_mouse_name(mouse_id id);
VX_API video::window* get_mouse_focus();

VX_API mouse_state get_mouse_state();
VX_API mouse_state get_global_mouse_state();
VX_API mouse_state get_relative_mouse_state();

VX_API void warp_mouse_in_window(const video::window* w, float x, float y);
VX_API void warp_mouse_global(float x, float y);

VX_API bool set_window_relative_mouse_mode(video::window* w, bool enabled);
VX_API bool get_window_relative_mouse_mode(const video::window* w);

VX_API bool capture_mouse(bool enabled);

///////////////////////////////////////////////////////////////////////////////
// cursor
///////////////////////////////////////////////////////////////////////////////

enum system_cursor
{
    SHAPE_ARROW,
    SHAPE_IBEAM,
    SHAPE_WAIT,
    SHAPE_CROSSHAIR,
    SHAPE_HAND,
    SHAPE_HRESIZE,
    SHAPE_VRESIZE,
    SHAPE_ALL_RESIZE,
    SHAPE_NOT_ALLOWED,
};

class cursor;

VX_API const cursor* create_cursor(const pixel::bitmask& mask, int hot_x, int hot_y);
VX_API const cursor* create_color_cursor(const pixel::surface_rgba8& surf, int hot_x, int hot_y);
VX_API const cursor* create_system_cursor(system_cursor id);

VX_API void destroy_cursor(cursor* c);

VX_API bool set_cursor(const cursor* c);
VX_API const cursor* get_cursor();
VX_API const cursor* get_default_cursor();

VX_API bool show_cursor();
VX_API bool hide_cursor();
VX_API bool cursor_visible();

} // namespace mouse
} // namespace app
} // namespace vx