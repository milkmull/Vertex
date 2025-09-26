#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/event/event_internal.hpp"
#include "vertex_impl/app/event/_platform/platform_event.hpp"
#include "vertex_impl/app/input/keyboard_internal.hpp"
#include "vertex_impl/app/video/video_internal.hpp"

///////////////////////////////////////////////////////////////////////////////
// feature macros
///////////////////////////////////////////////////////////////////////////////

#define VX_APP_LOG_EVENTS 1

#if VX_APP_LOG_EVENTS
#   include "vertex/system/log.hpp"
#endif // VX_APP_LOG_EVENTS

////////////////////////////////////////

// Feature macro indicating that the video subsystem supports both
// timed waits and external wakeups. In this mode, the OS can block
// while waiting for window events, and another thread can explicitly
// wake the window from that blocked state.
#define VX_EVENT_HAVE_WAIT_VIDEO_SUBSYSTEM (VX_VIDEO_HAVE_WAIT_EVENT_TIMEOUT && VX_VIDEO_HAVE_SEND_WAKEUP_EVENT) 

///////////////////////////////////////////////////////////////////////////////

namespace vx {
namespace app {
namespace event {

////////////////////////////////////////

#if defined(VX_APP_LOG_EVENTS)
static void log_event(const event&);
#endif // VX_APP_LOG_EVENTS

///////////////////////////////////////////////////////////////////////////////
// initialization
///////////////////////////////////////////////////////////////////////////////

bool events_instance::init(app_instance* owner)
{
    VX_ASSERT(!app);
    VX_ASSERT(owner);
    app = owner;
    return true;
}

bool events_instance::is_init() const
{
    return app != nullptr;
}

void events_instance::quit()
{
    app = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// event queue
///////////////////////////////////////////////////////////////////////////////

size_t event_queue::add(const event* events, size_t count)
{
    size_t added = 0;

    if (!events)
    {
        VX_ASSERT(false);
        return added;
    }

    os::lock_guard lock(mutex);

    for (size_t i = 0; i < count; ++i)
    {
        if (queue.size() + 1 >= MAX_EVENTS)
        {
            err::set(err::SIZE_ERROR, "event queue full");
            break;
        }

        const event& e = events[i];

        if (e.type == INTERNAL_EVENT_POLL_SENTINEL)
        {
            ++sentinel_pending;
        }

#if VX_APP_LOG_EVENTS
        log_event(e);
#endif // VX_APP_LOG_EVENTS

        queue.push_back(e);
        ++added;
    }

    return added;
}

////////////////////////////////////////

size_t event_queue::match(event_filter matcher, void* user_data, event* events, size_t count, bool remove)
{
    const bool copy = (events && count > 0);
    size_t matched = 0;

    os::lock_guard lock(mutex);

    if (queue.empty())
    {
        return matched;
    }

    auto it = queue.begin();
    while (it != queue.end() && (!copy || (matched < count)))
    {
        if (matcher(*it, user_data))
        {
            if (copy)
            {
                events[matched] = *it;
            }

            if (remove)
            {
                if (it->type == INTERNAL_EVENT_POLL_SENTINEL)
                {
                    --sentinel_pending;
                }

                it = queue.erase(it);
            }

            ++matched;
        }
        else
        {
            ++it;
        }
    }

    return matched;
}

////////////////////////////////////////

static bool poll_filter(const event& e, void*) noexcept
{
    return true;
}

bool event_queue::poll(event& e)
{
    return match(poll_filter, nullptr, &e, 1, true);
}

////////////////////////////////////////

static bool sentinel_filter(const event& e, void*) noexcept
{
    return (e.type == INTERNAL_EVENT_POLL_SENTINEL);
}

void event_queue::add_sentinel()
{
    event e;
    e.type = INTERNAL_EVENT_POLL_SENTINEL;

    if (sentinel_pending > 0)
    {
        match(sentinel_filter, nullptr, nullptr, 0, true);
    }

    add(&e, 1);
}

///////////////////////////////////////////////////////////////////////////////
// processing
///////////////////////////////////////////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_events.c#L1454

VX_API void pump_events(bool process_all)
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT_VOID();
    s_events_ptr->pump_events(process_all);
}

void events_instance::pump_events(bool process_all)
{
    events_instance_impl::pump_events(process_all);
}

////////////////////////////////////////

void events_instance::pump_events_maintenance()
{

}

////////////////////////////////////////

void events_instance::pump_events_internal(bool push_sentinel)
{
    if (app->is_video_init())
    {
        // release any keys heald down from last frame
        keyboard::keyboard_internal::release_auto_release_keys();

        // pump events from video subsystem
        video::video_internal::pump_events();
    }

    // pump events for other subsystems
    events_instance::pump_events_maintenance();

    if (push_sentinel)
    {
        data.queue.add_sentinel();
    }
}

////////////////////////////////////////

VX_API size_t add_events(const event* e, size_t count)
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT(0);
    return s_events_ptr->add_events(e, count);
}

size_t events_instance::add_events(const event* e, size_t count)
{
    const size_t n = data.queue.add(e, count);

#if VX_EVENT_HAVE_WAIT_VIDEO_SUBSYSTEM

    // if we happen to be blocking in another thread, send a
    // wakeup event to signal that an event was added to the queue

    if (n && app->is_video_init())
    {
        video::video_internal::send_wakeup_event();
    }

#endif // VX_EVENT_HAVE_WAIT_VIDEO_SUBSYSTEM

    return n;
}

////////////////////////////////////////

VX_API size_t match_events(event_filter matcher, void* user_data, event* events, size_t count, bool remove)
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT(0);
    return s_events_ptr->match_events(matcher, user_data, events, count, remove);
}

size_t events_instance::match_events(event_filter matcher, void* user_data, event* events, size_t count, bool remove)
{
    return data.queue.match(matcher, user_data, events, count, remove);
}

////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_events.c#L1506

time::time_point events_instance::get_polling_interval() const
{
    time::time_point interval = time::max();

    return interval;
}

////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_events.c#L1532

#if VX_EVENT_HAVE_WAIT_VIDEO_SUBSYSTEM

int events_instance::wait_event_timeout_video(video::window* w, event& e, time::time_point t, time::time_point start)
{
    // Get the global polling interval (or time::max if disabled)
    time::time_point poll_interval = get_polling_interval();
    const bool polling_enabled = (poll_interval != time::max());
    // Remaining time until the timeout expires
    time::time_point loop_timeout = t;

    while (true)
    {
        pump_events_internal(true);

        // attempt to get the next event
        const bool res = data.queue.match(poll_filter, nullptr, &e, 1, true);

        if (res && e.type != INTERNAL_EVENT_POLL_SENTINEL)
        {
            // found an event
            return 1;
        }

        // If a timeout was requested, update remaining time
        if (t.is_positive())
        {
            const time::time_point elapsed = os::get_ticks() - start;
            if (elapsed > loop_timeout)
            {
                return 0;
            }

            loop_timeout = t - elapsed;
        }

        // Clamp timeout to polling interval if periodic polling is enabled.
        // We only want to wait for events so long, so we don't hold up other
        // processes such as sensor updates.
        if (polling_enabled)
        {
            if (loop_timeout >= time::zero())
            {
                // we still have time left
                loop_timeout = std::min(loop_timeout, poll_interval);
            }
            else
            {
                // timeout already expired -> just use poll interval
                loop_timeout = poll_interval;
            }
        }

        // Block and wait for OS/window events up to loop_timeout
        const int result = video::video_internal::wait_event_timeout(w, loop_timeout);

        if (result <= 0)
        {
            if (result == 0 && polling_enabled && loop_timeout == poll_interval)
            {
                // Timeout expired but we're in periodic polling mode -> loop again
                continue;
            }

            // error or timeout elapsed
            return result;
        }

        // An event was pumped into the queue -> loop again to pick it up
    }

    return 0;
}

#endif // VX_EVENT_HAVE_WAIT_VIDEO_SUBSYSTEM

////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_events.c#L1606

VX_API bool wait_event_timeout(event& e, time::time_point t)
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT(false);
    return s_events_ptr->wait_event_timeout(e, t);
}

