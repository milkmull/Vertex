#pragma once

#include <vector>

#include "vertex/config/language_config.hpp"
#include "vertex/app/id.hpp"

namespace vx {
namespace app {
namespace pen {

///////////////////////////////////////////////////////////////////////////////
// pen
///////////////////////////////////////////////////////////////////////////////

using pen_id = id_type;
enum : pen_id { DEFAULT_PEN_ID = DEFAULT_ID };

enum pen_input_flags
{
    PEN_INPUT_DOWN        = VX_BIT(0),
    PEN_INPUT_BUTTON_1    = VX_BIT(1),
    PEN_INPUT_BUTTON_2    = VX_BIT(2),
    PEN_INPUT_BUTTON_3    = VX_BIT(3),
    PEN_INPUT_BUTTON_4    = VX_BIT(4),
    PEN_INPUT_BUTTON_5    = VX_BIT(5),
    PEN_INPUT_ERASER_TIP  = VX_BIT(30)
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

enum pen_device_type
{

};

} // namespace pen
} // namespace app
} // namespace vx