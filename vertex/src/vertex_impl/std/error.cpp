#include "vertex/std/error.hpp"
#include "vertex/os/error.hpp"
#include "vertex/os/thread.hpp"
#include "vertex_impl/os/_platform/platform_io.hpp"

#if !defined(VX_ERR_BUFFER_MAX_SIZE)
    #define VX_ERR_BUFFER_MAX_SIZE 1024
#endif // VX_ERR_BUFFER_MAX_SIZE

namespace vx {
namespace err {

//=============================================================================
// low-level formatting helper
//=============================================================================

struct error_formatter
{
    using C = char;
    using traits = str::char_traits<C>;

    char* ptr;
    size_t remaining;

    void append(const char* data, const size_t count)
    {
        if (remaining == 0)
        {
            return;
        }

        const size_t copy_count = (count <= remaining)
            ? count
            : remaining;

        ptr = traits::copy(ptr, data, copy_count);
        remaining -= copy_count;
    }

    // For string-literal labels: deduces the array length at compile time so
    // we never copy the literal's trailing '\0' into the middle of the buffer.
    template <size_t N>
    void append_literal(const char (&data)[N])
    {
        append(data, N - 1);
    }

    void append_cstr(const char* data)
    {
        append(data, str::length(data));
    }

    template <typename I>
    void append_number(I num)
    {
        constexpr size_t buffer_size = strconv::integer_buffer_traits<I>::buffer_size;
        char buffer[buffer_size];
        const auto result = strconv::write_integer_base10(num, buffer, buffer_size);
        append(buffer, result.count);
    }

    void format_message(bool system_error, error_type e, const char* msg, const error_site& site)
    {
        const char error_prefix[] = "[ERROR]: ";
        const char system_error_prefix[] = "[SYSTEM ERROR]: ";
        const char function_text[] = "function: ";
        const char file_text[] = "file: ";
        const char line_text[] = "line: ";
        const char separator_text[] = " | ";

        // prefix
        if (system_error)
        {
            append_literal(system_error_prefix);
        }
        else
        {
            append_literal(error_prefix);
        }

        // error code
        append_number(e);

        if (msg)
        {
            append_literal(separator_text);
            append_cstr(msg);
        }

        if (site.function)
        {
            append_literal(separator_text);
            append_literal(function_text);
            append_cstr(site.function);
        }

        if (site.file)
        {
            append_literal(separator_text);
            append_literal(file_text);
            append_cstr(site.file);
        }

        if (site.line > 0)
        {
            append_literal(separator_text);
            append_literal(line_text);
            append_number(site.line);
        }

        *ptr = '\0';
    }
};

//=============================================================================
// thread-local error state
//=============================================================================

struct info_impl
{
    bool is_system_error;
    error_type err;
    char message[VX_ERR_BUFFER_MAX_SIZE + 1];
    size_t message_size;
    os::thread_id thread;

private:

    void format_message(const char* msg, const error_site& site)
    {
        error_formatter fmt{ message, VX_ERR_BUFFER_MAX_SIZE };
        fmt.format_message(is_system_error, err, msg, site);
        message_size = (VX_ERR_BUFFER_MAX_SIZE - fmt.remaining);
    }

public:

    void update(bool system_error, error_type e, const char* msg, const error_site& site)
    {
        is_system_error = system_error;
        err = e;
        format_message(msg, site);
        thread = os::this_thread::get_id();
    }

    void clear()
    {
        is_system_error = false;
        err = error_type{};
        message[0] = '\0';
        message_size = 0;
    }
};

static thread_local info_impl s_err = {};
static error_hook_t s_hook = nullptr;

//=============================================================================
// printing
//=============================================================================

static void safe_print_impl(const char* data, const size_t count) noexcept
{
    os::write_raw_impl(os::stream::err, data, count, false);
}

void safe_print(const char* data, const size_t count)
{
    if (!data || count == 0)
    {
        return;
    }

    safe_print_impl(data, count);
}

//=============================================================================
// error formatting
//=============================================================================

void print_error(
    bool system_error,
    error_type e,
    const char* msg,
    const error_site& site) noexcept
{
    // Local buffer only - deliberately not thread-local and not shared with
    // any other error state. This runs on fatal/unrecoverable paths, where
    // we want zero dependency on shared mutable state that might itself be
    // in a bad way by the time we get here.
    constexpr size_t buffer_size = VX_ERR_BUFFER_MAX_SIZE;
    char buffer[buffer_size + 1];

    error_formatter fmt{ buffer, buffer_size };
    fmt.format_message(system_error, e, msg, site);
    const size_t written = buffer_size - fmt.remaining;

    safe_print(buffer, written);
}

//=============================================================================
// error accessors and manipulators
//=============================================================================

error_info get() noexcept
{
    return error_info{
        s_err.is_system_error,
        s_err.err,
        { s_err.message, s_err.message_size },
        s_err.thread
    };
}

error_type get_code() noexcept
{
    return s_err.err;
}

error_string get_message() noexcept
{
    return { s_err.message, s_err.message_size };
}

void _err_priv::set_impl(bool system_error, error_type e, const char* msg, const error_site& site)
{
    if (e == error_type{})
    {
        s_err.clear();
        return;
    }

    if (!s_hook)
    {
        s_err.update(system_error, e, msg, site);
        return;
    }

    info_impl tmp;
    tmp.update(system_error, e, msg, site);

    const error_info info{
        tmp.is_system_error,
        tmp.err,
        { tmp.message, tmp.message_size },
        tmp.thread
    };

    if (!s_hook(info))
    {
        s_err.clear();
        return;
    }

    s_err = tmp;
}

void set_last_os_error(const char* message)
{
    constexpr size_t buffer_size = VX_ERR_BUFFER_MAX_SIZE;
    char buffer[buffer_size + 1];

    error_formatter fmt{ buffer, buffer_size };

    // fold the caller-supplied context message in ahead of the OS-provided
    // error text, instead of dropping it
    if (message)
    {
        fmt.append_cstr(message);
        fmt.append_literal(": ");
    }

    const auto e = os::get_last_error();
    const size_t os_msg_capacity = fmt.remaining;
    const size_t os_msg_size = os::format_error(e, fmt.ptr, os_msg_capacity);

    fmt.ptr += os_msg_size;
    fmt.remaining -= os_msg_size;
    *fmt.ptr = '\0';

    _err_priv::set_impl(true, e, buffer, {});
}

//=============================================================================
// error hook
//=============================================================================

#if !defined(VX_ERR_DISABLE_HOOK)

void set_hook(error_hook_t hook) noexcept
{
    s_hook = hook;
}

error_hook_t get_hook() noexcept
{
    return s_hook;
}

    #if !defined(VX_ERR_DISABLE_PRINT_ERROR_HOOK)

bool print_error_hook(error_info info)
{
    if (info.message.size != 0)
    {
        safe_print(info.message.data, info.message.size);

        const char thread_text[] = " | thread: ";
        safe_print_impl(thread_text, mem::literal_size(thread_text));

        // thread id
        {
            using I = os::thread_id;
            using traits = strconv::integer_buffer_traits<I>;

            constexpr size_t buffer_size = traits::buffer_size;
            char buffer[buffer_size];

            const auto result = strconv::write_integer_base10(info.thread, buffer, buffer_size);
            safe_print_impl(buffer, result.count);
        }
    }

    return true;
}

    #endif // !defined(VX_ERR_DISABLE_PRINT_ERROR_HOOK)

#endif // !defined(VX_ERR_DISABLE_HOOK)

} // namespace err
} // namespace vx
