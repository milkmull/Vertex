#include "vertex/config/util.hpp"
#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/event/event_internal.hpp"
#include "vertex_impl/app/event/_platform/platform_event.hpp"

#if defined(VX_APP_VIDEO_ENABLED)
#   include "vertex_impl/app/input/keyboard_internal.hpp"
#   include "vertex_impl/app/video/video_internal.hpp"
#endif // VX_APP_VIDEO_ENABLED

///////////////////////////////////////////////////////////////////////////////
// feature macros
///////////////////////////////////////////////////////////////////////////////

#define VX_APP_LOG_EVENTS 1

#if VX_APP_LOG_EVENTS
#   include "vertex/system/log.hpp"
#endif // VX_APP_LOG_EVENTS

////////////////////////////////////////

#if defined(VX_APP_VIDEO_ENABLED)

// Feature macro indicating that the video subsystem supports both
// timed waits and external wakeups. In this mode, the OS can block
// while waiting for window events, and another thread can explicitly
// wake the window from that blocked state.
#define VX_EVENT_HAVE_WAIT_VIDEO_SUBSYSTEM (VX_VIDEO_BACKEND_HAVE_WAIT_EVENT_TIMEOUT && VX_VIDEO_BACKEND_HAVE_SEND_WAKEUP_EVENT)

#else

#define VX_EVENT_HAVE_WAIT_VIDEO_SUBSYSTEM 0

#endif // VX_APP_VIDEO_ENABLED

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

    // initialize ticks if they have not been already
    os::get_ticks();

    return true;
}

void events_instance::quit()
{
}

///////////////////////////////////////////////////////////////////////////////
// event queue
///////////////////////////////////////////////////////////////////////////////

