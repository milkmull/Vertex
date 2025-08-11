#pragma once

#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
#include "vertex/os/file.hpp"

namespace vx {
namespace os {
namespace _priv {

#define assert_is_open(h) VX_ASSERT_MESSAGE(h.is_valid(), "file not open")

struct file_impl
{
    static bool exists(const path& p)
    {
        const DWORD attrs = ::GetFileAttributesW(p.c_str());
        return (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY));
    }

    static bool open(handle& h, const path& p, file::mode mode)
    {
        VX_ASSERT_MESSAGE(!h.is_valid(), "file already open");

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

        h = ::CreateFileW(
            p.c_str(),
            access,
            FILE_SHARE_READ | FILE_SHARE_WRITE, // allow read and write sharing
            NULL,
            creation,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (!h.is_valid())
        {
            windows::error_message("CreateFileW()");
            return false;
        }

        return true;
    }

    static size_t size(const handle& h)
    {
        assert_is_open(h);

        LARGE_INTEGER size;

        if (!::GetFileSizeEx(h.get(), &size))
        {
            windows::error_message("GetFileSizeEx()");
            return file::INVALID_SIZE;
        }

        return static_cast<size_t>(size.QuadPart);
    }

    static bool seek(handle& h, int off, stream_position from)
    {
        assert_is_open(h);

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

        if (!::SetFilePointerEx(h.get(), distance, &distance, method))
        {
            windows::error_message("SetFilePointerEx()");
            return false;
        }

        return true;
    }

    static bool resize(handle& h, size_t size)
    {
        assert_is_open(h);

        if (!seek(h, static_cast<int>(size), stream_position::BEGIN))
        {
            return false;
        }

        if (!::SetEndOfFile(h.get()))
        {
            windows::error_message("SetEndOfFile()");
            return false;
        }

        return true;
    }

    static size_t tell(const handle& h)
    {
        assert_is_open(h);

        LARGE_INTEGER off{ 0 };
        off.QuadPart = 0;

        // Use SetFilePointerEx to query the current position
        if (!::SetFilePointerEx(h.get(), off, &off, FILE_CURRENT))
        {
            windows::error_message("SetFilePointerEx()");
            return file::INVALID_POSITION;
        }

        return static_cast<size_t>(off.QuadPart);
    }

    static bool flush(handle& h)
    {
        assert_is_open(h);

        if (!::FlushFileBuffers(h.get()))
        {
            windows::error_message("FlushFileBuffers()");
            return false;
        }

        return true;
    }

    static size_t read(handle& h, uint8_t* data, size_t size)
    {
        assert_is_open(h);

        DWORD count = 0;
        if (!::ReadFile(h.get(), data, static_cast<DWORD>(size), &count, NULL))
        {
            windows::error_message("ReadFile()");
            return 0;
        }

        return static_cast<size_t>(count);
    }

    static size_t write(handle& h, const uint8_t* data, size_t size)
    {
        assert_is_open(h);

        DWORD count = 0;
        if (!::WriteFile(h.get(), data, static_cast<DWORD>(size), &count, NULL))
        {
            windows::error_message("WriteFile()");
            return 0;
        }

        return static_cast<size_t>(count);
    }

    static file from_native_handle(typename handle::native_handle h, file::mode m)
    {
        return file::from_native_handle(h, m);
    }

    static typename handle::native_handle get_native_handle(const file& f)
    {
        return f.get_native_handle();
    }
};

} // namespace _priv
} // namespace os
} // namespace vx