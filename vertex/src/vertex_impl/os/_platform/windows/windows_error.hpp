#pragma once

#include "vertex/os/error.hpp"
#include "vertex/std/_memory/memory_base.hpp"
#include "vertex/std/string_cast.hpp"
#include "vertex_impl/os/_platform/windows/windows_tools.hpp"

namespace vx {
namespace os {

inline error_type get_last_error_impl()
{
    const DWORD code = ::GetLastError();
    return static_cast<error_type>(code);
}

inline void clear_last_error_impl()
{
    ::SetLastError(ERROR_SUCCESS);
}

inline size_t format_error_impl(error_type error, char* buffer, size_t size)
{
    WCHAR wbuffer[1024]{};

    DWORD wcount = ::FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error, 0,
        wbuffer, static_cast<DWORD>(mem::array_size(wbuffer)), NULL);

    // Kill CR/LF that FormatMessage() sticks at the end
    for (WCHAR* it = wbuffer; *it; ++it)
    {
        if (*it == L'\r')
        {
            wcount = static_cast<DWORD>(it - wbuffer);
            break;
        }
    }

    if (wcount == 0)
    {
        // code isn't a recognized system message id
        return 0;
    }

    const auto last = str::string_cast<char>(wbuffer, wcount, buffer, size);
    return static_cast<size_t>(buffer - last);
}


} // namespace os
} // namespace vx
