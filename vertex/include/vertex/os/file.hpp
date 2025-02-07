#pragma once

#include <vector>
#include <sstream>

#include "vertex/system/compiler.hpp"
#include "vertex/os/path.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// ostream
///////////////////////////////////////////////////////////////////////////////

namespace __detail {

template <typename file_t>
class ostream_proxy
{
public:

    ostream_proxy(file_t& f) : m_file(f) {}
    ~ostream_proxy() { flush(); }

    ostream_proxy(const ostream_proxy&) = delete;
    ostream_proxy(ostream_proxy&&) noexcept = default;

    ostream_proxy& operator=(const ostream_proxy&) = delete;
    ostream_proxy& operator=(ostream_proxy&&) noexcept = default;

public:

    template <typename T>
    ostream_proxy& operator<<(const T& value)
    {
        m_stream << value;
        return *this;
    }

    ostream_proxy& operator<<(std::ostream& (*func)(std::ostream&))
    {
        m_stream << func;
        return *this;
    }

    void flush()
    {
        if (m_stream.tellp())
        {
            m_file.write_text(m_stream.str());
            m_stream.str("");
            m_stream.clear();
        }
    }

private:

    file_t& m_file;
    std::ostringstream m_stream;
};

} // namespace __detail

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
        READ,
        WRITE,
        APPEND,
        READ_WRITE_EXISTS,
        READ_WRITE_CREATE
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

    VX_API static bool read_file(const path& p, std::vector<uint8_t>& data);
    VX_API static bool write_file(const path& p, const uint8_t* data, size_t size);

    template <typename IT>
    static inline bool write_file(const path& p, IT first, IT last)
    {
        const std::vector<uint8_t> buf(first, last);
        return write_file(p, buf);
    }

    VX_API static bool read_text_file(const path& p, std::string& text);
    VX_API static bool write_text_file(const path& p, const std::string& text);

    template <typename IT>
    static inline bool write_text_file(const path& p, IT first, IT last)
    {
        const std::string buf(first, last);
        return write_text_file(p, buf);
    }

public:

    VX_API bool open(const path& p, mode mode);
    VX_API bool is_open() const;
    VX_API void close();

    VX_API mode get_mode() const noexcept;
    VX_API bool can_read() const noexcept;
    VX_API bool can_write() const noexcept;

    VX_API size_t size() const;

    VX_API bool seek(size_t off, stream_position from = stream_position::BEGIN);
    VX_API size_t tell() const;
    VX_API bool eof() const;

    VX_API size_t read(uint8_t* data, size_t size);
    VX_API size_t write(const uint8_t* data, size_t size);

    VX_API size_t read_text(std::string& text, size_t count);
    VX_API size_t write_text(const std::string& text);

    VX_API bool flush();

public:

    template <typename T>
    size_t read_to(T& value)
    {
        value = T{};
        return read(reinterpret_cast<uint8_t*>(&value), sizeof(T));
    }

    template <typename T>
    size_t write_from(const T& data)
    {
        return write(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
    }

public:

    template <typename T>
    auto operator<<(const T& value)
    {
        __detail::ostream_proxy<file> stream(*this);
        stream << value;
        return stream;
    }

    auto operator<<(std::ostringstream& (*func)(std::ostringstream&))
    {
        __detail::ostream_proxy<file> stream(*this);
        stream << func;
        return stream;
    }

private:

    mode m_mode;

    class file_impl;
    std::unique_ptr<file_impl> m_impl;
};

} // namespace os
} // namespace vx
