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
enum : pen_id { default_pen_id = default_id };

enum pen_input_flags
{
    input_down        = VX_BIT(0),
    input_button_1    = VX_BIT(1),
    input_button_2    = VX_BIT(2),
    input_button_3    = VX_BIT(3),
    input_button_4    = VX_BIT(4),
    input_button_5    = VX_BIT(5),
    input_eraser_tip  = VX_BIT(30)
};

enum pen_axis
{
    axis_pressure,
    axis_xtilt,
    axis_ytilt,
    axis_distance,
    axis_rotation,
    axis_slider,
    axis_tangential_pressure,
    axis_count
};

enum pen_device_type
{

};

} // namespace pen
} // namespace app
} // namespace vx