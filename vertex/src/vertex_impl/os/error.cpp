#include "vertex/config/language_config.hpp"
#include "vertex_impl/os/_platform/platform_error.hpp"

namespace vx {
namespace os {

error_type get_last_error()
{
    return get_last_error_impl();
}

void clear_last_error()
{
    get_last_error_impl();
}

size_t format_error(error_type error, char* buffer, size_t size)
{
    return format_error_impl(error, buffer, size);
}

} // namespace os
} // namespace vx
