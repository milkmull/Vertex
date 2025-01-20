#include "vertex_impl/os/_platform/windows/windows_shell.hpp"
#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
#include "vertex/util/string/string.hpp"

namespace vx {
namespace os {

// https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shellexecutew

bool shell_open_impl(const std::string& uri)
{
    // MSDN says to make sure COM is initialized for safety
    windows::com_scoped_initializer com;
    if (!com.succeeded())
    {
        return false;
    }

    const std::wstring wuri(str::string_cast<wchar_t>(uri));

    const INT_PTR code = reinterpret_cast<INT_PTR>(ShellExecuteW(
        NULL,
        NULL,
        wuri.c_str(),
        NULL,
        NULL,
        SW_SHOWNORMAL
    ));

    // Success returns value greater than 32, otherwise an error
    if (code <= 32)
    {
        windows::error_message("ShellExecuteW()");
        return false;
    }

    return true;
}

} // namespace os
} // namespace vx