#pragma once

#include "vertex/os/time.hpp"

namespace vx {
namespace time {

///////////////////////////////////////////////////////////////////////////////
// Timer
///////////////////////////////////////////////////////////////////////////////

class timer
{
public:

    timer() = default;
    ~timer() = default;

    inline time_point start()
    {
        m_running = true;
        return (m_start_time = os::get_ticks());
    }

    inline time_point stop()
    {
        m_running = false;
        return (m_stop_time = os::get_ticks());
    }

    inline void reset()
    {
        m_running = false;
        m_start_time.zero();
        m_stop_time.zero();
    }

    inline bool running() const
    {
        return m_running;
    }
    inline time_point start_time() const
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
    inline time_point elapsed_time() const
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