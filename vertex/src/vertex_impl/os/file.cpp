#include "vertex_impl/os/__platform/file.hpp"

namespace vx {
namespace os {

VX_API file::file()
    : m_mode(mode::NONE) {}

VX_API file::~file()
{
    close();
}

VX_API file::file(file&& other) noexcept
    : m_mode(other.m_mode)
    , m_impl_data(std::move(other.m_impl_data))
{
    other.m_mode = mode::NONE;
}

VX_API file& file::operator=(file&& other) noexcept
{
    if (this != &other)
    {
        close();

        m_mode = other.m_mode;
        m_impl_data = std::move(other.m_impl_data);

        other.m_mode = mode::NONE;
    }

    return *this;
}

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

    if (!file_impl::open(p, mode, m_impl_data))
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
    return is_open() ? file_impl::resize(size, m_impl_data) : false;
}

VX_API bool file::seek(int off, stream_position from)
{
    return is_open() ? file_impl::seek(off, from, m_impl_data) : false;
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

VX_API size_t file::read_internal(uint8_t* data, size_t size)
{
    if (!is_open())
    {
        return 0;
    }

    if (!can_read())
    {
        VX_ERR(err::FILE_READ_FAILED) << "file not open in read mode";
        return 0;
    }

    return file_impl::read(data, size, m_impl_data);
}

VX_API size_t file::write_internal(const uint8_t* data, size_t size)
{
    if (!is_open())
    {
        return 0;
    }

    if (!can_write())
    {
        VX_ERR(err::FILE_WRITE_FAILED) << "file not open in write mode";
        return 0;
    }

    return file_impl::write(data, size, m_impl_data);
}

VX_API bool file::read_line(std::string& line)
{
    line.clear();

    if (!is_open())
    {
        return false;
    }

    if (!can_read())
    {
        VX_ERR(err::FILE_READ_FAILED) << "file not open in read mode";
        return false;
    }

    char c = 0;
    while (file_impl::read(reinterpret_cast<uint8_t*>(&c), 1, m_impl_data) == 1)
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

VX_API bool file::write_line(const char* line)
{
    if (!is_open())
    {
        return false;
    }

    if (!can_write())
    {
        VX_ERR(err::FILE_WRITE_FAILED) << "file not open in write mode";
        return false;
    }

    const size_t size = std::strlen(line);
    constexpr size_t line_end_size = sizeof(VX_LINE_END) - 1;

    return (file_impl::write(reinterpret_cast<const uint8_t*>(line), size, m_impl_data) == size)
        && (file_impl::write(reinterpret_cast<const uint8_t*>(VX_LINE_END), line_end_size, m_impl_data) == line_end_size);
}

} // namespace os
} // namespace vx