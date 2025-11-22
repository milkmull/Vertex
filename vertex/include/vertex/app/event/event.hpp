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
// event type
///////////////////////////////////////////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/main/include/SDL3/SDL_events.h#L84

enum event_type : event_type_t
{
    event_last = 0xFFFF,
    event_first = 0,
    invalid_event = event_first,

    // app events
    app_quit,
    app_terminating,
    app_low_memory,
    app_will_enter_background,
    app_did_enter_background,
    app_will_enter_foreground,
    app_did_enter_foreground,
    app_locale_changed,
    app_system_theme_changed,

    _app_event_first = app_quit,
    _app_event_last = app_system_theme_changed,

    // display events
    display_added,
    display_removed,
    display_moved,
    display_orientation_changed,
    display_desktop_mode_changed,
    display_current_mode_changed,
    display_content_scale_changed,

    _display_event_first = display_added,
    _display_event_last = display_content_scale_changed,

    // window events
    window_created,
    window_destroyed,
    window_shown,
    window_hidden,
    window_exposed,
    window_occluded,
    window_moved,
    window_resized,
    window_pixel_size_changed,
    window_minimized,
    window_maximized,
    window_restored,
    window_enter_fullscreen,
    window_leave_fullscreen,
    window_gained_focus,
    window_lost_focus,
    window_mouse_enter,
    window_mouse_leave,
    window_hit_test,
    window_display_changed,
    window_display_scale_changed,
    window_safe_area_changed,
    window_close_requested,

    _window_event_first = window_created,
    _window_event_last = window_close_requested,

    // keyboard events
    keyboard_added,
    keyboard_removed,
    keymap_changed,
    key_down,
    key_up,
    screen_keyboard_shown,
    screen_keyboard_hidden,

    _keyboard_event_first = keyboard_added,
    _keyboard_event_last = key_up,

    // text events
    text_editing,
    text_input,
    text_editing_candidates,

    _text_event_first = text_editing,
    _text_event_last = text_editing_candidates,

    // mouse events
    mouse_added,
    mouse_removed,
    mouse_moved,
    mouse_button_down,
    mouse_button_up,
    mouse_wheel,

    _mouse_event_first = mouse_added,
    _mouse_event_last = mouse_wheel,

    // touch events
    finger_moved,
    finger_down,
    finger_up,
    finger_canceled,

    _touch_event_first = finger_moved,
    _touch_event_last = finger_canceled,

    // pinch events
    pinch_begin,
    pinch_update,
    pinch_end,

    _pinch_event_first = pinch_begin,
    _pinch_event_last = pinch_end,

    // pen events
    pen_proximity_in,
    pen_proximity_out,
    pen_moved,
    pen_up,
    pen_down,
    pen_button_up,
    pen_button_down,
    pen_axis_changed,

    _pen_event_first = pen_proximity_in,
    _pen_event_last = pen_axis_changed,

    // clipboard events
    clipboard_updated,

    _clipboard_event_first = clipboard_updated,
    _clipboard_event_last = clipboard_updated,

    // drag and drop events
    drop_file,
    drop_text,
    drop_begin,
    drop_end,
    drop_position,

    _drop_event_first = drop_file,
    _drop_event_last = drop_position,

    // internal events (unchanged because there is no paired last event)
    internal_event_poll_sentinel,

    _internal_event_first = internal_event_poll_sentinel,
    _internal_event_last = internal_event_poll_sentinel,

    // user event
    user_event = 0x8000
};

///////////////////////////////////////////////////////////////////////////////
// user events
///////////////////////////////////////////////////////////////////////////////

inline constexpr event_type_t make_user_event(uint16_t number) noexcept
{
    const uint32_t target = user_event + number;
    return (target < event_last) ? target : invalid_event;
}

inline constexpr bool is_user_event(event_type_t type) noexcept
{
    return (user_event <= type) && (type < event_last);
}

///////////////////////////////////////////////////////////////////////////////
// event category
///////////////////////////////////////////////////////////////////////////////

enum event_category
{
    category_none,
    category_app,
    category_display,
    category_window,
    category_keyboard,
    category_text,
    category_mouse,
    category_touch,
    category_pinch,
    category_pen,
    category_clipboard,
    category_drop,
    category_internal,
    category_user
};

