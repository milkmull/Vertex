#include "vertex/system/platform.hpp"

#if defined(VX_PLATFORM_WINDOWS)

#include "vertex_impl/_platform/_win32/win32_header.hpp"
#include "vertex/os/memory.hpp"

namespace vx {
namespace os {

bool get_memory_info(memory_info& info)
{
    PERFORMANCE_INFORMATION pi{ sizeof(PERFORMANCE_INFORMATION) };

    if (!GetPerformanceInfo(&pi, pi.cb))
    {
        WindowsErrorMessage("GetPerformanceInfo()");
        return false;
    }

    info.physical = static_cast<size_t>(pi.PhysicalTotal * pi.PageSize);
    info.free = static_cast<size_t>(pi.PhysicalAvailable * pi.PageSize);
    info.available = static_cast<size_t>(pi.CommitLimit * pi.PageSize);

    return true;
}

} // namespace os
} // namespace vx

#endif // VX_PLATFORM_WINDOWS
