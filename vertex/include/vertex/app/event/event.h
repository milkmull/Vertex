#pragma once

#include <mutex>
#include <deque>
#include <vector>

#include "../_priv/device_id.h"
#include "../video/video.h"
#include "keyboard.h"
#include "mouse.h"

// https://www.glfw.org/docs/3.3/input_guide.html

#ifdef MOUSE_MOVED
#   undef MOUSE_MOVED
#endif

namespace vx {
namespace app {

enum event_type : uint32_t
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

class event
{
public:

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
            device_id display_id;
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
        display_current_mode_changed;

        struct
        {
            device_id display_id;
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
            device_id window_id;
            int32_t x, y;
        }
        window_moved;

        struct
        {
            device_id window_id;
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
            device_id window_id;
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
            device_id window_id;
            int32_t scancode;
            keyboard::key key;
            uint8_t repeate;
        }
        key_down;

        struct
        {
            device_id window_id;
            int32_t scancode;
            keyboard::key key;
        }
        key_up;

        struct
        {
            device_id window_id;
            char32_t text;
        }
        text_input;

        // mouse events

        struct
        {
            device_id window_id;
            int32_t x, y;
            int32_t dx, dy;
        }
        mouse_moved;

        struct
        {
            device_id window_id;
            mouse::button button;
            int32_t x, y;
            uint8_t clicks;
        }
        mouse_button_down;

        struct
        {
            device_id window_id;
            mouse::button button;
            int32_t x, y;
        }
        mouse_button_up;

        struct
        {
            device_id window_id;
            mouse::wheel wheel;
            float delta;
            int32_t x, y;
        }
        mouse_wheel;
    };

private:

    static std::mutex s_mutex;
    static std::deque<event> s_events;

public:

    static void pump_events(bool process_all);

    static std::vector<event> peek_events();
    static bool has_event(event_type type);

    static bool post_event(const event& e);
    static bool poll_event(event& e);
    static void wait_events();
    static void wait_events_timeout(unsigned int timeout_ms);

    static size_t flush_events(event_type type);

private:

    class event_impl;

};

}
}