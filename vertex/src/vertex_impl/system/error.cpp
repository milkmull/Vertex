#include "vertex/system/error.hpp"

#if VX_ERROR_PRINTING_AVAILABLE
#   include <iostream>
#endif

namespace vx {
namespace err {

struct info_impl
{
    code code;
    char message[ERROR_MESSAGE_MAX_SIZE + 1];
};

static thread_local info_impl s_err;

///////////////////////////////////////////////////////////////////////////////
// error printing
///////////////////////////////////////////////////////////////////////////////

#if VX_ERROR_PRINTING_AVAILABLE

static bool s_print_errors = false;

VX_API void __detail::set_error_printing_enabled(bool enabled) noexcept
{
    s_print_errors = enabled;
}

#endif // VX_ERROR_PRINTING_AVAILABLE

///////////////////////////////////////////////////////////////////////////////
// error accessors and manipulators
///////////////////////////////////////////////////////////////////////////////

VX_API info get() noexcept
{
    return { s_err.code, s_err.message };
}

VX_API void set(code code, const char* msg) noexcept
{
#if (VX_ERROR_PRINTING_AVAILABLE)

    if (s_print_errors)
    {
        std::cerr << "[ERROR] " << static_cast<int>(code) << ": " << msg << std::endl;
    }

#endif // VX_ERROR_PRINTING_AVAILABLE

    s_err.code = code;

    constexpr size_t max_size = ERROR_MESSAGE_MAX_SIZE;
    const size_t msg_size = std::strlen(msg);
    std::memcpy(s_err.message, msg, std::min(max_size, msg_size));
}

} // namespace err
} // namespace vx