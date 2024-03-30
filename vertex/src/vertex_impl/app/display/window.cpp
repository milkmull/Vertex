#include <chrono>
#include <thread>

#include "vertex/config.h"
#include "vertex/app/display/window.h"

#if defined(VX_SYSTEM_WINDOWS)

#include "win32_impl/app/display/win32_window.h"
using window_impl_type = vx::app::win32_window;

#endif

namespace vx {
namespace app {

std::unique_ptr<window> window::create(const std::string& title, const math::vec2i& size, const math::vec2i& position)
{
    return std::make_unique<window_impl_type>(title, size, position);
}

// =============== events ===============

void window::post_event(const event& e)
{
    m_events.push(e);
}

bool window::pop_event(event& e, bool block)
{
    if (m_events.empty())
    {
        process_events();

        if (block)
        {
            // while blocking, sleep in periods of 10 ms while checking for new events
            while (m_events.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                process_events();
            }
        }
    }

    // if there are any events in the queue return the next one
    if (!m_events.empty())
    {
        e = m_events.front();
        m_events.pop();

        return true;
    }

    return false;
}

}
}