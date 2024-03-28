#pragma once

#include <array>

namespace vx {
namespace app {
namespace joystick {

enum joystick : int
{
    JOYSTICK_1          = 0,
    JOYSTICK_2          = 1,
    JOYSTICK_3          = 2,
    JOYSTICK_4          = 3,
    JOYSTICK_5          = 4,
    JOYSTICK_6          = 5,
    JOYSTICK_7          = 6,
    JOYSTICK_8          = 7,
    JOYSTICK_9          = 8,
    JOYSTICK_10         = 9,
    JOYSTICK_11         = 10,
    JOYSTICK_12         = 11,
    JOYSTICK_13         = 12,
    JOYSTICK_14         = 13,
    JOYSTICK_15         = 14,
    JOYSTICK_16         = 15,
};

enum button : int
{
    BUTTON_UNKNOWN             = -1,

    BUTTON_A                   = 0,
    BUTTON_B                   = 1,
    BUTTON_X                   = 2,
    BUTTON_Y                   = 3,

    BUTTON_CROSS               = BUTTON_A,
    BUTTON_CIRCLE              = BUTTON_B,
    BUTTON_SQUARE              = BUTTON_X,
    BUTTON_TRIANGLE            = BUTTON_Y,

    BUTTON_LEFT_BUMPER         = 4,
    BUTTON_RIGHT_BUMPER        = 5,

    BUTTON_BACK                = 6,
    BUTTON_START               = 7,
    BUTTON_GUIDE               = 8,

    BUTTON_LEFT_THUMB          = 9,
    BUTTON_RIGHT_THUMB         = 10,

    BUTTON_DPAD_UP             = 11,
    BUTTON_DPAD_RIGHT          = 12,
    BUTTON_DPAD_DOWN           = 13,
    BUTTON_DPAD_LEFT           = 14
};

enum axis : int
{
    AXIS_LEFT_X              = 0,
    AXIS_LEFT_Y              = 1,
    AXIS_RIGHT_X             = 2,
    AXIS_RIGHT_Y             = 3,
    AXIS_LEFT_TRIGGER        = 4,
    AXIS_RIGHT_TRIGGER       = 5
};

struct state
{
    std::array<bool, 15> buttons;
    std::array<float, 6> axes;
};

bool is_present(joystick id);
const char* get_name(joystick id);
state get_state(joystick id);

}
}
}