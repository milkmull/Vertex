#include "vertex_impl/app/event/event_watch.hpp"

namespace vx {
namespace app {
namespace event {

void watch_pair::clear()
{
    filter.callback = nullptr;
    filter.user_data = nullptr;
    filter.removed = false;

    watchers.clear();
}

////////////////////////////////////////

void event_watch_list::clear()
{
    os::lock_guard lock(mutex);

    for (auto& pair : watch_list)
    {
        pair.clear();
    }

    dispatching = false;
    removed = false;
}

////////////////////////////////////////

void event_watch_list::set_filter(event_filter f, void* user_data, event_watch_priority priority)
{
    os::lock_guard lock(mutex);

    watch_list[priority].filter.callback = f;
    watch_list[priority].filter.user_data = user_data;
}

////////////////////////////////////////

void event_watch_list::get_filter(event_filter& f, void*& user_data, event_watch_priority priority) const
{
    os::lock_guard lock(mutex);

    f = watch_list[priority].filter.callback;
    user_data = watch_list[priority].filter.user_data;
}

////////////////////////////////////////

void event_watch_list::add_watch(event_filter callback, void* user_data, event_watch_priority priority)
{
    os::lock_guard lock(mutex);
    watch_list[priority].watchers.push_back(event_watcher{ callback, user_data, false });
}

////////////////////////////////////////

void event_watch_list::remove_watch(event_filter callback, void* user_data, event_watch_priority priority)
{
    os::lock_guard lock(mutex);

    auto& watchers = watch_list[priority].watchers;

    for (auto it = watchers.begin(); it != watchers.end(); ++it)
    {
        if (it->callback == callback && it->user_data == user_data)
        {
            if (!dispatching)
            {
                watchers.erase(it);
            }
            else
            {
                it->removed = true;
                removed = true;
            }

            break;
        }
    }
}

////////////////////////////////////////

void event_watch_list::prune_removed_watchers(event_watch_priority priority)
{
    auto& watchers = watch_list[priority].watchers;

    for (auto it = watchers.begin(); it != watchers.end();)
    {
        if (it->removed)
        {
            it = watchers.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

////////////////////////////////////////

bool event_watch_list::dispatch(event& e, event_watch_priority priority)
{
    if (e.type == internal_event_poll_sentinel)
    {
        return true;
    }

    os::lock_guard lock(mutex);

    const auto& filter = watch_list[priority].filter;
    const auto& watchers = watch_list[priority].watchers;

    if (!filter.callback && watchers.empty())
    {
        // nothing to do
        return true;
    }

    if (filter.callback && !filter.callback(e, filter.user_data))
    {
        // event got filtered out
        return false;
    }

    if (watchers.empty())
    {
        // early exit
        return true;
    }

    dispatching = true;

    for (size_t i = 0; i < watchers.size(); ++i)
    {
        auto& watcher = watchers[i];

        // only call the watcher if it was not removed in a callback
        if (!watcher.removed)
        {
            watcher.callback(e, watcher.user_data);
        }
    }

    dispatching = false;

    // remove any watchers removed in callbacks
    if (removed)
    {
        prune_removed_watchers(priority);
        removed = false;
    }

    return true;
}

////////////////////////////////////////

bool event_watch_list::dispatch_all(event& e)
{
    if (!dispatch(e, event_watch_priority_early))
    {
        return false;
    }

    return dispatch(e, event_watch_priority_normal);
}

} // namespace event
} // namespace app
} // namespace vx
