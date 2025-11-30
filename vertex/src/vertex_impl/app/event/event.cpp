#include "vertex/config/util.hpp"
#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/event/event_internal.hpp"
#include "vertex_impl/app/event/_platform/platform_event.hpp"
#include "vertex_impl/app/event/temporary_memory_pool.hpp"

#if defined(VX_APP_VIDEO_ENABLED)
#   include "vertex_impl/app/input/keyboard_internal.hpp"
#   include "vertex_impl/app/video/video_internal.hpp"
#endif // VX_APP_VIDEO_ENABLED

//=============================================================================
// feature macros
//=============================================================================

#define VX_APP_LOG_EVENTS 1

#if VX_APP_LOG_EVENTS
#   include "vertex/system/log.hpp"
#endif // VX_APP_LOG_EVENTS

//=============================================================================
// helper macros
//=============================================================================

#define video_ptr app->data.video_ptr 

//=============================================================================

namespace vx {
namespace app {
namespace event {

//=============================================================================

#if defined(VX_APP_LOG_EVENTS)
static void log_event(const event&);
#endif // VX_APP_LOG_EVENTS

//=============================================================================
// memory
//=============================================================================

static temporary_memory_pool& get_temporary_memory_pool()
{
    static thread_local temporary_memory_pool stl_pool;
    return stl_pool;
}

//=============================================================================

static void claim_event_temporary_memory(event_queue_entry& entry)
{
#   define claim(ptr) entry.memory = get_temporary_memory_pool().release_entry(ptr)

    switch (entry.e.type)
    {
#if defined(VX_APP_VIDEO_ENABLED)

        case text_input:
        {
            claim(entry.e.text_event.text_input.text);
            break;
        }

#endif // VX_APP_VIDEO_ENABLED

        default:
        {
            return;
        }
    }

#   undef claim
}

//=============================================================================

static void return_event_temporary_memory(event_queue_entry& entry)
{
    if (entry.memory)
    {
        get_temporary_memory_pool().give_entry(entry.memory);
        entry.memory = nullptr;
    }
}

//=============================================================================

void* allocate_temporary_memory_raw(size_t size)
{
    return get_temporary_memory_pool().allocate_memory(size);
}

const char* create_temporary_string(const char* src)
{
    if (!src)
    {
        return nullptr;
    }

    const size_t size = std::strlen(src) + 1;
    char* dst = static_cast<char*>(allocate_temporary_memory_raw(sizeof(char) * size));

    if (!dst)
    {
        return nullptr;
    }

    std::strcpy(dst, src);
    give_temporary_memory(dst);
    return dst;
}

const char** create_temporary_string_array(const char* const* src, size_t count)
{
    // Space for the pointer table:
    // (count + 1) entries of 'const char*'
    // The +1 is for the final nullptr terminator.
    size_t total_length = (count + 1) * sizeof(char);

    // Add the space required for all strings stored consecutively,
    // each including its null terminator.
    for (size_t i = 0; i < count; ++i)
    {
        total_length += std::strlen(src[i]) + 1;
    }

    // Allocate a single contiguous block:
    // [pointer table][string data...]
    const char** dst = allocate_temporary_memory<const char**>(total_length);
    if (!dst)
    {
        return nullptr;
    }

    // Pointer where string storage begins.
    // This jumps past the pointer table:
    char* ptr = reinterpret_cast<char*>(const_cast<char**>(dst + (count + 1)));

    // Copy each string into the packed region and
    // populate the pointer table to point at each copied string.
    for (size_t i = 0; i < count; ++i)
    {
        const size_t length = std::strlen(src[i]) + 1;
        std::memcpy(ptr, src[i], length);
        ptr += length;
    }

    // Null-terminate the pointer array
    dst[count] = nullptr;
    return dst;
}

const void* release_temporary_memory(const void* ptr)
{
    return get_temporary_memory_pool().release_memory(ptr);
}

void give_temporary_memory(void* ptr)
{
    get_temporary_memory_pool().give_memory(ptr);
}

//=============================================================================

static void free_temporary_memory()
{
    get_temporary_memory_pool().clear();
}

//=============================================================================
// event queue
//=============================================================================

bool event_queue::start()
{
    os::lock_guard lock(mutex);
    active = true;
    return true;
}

void event_queue::stop()
{
    os::lock_guard lock(mutex);
    active = false;
    clear();
}

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

#if VX_APP_LOG_EVENTS
        log_event(e);
#endif // VX_APP_LOG_EVENTS

        if (e.type == internal_event_poll_sentinel)
        {
            ++sentinel_pending;
        }

        queue.push_back(event_queue_entry{ e, nullptr });
        claim_event_temporary_memory(queue.back());

        ++added;
    }

