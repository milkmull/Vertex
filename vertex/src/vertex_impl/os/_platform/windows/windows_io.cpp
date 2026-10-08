#include "vertex/config/assert.hpp"
#include "vertex/os/io.hpp"
#include "vertex_impl/os/_platform/windows/windows_tools.hpp"

namespace vx {
namespace os {

#define assert_is_open(h) VX_ASSERT(h.is_valid())

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

static rw_result read_impl(HANDLE h, uint8_t* data, size_t size)
{
    size_t total = 0;

    while (size > 0)
    {
        DWORD read = 0;

        const DWORD chunk = size > static_cast<size_t>(MAXDWORD)
            ? MAXDWORD
            : static_cast<DWORD>(size);

        if (!::ReadFile(h, data, chunk, &read, NULL))
        {
            err::set_last_os_error("ReadFile");
            return { total, err::system_error };
        }

        if (read == 0)
        {
            return { total, err::eof };
        }

        data += read;
        size -= read;
        total += static_cast<size_t>(read);
    }

    return { total, err::none };
}

static rw_result write_impl(HANDLE h, const uint8_t* data, size_t size, bool allow_error)
{
    size_t total = 0;

    while (size > 0)
    {
        DWORD written = 0;

        const DWORD chunk = size > static_cast<size_t>(MAXDWORD)
            ? MAXDWORD
            : static_cast<DWORD>(size);

        if (!::WriteFile(h, data, chunk, &written, nullptr))
        {
            if (allow_error)
            {
                err::set_last_os_error("WriteFile");
            }
            return { total, err::system_error };
        }

        if (written == 0)
        {
            return { total, err::failed };
        }

        data += written;
        size -= written;
        total += static_cast<size_t>(written);
    }

    return { total, err::none };
}

rw_result read_stream(stream s, char* data, size_t size)
{
    HANDLE h = get_handle(s);
    return read_impl(h, reinterpret_cast<uint8_t*>(data), size);
}

rw_result _os_io_priv::write_stream_impl(stream s, const char* data, size_t size, bool allow_error)
{
    HANDLE h = get_handle(s);
    return write_impl(h, reinterpret_cast<const uint8_t*>(data), size, allow_error);
}

bool file_exists(const path& p)
{
    const DWORD attrs = ::GetFileAttributesW(p.c_str());
    return (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY));
}

success open(handle& h, const path& p, typename rw_mode::enum_type mode)
{
    VX_ASSERT(!h.is_valid());

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
        p.c_str(),
        access,
        FILE_SHARE_READ | FILE_SHARE_WRITE, // allow read and write sharing
        NULL,
        creation,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (!h.is_valid())
    {
        err::set_last_os_error("CreateFileW");
        return err::system_error;
    }

    return success{};
}

rw_result read(handle& h, uint8_t* data, size_t size)
{
    assert_is_open(h);
    return read_impl(h.get(), data, size);
}


rw_result write(handle& h, const uint8_t* data, size_t size)
{
    assert_is_open(h);
    return write_impl(h.get(), data, size, true);
}

success flush(handle& h)
{
    assert_is_open(h);

    if (!::FlushFileBuffers(h.get()))
    {
        err::set_last_os_error("FlushFileBuffers");
        return err::system_error;
    }

    return success{};
}

expected<file_position, error> tell(const handle& h)
{
    assert_is_open(h);

    LARGE_INTEGER off{};

    // Use SetFilePointerEx to query the current position
    if (!::SetFilePointerEx(h.get(), off, &off, FILE_CURRENT))
    {
        err::set_last_os_error("SetFilePointerEx");
        return make_unexpected<error>(err::system_error);
    }

    return static_cast<file_position>(off.QuadPart);
}

success seek(handle& h, file_offset off, stream_position from)
{
    assert_is_open(h);

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

    if (!::SetFilePointerEx(h.get(), distance, &distance, method))
    {
        err::set_last_os_error("SetFilePointerEx");
        return err::system_error;
    }

    return success{};
}

expected<size_t, error> file_size(const handle& h)
{
    assert_is_open(h);

    LARGE_INTEGER file_size{};

    if (!::GetFileSizeEx(h.get(), &file_size))
    {
        err::set_last_os_error("GetFileSizeEx");
        return make_unexpected<error>(err::system_error);
    }

    return static_cast<size_t>(file_size.QuadPart);
}

} // namespace os
} // namespace vx
