#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/os/error_code.hpp"
#include "vertex/os/native_string.hpp"

namespace vx {
namespace os {

VX_API size_t format_error(native_error error, native_buffer buffer, size_t size);

inline size_t format_last_error(native_buffer buffer, size_t size)
{
    const auto error = get_last_error();
    return format_error(error, buffer, size);
}

} // namespace os
} // namespace vx
