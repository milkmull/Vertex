#pragma once

#if defined(VX_APPLICATION)

#include "keyboard.h"
#include "mouse.h"
#include "joystick.h"

// https://www.glfw.org/docs/3.3/input_guide.html

namespace vx {
namespace app {
namespace event {

enum category : int
{
    CATEGORY_WINDOW,
    CATEGORY_KEY,
    CATEGORY_MOUSE,
    CATEGORY_JOYSTICK
};

enum type : int
{
    TYPE_WINDOW_CLOSE,
    TYPE_WINDOW_RESIZE,
    TYPE_WINDOW_MOVE,
    TYPE_WINDOW_ICONIFY,
    TYPE_WINDOW_MAXIMIZE,
    TYPE_WINDOW_FOCUS,

    TYPE_KEY_DOWN,
    TYPE_KEY_REPEAT,
    TYPE_KEY_UP,

    TYPE_TEXT_INPUT,

    TYPE_CURSOR_MOVE,
    TYPE_CURSOR_ENTER,

    TYPE_MOUSE_BUTTON_DOWN,
    TYPE_MOUSE_BUTTON_UP,
    TYPE_MOUSE_SCROLL,

    TYPE_JOYSTICK_CONNECTION
};

struct event
{
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

    struct state_event
    {
        bool value;
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
        mouse::button button;
        float x;
        float y;
    };

    struct joystick_connection_event
    {
        joystick::joystick id;
        bool connected;
    };

    type type;

    union
    {
        size_event window_resize;
        move_event window_move;
        state_event window_iconify;
        state_event window_maximize;
        state_event window_focus;

        key_event key;
        text_input_event text_input;

        move_event cursor_move;
        state_event cursor_enter;

        mouse_button_event mouse_button;
        mouse_scroll_event mouse_scroll;

        state_event joystick_connection;
    };

};

using callback_fn = void(*)(event&);

}
}
}

#endif