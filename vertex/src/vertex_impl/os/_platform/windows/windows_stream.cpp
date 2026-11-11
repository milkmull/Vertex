#include "vertex/config/assert.hpp"
#include "vertex/os/error_code.hpp"
#include "vertex/os/stream.hpp"
#include "vertex_impl/os/_platform/windows/windows_header.hpp"

namespace vx {
namespace os {

native_handle get_stream_handle(stream s)
{
    static constexpr DWORD std_handles[] = {
        STD_INPUT_HANDLE,
        STD_OUTPUT_HANDLE,
        STD_ERROR_HANDLE
    };

    const size_t i = static_cast<size_t>(s);
    VX_ASSERT(i <= 2);
    return ::GetStdHandle(std_handles[i]);
}

// Failure of a WriteFile call. `report` is false for callers that must not trigger
// message formatting/logging (e.g. the logger writing to stderr).
static error write_failure()
{
    const DWORD e = ::GetLastError();
    err::code c = to_error_code(e);

    // a broken pipe is eof for readers, but a plain failure for writers
    if (c == err::eof)
    {
        c = err::system_error;
    }
    if (c == err::none)
    {
        c = err::failed;
    }

    return error(c);
}

rw_result _stream_priv::write_impl(native_handle h, const uint8_t* data, size_t size)
{
    size_t total = 0;

    while (size > 0)
    {
        DWORD written = 0;

        const DWORD chunk = size > static_cast<size_t>(MAXDWORD)
            ? MAXDWORD
            : static_cast<DWORD>(size);

        if (!::WriteFile(h, data, chunk, &written, nullptr))
        {
            return { total, write_failure() };
        }

        if (written == 0)
        {
            return { total, err::failed };
        }

        data += written;
        size -= written;
        total += static_cast<size_t>(written);
    }

    return { total, err::none };
}

} // namespace os
} // namespace vx
