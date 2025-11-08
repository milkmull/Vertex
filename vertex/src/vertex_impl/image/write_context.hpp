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
    none = 0,
    bad_file,
    max_size
};

///////////////////////////////////////////////////////////////////////////////
// write context
///////////////////////////////////////////////////////////////////////////////

struct write_context
{
    ///////////////////////////////////////////////////////////////////////////////
    // error info
    ///////////////////////////////////////////////////////////////////////////////

    write_error err = write_error::none;
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
            err = write_error::bad_file;
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