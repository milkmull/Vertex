#pragma once

#include <cstdint>

#include "vertex/system/compiler.hpp"

namespace vx {
namespace os {

enum class power_state
{
    unknown,
    on_battery,
    no_battery,
    charging,
    charged
};

enum : uint8_t { battery_percent_unknown = 0xFF };

struct power_info
{
    power_state state;
    uint8_t percent;
};

VX_API bool get_power_info(power_info& info);

} // namespace os
} // namespace vx