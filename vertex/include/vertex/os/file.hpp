#pragma once

#include <vector>
#include <sstream>

#include "vertex/system/error.hpp"

namespace vx {
namespace os {

class process;

// https://man7.org/linux/man-pages/man3/fopen.3.html

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

    file();
    ~file();

    file(const file&) = delete;
    file(file&&) noexcept;

    file& operator=(const file&) = delete;
    file& operator=(file&&) noexcept;

    friend void swap(file& lhs, file& rhs) noexcept;

public:

    static bool exists(const std::string& path);
    static bool create(const std::string& path);

    static bool read_file(const std::string& path, std::vector<uint8_t>& data);
    static bool write_file(const std::string& path, const uint8_t* data, size_t size);

    static bool read_text_file(const std::string& path, std::string& text);
    static bool write_text_file(const std::string& path, const std::string& text);

public:

    bool open(const std::string& path, mode mode);
    bool is_open() const;
    void close();

    mode get_mode() const;
    bool can_read() const;
    bool can_write() const;

    size_t size() const;

    bool seek(size_t off, stream_position from = stream_position::BEGIN);
    size_t tell() const;
    bool eof() const;

    size_t read(uint8_t* data, size_t size);
    size_t write(const uint8_t* data, size_t size);

    size_t read_text(std::string& text, size_t count);
    size_t write_text(const std::string& text);

    bool flush();

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

    class ostream_proxy;

    template <typename T>
    ostream_proxy operator<<(const T& value)
    {
        ostream_proxy stream(*this);
        stream << value;
        return stream;
    }

    ostream_proxy operator<<(std::ostream& (*func)(std::ostream&));

private:

    mode m_mode;

    class file_impl;
    std::unique_ptr<file_impl> m_impl;
};

class file::ostream_proxy
{
public:

    ostream_proxy(file& f) : m_file(f) {}
    ~ostream_proxy() { flush(); }

    ostream_proxy(const ostream_proxy&) = delete;
    ostream_proxy(ostream_proxy&&) noexcept = default;

    ostream_proxy& operator=(const ostream_proxy&) = delete;
    ostream_proxy& operator=(ostream_proxy&&) noexcept = default;

    template <typename T>
    ostream_proxy& operator<<(const T& value)
    {
        m_oss << value;
        return *this;
    }

    ostream_proxy& operator<<(std::ostream& (*func)(std::ostream&))
    {
        m_oss << func;
        return *this;
    }

    void flush()
    {
        if (m_oss.tellp())
        {
            m_file.write_text(m_oss.str());
            m_oss.str("");
            m_oss.clear();
        }
    }

private:

    file& m_file;
    std::ostringstream m_oss;
};

} // namespace os
} // namespace vx
