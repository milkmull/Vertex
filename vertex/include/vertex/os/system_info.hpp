#pragma once

#include <string>

#include "vertex/config/os.hpp"
#include "vertex/config/language_config.hpp"

namespace vx {
namespace os {

constexpr const char* get_name()
{
#if defined(VX_OS_CYGWIN)
    return "Cygwin";

#elif defined(VX_OS_LINUX)
    return "Linux";

#elif defined(VX_OS_IOS)
    return "iOS";
#elif defined(VX_OS_MACOS)
    return "Mac OS";

#elif defined(VX_OS_BSD_BSDI)
    return "BSDi BSD/OS"
#elif defined(VX_OS_DRAGONFLY_BSD)
    return "DragonFly BSD"
#elif defined(VX_OS_BSD_FREE)
    return "Free BSD"
#elif defined(VX_OS_BSD_NET)
    return "NetBSD"
#elif defined(VX_OS_BSD_OPEN)
    return "OpenBSD"
#elif defined(VX_OS_BSD)
    return "BSD"

#elif defined(VX_OS_WINDOWS)
    return "Windows";

#else
    return "Dummy";

#endif
}

VX_API std::string get_model_name();
VX_API std::string get_system_name();
VX_API std::string get_distribution_name();

VX_API std::string get_processor_name();
VX_API uint32_t get_processor_count();

VX_API bool get_version(uint32_t* major, uint32_t* minor, uint32_t* patch);

} // namespace os
} // namespace vx