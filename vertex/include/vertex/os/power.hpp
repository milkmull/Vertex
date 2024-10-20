#pragma once

#include <cstdint>

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

enum : size_t  { BATTERY_SECONDS_UNKNOWN = -1 };
enum : uint8_t { BATTERY_PERCENT_UNKNOWN = 0xFF };

struct power_info
{
    power_state state;
    size_t seconds;
    uint8_t percent;
};

bool get_power_info(power_info& info);

} // namespace os
} // namespace vx