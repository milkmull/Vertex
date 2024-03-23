#pragma once

#if defined(VX_APPLICATION)

#include "keyboard.h"
#include "mouse.h"
#include "joystick.h"

// https://www.glfw.org/docs/3.3/input_guide.html

namespace vx {
namespace app {

enum class event_category : int
{
    WINDOW,
    KEY,
    MOUSE,
    JOYSTICK
};

enum class event_type : int
{
    WINDOW_CLOSED,
    WINDOW_RESIZE,
    WINDOW_GAIN_FOCUS,
    WINDOW_LOSE_FOCUS,

    KEY_DOWN,
    KEY_REPEAT,
    KEY_UP,

    TEXT_INPUT,

    MOUSE_MOVE,
    MOUSE_BUTTON_DOWN,
    MOUSE_BUTTON_UP,
    MOUSE_SCROLL,

    MOUSE_ENTER,
    MOUSE_LEAVE,

    JOYSTICK_BUTTON_DOWN,
    JOYSTICK_BUTTON_UP
};

struct event
{

    struct window_resize_event
    {
        unsigned int width;
        unsigned int height;
    };

    struct key_event
    {
        keyboard::key key;
        keyboard::scan_code scan_code;
    };

    struct text_input_event
    {
        char32_t unicode;
    };

    struct mouse_move_event
    {
        int x;
        int y;
    };

    struct mouse_button_event
    {
        mouse::button button;
        int x;
        int y;
    };

    struct mouse_scroll_event
    {
        mouse::button button;
        float delta;
        int x;
        int y;
    };

    event_type type;

    union
    {
        window_resize_event window_resize;
        key_event key;
        text_input_event text_input;
        mouse_move_event mouse_move;
        mouse_button_event mouse_button;
        mouse_scroll_event mouse_scroll;
    };

};

}
}

#endif