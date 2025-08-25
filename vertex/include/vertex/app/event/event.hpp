#pragma once

#include "vertex/app/device_id.hpp"
#include "vertex/app/video/video.hpp"
#include "vertex/app/input/mouse.hpp"
#include "vertex/app/input/keyboard.hpp"

// https://www.glfw.org/docs/3.3/input_guide.html

#if defined(MOUSE_MOVED)
#   undef MOUSE_MOVED
#endif

namespace vx {
namespace app {
namespace event {

using event_type_t = uint32_t;

///////////////////////////////////////////////////////////////////////////////
// event category
///////////////////////////////////////////////////////////////////////////////

enum event_category : uint32_t
{
    CATEGORY_APP,
    CATEGORY_DISPLAY,
    CATEGORY_WINDOW,
    CATEGORY_KEY,
    CATEGORY_MOUSE,
    CATEGORY_FINGER,
    CATEGORY_PEN,
    CATEGORY_CLIPBOARD,
    CATEGORY_DROP
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
    USER_FLAG_SHIFT = 31,
    USER_FLAG_MASK = 0x80000000,

    CATEGORY_SHIFT = 24,
    CATEGORY_MASK = 0x7F000000,
    
    NUMBER_SHIFT = 0,
    NUMBER_MASK = 0x00FFFFFF
};

} // namespace _priv

inline constexpr event_type_t make_event(event_category category, uint32_t number, bool user_defined = false) noexcept
{
    return (static_cast<event_type_t>(user_defined) << _priv::USER_FLAG_SHIFT) |
        (static_cast<event_type_t>(category) << _priv::CATEGORY_SHIFT) |
        (number & _priv::NUMBER_MASK);
}

///////////////////////////////////////////////////////////////////////////////
// decoding helpers
///////////////////////////////////////////////////////////////////////////////

inline constexpr bool is_user_event(event_type_t type) noexcept
{
    return type & _priv::USER_FLAG_MASK;
}

inline constexpr event_category get_category(event_type_t type) noexcept
{
    return static_cast<event_category>((type >> _priv::CATEGORY_SHIFT) & _priv::CATEGORY_MASK);
}

inline constexpr uint32_t get_number(event_type_t type) noexcept
{
    return type & _priv::NUMBER_MASK;
}

///////////////////////////////////////////////////////////////////////////////
// event type
///////////////////////////////////////////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/main/include/SDL3/SDL_events.h#L84

#define EVENT_TYPE(name, cat, i) name = make_event(cat, i)

enum event_type : event_type_t
{
    // app events
    EVENT_TYPE(APP_EVENT_FIRST,                 CATEGORY_APP,       0),
    EVENT_TYPE(APP_QUIT,                        CATEGORY_APP,       1),
    EVENT_TYPE(APP_TERMINATING,                 CATEGORY_APP,       2),
    EVENT_TYPE(APP_WILL_ENTER_BACKGROUND,       CATEGORY_APP,       3),
    EVENT_TYPE(APP_DID_ENTER_BACKGROUND,        CATEGORY_APP,       4),
    EVENT_TYPE(APP_WILL_ENTER_FOREGROUND,       CATEGORY_APP,       5),
    EVENT_TYPE(APP_DID_ENTER_FOREGROUND,        CATEGORY_APP,       6),
    EVENT_TYPE(APP_LOCALE_CHANGED,              CATEGORY_APP,       7),
    EVENT_TYPE(APP_SYSTEM_THEME_CHANGED,        CATEGORY_APP,       8),
    EVENT_TYPE(APP_EVENT_LAST,                  CATEGORY_APP,       9),

