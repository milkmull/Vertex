#include "vertex/system/platform.hpp"

#if defined(VX_PLATFORM_WINDOWS)
#   include "_win32/win32_file.hpp"
#else

#endif

namespace vx {

static void file_not_open_error()
{
    VX_ERROR(error::error_code::FILE_OPERATION_FAILED) << "file not open";
}

#define IS_READ_MODE(mode) (mode == file_mode::READ || mode == file_mode::READ_WRITE_EXISTS || mode == file_mode::READ_WRITE_CREATE)
#define IS_WRITE_MODE(mode) (mode != file_mode::READ)
#define IS_APPEND_MODE(mode) (mode == file_mode::APPEND)

file::file() : m_mode{} {}

file::~file()
{
    close();
}

file::file(file&& f) noexcept
    : m_mode(f.m_mode)
    , m_impl(std::move(f.m_impl)) {}

file& file::operator=(file&& f) noexcept
{
    m_mode = f.m_mode;
    m_impl = std::move(f.m_impl);
    return *this;
}

bool file::create(const std::string& path)
{
    file f;
    return f.open(path, file_mode::WRITE);
}

bool file::read_file(const std::string& path, std::vector<uint8_t>& data)
{
    bool success = false;

    file f;
    if (f.open(path, file_mode::READ))
    {
        const size_t size = f.size();
        data.resize(size);
        success = f.read(data.data(), size);
    }

    return success;
}

bool file::write_file(const std::string& path, const uint8_t* data, size_t size)
{
    file f;
    return f.open(path, file_mode::WRITE) && f.write(data, size);
}

bool file::read_text_file(const std::string& path, std::string& text)
{
    bool success = false;

    file f;
    if (f.open(path, file_mode::READ))
    {
        const size_t size = f.size();
        text.resize(size);
        success = f.read(reinterpret_cast<uint8_t*>(text.data()), size);
    }

    return success;
}

bool file::write_text_file(const std::string& path, const std::string& text)
{
    return write_file(path, reinterpret_cast<const uint8_t*>(text.data()), text.size());
}

bool file::open(const std::string& path, file_mode mode)
{
    if (is_open())
    {
        VX_ERROR(error::error_code::FILE_OPEN_FAILED) << "file already open";
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

bool file::is_open() const
{
    return m_impl && m_impl->is_open();
}

void file::close()
{
    if (m_impl)
    {
        m_impl->close();
        m_impl = nullptr;
    }
}

size_t file::size() const
{
    if (!is_open())
    {
        file_not_open_error();
        return 0;
    }

    return m_impl->size();
}

bool file::seek(size_t off, stream_position from)
{
    if (!is_open())
    {
        file_not_open_error();
        return false;
    }

    return m_impl->seek(off, from);
}

size_t file::tell() const
{
    if (!is_open())
    {
        file_not_open_error();
        return INVALID_POSITION;
    }

    return m_impl->tell();
}

bool file::eof() const
{
    return tell() >= size();
}

size_t file::read(uint8_t* data, size_t size)
{
    if (!is_open())
    {
        file_not_open_error();
        return 0;
    }

    if (!IS_READ_MODE(m_mode))
    {
        VX_ERROR(error::error_code::FILE_READ_FAILED) << "file not open in read mode";
        return 0;
    }

    return m_impl->read(data, size);
}

size_t file::write(const uint8_t* data, size_t size)
{
    if (!is_open())
    {
        file_not_open_error();
        return false;
    }

    if (!IS_WRITE_MODE(m_mode))
    {
        VX_ERROR(error::error_code::FILE_WRITE_FAILED) << "file not open in write mode";
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

size_t file::read_text(std::string& text, size_t count)
{
    text.resize(count);
    return read(reinterpret_cast<uint8_t*>(text.data()), count);
}

size_t file::write_text(const std::string& text)
{
    return write(reinterpret_cast<const uint8_t*>(text.data()), text.size());
}

bool file::flush()
{
    if (!is_open())
    {
        file_not_open_error();
        return false;
    }

    return m_impl->flush();
}

} // namespace vx