#include "vertex/std/error.hpp"
#include "vertex/os/error.hpp"
#include "vertex/os/thread.hpp"
#include "vertex_impl/os/_platform/platform_io.hpp"

#if !defined(VX_ERR_BUFFER_MAX_SIZE)
    #define VX_ERR_BUFFER_MAX_SIZE 1024
#endif // VX_ERR_BUFFER_MAX_SIZE

namespace vx {
namespace err {

struct error_writer
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

    void format_message(error_type e, const char* msg, const char* function, const char* file, int line)
    {
        const char start_text[] = "[SYSTEM ERROR]: ";
        const char function_text[] = "function: ";
        const char file_text[] = "file: ";
        const char line_text[] = "line: ";
        const char separator_text[] = " | ";

        // start
        append_literal(start_text);

        // error code
        append_number(e);

        if (msg)
        {
            append_literal(separator_text);
            append_cstr(msg);
        }

        if (function)
        {
            append_literal(separator_text);
            append_literal(function_text);
            append_cstr(function);
        }

        if (file)
        {
            append_literal(separator_text);
            append_literal(file_text);
            append_cstr(file);
        }

        if (line > 0)
        {
            append_literal(separator_text);
            append_literal(line_text);
            append_number(line);
        }

        *ptr = '\0';
    }
};

struct info_impl
{
    error_type err;
    char message[VX_ERR_BUFFER_MAX_SIZE + 1];
    size_t message_size;
    os::thread_id thread;

private:

    void format_message(const char* msg, const char* function, const char* file, int line)
    {
        error_writer writer{ message, VX_ERR_BUFFER_MAX_SIZE };
        writer.format_message(err, msg, function, file, line);
        message_size = (VX_ERR_BUFFER_MAX_SIZE - writer.remaining);
    }

public:

    void update(error_type e, const char* msg, const char* function, const char* file, int line)
    {
        err = e;
        format_message(msg, function, file, line);
        thread = os::this_thread::get_id();
    }

    void clear()
    {
        err = error_type{};
        message[0] = '\0';
        message_size = 0;
    }
};

static thread_local info_impl s_err = {};
static error_hook_t s_hook = nullptr;

//=============================================================================
// error accessors and manipulators
//=============================================================================

error_info get() noexcept
{
    return error_info{ s_err.err, s_err.message };
}

error_type get_code() noexcept
{
    return s_err.err;
}

error_string get_message() noexcept
{
    return { s_err.message, s_err.message_size };
}

void set(error_type e, const char* msg, const char* function, const char* file, int line)
{
    if (e == error_type{})
    {
        s_err.clear();
        return;
    }

    if (!s_hook)
    {
        s_err.update(e, msg, function, file, line);
        return;
    }

    info_impl tmp;
    tmp.update(e, msg, function, file, line);

    const error_info info{ tmp.err, tmp.message, tmp.thread };
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

    const auto e = os::get_last_error();
    const size_t written = os::format_error(e, buffer, buffer_size);
    buffer[written] = '\0';

    set(e, buffer, message, nullptr, 0);
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

static void safe_print_impl(const char* data, const size_t count) noexcept
{
    os::write_raw_impl(os::stream::err, data, count, false);
}

    #if !defined(VX_ERR_DISABLE_PRINT_ERROR_HOOK)

bool print_error_hook(error_info info)
{
    if (!info.message.size == 0)
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

void safe_print(const char* data, const size_t count)
{
    if (!data || count == 0)
    {
        return;
    }

    safe_print_impl(data, count);
}

} // namespace err
} // namespace vx
