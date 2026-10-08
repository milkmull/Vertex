#pragma once

#include "vertex/os/io.hpp"
#include "vertex/std/util.hpp"

namespace vx {

//=============================================================================
// file
//=============================================================================

class file
{
public:

    using rw_result = os::rw_result;
    using stream_position = os::stream_position;
    using mode = os::rw_mode;
    using position_type = os::file_position;
    using offset_type = os::file_offset;

public:

    file() noexcept = default;

    ~file()
    {
        close();
    }

    file(const file&) = delete;
    file& operator=(const file&) = delete;

    file(file&& other) noexcept
        : m_mode(other.m_mode)
        , m_handle(std::move(other.m_handle))
    {
        other.m_mode = mode::none;
    }

    file& operator=(file&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        close();

        m_mode = other.m_mode;
        m_handle = std::move(other.m_handle);
        other.m_mode = mode::none;

        return *this;
    }

    void swap(file& other) noexcept
    {
        vx::swap(m_mode, other.m_mode);
        vx::swap(m_handle, other.m_handle);
    }

public:

    static success create(const os::path& p)
    {
        file f;
        return f.open(p.c_str(), mode::write);
    }

public:

    success open(const os::path& p, typename mode::enum_type file_mode)
    {
        VX_ASSERT(!is_open());
        VX_ASSERT(file_mode != mode::none);

        const auto ok = os::open(m_handle, p.c_str(), file_mode);
        if (ok)
        {
            m_mode = file_mode;
        }

        return ok;
    }

    bool is_open() const
    {
        return m_handle.is_valid();
    }

    explicit operator bool() const
    {
        return is_open();
    }

    void close()
    {
        m_handle.close();
        m_mode = mode::none;
    }

    mode get_mode() const noexcept
    {
        return m_mode;
    }

    bool can_read() const noexcept
    {
        return (m_mode & mode::_can_read);
    }

    bool can_write() const noexcept
    {
        return (m_mode & mode::_can_write);
    }

    bool can_read_write() const noexcept
    {
        return can_read() && can_write();
    }

    success seek(offset_type off, stream_position from = stream_position::begin)
    {
        VX_ASSERT(is_open());
        return os::seek(m_handle, off, from);
    }

    expected<position_type, error> tell() const
    {
        VX_ASSERT(is_open());
        return os::tell(m_handle);
    }

    success flush()
    {
        VX_ASSERT(is_open());
        return os::flush(m_handle);
    }

    expected<size_t, error> size() const
    {
        VX_ASSERT(is_open());
        return os::file_size(m_handle);
    }

public:

    //=============================================================================
    // generic read functions
    //=============================================================================

    rw_result read(uint8_t* data, size_t size)
    {
        VX_ASSERT(is_open());
        VX_ASSERT(can_read());
        return os::read(m_handle, data, size);
    }

    template <typename T>
    rw_result read(T& data)
    {
        return read(reinterpret_cast<uint8_t*>(&data), sizeof(T));
    }

    template <typename T>
    rw_result read(T* data, size_t count)
    {
        return read(reinterpret_cast<uint8_t*>(data), sizeof(T) * count);
    }

    //=============================================================================
    // generic write functions
    //=============================================================================

    rw_result write(const uint8_t* data, size_t size)
    {
        VX_ASSERT(is_open());
        VX_ASSERT(can_write());
        return os::write(m_handle, data, size);
    }

    template <typename T>
    rw_result write(const T& data)
    {
        return write(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
    }

    template <typename T>
    rw_result write(const T* data, size_t count)
    {
        return write(reinterpret_cast<const uint8_t*>(data), sizeof(T) * count);
    }

    rw_result endl()
    {
        return write(reinterpret_cast<const uint8_t*>(VX_LINE_END), sizeof(VX_LINE_END) - 1);
    }

private:

    typename mode::enum_type m_mode = mode::none;
    os::handle m_handle;
};

} // namespace vx
