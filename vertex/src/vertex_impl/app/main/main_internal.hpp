#pragma once

#define VX_APP_MAIN_NO_IMPL

#include "vertex/app/main.hpp"
#include "vertex/os/atomic.hpp"

namespace vx {
namespace app {

struct app_runner_data
{
    quit_callback_t quit_fn = nullptr;
    iterate_callback_t iterate_fn = nullptr;
    event_callback_t event_fn = nullptr;

    os::atomic<app_result> result;
    void* app_state = nullptr;
};

class app_runner_base
{
public:

    app_runner_base() = default;
    ~app_runner_base() = default;

    app_runner_base(const app_runner_base&) = delete;
    app_runner_base& operator=(const app_runner_base&) = delete;

    app_runner_base(app_runner_base&&) noexcept = delete;
    app_runner_base& operator=(app_runner_base&&) noexcept = delete;

public:

    app_result init(
        int argc,
        char* argv[],
        init_callback_t init_fn,
        iterate_callback_t iterate_fn,
        event_callback_t event_fn,
        quit_callback_t quit_fn);
    void quit(app_result result);
    app_result iterate(bool pump_events);

    bool event_watch(event::event& e);
    void dispatch_events();
    void dispatch_event(event::event& e);

public:

    app_runner_data data;
};

inline constexpr bool using_callbacks() noexcept
{
#if defined(VX_APP_USE_CALLBACKS)
    return true;
#else
    return false;
#endif // VX_APP_USE_CALLBACKS
}

int call_main_function(int argc, char* argv[], _priv::main_t main_fn);

} // namespace app
} // namespace vx
