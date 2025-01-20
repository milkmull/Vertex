#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
// Seems like this must be included after windows.h to have access to K32GetPerformanceInfo()
#include <psapi.h> // Performance Info

#include "vertex_impl/os/_platform/windows/windows_memory.hpp"

namespace vx {
namespace os {

bool get_memory_info_impl(memory_info& info)
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