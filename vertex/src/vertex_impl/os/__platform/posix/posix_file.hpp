#pragma once

#include <sys/stat.h>

#include "vertex_impl/os/__platform/posix/posix_tools.hpp"
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
        struct stat stat_buf;
        return (::stat(p.c_str(), &stat_buf) == 0 && !S_ISDIR(stat_buf.st_mode));
    }

    static bool open(__detail::file_impl_data& fd, const path& p, file::mode mode)
    {
        VX_ASSERT_MESSAGE(!fd.handle.is_valid(), "file already open");

        int flags = 0;
        mode_t file_mode = 0;

        switch (mode)
        {
            // File must exist
            case file::mode::READ:
            {
                flags = O_RDONLY;
                break;
            }
            // Always create or truncate the file
            case file::mode::WRITE:
            {
                flags = O_WRONLY | O_CREAT | O_TRUNC;
                file_mode = S_IRUSR | S_IWUSR; // Read and write permissions
                break;
            }
            // Create if not exists, otherwise open existing
            case file::mode::APPEND:
            {
                flags = O_WRONLY | O_CREAT | O_APPEND;
                file_mode = S_IRUSR | S_IWUSR;
                break;
            }
            // File must exist
            case file::mode::READ_WRITE_EXISTS:
            {
                flags = O_RDWR;
                break;
            }
            // Truncate if file exists, or create a new one
            case file::mode::READ_WRITE_CREATE:
            {
                flags = O_RDWR | O_CREAT | O_TRUNC;
                file_mode = S_IRUSR | S_IWUSR;
                break;
            }
        }

        fd.handle = ::open(p.c_str(), flags, file_mode);

        if (!fd.handle.is_valid())
        {
            posix::error_message("open()");
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

        struct stat stat_buf;
        if (::fstat(fd.handle.get(), &stat_buf) != 0)
        {
            posix::error_message("fstat()");
            return file::INVALID_SIZE;
        }

        return static_cast<size_t>(stat_buf.st_size);
    }

    static bool resize(__detail::file_impl_data& fd, size_t size)
    {
        assert_is_open(fd.handle);

        if (::ftruncate(fd.handle.get(), size) != 0)
        {
            posix::error_message("ftruncate()");
            return false;
        }

        return true;
    }

    static bool seek(__detail::file_impl_data& fd, int off, stream_position from)
    {
        assert_is_open(fd.handle);

        int whence = 0;
        switch (from)
        {
            case stream_position::BEGIN:
            {
                whence = SEEK_SET;
                break;
            }
            case stream_position::CURRENT:
            {
                whence = SEEK_CUR;
                break;
            }
            case stream_position::END:
            {
                whence = SEEK_END;
                break;
            }
        }

        if (::lseek(fd.handle.get(), off, whence) == static_cast<off_t>(-1))
        {
            posix::error_message("lseek()");
            return false;
        }

        return true;
    }

    static size_t tell(const __detail::file_impl_data& fd)
    {
        assert_is_open(fd.handle);

        const off_t pos = ::lseek(fd.handle.get(), 0, SEEK_CUR);
        if (pos == static_cast<off_t>(-1))
        {
            posix::error_message("lseek()");
            return file::INVALID_POSITION;
        }

        return static_cast<size_t>(pos);
    }

    static bool flush(__detail::file_impl_data& fd)
    {
        assert_is_open(fd.handle);

        if (::fsync(fd.handle.get()) != 0)
        {
            posix::error_message("fsync()");
            return false;
        }

        return true;
    }

    static size_t read(__detail::file_impl_data& fd, uint8_t* data, size_t size)
    {
        assert_is_open(fd.handle);

        const ssize_t count = ::read(fd.handle.get(), data, size);
        if (count < 0)
        {
            posix::error_message("read()");
            return 0;
        }

        return static_cast<size_t>(count);
    }

    static size_t write(__detail::file_impl_data& fd, const uint8_t* data, size_t size)
    {
        assert_is_open(fd.handle);

        ssize_t count = ::write(fd.handle.get(), data, size);
        if (count < 0)
        {
            posix::error_message("write()");
            return 0;
        }

        return static_cast<size_t>(count);
    }

    static file from_handle(int handle, file::mode mode)
    {
        file f;

        if (handle < 0)
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

    static int get_handle(file& f)
    {
        return f.m_impl_data.handle.get();
    }

};

} // namespace __detail
} // namespace os
} // namespace vx