bool events_instance::wait_event_timeout(event& e, time::time_point t)
{
    const bool zero_timeout = t.is_zero();

    // initialize to zero
    time::time_point start, expiration;

    if (t.is_positive() > 0)
    {
        start = os::get_ticks();
        expiration = start + t;
    }

    // if no poll sentinel is pending, pump events
    if (data.queue.sentinel_pending == 0)
    {
        pump_events_internal(true);
    }

    // attempt to get the next event
    const bool res = data.queue.match(poll_filter, nullptr, &e, 1, true);

    if (zero_timeout)
    {
        // looking for an event right away, we succeed
        // as long at the event is not the sentinel
        return res && (e.type != INTERNAL_EVENT_POLL_SENTINEL);
    }

    VX_ASSERT(!zero_timeout);

    if (res && e.type != INTERNAL_EVENT_POLL_SENTINEL)
    {
        // found an event right away, no need to wait
        return true;
    }

    // no immediate event was found, continue to poll and wait

#if VX_EVENT_HAVE_WAIT_VIDEO_SUBSYSTEM

    if (app->is_video_init())
    {
        video::window* w = video::video_internal::get_active_window();
        if (w)
        {
            const int result = wait_event_timeout_video(w, e, t, start);

            if (result == 1)
            {
                return true;
            }
            if (result == 0)
            {
                return false;
            }

            // fall back to polling if error
        }
    }

#endif // VX_EVENT_HAVE_WAIT_VIDEO_SUBSYSTEM

    while (true)
    {
        pump_events_internal(true);

        if (data.queue.match(poll_filter, nullptr, &e, 1, true))
        {
            // found an event
            return true;
        }

        time::time_point delay = time::milliseconds(DEFAULT_POLL_INTERVAL_MS);

        if (t.is_positive())
        {
            const time::time_point now = os::get_ticks();
            if (now >= expiration)
            {
                // timeout expired and no events
                return false;
            }

            delay = std::min((expiration - now), delay);
        }

        os::sleep(delay);
    }

    return false;
}

