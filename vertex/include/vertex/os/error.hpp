#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/os/error_type.hpp"

namespace vx {
namespace os {

VX_API error_type get_last_error();
VX_API void clear_last_error();

VX_API size_t format_error(error_type error, char* buffer, size_t size);

inline size_t format_last_error(char* buffer, size_t size)
{
    const auto error = get_last_error();
    return format_error(error, buffer, size);
}

} // namespace os
} // namespace vx
