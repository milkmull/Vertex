#include "vertex/system/platform_config.hpp"

#if defined(__VX_OS_WINDOWS_SHELL)

#include "vertex_impl/_platform/_windows/windows_header.hpp"
#include "vertex/os/shell.hpp"
#include "vertex/util/string/string.hpp"

namespace vx {
namespace os {

// https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shellexecutew

VX_API bool shell_open(const std::string& uri)
{
    // MSDN says to make sure COM is initialized for safety
    com_scoped_initializer com;
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
        windows_error_message("ShellExecuteW()");
        return false;
    }

    return true;
}

} // namespace os
} // namespace vx

#endif // VX_OS_WINDOWS_SHELL
