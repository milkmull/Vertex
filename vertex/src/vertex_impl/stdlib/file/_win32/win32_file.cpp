#include "vertex_impl/stdlib/file/_win32/win32_file.hpp"

#if defined(VX_PLATFORM_WINDOWS)

#include "vertex_impl/_platform/_win32/win32_header.hpp"
#include "vertex/stdlib/file/file.hpp"
#include "vertex/stdlib/string/string.hpp"
#include "vertex/system/assert.hpp"

namespace vx {

file::file_impl::file_impl()
    : m_handle(INVALID_HANDLE_VALUE) {}

file::file_impl::~file_impl()
{
    close();
}

bool file::file_impl::open(const std::string& path, file_mode mode)
{
    VX_ASSERT(!is_open(), "file already open");

    DWORD access = 0;
    DWORD creation = 0;
    DWORD share = 0;
    DWORD flags = FILE_ATTRIBUTE_NORMAL;

    switch (mode)
    {
        // File must exist
        case file_mode::READ:
        {
            access = GENERIC_READ;
            creation = OPEN_EXISTING;
            // Allow read sharing if we are not writing
            share = FILE_SHARE_READ;
            break;
        }
        // Always create or truncate the file
        case file_mode::WRITE:
        {
            access = GENERIC_WRITE;
            creation = CREATE_ALWAYS;
            break;
        }
        // Create if not exists, otherwise open existing
        case file_mode::APPEND:
        {
            access = FILE_APPEND_DATA;
            creation = OPEN_ALWAYS;
            break;
        }
        // File must exist
        case file_mode::READ_WRITE_EXISTS:
        {
            access = GENERIC_READ | GENERIC_WRITE;
            creation = OPEN_EXISTING;
            break;
        }
        // Truncate if file exists, or create a new one
        case file_mode::READ_WRITE_CREATE:
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
        VX_ERROR(error::error_code::FILE_OPEN_FAILED) << "failed to open file: " << path;
        return false;
    }

    return true;
}

bool file::file_impl::is_open() const
{
    return m_handle != INVALID_HANDLE_VALUE;
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
        WindowsErrorMessage("GetFileSizeEx()");
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
        WindowsErrorMessage("SetFilePointerEx()");
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
        WindowsErrorMessage("SetFilePointerEx()");
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
        WindowsErrorMessage("ReadFile()");
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
        WindowsErrorMessage("WriteFile()");
        return 0;
    }

    return static_cast<size_t>(count);
}

bool file::file_impl::flush()
{
    VX_ASSERT(is_open(), "file not open");

    if (!FlushFileBuffers(m_handle))
    {
        WindowsErrorMessage("FlushFileBuffers()");
        return false;
    }

    return true;
}

} // namespace vx

#endif
