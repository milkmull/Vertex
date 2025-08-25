#include "vertex_impl/app/event/event_internal.hpp"
#include "vertex_impl/app/event/_platform/platform_event.hpp"
#include "vertex_impl/app/app_internal.hpp"

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

///////////////////////////////////////////////////////////////////////////////
// initialization
///////////////////////////////////////////////////////////////////////////////

bool events_instance::init()
{
    return true;
}

void events_instance::quit()
{

}

///////////////////////////////////////////////////////////////////////////////
// processing
///////////////////////////////////////////////////////////////////////////////

void events_instance::ring_buffer::ensure_capacity(size_t new_capacity)
{
    if (new_capacity <= events.size())
    {
        return;
    }

    // allocate new buffer
    std::vector<event> new_events(new_capacity);

    // copy old data in order
    for (size_t i = 0; i < count; ++i)
    {
        const size_t j = (head + i) % events.size();
        new_events[i] = events[j];
    }

    events.swap(new_events);
    head = 0;
    tail = count;
}

////////////////////////////////////////

VX_API void pump_events(bool process_all)
{
    VX_CHECK_EVENT_SUBSYSTEM_INIT_VOID();
    s_events->pump_events(process_all);
}

void events_instance::pump_events(bool process_all)
{
    events_instance_impl::pump_events(process_all);
}

////////////////////////////////////////

VX_API bool post_event(const event& e)
{
    VX_CHECK_EVENT_SUBSYSTEM_INIT(false);
    return s_events->post_event(e);
}

bool events_instance::post_event(const event& e)
{
#if defined(VX_APP_LOG_EVENTS)
    log_event(e);
#endif

    ring_buffer& buf = s_event_data->ring_buffer;
    os::lock_guard<os::mutex> lock(buf.mutex);

    // grow if full
    if (buf.count == buf.events.size())
    {
        const size_t new_capacity = buf.events.empty() ? DEFAULT_EVENT_BUFFER_SIZE : buf.events.size() * DEFAULT_EVENT_BUFFER_GROW_FACTOR;
        ensure_capacity(buf, new_capacity);
    }

    buf.events[buf.tail] = e;
    buf.tail = (buf.tail + 1) % buf.events.size();
    ++buf.count;

    return true;
}

////////////////////////////////////////

VX_API bool poll_event(event& e)
{
    VX_CHECK_EVENT_SUBSYSTEM_INIT(false);
    pump_events(false);

    event_ring_buffer& buf = s_event_data->ring_buffer;
    std::lock_guard<os::mutex> lock(buf.mutex);

    if (buf.count == 0)
    {
        return false;
    }

    // If there are any events in the queue return the next one
    e = buf.events[buf.head];
    buf.head = (buf.head + 1) % buf.events.size();
    --buf.count;

    return true;
}

////////////////////////////////////////

VX_API void wait_events()
{
    VX_CHECK_EVENT_SUBSYSTEM_INIT_VOID();
    //event_impl::wait_events();
}

////////////////////////////////////////

VX_API void wait_events_timeout(unsigned int timeout_ms)
{
    VX_CHECK_EVENT_SUBSYSTEM_INIT_VOID();
    //event_impl::wait_events_timeout(timeout_ms);
}

////////////////////////////////////////

static bool event_type_eq(const event& e, void* user_data)
{
    return e.type == *static_cast<event_type*>(user_data);
}

////////////////////////////////////////

VX_API size_t flush_events(event_type type)
{
    return filter_events(event_type_eq, &type);
}

////////////////////////////////////////

VX_API size_t filter_events(event_filter filter, void* user_data)
{
    VX_CHECK_EVENT_SUBSYSTEM_INIT(0);

    event_ring_buffer& buf = s_event_data->ring_buffer;
    os::lock_guard lock(buf.mutex);

    size_t removed = 0;

    if (buf.count == 0)
    {
        return removed;
    }

    // Step 1: Skip initial block of filtered events
    while (buf.count > 0 && filter(buf.events[buf.head], user_data))
    {
        buf.head = (buf.head + 1) % buf.events.size();
        --buf.count;
        ++removed;
    }

    // If everything was removed, we’re done
    if (buf.count == 0)
    {
        buf.tail = buf.head;
        return removed;
    }

    // Step 2: Compact remaining matching events in place
    size_t copy_tail = buf.head;
    const size_t original_count = buf.count; // after initial skip

    for (size_t i = 0; i < original_count; ++i)
    {
        const size_t j = (buf.head + i) % buf.events.size();

        if (filter(buf.events[j], user_data))
        {
            ++removed;
            --buf.count; // adjust count as we remove
        }
        else
        {
            if (copy_tail != j)
            {
                buf.events[copy_tail] = std::move(buf.events[j]);
            }

            copy_tail = (copy_tail + 1) % buf.events.size();
        }
    }

    // set the tail to the new tail
    buf.tail = copy_tail;

    return removed;
}

///////////////////////////////////////////////////////////////////////////////
// callback
///////////////////////////////////////////////////////////////////////////////

VX_API void set_event_process_callback(event_process_callback callback)
{
    VX_CHECK_EVENT_SUBSYSTEM_INIT_VOID();
    s_event_data->process_callback = callback;
}

///////////////////////////////////////////////////////////////////////////////
// checking
///////////////////////////////////////////////////////////////////////////////

VX_API bool has_event(event_type type)
{
    VX_CHECK_EVENT_SUBSYSTEM_INIT(false);

    event_ring_buffer& buf = s_event_data->ring_buffer;
    os::lock_guard lock(buf.mutex);

    for (size_t i = 0; i < buf.count; ++i)
    {
        const size_t j = (buf.head + 1) % buf.events.size();
        if (buf.events[j].type == type)
        {
            return true;
        }
    }

    return false;
}

////////////////////////////////////////

VX_API size_t event_count()
{
    VX_CHECK_EVENT_SUBSYSTEM_INIT(0);
    event_ring_buffer& buf = s_event_data->ring_buffer;
    os::lock_guard lock(buf.mutex);
    return buf.count;
}

////////////////////////////////////////

VX_API std::vector<event> get_events()
{
    std::vector<event> events;
    VX_CHECK_EVENT_SUBSYSTEM_INIT(events);

    event_ring_buffer& buf = s_event_data->ring_buffer;
    os::lock_guard lock(buf.mutex);
    events.resize(buf.count);

    for (size_t i = 0; i < buf.count; ++i)
    {
        const size_t j = (buf.head + i) % buf.events.size();
        events[i] = buf.events[j];
    }

    return events;
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