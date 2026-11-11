#pragma once

#include "vertex/config/flags.hpp"
#include "vertex/config/os.hpp"
#include "vertex/os/handle.hpp"
#include "vertex/os/path.hpp"
#include "vertex/std/expected.hpp"

namespace vx {
namespace os {

enum class stream
{
    in,
    out,
    err
};

enum class stream_position
{
    begin,
    current,
    end
};

// https://man7.org/linux/man-pages/man3/fopen.3.html

VX_FLAGS_DECLARE_BEGIN(rw_mode){
    none = 0,

    read = VX_BIT(0),              // file must exist
    write = VX_BIT(1),             // always create or truncate the file
    append = VX_BIT(2),            // create if not exists, otherwise open existing
    read_write_exists = VX_BIT(3), // file must exist
    read_write_create = VX_BIT(4), // truncate if file exists, or create a new one

    _can_read = read | read_write_exists | read_write_create,
    _can_write = write | read_write_exists | read_write_create | append,
    _can_read_write = _can_read | _can_write

} VX_FLAGS_DECLARE_END(rw_mode);

struct rw_result
{
    size_t bytes; // bytes successfully transferred
    error err;    // error terminating the operation, or none
};

//=========================================================================
// stream ops
//=========================================================================

VX_API rw_result read_stream(stream s, char* data, size_t size);

namespace _os_io_priv {

// used by error system to prevent infinite recursion when printing errors
VX_API rw_result write_stream_impl(stream s, const char* data, size_t size, bool allow_error);

} // namespace _os_io_priv

inline rw_result write_stream(stream s, const char* data, size_t size)
{
    return _os_io_priv::write_stream_impl(s, data, size, false);
}

//=========================================================================

VX_API bool file_exists(const path& p);

//=========================================================================
// file ops
//=========================================================================

VX_API success open(handle& h, const path& p, typename rw_mode::enum_type mode);

inline void close(handle& h) noexcept
{
    h.close();
}

VX_API rw_result read(handle& h, uint8_t* data, size_t size);
VX_API rw_result write(handle& h, const uint8_t* data, size_t size);
VX_API success flush(handle& h);

using file_position = int64_t;
using file_offset = int64_t;

VX_API expected<file_position, error> tell(const handle& h);
VX_API success seek(handle& h, file_offset off, stream_position from);

VX_API expected<size_t, error> file_size(const handle& h);

} // namespace os
} // namespace vx
