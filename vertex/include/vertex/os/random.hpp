#pragma once

#include <cstdint>

#include "vertex/config/language_config.hpp"

namespace vx {
namespace os {

/**
 * @brief Retrieve a sequence of random bytes (entropy) using system-specific methods.
 *
 * This function generates a sequence of random bytes using system-based entropy sources,
 * such as `/dev/urandom` on Unix-like systems or the Windows Crypto API on Windows systems.
 * It is commonly used for cryptographic or other applications that require secure random data.
 *
 * @param[out] bytes A pointer to a buffer where the random bytes will be stored.
 * @param[in] count The number of random bytes to generate.
 * @return `true` if the random bytes were successfully generated, `false` otherwise.
 *
 * @note The implementation may vary depending on the operating system, using platform-specific
 * methods to collect entropy.
 */
VX_API bool get_entropy(uint8_t* bytes, size_t count);

} // namespace os
} // namespace vx