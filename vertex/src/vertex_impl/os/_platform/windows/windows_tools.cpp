#include <combaseapi.h> // CoInitializeEx

#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
#include "vertex/util/memory/memory.hpp"
#include "vertex/system/error.hpp"
#include "vertex/util/string/string_cast.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// Handle
///////////////////////////////////////////////////////////////////////////////

void handle::close_impl() noexcept
{
    ::CloseHandle(m_handle);
}

namespace windows {

///////////////////////////////////////////////////////////////////////////////
// Error Handling
///////////////////////////////////////////////////////////////////////////////

void error_message(const char* msg)
{
    const size_t msg_size = std::strlen(msg);

    DWORD code = ::GetLastError();
    WCHAR buffer[1024]{};

    ::FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, code, 0,
        buffer, static_cast<DWORD>(mem::array_size(buffer)), NULL
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

    VX_ERR(vx::err::SYSTEM_ERROR)
        << msg
        << ((msg_size == 0) ? "" : ": ")
        << str::string_cast<char>(buffer);
}

///////////////////////////////////////////////////////////////////////////////
// COM
///////////////////////////////////////////////////////////////////////////////

bool com_initializer::initialize() noexcept
{
    m_hr = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    if (m_hr == RPC_E_CHANGED_MODE)
    {
        // Retry with multithreaded apartment model
        m_hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);
    }

    // S_FALSE means success but COM was already initialized on the thread
    // so we treat is as successful (still need to call com_uninitialize)
    if (m_hr == S_FALSE)
    {
        m_hr = S_OK;
    }

    if (!succeeded())
    {
        error_message("CoInitializeEx()");
        return false;
    }

    return true;
}

void com_initializer::uninitialize() noexcept
{
    if (succeeded())
    {
        ::CoUninitialize();
        m_hr = E_FAIL;
    }
}

///////////////////////////////////////////////////////////////////////////////
// OLE
///////////////////////////////////////////////////////////////////////////////

bool ole_initializer::initialize() noexcept
{
    m_hr = ::OleInitialize(NULL);

    if (!succeeded())
    {
        error_message("OleInitialize()");
        return false;
    }

    return true;
}

void ole_initializer::uninitialize() noexcept
{
    if (succeeded())
    {
        ::OleUninitialize();
        m_hr = E_FAIL;
    }
}

} // namespace windows
} // namespace os
} // namespace vx