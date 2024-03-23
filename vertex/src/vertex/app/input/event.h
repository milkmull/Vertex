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
    WINDOW_CLOSE,
    WINDOW_RESIZE,
    WINDOW_MOVE,
    WINDOW_ICONIFY,
    WINDOW_MAXIMIZE,
    WINDOW_FOCUS,

    KEY_DOWN,
    KEY_REPEAT,
    KEY_UP,

    TEXT_INPUT,

    CURSOR_MOVE,
    CURSOR_ENTER,

    MOUSE_BUTTON_DOWN,
    MOUSE_BUTTON_UP,
    MOUSE_SCROLL,

    JOYSTICK_BUTTON_DOWN,
    JOYSTICK_BUTTON_UP
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

    event_type type;

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
    };

};

}
}

#endif