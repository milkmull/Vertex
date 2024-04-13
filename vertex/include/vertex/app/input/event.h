#pragma once

#include "keyboard.h"
#include "mouse.h"
#include "joystick.h"

// https://www.glfw.org/docs/3.3/input_guide.html

namespace vx {
namespace app {

enum category : int
{
    CATEGORY_WINDOW,
    CATEGORY_KEY,
    CATEGORY_MOUSE,
    CATEGORY_JOYSTICK
};

enum event_type : int
{
    WINDOW_CLOSE,
    WINDOW_RESIZE,
    WINDOW_MINIMIZE,
    WINDOW_MAXIMIZE,
    WINDOW_MOVE,
    WINDOW_FOCUS,
    WINDOW_SHOW,

    KEY_DOWN,
    KEY_REPEAT,
    KEY_UP,

    TEXT_INPUT,

    MOUSE_MOVE,
    MOUSE_HOVER,

    MOUSE_BUTTON_DOWN,
    MOUSE_BUTTON_UP,
    MOUSE_SCROLL,

    JOYSTICK_CONNECTION
};

struct event
{
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

    struct joystick_connection_event
    {
        joystick::joystick id;
        bool connected;
    };

    event_type type;

    union
    {
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

        state_event joystick_connection;
    };

};

using event_callback_fn = void(*)(event&);

}
}