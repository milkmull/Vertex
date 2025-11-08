#pragma once

#include <cstdint>
#include <memory>

#include "vertex/config/language_config.hpp"
#include "vertex/config/assert.hpp"
#include "vertex/image/defs.hpp"
#include "vertex/os/file.hpp"
#include "vertex_impl/image/util.hpp"

namespace vx {
namespace img {

///////////////////////////////////////////////////////////////////////////////
// errors
///////////////////////////////////////////////////////////////////////////////

enum class load_error
{
    none = 0,

    bad_file,
    max_size,
    out_of_memory,

    corrupt_bmp,
    corrupt_jpeg,
    corrupt_png,

    unsupported_pixel_format
};

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
    size_t bytes_read;

    size_t total_bytes_read() const
    {
        return bytes_read + (ptr - buffer);
    }

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
        bytes_read = 0;

        return true;
    }

    void refill_buffer()
    {
        const size_t n = file.read(buffer, sizeof(buffer));

        if (n > 0)
        {
            ptr = buffer;
            buffer_end = ptr + n;
            bytes_read += (ptr - buffer);
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

    bool read_n(uint8_t* dst, size_t n)
    {
        size_t remaining = n;

        while (remaining > 0)
        {
            if (ptr == buffer_end)
            {
                refill_buffer();
                if (eof && (buffer_end - ptr) < remaining)
                {
                    return false; // not enough data to fulfill the request
                }
            }

            const size_t available = static_cast<size_t>(buffer_end - ptr);
            const size_t to_copy = (available < remaining) ? available : remaining;

            std::memcpy(dst, ptr, to_copy);
            dst += to_copy;
            ptr += to_copy;
            remaining -= to_copy;
        }

        return true;
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