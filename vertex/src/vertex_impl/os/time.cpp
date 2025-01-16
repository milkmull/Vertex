#include "vertex/os/time.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// Ticks
///////////////////////////////////////////////////////////////////////////////

struct ticks_data
{
    int64_t start_ticks;
    int64_t ticks_per_second;

    ticks_data()
    {
        start_ticks = get_performance_counter();
        ticks_per_second = get_performance_frequency();
    }
};

static ticks_data s_ticks_data{};

VX_API time::time_point get_ticks()
{
    const int64_t elapsed_ticks = get_performance_counter() - s_ticks_data.start_ticks;
    const int64_t ns = (elapsed_ticks * std::nano::den) / s_ticks_data.ticks_per_second;
    return time::nanoseconds(ns);
}

} // namespace os
} // namespace vx