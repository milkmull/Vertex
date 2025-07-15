#pragma once

#include <cstdint>
#include <memory>

#include "vertex/config/language_config.hpp"
#include "vertex/config/assert.hpp"
#include "vertex/image/defs.hpp"
#include "vertex/os/file.hpp"

namespace vx {
namespace img {

///////////////////////////////////////////////////////////////////////////////
// errors
///////////////////////////////////////////////////////////////////////////////

enum class load_error
{
    NONE = 0,

    BAD_FILE,
    BAD_BMP,
    MAX_SIZE
};

///////////////////////////////////////////////////////////////////////////////
// overflow helpers
///////////////////////////////////////////////////////////////////////////////

VX_FORCE_INLINE bool will_multiply_overflow(size_t a, size_t b) noexcept
{
    if (a == 0 || b == 0) return false;
    return a > std::numeric_limits<size_t>::max() / b;
}

VX_FORCE_INLINE bool will_multiply_overflow(size_t a, size_t b, size_t c) noexcept
{
    // Check a * b first
    if (will_multiply_overflow(a, b)) return true;

    // If a * b is safe, calculate it
    const size_t ab = a * b;

    // Now check (a * b) * c
    return will_multiply_overflow(ab, c);
}

///////////////////////////////////////////////////////////////////////////////
// load context
///////////////////////////////////////////////////////////////////////////////

struct load_context
{
    ///////////////////////////////////////////////////////////////////////////////
    // error info
    ///////////////////////////////////////////////////////////////////////////////

    load_error err = load_error::NONE;
    const char* msg = nullptr;

    ///////////////////////////////////////////////////////////////////////////////
    // image info
    ///////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<uint8_t[]> out;
    size_t w, h;
    size_t channels;

    VX_FORCE_INLINE size_t output_size() const noexcept
    {
        return w * h * channels;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // file
    ///////////////////////////////////////////////////////////////////////////////

    os::file file;
    bool eof = false;

    uint8_t buffer[128];
    uint8_t* ptr = nullptr;
    uint8_t* buffer_end = nullptr;

    bool open_file(const char* filename)
    {
        if (!file.open(filename, os::file::mode::READ))
        {
            err = load_error::BAD_FILE;
            return false;
        }

        buffer_end = buffer + sizeof(buffer);
        ptr = buffer_end;
        eof = false;

        return true;
    }

    void refill_buffer()
    {
        const size_t n = file.read(buffer, sizeof(buffer));

        if (n > 0)
        {
            ptr = buffer;
            buffer_end = ptr + n;
        }
        else
        {
            // end of file
            ptr = buffer;
            buffer_end = ptr + 1;
            buffer[0] = 0;
            eof = true;
        }
    }

    void skip(size_t n)
    {
        const size_t remaining_bytes = buffer_end - ptr;

        if (n > remaining_bytes)
        {
            // skipping past our current buffer
            // compute extra bytes and seek ahead
            const int nseek = static_cast<int>(n - remaining_bytes);
            file.seek(nseek, os::stream_position::CURRENT);
            ptr = buffer_end; // signals that buffer needs to be refilled
        }
        else
        {
            ptr += n;
        }
    }

    void rewind(size_t n)
    {
        const size_t read_bytes = ptr - buffer;

        if (n > read_bytes)
        {
            // We need to seek backwards in the file
            const int nseek = -static_cast<int>(n - read_bytes);
            file.seek(nseek, os::stream_position::CURRENT);
            ptr = buffer_end; // signals that buffer needs to be refilled
        }
        else
        {
            ptr -= n;
        }
    }

    void rewind_buffer()
    {
        ptr = buffer;
    }

    VX_FORCE_INLINE uint8_t read_8() noexcept
    {
        if (ptr == buffer_end)
        {
            refill_buffer();
        }

        if (eof)
        {
            return 0;
        }

        return *ptr++;
    }

    // Read 16-bit little-endian
    VX_FORCE_INLINE uint16_t read_16_le() noexcept
    {
        return (
            (static_cast<uint16_t>(read_8()) << 0) |
            (static_cast<uint16_t>(read_8()) << 8)
            );
    }

    // Read 16-bit big-endian
    VX_FORCE_INLINE uint16_t read_16_be() noexcept
    {
        return (
            (static_cast<uint16_t>(read_8()) << 8) |
            (static_cast<uint16_t>(read_8()) << 0)
            );
    }

    // Read 32-bit little-endian
    VX_FORCE_INLINE uint32_t read_32_le() noexcept
    {
        return (
            (static_cast<uint32_t>(read_8()) << 0) |
            (static_cast<uint32_t>(read_8()) << 8) |
            (static_cast<uint32_t>(read_8()) << 16) |
            (static_cast<uint32_t>(read_8()) << 24)
            );
    }

    // Read 32-bit big-endian
    VX_FORCE_INLINE uint32_t read_32_be() noexcept
    {
        return (
            (static_cast<uint32_t>(read_8()) << 24) |
            (static_cast<uint32_t>(read_8()) << 16) |
            (static_cast<uint32_t>(read_8()) << 8) |
            (static_cast<uint32_t>(read_8()) << 0)
            );
    }

};

} // namespace img
} // namespace vx