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

///////////////////////////////////////////////////////////////////////////////
// event type
///////////////////////////////////////////////////////////////////////////////

enum event_type
{
    // app events
    APP_TERMINATING,

    // display events
    DISPLAY_ADDED,
    DISPLAY_REMOVED,
    DISPLAY_MOVED,
    DISPLAY_ORIENTATION_CHANGED,
    DISPLAY_DESKTOP_MODE_CHANGED,
    DISPLAY_CURRENT_MODE_CHANGED,
    DISPLAY_CONTENT_SCALE_CHANGED,

    // window events
    WINDOW_SHOWN,
    WINDOW_HIDDEN,
    WINDOW_MOVED,
    WINDOW_RESIZED,
    WINDOW_MINIMIZED,
    WINDOW_MAXIMIZED,
    WINDOW_RESTORED,
    WINDOW_ENTER_FULLSCREEN,
    WINDOW_LEAVE_FULLSCREEN,
    WINDOW_GAINED_FOCUS,
    WINDOW_LOST_FOCUS,
    WINDOW_MOUSE_ENTER,
    WINDOW_MOUSE_LEAVE,
    WINDOW_DISPLAY_CHANGED,
    WINDOW_DISPLAY_SCALE_CHANGED,
    WINDOW_CLOSE_REQUESTED,
    WINDOW_DESTROYED,

    // key events
    KEY_DOWN,
    KEY_UP,
    TEXT_INPUT,

    // mouse events
    MOUSE_MOVED,
    MOUSE_BUTTON_DOWN,
    MOUSE_BUTTON_UP,
    MOUSE_WHEEL
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

    union
    {
        device_id window_id;
        device_id display_id;
    };

    union
    {
        // display events

        struct
        {
            video::display_orientation orientation;
        }
        display_orientation_changed;

        struct
        {
            float x, y;
        }
        display_content_scale_changed;

        // window events

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
            device_id display_id;
        }
        window_display_changed;

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
VX_API const event* enum_events(size_t i);

} // namespace event
} // namespace app
} // namespace vx