#pragma once

#include "vertex/app/id.hpp"
#include "vertex/util/time.hpp"

#if defined(VX_APP_VIDEO_ENABLED)

#include "vertex/app/video/video.hpp"
#include "vertex/app/input/mouse.hpp"
#include "vertex/app/input/keyboard.hpp"
#include "vertex/app/input/touch.hpp"
#include "vertex/app/input/pen.hpp"

#endif // VX_APP_VIDEO_ENABLED


// https://www.glfw.org/docs/3.3/input_guide.html

namespace vx {
namespace app {
namespace event {

using event_type_t = uint32_t;

///////////////////////////////////////////////////////////////////////////////
// event category
///////////////////////////////////////////////////////////////////////////////

enum event_category : uint8_t
{
    category_none = 0,
    category_app,
    category_display,
    category_window,
    category_key,
    category_mouse,
    category_touch,
    category_pinch,
    category_pen,
    category_clipboard,
    category_drop,
    category_internal
};

///////////////////////////////////////////////////////////////////////////////
// event encoding
///////////////////////////////////////////////////////////////////////////////
//
// [31]    -> user-defined flag (1 bit)
// [30:24] -> category (7 bits, enough for 128 categories)
// [23:0]  -> event number/index (24 bits, up to ~16 million per category)
//
///////////////////////////////////////////////////////////////////////////////

namespace _priv {

enum : uint32_t
{
    user_flag_shift = 24,
    user_flag_mask  = 0xFF000000,  // top byte

    category_shift  = 16,
    category_mask   = 0x00FF0000,  // next byte

    number_shift    = 0,
    number_mask     = 0x0000FFFF   // low 16 bits
};

} // namespace _priv

inline constexpr event_type_t make_event(uint8_t category, uint16_t number, bool user_defined = false) noexcept
{
    return (static_cast<event_type_t>(user_defined ? 1u : 0u) << _priv::user_flag_shift) |
           (static_cast<event_type_t>(category) << _priv::category_shift) |
           static_cast<event_type_t>(number);
}

inline constexpr event_type_t make_user_event(uint8_t category, uint16_t number) noexcept
{
    return make_event(category, number, true);
}

///////////////////////////////////////////////////////////////////////////////
// decoding helpers
///////////////////////////////////////////////////////////////////////////////

inline constexpr bool is_user_event(event_type_t type) noexcept
{
    return static_cast<bool>(type & _priv::user_flag_mask);
}

inline constexpr uint8_t get_category(event_type_t type) noexcept
{
    return static_cast<uint8_t>((type >> _priv::category_shift) & _priv::category_mask);
}

inline constexpr uint16_t get_number(event_type_t type) noexcept
{
    return static_cast<uint16_t>(type & _priv::number_mask);
}

///////////////////////////////////////////////////////////////////////////////
// event type
///////////////////////////////////////////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/main/include/SDL3/SDL_events.h#L84

#define EVENT_TYPE(name, cat, i) name = make_event(cat, i)
#define EVENT_TYPE_USER(name, cat, i) name = make_event(cat, i, true)

enum event_type : event_type_t
{
    invalid_event = 0,

    // app events
    EVENT_TYPE(app_event_first,                 category_app,       0),
    EVENT_TYPE(app_quit,                        category_app,       1),
    EVENT_TYPE(app_terminating,                 category_app,       2),
    EVENT_TYPE(app_low_memory,                  category_app,       3),
    EVENT_TYPE(app_will_enter_background,       category_app,       4),
    EVENT_TYPE(app_did_enter_background,        category_app,       5),
    EVENT_TYPE(app_will_enter_foreground,       category_app,       6),
    EVENT_TYPE(app_did_enter_foreground,        category_app,       7),
    EVENT_TYPE(app_locale_changed,              category_app,       8),
    EVENT_TYPE(app_system_theme_changed,        category_app,       9),
    EVENT_TYPE(app_event_last,                  category_app,      10),

    // display events
    EVENT_TYPE(display_event_first,             category_display,   0),
    EVENT_TYPE(display_added,                   category_display,   1),
    EVENT_TYPE(display_removed,                 category_display,   2),
    EVENT_TYPE(display_moved,                   category_display,   3),
    EVENT_TYPE(display_orientation_changed,     category_display,   4),
    EVENT_TYPE(display_desktop_mode_changed,    category_display,   5),
    EVENT_TYPE(display_current_mode_changed,    category_display,   6),
    EVENT_TYPE(display_content_scale_changed,   category_display,   7),
    EVENT_TYPE(display_event_last,              category_display,   8),

