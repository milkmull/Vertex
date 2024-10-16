#pragma once

#include <vector>

#include "vertex/system/error.hpp"

namespace vx {

// https://man7.org/linux/man-pages/man3/fopen.3.html

enum class file_mode
{
    READ,
    WRITE,
    APPEND,
    READ_WRITE_EXISTS,
    READ_WRITE_CREATE
};

enum class stream_position
{
    BEGIN,
    CURRENT,
    END
};

class file
{
public:

    enum : size_t { INVALID_SIZE = -1, INVALID_POSITION = -1 };

    file();
    ~file();

    file(const file&) = delete;
    file(file&&) noexcept;

    file& operator=(const file&) = delete;
    file& operator=(file&&) noexcept;

public:

    static bool create(const std::string& path);

    static bool read_file(const std::string& path, std::vector<uint8_t>& data);
    static bool write_file(const std::string& path, const uint8_t* data, size_t size);

    static bool read_text_file(const std::string& path, std::string& text);
    static bool write_text_file(const std::string& path, const std::string& text);

    bool open(const std::string& path, file_mode mode);
    bool is_open() const;
    void close();

    size_t size() const;

    bool seek(size_t off, stream_position from = stream_position::BEGIN);
    size_t tell() const;
    bool eof() const;

    size_t read(uint8_t* data, size_t size);
    size_t write(const uint8_t* data, size_t size);

    size_t read_text(std::string& text, size_t count);
    size_t write_text(const std::string& text);

    bool flush();

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

private:

    file_mode m_mode;

    class file_impl;
    std::unique_ptr<file_impl> m_impl;
};

} // namespace vx
