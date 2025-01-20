#include "vertex_impl/os/_platform/windows/windows_file.hpp"
#include "vertex/util/string/string.hpp"

namespace vx {
namespace os {

#define assert_is_open() VX_ASSERT_MESSAGE(is_open(), "file not open")

bool file::file_impl::exists(const path& p)
{
    const DWORD attrs = GetFileAttributesW(p.c_str());
    return (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY));
}

bool file::file_impl::open(const path& p, mode mode)
{
    VX_ASSERT_MESSAGE(!is_open(), "file already open");

    DWORD access = 0;
    DWORD creation = 0;
    DWORD share = 0;
    DWORD flags = FILE_ATTRIBUTE_NORMAL;

    switch (mode)
    {
        // File must exist
        case mode::READ:
        {
            access = GENERIC_READ;
            creation = OPEN_EXISTING;
            // Allow read sharing if we are not writing
            share = FILE_SHARE_READ;
            break;
        }
        // Always create or truncate the file
        case mode::WRITE:
        {
            access = GENERIC_WRITE;
            creation = CREATE_ALWAYS;
            break;
        }
        // Create if not exists, otherwise open existing
        case mode::APPEND:
        {
            access = FILE_APPEND_DATA;
            creation = OPEN_ALWAYS;
            break;
        }
        // File must exist
        case mode::READ_WRITE_EXISTS:
        {
            access = GENERIC_READ | GENERIC_WRITE;
            creation = OPEN_EXISTING;
            break;
        }
        // Truncate if file exists, or create a new one
        case mode::READ_WRITE_CREATE:
        {
            access = GENERIC_READ | GENERIC_WRITE;
            creation = CREATE_ALWAYS;
            break;
        }
    }

    m_handle = CreateFileW(
        p.c_str(),
        access,
        share,
        NULL,
        creation,
        flags,
        NULL
    );

    if (!m_handle.is_valid())
    {
        VX_ERR(err::FILE_OPEN_FAILED) << "failed to open file: " << p;
        return false;
    }

    return true;
}

void file::file_impl::close() {}

size_t file::file_impl::size() const
{
    assert_is_open();

    LARGE_INTEGER size;

    if (!GetFileSizeEx(m_handle.get(), &size))
    {
        windows::error_message("GetFileSizeEx()");
        return INVALID_SIZE;
    }

    return static_cast<size_t>(size.QuadPart);
}

bool file::file_impl::seek(size_t off, stream_position from)
{
    assert_is_open();

    LARGE_INTEGER distance{ 0 };
    distance.QuadPart = off;

    DWORD method = 0;
    switch (from)
    {
        case stream_position::BEGIN:
        {
            method = FILE_BEGIN;
            break;
        }
        case stream_position::CURRENT:
        {
            method = FILE_CURRENT;
            break;
        }
        case stream_position::END:
        {
            method = FILE_END;
            break;
        }
    }

    if (!SetFilePointerEx(m_handle.get(), distance, &distance, method))
    {
        windows::error_message("SetFilePointerEx()");
        return false;
    }

    return true;
}

size_t file::file_impl::tell() const
{
    assert_is_open();

    LARGE_INTEGER off{ 0 };
    off.QuadPart = 0;

    // Use SetFilePointerEx to query the current position
    if (!SetFilePointerEx(m_handle.get(), off, &off, FILE_CURRENT))
    {
        windows::error_message("SetFilePointerEx()");
        return INVALID_POSITION;
    }

    return static_cast<size_t>(off.QuadPart);
}

size_t file::file_impl::read(uint8_t* data, size_t size)
{
    assert_is_open();

    DWORD count = 0;
    if (!ReadFile(m_handle.get(), data, static_cast<DWORD>(size), &count, nullptr))
    {
        windows::error_message("ReadFile()");
        return 0;
    }

    return static_cast<size_t>(count);
}

size_t file::file_impl::write(const uint8_t* data, size_t size)
{
    assert_is_open();

    DWORD count = 0;
    if (!WriteFile(m_handle.get(), data, static_cast<DWORD>(size), &count, NULL))
    {
        windows::error_message("WriteFile()");
        return 0;
    }

    return static_cast<size_t>(count);
}

bool file::file_impl::flush()
{
    assert_is_open();

    if (!FlushFileBuffers(m_handle.get()))
    {
        windows::error_message("FlushFileBuffers()");
        return false;
    }

    return true;
}

file file::file_impl::from_handle(HANDLE handle, mode mode)
{
    file f;

    if (handle == INVALID_HANDLE_VALUE)
    {
        return f;
    }

    f.m_impl = std::make_unique<file_impl>();
    if (!f.m_impl)
    {
        return f;
    }

    f.m_impl->m_handle = handle;
    f.m_mode = mode;

    return f;
}

HANDLE file::file_impl::get_handle()
{
    return m_handle.get();
}

} // namespace os
} // namespace vx
