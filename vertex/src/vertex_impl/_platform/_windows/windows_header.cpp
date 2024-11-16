#include "vertex_impl/_platform/_windows/windows_header.hpp"

#include "vertex/util/memory/memory.hpp"
#include "vertex/system/error.hpp"
#include "vertex/util/string/string.hpp"

///////////////////////////////////////////////////////////////////////////////
// Error Handling
///////////////////////////////////////////////////////////////////////////////

void windows_error_message(const std::string& msg)
{
    DWORD code = GetLastError();
    WCHAR buffer[1024]{};

    FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, code, 0,
        buffer, static_cast<DWORD>(vx::mem::array_size(buffer)), NULL
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

    VX_ERR(vx::err::PLATFORM_ERROR)
        << msg
        << (msg.empty() ? "" : ": ")
        << vx::str::wstring_to_string(buffer);
}

///////////////////////////////////////////////////////////////////////////////
// COM
///////////////////////////////////////////////////////////////////////////////

com_scoped_initializer::com_scoped_initializer()
{
    m_hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    if (m_hr == RPC_E_CHANGED_MODE)
    {
        // Retry with multithreaded apartment model
        m_hr = CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);
    }

    // S_FALSE means success but COM was already initialized on the thread
    // so we treat is as successful (still need to call com_uninitialize)
    if (m_hr == S_FALSE)
    {
        m_hr = S_OK;
    }

    if (!succeeded())
    {
        windows_error_message("CoInitializeEx()");
    }
}

com_scoped_initializer::~com_scoped_initializer()
{
    if (succeeded())
    {
        CoUninitialize();
    }
}

bool com_scoped_initializer::succeeded() const
{
    return SUCCEEDED(m_hr);
}