#pragma once

#include <vector>

#include "vertex/config/language_config.hpp"

namespace vx {
namespace app {
namespace pen {

///////////////////////////////////////////////////////////////////////////////
// pen
///////////////////////////////////////////////////////////////////////////////

namespace _priv { class pen_internal; }

using pen_id = size_t;

enum pen_input_flags
{
    PEN_INPUT_DOWN        = (1u << 0),
    PEN_INPUT_BUTTON_1    = (1u << 1),
    PEN_INPUT_BUTTON_2    = (1u << 2),
    PEN_INPUT_BUTTON_3    = (1u << 3),
    PEN_INPUT_BUTTON_4    = (1u << 4),
    PEN_INPUT_BUTTON_5    = (1u << 5),
    PEN_INPUT_ERASER_TIP  = (1u << 30)
};

enum pen_axis
{
    PEN_AXIS_PRESSURE,
    PEN_AXIS_XTILT,
    PEN_AXIS_YTILT,
    PEN_AXIS_DISTANCE,
    PEN_AXIS_ROTATION,
    PEN_AXIS_SLIDER,
    PEN_AXIS_TANGENTIAL_PRESSURE,
    PEN_AXIS_COUNT
};

} // namespace pen
} // namespace app
} // namespace vx