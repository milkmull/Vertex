#include <cstdint>

#include "vertex/config/assert.hpp"
#include "vertex/os/atomic.hpp"
#include "vertex_impl/os/_platform/platform_time.hpp"

namespace vx {

///////////////////////////////////////////////////////////////////////////////
// Time
///////////////////////////////////////////////////////////////////////////////

time::datetime time::time_point::to_datetime(bool local) const
{
    return os::time_point_to_datetime_impl(*this, local);
}

namespace os {

///////////////////////////////////////////////////////////////////////////////
// Current Time
///////////////////////////////////////////////////////////////////////////////

time::time_point system_time()
{
    return system_time_impl();
}

///////////////////////////////////////////////////////////////////////////////
// Ticks
///////////////////////////////////////////////////////////////////////////////

static inline uint32_t calculate_gcd(uint32_t a, uint32_t b)
{
    if (b == 0)
    {
        return a;
    }

    return calculate_gcd(b, (a % b));
}

struct ticks_data
{
    int64_t start_ticks;
    int64_t ticks_per_second;
    uint32_t num_ns;
    uint32_t den_ns;

    ticks_data()
    {
        start_ticks = get_performance_counter_impl();
        ticks_per_second = get_performance_frequency_impl();

        const uint32_t gcd = calculate_gcd(
            static_cast<uint32_t>(time::nanoseconds_per_second),
            static_cast<uint32_t>(ticks_per_second));

        num_ns = static_cast<uint32_t>(time::nanoseconds_per_second) / gcd;
        den_ns = static_cast<uint32_t>(ticks_per_second / gcd);
    }
};

time::time_point get_ticks()
{
    // initialize on first call
    static const ticks_data s_ticks_data{};

    const int64_t elapsed_ticks = get_performance_counter_impl() - s_ticks_data.start_ticks;

    int64_t value = (elapsed_ticks * s_ticks_data.num_ns);
    VX_ASSERT(value >= elapsed_ticks);
    value /= s_ticks_data.den_ns;

    return time::nanoseconds(value);
}

int64_t get_performance_counter()
{
    return get_performance_counter_impl();
}

int64_t get_performance_frequency()
{
    return get_performance_frequency_impl();
}

///////////////////////////////////////////////////////////////////////////////
// Sleep
///////////////////////////////////////////////////////////////////////////////

void sleep(const time::time_point& t)
{
    sleep_impl(t);
}

// https://github.com/libsdl-org/SDL/blob/main/src/timer/SDL_timer.c#L664

void sleep_precise(const time::time_point& t)
{
    time::time_point current = get_ticks();
    const time::time_point target = current + t;

    // Sleep for a short number of cycles when real sleeps are desired.
    // We'll use 1 ms, it's the minimum guaranteed to produce real sleeps across
    // all platforms.
    constexpr time::time_point short_sleep = time::milliseconds(1);

    // Sleep in small segments until we get near the target.
    // While doing this, record the largest amount the system actually slept,
    // since real sleep calls often overshoot their requested interval.
    time::time_point max_sleep = short_sleep;
    while (current + max_sleep < target)
    {
        // Sleep for a short time
        sleep(short_sleep);

        const time::time_point now = get_ticks();
        const time::time_point next_sleep = (now - current);

        // If the sleep call took longer than requested, remember that.
        // It tells us how much the OS tends to overshoot, which we can use later.
        if (next_sleep > max_sleep)
        {
            max_sleep = next_sleep;
        }

        current = now;
    }

    // At this point, we're close to the target time.
    // Attempt one more sleep that’s adjusted by the worst overshoot seen so far.
    // This helps reach the target without jumping past it due to sleep inaccuracy.
    if (current < target && (target - current) > (max_sleep - short_sleep))
    {
        const time::time_point delay = (target - current) - (max_sleep - short_sleep);
        sleep(delay);
        current = get_ticks();
    }

    // If we are still behind the target by more than one small interval,
    // perform additional short sleeps instead of spinning.
    // This avoids unnecessary CPU usage when the remaining gap is still large
    // compared to our minimal sleep duration.
    while (current + short_sleep < target)
    {
        sleep(short_sleep);
        current = get_ticks();
    }

    // Only a very small remainder should now be left.
    // Spin for the final portion to reach the target as precisely as possible.
    // This avoids the unpredictable overshoot that another sleep() would cause.
    while (current < target)
    {
        cpu_pause();
        current = get_ticks();
    }
}

} // namespace os
} // namespace vx
