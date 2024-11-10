#include "vertex/core/platform_config.hpp"

#if defined(VX_OS_WINDOWS_FILE)
#   include "_windows/windows_file.hpp"
#else

#endif

namespace vx {
namespace os {

#define IS_READ_MODE(m)   (m == file::mode::READ || m == file::mode::READ_WRITE_EXISTS || m == file::mode::READ_WRITE_CREATE)
#define IS_WRITE_MODE(m)  (m != file::mode::READ)
#define IS_APPEND_MODE(m) (m == file::mode::APPEND)

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

VX_API void swap(file& lhs, file& rhs) noexcept
{
    std::swap(lhs.m_mode, rhs.m_mode);
    std::swap(lhs.m_impl, rhs.m_impl);
}

VX_API bool file::exists(const std::string& path)
{
    return file_impl::exists(path);
}

VX_API bool file::create(const std::string& path)
{
    file f;
    return f.open(path, mode::WRITE);
}

VX_API bool file::read_file(const std::string& path, std::vector<uint8_t>& data)
{
    bool success = false;

    file f;
    if (f.open(path, mode::READ))
    {
        const size_t size = f.size();
        data.resize(size);
        success = f.read(data.data(), size);
    }

    return success;
}

VX_API bool file::write_file(const std::string& path, const uint8_t* data, size_t size)
{
    file f;
    return f.open(path, mode::WRITE) && f.write(data, size);
}

VX_API bool file::read_text_file(const std::string& path, std::string& text)
{
    bool success = false;

    file f;
    if (f.open(path, mode::READ))
    {
        const size_t size = f.size();
        text.resize(size);
        success = f.read(reinterpret_cast<uint8_t*>(text.data()), size);
    }

    return success;
}

VX_API bool file::write_text_file(const std::string& path, const std::string& text)
{
    return write_file(path, reinterpret_cast<const uint8_t*>(text.data()), text.size());
}

VX_API bool file::open(const std::string& path, mode mode)
{
    if (is_open())
    {
        VX_ERR(err::FILE_OPEN_FAILED) << "file already open";
        return false;
    }

    m_impl = std::make_unique<file_impl>();
    if (!m_impl)
    {
        return false;
    }

    if (!m_impl->open(path, mode))
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

VX_API bool file::can_read() const
{
    return IS_READ_MODE(m_mode);
}

VX_API bool file::can_write() const
{
    return IS_WRITE_MODE(m_mode);
}

VX_API file::mode file::get_mode() const
{
    return m_mode;
}

VX_API size_t file::size() const
{
    if (!is_open())
    {
        return 0;
    }

    return m_impl->size();
}

VX_API bool file::seek(size_t off, stream_position from)
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
    return tell() >= size();
}

VX_API size_t file::read(uint8_t* data, size_t size)
{
    if (!is_open())
    {
        return 0;
    }

    if (!IS_READ_MODE(m_mode))
    {
        VX_ERR(err::FILE_READ_FAILED) << "file not open in read mode";
        return 0;
    }

    return m_impl->read(data, size);
}

VX_API size_t file::write(const uint8_t* data, size_t size)
{
    if (!is_open())
    {
        return 0;
    }

    if (!IS_WRITE_MODE(m_mode))
    {
        VX_ERR(err::FILE_WRITE_FAILED) << "file not open in write mode";
        return 0;
    }

    if (IS_APPEND_MODE(m_mode))
    {
        if (!seek(0, stream_position::END))
        {
            return false;
        }
    }

    return m_impl->write(data, size);
}

VX_API size_t file::read_text(std::string& text, size_t count)
{
    text.resize(count);
    return read(reinterpret_cast<uint8_t*>(text.data()), count);
}

VX_API size_t file::write_text(const std::string& text)
{
    return write(reinterpret_cast<const uint8_t*>(text.data()), text.size());
}

VX_API bool file::flush()
{
    if (!is_open())
    {
        return false;
    }

    return m_impl->flush();
}

VX_API file::ostream_proxy file::operator<<(std::ostream& (*func)(std::ostream&))
{
    ostream_proxy stream(*this);
    stream << func;
    return stream;
}

} // namespace os
} // namespace vx