    // display events
    EVENT_TYPE(DISPLAY_EVENT_FIRST,             CATEGORY_DISPLAY,   0),
    EVENT_TYPE(DISPLAY_ADDED,                   CATEGORY_DISPLAY,   1),
    EVENT_TYPE(DISPLAY_REMOVED,                 CATEGORY_DISPLAY,   2),
    EVENT_TYPE(DISPLAY_MOVED,                   CATEGORY_DISPLAY,   3),
    EVENT_TYPE(DISPLAY_ORIENTATION_CHANGED,     CATEGORY_DISPLAY,   4),
    EVENT_TYPE(DISPLAY_DESKTOP_MODE_CHANGED,    CATEGORY_DISPLAY,   5),
    EVENT_TYPE(DISPLAY_CURRENT_MODE_CHANGED,    CATEGORY_DISPLAY,   6),
    EVENT_TYPE(DISPLAY_CONTENT_SCALE_CHANGED,   CATEGORY_DISPLAY,   7),
    EVENT_TYPE(DISPLAY_EVENT_LAST,              CATEGORY_DISPLAY,   8),

    // window events
    EVENT_TYPE(WINDOW_EVENT_FIRST,              CATEGORY_WINDOW,    0),
    EVENT_TYPE(WINDOW_CREATED,                  CATEGORY_WINDOW,    1),
    EVENT_TYPE(WINDOW_DESTROYED,                CATEGORY_WINDOW,    2),
    EVENT_TYPE(WINDOW_SHOWN,                    CATEGORY_WINDOW,    3),
    EVENT_TYPE(WINDOW_HIDDEN,                   CATEGORY_WINDOW,    4),
    EVENT_TYPE(WINDOW_EXPOSED,                  CATEGORY_WINDOW,    5),
    EVENT_TYPE(WINDOW_OCCLUDED,                 CATEGORY_WINDOW,    6),
    EVENT_TYPE(WINDOW_MOVED,                    CATEGORY_WINDOW,    7),
    EVENT_TYPE(WINDOW_RESIZED,                  CATEGORY_WINDOW,    8),
    EVENT_TYPE(WINDOW_PIXEL_SIZE_CHANGED,       CATEGORY_WINDOW,    9),
    EVENT_TYPE(WINDOW_MINIMIZED,                CATEGORY_WINDOW,   10),
    EVENT_TYPE(WINDOW_MAXIMIZED,                CATEGORY_WINDOW,   11),
    EVENT_TYPE(WINDOW_RESTORED,                 CATEGORY_WINDOW,   12),
    EVENT_TYPE(WINDOW_ENTER_FULLSCREEN,         CATEGORY_WINDOW,   13),
    EVENT_TYPE(WINDOW_LEAVE_FULLSCREEN,         CATEGORY_WINDOW,   14),
    EVENT_TYPE(WINDOW_GAINED_FOCUS,             CATEGORY_WINDOW,   15),
    EVENT_TYPE(WINDOW_LOST_FOCUS,               CATEGORY_WINDOW,   16),
    EVENT_TYPE(WINDOW_MOUSE_ENTER,              CATEGORY_WINDOW,   17),
    EVENT_TYPE(WINDOW_MOUSE_LEAVE,              CATEGORY_WINDOW,   18),
    EVENT_TYPE(WINDOW_HIT_TEST,                 CATEGORY_WINDOW,   19),
    EVENT_TYPE(WINDOW_DISPLAY_CHANGED,          CATEGORY_WINDOW,   20),
    EVENT_TYPE(WINDOW_DISPLAY_SCALE_CHANGED,    CATEGORY_WINDOW,   21),
    EVENT_TYPE(WINDOW_SAFE_AREA_CHANGED,        CATEGORY_WINDOW,   22),
    EVENT_TYPE(WINDOW_CLOSE_REQUESTED,          CATEGORY_WINDOW,   23),
    EVENT_TYPE(WINDOW_EVENT_LAST,               CATEGORY_WINDOW,   24),

