#include "vertex/app/event/event.h"

#if defined(VX_SYSTEM_WINDOWS)

#include "win32_impl/event/win32_event.h"

#endif

namespace vx {
namespace app {

std::deque<event> event::s_events = std::deque<event>();
std::mutex event::s_mutex = std::mutex();

void event::pump_events(bool process_all)
{
    event_impl::pump_events(process_all);
}

std::vector<event> event::peek_events()
{
    s_mutex.lock();
    std::vector<event> events(s_events.begin(), s_events.end());
    s_mutex.unlock();
    return events;
}

bool event::has_event(event_type type)
{
    bool found = false;
    s_mutex.lock();

    for (const event& e : s_events)
    {
        if (e.type == type)
        {
            found = true;
            break;
        }
    }

    s_mutex.unlock();
    return found;
}

bool event::post_event(const event& e)
{
    s_mutex.lock();
    s_events.push_back(e);
    s_mutex.unlock();

    return true;
}

bool event::poll_event(event& e)
{
    pump_events(false);

    bool found = false;
    s_mutex.lock();

    // If there are any events in the queue return the next one
    if (!s_events.empty())
    {
        e = s_events.front();
        s_events.pop_front();

        found = true;
    }

    s_mutex.unlock();
    return found;
}

void event::wait_events()
{
    event_impl::wait_events();
}

void event::wait_events_timeout(unsigned int timeout_ms)
{
    event_impl::wait_events_timeout(timeout_ms);
}

size_t event::flush_events(event_type type)
{
    size_t count = 0;
    s_mutex.lock();

    auto it = s_events.begin();
    while (it != s_events.end())
    {
        if (it->type == type)
        {
            ++count;
            it = s_events.erase(it);
        }
        else
        {
            ++it;
        }
    }

    s_mutex.unlock();
    return count;
}

}
}