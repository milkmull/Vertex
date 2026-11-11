#pragma once

#include "vertex/config/flags.hpp"
#include "vertex/config/os.hpp"
#include "vertex/os/handle.hpp"
#include "vertex/os/native_string.hpp"
#include "vertex/os/stream.hpp"
#include "vertex/std/error/error_code.hpp"

namespace vx {
namespace os {

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

//=========================================================================
// streams
//=========================================================================

VX_API success open(native_handle, const native_cstring p, typename rw_mode::enum_type mode);

inline void close(native_handle h) noexcept
{
    close_handle(h);
}

VX_API rw_result read(native_handle h, uint8_t* data, size_t size);
VX_API rw_result write(native_handle h, const uint8_t* data, size_t size);

VX_API success flush(native_handle h);
VX_API success truncate(native_handle h, uint64_t size);

using file_position = int64_t;
using file_offset = int64_t;

VX_API success tell(const native_handle h, file_position& pos);
VX_API success seek(native_handle h, file_offset off, stream_position from);

VX_API success file_size(native_handle h, uint64_t& size);
VX_API success set_file_size(native_handle h, uint64_t size);

//=========================================================================
// standard stream helpers
//=========================================================================

inline rw_result read_in(uint8_t* data, size_t size)
{
    return read(get_stream_handle(stream::in), data, size);
}

inline rw_result write_out(const uint8_t* data, size_t size)
{
    return write(get_stream_handle(stream::out), data, size);
}

inline rw_result write_err(const uint8_t* data, size_t size)
{
    return write(get_stream_handle(stream::err), data, size);
}

} // namespace os
} // namespace vx
