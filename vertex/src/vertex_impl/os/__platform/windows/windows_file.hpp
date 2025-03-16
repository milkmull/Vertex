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

    static bool open(__detail::file_impl_data& fd, const path& p, file::mode mode)
    {
        VX_ASSERT_MESSAGE(!fd.h.is_valid(), "file already open");

        DWORD access = 0;
        DWORD creation = 0;

        switch (mode)
        {
            // File must exist
            case file::mode::NONE:
            case file::mode::READ:
            {
                access = GENERIC_READ;
                creation = OPEN_EXISTING;
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

        fd.h = CreateFileW(
            p.c_str(),
            access,
            FILE_SHARE_READ | FILE_SHARE_WRITE, // allow read and write sharing
            NULL,
            creation,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (!fd.h.is_valid())
        {
            windows::error_message("CreateFileW()");
            return false;
        }

        return true;
    }

    static bool is_open(const __detail::file_impl_data& fd)
    {
        return fd.h.is_valid();
    }

    static void close(__detail::file_impl_data& fd)
    {
        fd.h.close();
    }

    static size_t size(const __detail::file_impl_data& fd)
    {
        assert_is_open(fd.h);

        LARGE_INTEGER size;

        if (!GetFileSizeEx(fd.h.get(), &size))
        {
            windows::error_message("GetFileSizeEx()");
            return file::INVALID_SIZE;
        }

        return static_cast<size_t>(size.QuadPart);
    }

    static bool resize(__detail::file_impl_data& fd, size_t size)
    {
        assert_is_open(fd.h);

        if (!seek(fd, static_cast<int>(size), stream_position::BEGIN))
        {
            return false;
        }

        if (!SetEndOfFile(fd.h.get()))
        {
            windows::error_message("SetEndOfFile()");
            return false;
        }

        return true;
    }

    static bool seek(__detail::file_impl_data& fd, int off, stream_position from)
    {
        assert_is_open(fd.h);

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

        if (!SetFilePointerEx(fd.h.get(), distance, &distance, method))
        {
            windows::error_message("SetFilePointerEx()");
            return false;
        }

        return true;
    }

    static size_t tell(const __detail::file_impl_data& fd)
    {
        assert_is_open(fd.h);

        LARGE_INTEGER off{ 0 };
        off.QuadPart = 0;

        // Use SetFilePointerEx to query the current position
        if (!SetFilePointerEx(fd.h.get(), off, &off, FILE_CURRENT))
        {
            windows::error_message("SetFilePointerEx()");
            return file::INVALID_POSITION;
        }

        return static_cast<size_t>(off.QuadPart);
    }

    static bool flush(__detail::file_impl_data& fd)
    {
        assert_is_open(fd.h);

        if (!FlushFileBuffers(fd.h.get()))
        {
            windows::error_message("FlushFileBuffers()");
            return false;
        }

        return true;
    }

    static size_t read(__detail::file_impl_data& fd, uint8_t* data, size_t size)
    {
        assert_is_open(fd.h);

        DWORD count = 0;
        if (!ReadFile(fd.h.get(), data, static_cast<DWORD>(size), &count, NULL))
        {
            windows::error_message("ReadFile()");
            return 0;
        }

        return static_cast<size_t>(count);
    }

    static size_t write(__detail::file_impl_data& fd, const uint8_t* data, size_t size)
    {
        assert_is_open(fd.h);

        DWORD count = 0;
        if (!WriteFile(fd.h.get(), data, static_cast<DWORD>(size), &count, NULL))
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

        f.m_impl_data.h = handle;
        f.m_mode = mode;

        return f;
    }

    static HANDLE get_handle(file& f)
    {
        return f.m_impl_data.h.get();
    }

};

} // namespace __detail
} // namespace os
} // namespace vx