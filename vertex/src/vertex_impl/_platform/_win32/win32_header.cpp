#include "vertex_impl/_platform/_win32/win32_header.hpp"

#include "vertex/system/memory.hpp"
#include "vertex/stdlib/string/string.hpp"

void WindowsErrorMessage(const std::string& msg)
{
    DWORD code = GetLastError();
    WCHAR buffer[1024]{};

    FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, code, 0,
        buffer, static_cast<DWORD>(vx::array_size(buffer)), NULL
    );

    // Kill CR/LF that FormatMessage() sticks at the end
    for (WCHAR* it = buffer; *it; ++it)
    {
        if (*it == L'\r')
        {
            *it = 0;
            break;
        }
    }

    VX_ERROR(vx::error::error_code::PLATFORM_ERROR)
        << msg
        << (msg.empty() ? "" : ": ")
        << vx::str::wstring_to_string(buffer);
}