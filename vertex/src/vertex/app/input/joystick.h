#pragma once

#if defined(VX_APPLICATION)

#include <vector>

namespace vx {
namespace app {
namespace joystick {

enum class button : int
{
    UNKNOWN = -1,

    A = 0,
    B = 1,
    X = 2,
    Y = 3,

    BACK = 4,
    GUIDE = 5,
    START = 6,

    LEFT_STICK = 7,
    RIGHT_STICK = 8,

    LEFT_SHOULDER = 9,
    RIGHT_SHOULDER = 10,

    DPAD_UP = 11,
    DPAD_DOWN = 12,
    DPAD_LEFT = 13,
    DPAD_RIGHT = 14,

    PADDLE_1 = 15,
    PADDLE_2 = 16,
    PADDLE_3 = 17,
    PADDLE_4 = 18,

    TOUCH_PAD = 19,

    EXTRA_1 = 20
};

struct state
{
    unsigned char buttons[15];
    float axes[6];
};

bool is_present(int id);
const char* get_name(int id);
state get_state(int id);

}
}
}

#endif