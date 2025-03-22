#pragma once

#include <cstring>
#include <vector>

#include "vertex/os/handle.hpp"
#include "vertex/os/path.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// file
///////////////////////////////////////////////////////////////////////////////

enum class stream_position
{
    BEGIN,
    CURRENT,
    END
};

class process;

class file
{
    friend process;

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

    enum : size_t
    {
        INVALID_SIZE        = std::numeric_limits<size_t>::max(),
        INVALID_POSITION    = std::numeric_limits<size_t>::max()
    };

public:

    VX_API file() noexcept;
    VX_API ~file();

    file(const file&) = delete;
    file& operator=(const file&) = delete;

    VX_API file(file&& other) noexcept;
    VX_API file& operator=(file&& other) noexcept;

    VX_API void swap(file& other) noexcept;

public:

    VX_API static bool exists(const path& p);
    VX_API static bool create(const path& p);

public:

    VX_API bool open(const path& p, mode file_mode);
    VX_API bool is_open() const;
    VX_API void close();

    mode get_mode() const noexcept { return m_mode; }
    bool can_read() const noexcept { return (m_mode == mode::READ || m_mode == mode::READ_WRITE_EXISTS || m_mode == mode::READ_WRITE_CREATE); }
    bool can_write() const noexcept { return (m_mode == mode::WRITE || m_mode == mode::READ_WRITE_EXISTS || m_mode == mode::READ_WRITE_CREATE || m_mode == mode::APPEND); }

    VX_API size_t size() const;
    VX_API bool resize(size_t size);
    bool clear() { return resize(0); }

    VX_API bool seek(int off, stream_position from = stream_position::BEGIN);
    VX_API size_t tell() const;
    VX_API bool eof() const;

    VX_API bool flush();

public:

    ///////////////////////////////////////////////////////////////////////////////
    // generic read functions
    ///////////////////////////////////////////////////////////////////////////////

    VX_API size_t read(uint8_t* data, size_t size);

    template <typename T>
    size_t read(T& data)
    {
        return read(reinterpret_cast<uint8_t*>(&data), sizeof(T));
    }

    template <typename T>
    size_t read(T* data, size_t count)
    {
        return read(reinterpret_cast<uint8_t*>(data), sizeof(T) * count);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // generic write functions
    ///////////////////////////////////////////////////////////////////////////////

    VX_API size_t write(const uint8_t* data, size_t size);

    template <typename T>
    size_t write(const T& data)
    {
        return write(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
    }

    template <typename T>
    size_t write(const T* data, size_t count)
    {
        return write(reinterpret_cast<const uint8_t*>(data), sizeof(T) * count);
    }


    ///////////////////////////////////////////////////////////////////////////////
    // text write functions
    ///////////////////////////////////////////////////////////////////////////////

    size_t write(const char* text)
    {
        return write(reinterpret_cast<const uint8_t*>(text), std::strlen(text));
    }

    size_t write(const std::string& text)
    {
        return write(reinterpret_cast<const uint8_t*>(text.data()), text.size());
    }

    ///////////////////////////////////////////////////////////////////////////////
    // read and write line functions
    ///////////////////////////////////////////////////////////////////////////////

    VX_API bool read_line(std::string& line);
    VX_API bool write_line(const char* line);

    bool write_line(const std::string& line)
    {
        return write_line(line.c_str());
    }

public:

    ///////////////////////////////////////////////////////////////////////////////
    // file read functions
    ///////////////////////////////////////////////////////////////////////////////

    static bool read_file(const path& p, std::vector<uint8_t>& data)
    {
        file f;
        if (!f.open(p, mode::READ))
        {
            return false;
        }

        data.resize(f.size());
        return f.read(data.data(), data.size());
    }

    static bool read_file(const path& p, std::string& text)
    {
        file f;
        if (!f.open(p, mode::READ))
        {
            return false;
        }

        text.resize(f.size());
        return f.read(reinterpret_cast<uint8_t*>(text.data()), text.size());
    }

    ///////////////////////////////////////////////////////////////////////////////
    // file write functions
    ///////////////////////////////////////////////////////////////////////////////

    static bool write_file(const path& p, const uint8_t* data, size_t size)
    {
        file f;
        return f.open(p, mode::WRITE) && f.write(data, size);
    }

    VX_API static bool write_file(const path& p, const char* text);

    static bool write_file(const path& p, const std::string& text)
    {
        return write_file(p, text.c_str());
    }

    static bool clear_file(const path& p)
    {
        file f;
        return f.open(p, mode::READ_WRITE_EXISTS) && f.clear();
    }

public:

    static file from_handle(handle h, mode m);
    handle get_handle() const { return m_handle.get(); }

private:

    mode m_mode = mode::NONE;
    handle m_handle;
};

} // namespace os
} // namespace vx
