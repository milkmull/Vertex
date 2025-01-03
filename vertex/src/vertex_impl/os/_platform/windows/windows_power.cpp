#include "vertex/system/platform_config.hpp"

#if defined(__VX_OS_WINDOWS_POWER)

#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
#include "vertex/os/power.hpp"

namespace vx {
namespace os {

// https://learn.microsoft.com/en-us/windows/win32/api/winbase/ns-winbase-system_power_status

VX_API bool get_power_info(power_info& info)
{
    info.state = power_state::UNKNOWN;
    info.percent = BATTERY_PERCENT_UNKNOWN;

    SYSTEM_POWER_STATUS status{};
    if (!GetSystemPowerStatus(&status))
    {
        windows::error_message("GetSystemPowerStatus()");
        return false;
    }

    if (status.BatteryFlag == BATTERY_FLAG_UNKNOWN)
    {
        info.state = power_state::UNKNOWN;
    }
    else if (status.BatteryFlag == BATTERY_FLAG_NO_BATTERY)
    {
        info.state = power_state::NO_BATTERY;
    }
    else if (status.BatteryFlag == BATTERY_FLAG_CHARGING)
    {
        info.state = power_state::CHARGING;
    }
    else if (status.ACLineStatus == AC_LINE_ONLINE)
    {
        info.state = power_state::CHARGED;
    }
    else
    {
        info.state = power_state::ON_BATTERY;
    }

    if (status.BatteryLifePercent != BATTERY_PERCENTAGE_UNKNOWN)
    {
        info.percent = std::min(
            static_cast<uint8_t>(status.BatteryLifePercent),
            static_cast<uint8_t>(100)
        );
    }

    return true;
}

} // namespace os
} // namespace vx

#endif // VX_OS_WINDOWS_POWER