    // window events
    EVENT_TYPE(window_event_first,              category_window,    0),
    EVENT_TYPE(window_created,                  category_window,    1),
    EVENT_TYPE(window_destroyed,                category_window,    2),
    EVENT_TYPE(window_shown,                    category_window,    3),
    EVENT_TYPE(window_hidden,                   category_window,    4),
    EVENT_TYPE(window_exposed,                  category_window,    5),
    EVENT_TYPE(window_occluded,                 category_window,    6),
    EVENT_TYPE(window_moved,                    category_window,    7),
    EVENT_TYPE(window_resized,                  category_window,    8),
    EVENT_TYPE(window_pixel_size_changed,       category_window,    9),
    EVENT_TYPE(window_minimized,                category_window,   10),
    EVENT_TYPE(window_maximized,                category_window,   11),
    EVENT_TYPE(window_restored,                 category_window,   12),
    EVENT_TYPE(window_enter_fullscreen,         category_window,   13),
    EVENT_TYPE(window_leave_fullscreen,         category_window,   14),
    EVENT_TYPE(window_gained_focus,             category_window,   15),
    EVENT_TYPE(window_lost_focus,               category_window,   16),
    EVENT_TYPE(window_mouse_enter,              category_window,   17),
    EVENT_TYPE(window_mouse_leave,              category_window,   18),
    EVENT_TYPE(window_hit_test,                 category_window,   19),
    EVENT_TYPE(window_display_changed,          category_window,   20),
    EVENT_TYPE(window_display_scale_changed,    category_window,   21),
    EVENT_TYPE(window_safe_area_changed,        category_window,   22),
    EVENT_TYPE(window_close_requested,          category_window,   23),
    EVENT_TYPE(window_event_last,               category_window,   24),

    // key events
    EVENT_TYPE(key_event_first,                 category_key,       0),
    EVENT_TYPE(keyboard_added,                  category_key,       1),
    EVENT_TYPE(keyboard_removed,                category_key,       2),
    EVENT_TYPE(key_down,                        category_key,       3),
    EVENT_TYPE(key_up,                          category_key,       4),
    EVENT_TYPE(text_editing,                    category_key,       5),
    EVENT_TYPE(text_input,                      category_key,       6),
    EVENT_TYPE(keymap_changed,                  category_key,       7),
    EVENT_TYPE(text_editing_candidates,         category_key,       8),
    EVENT_TYPE(key_event_last,                  category_key,       9),

    // mouse events
    EVENT_TYPE(mouse_event_first,               category_mouse,     0),
    EVENT_TYPE(mouse_added,                     category_mouse,     1),
    EVENT_TYPE(mouse_removed,                   category_mouse,     2),
    EVENT_TYPE(mouse_moved,                     category_mouse,     3),
    EVENT_TYPE(mouse_button_down,               category_mouse,     4),
    EVENT_TYPE(mouse_button_up,                 category_mouse,     5),
    EVENT_TYPE(mouse_wheel,                     category_mouse,     6),
    EVENT_TYPE(mouse_event_last,                category_mouse,     7),

    // touch events
    EVENT_TYPE(finger_event_first,              category_touch,     0),
    EVENT_TYPE(finger_moved,                    category_touch,     1),
    EVENT_TYPE(finger_down,                     category_touch,     2),
    EVENT_TYPE(finger_up,                       category_touch,     3),
    EVENT_TYPE(finger_canceled,                 category_touch,     4),
    EVENT_TYPE(finger_event_last,               category_touch,     5),

    // pinch events
    EVENT_TYPE(pinch_event_first,               category_touch,     0),
    EVENT_TYPE(pinch_begin,                     category_touch,     1),
    EVENT_TYPE(pinch_update,                    category_touch,     2),
    EVENT_TYPE(pinch_end,                       category_touch,     3),
    EVENT_TYPE(pinch_event_last,                category_touch,     4),

