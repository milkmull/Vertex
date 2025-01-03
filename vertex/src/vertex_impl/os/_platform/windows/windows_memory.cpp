#include "vertex/system/platform_config.hpp"

#if defined(__VX_OS_WINDOWS_MEMORY)

#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
#include "vertex/os/memory.hpp"

namespace vx {
namespace os {

VX_API bool get_memory_info(memory_info& info)
{
    PERFORMANCE_INFORMATION pi{ sizeof(PERFORMANCE_INFORMATION) };

    if (!GetPerformanceInfo(&pi, pi.cb))
    {
        windows::error_message("GetPerformanceInfo()");
        return false;
    }

    info.physical = static_cast<size_t>(pi.PhysicalTotal * pi.PageSize);
    info.free = static_cast<size_t>(pi.PhysicalAvailable * pi.PageSize);
    info.available = static_cast<size_t>(pi.CommitLimit * pi.PageSize);

    return true;
}

} // namespace os
} // namespace vx

#endif // VX_OS_WINDOWS_MEMORY
