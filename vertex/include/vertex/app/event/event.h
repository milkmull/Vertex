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

private:

    static std::mutex s_mutex;
    static std::deque<event> s_events;

public:

    using event_process_callback_t = void(*)();
    static void set_event_process_callback(event_process_callback_t callback);

private:

    static event_process_callback_t s_event_process_callback;

public:

    static void pump_events(bool process_all);

    static std::vector<event> peek_events();
    static bool has_event(event_type type);

    static bool post_event(const event& e);
    static bool poll_event(event& e);
    static void wait_events();
    static void wait_events_timeout(unsigned int timeout_ms);

    static size_t flush_events(event_type type);

    using event_filter = bool(*)(void*, const event&);
    static size_t filter_events(event_filter filter, void* user_data);

#define VX_LOG_APP_EVENTS 1

#if defined(VX_LOG_APP_EVENTS)
    static void log_event(const event& e);
#endif

private:

    class event_impl;

};

}
}