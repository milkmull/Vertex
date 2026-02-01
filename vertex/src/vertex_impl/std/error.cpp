#include <cstdio>
#include <cstring>
#include <stdarg.h>

#include "vertex/std/error.hpp"
#include "vertex/os/thread.hpp"

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
static error_hook_t s_hook = nullptr;

//=============================================================================
// error accessors and manipulators
//=============================================================================

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
    if (err == none)
    {
        s_err.err = err;
        s_err.message[0] = 0;
        return;
    }

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

    if (s_hook && !s_hook(err, out, os::this_thread::get_id()))
    {
        return;
    }

    s_err.err = err;
}

void set(code err) noexcept
{
    set(err, code_to_string(err));
}

//=============================================================================
// error hook
//=============================================================================

VX_API void set_hook(error_hook_t hook) noexcept
{
    s_hook = hook;
}

VX_API error_hook_t get_hook() noexcept
{
    return s_hook;
}

VX_API bool print_error_hook(code err, const char* msg, os::thread_id thread)
{
    const char* safe_msg = msg ? msg : "(null)";

    std::fprintf(stderr,
        "error: code=%u thread=%llu msg=%s\n",
        static_cast<unsigned>(err),
        static_cast<unsigned long long>(thread),
        safe_msg);

    return true;
}

} // namespace err
} // namespace vx
