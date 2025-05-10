#pragma once

#include <string>

#include "vertex/config/os.hpp"
#include "vertex/config/language_config.hpp"

namespace vx {
namespace os {

/**
 * @brief Returns the name of the operating system.
 *
 * This function returns a string representing the name of the operating system
 * based on the pre-defined OS macros. These macros are typically defined at
 * compile time, depending on the platform.
 *
 * Possible return values:
 * - "Cygwin" for Cygwin
 * - "Linux" for Linux-based systems
 * - "iOS" for iOS
 * - "Mac OS" for macOS
 * - "BSD" for various BSD flavors (e.g., FreeBSD, OpenBSD, NetBSD)
 * - "Windows" for Windows-based systems
 * - "Dummy" if no recognized OS macro is defined
 *
 * @return The name of the operating system.
 */
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

/**
 * @brief Retrieves the model name of the system.
 *
 * This function attempts to return the model name of the system (e.g., "MacBook Pro").
 * The actual method of retrieving this information is platform-dependent.
 *
 * @return The model name as a string.
 */
VX_API std::string get_model_name();

/**
 * @brief Retrieves the system's hostname or computer name.
 *
 * This function returns the name of the system (e.g., the machine's hostname or
 * computer name) depending on the platform.
 * The system name (hostname) is typically used to uniquely identify a machine
 * on a network.
 *
 * @return The system's hostname or computer name as a string.
 */
VX_API std::string get_system_name();

/**
 * @brief Retrieves the distribution name of the system (e.g., "Ubuntu", "Debian").
 *
 * This function is typically used to get information about the OS distribution
 * (for Linux-like systems).
 *
 * @return The distribution name as a string.
 */
VX_API std::string get_distribution_name();

/**
 * @brief Retrieves the processor name (e.g., "Intel Core i7").
 *
 * This function attempts to return the processor name of the system.
 * The actual method of retrieving this information is platform-dependent.
 *
 * @return The processor name as a string.
 */
VX_API std::string get_processor_name();

/**
 * @brief Retrieves the number of processors (or CPU cores) on the system.
 *
 * This function returns the number of processors or cores available on the system.
 *
 * @return The number of processors (cores) available.
 */
VX_API uint32_t get_processor_count();

/**
 * @brief Retrieves the version of the system (major, minor, and patch).
 *
 * This function attempts to get the version of the operating system, breaking it down
 * into major, minor, and patch version components.
 *
 * @param[out] major A pointer to store the major version number.
 * @param[out] minor A pointer to store the minor version number.
 * @param[out] patch A pointer to store the patch version number.
 * @return `true` if the version was successfully retrieved, `false` otherwise.
 */
VX_API bool get_version(uint32_t* major, uint32_t* minor, uint32_t* patch);

} // namespace os
} // namespace vx