#pragma once

#include <list>

#include "vertex_impl/app/video/_platform/platform_features.hpp"
#include "vertex_impl/app/event/event_watch.hpp"

namespace vx {
namespace app {

class app_instance;

namespace event {

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

enum
{
    max_events = 65535,
    default_poll_interval_ms = 1,
    disabled_events_size = 256
};

///////////////////////////////////////////////////////////////////////////////
// memory
///////////////////////////////////////////////////////////////////////////////

void* allocate_temporary_memory_raw(size_t size);
const char* create_temporary_string(const char* src);

template <typename T>
inline T allocate_temporary_memory(size_t size)
{
    return static_cast<T>(allocate_temporary_memory_raw(size));
}

template <typename T>
inline T* create_temporary_instance()
{
    return allocate_temporary_memory<T*>(sizeof(T));
}

template <typename T>
inline T* create_temporary_array(size_t count)
{
    const size_t total_size = sizeof(T) * count;
    return allocate_temporary_memory<T*>(total_size);
}

const void* release_temporary_memory(const void* ptr);
void give_temporary_memory(void* ptr);

///////////////////////////////////////////////////////////////////////////////
// event queue
///////////////////////////////////////////////////////////////////////////////

struct event_queue_entry
{
    event event;
    struct temp_memory_entry* memory;
};

struct event_queue
{
    bool active;
    std::list<event_queue_entry> queue;
    os::recursive_mutex mutex;

    bool start();
    void stop();

    size_t add(const event* e, size_t count);
    size_t match(event_filter matcher, void* user_data, event* events, size_t count, bool remove);
    bool poll(event& e);
    void clear();

    os::atomic<size_t> sentinel_pending = 0; // incremented every time an event poll ends
    void add_sentinel();
};

///////////////////////////////////////////////////////////////////////////////
// event data
///////////////////////////////////////////////////////////////////////////////

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

    void init_data();

    bool init(app_instance* owner);
    void quit();

    ///////////////////////////////////////////////////////////////////////////////
    // signal
    ///////////////////////////////////////////////////////////////////////////////

    bool init_signal_handler();
    void quit_signal_handler();
    void send_pending_signal_events();
    void send_quit();

    ///////////////////////////////////////////////////////////////////////////////
    // loop
    ///////////////////////////////////////////////////////////////////////////////

    bool start_loop();
    void stop_loop();

    ///////////////////////////////////////////////////////////////////////////////
    // pump
    ///////////////////////////////////////////////////////////////////////////////

    void pump_events();
    void pump_events_internal(bool push_sentinel);
    void pump_events_maintenance();

    ///////////////////////////////////////////////////////////////////////////////
    // edit
    ///////////////////////////////////////////////////////////////////////////////

    size_t add_events(const event* e, size_t count);
    size_t match_events(event_filter matcher, void* user_data, event* events, size_t count, bool remove);
    size_t flush_events(event_type type);

    ///////////////////////////////////////////////////////////////////////////////
    // poll
    ///////////////////////////////////////////////////////////////////////////////

    time::time_point get_polling_interval() const;

#if VX_EVENT_HAVE_WAIT_VIDEO_SUBSYSTEM
    int wait_event_timeout_video(video::window_id w, event& e, time::time_point t, time::time_point start);
#endif // VX_EVENT_HAVE_WAIT_VIDEO_SUBSYSTEM
    bool wait_event_timeout(event& e, time::time_point t);

    bool poll_event(event& e);

    ///////////////////////////////////////////////////////////////////////////////
    // push
    ///////////////////////////////////////////////////////////////////////////////

    bool push_event(event& e);
    bool push_event_filtered(event& e, event_filter filter, void* user_data);

    ///////////////////////////////////////////////////////////////////////////////
    // watch
    ///////////////////////////////////////////////////////////////////////////////

    void set_event_filter(event_filter filter, void* user_data, event_watch_priority priority);
    void get_event_filter(event_filter& filter, void*& user_data, event_watch_priority priority);

    void add_event_watch(event_filter callback, void* user_data, event_watch_priority priority);
    void remove_event_watch(event_filter callback, void* user_data, event_watch_priority priority);
    bool dispatch_event_watch(const event& e);

    ///////////////////////////////////////////////////////////////////////////////
    // events
    ///////////////////////////////////////////////////////////////////////////////

    void send_critical_event(event_type type);

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    app_instance* app = nullptr;
    events_data data;
};

} // namespace event
} // namespace app
} // namespace vx