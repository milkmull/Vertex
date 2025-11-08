#include <cstring>
#include <string>

#include "vertex/system/error.hpp"

#if VX_ERROR_PRINTING_AVAILABLE
#   include <iostream>
#endif

namespace vx {
namespace err {

struct info_impl
{
    code err = none;
    std::string msg;
};

static thread_local info_impl s_err;

///////////////////////////////////////////////////////////////////////////////
// error printing
///////////////////////////////////////////////////////////////////////////////

#if VX_ERROR_PRINTING_AVAILABLE

static bool s_print_errors = false;

VX_API void _priv::set_error_printing_enabled(bool enabled) noexcept
{
    s_print_errors = enabled;
}

#endif // VX_ERROR_PRINTING_AVAILABLE

///////////////////////////////////////////////////////////////////////////////
// error accessors and manipulators
///////////////////////////////////////////////////////////////////////////////

VX_API info get() noexcept
{
    return { s_err.err, s_err.msg.c_str() };
}

VX_API void set(code err, const char* msg) noexcept
{
#if (VX_ERROR_PRINTING_AVAILABLE)

    if (s_print_errors && err != none)
    {
        std::cerr << "[ERROR] " << static_cast<int>(err) << ": " << msg << std::endl;
    }

#endif // VX_ERROR_PRINTING_AVAILABLE

    s_err.err = err;
    s_err.msg = msg;
}

} // namespace err
} // namespace vx