////////////////////////////////////////

VX_API bool push_event(const event& e)
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT(false);
    return s_events_ptr->push_event(e);
}

bool events_instance::push_event(const event& e)
{
    if (!dispatch_event_watch(e))
    {
        return false;
    }

    return add_events(&e, 1) == 1;
}

////////////////////////////////////////

VX_API bool poll_event(event& e)
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT(false);
    return s_events_ptr->poll_event(e);
}

bool events_instance::poll_event(event& e)
{
    return wait_event_timeout(e, time::zero());
}

////////////////////////////////////////

VX_API void set_event_filter(event_filter filter, void* user_data)
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT_VOID();
    s_events_ptr->set_event_filter(filter, user_data);
}

void events_instance::set_event_filter(event_filter filter, void* user_data)
{
    os::lock_guard lock(data.watch.mutex);

    data.watch.filter.callback = filter;
    data.watch.filter.user_data = user_data;

    // filter current events
    data.queue.match(filter, user_data, nullptr, 0, true);
}

////////////////////////////////////////

VX_API void get_event_filter(event_filter& filter, void*& user_data)
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT_VOID();
    s_events_ptr->get_event_filter(filter, user_data);
}

void events_instance::get_event_filter(event_filter& filter, void*& user_data)
{
    os::lock_guard lock(data.watch.mutex);

    filter = data.watch.filter.callback;
    user_data = data.watch.filter.user_data;
}

////////////////////////////////////////

VX_API void add_event_watch(event_filter callback, void* user_data)
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT_VOID();
    s_events_ptr->add_event_watch(callback, user_data);
}

void events_instance::add_event_watch(event_filter callback, void* user_data)
{
    event_watcher watcher{ callback, user_data, false };

    os::lock_guard lock(data.watch.mutex);
    data.watch.watchers.push_back(watcher);
}

////////////////////////////////////////

VX_API void remove_event_watch(event_filter callback, void* user_data)
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT_VOID();
    s_events_ptr->remove_event_watch(callback, user_data);
}

void events_instance::remove_event_watch(event_filter callback, void* user_data)
{
    os::lock_guard lock(data.watch.mutex);
   
    for (auto it = data.watch.watchers.begin(); it != data.watch.watchers.end(); ++it)
    {
        if (it->callback == callback && it->user_data == user_data)
        {
            if (!data.watch.dispatching)
            {
                data.watch.watchers.erase(it);
            }
            else
            {
                it->removed = true;
                data.watch.removed = true;
            }

            break;
        }
    }
}

////////////////////////////////////////

