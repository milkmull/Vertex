#include "vertex_impl/os/_windows/windows_file.hpp"

#if defined(VX_OS_WINDOWS_FILE)

#include "vertex_impl/os/_windows/windows_file.hpp"
#include "vertex/util/string/string.hpp"
#include "vertex/core/assert.hpp"

namespace vx {
namespace os {

file::file_impl::file_impl()
    : m_handle(INVALID_HANDLE_VALUE) {}

file::file_impl::~file_impl()
{
    close();
}

bool file::file_impl::exists(const std::string& path)
{
    const std::wstring wpath(str::string_to_wstring(path));
    const DWORD fileAttr = GetFileAttributesW(wpath.c_str());
    return (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY));
}

bool file::file_impl::open(const std::string& path, mode mode)
{
    VX_ASSERT(!is_open(), "file already open");

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

    const std::wstring wpath = str::string_to_wstring(path);

    m_handle = CreateFileW(
        wpath.c_str(),
        access,
        share,
        NULL,
        creation,
        flags,
        NULL
    );

    if (m_handle == INVALID_HANDLE_VALUE)
    {
        VX_ERR(err::FILE_OPEN_FAILED) << "failed to open file: " << path;
        return false;
    }

    return true;
}

bool file::file_impl::is_open() const
{
    return (m_handle != INVALID_HANDLE_VALUE);
}

void file::file_impl::close()
{
    if (m_handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_handle);
        m_handle = INVALID_HANDLE_VALUE;
    }
}

size_t file::file_impl::size() const
{
    VX_ASSERT(is_open(), "file not open");

    LARGE_INTEGER size;

    if (!GetFileSizeEx(m_handle, &size))
    {
        windows_error_message("GetFileSizeEx()");
        return INVALID_SIZE;
    }

    return static_cast<size_t>(size.QuadPart);
}

bool file::file_impl::seek(size_t off, stream_position from)
{
    VX_ASSERT(is_open(), "file not open");

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

    if (!SetFilePointerEx(m_handle, distance, &distance, method))
    {
        windows_error_message("SetFilePointerEx()");
        return false;
    }

    return true;
}

size_t file::file_impl::tell() const
{
    VX_ASSERT(is_open(), "file not open");

    LARGE_INTEGER off{ 0 };
    off.QuadPart = 0;

    // Use SetFilePointerEx to query the current position
    if (!SetFilePointerEx(m_handle, off, &off, FILE_CURRENT))
    {
        windows_error_message("SetFilePointerEx()");
        return INVALID_POSITION;
    }

    return static_cast<size_t>(off.QuadPart);
}

size_t file::file_impl::read(uint8_t* data, size_t size)
{
    VX_ASSERT(is_open(), "file not open");

    DWORD count = 0;
    if (!ReadFile(m_handle, data, static_cast<DWORD>(size), &count, nullptr))
    {
        windows_error_message("ReadFile()");
        return 0;
    }

    return static_cast<size_t>(count);
}

size_t file::file_impl::write(const uint8_t* data, size_t size)
{
    VX_ASSERT(is_open(), "file not open");

    DWORD count = 0;
    if (!WriteFile(m_handle, data, static_cast<DWORD>(size), &count, NULL))
    {
        windows_error_message("WriteFile()");
        return 0;
    }

    return static_cast<size_t>(count);
}

bool file::file_impl::flush()
{
    VX_ASSERT(is_open(), "file not open");

    if (!FlushFileBuffers(m_handle))
    {
        windows_error_message("FlushFileBuffers()");
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
    return m_handle;
}

} // namespace os
} // namespace vx

#endif // VX_OS_WINDOWS_FILE
