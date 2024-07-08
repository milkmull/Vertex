#pragma once

#include <mutex>
#include <deque>
#include <vector>

#include "../_priv/device_id.h"
#include "keyboard.h"
#include "mouse.h"
#include "joystick.h"

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

enum button_state : uint32_t
{
    PRESSED,
    RELEASED
};

class event
{
public:

    struct display_event_data
    {
        device_id display_id;
    };

    struct window_event_data
    {
        device_id window_id;
        int32_t data1, data2;
    };

    struct keyboard_event_data
    {
        device_id window_id;
        int32_t scancode;
        keyboard::key key;
        button_state state;
        uint8_t repeate;
    };

    struct text_input_event_data
    {
        device_id window_id;
        char32_t text;
    };

    struct mouse_motion_event_data
    {
        device_id window_id;
        int32_t x, y;
        int32_t dx, dy;
    };

    struct mouse_button_event_data
    {
        device_id window_id;
        mouse::button button;
        button_state state;
        uint8_t clicks;
        int32_t x, y;
    };

    struct mouse_wheel_event_data
    {
        device_id window_id;
        mouse::wheel wheel;
        float delta;
        int32_t x, y;
    };

    event_type type;

    union
    {
        display_event_data display_event;

        window_event_data window_event;

        keyboard_event_data keyboard_event;
        text_input_event_data text_input_event;

        mouse_motion_event_data mouse_motion_event;
        mouse_button_event_data mouse_button_event;
        mouse_wheel_event_data mouse_wheel_event;
    };

private:

    static std::mutex s_mutex;
    static std::deque<event> s_events;

public:

    static void pump_events(bool process_all);

    static std::vector<event> peek_events();
    static bool has_event(event_type type);

    static void post_event(const event& e);
    static bool poll_event(event& e);
    static void wait_events();
    static void wait_events_timeout(unsigned int timeout_ms);

    static size_t flush_events(event_type type);

private:

    class event_impl;

};

}
}