void events_instance::prune_removed_watchers()
{
    for (auto it = data.watch.watchers.begin(); it != data.watch.watchers.end();)
    {
        if (it->removed)
        {
            it = data.watch.watchers.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

bool events_instance::dispatch_event_watch(const event& e)
{
    if (e.type == event_type::INTERNAL_EVENT_POLL_SENTINEL)
    {
        return true;
    }

    if (!data.watch.filter.callback && data.watch.watchers.empty())
    {
        // nothing to do
        return true;
    }

    os::lock_guard lock(data.watch.mutex);

    const auto filter = data.watch.filter;

    if (filter.callback && !filter.callback(e, filter.user_data))
    {
        // event got filtered out
        return false;
    }

    if (data.watch.watchers.empty())
    {
        // early exit
        return true;
    }

    data.watch.dispatching = true;

    for (size_t i = 0; i < data.watch.watchers.size(); ++i)
    {
        auto& watcher = data.watch.watchers[i];
        
        if (!watcher.removed)
        {
            watcher.callback(e, watcher.user_data);
        }
    }

    data.watch.dispatching = false;

    if (data.watch.removed)
    {
        prune_removed_watchers();
        data.watch.removed = false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// event logging
///////////////////////////////////////////////////////////////////////////////

#if VX_APP_LOG_EVENTS

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
                " display_id: ", e.display_added.display_id,
                " }"
            );
            break;
        }
        case DISPLAY_REMOVED:
        {
            VX_LOG_INFO(
                "DISPLAY_REMOVED {",
                " display_id: ", e.display_removed.display_id,
                " }"
            );
            break;
        }
        case DISPLAY_MOVED:
        {
            VX_LOG_INFO(
                "DISPLAY_MOVED {",
                " display_id: ", e.display_moved.display_id,
                " }"
            );
            break;
        }
        case DISPLAY_ORIENTATION_CHANGED:
        {
            VX_LOG_INFO(
                "DISPLAY_ORIENTATION_CHANGED {",
                " display_id: ", e.display_orientation_changed.display_id, ',',
                " orientation: ", static_cast<int>(e.display_orientation_changed.orientation),
                " }"
            );
            break;
        }
        case DISPLAY_DESKTOP_MODE_CHANGED:
        {
            VX_LOG_INFO(
                "DISPLAY_DESKTOP_MODE_CHANGED {",
                " display_id: ", e.display_desktop_mode_changed.display_id,
                " }"
            );
            break;
        }
        case DISPLAY_CURRENT_MODE_CHANGED:
        {
            VX_LOG_INFO(
                "DISPLAY_CURRENT_MODE_CHANGED {",
                " display_id: ", e.display_current_mode_changed.display_id,
                " }"
            );
            break;
        }
        case DISPLAY_CONTENT_SCALE_CHANGED:
        {
            VX_LOG_INFO(
                "DISPLAY_CONTENT_SCALE_CHANGED {",
                " display_id: ", e.display_content_scale_changed.display_id, ',',
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
                " window_id: ", e.window_shown.window_id,
                " }"
            );
            break;
        }
        case WINDOW_HIDDEN:
        {
            VX_LOG_INFO(
                "WINDOW_HIDDEN {",
                " window_id: ", e.window_hidden.window_id,
                " }"
            );
            break;
        }
        case WINDOW_MOVED:
        {
            VX_LOG_INFO(
                "WINDOW_MOVED {",
                " window_id: ", e.window_moved.window_id,
                " position: {", e.window_moved.x, ", ", e.window_moved.y, " }",
                " }"
            );
            break;
        }
        case WINDOW_RESIZED:
        {
            VX_LOG_INFO(
                "WINDOW_RESIZED {",
                " window_id: ", e.window_resized.window_id, ',',
                " size: {", e.window_resized.w, ", ", e.window_resized.h, " }",
                " }"
            );
            break;
        }
        case WINDOW_MINIMIZED:
        {
            VX_LOG_INFO(
                "WINDOW_MINIMIZED {",
                " window_id: ", e.window_minimized.window_id,
                " }"
            );
            break;
        }
        case WINDOW_MAXIMIZED:
        {
            VX_LOG_INFO(
                "WINDOW_MAXIMIZED {",
                " window_id: ", e.window_maximized.window_id,
                " }"
            );
            break;
        }
        case WINDOW_RESTORED:
        {
            VX_LOG_INFO(
                "WINDOW_RESTORED {",
                " window_id: ", e.window_restored.window_id,
                " }"
            );
            break;
        }
        case WINDOW_ENTER_FULLSCREEN:
        {
            VX_LOG_INFO(
                "WINDOW_ENTER_FULLSCREEN {",
                " window_id: ", e.window_enter_fullscreen.window_id,
                " }"
            );
            break;
        }
        case WINDOW_LEAVE_FULLSCREEN:
        {
            VX_LOG_INFO(
                "WINDOW_LEAVE_FULLSCREEN {",
                " window_id: ", e.window_leave_fullscreen.window_id,
                " }"
            );
            break;
        }
        case WINDOW_GAINED_FOCUS:
        {
            VX_LOG_INFO(
                "WINDOW_GAINED_FOCUS {",
                " window_id: ", e.window_gained_focus.window_id,
                " }"
            );
            break;
        }
        case WINDOW_LOST_FOCUS:
        {
            VX_LOG_INFO(
                "WINDOW_LOST_FOCUS {",
                " window_id: ", e.window_lost_focus.window_id,
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
                " window_id: ", e.window_display_changed.window_id, ',',
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
                " window_id: ", e.window_close_requested.window_id,
                " }"
            );
            break;
        }
        case WINDOW_DESTROYED:
        {
            VX_LOG_INFO(
                "WINDOW_DESTROYED {",
                " window_id: ", e.window_destroyed.window_id,
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