    return added;
}

//=============================================================================

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
        if (!matcher || matcher(it->e, user_data))
        {
            if (copy)
            {
                events[matched] = it->e;
            }

            if (remove)
            {
                if (it->e.type == internal_event_poll_sentinel)
                {
                    --sentinel_pending;
                }

                return_event_temporary_memory(*it);
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

//=============================================================================

bool event_queue::poll(event& e)
{
    return match(nullptr, nullptr, &e, 1, true);
}

//=============================================================================

void event_queue::clear()
{
    match(nullptr, nullptr, nullptr, queue.size(), true);
    VX_ASSERT(queue.empty());
    VX_ASSERT(sentinel_pending == 0);
}

//=============================================================================

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

//=============================================================================
// initialization
//=============================================================================

events_instance::events_instance() = default;
events_instance::~events_instance() { quit(); }

bool events_instance::init(app_instance* owner)
{
    if (app)
    {
        quit();
    }
    app = owner;

    // initialize ticks if they have not been already
    os::get_ticks();

    if (!start_loop())
    {
        goto failed;
    }

    init_signal_handler();

    return true;

    failed:
    {
        quit();
        return false;
    }
}

//=============================================================================

void events_instance::quit()
{
    quit_signal_handler();
    stop_loop();
    data.watch.clear();
    app = nullptr;
}

//=============================================================================
// loop
//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_events.c#L1008

bool events_instance::start_loop()
{
    return data.queue.start();
}

//=============================================================================

void events_instance::stop_loop()
{
    data.queue.stop();
}

//=============================================================================
// pump
//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_events.c#L1454

VX_API void pump_events()
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT_VOID();
    s_events_ptr->pump_events();
}

void events_instance::pump_events()
{
    pump_events_internal(false);
}

//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_events.c#L1458

void events_instance::pump_events_maintenance()
{
    send_pending_signal_events();
}

//=============================================================================

void events_instance::pump_events_internal(bool push_sentinel)
{
    free_temporary_memory();

#if defined(VX_APP_VIDEO_ENABLED)

    if (app->is_video_init())
    {
        // release any keys heald down from last frame
        video_ptr->data.keyboard_ptr->release_auto_release_keys();

        // pump events from video subsystem
        video_ptr->pump_events();
    }

#endif // VX_APP_VIDEO_ENABLED

    // pump events for other subsystems
    pump_events_maintenance();

    if (push_sentinel)
    {
        data.queue.add_sentinel();
    }
}

//=============================================================================
// edit
//=============================================================================

size_t events_instance::add_events(const event* e, size_t count)
{
    const size_t n = data.queue.add(e, count);

    // if we happen to be blocking in another thread, send a
    // wakeup event to signal that an event was added to the queue

#if defined(VX_APP_VIDEO_ENABLED)

    if (n && app->is_video_init())
    {
        video_ptr->send_wakeup_event();
    }

#endif // VX_APP_VIDEO_ENABLED

    return n;
}

//=============================================================================

VX_API size_t match_events(event_filter matcher, void* user_data, event* events, size_t count, bool remove)
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT(0);
    return s_events_ptr->match_events(matcher, user_data, events, count, remove);
}

size_t events_instance::match_events(event_filter matcher, void* user_data, event* events, size_t count, bool remove)
{
    return data.queue.match(matcher, user_data, events, count, remove);
}

//=============================================================================

size_t events_instance::flush_events(event_type type)
{
    return match_events(type_matcher, &type, nullptr, 0, true);
}

//=============================================================================
// poll
//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_events.c#L1506

time::time_point events_instance::get_polling_interval() const
{
    time::time_point interval = time::max();

    return interval;
}

//=============================================================================

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
        const bool res = data.queue.match(nullptr, nullptr, &e, 1, true);

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
        const int result = video_ptr->wait_event_timeout(w, loop_timeout);

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

//=============================================================================

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
    const bool res = data.queue.match(nullptr, nullptr, &e, 1, true);

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
        video::window_id w = video_ptr->get_active_window();
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

        if (data.queue.match(nullptr, nullptr, &e, 1, true))
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

//=============================================================================

VX_API bool poll_event(event& e)
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT(false);
    return s_events_ptr->poll_event(e);
}

bool events_instance::poll_event(event& e)
{
    return wait_event_timeout(e, time::zero());
}

//=============================================================================
// push
//=============================================================================

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

//=============================================================================
// watch
//=============================================================================

VX_API void set_event_filter(event_filter filter, void* user_data)
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT_VOID();
    s_events_ptr->set_event_filter(filter, user_data, event_watch_priority_normal);
}

void events_instance::set_event_filter(event_filter filter, void* user_data, event_watch_priority priority)
{
    data.watch.set_filter(filter, user_data, priority);
    // filter current events
    data.queue.match(filter, user_data, nullptr, 0, true);
}

//=============================================================================

VX_API void get_event_filter(event_filter& filter, void*& user_data)
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT_VOID();
    s_events_ptr->get_event_filter(filter, user_data, event_watch_priority_normal);
}

void events_instance::get_event_filter(event_filter& filter, void*& user_data, event_watch_priority priority)
{
    data.watch.get_filter(filter, user_data, priority);
}

//=============================================================================

VX_API void add_event_watch(event_filter callback, void* user_data)
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT_VOID();
    s_events_ptr->add_event_watch(callback, user_data, event_watch_priority_normal);
}

void events_instance::add_event_watch(event_filter callback, void* user_data, event_watch_priority priority)
{
    data.watch.add_watch(callback, user_data, priority);
}

