#include "vertex/system/platform_config.hpp"
#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/_platform/windows/windows_file.hpp"
#else
#   include "vertex_impl/os/_platform/dummy/dummy_file.hpp"
#endif

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
    , m_impl(std::move(other.m_impl))
{
    other.m_mode = mode::NONE;
    other.m_impl = nullptr;
}

VX_API file& file::operator=(file&& other) noexcept
{
    if (this != &other)
    {
        close();

        m_mode = other.m_mode;
        m_impl = std::move(other.m_impl);

        other.m_mode = mode::NONE;
        other.m_impl = nullptr;
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

    m_impl = std::make_unique<file_impl>();
    if (!m_impl)
    {
        return false;
    }

    if (!m_impl->open(p, mode))
    {
        m_impl = nullptr;
        return false;
    }

    m_mode = mode;
    return true;
}

VX_API bool file::is_open() const
{
    return (m_mode != mode::NONE) && m_impl && m_impl->is_open();
}

VX_API void file::close()
{
    if (m_impl)
    {
        m_impl->close();
        m_impl = nullptr;
    }

    m_mode = mode::NONE;
}

VX_API size_t file::size() const
{
    if (!is_open())
    {
        return 0;
    }

    return m_impl->size();
}

VX_API bool file::resize(size_t size)
{
    if (!is_open())
    {
        return false;
    }

    return m_impl->resize(size);
}

VX_API bool file::seek(int off, stream_position from)
{
    if (!is_open())
    {
        return false;
    }

    return m_impl->seek(off, from);
}

VX_API size_t file::tell() const
{
    if (!is_open())
    {
        return INVALID_POSITION;
    }

    return m_impl->tell();
}

VX_API bool file::eof() const
{
    if (!is_open())
    {
        return false;
    }

    return tell() >= size();
}

VX_API bool file::flush()
{
    if (!is_open())
    {
        return false;
    }

    return m_impl->flush();
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

    return m_impl->read(data, size);
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

    return m_impl->write(data, size);
}

} // namespace os
} // namespace vx