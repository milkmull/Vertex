#pragma once

#include "vertex/os/time.hpp"

namespace vx {
namespace time {

//=============================================================================
// Timer
//=============================================================================

class timer
{
public:

    timer() noexcept = default;

    void start() noexcept
    {
        m_running = true;
        m_start_ticks = os::get_ticks();
    }

    void restart() noexcept
    {
        start();
    }

    void stop() noexcept
    {
        m_running = false;
        m_stop_ticks = os::get_ticks();
    }

    void reset() noexcept
    {
        VX_ASSERT(is_running());
        m_running = false;
        m_start_ticks.zero();
        m_stop_ticks.zero();
    }

    bool is_running() const noexcept
    {
        return m_running;
    }

    duration start_ticks() const noexcept
    {
        return m_start_ticks;
    }

    duration stop_ticks() const noexcept
    {
        return m_stop_ticks;
    }

    duration elapsed_stopped() const noexcept
    {
        VX_ASSERT(is_running());
        return os::get_ticks() - m_start_ticks;
    }

    duration elapsed_running() const noexcept
    {
        VX_ASSERT(!is_running());
        return m_stop_ticks - m_start_ticks;
    }

    duration elapsed() const noexcept
    {
        if (is_running())
        {
            return elapsed_running();
        }
        return elapsed_stopped();
    }

private:

    bool m_running = false;
    duration m_start_ticks;
    duration m_stop_ticks;
};

} // namespace time
} // namespace vx
