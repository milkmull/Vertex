#pragma once

#include "vertex_impl/os/_platform/windows/windows_header.hpp"
#include "vertex/os/handle.hpp"
#include "vertex/util/time/time.hpp"

namespace vx {
namespace os {
namespace windows {

///////////////////////////////////////////////////////////////////////////////
// Error Handling
///////////////////////////////////////////////////////////////////////////////

void error_message(const char* msg);

///////////////////////////////////////////////////////////////////////////////
// time
///////////////////////////////////////////////////////////////////////////////

// Epoch difference between 1601-01-01 and 1970-01-01 in 100 nanosecond units
#define DELTA_EPOCH_1601_100NS (11644473600ull * 10000000ull)

inline constexpr time::time_point time_point_from_file_time(DWORD low, DWORD high) noexcept
{
    // Convert FILETIME (100-nanosecond intervals since 1601-01-01) to a 64-bit integer
    uint64_t wtime = (static_cast<uint64_t>(high) << 32) | low;
    // Adjust for Unix epoch and convert to nanoseconds (100-nanosecond intervals to nanoseconds)
    uint64_t ticks = (wtime - DELTA_EPOCH_1601_100NS) * 100;
    // Already in nanoseconds
    return time::time_point{ static_cast<int64_t>(ticks) };
}

inline constexpr void time_point_to_file_time(time::time_point t, DWORD& low, DWORD& high) noexcept
{
    // Convert time_point from nanoseconds since Unix epoch to 100-nanosecond intervals since 1601-01-01
    uint64_t wtime = static_cast<uint64_t>((t.as_nanoseconds() / 100) + DELTA_EPOCH_1601_100NS);

    // Split the 64-bit ticks into two 32-bit parts for FILETIME
    low = static_cast<DWORD>(wtime);
    high = static_cast<DWORD>(wtime >> 32);
}

#   undef DELTA_EPOCH_1601_100NS

///////////////////////////////////////////////////////////////////////////////
// COM
///////////////////////////////////////////////////////////////////////////////

class com_scoped_initializer
{
public:

    com_scoped_initializer() noexcept;
    ~com_scoped_initializer() noexcept;

    com_scoped_initializer(const com_scoped_initializer&) = delete;
    com_scoped_initializer& operator=(const com_scoped_initializer&) = delete;

    bool succeeded() const noexcept;

private:

    HRESULT m_hr;
};

} // namespace windows
} // namespace os
} // namespace vx