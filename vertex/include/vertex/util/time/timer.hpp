#pragma once

#include "vertex/os/time.hpp"

namespace vx {
namespace time {

///////////////////////////////////////////////////////////////////////////////
// Timer
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief A timer class for measuring elapsed time.
 *
 * This class provides functionality for starting, stopping, resetting, and querying
 * the elapsed time between a start and stop event. It internally tracks the start and
 * stop time points, and can also determine if the timer is currently running.
 */
class timer
{
public:

    timer() noexcept = default;
    ~timer() noexcept = default;

    /**
     * @brief Starts the timer.
     *
     * Starts the timer and records the start time. If the timer was previously stopped,
     * it will be reset. Returns the start time of the timer.
     *
     * @return The start time point of the timer.
     */
    inline time_point start() noexcept
    {
        m_stop_time.zero();
        m_running = true;
        m_start_time = os::get_ticks();
        return m_start_time;
    }

    /**
     * @brief Stops the timer.
     *
     * Stops the timer if it is currently running and records the stop time.
     * Returns the stop time point.
     *
     * @return The stop time point of the timer.
     */
    inline time_point stop() noexcept
    {
        if (m_running)
        {
            m_running = false;
            m_stop_time = os::get_ticks();
        }
        return m_stop_time;
    }

    /**
     * @brief Resets the timer.
     *
     * Resets the timer by clearing the start and stop time points and marking the timer as stopped.
     */
    inline void reset() noexcept
    {
        m_running = false;
        m_start_time.zero();
        m_stop_time.zero();
    }

    /**
     * @brief Checks if the timer is currently running.
     *
     * @return True if the timer is running, false otherwise.
     */
    inline bool running() const noexcept
    {
        return m_running;
    }

    /**
     * @brief Gets the start time of the timer.
     *
     * @return The start time point of the timer.
     */
    inline time_point start_time() const noexcept
    {
        return m_start_time;
    }

    /**
     * @brief Gets the elapsed time.
     *
     * Returns the time elapsed since the timer was started. If the timer is running,
     * it will return the difference between the current time and the start time.
     * If the timer has been stopped, it will return the difference between the start
     * and stop time points.
     *
     * @return The elapsed time since the timer started.
     */
    inline time_point elapsed_time() const noexcept
    {
        if (m_running)
        {
            return os::get_ticks() - m_start_time;
        }
        return m_stop_time - m_start_time;
    }

private:

    bool m_running = false;
    time_point m_start_time;
    time_point m_stop_time;
};

} // namespace time
} // namespace vx