    // pen events
    EVENT_TYPE(pen_event_first,                 category_pen,       0),
    EVENT_TYPE(pen_proximity_in,                category_pen,       1),
    EVENT_TYPE(pen_proximity_out,               category_pen,       2),
    EVENT_TYPE(pen_moved,                       category_pen,       3),
    EVENT_TYPE(pen_up,                          category_pen,       4),
    EVENT_TYPE(pen_down,                        category_pen,       5),
    EVENT_TYPE(pen_button_up,                   category_pen,       6),
    EVENT_TYPE(pen_button_down,                 category_pen,       7),
    EVENT_TYPE(pen_axis_changed,                category_pen,       8),
    EVENT_TYPE(pen_event_last,                  category_pen,       9),

    // clipboard events
    EVENT_TYPE(clipboard_event_first,           category_clipboard, 0),
    EVENT_TYPE(clipboard_updated,               category_clipboard, 1),
    EVENT_TYPE(clipboard_event_last,            category_clipboard, 2),

    // drag and drop events
    EVENT_TYPE(drop_event_first,                category_drop,      0),
    EVENT_TYPE(drop_file,                       category_drop,      1),
    EVENT_TYPE(drop_text,                       category_drop,      2),
    EVENT_TYPE(drop_begin,                      category_drop,      3),
    EVENT_TYPE(drop_end,                        category_drop,      4),
    EVENT_TYPE(drop_position,                   category_drop,      5),
    EVENT_TYPE(drop_event_last,                 category_drop,      6),

    // internal events
    EVENT_TYPE(internal_event_poll_sentinel,    category_internal,  0)
};

#undef EVENT_TYPE
#undef EVENT_TYPE_USER

///////////////////////////////////////////////////////////////////////////////
// app events
///////////////////////////////////////////////////////////////////////////////

struct app_event_common {};

struct system_theme_changed_event
{
#if defined(VX_APP_VIDEO_ENABLED)
    video::system_theme system_theme;
#endif // VX_APP_VIDEO_ENABLED
};

struct app_event_type
{
    app_event_common common;

    union
    {
        system_theme_changed_event system_theme_changed;
    };
};

#if defined(VX_APP_VIDEO_ENABLED)

///////////////////////////////////////////////////////////////////////////////
// display events
///////////////////////////////////////////////////////////////////////////////

struct display_event_common
{
    video::display_id display_id;
};

struct display_added_event {};
struct display_removed_event {};
struct display_moved_event {};

struct display_orientation_changed_event
{
    video::display_orientation orientation;
};

struct display_desktop_mode_changed_event {};
struct display_current_mode_changed_event {};

struct display_content_scale_changed_event
{
    float x, y;
};

struct display_event_type
{
    display_event_common comon;

    union
    {
        display_added_event display_added;
        display_removed_event display_removed;
        display_moved_event display_moved;
        display_orientation_changed_event display_orientation_changed;
        display_desktop_mode_changed_event display_desktop_mode_changed;
        display_current_mode_changed_event display_current_mode_changed;
        display_content_scale_changed_event display_content_scale_changed;
    };
};

///////////////////////////////////////////////////////////////////////////////
// window events
///////////////////////////////////////////////////////////////////////////////

struct window_event_common
{
    video::window_id window_id;
};

struct window_created_event {};
struct window_destroyed_event {};

struct window_shown_event {};
struct window_hidden_event {};

struct window_exposed_event {};
struct window_occluded_event {};

struct window_moved_event
{
    int32_t x, y;
};

struct window_resized_event
{
    int32_t w, h;
};

struct window_pixel_size_changed_event
{
    int32_t w, h;
};

struct window_minimized_event {};
struct window_maximized_event {};
struct window_restored_event {};

struct window_enter_fullscreen_event {};
struct window_leave_fullscreen_event {};

struct window_gained_focus_event {};
struct window_lost_focus_event {};

struct window_mouse_enter_event {};
struct window_mouse_leave_event {};

struct window_hit_test_event {};

struct window_display_changed_event
{
    video::display_id display_id;
};

struct window_display_scale_changed_event
{
    float xscale, yscale;
};

struct window_safe_area_changed_event
{
    int32_t x, y, w, h;
};

struct window_close_requested_event {};

struct window_event_type
{
    window_event_common common;

