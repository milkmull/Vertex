#pragma once

#include "vertex/app/id.hpp"
#include "vertex/pixel/mask.hpp"

namespace vx {
namespace app {

namespace video { class window; }

namespace mouse {

///////////////////////////////////////////////////////////////////////////////
// mouse id
///////////////////////////////////////////////////////////////////////////////

using mouse_id = id_type;
enum : mouse_id { DEFAULT_MOUSE_ID = DEFAULT_ID };

///////////////////////////////////////////////////////////////////////////////
// mouse state
///////////////////////////////////////////////////////////////////////////////

enum button
{
    NONE            = 0,

    BUTTON_LEFT     = (1 << 0),
    BUTTON_RIGHT    = (1 << 1),
    BUTTON_MIDDLE   = (1 << 2),

    BUTTON_EXTRA_1  = (1 << 3),
    BUTTON_EXTRA_2  = (1 << 4)
};

enum wheel_direction
{
    VERTICAL,
    HORIZONTAL
};

struct mouse_state
{
    button button_flags;
    float x, y;

    bool left_button_down() const noexcept { return button_flags & button::BUTTON_LEFT; }
    bool right_button_down() const noexcept { return button_flags & button::BUTTON_RIGHT; }
    bool middle_button_down() const noexcept { return button_flags & button::BUTTON_MIDDLE; }
    bool extra_1_button_down() const noexcept { return button_flags & button::BUTTON_EXTRA_1; }
    bool extra_2_button_down() const noexcept { return button_flags & button::BUTTON_EXTRA_2; }
};

///////////////////////////////////////////////////////////////////////////////
// mouse API
///////////////////////////////////////////////////////////////////////////////

// Devices
VX_API bool any_connected();
VX_API std::vector<mouse_id> list_ids();
VX_API const char* get_name(mouse_id id = DEFAULT_MOUSE_ID);

// Focus
VX_API const video::window* get_focus();

// State
VX_API mouse_state get_state();            // buttons + pos relative to focus
VX_API mouse_state get_global_state();     // buttons + global pos
VX_API mouse_state get_relative_state();   // buttons + relative motion since last call

// Position
VX_API void set_position_in_window(const video::window* w, const math::vec2& position);
VX_API void set_position_global(const math::vec2& position);

// Relative mode
VX_API bool set_relative_mode(const video::window* w, bool enabled);
VX_API bool relative_mode_enabled(const video::window* w);

// Capture
VX_API bool set_capture(bool enabled);

///////////////////////////////////////////////////////////////////////////////
// cursor
///////////////////////////////////////////////////////////////////////////////

namespace _priv { class cursor_impl; }

using cursor_id = id_type;

enum cursor_shape
{
    CURSOR_SHAPE_USER_DEFINED,
    CURSOR_SHAPE_ARROW,
    CURSOR_SHAPE_IBEAM,
    CURSOR_SHAPE_WAIT,
    CURSOR_SHAPE_CROSSHAIR,
    CURSOR_SHAPE_HAND,
    CURSOR_SHAPE_HRESIZE,
    CURSOR_SHAPE_VRESIZE,
    CURSOR_SHAPE_ALL_RESIZE,
    CURSOR_SHAPE_NOT_ALLOWED,
};

// Creation / Destruction
VX_API cursor_id create_cursor(const pixel::bitmask& mask, const math::vec2i& hotspot);
VX_API cursor_id create_color_cursor(const pixel::surface_rgba8& surf, const math::vec2i& hotspot);
VX_API cursor_id create_system_cursor(cursor_shape id);

// Cursor Info
VX_API cursor_shape get_shape(cursor_id id);
VX_API math::vec2i get_hotspot(cursor_id id);

// Active cursor
VX_API bool set_cursor(cursor_id id);
VX_API cursor_id get_cursor();
VX_API cursor_id get_default_cursor();

// Visibility
VX_API bool show_cursor();
VX_API bool hide_cursor();
VX_API bool cursor_visible();

} // namespace mouse
} // namespace app
} // namespace vx