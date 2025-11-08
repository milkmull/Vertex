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

namespace _priv {

struct file_impl;

} // namespace _priv

///////////////////////////////////////////////////////////////////////////////

enum class stream_position
{
    begin,
    current,
    end
};

/**
 * @brief A class for handling file operations.
 *
 * The `file` class provides methods for reading, writing, opening, closing, and manipulating files.
 * It supports a variety of file modes, including reading, writing, and appending, and includes
 * functionality to query file sizes, seek within files, and flush data.
 */
class file
{
public:

    // https://man7.org/linux/man-pages/man3/fopen.3.html

    /**
     * @brief Enumerates the different file access modes.
     *
     * The modes determine how a file is opened (read-only, write-only, append, etc.).
     */
    enum class mode
    {
        none,
        read,               // file must exist
        write,              // always create or truncate the file
        append,             // create if not exists, otherwise open existing
        read_write_exists,  // file must exist
        read_write_create   // truncate if file exists, or create a new one
    };


    enum : size_t
    {
        invalid_size        = std::numeric_limits<size_t>::max(),
        invalid_position    = invalid_size
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

    /**
     * @brief Checks if a file exists.
     *
     * @param p The path to the file.
     * @return True if the file exists, false otherwise.
     */
    VX_API static bool exists(const path& p);

    /**
     * @brief Creates a file at the specified path if it doesn't exist. If the file already exists, it will be truncated to size 0.
     *
     * This function attempts to create the file by opening it in `WRITE` mode. If the file already exists, its content will be erased.
     * If the file doesn't exist, a new, empty file will be created.
     *
     * @param p The path of the file to be created.
     * @return `true` if the file was successfully created or truncated, `false` otherwise.
     *
     * @note The file is opened in `WRITE` mode, which means it will always truncate the file to size 0 if it already exists.
     * Ensure you are aware that existing content will be lost.
     */
    VX_API static bool create(const path& p);

public:

    /**
     * @brief Opens a file.
     *
     * Opens the specified file in the given mode.
     *
     * @param p The path to the file.
     * @param file_mode The mode in which to open the file.
     * @return True if the file was successfully opened, false otherwise.
     */
    VX_API bool open(const path& p, mode file_mode);

    /**
     * @brief Checks if the file is currently open.
     *
     * @return True if the file is open, false otherwise.
     */
    VX_API bool is_open() const;

    /**
     * @brief Closes the file.
     *
     * Closes the file if it is open.
     */
    VX_API void close();

    /**
     * @brief Gets the current file mode.
     *
     * @return The mode in which the file was opened.
     */
    mode get_mode() const noexcept { return m_mode; }

    /**
     * @brief Checks if the file is open in a mode that allows reading.
     *
     * @return True if the file is open for reading, false otherwise.
     */
    bool can_read() const noexcept { return (m_mode == mode::read || m_mode == mode::read_write_exists || m_mode == mode::read_write_create); }

    /**
     * @brief Checks if the file is open in a mode that allows writing.
     *
     * @return True if the file is open for writing, false otherwise.
     */
    bool can_write() const noexcept { return (m_mode == mode::write || m_mode == mode::read_write_exists || m_mode == mode::read_write_create || m_mode == mode::append); }

    /**
     * @brief Gets the size of the file.
     *
     * @return The size of the file in bytes.
     */
    VX_API size_t size() const;

    /**
     * @brief Resizes the file.
     *
     * Resizes the file to the given size. If the file is larger than the specified size,
     * data will be truncated. If smaller, it will be padded with null bytes.
     *
     * @param size The new size of the file in bytes.
     * @return True if the file was resized successfully, false otherwise.
     */
    VX_API bool resize(size_t size);

    /**
     * @brief Clears the file.
     *
     * Resizes the file to zero bytes, effectively clearing its contents.
     *
     * @return True if the file was successfully cleared, false otherwise.
     */
    bool clear() { return resize(0); }

    /**
     * @brief Seeks to a given position in the file.
     *
     * Seeks to the specified offset from a given stream position.
     *
     * @param off The offset to seek to.
     * @param from The position from which to seek (e.g., begin, current, end).
     * @return True if the seek was successful, false otherwise.
     */
    VX_API bool seek(int off, stream_position from = stream_position::begin);

    /**
     * @brief Gets the current file position.
     *
     * @return The current position within the file.
     */
    VX_API size_t tell() const;

