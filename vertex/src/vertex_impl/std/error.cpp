#include <cstdio>
#include <cstring>
#include <stdarg.h>

#include "vertex/std/error.hpp"

#if VX_ERROR_PRINTING_AVAILABLE
#   include <iostream>
#endif

namespace vx {
namespace err {

enum : size_t
{
    error_message_max_size = 1024
};

struct info_impl
{
    code err;
    char message[error_message_max_size + 1];
};

static thread_local info_impl s_err;

///////////////////////////////////////////////////////////////////////////////
// error printing
///////////////////////////////////////////////////////////////////////////////

static bool s_print_errors = false;

#if VX_ERROR_PRINTING_AVAILABLE

void _priv::set_error_printing_enabled(bool enabled)
{
    s_print_errors = enabled;
}

#endif // VX_ERROR_PRINTING_AVAILABLE

///////////////////////////////////////////////////////////////////////////////
// error accessors and manipulators
///////////////////////////////////////////////////////////////////////////////

void print(const char* msg)
{
    if (!msg)
    {
        return;
    }

    std::fwrite(msg, 1, std::strlen(msg), stderr);
    std::fputc('\n', stderr);
    std::fflush(stderr);
}

code get_code() noexcept
{
    return s_err.err;
}

const char* get_message() noexcept
{
    return s_err.message;
}

void set(code err, const char* msg, const char* function, const char* file, int line)
{
    char* out = s_err.message;
    constexpr size_t cap = error_message_max_size;
    size_t used = 0;

    auto append = [&](const char* fmt, ...)
    {
        if (used >= cap)
        {
            return;
        }

        va_list ap;
        va_start(ap, fmt);
        int n = std::vsnprintf(out + used, cap - used, fmt, ap);
        va_end(ap);

        if (n > 0)
        {
            if (static_cast<size_t>(n) > cap - used)
            {
                used = cap; // truncated
            }
            else
            {
                used += static_cast<size_t>(n);
            }
        }
    };

    append("[ERROR] %s", msg ? msg : code_to_string(err));

    if (function)
    {
        append(" | function: %s", function);
    }
    if (file)
    {
        append(" | file: %s", file);
    }
    if (line > 0)
    {
        append(" | line: %d", line);
    }

#if (VX_ERROR_PRINTING_AVAILABLE)

    print_error(out);

#endif // VX_ERROR_PRINTING_AVAILABLE

    s_err.err = err;
}

void set(code err) noexcept
{
    set(err, code_to_string(err));
}

} // namespace err
} // namespace vx
