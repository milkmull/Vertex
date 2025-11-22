#pragma once

#include "vertex/app/event/event.hpp"
#include "vertex/os/mutex.hpp"

namespace vx {
namespace app {
namespace event {

struct event_watcher
{
    event_filter callback = nullptr;
    void* user_data = nullptr;
    bool removed = false;
};

struct event_watch_list
{
    mutable os::recursive_mutex mutex;
    event_watcher filter;
    std::vector<event_watcher> watchers;
    bool dispatching = false;
    bool removed = false;

    void clear();

    void set_filter(event_filter f, void* user_data);
    void get_filter(event_filter& f, void*& user_data) const;

    void add_watch(event_filter callback, void* user_data);
    void remove_watch(event_filter callback, void* user_data);

    void prune_removed_watchers();
    bool dispatch(const event& e);
};

} // namespace event
} // namespace app
} // namespace vx