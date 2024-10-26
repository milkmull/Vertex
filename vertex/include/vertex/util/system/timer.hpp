#pragma once

#include "vertex/os/time.hpp"

namespace vx {

///////////////////////////////////////////////////////////////////////////////
// Timer
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief A high-resolution timer for measuring elapsed time.
///
/// The 'timer' class provides functionality to measure elapsed time using
/// high-resolution ticks. It supports starting, stopping, and resetting the
/// timer, as well as querying the current state and elapsed time.
///
/// @note The timer measures time in nanoseconds.
///////////////////////////////////////////////////////////////////////////////
class timer
{
public:

    timer() = default;
    ~timer() = default;

    inline uint64_t start()
    {
        m_running = true;
        return (m_start_time = os::time::get_ticks_ns());
    }

    inline uint64_t stop()
    {
        m_running = false;
        return (m_stop_time = os::time::get_ticks_ns());
    }

    inline void reset()
    {
        m_running = false;
        m_start_time = m_stop_time = 0;
    }

    inline bool running() const
    {
        return m_running;
    }
    inline uint64_t start_time() const
    {
        return m_start_time;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// @brief Retrieves the elapsed time since the timer was started.
    ///
    /// This method calculates the elapsed time in nanoseconds. If the timer is
    /// running, it returns the time elapsed since the last start. If the timer
    /// is stopped, it returns the time between the start and stop times.
    ///
    /// @return The elapsed time in nanoseconds.
    ///////////////////////////////////////////////////////////////////////////
    inline uint64_t elapsed() const
    {
        if (m_running)
        {
            return os::time::get_ticks_ns() - m_start_time;
        }
        return m_stop_time - m_start_time;
    }

private:

    bool m_running;
    uint64_t m_start_time;
    uint64_t m_stop_time;

};

} // namespace vx