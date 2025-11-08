#include "vertex_impl/os/_platform/platform_file.hpp"

namespace vx {
namespace os {

VX_API file::file() noexcept {}

VX_API file::~file() { close(); }

VX_API file::file(file&& other) noexcept
    : m_mode(other.m_mode)
    , m_handle(std::move(other.m_handle))
{
    other.m_mode = mode::none;
}

VX_API file& file::operator=(file&& other) noexcept
{
    if (this != &other)
    {
        close();

        m_mode = other.m_mode;
        m_handle = std::move(other.m_handle);

        other.m_mode = mode::none;
    }

    return *this;
}

VX_API void file::swap(file& other) noexcept
{
    std::swap(m_mode, other.m_mode);
    std::swap(m_handle, other.m_handle);
}

VX_API bool file::exists(const path& p)
{
    return _priv::file_impl::exists(p);
}

VX_API bool file::create(const path& p)
{
    file f;
    return f.open(p, mode::write);
}

VX_API bool file::open(const path& p, mode file_mode)
{
    if (is_open())
    {
        err::set(err::file_open_failed, "file already open");
        return false;
    }

    if (file_mode == mode::none)
    {
        err::set(err::file_open_failed, "invalid file mode");
        return false;
    }

    if (!_priv::file_impl::open(m_handle, p, file_mode))
    {
        return false;
    }

    m_mode = file_mode;
    return true;
}

VX_API bool file::is_open() const
{
    return (m_mode != mode::none) && m_handle.is_valid();
}

VX_API void file::close()
{
    m_handle.close();
    m_mode = mode::none;
}

VX_API size_t file::size() const
{
    return is_open() ? _priv::file_impl::size(m_handle) : 0;
}

VX_API bool file::resize(size_t size)
{
    return is_open() ? _priv::file_impl::resize(m_handle, size) : false;
}

VX_API bool file::seek(int off, stream_position from)
{
    return is_open() ? _priv::file_impl::seek(m_handle, off, from) : false;
}

VX_API size_t file::tell() const
{
    return is_open() ? _priv::file_impl::tell(m_handle) : invalid_position;
}

VX_API bool file::eof() const
{
    if (!is_open())
    {
        return false;
    }

    return _priv::file_impl::tell(m_handle) >= _priv::file_impl::size(m_handle);
}

VX_API bool file::flush()
{
    return is_open() ? _priv::file_impl::flush(m_handle) : false;
}

static bool read_check(const bool can_read)
{
    if (!can_read)
    {
        err::set(err::file_read_failed, "file not open in read mode");
    }
    return can_read;
}

static bool write_check(const bool can_write)
{
    if (!can_write)
    {
        err::set(err::file_write_failed, "file not open in write mode");
    }
    return can_write;
}

size_t file::read(uint8_t* data, size_t size)
{
    return !read_check(can_read()) ? 0 : _priv::file_impl::read(m_handle, data, size);
}

size_t file::write(const uint8_t* data, size_t size)
{
    return !write_check(can_write()) ? 0 : _priv::file_impl::write(m_handle, data, size);
}

VX_API bool file::read_line(std::string& line)
{
    if (!read_check(can_read()))
    {
        return false;
    }

    line.clear();

    char c = 0;
    while (_priv::file_impl::read(m_handle, reinterpret_cast<uint8_t*>(&c), 1) == 1)
    {
        if (c == '\n')
        {
#if defined(VX_OS_WINDOWS)

            // remove carriage return on windows
            if (!line.empty() && line.back() == '\r')
            {
                line.pop_back();
            }

#endif // VX_OS_WINDOWS

            return true;
        }

        line.push_back(c);
    }

    return false;
}

static bool write_line_internal(handle& h, const char* first, size_t size)
{
    constexpr size_t line_end_size = sizeof(VX_LINE_END) - 1;

    // NOTE: write check should happen before calling this function
    return (_priv::file_impl::write(h, reinterpret_cast<const uint8_t*>(first), size) == size)
        && (_priv::file_impl::write(h, reinterpret_cast<const uint8_t*>(VX_LINE_END), line_end_size) == line_end_size);
}

VX_API bool file::write_line(const char* line)
{
    return write_check(can_write()) && write_line_internal(m_handle, line, std::strlen(line));
}

VX_API bool file::write_file(const path& p, const char* text)
{
    file f;
    if (!f.open(p, mode::write))
    {
        return false;
    }

#if defined(VX_OS_WINDOWS)

    const char* s = text;
    const char* e = text;

    // Iterate over the text and split into lines
    while (*e != '\0')
    {
        if (*e == '\n')
        {
            if (!write_line_internal(f.m_handle, s, static_cast<size_t>(e - s)))
            {
                return false;
            }

            s = e + 1;
        }

        ++e;
}

    // Write the last section (no newline)
    if (s != e)
    {
        const size_t last_line_size = static_cast<size_t>(e - s);
        return (_priv::file_impl::write(f.m_handle, reinterpret_cast<const uint8_t*>(s), last_line_size) == last_line_size);
    }

    return true;

#else

    const size_t file_size = std::strlen(text);
    return (_priv::file_impl::write(f.m_handle, reinterpret_cast<const uint8_t*>(text), file_size) == file_size);

#endif // VX_OS_WINDOWS
}

file file::from_native_handle(typename handle::native_handle h, mode m)
{
    file f;

    if (!handle::is_valid_handle(h))
    {
        return f;
    }

    if (m == file::mode::none)
    {
        return f;
    }

    f.m_handle = h;
    f.m_mode = m;

    return f;
}

} // namespace os
} // namespace vx