    union
    {
        window_created_event window_created;
        window_destroyed_event window_destroyed;
        window_shown_event window_shown;
        window_hidden_event window_hidden;
        window_exposed_event window_exposed;
        window_occluded_event window_occluded;
        window_moved_event window_moved;
        window_resized_event window_resized;
        window_pixel_size_changed_event window_pixel_size_changed;
        window_minimized_event window_minimized;
        window_maximized_event window_maximized;
        window_restored_event window_restored;
        window_enter_fullscreen_event window_enter_fullscreen;
        window_leave_fullscreen_event window_leave_fullscreen;
        window_gained_focus_event window_gained_focus;
        window_lost_focus_event window_lost_focus;
        window_mouse_enter_event window_mouse_enter;
        window_mouse_leave_event window_mouse_leave;
        window_hit_test_event window_hit_test;
        window_display_changed_event window_display_changed;
        window_display_scale_changed_event window_display_scale_changed;
        window_safe_area_changed_event window_safe_area_changed;
        window_close_requested_event window_close_requested;
    };
};

///////////////////////////////////////////////////////////////////////////////
// key events
///////////////////////////////////////////////////////////////////////////////

struct key_event_common
{
    keyboard::keyboard_id keyboard_id;
};

struct keyboard_added_event {};
struct keyboard_removed_event {};

struct key_press_event_base
{
    video::window_id window_id;
    keyboard::scancode scancode;
    keyboard::keycode key;
    keyboard::key_mod mods;
    uint16_t raw;
    bool repeate;
};

struct key_down_event : key_press_event_base {};
struct key_up_event : key_press_event_base {};

struct key_event_type
{
    key_event_common common;

    union
    {
        keyboard_added_event keyboard_added;
        keyboard_removed_event keyboard_removed;
        key_down_event key_down;
        key_up_event key_up;
    };
};

///////////////////////////////////////////////////////////////////////////////
// text events
///////////////////////////////////////////////////////////////////////////////

struct text_event_common {};

struct text_input_event
{
    char32_t text;
};

struct text_event_type
{
    text_event_common common;

    union
    {
        text_input_event text_input;
    };
};

///////////////////////////////////////////////////////////////////////////////
// mouse events
///////////////////////////////////////////////////////////////////////////////

struct mouse_event_common
{
    mouse::mouse_id mouse_id;
    video::window_id window_id;
    float x, y;
};

struct mouse_moved_event
{
    float dx, dy;
};

struct mouse_button_down_event
{
    mouse::button buttons;
    uint8_t clicks;
};

struct mouse_button_up_event
{
    mouse::button buttons;
};

struct mouse_wheel_event
{
    mouse::wheel_direction wheel;
    float delta;
};

struct mouse_event_type
{
    mouse_event_common common;

    union
    {
        mouse_moved_event mouse_moved;
        mouse_button_down_event mouse_button_down;
        mouse_button_up_event mouse_button_up;
        mouse_wheel_event mouse_wheel;
    };
};

///////////////////////////////////////////////////////////////////////////////
// touch events
///////////////////////////////////////////////////////////////////////////////

struct touch_event_common
{
    touch::touch_id touch_id;
    touch::finger_id finger_id;
    float x, y;
    float pressure;
    video::window_id window_id;
};

struct finger_moved_event
{
    float dx, dy;
    float pressure_difference;
};

struct finger_down_event {};
struct finger_up_event {};
struct finger_cancelled_event {};

struct touch_event_type
{
    touch_event_common common;

    union
    {
        finger_moved_event finger_moved;
        finger_down_event finger_down;
        finger_up_event finger_up;
        finger_cancelled_event finger_cancelled;
    };
};

///////////////////////////////////////////////////////////////////////////////
// pinch events
///////////////////////////////////////////////////////////////////////////////

struct pinch_event_common
{
    float scale;
    video::window_id window_id;
};

struct pinch_event_begin {};
struct pinch_event_update {};
struct pinch_event_end {};

struct pinch_event_type
{
    pinch_event_common common;

    union
    {
        pinch_event_begin pinch_begin;
        pinch_event_update pinch_update;
        pinch_event_end pinch_end;
    };
};

///////////////////////////////////////////////////////////////////////////////
// pen events
///////////////////////////////////////////////////////////////////////////////

struct pen_event_common
{
    pen::pen_id pen_id;
    float x, y;
    pen::input_state state;
    video::window_id window_id;
};

struct pen_proximity_in_event {};
struct pen_proximity_out_event {};

struct pen_moved_event {};

struct pen_touch_event
{
    bool eraser;
};

struct pen_button_event
{
    uint8_t button;
};

struct pen_axis_changed_event
{
    pen::axis_type axis;
    float value;
};

struct pen_event_type
{
    pen_event_common common;

