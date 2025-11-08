#pragma once

#include <list>

#include "vertex/app/event/event.hpp"
#include "vertex/os/mutex.hpp"

namespace vx {
namespace app {

class app_instance;

namespace event {

///////////////////////////////////////////////////////////////////////////////
// event data
///////////////////////////////////////////////////////////////////////////////

enum
{
    max_events = 65535,
    default_poll_interval_ms = 1
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

    os::atomic<size_t> sentinel_pending = 0; // incremented every time an event poll ends
    void add_sentinel();
};

////////////////////////////////////////

struct event_watcher
{
    event_filter callback = nullptr;
    void* user_data = nullptr;
    bool removed = false;
};

struct event_watch_list
{
    os::recursive_mutex mutex;
    event_watcher filter;
    std::vector<event_watcher> watchers;
    bool dispatching = false;
    bool removed = false;
};

////////////////////////////////////////

struct events_data
{
    event_queue queue;
    event_watch_list watch;
};

///////////////////////////////////////////////////////////////////////////////
// event_internal
///////////////////////////////////////////////////////////////////////////////

class events_instance
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // initialization
    ///////////////////////////////////////////////////////////////////////////////

    bool init(app_instance* owner);
    void quit();

    ///////////////////////////////////////////////////////////////////////////////
    // processing
    ///////////////////////////////////////////////////////////////////////////////

    void pump_events(bool process_all);
    void pump_events_internal(bool push_sentinel);
    void pump_events_maintenance();

    size_t add_events(const event* e, size_t count);
    size_t match_events(event_filter matcher, void* user_data, event* events, size_t count, bool remove);

    time::time_point get_polling_interval() const;
#if defined(VX_APP_VIDEO_ENABLED)
    int wait_event_timeout_video(video::window_id w, event& e, time::time_point t, time::time_point start);
#endif // VX_APP_VIDEO_ENABLED
    bool wait_event_timeout(event& e, time::time_point t);

    bool push_event(event& e);
    bool push_event_filtered(event& e, event_filter filter, void* user_data);

    bool poll_event(event& e);

    void set_event_filter(event_filter filter, void* user_data);
    void get_event_filter(event_filter& filter, void*& user_data);

    void add_event_watch(event_filter callback, void* user_data);
    void remove_event_watch(event_filter callback, void* user_data);
    void prune_removed_watchers();
    bool dispatch_event_watch(const event& e);

    ///////////////////////////////////////////////////////////////////////////////
    // processing
    ///////////////////////////////////////////////////////////////////////////////

    app_instance* app = nullptr;
    events_data data;
};

} // namespace event
} // namespace app
} // namespace vx