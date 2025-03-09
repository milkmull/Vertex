#pragma once

#include <vector>

#include "vertex/os/__platform/file_impl_data.hpp"
#include "vertex/os/path.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// file
///////////////////////////////////////////////////////////////////////////////

namespace __detail {

class file_impl;

} // namespace __detail

enum class stream_position
{
    BEGIN,
    CURRENT,
    END
};

class file
{
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
    file& operator=(const file&) = delete;

    VX_API file(file&&) noexcept;
    VX_API file& operator=(file&&) noexcept;

public:

    VX_API static bool exists(const path& p);
    VX_API static bool create(const path& p);

public:

    VX_API bool open(const path& p, mode mode);
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

private:

    VX_API bool read_check() const;
    VX_API bool write_check() const;

    VX_API size_t read_internal(uint8_t* data, size_t size);
    VX_API size_t write_internal(const uint8_t* data, size_t size);
    VX_API bool write_line_internal(const char* first, size_t size);

#if defined(VX_PLATFORM_WINDOWS)

    VX_API bool windows_write_text_file_internal(const char* text, size_t size);

#endif // VX_PLATFORM_WINDOWS

public:

    ///////////////////////////////////////////////////////////////////////////////
    // generic read functions
    ///////////////////////////////////////////////////////////////////////////////

    template <typename T>
    size_t read(T& data)
    {
        return read_internal(reinterpret_cast<uint8_t*>(&data), sizeof(T));
    }

    template <typename T>
    size_t read(T* data, size_t count)
    {
        return read_internal(reinterpret_cast<uint8_t*>(data), sizeof(T) * count);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // generic write functions
    ///////////////////////////////////////////////////////////////////////////////

    template <typename T>
    size_t write(const T& data)
    {
        return write_internal(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
    }

    template <typename T>
    size_t write(const T* data, size_t count)
    {
        return write_internal(reinterpret_cast<const uint8_t*>(data), sizeof(T) * count);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // text read functions
    ///////////////////////////////////////////////////////////////////////////////

    VX_API bool read_line(std::string& line);

    ///////////////////////////////////////////////////////////////////////////////
    // text write functions
    ///////////////////////////////////////////////////////////////////////////////

    size_t write(const char* text)
    {
        return write_internal(reinterpret_cast<const uint8_t*>(text), std::strlen(text));
    }

    size_t write(const std::string& text)
    {
        return write_internal(reinterpret_cast<const uint8_t*>(text.data()), text.size());
    }

    bool write_line(const char* line) { return write_check() && write_line_internal(line, std::strlen(line)); }
    bool write_line(const std::string& line) { return write_check() && write_line_internal(line.c_str(), line.size()); }

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
        return f.read_internal(data.data(), data.size());
    }

    static bool read_file(const path& p, std::string& text)
    {
        file f;
        if (!f.open(p, mode::READ))
        {
            return false;
        }

        text.resize(f.size());
        return f.read_internal(reinterpret_cast<uint8_t*>(text.data()), text.size());
    }

    ///////////////////////////////////////////////////////////////////////////////
    // file write functions
    ///////////////////////////////////////////////////////////////////////////////

    static bool write_file(const path& p, const uint8_t* data, size_t size)
    {
        file f;
        return f.open(p, mode::WRITE) && f.write_internal(data, size);
    }

    static bool write_file(const path& p, const char* text)
    {
        file f;
        return f.open(p, mode::WRITE) &&

#if defined(VX_PLATFORM_WINDOWS)

            f.windows_write_text_file_internal(text, std::strlen(text));

#else

            f.write_internal(reinterpret_cast<const uint8_t*>(text), std::strlen(text));

#endif // VX_PLATFORM_WINDOWS
    }

    static bool write_file(const path& p, const std::string& text)
    {
        return write_file(p, text.c_str());
    }

    static bool clear_file(const path& p)
    {
        file f;
        return f.open(p, mode::READ_WRITE_EXISTS) && f.clear();
    }

private:

    using file_impl = __detail::file_impl;
    friend file_impl;

    mode m_mode;

    using impl_data = __detail::file_impl_data;
    impl_data m_impl_data;
};

} // namespace os
} // namespace vx