inline constexpr event_category get_category(event_type_t type) noexcept
{
    if (type > event_last)
    {
        return category_none;
    }
    if (type >= user_event)
    {
        return category_user;
    }
    if (type >= _internal_event_first && type <= _internal_event_last)
    {
        return category_internal;
    }
    if (type >= _drop_event_first && type <= _drop_event_last)
    {
        return category_drop;
    }
    if (type >= _clipboard_event_first && type <= _clipboard_event_last)
    {
        return category_clipboard;
    }
    if (type >= _pen_event_first && type <= _pen_event_last)
    {
        return category_pen;
    }
    if (type >= _pinch_event_first && type <= _pinch_event_last)
    {
        return category_pinch;
    }
    if (type >= _touch_event_first && type <= _touch_event_last)
    {
        return category_touch;
    }
    if (type >= _mouse_event_first && type <= _mouse_event_last)
    {
        return category_mouse;
    }
    if (type >= _text_event_first && type <= _text_event_last)
    {
        return category_text;
    }
    if (type >= _keyboard_event_first && type <= _keyboard_event_last)
    {
        return category_keyboard;
    }
    if (type >= _window_event_first && type <= _window_event_last)
    {
        return category_window;
    }
    if (type >= _display_event_first && type <= _display_event_last)
    {
        return category_display;
    }
    if (type >= _app_event_first && type <= _app_event_last)
    {
        return category_app;
    }

    return category_none;
}

///////////////////////////////////////////////////////////////////////////////
// app events
///////////////////////////////////////////////////////////////////////////////

struct app_event_common {};

#if defined(VX_APP_VIDEO_ENABLED)

struct system_theme_changed_event
{
    video::system_theme system_theme;
};

#endif // VX_APP_VIDEO_ENABLED

struct app_event_type
{
    app_event_common common;

    union
    {
#if defined(VX_APP_VIDEO_ENABLED)

        system_theme_changed_event system_theme_changed;

#endif // VX_APP_VIDEO_ENABLED
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

struct display_orientation_changed_event
{
    video::display_orientation orientation;
};

struct display_content_scale_changed_event
{
    float x, y;
};

struct display_event_type
{
    display_event_common common;

    union
    {
        display_orientation_changed_event display_orientation_changed;
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

struct window_event_type
{
    window_event_common common;

    union
    {
        window_moved_event window_moved;
        window_resized_event window_resized;
        window_pixel_size_changed_event window_pixel_size_changed;
        window_display_changed_event window_display_changed;
        window_display_scale_changed_event window_display_scale_changed;
        window_safe_area_changed_event window_safe_area_changed;
    };
};

///////////////////////////////////////////////////////////////////////////////
// keyboard events
///////////////////////////////////////////////////////////////////////////////

struct keyboard_event_common
{
    keyboard::keyboard_id keyboard_id;
    video::window_id window_id;
};

struct key_event
{
    keyboard::scancode scancode;
    keyboard::keycode key;
    keyboard::key_mod mods;
    uint16_t raw;
    bool repeat;
    bool down;
};

struct keyboard_event_type
{
    keyboard_event_common common;

    union
    {
        key_event key;
    };
};

///////////////////////////////////////////////////////////////////////////////
// text events
///////////////////////////////////////////////////////////////////////////////

struct text_event_common {};

struct text_input_event
{
    const char* text;
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

struct mouse_button_event
{
    uint8_t button;
    uint8_t clicks;
    bool down;
};

struct mouse_wheel_event
{
    mouse::wheel_direction direction;
    float x, y;
    int ix, iy;
};

struct mouse_event_type
{
    mouse_event_common common;

    union
    {
        mouse_moved_event mouse_moved;
        mouse_button_event mouse_button;
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

struct touch_event_type
{
    touch_event_common common;

    union
    {
        finger_moved_event finger_moved;
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

struct pinch_event_type
{
    pinch_event_common common;
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

        // keyboard events

        keyboard_event_type keyboard_event;

        // text events

        text_event_type text_event;

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