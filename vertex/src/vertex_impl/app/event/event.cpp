#include "vertex/app/event/event.h"

#if defined(VX_SYSTEM_WINDOWS)
#   include "_win32/win32_event.h"
#endif

#if defined(VX_LOG_APP_EVENTS)
#   include "vertex/system/log.h"
#endif

#ifdef MOUSE_MOVED
#   undef MOUSE_MOVED
#endif

namespace vx {
namespace app {

std::deque<event> event::s_events = std::deque<event>();
std::mutex event::s_mutex = std::mutex();
event::event_process_callback_t s_event_process_callback = nullptr;

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
#if defined(VX_LOG_APP_EVENTS)
    log_event(e);
#endif

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

size_t event::filter_events(event_filter filter, void* user_data)
{
    size_t count = 0;

    if (filter)
    {
        s_mutex.lock();

        auto it = s_events.begin();
        while (it != s_events.end())
        {
            if (filter(user_data, *it))
            {
                it = s_events.erase(it);
                ++count;
            }
            else
            {
                ++it;
            }
        }

        s_mutex.unlock();
    }

    return count;
}

#define VX_LOG_APP_EVENTS 1

#if defined(VX_LOG_APP_EVENTS)

void event::log_event(const event& e)
{
    switch (e.type)
    {
        // app events
        case APP_TERMINATING:
        {
            VX_LOG_INFO << "APP_TERMINATING";
            break;
        }

        // display events
        case DISPLAY_ADDED:
        {
            VX_LOG_INFO << "DISPLAY_ADDED {"
                << "display_id: " << e.display_id
                << " }";
            break;
        }
        case DISPLAY_REMOVED:
        {
            VX_LOG_INFO << "DISPLAY_REMOVED {"
                << " display_id: " << e.display_id
                << " }";
            break;
        }
        case DISPLAY_MOVED:
        {
            VX_LOG_INFO << "DISPLAY_MOVED {"
                << " display_id: " << e.display_id
                << " }";
            break;
        }
        case DISPLAY_ORIENTATION_CHANGED:
        {
            VX_LOG_INFO << "DISPLAY_ORIENTATION_CHANGED {"
                << " display_id: "          << e.display_id
                << " orientation: " << static_cast<int>(e.display_orientation_changed.orientation)
                << " }";
            break;
        }
        case DISPLAY_DESKTOP_MODE_CHANGED:
        {
            VX_LOG_INFO << "DISPLAY_DESKTOP_MODE_CHANGED {"
                << " display_id: " << e.display_id
                << " }";
            break;
        }
        case DISPLAY_CURRENT_MODE_CHANGED:
        {
            VX_LOG_INFO << "DISPLAY_CURRENT_MODE_CHANGED {"
                << " display_id: " << e.display_id
                << " }";
            break;
        }
        case DISPLAY_CONTENT_SCALE_CHANGED:
        {
            VX_LOG_INFO << "DISPLAY_CONTENT_SCALE_CHANGED {"
                << " display_id: " << e.display_id
                << " scale: " << "{ " << e.display_content_scale_changed.x << ", " << e.display_content_scale_changed.y << " }"
                << " }";
            break;
        }

        // window events
        case WINDOW_SHOWN:
        {
            VX_LOG_INFO << "WINDOW_SHOWN {"
                << " window_id: " << e.window_id
                << " }";
            break;
        }
        case WINDOW_HIDDEN:
        {
            VX_LOG_INFO << "WINDOW_HIDDEN {"
                << " window_id: " << e.window_id
                << " }";
            break;
        }
        case WINDOW_MOVED:
        {
            VX_LOG_INFO << "WINDOW_MOVED {"
                << " window_id: " << e.window_id
                << " position: " << "{ " << e.window_moved.x << ", " << e.window_moved.y << " }"
                << " }";
            break;
        }
        case WINDOW_RESIZED:
        {
            VX_LOG_INFO << "WINDOW_RESIZED {"
                << " window_id: " << e.window_id
                << " size: " << "{ " << e.window_resized.w << ", " << e.window_resized.h << " }"
                << " }";
            break;
        }
        case WINDOW_MINIMIZED:
        {
            VX_LOG_INFO << "WINDOW_MINIMIZED {"
                << " window_id: " << e.window_id
                << " }";
            break;
        }
        case WINDOW_MAXIMIZED:
        {
            VX_LOG_INFO << "WINDOW_MAXIMIZED {"
                << " window_id: " << e.window_id
                << " }";
            break;
        }
        case WINDOW_RESTORED:
        {
            VX_LOG_INFO << "WINDOW_RESTORED {"
                << " window_id: " << e.window_id
                << " }";
            break;
        }
        case WINDOW_ENTER_FULLSCREEN:
        {
            VX_LOG_INFO << "WINDOW_ENTER_FULLSCREEN {"
                << " window_id: " << e.window_id
                << " }";
            break;
        }
        case WINDOW_LEAVE_FULLSCREEN:
        {
            VX_LOG_INFO << "WINDOW_LEAVE_FULLSCREEN {"
                << " window_id: " << e.window_id
                << " }";
            break;
        }
        case WINDOW_GAINED_FOCUS:
        {
            VX_LOG_INFO << "WINDOW_GAINED_FOCUS {"
                << " window_id: " << e.window_id
                << " }";
            break;
        }
        case WINDOW_LOST_FOCUS:
        {
            VX_LOG_INFO << "WINDOW_LOST_FOCUS {"
                << " window_id: " << e.window_id
                << " }";
            break;
        }
        case WINDOW_MOUSE_ENTER:
        case WINDOW_MOUSE_LEAVE:
        {
            break;
        }
        case WINDOW_DISPLAY_CHANGED:
        {
            VX_LOG_INFO << "WINDOW_DISPLAY_CHANGED {"
                << " window_id: " << e.window_id
                << " display_id: " << e.display_id
                << " }";
            break;
        }
        case WINDOW_DISPLAY_SCALE_CHANGED:
        {
            break;
        }
        case WINDOW_CLOSE_REQUESTED:
        {
            VX_LOG_INFO << "WINDOW_CLOSE_REQUESTED {"
                << " window_id: " << e.window_id
                << " }";
            break;
        }
        case WINDOW_DESTROYED:
        {
            VX_LOG_INFO << "WINDOW_DESTROYED {"
                << " window_id: " << e.window_id
                << " }";
            break;
        }

        // key events
        case KEY_DOWN:
        case KEY_UP:
        case TEXT_INPUT:
        {
            break;
        }

        // mouse events
        case MOUSE_MOVED:
        case MOUSE_BUTTON_DOWN:
        case MOUSE_BUTTON_UP:
        case MOUSE_WHEEL:
        {
            break;
        }
        default:
        {
            break;
        }
    }
}
#endif

}
}