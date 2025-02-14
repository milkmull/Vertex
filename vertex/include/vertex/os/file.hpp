#pragma once

#include <vector>
#include <sstream>

#include "vertex/system/compiler.hpp"
#include "vertex/os/path.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// file
///////////////////////////////////////////////////////////////////////////////

class process;

enum class stream_position
{
    BEGIN,
    CURRENT,
    END
};

class file
{
    friend os::process;

public:

    // https://man7.org/linux/man-pages/man3/fopen.3.html

    enum class mode
    {
        NONE,
        READ,               // File must exist
        WRITE,              // Always create or truncate the file
        APPEND,             // Create if not exists, otherwise open existing
        READ_WRITE_EXISTS,  // File must exist
        READ_WRITE_CREATE   // Truncate if file exists, or create a new one
    };

    enum : size_t { INVALID_SIZE = -1, INVALID_POSITION = -1 };

public:

    VX_API file();
    VX_API ~file();

    file(const file&) = delete;
    VX_API file(file&&) noexcept;

    file& operator=(const file&) = delete;
    VX_API file& operator=(file&&) noexcept;

public:

    VX_API static bool exists(const path& p);
    VX_API static bool create(const path& p);

public:

    VX_API bool open(const path& p, mode mode);
    VX_API bool is_open() const;
    VX_API void close();

    inline mode get_mode() const noexcept { return m_mode; }
    inline bool can_read() const noexcept { return (m_mode == mode::READ || m_mode == mode::READ_WRITE_EXISTS || m_mode == mode::READ_WRITE_CREATE); }
    inline bool can_write() const noexcept { return (m_mode == mode::WRITE || m_mode == mode::READ_WRITE_EXISTS || m_mode == mode::READ_WRITE_CREATE || m_mode == mode::APPEND); }

    VX_API size_t size() const;
    VX_API bool resize(size_t size);

    VX_API bool seek(int off, stream_position from = stream_position::BEGIN);
    VX_API size_t tell() const;
    VX_API bool eof() const;

    VX_API bool flush();

private:

    VX_API size_t read_internal(uint8_t* data, size_t size);
    VX_API size_t write_internal(const uint8_t* data, size_t size);

public:

    template <typename T>
    inline size_t read(T& data)
    {
        return read_internal(reinterpret_cast<uint8_t*>(&data), sizeof(T));
    }

    template <typename T>
    inline size_t read(T* data, size_t count)
    {
        return read_internal(reinterpret_cast<uint8_t*>(data), sizeof(T) * count);
    }

    template <typename T>
    inline size_t write(const T& data)
    {
        return write_internal(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
    }

    template <typename T>
    inline size_t write(const T* data, size_t count)
    {
        return write_internal(reinterpret_cast<const uint8_t*>(data), sizeof(T) * count);
    }

public:

    static inline bool read_file(const path& p, std::vector<uint8_t>& data)
    {
        bool success = false;

        file f;
        if (f.open(p, mode::READ))
        {
            const size_t size = f.size();
            data.resize(size);
            success = f.read_internal(data.data(), size);
        }

        return success;
    }

    static inline bool write_file(const path& p, const uint8_t* data, size_t size)
    {
        file f;
        return f.open(p, mode::WRITE) && f.write_internal(data, size);
    }

private:

    mode m_mode;

    class file_impl;
    std::unique_ptr<file_impl> m_impl;
};

} // namespace os
} // namespace vx
