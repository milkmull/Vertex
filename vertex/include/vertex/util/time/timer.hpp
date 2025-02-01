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

    timer() noexcept = default;
    ~timer() noexcept = default;

    inline time_point start() noexcept
    {
        m_stop_time.zero();
        m_running = true;
        m_start_time = os::get_ticks();
        return m_start_time;
    }

    inline time_point stop() noexcept
    {
        if (m_running)
        {
            m_running = false;
            m_stop_time = os::get_ticks();
        }
        return m_stop_time;
    }

    inline void reset() noexcept
    {
        m_running = false;
        m_start_time.zero();
        m_stop_time.zero();
    }

    inline bool running() const noexcept
    {
        return m_running;
    }

    inline time_point start_time() const noexcept
    {
        return m_start_time;
    }

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