    union
    {
        pen_proximity_in_event pen_proximity_in;
        pen_proximity_out_event pen_proximity_out;
        pen_moved_event pen_moved;
        pen_touch_event pen_touch;
        pen_button_event pen_button;
        pen_axis_changed_event pen_axis_changed;
    };
};

///////////////////////////////////////////////////////////////////////////////
// clipboard events
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// drag and drop events
///////////////////////////////////////////////////////////////////////////////

#endif // VX_APP_VIDEO_ENABLED

///////////////////////////////////////////////////////////////////////////////
// user events
///////////////////////////////////////////////////////////////////////////////

struct user_event_type
{
    void* user_data;
};

///////////////////////////////////////////////////////////////////////////////
// event
///////////////////////////////////////////////////////////////////////////////

class event
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // event data
    ///////////////////////////////////////////////////////////////////////////////

    event_type type;
    time::time_point time;

    union
    {
        // app events

        app_event_type app_event{};

#if defined(VX_APP_VIDEO_ENABLED)

        // display events

        display_event_type display_event;

        // window events

        window_event_type window_event;

        // key events

        key_event_type key_event;

        // mouse events

        mouse_event_type mouse_event;

        // touch events

        touch_event_type touch_event;

        // pinch events

        pinch_event_type pinch_event;

        // pen events

        pen_event_type pen_event;

#endif // VX_APP_VIDEO_ENABLED

        // user event

        user_event_type user_event;
    };

#if defined(VX_APP_VIDEO_ENABLED)

    video::window_id get_window_id() const
    {
        if (get_category(type) == category_window)
        {
            return window_event.common.window_id;
        }

        return invalid_id;
    }

#endif // VX_APP_VIDEO_ENABLED

};

///////////////////////////////////////////////////////////////////////////////
// processing
///////////////////////////////////////////////////////////////////////////////

VX_API void pump_events();

////////////////////////////////////////

VX_API size_t add_events(const event* e, size_t count);

inline bool add_event(const event& e) { return add_events(&e, 1) == 1; }

////////////////////////////////////////

using event_filter = bool(*)(const event&, void*);

////////////////////////////////////////

VX_API size_t match_events(event_filter matcher, void* user_data, event* events, size_t count, bool remove);

inline bool type_matcher(const event& e, void* user_data) noexcept
{
    const event_type type = *reinterpret_cast<event_type*>(user_data);
    return e.type == type;
}

inline bool category_matcher(const event& e, void* user_data) noexcept
{
    const event_category category = *reinterpret_cast<event_category*>(user_data);
    return get_category(e.type) == category;
}

////////////////////////////////////////

inline bool has_events(event_filter matcher, void* user_data = nullptr)
{
    return match_events(matcher, user_data, nullptr, 0, false) > 0;
}

inline bool has_event(event_type type)
{
    return has_events(type_matcher, &type);
}

////////////////////////////////////////

inline size_t peek_events(event_filter matcher, void* user_data, event* out, size_t count)
{
    return match_events(matcher, user_data, out, count, false);
}

////////////////////////////////////////

inline size_t get_events(event_filter matcher, void* user_data, event* out, size_t count)
{
    return match_events(matcher, user_data, out, count, true);
}

inline bool get_event(event& e, event_type type)
{
    return get_events(type_matcher, &type, &e, 1) == 1;
}

////////////////////////////////////////

inline size_t flush_events(event_filter matcher, void* user_data)
{
    return match_events(matcher, user_data, nullptr, 0, true);
}

inline void flush_events(event_type type)
{
    flush_events(type_matcher, &type);
}

////////////////////////////////////////

VX_API bool wait_event_timeout(event& e, time::time_point t);

inline bool wait_event(event& e)
{
    return wait_event_timeout(e, time::max());
}

////////////////////////////////////////

VX_API bool push_event(event& e);
VX_API bool poll_event(event& e);

////////////////////////////////////////

VX_API void set_event_filter(event_filter filter, void* user_data);
VX_API void get_event_filter(event_filter& filter, void*& user_data);
inline void clear_event_filter() { set_event_filter(nullptr, nullptr); }

////////////////////////////////////////

VX_API void add_event_watch(event_filter callback, void* user_data);
VX_API void remove_event_watch(event_filter callback, void* user_data);

} // namespace event
} // namespace app
} // namespace vx