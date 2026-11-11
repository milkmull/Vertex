#include "vertex/std/error/fatal.hpp"

#if !defined(VX_ERR_BUFFER_MAX_SIZE)
    #define VX_ERR_BUFFER_MAX_SIZE 1024
#endif // VX_ERR_BUFFER_MAX_SIZE

namespace vx {
namespace err {

//=============================================================================
// formatter
//=============================================================================

struct error_formatter
{
    char* begin;
    char* ptr;
    size_t remaining;

    error_formatter(char* buffer, const size_t capacity) noexcept
        : begin(buffer)
        , ptr(buffer)
        , remaining(capacity > 0 ? capacity - 1 : 0)
    {
    }

    void append(const char* data, const size_t count) noexcept
    {
        const size_t copy_count = (count <= remaining) ? count : remaining;

        for (size_t i = 0; i < copy_count; ++i)
        {
            ptr[i] = data[i];
        }

        ptr += copy_count;
        remaining -= copy_count;
    }

    // For string-literal labels: deduces the array length at compile time so
    // we never copy the literal's trailing '\0' into the middle of the buffer.
    template <size_t N>
    void append_literal(const char (&data)[N]) noexcept
    {
        append(data, N - 1);
    }

    void append_cstr(const char* data) noexcept
    {
        if (!data)
        {
            return;
        }

        size_t len = 0;
        while (data[len] != '\0')
        {
            ++len;
        }

        append(data, len);
    }

    void append_uint(uint64_t value) noexcept
    {
        char tmp[20]; // UINT64_MAX has 20 digits
        char* p = tmp + sizeof(tmp);

        do
        {
            *--p = static_cast<char>('0' + (value % 10));
            value /= 10;

        } while (value != 0);

        append(p, static_cast<size_t>((tmp + sizeof(tmp)) - p));
    }

    void append_int(const int64_t value) noexcept
    {
        if (value < 0)
        {
            append_literal("-");
            // negate in unsigned space so INT64_MIN is well-defined
            append_uint(0ull - static_cast<uint64_t>(value));
        }
        else
        {
            append_uint(static_cast<uint64_t>(value));
        }
    }

    void format_message(
        const bool system_error,
        const int64_t error_code,
        const char* msg,
        const error_site& site) noexcept
    {
        if (system_error)
        {
            append_literal("[SYSTEM ERROR]: ");
        }
        else
        {
            append_literal("[ERROR]: ");
        }

        append_int(error_code);

        if (msg)
        {
            append_literal(" | ");
            append_cstr(msg);
        }

        if (site.function)
        {
            append_literal(" | function: ");
            append_cstr(site.function);
        }

        if (site.file)
        {
            append_literal(" | file: ");
            append_cstr(site.file);
        }

        if (site.line > 0)
        {
            append_literal(" | line: ");
            append_uint(site.line);
        }
    }

    /// Appends '\n' (always fits) and returns the total byte count.
    size_t finish() noexcept
    {
        *ptr++ = '\n';
        return static_cast<size_t>(ptr - begin);
    }
};

void fatal_print(
    const bool system_error,
    const int64_t ec,
    const char* msg,
    const error_site& site) noexcept
{
    char buffer[VX_ERR_BUFFER_MAX_SIZE];

    error_formatter fmt(buffer, sizeof(buffer));
    fmt.format_message(system_error, ec, msg, site);

    safe_print(buffer, fmt.finish());
}

} // namespace err
} // namespace vx
