#pragma once

#include "vertex/util/time/time.hpp"
#include "vertex/util/time/datetime.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// Current Time
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Retrieves the current system time.
 *
 * @return The current system time.
 */
VX_API time::time_point system_time();

///////////////////////////////////////////////////////////////////////////////
// Ticks
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Retrieves the current time in terms of "ticks" (elapsed time since
 * the program started).
 *
 * This function provides a high-resolution time measurement, useful for
 * profiling or measuring time intervals. The returned value represents
 * the time elapsed since the program started, in nanoseconds.
 *
 * @return The current time elapsed since the program started, in nanoseconds.
 */
VX_API time::time_point get_ticks();

/**
 * @brief Retrieves the current value of a high-resolution performance counter.
 *
 * The performance counter is typically used for precise time measurements,
 * offering microsecond-level precision.
 *
 * @return The current performance counter value.
 */
VX_API int64_t get_performance_counter();

/**
 * @brief Retrieves the frequency of the high-resolution performance counter.
 *
 * The frequency represents the number of ticks per second for the performance
 * counter. It is used to convert the performance counter value to actual time
 * units (e.g., seconds).
 *
 * @return The performance counter frequency in ticks per second.
 */
VX_API int64_t get_performance_frequency();

///////////////////////////////////////////////////////////////////////////////
// Sleep
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Causes the current thread to sleep until the specified time point.
 *
 * This function pauses the execution of the current thread until the time
 * point `t` is reached.
 *
 * @param t The time point until which the thread should sleep.
 */
VX_API void sleep(const time::time_point& t);

} // namespace os
} // namespace vx