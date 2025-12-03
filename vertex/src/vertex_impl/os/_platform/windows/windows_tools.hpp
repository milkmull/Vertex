#pragma once

#include "vertex_impl/os/_platform/windows/windows_header.hpp"
#include "vertex/os/handle.hpp"
#include "vertex/util/time/time.hpp"

namespace vx {
namespace os {
namespace windows {

///////////////////////////////////////////////////////////////////////////////
// Alignment
///////////////////////////////////////////////////////////////////////////////

// See https://github.com/libsdl-org/SDL/pull/7607
// force_align_arg_pointer attribute requires gcc >= 4.2.x.
#if defined(__clang__)
#   define _HAVE_FORCE_ALIGN_ARG_POINTER
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 2))
#   define _HAVE_FORCE_ALIGN_ARG_POINTER
#endif

#if defined(__GNUC__) && defined(__i386__) && defined(_HAVE_FORCE_ALIGN_ARG_POINTER)
#   define VX_MINGW32_FORCE_ALIGN __attribute__((force_align_arg_pointer))
#else
#   define VX_MINGW32_FORCE_ALIGN
#endif

#undef _HAVE_FORCE_ALIGN_ARG_POINTER

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

#undef DELTA_EPOCH_1601_100NS

///////////////////////////////////////////////////////////////////////////////
// helpers
///////////////////////////////////////////////////////////////////////////////

const char* check_default_args(int* pargc, char*** pargv, void** pallocated);

///////////////////////////////////////////////////////////////////////////////
// Scoped Manager
///////////////////////////////////////////////////////////////////////////////

template <typename T>
class scoped_manager
{
public:

    scoped_manager() { m_initializer.initialize(); }
    ~scoped_manager() { m_initializer.uninitialize(); }

    scoped_manager(const scoped_manager&) = delete;
    scoped_manager& operator=(const scoped_manager&) = delete;

    bool succeeded() const { return m_initializer.succeeded(); }

private:

    T m_initializer;
};

///////////////////////////////////////////////////////////////////////////////
// COM
///////////////////////////////////////////////////////////////////////////////

class com_initializer
{
public:

    com_initializer() noexcept = default;
    ~com_initializer() noexcept = default;

    com_initializer(const com_initializer&) = delete;
    com_initializer& operator=(const com_initializer&) = delete;

    bool initialize() noexcept;
    bool succeeded() const noexcept { return SUCCEEDED(m_hr); }
    void uninitialize() noexcept;

private:

    HRESULT m_hr = E_FAIL;
};

using com_scoped_initializer = scoped_manager<com_initializer>;

///////////////////////////////////////////////////////////////////////////////
// OLE
///////////////////////////////////////////////////////////////////////////////

class ole_initializer
{
public:

    ole_initializer() noexcept = default;
    ~ole_initializer() noexcept = default;

    ole_initializer(const ole_initializer&) = delete;
    ole_initializer& operator=(const ole_initializer&) = delete;

    bool initialize() noexcept;
    bool succeeded() const noexcept { return SUCCEEDED(m_hr); }
    void uninitialize() noexcept;

private:

    HRESULT m_hr = E_FAIL;
};

using ole_scoped_initializer = scoped_manager<ole_initializer>;

} // namespace windows
} // namespace os
} // namespace vx