size_t event_queue::add(const event* events, size_t count)
{
    size_t added = 0;

    if (!events || count == 0)
    {
        VX_ASSERT(false);
        return added;
    }

    os::lock_guard lock(mutex);

    for (size_t i = 0; i < count; ++i)
    {
        if (queue.size() + 1 >= max_events)
        {
            err::set(err::size_error, "event queue full");
            break;
        }

        const event& e = events[i];

        if (e.type == internal_event_poll_sentinel)
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
                if (it->type == internal_event_poll_sentinel)
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
    return (e.type == internal_event_poll_sentinel);
}

void event_queue::add_sentinel()
{
    event e;
    e.type = internal_event_poll_sentinel;

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
    pump_events_internal(false);
}

////////////////////////////////////////

void events_instance::pump_events_maintenance()
{

}

////////////////////////////////////////

void events_instance::pump_events_internal(bool push_sentinel)
{
#if defined(VX_APP_VIDEO_ENABLED)

    if (app->is_video_init())
    {
        // release any keys heald down from last frame
        app->data.video_ptr->data.keyboard_ptr->release_auto_release_keys();

        // pump events from video subsystem
        app->data.video_ptr->pump_events();
    }

#endif // VX_APP_VIDEO_ENABLED

    // pump events for other subsystems
    pump_events_maintenance();

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

    // if we happen to be blocking in another thread, send a
    // wakeup event to signal that an event was added to the queue

#if defined(VX_APP_VIDEO_ENABLED)

    if (n && app->is_video_init())
    {
        app->data.video_ptr->send_wakeup_event();
    }

#endif // VX_APP_VIDEO_ENABLED

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

#if defined(VX_APP_VIDEO_ENABLED)

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_events.c#L1532

#if VX_EVENT_HAVE_WAIT_VIDEO_SUBSYSTEM

int events_instance::wait_event_timeout_video(video::window_id w, event& e, time::time_point t, time::time_point start)
{
    VX_ASSERT(app->is_video_init());

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

        if (res && e.type != internal_event_poll_sentinel)
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
        const int result = app->data.video_ptr->wait_event_timeout(w, loop_timeout);

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

#else

int events_instance::wait_event_timeout_video(video::window_id, event&, time::time_point, time::time_point) { return 0; }

#endif // VX_EVENT_HAVE_WAIT_VIDEO_SUBSYSTEM

#endif // VX_APP_VIDEO_ENABLED

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

    if (t.is_positive())
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
        return res && (e.type != internal_event_poll_sentinel);
    }

    VX_ASSERT(!zero_timeout);

    if (res && e.type != internal_event_poll_sentinel)
    {
        // found an event right away, no need to wait
        return true;
    }

    // no immediate event was found, continue to poll and wait

#if VX_EVENT_HAVE_WAIT_VIDEO_SUBSYSTEM

    if (app->is_video_init())
    {
        video::window_id w = app->data.video_ptr->get_active_window();
        if (is_valid_id(w))
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

        time::time_point delay = time::milliseconds(default_poll_interval_ms);

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

VX_API bool push_event(event& e)
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT(false);
    return s_events_ptr->push_event(e);
}

bool events_instance::push_event(event& e)
{
    return push_event_filtered(e, nullptr, nullptr);
}

bool events_instance::push_event_filtered(event& e, event_filter filter, void* user_data)
{
    if (e.time.is_zero())
    {
        e.time = os::get_ticks();
    }

    if (!dispatch_event_watch(e))
    {
        return false;
    }

    if (filter)
    {
        match_events(filter, user_data, nullptr, 0, true);
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
    VX_VEC_GROW_BY_1(data.watch.watchers, watcher);
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
    if (e.type == internal_event_poll_sentinel)
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
        
        // only call the watcher if it was not removed in a callback
        if (!watcher.removed)
        {
            watcher.callback(e, watcher.user_data);
        }
    }

    data.watch.dispatching = false;

    // remove any watchers removed in callbacks
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
        case app_terminating:
        {
            VX_LOG_INFO("app_terminating");
            break;
        }

#if defined(VX_APP_VIDEO_ENABLED)

        // display events
        case display_added:
        {
            VX_LOG_INFO(
                "display_added {",
                " time: ", e.time.as_nanoseconds(),
                " display_id: ", e.display_event.comon.display_id,
                " }"
            );
            break;
        }
        case display_removed:
        {
            VX_LOG_INFO(
                "display_removed {",
                " time: ", e.time.as_nanoseconds(),
                " display_id: ", e.display_event.comon.display_id,
                " }"
            );
            break;
        }
        case display_moved:
        {
            VX_LOG_INFO(
                "display_moved {",
                " time: ", e.time.as_nanoseconds(),
                " display_id: ", e.display_event.comon.display_id,
                " }"
            );
            break;
        }
        case display_orientation_changed:
        {
            VX_LOG_INFO(
                "display_orientation_changed {",
                " time: ", e.time.as_nanoseconds(),
                " display_id: ", e.display_event.comon.display_id, ',',
                " orientation: ", static_cast<int>(e.display_event.display_orientation_changed.orientation),
                " }"
            );
            break;
        }
        case display_desktop_mode_changed:
        {
            VX_LOG_INFO(
                "display_desktop_mode_changed {",
                " time: ", e.time.as_nanoseconds(),
                " display_id: ", e.display_event.comon.display_id,
                " }"
            );
            break;
        }
        case display_current_mode_changed:
        {
            VX_LOG_INFO(
                "display_current_mode_changed {",
                " time: ", e.time.as_nanoseconds(),
                " display_id: ", e.display_event.comon.display_id,
                " }"
            );
            break;
        }
        case display_content_scale_changed:
        {
            VX_LOG_INFO(
                "display_content_scale_changed {",
                " time: ", e.time.as_nanoseconds(),
                " display_id: ", e.display_event.comon.display_id, ',',
                " scale: {", e.display_event.display_content_scale_changed.x, ", ", e.display_event.display_content_scale_changed.y, " }",
                " }"
            );
            break;
        }

        // window events
        case window_shown:
        {
            VX_LOG_INFO(
                "window_shown {",
                " time: ", e.time.as_nanoseconds(),
                " window_id: ", e.window_event.common.window_id,
                " }"
            );
            break;
        }
        case window_hidden:
        {
            VX_LOG_INFO(
                "window_hidden {",
                " time: ", e.time.as_nanoseconds(),
                " window_id: ", e.window_event.common.window_id,
                " }"
            );
            break;
        }
        case window_moved:
        {
            VX_LOG_INFO(
                "window_moved {",
                " time: ", e.time.as_nanoseconds(),
                " window_id: ", e.window_event.common.window_id,
                " position: {", e.window_event.window_moved.x, ", ", e.window_event.window_moved.y, " }",
                " }"
            );
            break;
        }
        case window_resized:
        {
            VX_LOG_INFO(
                "window_resized {",
                " time: ", e.time.as_nanoseconds(),
                " window_id: ", e.window_event.common.window_id, ',',
                " size: {", e.window_event.window_resized.w, ", ", e.window_event.window_resized.h, " }",
                " }"
            );
            break;
        }
        case window_minimized:
        {
            VX_LOG_INFO(
                "window_minimized {",
                " time: ", e.time.as_nanoseconds(),
                " window_id: ", e.window_event.common.window_id,
                " }"
            );
            break;
        }
        case window_maximized:
        {
            VX_LOG_INFO(
                "window_maximized {",
                " time: ", e.time.as_nanoseconds(),
                " window_id: ", e.window_event.common.window_id,
                " }"
            );
            break;
        }
        case window_restored:
        {
            VX_LOG_INFO(
                "window_restored {",
                " time: ", e.time.as_nanoseconds(),
                " window_id: ", e.window_event.common.window_id,
                " }"
            );
            break;
        }
        case window_enter_fullscreen:
        {
            VX_LOG_INFO(
                "window_enter_fullscreen {",
                " time: ", e.time.as_nanoseconds(),
                " window_id: ", e.window_event.common.window_id,
                " }"
            );
            break;
        }
        case window_leave_fullscreen:
        {
            VX_LOG_INFO(
                "window_leave_fullscreen {",
                " time: ", e.time.as_nanoseconds(),
                " window_id: ", e.window_event.common.window_id,
                " }"
            );
            break;
        }
        case window_gained_focus:
        {
            VX_LOG_INFO(
                "window_gained_focus {",
                " time: ", e.time.as_nanoseconds(),
                " window_id: ", e.window_event.common.window_id,
                " }"
            );
            break;
        }
        case window_lost_focus:
        {
            VX_LOG_INFO(
                "window_lost_focus {",
                " time: ", e.time.as_nanoseconds(),
                " window_id: ", e.window_event.common.window_id,
                " }"
            );
            break;
        }
        case window_mouse_enter:
        case window_mouse_leave:
        {
            break;
        }
        case window_display_changed:
        {
            VX_LOG_INFO(
                "window_display_changed {",
                " time: ", e.time.as_nanoseconds(),
                " window_id: ", e.window_event.common.window_id, ',',
                " display_id: ", e.window_event.window_display_changed.display_id,
                " }"
            );
            break;
        }
        case window_display_scale_changed:
        {
            break;
        }
        case window_close_requested:
        {
            VX_LOG_INFO(
                "window_close_requested {",
                " time: ", e.time.as_nanoseconds(),
                " window_id: ", e.window_event.common.window_id,
                " }"
            );
            break;
        }
        case window_destroyed:
        {
            VX_LOG_INFO(
                "window_destroyed {",
                " time: ", e.time.as_nanoseconds(),
                " window_id: ", e.window_event.common.window_id,
                " }"
            );
            break;
        }

        // key events
        case key_down:
        case key_up:
        case text_input:
        {
            break;
        }

        // mouse events
        case mouse_moved:
        case mouse_button_down:
        case mouse_button_up:
        case mouse_wheel:
        {
            break;
        }

#endif // VX_APP_VIDEO_ENABLED

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