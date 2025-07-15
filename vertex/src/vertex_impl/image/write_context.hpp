#pragma once

#include <cstdint>
#include <memory>
#include <stdarg.h>

#include "vertex/config/language_config.hpp"
#include "vertex/config/assert.hpp"
#include "vertex/image/defs.hpp"
#include "vertex/os/file.hpp"

namespace vx {
namespace img {

///////////////////////////////////////////////////////////////////////////////
// errors
///////////////////////////////////////////////////////////////////////////////

enum class write_error
{
    NONE = 0,

    BAD_FILE,
    MAX_SIZE
};

///////////////////////////////////////////////////////////////////////////////
// write context
///////////////////////////////////////////////////////////////////////////////

struct write_context
{
    ///////////////////////////////////////////////////////////////////////////////
    // error info
    ///////////////////////////////////////////////////////////////////////////////

    write_error err = write_error::NONE;
    const char* msg = nullptr;

    ///////////////////////////////////////////////////////////////////////////////
    // image info
    ///////////////////////////////////////////////////////////////////////////////

    const uint8_t* data;
    size_t w, h;
    size_t channels;

    VX_FORCE_INLINE size_t data_size() const noexcept
    {
        return w * h * channels;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // file
    ///////////////////////////////////////////////////////////////////////////////

    os::file file;

    bool open_file(const char* filename)
    {
        if (!file.open(filename, os::file::mode::WRITE))
        {
            err = write_error::BAD_FILE;
            return false;
        }

        return true;
    }

    void close_file()
    {
        file.close();
    }

    VX_FORCE_INLINE void write(const uint8_t* d, size_t size)
    {
        file.write(d, size);
    }

    VX_FORCE_INLINE void write_byte(const uint8_t b)
    {
        write(&b, 1);
    }
};

} // namespace img
} // namespace vx