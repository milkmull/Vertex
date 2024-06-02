#pragma once

#include <mutex>

#include "keyboard.h"
#include "mouse.h"
#include "joystick.h"

// https://www.glfw.org/docs/3.3/input_guide.html

namespace vx {
namespace app {

enum event_type : int
{
    // display events
    DISPLAY_ORIENTATION,
    DISPLAY_CONNECTION,
    DISPLAY_MOVE,
    DISPLAY_CONTENT_SCALE,

    // window events
    WINDOW_CLOSE,
    WINDOW_RESIZE,
    WINDOW_MINIMIZE,
    WINDOW_MAXIMIZE,
    WINDOW_MOVE,
    WINDOW_FOCUS,
    WINDOW_SHOW,

    // key events
    KEY_DOWN,
    KEY_REPEAT,
    KEY_UP,
    TEXT_INPUT,

    // mouse events
    MOUSE_MOVE,
    MOUSE_HOVER,
    MOUSE_BUTTON_DOWN,
    MOUSE_BUTTON_UP,
    MOUSE_SCROLL,

    JOYSTICK_CONNECTION
};

class event
{
public:

    struct device_event
    {
        uint32_t id;
        bool connected;
    };

    struct state_event
    {
        bool value;
    };

    struct size_event
    {
        int width;
        int height;
    };

    struct move_event
    {
        int x;
        int y;
    };

    struct key_event
    {
        keyboard::key key;
        int scancode;
    };

    struct text_input_event
    {
        char32_t codepoint;
    };

    struct mouse_button_event
    {
        mouse::button button;
        int x;
        int y;
    };

    struct mouse_scroll_event
    {
        mouse::wheel wheel;
        float delta;
        int x;
        int y;
    };

    event_type type;
    window* window;

    union
    {
        device_event monitor_added;

        size_event window_resize;
        state_event window_minimize;
        state_event window_maximize;
        move_event window_move;
        state_event window_focus;
        state_event window_show;

        key_event key;
        text_input_event text_input;

        move_event mouse_move;
        state_event mouse_hover;

        mouse_button_event mouse_button;
        mouse_scroll_event mouse_scroll;
    };

private:

    static std::mutex s_mutex;
    static std::queue<event> s_events;

public:

    static void push_event(const event& e)
    {
        s_mutex.lock();
        s_events.push(e);
        s_mutex.unlock();
    }

    static bool pop_event(event& e, bool block = false)
    {
        s_mutex.lock();

        bool found = false;

        // If there are any events in the queue return the next one
        if (!s_events.empty())
        {
            e = s_events.front();
            s_events.pop();

            found = true;
        }

        s_mutex.unlock();

        return found;
    }

};

}
}