#include "vertex_impl/app/event/_platform/platform_event.hpp"
#include "vertex/os/mutex.hpp"

#define VX_APP_LOG_EVENTS 1

#if defined(VX_APP_LOG_EVENTS)
#   include "vertex/system/log.hpp"
#endif // VX_APP_LOG_EVENTS

namespace vx {
namespace app {
namespace event {

#if defined(VX_APP_LOG_EVENTS)
static void log_event(const event&);
#endif // VX_APP_LOG_EVENTS

struct event_data
{
    os::mutex mutex;
    std::vector<event> events;
    size_t head = 0;           // read index
    size_t tail = 0;           // write index
    size_t count = 0;          // number of events in queue
    event_process_callback process_callback = nullptr;
};

static event_data s_event_data;

///////////////////////////////////////////////////////////////////////////////
// processing
///////////////////////////////////////////////////////////////////////////////

static void ensure_capacity(size_t new_capacity)
{
    if (new_capacity <= s_event_data.events.size())
    {
        return;
    }

    // allocate new buffer
    std::vector<event> new_events(new_capacity);

    // copy old data in order
    for (size_t i = 0; i < s_event_data.count; ++i)
    {
        const size_t j = (s_event_data.head + i) % s_event_data.events.size();
        new_events[i] = s_event_data.events[j];
    }

    s_event_data.events.swap(new_events);
    s_event_data.head = 0;
    s_event_data.tail = s_event_data.count;
}

VX_API void pump_events(bool process_all)
{
    pump_events_impl(process_all);
}

enum
{
    DEFAULT_EVENT_BUFFER_SIZE = 64,
    DEFAULT_EVENT_BUFFER_GROW_FACTOR = 2
};

VX_API bool post_event(const event& e)
{
#   if defined(VX_APP_LOG_EVENTS)
        log_event(e);
#   endif

    os::lock_guard<os::mutex> lock(s_event_data.mutex);

    // grow if full
    if (s_event_data.count == s_event_data.events.size())
    {
        const size_t new_capacity = s_event_data.events.empty() ? DEFAULT_EVENT_BUFFER_SIZE : s_event_data.events.size() * DEFAULT_EVENT_BUFFER_GROW_FACTOR;
        ensure_capacity(new_capacity);
    }

    s_event_data.events[s_event_data.tail] = e;
    s_event_data.tail = (s_event_data.tail + 1) % s_event_data.events.size();
    ++s_event_data.count;

    return true;
}

VX_API bool poll_event(event& e)
{
    pump_events(false);

    std::lock_guard<os::mutex> lock(s_event_data.mutex);

    if (s_event_data.count == 0)
    {
        return false;
    }

    // If there are any events in the queue return the next one
    e = s_event_data.events[s_event_data.head];
    s_event_data.head = (s_event_data.head + 1) % s_event_data.events.size();
    --s_event_data.count;

    return true;
}

VX_API void wait_events()
{
    //event_impl::wait_events();
}

VX_API void wait_events_timeout(unsigned int timeout_ms)
{
    //event_impl::wait_events_timeout(timeout_ms);
}

static bool event_type_eq(const event& e, void* user_data)
{
    return e.type == *static_cast<event_type*>(user_data);
}

VX_API size_t flush_events(event_type type)
{
    return filter_events(event_type_eq, &type);
}

VX_API size_t filter_events(event_filter filter, void* user_data)
{
    os::lock_guard lock(s_event_data.mutex);

    size_t removed = 0;

    if (s_event_data.count == 0)
    {
        return removed;
    }

    // Step 1: Skip initial block of filtered events
    while (s_event_data.count > 0 && filter(s_event_data.events[s_event_data.head], user_data))
    {
        s_event_data.head = (s_event_data.head + 1) % s_event_data.events.size();
        --s_event_data.count;
        ++removed;
    }

    // If everything was removed, we’re done
    if (s_event_data.count == 0)
    {
        s_event_data.tail = s_event_data.head;
        return removed;
    }

    // Step 2: Compact remaining matching events in place
    size_t copy_tail = s_event_data.head;
    const size_t original_count = s_event_data.count; // after initial skip

    for (size_t i = 0; i < original_count; ++i)
    {
        const size_t j = (s_event_data.head + i) % s_event_data.events.size();

        if (filter(s_event_data.events[j], user_data))
        {
            ++removed;
            --s_event_data.count; // adjust count as we remove
        }
        else
        {
            if (copy_tail != j)
            {
                s_event_data.events[copy_tail] = std::move(s_event_data.events[j]);
            }

            copy_tail = (copy_tail + 1) % s_event_data.events.size();
        }
    }

    // set the tail to the new tail
    s_event_data.tail = copy_tail;

    return removed;
}

///////////////////////////////////////////////////////////////////////////////
// callback
///////////////////////////////////////////////////////////////////////////////

VX_API void set_event_process_callback(event_process_callback callback)
{
    s_event_data.process_callback = callback;
}

///////////////////////////////////////////////////////////////////////////////
// checking
///////////////////////////////////////////////////////////////////////////////

VX_API bool has_event(event_type type)
{
    os::lock_guard lock(s_event_data.mutex);

    for (size_t i = 0; i < s_event_data.count; ++i)
    {
        const size_t j = (s_event_data.head + 1) % s_event_data.events.size();
        if (s_event_data.events[j].type == type)
        {
            return true;
        }
    }

    return false;
}

VX_API size_t event_count()
{
    return s_event_data.count;
}

VX_API const event* enum_events(size_t i)
{
    if (i >= s_event_data.count)
    {
        return nullptr;
    }

    const size_t j = (s_event_data.head + i) % s_event_data.events.size();
    return &s_event_data.events[j];
}

///////////////////////////////////////////////////////////////////////////////
// event logging
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_APP_LOG_EVENTS)

static void log_event(const event& e)
{
    switch (e.type)
    {
        // app events
        case APP_TERMINATING:
        {
            VX_LOG_INFO("APP_TERMINATING");
            break;
        }

        // display events
        case DISPLAY_ADDED:
        {
            VX_LOG_INFO(
                "DISPLAY_ADDED {",
                " display_id: ", e.display_id,
                " }"
            );
            break;
        }
        case DISPLAY_REMOVED:
        {
            VX_LOG_INFO(
                "DISPLAY_REMOVED {",
                " display_id: ", e.display_id,
                " }"
            );
            break;
        }
        case DISPLAY_MOVED:
        {
            VX_LOG_INFO(
                "DISPLAY_MOVED {",
                " display_id: ", e.display_id,
                " }"
            );
            break;
        }
        case DISPLAY_ORIENTATION_CHANGED:
        {
            VX_LOG_INFO(
                "DISPLAY_ORIENTATION_CHANGED {",
                " display_id: ", e.display_id, ',',
                " orientation: ", static_cast<int>(e.display_orientation_changed.orientation),
                " }"
            );
            break;
        }
        case DISPLAY_DESKTOP_MODE_CHANGED:
        {
            VX_LOG_INFO(
                "DISPLAY_DESKTOP_MODE_CHANGED {",
                " display_id: ", e.display_id,
                " }"
            );
            break;
        }
        case DISPLAY_CURRENT_MODE_CHANGED:
        {
            VX_LOG_INFO(
                "DISPLAY_CURRENT_MODE_CHANGED {",
                " display_id: ", e.display_id,
                " }"
            );
            break;
        }
        case DISPLAY_CONTENT_SCALE_CHANGED:
        {
            VX_LOG_INFO(
                "DISPLAY_CONTENT_SCALE_CHANGED {",
                " display_id: ", e.display_id, ',',
                " scale: {", e.display_content_scale_changed.x, ", ", e.display_content_scale_changed.y, " }",
                " }"
            );
            break;
        }

        // window events
        case WINDOW_SHOWN:
        {
            VX_LOG_INFO(
                "WINDOW_SHOWN {",
                " window_id: ", e.window_id,
                " }"
            );
            break;
        }
        case WINDOW_HIDDEN:
        {
            VX_LOG_INFO(
                "WINDOW_HIDDEN {",
                " window_id: ", e.window_id,
                " }"
            );
            break;
        }
        case WINDOW_MOVED:
        {
            VX_LOG_INFO(
                "WINDOW_MOVED {",
                " window_id: ", e.window_id,
                " position: {", e.window_moved.x, ", ", e.window_moved.y, " }",
                " }"
            );
            break;
        }
        case WINDOW_RESIZED:
        {
            VX_LOG_INFO(
                "WINDOW_RESIZED {",
                " window_id: ", e.window_id, ',',
                " size: {", e.window_resized.w, ", ", e.window_resized.h, " }",
                " }"
            );
            break;
        }
        case WINDOW_MINIMIZED:
        {
            VX_LOG_INFO(
                "WINDOW_MINIMIZED {",
                " window_id: ", e.window_id,
                " }"
            );
            break;
        }
        case WINDOW_MAXIMIZED:
        {
            VX_LOG_INFO(
                "WINDOW_MAXIMIZED {",
                " window_id: ", e.window_id,
                " }"
            );
            break;
        }
        case WINDOW_RESTORED:
        {
            VX_LOG_INFO(
                "WINDOW_RESTORED {",
                " window_id: ", e.window_id,
                " }"
            );
            break;
        }
        case WINDOW_ENTER_FULLSCREEN:
        {
            VX_LOG_INFO(
                "WINDOW_ENTER_FULLSCREEN {",
                " window_id: ", e.window_id,
                " }"
            );
            break;
        }
        case WINDOW_LEAVE_FULLSCREEN:
        {
            VX_LOG_INFO(
                "WINDOW_LEAVE_FULLSCREEN {",
                " window_id: ", e.window_id,
                " }"
            );
            break;
        }
        case WINDOW_GAINED_FOCUS:
        {
            VX_LOG_INFO(
                "WINDOW_GAINED_FOCUS {",
                " window_id: ", e.window_id,
                " }"
            );
            break;
        }
        case WINDOW_LOST_FOCUS:
        {
            VX_LOG_INFO(
                "WINDOW_LOST_FOCUS {",
                " window_id: ", e.window_id,
                " }"
            );
            break;
        }
        case WINDOW_MOUSE_ENTER:
        case WINDOW_MOUSE_LEAVE:
        {
            break;
        }
        case WINDOW_DISPLAY_CHANGED:
        {
            VX_LOG_INFO(
                "WINDOW_DISPLAY_CHANGED {",
                " window_id: ", e.window_id, ',',
                " display_id: ", e.window_display_changed.display_id,
                " }"
            );
            break;
        }
        case WINDOW_DISPLAY_SCALE_CHANGED:
        {
            break;
        }
        case WINDOW_CLOSE_REQUESTED:
        {
            VX_LOG_INFO(
                "WINDOW_CLOSE_REQUESTED {",
                " window_id: ", e.window_id,
                " }"
            );
            break;
        }
        case WINDOW_DESTROYED:
        {
            VX_LOG_INFO(
                "WINDOW_DESTROYED {",
                " window_id: ", e.window_id,
                " }"
            );
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

#endif // VX_APP_LOG_EVENTS

} // namespace event
} // namespace app
} // namespace vx