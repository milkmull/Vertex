#include "vertex/system/platform_config.hpp"
#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/_platform/windows/windows_shell.hpp"
#else
#   include "vertex_impl/os/_platform/dummy/dummy_shell.hpp"
#endif

namespace vx {
namespace os {

VX_API bool shell_open(const std::string& uri)
{
    return shell_open_impl(uri);
}

} // namespace os
} // namespace vx