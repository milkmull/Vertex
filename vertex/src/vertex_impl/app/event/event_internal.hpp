#pragma once

#include <list>

#include "vertex/app/event/event.hpp"
#include "vertex/os/mutex.hpp"

namespace vx {
namespace app {
namespace event {

///////////////////////////////////////////////////////////////////////////////
// event data
///////////////////////////////////////////////////////////////////////////////

enum
{
    MAX_EVENTS = 65535,
    DEFAULT_POLL_INTERVAL_MS = 1
};

////////////////////////////////////////

struct event_queue
{
    std::list<event> queue;
    using iterator_type = typename decltype(queue)::iterator;
    os::recursive_mutex mutex;

    size_t add(const event* e, size_t count);
    size_t match(event_filter matcher, void* user_data, event* events, size_t count, bool remove);
    bool poll(event& e);

    os::atomic<size_t> sentinel_pending = 0;       // incremented every time an event poll ends
    void add_sentinel();
};

////////////////////////////////////////

struct event_watcher
{
    event_filter callback;
    void* user_data;
    bool removed;
};

struct event_watch_list
{
    os::recursive_mutex mutex;
    event_watcher filter;
    std::vector<event_watcher> watchers;
    bool dispatching;
    bool removed;
};

////////////////////////////////////////

struct event_data
{
    event_queue queue;
    event_watch_list watch;
    uint8_t refcount = 0;
};

///////////////////////////////////////////////////////////////////////////////
// event_internal
///////////////////////////////////////////////////////////////////////////////

class event_internal
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // initialization
    ///////////////////////////////////////////////////////////////////////////////

    static bool init();
    static bool is_init();
    static void quit();

    ///////////////////////////////////////////////////////////////////////////////
    // processing
    ///////////////////////////////////////////////////////////////////////////////

    static void pump_events(bool process_all);
    static void pump_events_maintenance();

    static size_t add_events(const event* e, size_t count);
    static size_t match_events(event_filter matcher, void* user_data, event* events, size_t count, bool remove);

    static time::time_point get_polling_interval();
    static int wait_event_timeout_video(video::window* w, event& e, time::time_point t, time::time_point start);
    static bool wait_event_timeout(event& e, time::time_point t);

    static bool push_event(const event& e);
    static bool poll_event(event& e);

    static void set_event_filter(event_filter filter, void* user_data);
    static void get_event_filter(event_filter& filter, void*& user_data);

    static void add_event_watch(event_filter callback, void* user_data);
    static void remove_event_watch(event_filter callback, void* user_data);
    static bool dispatch_event_watch(const event& e);
};

} // namespace event
} // namespace app
} // namespace vx