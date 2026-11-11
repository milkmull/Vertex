#include "vertex/os/error_code.hpp"
#include "vertex_impl/os/_platform/windows/windows_header.hpp"

namespace vx {
namespace os {

// vx::error only carries a vx::err::code, so Win32 errors are mapped onto that set.
// Anything without a matching code becomes err::system_error. If you need to tell
// e.g. "directory not empty" or "cross-device rename" apart, add dedicated codes
// to err::code and extend this switch.
err::code to_error_code(native_error e) noexcept
{
    switch (e)
    {
        case ERROR_SUCCESS:
        {
            return err::none;
        }
        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
        case ERROR_INVALID_DRIVE:
        case ERROR_BAD_NETPATH:
        case ERROR_BAD_NET_NAME:
        {
            return err::resource_not_found;
        }
        case ERROR_ALREADY_EXISTS:
        case ERROR_FILE_EXISTS:
        {
            return err::resource_already_exists;
        }
        case ERROR_ACCESS_DENIED:
        case ERROR_SHARING_VIOLATION:
        case ERROR_LOCK_VIOLATION:
        case ERROR_WRITE_PROTECT:
        case ERROR_PRIVILEGE_NOT_HELD: // symlink creation without developer mode
        {
            return err::permission_denied;
        }
        case ERROR_NOT_ENOUGH_MEMORY:
        case ERROR_OUTOFMEMORY:
        {
            return err::out_of_memory;
        }
        case ERROR_INSUFFICIENT_BUFFER:
        case ERROR_FILE_TOO_LARGE:
        {
            return err::size_error;
        }
        case ERROR_INVALID_PARAMETER:
        case ERROR_INVALID_HANDLE:
        case ERROR_INVALID_NAME:
        case ERROR_BAD_PATHNAME:
        case ERROR_FILENAME_EXCED_RANGE:
        case ERROR_DIRECTORY:            // file_size() on a directory
        case ERROR_NOT_A_REPARSE_POINT:  // read_symlink() on a non-link
        case ERROR_REPARSE_TAG_MISMATCH: // read_symlink() on a non-symlink reparse point
        {
            return err::invalid_argument;
        }
        case ERROR_INVALID_DATA:
        {
            return err::invalid_data;
        }
        case ERROR_NOT_SUPPORTED:
        case ERROR_CALL_NOT_IMPLEMENTED:
        case ERROR_INVALID_FUNCTION: // filesystem does not support the operation
        {
            return err::unsupported_operation;
        }
        case ERROR_HANDLE_EOF:
        {
            return err::eof;
        }
        default:
        {
            return err::system_error;
        }
    }
}

native_error get_last_error()
{
    return static_cast<native_error>(::GetLastError());
}

void set_last_error(native_error error)
{
    ::SetLastError(static_cast<DWORD>(error));
}

} // namespace os
} // namespace vx
