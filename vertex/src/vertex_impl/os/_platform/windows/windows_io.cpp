#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
#include "vertex/std/io.hpp"
#include "vertex/os/io.hpp"

namespace vx {
namespace os {

static HANDLE get_handle(stream s)
{
    switch (s)
    {
        case stream::in:
        {
            return ::GetStdHandle(STD_INPUT_HANDLE);
        }
        case stream::out:
        {
            return ::GetStdHandle(STD_OUTPUT_HANDLE);
        }
        case stream::err:
        {
            return ::GetStdHandle(STD_ERROR_HANDLE);
        }
    }

    return INVALID_HANDLE_VALUE;
}

size_t write_raw_impl(stream s, const char* data, size_t size, bool allow_error)
{
    HANDLE handle = get_handle(s);
    size_t total = 0;

    while (size > 0)
    {
        DWORD written = 0;

        const DWORD chunk = size > static_cast<size_t>(MAXDWORD)
            ? MAXDWORD
            : static_cast<DWORD>(size);

        if (!::WriteFile(handle, data, chunk, &written, nullptr) || written == 0)
        {
            if (allow_error)
            {
                err::set_last_os_error("WriteFile");
            }
            break;
        }

        data += written;
        size -= written;
        total += static_cast<size_t>(written);
    }

    return total;
}

size_t read_raw_impl(stream s, char* data, size_t size)
{
    HANDLE handle = get_handle(s);
    size_t total = 0;

    while (size > 0)
    {
        DWORD read = 0;

        const DWORD chunk = size > static_cast<size_t>(MAXDWORD)
            ? MAXDWORD
            : static_cast<DWORD>(size);

        if (!::ReadFile(handle, data, chunk, &read, NULL) || read == 0)
        {
            err::set_last_os_error("ReadFile");
            break;
        }

        data += read;
        size -= read;
        total += static_cast<size_t>(read);
    }

    return total;
}

} // namespace os
} // namespace vx
