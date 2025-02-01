#include "vertex/system/platform_config.hpp"
#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/_platform/windows/windows_time.hpp"
#else
#   include "vertex_impl/os/_platform/dummy/dummy_time.hpp"
#endif

namespace vx {

///////////////////////////////////////////////////////////////////////////////
// Time
///////////////////////////////////////////////////////////////////////////////

VX_API time::datetime time::time_point::to_datetime(bool local) const
{
    return os::time_point_to_datetime_impl(*this, local);
}

namespace os {

///////////////////////////////////////////////////////////////////////////////
// Current Time
///////////////////////////////////////////////////////////////////////////////

VX_API time::time_point system_time()
{
    return system_time_impl();
}

///////////////////////////////////////////////////////////////////////////////
// Ticks
///////////////////////////////////////////////////////////////////////////////

struct ticks_data
{
    int64_t start_ticks;
    int64_t ticks_per_second;

    ticks_data()
    {
        start_ticks = get_performance_counter_impl();
        ticks_per_second = get_performance_frequency_impl();
    }
};

static ticks_data s_ticks_data{};

VX_API time::time_point get_ticks()
{
    const int64_t elapsed_ticks = get_performance_counter_impl() - s_ticks_data.start_ticks;
    const int64_t ns = (elapsed_ticks * 1000000000ll) / s_ticks_data.ticks_per_second;
    return time::nanoseconds(ns);
}

VX_API int64_t get_performance_counter()
{
    return get_performance_counter_impl();
}

VX_API int64_t get_performance_frequency()
{
    return get_performance_frequency_impl();
}

///////////////////////////////////////////////////////////////////////////////
// Sleep
///////////////////////////////////////////////////////////////////////////////

VX_API void sleep(const time::time_point& t)
{
    sleep_impl(t);
}

} // namespace os

} // namespace vx