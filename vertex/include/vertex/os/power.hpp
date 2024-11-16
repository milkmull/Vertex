#pragma once

#include <cstdint>

#include "vertex/system/compiler.hpp"

namespace vx {
namespace os {

enum class power_state
{
    UNKNOWN,
    ON_BATTERY,
    NO_BATTERY,
    CHARGING,
    CHARGED
};

enum : uint8_t { BATTERY_PERCENT_UNKNOWN = 0xFF };

struct power_info
{
    power_state state;
    uint8_t percent;
};

VX_API bool get_power_info(power_info& info);

} // namespace os
} // namespace vx