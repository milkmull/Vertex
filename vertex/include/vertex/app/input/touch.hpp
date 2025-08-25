#pragma once

#include <vector>

#include "vertex/config/language_config.hpp"

namespace vx {
namespace app {
namespace touch {

///////////////////////////////////////////////////////////////////////////////
// touch
///////////////////////////////////////////////////////////////////////////////

namespace _priv { class touch_internal; }

using touch_id = size_t;
using finger_id = size_t;

enum touch_device_type
{
    TOUCH_DEVICE_INVALID = -1,
    TOUCH_DEVICE_DIRECT,
    TOUCH_DEVICE_INDIRECT_ABSOLUTE,
    TOUCH_DEVICE_INDIRECT_RELATIVE
};

struct finger
{
    finger_id id;
    float x, y;
    float pressure;
};

///////////////////////////////////////////////////////////////////////////////
// touch
///////////////////////////////////////////////////////////////////////////////

VX_API std::vector<touch_id> list_touch_device_ids();
VX_API const char* get_touch_device_name(touch_id id);
VX_API touch_device_type get_touch_device_type(touch_id id);

VX_API std::vector<finger> get_touch_fingers(touch_id id);

} // namespace touch
} // namespace app
} // namespace vx