    /**
     * @brief Checks if the end of the file has been reached.
     *
     * @return True if the file pointer is at the end of the file, false otherwise.
     */
    VX_API bool eof() const;

    /**
     * @brief Flushes the file buffer.
     *
     * Ensures that all buffered data is written to the file.
     *
     * @return True if the flush was successful, false otherwise.
     */
    VX_API bool flush();

public:

    ///////////////////////////////////////////////////////////////////////////////
    // generic read functions
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Reads data from the file.
     *
     * Reads the specified number of bytes into the provided buffer.
     *
     * @param data The buffer to read data into.
     * @param size The number of bytes to read.
     * @return The number of bytes actually read.
     */
    VX_API size_t read(uint8_t* data, size_t size);

    /**
     * @brief Template function to read a single object from the file.
     *
     * Reads an object of type T from the file.
     *
     * @tparam T The type of object to read.
     * @param data The object to read into.
     * @return The number of bytes read.
     */
    template <typename T>
    size_t read(T& data)
    {
        return read(reinterpret_cast<uint8_t*>(&data), sizeof(T));
    }

    /**
     * @brief Template function to read an array of objects from the file.
     *
     * Reads an array of objects of type T from the file.
     *
     * @tparam T The type of object to read.
     * @param data The array to read into.
     * @param count The number of objects to read.
     * @return The number of bytes read.
     */
    template <typename T>
    size_t read(T* data, size_t count)
    {
        return read(reinterpret_cast<uint8_t*>(data), sizeof(T) * count);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // generic write functions
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Writes data to the file.
     *
     * Writes the specified number of bytes from the provided buffer to the file.
     *
     * @param data The buffer containing the data to write.
     * @param size The number of bytes to write.
     * @return The number of bytes actually written.
     */
    VX_API size_t write(const uint8_t* data, size_t size);

    /**
     * @brief Template function to write a single object to the file.
     *
     * Writes an object of type T to the file.
     *
     * @tparam T The type of object to write.
     * @param data The object to write.
     * @return The number of bytes written.
     */
    template <typename T>
    size_t write(const T& data)
    {
        return write(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
    }

    /**
     * @brief Template function to write an array of objects to the file.
     *
     * Writes an array of objects of type T to the file.
     *
     * @tparam T The type of object to write.
     * @param data The array of objects to write.
     * @param count The number of objects to write.
     * @return The number of bytes written.
     */
    template <typename T>
    size_t write(const T* data, size_t count)
    {
        return write(reinterpret_cast<const uint8_t*>(data), sizeof(T) * count);
    }

    /**
     * @brief Writes a platform-specific line ending (either "\r\n" on Windows or "\n" on other platforms) to the file.
     *
     * This function writes a line ending to the file to properly handle platform-specific newline characters. On Windows, it writes
     * the carriage return and newline sequence (`\r\n`), while on other platforms (e.g., Linux, macOS), it writes just the newline (`\n`).
     *
     * @return `true` if the line ending was successfully written, `false` otherwise.
     *
     * @note The function writes only the line ending and does not add any other content. The newline behavior is adapted to the current platform.
     */
    bool endl()
    {
        return (write(VX_LINE_END) == sizeof(VX_LINE_END));
    }

    ///////////////////////////////////////////////////////////////////////////////
    // text write functions
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Writes a C-string to the file.
     *
     * @param text The C-string to write.
     * @return The number of bytes written.
     */
    size_t write(const char* text)
    {
        return write(reinterpret_cast<const uint8_t*>(text), std::strlen(text));
    }

    /**
     * @brief Writes a std::string to the file.
     *
     * @param text The string to write.
     * @return The number of bytes written.
     */
    size_t write(const std::string& text)
    {
        return write(reinterpret_cast<const uint8_t*>(text.data()), text.size());
    }

    ///////////////////////////////////////////////////////////////////////////////
    // read and write line functions
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Reads a single line from the file, handling platform-specific line endings.
     *
     * This function reads a line from the file and correctly handles different newline conventions across platforms. On Windows,
     * lines ending with `\r\n` are handled properly, while on other platforms, lines ending with `\n` are expected.
     *
     * @param line A reference to a string to store the read line.
     * @return `true` if a line was successfully read, `false` if an error occurred or end of file was reached.
     */
    VX_API bool read_line(std::string& line);

    /**
     * @brief Writes a single line to the file with the appropriate platform-specific line ending.
     *
     * This function writes a line to the file with a newline character at the end, ensuring that the correct line ending is used
     * based on the platform. On Windows, it writes `\r\n`, and on other platforms, it writes `\n`.
     *
     * @param line The line to be written to the file.
     * @return `true` if the line was successfully written, `false` otherwise.
     */
    VX_API bool write_line(const char* line);

    /**
     * @brief Writes a single line to the file with the appropriate platform-specific line ending.
     *
     * This overload of `write_line` writes a `std::string` to the file with the appropriate platform-specific line ending.
     * On Windows, it writes `\r\n`, and on other platforms, it writes `\n`.
     *
     * @param line The `std::string` to be written to the file.
     * @return `true` if the line was successfully written, `false` otherwise.
     */
    bool write_line(const std::string& line)
    {
        return write_line(line.c_str());
    }

public:

    ///////////////////////////////////////////////////////////////////////////////
    // file read functions
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Reads the entire contents of a file into a buffer.
     *
     * @param p The path to the file.
     * @param data The buffer to store the file contents.
     * @return True if the file was successfully read, false otherwise.
     */
    static bool read_file(const path& p, std::vector<uint8_t>& data)
    {
        file f;
        if (!f.open(p, mode::read))
        {
            return false;
        }

        data.resize(f.size());
        return f.read(data.data(), data.size());
    }

    /**
     * @brief Reads the entire contents of a file into a string.
     *
     * @param p The path to the file.
     * @param text The string to store the file contents.
     * @return True if the file was successfully read, false otherwise.
     */
    static bool read_file(const path& p, std::string& text)
    {
        file f;
        if (!f.open(p, mode::read))
        {
            return false;
        }

        text.resize(f.size());
        return f.read(reinterpret_cast<uint8_t*>(text.data()), text.size());
    }

    ///////////////////////////////////////////////////////////////////////////////
    // file write functions
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Writes data to a file at the specified path.
     *
     * This function opens the file in `WRITE` mode, which means:
     * - If the file already exists, it will be truncated (i.e., all previous content is discarded).
     * - If the file does not exist, it will be created.
     *
     * @param p The path of the file to write to.
     * @param data Pointer to the data to be written to the file.
     * @param size The size of the data to write in bytes.
     * @return `true` if the file was successfully opened and written to, `false` otherwise.
     */
    static bool write_file(const path& p, const uint8_t* data, size_t size)
    {
        file f;
        return f.open(p, mode::write) && f.write(data, size);
    }

    /**
     * @brief Writes text to a file at the specified path.
     *
     * This function opens the file in `WRITE` mode, which means:
     * - If the file already exists, it will be truncated (i.e., all previous content is discarded).
     * - If the file does not exist, it will be created.
     *
     * The file is opened with write-only access, and the provided text is written to the file.
     * The text is written as-is, with no modifications or formatting applied.
     *
     * @param p The path of the file to write to.
     * @param text The text to write to the file.
     * @return `true` if the file was successfully opened and written to, `false` otherwise.
     */
    VX_API static bool write_file(const path& p, const char* text);

    /**
     * @brief Writes a string to a file at the specified path.
     *
     * This function opens the file in `WRITE` mode, which means:
     * - If the file already exists, it will be truncated (i.e., all previous content is discarded).
     * - If the file does not exist, it will be created.
     *
     * The file is opened with write-only access, and the provided string is written to the file.
     * The string is written as-is, with no modifications or formatting applied.
     *
     * @param p The path of the file to write to.
     * @param text The string to write to the file.
     * @return `true` if the file was successfully opened and written to, `false` otherwise.
     */
    static bool write_file(const path& p, const std::string& text)
    {
        return write_file(p, text.c_str());
    }

    /**
     * @brief Clears the content of a file at the specified path.
     *
     * This function opens the file in `READ_WRITE_EXISTS` mode, which means:
     * - The file must already exist. If the file does not exist, this operation will fail.
     * - It will clear the file's contents by truncating the file to zero size.
     *
     * This operation does not delete the file; it only clears its content.
     *
     * @param p The path of the file to clear.
     * @return `true` if the file was successfully opened and cleared, `false` otherwise.
     */
    static bool clear_file(const path& p)
    {
        file f;
        return f.open(p, mode::read_write_exists) && f.clear();
    }

private:

    static file from_native_handle(typename handle::native_handle h, mode m);
    typename handle::native_handle get_native_handle() const { return m_handle.get(); }

private:

    friend _priv::file_impl;

    mode m_mode = mode::none;
    handle m_handle;
};

} // namespace os
} // namespace vx
