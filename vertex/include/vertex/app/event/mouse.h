#pragma once

#include "vertex/math/math/type/vec2i.h"

namespace vx {
namespace app {

class window;

namespace mouse {

enum button : int
{
    NONE = 0,

    BUTTON_1 = 1,
    BUTTON_2 = 2,
    BUTTON_3 = 3,
    BUTTON_4 = 4,
    BUTTON_5 = 5,
    BUTTON_6 = 6,
    BUTTON_7 = 7,
    BUTTON_8 = 8,
    BUTTON_9 = 9,

    BUTTON_LEFT = BUTTON_1,
    BUTTON_RIGHT = BUTTON_2,
    BUTTON_MIDDLE = BUTTON_3,

    BUTTON_WHEEL_UP = BUTTON_4,
    BUTTON_WHEEL_DOWN = BUTTON_5,
    BUTTON_WHEEL_LEFT = BUTTON_6,
    BUTTON_WHEEL_RIGHT = BUTTON_7,

    BUTTON_EXTRA_1 = BUTTON_8,
    BUTTON_EXTRA_2 = BUTTON_9
};

enum wheel : int
{
    VERTICAL,
    HORIZONTAL
};

math::vec2i get_mouse_position();
void set_mouse_position(const math::vec2i& position);

bool is_mouse_button_down(button button);

}
}
}