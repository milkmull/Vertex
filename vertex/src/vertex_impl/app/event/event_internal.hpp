#pragma once

#include "vertex/app/event/event.hpp"
#include "vertex/os/mutex.hpp"

#define VX_APP_LOG_EVENTS 1

#if defined(VX_APP_LOG_EVENTS)
#   include "vertex/system/log.hpp"
#endif // VX_APP_LOG_EVENTS

namespace vx {
namespace app {
namespace event {

///////////////////////////////////////////////////////////////////////////////
// instance
///////////////////////////////////////////////////////////////////////////////

class events_instance_impl;

class events_instance
{
private:

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    enum
    {
        DEFAULT_EVENT_BUFFER_SIZE = 64,
        DEFAULT_EVENT_BUFFER_GROW_FACTOR = 2
    };

    struct ring_buffer
    {
        std::vector<event> events;
        size_t head = 0;           // read index
        size_t tail = 0;           // write index
        size_t count = 0;          // number of events in queue
        mutable os::mutex mutex;

        void ensure_capacity(size_t new_capacity);
        bool post(const event& e);
        bool poll(event& e);
    };

    struct event_data
    {
        ring_buffer ring_buffer;
        event_process_callback process_callback = nullptr;
    };

    event_data data;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // initialization
    ///////////////////////////////////////////////////////////////////////////////

    bool init();
    void quit();

    ///////////////////////////////////////////////////////////////////////////////
    // processing
    ///////////////////////////////////////////////////////////////////////////////

    void pump_events(bool process_all);

    bool post_event(const event& e);
    bool poll_event(event& e);

    void wait_events();
    void wait_events_timeout(unsigned int timeout_ms);

    size_t flush_events(event_type type);
    size_t filter_events(event_filter filter, void* user_data);

    ///////////////////////////////////////////////////////////////////////////////
    // callback
    ///////////////////////////////////////////////////////////////////////////////

    void set_event_process_callback(event_process_callback callback);

    ///////////////////////////////////////////////////////////////////////////////
    // checking
    ///////////////////////////////////////////////////////////////////////////////

    bool has_event(event_type type) const;
    size_t event_count() const;
    std::vector<event> get_events() const;
};

} // namespace event
} // namespace app
} // namespace vx