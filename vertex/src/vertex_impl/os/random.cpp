#include "vertex/system/platform_config.hpp"

#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/_platform/windows/windows_random.hpp"
#endif

namespace vx {
namespace os {

VX_API bool get_entropy(uint8_t* bytes, size_t count)
{
    return get_entropy_impl(bytes, count);
}

} // namespace os
} // namespace vx