    // key events
    EVENT_TYPE(KEY_EVENT_FIRST,                 CATEGORY_KEY,       0),
    EVENT_TYPE(KEYBOARD_ADDED,                  CATEGORY_KEY,       1),
    EVENT_TYPE(KEYBOARD_REMOVED,                CATEGORY_KEY,       2),
    EVENT_TYPE(KEY_DOWN,                        CATEGORY_KEY,       3),
    EVENT_TYPE(KEY_UP,                          CATEGORY_KEY,       4),
    EVENT_TYPE(TEXT_EDITING,                    CATEGORY_KEY,       5),
    EVENT_TYPE(TEXT_INPUT,                      CATEGORY_KEY,       6),
    EVENT_TYPE(KEYMAP_CHANGED,                  CATEGORY_KEY,       7),
    EVENT_TYPE(TEXT_EDITING_CANDIDATES,         CATEGORY_KEY,       8),
    EVENT_TYPE(KEY_EVENT_LAST,                  CATEGORY_KEY,       9),

    // mouse events
    EVENT_TYPE(MOUSE_EVENT_FIRST,               CATEGORY_MOUSE,     0),
    EVENT_TYPE(MOUSE_ADDED,                     CATEGORY_MOUSE,     1),
    EVENT_TYPE(MOUSE_REMOVED,                   CATEGORY_MOUSE,     2),
    EVENT_TYPE(MOUSE_MOVED,                     CATEGORY_MOUSE,     3),
    EVENT_TYPE(MOUSE_BUTTON_DOWN,               CATEGORY_MOUSE,     4),
    EVENT_TYPE(MOUSE_BUTTON_UP,                 CATEGORY_MOUSE,     5),
    EVENT_TYPE(MOUSE_WHEEL,                     CATEGORY_MOUSE,     6),
    EVENT_TYPE(MOUSE_EVENT_LAST,                CATEGORY_MOUSE,     7),

    // touch events
    EVENT_TYPE(FINGER_EVENT_FIRST,              CATEGORY_FINGER,    0),
    EVENT_TYPE(FINGER_MOVED,                    CATEGORY_FINGER,    1),
    EVENT_TYPE(FINGER_DOWN,                     CATEGORY_FINGER,    2),
    EVENT_TYPE(FINGER_UP,                       CATEGORY_FINGER,    3),
    EVENT_TYPE(FINGER_CANCELED,                 CATEGORY_FINGER,    4),
    EVENT_TYPE(FINGER_EVENT_LAST,               CATEGORY_FINGER,    5),

    // pen events
    EVENT_TYPE(PEN_EVENT_FIRST,                 CATEGORY_PEN,       0),
    EVENT_TYPE(PEN_PROXIMITY_IN,                CATEGORY_PEN,       1),
    EVENT_TYPE(PEN_PROXIMITY_OUT,               CATEGORY_PEN,       2),
    EVENT_TYPE(PEN_MOVED,                       CATEGORY_PEN,       3),
    EVENT_TYPE(PEN_UP,                          CATEGORY_PEN,       4),
    EVENT_TYPE(PEN_DOWN,                        CATEGORY_PEN,       5),
    EVENT_TYPE(PEN_BUTTON_UP,                   CATEGORY_PEN,       6),
    EVENT_TYPE(PEN_BUTTON_DOWN,                 CATEGORY_PEN,       7),
    EVENT_TYPE(PEN_AXIS_CHANGED,                CATEGORY_PEN,       8),
    EVENT_TYPE(PEN_EVENT_LAST,                  CATEGORY_PEN,       9),

    // clipboard events
    EVENT_TYPE(CLIPBOARD_EVENT_FIRST,           CATEGORY_CLIPBOARD, 0),
    EVENT_TYPE(CLIPBOARD_UPDATED,               CATEGORY_CLIPBOARD, 1),
    EVENT_TYPE(CLIPBOARD_EVENT_LAST,            CATEGORY_CLIPBOARD, 2),

