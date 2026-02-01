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
    err::set(err::system_error, msg);

    //const size_t msg_size = std::strlen(msg);
    //
    //DWORD code = ::GetLastError();
    //WCHAR buffer[1024]{};
    //
    //::FormatMessageW(
    //    FORMAT_MESSAGE_FROM_SYSTEM,
    //    NULL, code, 0,
    //    buffer, static_cast<DWORD>(mem::array_size(buffer)), NULL
    //);
    //
    //// Kill CR/LF that FormatMessage() sticks at the end
    //for (WCHAR* it = buffer; *it; ++it)
    //{
    //    if (*it == L'\r')
    //    {
    //        *it = 0;
    //        break;
    //    }
    //}

    //VX_ERR(vx::err::system_error)
    //    << msg
    //    << ((msg_size == 0) ? "" : ": ")
    //    << str::string_cast<char>(buffer);
}

///////////////////////////////////////////////////////////////////////////////
// helpers
///////////////////////////////////////////////////////////////////////////////

const char* check_default_args(int* pargc, char*** pargv, void** pallocated)
{
    // If a valid argv is already provided, pass it through unchanged.
    // 
    // If argv is NULL, we treat that as a request to construct a UTF-8 argv[]
    // from the process command line. This is required on Windows because some
    // entry points (including those supplied by the engine) do not receive a
    // standard argc/argv.
    // 
    // In those cases we parse the wide-character command line and convert it
    // to a UTF-8 argument array.

    const char* out_of_mem_str = "Out of memory - aborting";
    const char* proc_err_str = "Error processing command line arguments - aborting";

    *pallocated = NULL;

    if (*pargv)
    {
        // Caller supplied an argv[]; leave argc/argv unchanged and return success.
        return NULL;
    }

    // Memory allocation must be done using the system heap, not any engine- or
    // application-defined custom allocator, since this runs before the app’s
    // memory system may be initialized.
    LPWSTR* argvw = NULL;
    char** argv = NULL;

    const LPWSTR command_line = ::GetCommandLineW();

    // Compute the UTF-8 buffer size needed to hold the entire command line.
    // WideCharToMultiByte() guarantees that converting individual arguments
    // will not exceed this size.
    const int arg_data_size = ::WideCharToMultiByte(CP_UTF8, 0, command_line, -1, NULL, 0, NULL, NULL);
    if (!arg_data_size)
    {
        return proc_err_str;
    }

    // Parse the command line into a wide-character argv[].
    // CommandLineToArgvW() returns argument boundaries consistent with standard
    // Windows command line rules.
    int argc = -1;
    argvw = ::CommandLineToArgvW(command_line, &argc);
    if (!argvw || argc < 0)
    {
        return out_of_mem_str;
    }

    // Allocate one contiguous block:
    // [argv pointers] + [UTF - 8 argument string buffer]
    // This simplifies cleanup and ensures efficient access.
    argv = static_cast<char**>(::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, (argc + 1ull) * sizeof(*argv) + arg_data_size));
    if (!argv)
    {
        ::LocalFree(argvw);
        return out_of_mem_str;
    }

    char* arg_data = reinterpret_cast<char*>(argv) + (argc + 1ull) * sizeof(*argv);
    int arg_data_index = 0;

    // Convert each wide argument to UTF-8 and populate argv[].
    // Each string is written into the contiguous arg_data buffer.
    for (int i = 0; i < argc; ++i)
    {
        const int bytes_written = ::WideCharToMultiByte(CP_UTF8, 0, argvw[i], -1, arg_data + arg_data_index, arg_data_size - arg_data_index, NULL, NULL);
        if (!bytes_written)
        {
            ::HeapFree(::GetProcessHeap(), 0, argv);
            ::LocalFree(argvw);
            return proc_err_str;
        }

        argv[i] = arg_data + arg_data_index;
        arg_data_index += bytes_written;
    }

    argv[argc] = NULL;
    ::LocalFree(argvw);

    // Output the constructed argc/argv values. *pallocated receives the pointer
    // to the allocated block so callers can free it later.
    *pargc = argc;
    *pallocated = argv;
    *pargv = argv;

    // success
    return NULL;
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
