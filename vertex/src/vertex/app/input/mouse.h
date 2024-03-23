#pragma once

#if defined(VX_APPLICATION)

namespace vx {
namespace app {
namespace mouse {

enum class button : int
{
    NONE        = 0,

    BUTTON_1    = 1,
    BUTTON_2    = 2,
    BUTTON_3    = 3,
    BUTTON_4    = 4,
    BUTTON_5    = 5,
    BUTTON_6    = 6,
    BUTTON_7    = 7,
    BUTTON_8    = 8,
    BUTTON_9    = 9,

    LEFT        = BUTTON_1,
    RIGHT       = BUTTON_2,
    MIDDLE      = BUTTON_3,

    WHEEL_UP    = BUTTON_4,
    WHEEL_DOWN  = BUTTON_5,
    WHEEL_LEFT  = BUTTON_6,
    WHEEL_RIGHT = BUTTON_7,

    EXTRA_1     = BUTTON_8,
    EXTRA_2     = BUTTON_9
};

}
}
}

#endif