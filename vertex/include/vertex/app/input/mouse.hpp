#pragma once

#include "vertex/config/flags.hpp"
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

enum : mouse_id
{
    global_mouse_id = 1,
    default_mouse_id = 2,
    touch_mouse_id = max_id,
    pen_mouse_id = max_id - 1
};

///////////////////////////////////////////////////////////////////////////////
// mouse state
///////////////////////////////////////////////////////////////////////////////

VX_FLAGS_UT_DECLARE_BEGIN(button, uint8_t)
{
    none         = 0,
    left         = VX_BIT(0),
    right        = VX_BIT(1),
    middle       = VX_BIT(2),
    extra_1      = VX_BIT(3),
    extra_2      = VX_BIT(4)
}
VX_FLAGS_DECLARE_END(button)

enum : size_t { button_count = 5 };

inline constexpr button button_mask(uint8_t x) noexcept
{
    return static_cast<button>(1u << (x - 1));
}

enum wheel_direction
{
    wheel_vertical,
    wheel_horizontal
};

struct mouse_state
{
    button button_flags;
    float x, y;

    bool left_button_down() const noexcept { return button_flags & button::left; }
    bool right_button_down() const noexcept { return button_flags & button::right; }
    bool middle_button_down() const noexcept { return button_flags & button::middle; }
    bool extra_1_button_down() const noexcept { return button_flags & button::extra_1; }
    bool extra_2_button_down() const noexcept { return button_flags & button::extra_2; }

    bool left_button_up() const noexcept { return !left_button_down(); }
    bool right_button_up() const noexcept { return !right_button_down(); }
    bool middle_button_up() const noexcept { return !middle_button_down(); }
    bool extra_1_button_up() const noexcept { return !extra_1_button_down(); }
    bool extra_2_button_up() const noexcept { return !extra_2_button_down(); }
};

///////////////////////////////////////////////////////////////////////////////
// mouse API
///////////////////////////////////////////////////////////////////////////////

// Devices
VX_API bool any_connected();
VX_API std::vector<mouse_id> list_ids();
VX_API const char* get_name(mouse_id id = default_mouse_id);

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

using cursor_id = id_type;

enum class cursor_shape
{
    user_defined,
    arrow,
    ibeam,
    wait,
    crosshair,
    hand,
    hresize,
    vresize,
    all_resize,
    not_allowed,
};

class cursor
{
public:

    // implicit on purpose: allows seamless conversion
    cursor(cursor_id id) : m_id(id) {}
    ~cursor() {}

public:

    // implicit conversion back to id
    operator cursor_id() const noexcept { return m_id; }

    cursor_id id() const noexcept { return m_id; }
    bool is_valid() const noexcept { return is_valid_id(m_id); }
    VX_API bool exists() const;

    VX_API cursor_shape get_shape() const;
    VX_API math::vec2i get_hotspot() const;

private:

    cursor_id m_id;
};

// Creation / Destruction
VX_API cursor_id create_cursor(const pixel::bitmask& mask, const math::vec2i& hotspot);
VX_API cursor_id create_color_cursor(const pixel::surface_rgba8& surf, const math::vec2i& hotspot);
VX_API cursor_id create_system_cursor(cursor_shape id);

// Active cursor
VX_API bool set_cursor(cursor c);
VX_API cursor_id get_cursor();
VX_API cursor_id get_default_cursor();

// Visibility
VX_API bool show_cursor();
VX_API bool hide_cursor();
VX_API bool cursor_visible();

} // namespace mouse
} // namespace app
} // namespace vx