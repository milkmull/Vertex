#include "vertex/system/platform_config.hpp"
#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/_platform/windows/windows_memory.hpp"
#else
#   include "vertex_impl/os/_platform/dummy/dummy_memory.hpp"
#endif

namespace vx {
namespace os {

VX_API bool get_memory_info(memory_info& info)
{
    return get_memory_info_impl(info);
}

} // namespace os
} // namespace vx