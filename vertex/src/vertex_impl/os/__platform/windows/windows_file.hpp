#pragma once

#include "vertex_impl/os/__platform/windows/windows_tools.hpp"
#include "vertex/os/file.hpp"

namespace vx {
namespace os {
namespace __detail {

#define assert_is_open(h) VX_ASSERT_MESSAGE(h.is_valid(), "file not open")

class file_impl
{
public:

    static bool exists(const path& p)
    {
        const DWORD attrs = GetFileAttributesW(p.c_str());
        return (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY));
    }

    static bool open(const path& p, file::mode mode, __detail::file_impl_data& fd)
    {
        VX_ASSERT_MESSAGE(!fd.handle.is_valid(), "file already open");

        DWORD access = 0;
        DWORD creation = 0;
        DWORD share = 0;
        DWORD flags = FILE_ATTRIBUTE_NORMAL;

        switch (mode)
        {
            // File must exist
            case file::mode::READ:
            {
                access = GENERIC_READ;
                creation = OPEN_EXISTING;
                // Allow read sharing if we are not writing
                share = FILE_SHARE_READ;
                break;
            }
            // Always create or truncate the file
            case file::mode::WRITE:
            {
                access = GENERIC_WRITE;
                creation = CREATE_ALWAYS;
                break;
            }
            // Create if not exists, otherwise open existing
            case file::mode::APPEND:
            {
                access = FILE_APPEND_DATA;
                creation = OPEN_ALWAYS;
                break;
            }
            // File must exist
            case file::mode::READ_WRITE_EXISTS:
            {
                access = GENERIC_READ | GENERIC_WRITE;
                creation = OPEN_EXISTING;
                break;
            }
            // Truncate if file exists, or create a new one
            case file::mode::READ_WRITE_CREATE:
            {
                access = GENERIC_READ | GENERIC_WRITE;
                creation = CREATE_ALWAYS;
                break;
            }
        }

        fd.handle = CreateFileW(
            p.c_str(),
            access,
            share,
            NULL,
            creation,
            flags,
            NULL
        );

        if (!fd.handle.is_valid())
        {
            windows::error_message("CreateFileW()");
            return false;
        }

        return true;
    }

    static bool is_open(const __detail::file_impl_data& fd)
    {
        return fd.handle.is_valid();
    }

    static void close(__detail::file_impl_data& fd)
    {
        fd.handle.close();
    }

    static size_t size(const __detail::file_impl_data& fd)
    {
        assert_is_open(fd.handle);

        LARGE_INTEGER size;

        if (!GetFileSizeEx(fd.handle.get(), &size))
        {
            windows::error_message("GetFileSizeEx()");
            return file::INVALID_SIZE;
        }

        return static_cast<size_t>(size.QuadPart);
    }

    static bool resize(size_t size, __detail::file_impl_data& fd)
    {
        assert_is_open(fd.handle);

        if (!seek(static_cast<int>(size), stream_position::BEGIN, fd))
        {
            return false;
        }

        if (!SetEndOfFile(fd.handle.get()))
        {
            windows::error_message("SetEndOfFile()");
            return false;
        }

        return true;
    }

    static bool seek(int off, stream_position from, __detail::file_impl_data& fd)
    {
        assert_is_open(fd.handle);

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

        if (!SetFilePointerEx(fd.handle.get(), distance, &distance, method))
        {
            windows::error_message("SetFilePointerEx()");
            return false;
        }

        return true;
    }

    static size_t tell(const __detail::file_impl_data& fd)
    {
        assert_is_open(fd.handle);

        LARGE_INTEGER off{ 0 };
        off.QuadPart = 0;

        // Use SetFilePointerEx to query the current position
        if (!SetFilePointerEx(fd.handle.get(), off, &off, FILE_CURRENT))
        {
            windows::error_message("SetFilePointerEx()");
            return file::INVALID_POSITION;
        }

        return static_cast<size_t>(off.QuadPart);
    }

    static bool flush(__detail::file_impl_data& fd)
    {
        assert_is_open(fd.handle);

        if (!FlushFileBuffers(fd.handle.get()))
        {
            windows::error_message("FlushFileBuffers()");
            return false;
        }

        return true;
    }

    static size_t read(uint8_t* data, size_t size, __detail::file_impl_data& fd)
    {
        assert_is_open(fd.handle);

        DWORD count = 0;
        if (!ReadFile(fd.handle.get(), data, static_cast<DWORD>(size), &count, NULL))
        {
            windows::error_message("ReadFile()");
            return 0;
        }

        return static_cast<size_t>(count);
    }

    static size_t write(const uint8_t* data, size_t size, __detail::file_impl_data& fd)
    {
        assert_is_open(fd.handle);

        DWORD count = 0;
        if (!WriteFile(fd.handle.get(), data, static_cast<DWORD>(size), &count, NULL))
        {
            windows::error_message("WriteFile()");
            return 0;
        }

        return static_cast<size_t>(count);
    }

    static file from_handle(HANDLE handle, file::mode mode)
    {
        file f;

        if (handle == INVALID_HANDLE_VALUE)
        {
            return f;
        }

        if (mode == file::mode::NONE)
        {
            return f;
        }

        f.m_impl_data.handle = handle;
        f.m_mode = mode;

        return f;
    }

    static HANDLE get_handle(file& f)
    {
        return f.m_impl_data.handle.get();
    }

};

} // namespace __detail
} // namespace os
} // namespace vx
