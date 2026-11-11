#include "vertex/config/assert.hpp"
#include "vertex/os/error.hpp"
#include "vertex/os/io.hpp"
#include "vertex_impl/os/_platform/windows/windows_error_helpers.hpp"

namespace vx {
namespace os {

rw_result read(native_handle h, uint8_t* data, size_t size)
{
    size_t total = 0;

    while (size > 0)
    {
        DWORD got = 0;

        const DWORD chunk = size > static_cast<size_t>(MAXDWORD)
            ? MAXDWORD
            : static_cast<DWORD>(size);

        if (!::ReadFile(h, data, chunk, &got, nullptr))
        {
            // ERROR_BROKEN_PIPE / ERROR_HANDLE_EOF map to err::eof (no message formatting)
            return { total, fail("ReadFile").value };
        }

        if (got == 0)
        {
            return { total, err::eof };
        }

        data += got;
        size -= got;
        total += static_cast<size_t>(got);
    }

    return { total, err::none };
}

rw_result write(native_handle h, const uint8_t* data, size_t size)
{
    const rw_result res = _stream_priv::write_impl(h, data, size);
    if (res.err == err::system_error)
    {
        err::set_last_os_error("WriteFile");
    }

    return res;
}

success open(native_handle h, const native_cstring p, typename rw_mode::enum_type mode)
{
    DWORD access = 0;
    DWORD creation = 0;

    switch (mode)
    {
        // File must exist
        case rw_mode::none:
        case rw_mode::read:
        {
            access = GENERIC_READ;
            creation = OPEN_EXISTING;
            break;
        }
        // Always create or truncate the file
        case rw_mode::write:
        {
            access = GENERIC_WRITE;
            creation = CREATE_ALWAYS;
            break;
        }
        // Create if not exists, otherwise open existing
        case rw_mode::append:
        {
            access = FILE_APPEND_DATA;
            creation = OPEN_ALWAYS;
            break;
        }
        // File must exist
        case rw_mode::read_write_exists:
        {
            access = GENERIC_READ | GENERIC_WRITE;
            creation = OPEN_EXISTING;
            break;
        }
        // Truncate if file exists, or create a new one
        case rw_mode::read_write_create:
        {
            access = GENERIC_READ | GENERIC_WRITE;
            creation = CREATE_ALWAYS;
            break;
        }
    }

    h = ::CreateFileW(
        p,
        access,
        FILE_SHARE_READ | FILE_SHARE_WRITE, // allow read and write sharing
        NULL,
        creation,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (!is_valid_handle(h))
    {
        return fail("CreateFileW");
    }

    return success{};
}

success flush(native_handle h)
{
    if (!::FlushFileBuffers(h))
    {
        const DWORD e = ::GetLastError();

        // consoles and pipes have nothing to flush and reject FlushFileBuffers
        if ((e == ERROR_INVALID_HANDLE || e == ERROR_INVALID_FUNCTION) && ::GetFileType(h) != FILE_TYPE_DISK)
        {
            return success{};
        }

        ::SetLastError(e);
        return fail("FlushFileBuffers");
    }

    return success{};
}

success truncate(native_handle h, uint64_t size)
{
    // does not move the file pointer, unlike SetFilePointerEx + SetEndOfFile
    FILE_END_OF_FILE_INFO eof{};
    eof.EndOfFile.QuadPart = static_cast<LONGLONG>(size);

    if (!::SetFileInformationByHandle(h, FileEndOfFileInfo, &eof, sizeof(eof)))
    {
        return fail("SetFileInformationByHandle");
    }

    return success{};
}

success tell(const native_handle h, file_position& pos)
{
    LARGE_INTEGER off{};

    // Use SetFilePointerEx to query the current position
    if (!::SetFilePointerEx(h, off, &off, FILE_CURRENT))
    {
        return fail("SetFilePointerEx");
    }

    pos = static_cast<file_position>(off.QuadPart);
    return success{};
}

success seek(native_handle h, file_offset off, stream_position from)
{
    LARGE_INTEGER distance{ 0 };
    distance.QuadPart = off;

    DWORD method = 0;
    switch (from)
    {
        case stream_position::begin:
        {
            method = FILE_BEGIN;
            break;
        }
        case stream_position::current:
        {
            method = FILE_CURRENT;
            break;
        }
        case stream_position::end:
        {
            method = FILE_END;
            break;
        }
    }

    if (!::SetFilePointerEx(h, distance, &distance, method))
    {
        return fail("SetFilePointerEx");
    }

    return success{};
}

success file_size(const native_handle h, uint64_t& size)
{
    LARGE_INTEGER lsize{};

    if (!::GetFileSizeEx(h, &lsize))
    {
        return fail("GetFileSizeEx");
    }

    size = static_cast<uint64_t>(lsize.QuadPart);
    return success{};
}

success set_file_size(native_handle h, uint64_t size)
{
    if (size > static_cast<uint64_t>(INT64_MAX))
    {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return fail("set_file_size");
    }

    // Does not move the file pointer, unlike SetFilePointerEx + SetEndOfFile.
    // The handle needs FILE_WRITE_DATA (so not an append-only handle).
    FILE_END_OF_FILE_INFO eof{};
    eof.EndOfFile.QuadPart = static_cast<LONGLONG>(size);

    if (!::SetFileInformationByHandle(h, FileEndOfFileInfo, &eof, sizeof(eof)))
    {
        return fail("SetFileInformationByHandle");
    }

    return success{};
}

} // namespace os
} // namespace vx
