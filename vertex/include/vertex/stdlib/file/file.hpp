#pragma once

#include <fstream>
#include <vector>

namespace vx {

enum class file_mode
{
    READ      = (1 << 0),
    WRITE     = (1 << 1),
    READWRITE = (READ | WRITE),
    APPEND    = (1 << 2)
};

class file
{
public:

    file() = default;
    ~file() { close(); }

    bool open(const std::string& path, file_mode mode)
    {
        std::ios_base::openmode om = std::ios::binary;

        switch (mode)
        {
            case file_mode::READ:
            {
                om |= std::ios::in;
                break;
            }
            case file_mode::WRITE:
            {
                om |= std::ios::out | std::ios::trunc;
                break;
            }
            case file_mode::READWRITE:
            {
                om |= std::ios::in | std::ios::out;
                break;
            }
            case file_mode::APPEND:
            {
                om |= std::ios::out | std::ios::app;
                break;
            }
        }

        m_file.open(path, om);
        return m_file.is_open();
    }

    bool is_open() const
    {
        return m_file.is_open();
    }

    void close()
    {
        if (m_file.is_open())
        {
            m_file.close();
        }
    }

    void seek(size_t position)
    {
        if (m_file.is_open())
        {
            // Ensure both get and put pointers are moved
            m_file.seekg(position);
            m_file.seekp(position);
        }
    }

    size_t tell()
    {
        return is_open() ? static_cast<size_t>(m_file.tellg()) : 0;
    }

    std::vector<uint8_t> read()
    {
        if (!is_open())
        {
            throw std::runtime_error("File not open for reading");
        }

        const size_t filesize = size();
        std::vector<uint8_t> buffer(filesize);
        m_file.read(reinterpret_cast<char*>(buffer.data()), filesize);

        return buffer;
    }

    size_t read(uint8_t* data, size_t size)
    {
        if (!is_open())
        {
            throw std::runtime_error("File not open for reading");
        }

        size_t count = tell();
        m_file.read(reinterpret_cast<char*>(data), size);
        count = tell() - count;

        return count;
    }

    template <typename T>
    size_t read_to(T& value)
    {
        if (!is_open())
        {
            throw std::runtime_error("File not open for reading");
        }

        size_t count = tell();
        m_file.read(reinterpret_cast<char*>(&value), sizeof(T));
        count = tell() - count;

        return count;
    }

    void write(const uint8_t* data, size_t size)
    {
        if (!is_open())
        {
            throw std::runtime_error("File not open for writing");
        }

        m_file.write(reinterpret_cast<const char*>(data), size);
    }

    template <typename T>
    void write_from(const T& data)
    {
        if (!is_open())
        {
            throw std::runtime_error("File not open for writing");
        }

        m_file.write(reinterpret_cast<const char*>(&data), sizeof(T));
    }

    size_t size()
    {
        if (!is_open())
        {
            throw std::runtime_error("File not open");
        }

        const std::streampos position = m_file.tellg();
        m_file.seekg(0, std::ios::end);
        const size_t size = m_file.tellg();
        m_file.seekg(position);

        return size;
    }

private:

    std::fstream m_file;
};

} // namespace vx
