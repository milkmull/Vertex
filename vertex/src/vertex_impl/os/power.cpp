#include "vertex/system/platform_config.hpp"

#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/_platform/windows/windows_power.hpp"
#endif

namespace vx {
namespace os {

VX_API bool get_power_info(power_info& info)
{
    return get_power_info_impl(info);
}

} // namespace os
} // namespace vx