//=============================================================================

VX_API void remove_event_watch(event_filter callback, void* user_data)
{
    VX_CHECK_EVENTS_SUBSYSTEM_INIT_VOID();
    s_events_ptr->remove_event_watch(callback, user_data, event_watch_priority_normal);
}

void events_instance::remove_event_watch(event_filter callback, void* user_data, event_watch_priority priority)
{
    data.watch.remove_watch(callback, user_data, priority);
}

//=============================================================================

bool events_instance::dispatch_event_watch(const event& e)
{
    return data.watch.dispatch_all(e);
}

//=============================================================================
// events
//=============================================================================

void events_instance::send_critical_event(event_type type)
{
    event e{};
    e.type = type;

#if VX_APP_LOG_EVENTS
    log_event(e);
#endif // VX_APP_LOG_EVENTS

    // needs to be handled in call stack by event watcher
    dispatch_event_watch(e);
}

//=============================================================================
// drop events
//=============================================================================

bool events_instance::send_drop_event(const window_ptr_type w, const event_type type, const char* source, const char* drop_data, float x, float y)
{
#if defined(VX_APP_VIDEO_ENABLED)
#   define set_drop_event_window_id(w, e) e.drop_event.common.window_id = w ? w->data.id : invalid_id
#else
#   define set_drop_event_window_id(w, e)
#endif

    bool& is_dropping_ref = 
#if defined(VX_APP_VIDEO_ENABLED)
        w ? w->data.is_dropping :
#endif // VX_APP_VIDEO_ENABLED
        data.drop.is_dropping;

    if (!is_dropping_ref)
    {
        event e{};
        e.type = drop_begin;
        set_drop_event_window_id(w, e);

        if (!push_event(e))
        {
            return false;
        }
    }

    event e{};
    e.type = type;
    set_drop_event_window_id(w, e);

    const char* event_source = nullptr;
    const char* event_data = nullptr;

    if (source && *source)
    {
        event_source = create_temporary_string(source);
        if (!event_source)
        {
            return false;
        }
    }

    if (drop_data && *drop_data)
    {
        event_data = create_temporary_string(drop_data);
        if (!event_data)
        {
            return false;
        }
    }

    switch (type)
    {
        case drop_file:
        {
            e.drop_event.drop_file.source = event_source;
            e.drop_event.drop_file.file = event_data;
            break;
        }
        case drop_text:
        {
            e.drop_event.drop_text.text = event_data;
            break;
        }
        case drop_position:
        {
            data.drop.last_x = x;
            data.drop.last_y = y;
            break;
        }
        default:
        {
            break;
        }
    }

    e.drop_event.common.x = data.drop.last_x;
    e.drop_event.common.y = data.drop.last_y;

    if (!push_event(e))
    {
        return false;
    }

    if (type == drop_complete)
    {
        is_dropping_ref = false;

        data.drop.last_x = 0;
        data.drop.last_y = 0;
    }

    return true;

#   undef set_drop_event_window_id
}

//=============================================================================

bool events_instance::send_drop_file(const window_ptr_type w, const char* source, const char* file)
{
    return send_drop_event(w, drop_file, source, file, 0, 0);
}

//=============================================================================

bool events_instance::send_drop_position(const window_ptr_type w, float x, float y)
{
    return send_drop_event(w, drop_position, nullptr, nullptr, x, y);
}

//=============================================================================

bool events_instance::send_drop_text(const window_ptr_type w, const char* text)
{
    return send_drop_event(w, drop_text, nullptr, text, 0, 0);
}

//=============================================================================

bool events_instance::send_drop_complete(const window_ptr_type w)
{
    return send_drop_event(w, drop_complete, nullptr, nullptr, 0, 0);
}

//=============================================================================
// event logging
//=============================================================================

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
                " display_id: ", e.display_event.common.display_id,
                " }"
            );
            break;
        }
        case display_removed:
        {
            VX_LOG_INFO(
                "display_removed {",
                " time: ", e.time.as_nanoseconds(),
                " display_id: ", e.display_event.common.display_id,
                " }"
            );
            break;
        }
        case display_moved:
        {
            VX_LOG_INFO(
                "display_moved {",
                " time: ", e.time.as_nanoseconds(),
                " display_id: ", e.display_event.common.display_id,
                " }"
            );
            break;
        }
        case display_orientation_changed:
        {
            VX_LOG_INFO(
                "display_orientation_changed {",
                " time: ", e.time.as_nanoseconds(),
                " display_id: ", e.display_event.common.display_id, ',',
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
                " display_id: ", e.display_event.common.display_id,
                " }"
            );
            break;
        }
        case display_current_mode_changed:
        {
            VX_LOG_INFO(
                "display_current_mode_changed {",
                " time: ", e.time.as_nanoseconds(),
                " display_id: ", e.display_event.common.display_id,
                " }"
            );
            break;
        }
        case display_content_scale_changed:
        {
            VX_LOG_INFO(
                "display_content_scale_changed {",
                " time: ", e.time.as_nanoseconds(),
                " display_id: ", e.display_event.common.display_id, ',',
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