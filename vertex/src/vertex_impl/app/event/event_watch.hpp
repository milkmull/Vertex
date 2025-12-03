#pragma once

#include "vertex/app/event/event.hpp"
#include "vertex/os/mutex.hpp"

namespace vx {
namespace app {
namespace event {

enum event_watch_priority
{
    event_watch_priority_early = 0,
    event_watch_priority_normal = 1,
    event_watch_priority_count = 2
};

struct event_watcher
{
    event_filter callback = nullptr;
    void* user_data = nullptr;
    bool removed = false;
};

struct watch_pair
{
    event_watcher filter;
    std::vector<event_watcher> watchers;
    void clear();
};

struct event_watch_list
{
    mutable os::recursive_mutex mutex;
    watch_pair watch_list[event_watch_priority_count];
    bool dispatching = false;
    bool removed = false;

    void clear();

    void set_filter(event_filter f, void* user_data, event_watch_priority priority);
    void get_filter(event_filter& f, void*& user_data, event_watch_priority priority) const;

    void add_watch(event_filter callback, void* user_data, event_watch_priority priority);
    void remove_watch(event_filter callback, void* user_data, event_watch_priority priority);

    void prune_removed_watchers(event_watch_priority priority);
    bool dispatch(event& e, event_watch_priority priority);
    bool dispatch_all(event& e);
};

} // namespace event
} // namespace app
} // namespace vx