    // drag and drop events
    EVENT_TYPE(DROP_EVENT_FIRST,                CATEGORY_DROP,      0),
    EVENT_TYPE(DROP_FILE,                       CATEGORY_DROP,      1),
    EVENT_TYPE(DROP_TEXT,                       CATEGORY_DROP,      2),
    EVENT_TYPE(DROP_BEGIN,                      CATEGORY_DROP,      3),
    EVENT_TYPE(DROP_END,                        CATEGORY_DROP,      4),
    EVENT_TYPE(DROP_POSITION,                   CATEGORY_DROP,      5),
    EVENT_TYPE(DROP_EVENT_LAST,                 CATEGORY_DROP,      6)
};

#undef EVENT_TYPE

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

    union
    {
        // display events

        struct
        {
            device_id display_id;
        }
        display_added;

        struct
        {
            device_id display_id;
        }
        display_removed;

        struct
        {
            device_id display_id;
        }
        display_moved;

        struct
        {
            video::display_orientation orientation;
        }
        display_orientation_changed;

        struct
        {
            device_id display_id;
        }
        display_desktop_mode_changed;

        struct
        {
            device_id display_id;
        }
        current_mode_changed;

        struct
        {
            float x, y;
        }
        display_content_scale_changed;

        // window events

        struct
        {
            device_id window_id;
        }
        window_shown;

        struct
        {
            device_id window_id;
        }
        window_hidden;

        struct
        {
            int32_t x, y;
        }
        window_moved;

        struct
        {
            int32_t w, h;
        }
        window_resized;

        struct
        {
            device_id window_id;
        }
        window_minimized;

        struct
        {
            device_id window_id;
        }
        window_maximized;

        struct
        {
            device_id window_id;
        }
        window_restored;

        struct
        {
            device_id window_id;
        }
        window_enter_fullscreen;

        struct
        {
            device_id window_id;
        }
        window_leave_fullscreen;

        struct
        {
            device_id window_id;
        }
        window_gained_focus;

        struct
        {
            device_id window_id;
        }
        window_lost_focus;

        struct
        {
            device_id window_id;
        }
        window_mouse_enter;

        struct
        {
            device_id window_id;
        }
        window_mouse_leave;

        struct
        {
            device_id display_id;
        }
        window_display_changed;

        struct
        {
            device_id window_id;
        }
        window_display_scale_changed;

        struct
        {
            device_id window_id;
        }
        window_close_requested;

        struct
        {
            device_id window_id;
        }
        window_destroyed;

        // key events

        struct
        {
            int32_t scancode;
            keyboard::key key;
            uint8_t repeate;
        }
        key_down;

        struct
        {
            int32_t scancode;
            keyboard::key key;
        }
        key_up;

        struct
        {
            char32_t text;
        }
        text_input;

        // mouse events

        struct
        {
            int32_t x, y;
            int32_t dx, dy;
        }
        mouse_moved;

        struct
        {
            mouse::button button;
            int32_t x, y;
            uint8_t clicks;
        }
        mouse_button_down;

        struct
        {
            mouse::button button;
            int32_t x, y;
        }
        mouse_button_up;

        struct
        {
            mouse::wheel wheel;
            float delta;
            int32_t x, y;
        }
        mouse_wheel;
    };
};

///////////////////////////////////////////////////////////////////////////////
// processing
///////////////////////////////////////////////////////////////////////////////

VX_API void pump_events(bool process_all);

VX_API bool post_event(const event& e);
VX_API bool poll_event(event& e);

VX_API void wait_events();
VX_API void wait_events_timeout(unsigned int timeout_ms);

VX_API size_t flush_events(event_type type);

using event_filter = bool(*)(const event&, void*);
VX_API size_t filter_events(event_filter filter, void* user_data);

///////////////////////////////////////////////////////////////////////////////
// callback
///////////////////////////////////////////////////////////////////////////////

using event_process_callback = void(*)();
VX_API void set_event_process_callback(event_process_callback callback);

///////////////////////////////////////////////////////////////////////////////
// checking
///////////////////////////////////////////////////////////////////////////////

VX_API bool has_event(event_type type);
VX_API size_t event_count();
VX_API std::vector<event> get_events();

} // namespace event
} // namespace app
} // namespace vx