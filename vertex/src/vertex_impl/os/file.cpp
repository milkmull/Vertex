#include "vertex_impl/os/__platform/file.hpp"

namespace vx {
namespace os {

VX_API bool file::exists(const path& p)
{
    return file_impl::exists(p);
}

VX_API bool file::create(const path& p)
{
    file f;
    return f.open(p, mode::WRITE);
}

VX_API bool file::open(const path& p, mode mode)
{
    if (is_open())
    {
        VX_ERR(err::FILE_OPEN_FAILED) << "file already open";
        return false;
    }

    if (mode == mode::NONE)
    {
        VX_ERR(err::FILE_OPEN_FAILED) << "invalid file mode";
        return false;
    }

    if (!file_impl::open(m_impl_data, p, mode))
    {
        return false;
    }

    m_mode = mode;
    return true;
}

VX_API bool file::is_open() const
{
    return (m_mode != mode::NONE) && file_impl::is_open(m_impl_data);
}

VX_API void file::close()
{
    file_impl::close(m_impl_data);
    m_mode = mode::NONE;
}

VX_API size_t file::size() const
{
    return is_open() ? file_impl::size(m_impl_data) : 0;
}

VX_API bool file::resize(size_t size)
{
    return is_open() ? file_impl::resize(m_impl_data, size) : false;
}

VX_API bool file::seek(int off, stream_position from)
{
    return is_open() ? file_impl::seek(m_impl_data, off, from) : false;
}

VX_API size_t file::tell() const
{
    return is_open() ? file_impl::tell(m_impl_data) : INVALID_POSITION;
}

VX_API bool file::eof() const
{
    if (!is_open())
    {
        return false;
    }

    return file_impl::tell(m_impl_data) >= file_impl::size(m_impl_data);
}

VX_API bool file::flush()
{
    return is_open() ? file_impl::flush(m_impl_data) : false;
}

bool file::read_check() const
{
    if (!can_read())
    {
        VX_ERR(err::FILE_READ_FAILED) << "file not open in read mode";
        return false;
    }

    return true;
}

bool file::write_check() const
{
    if (!can_write())
    {
        VX_ERR(err::FILE_WRITE_FAILED) << "file not open in write mode";
        return false;
    }

    return true;
}

size_t file::read_internal(uint8_t* data, size_t size)
{
    return !read_check() ? 0 : file_impl::read(m_impl_data, data, size);
}

size_t file::write_internal(const uint8_t* data, size_t size)
{
    return !write_check() ? 0 : file_impl::write(m_impl_data, data, size);
}

VX_API bool file::read_line(std::string& line)
{
    if (!read_check())
    {
        return false;
    }

    line.clear();

    char c = 0;
    while (file_impl::read(m_impl_data, reinterpret_cast<uint8_t*>(&c), 1) == 1)
    {
        if (c == '\n')
        {
#if defined(VX_PLATFORM_WINDOWS)

            // remove carriage return on windows
            if (!line.empty() && line.back() == '\r')
            {
                line.pop_back();
            }

#endif // VX_PLATFORM_WINDOWS

            return true;
        }

        line.push_back(c);
    }

    return false;
}

bool file::write_line_internal(const char* first, size_t size)
{
    constexpr size_t line_end_size = sizeof(VX_LINE_END) - 1;

    // NOTE: write check should happen before calling this function
    return (file_impl::write(m_impl_data, reinterpret_cast<const uint8_t*>(first), size) == size)
        && (file_impl::write(m_impl_data, reinterpret_cast<const uint8_t*>(VX_LINE_END), line_end_size) == line_end_size);
}

#if defined(VX_PLATFORM_WINDOWS)

bool file::windows_write_text_file_internal(const char* text, size_t size)
{
    const char* s = text;
    const char* e = text;

    // Iterate over the text and split into lines
    while (*e != '\0')
    {
        if (*e == '\n')
        {
            write_line_internal(s, static_cast<size_t>(e - s));
            s = e + 1;
        }

        ++e;
    }

    // Write the last section (no newline)
    if (s != e)
    {
        write_internal(reinterpret_cast<const uint8_t*>(s), static_cast<size_t>(e - s));
    }

    return true;
}

#endif // VX_PLATFORM_WINDOWS

} // namespace os
} // namespace vx