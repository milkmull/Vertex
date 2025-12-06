#include "vertex/app/app.hpp"
#include "vertex/app/event/event.hpp"
#include "vertex/util/memory/memory.hpp"
#include "vertex_impl/app/main/main_internal.hpp"

namespace vx {
namespace app {

// Return true if this event needs to be processed before returning from the event watcher
static bool should_dispatch_immediately(event::event_type_t type)
{
    switch (type)
    {
        case event::app_terminating:
        case event::app_low_memory:
        case event::app_will_enter_background:
        case event::app_did_enter_background:
        case event::app_will_enter_foreground:
        case event::app_did_enter_foreground:
        {
            return true;
        }
        default:
        {
            return false;
        }
    }
}

static bool main_callback_event_watcher(event::event& e, void* user_data)
{
    runner_instance* runner = static_cast<runner_instance*>(user_data);
    return runner->event_watch(e);
}

app_result runner_instance::init(
    int argc,
    char* argv[],
    init_callback_t init_fn,
    iterate_callback_t iterate_fn,
    event_callback_t event_fn,
    quit_callback_t quit_fn)
{
    data.iterate_fn = iterate_fn;
    data.event_fn = event_fn;
    data.quit_fn = quit_fn;

    data.result = app_result::continue_;

    if (!app::init())
    {
        data.result = app_result::terminate_failure;
        return app_result::terminate_failure;
    }

    app_result result = init_fn(&data.app_state, argc, argv);
    app_result expected = app_result::continue_;

    if (data.result.compare_exchange_strong(expected, result) && result == app_result::continue_)
    {
        // make sure we definitely have events initialized, even if the app didn't do it
        if (!app::init_subsystem(app::init_flags::events))
        {
            data.result = app_result::terminate_failure;
            return app_result::terminate_failure;
        }

        event::add_event_watch(main_callback_event_watcher, nullptr);
    }

    return data.result.load();
}

//=============================================================================

void runner_instance::quit(app_result result)
{
    event::remove_event_watch(main_callback_event_watcher, nullptr);
    data.quit_fn(data.app_state, result);
    data.app_state = nullptr;
    app::quit();
}

//=============================================================================

app_result runner_instance::iterate(bool pump_events)
{
    if (pump_events)
    {
        event::pump_events();
    }

    dispatch_events();

    app_result result = data.result.load();
    if (result == app_result::continue_)
    {
        result = data.iterate_fn(data.app_state);
        app_result expected = app_result::continue_;

        if (!data.result.compare_exchange_strong(expected, app_result::continue_))
        {
            // something else already set a quit result, keep that.
            result = data.result.load();
        }
    }

    return result;
}

//=============================================================================

bool runner_instance::event_watch(event::event& e)
{
    if (should_dispatch_immediately(e.type))
    {
        // Make sure any currently queued events are processed then dispatch this before continuing
        dispatch_events();
        dispatch_event(e);

        // Make sure that we quit if we get a terminating event
        if (e.type == event::app_terminating)
        {
            app_result expected = app_result::continue_;
            data.result.compare_exchange_strong(expected, app_result::terminate_success);
        }
    }
    else
    {
        // We'll process this event later from the main event queue
    }

    return true;
}

void runner_instance::dispatch_event(event::event& e)
{
    if (data.result.load() == app_result::continue_)
    {
        // if already quitting, don't send the event to the app.
        const app_result result = data.event_fn(data.app_state, e);
        app_result expected = app_result::continue_;
        data.result.compare_exchange_strong(expected, result);
    }
}

void runner_instance::dispatch_events()
{
    event::event events[16];

    while (true)
    {
        const size_t count = event::match_events(nullptr, nullptr, events, mem::array_size(events), true);
        if (count == 0)
        {
            break;
        }

        for (size_t i = 0; i < count; ++i)
        {
            event::event& e = events[i];

            if (!should_dispatch_immediately(e.type))
            {
                dispatch_event(e);
            }
        }
    }
}

//=============================================================================

static void check_default_args(int* argc, char*** argv)
{
    if (!*argv)
    {
        static char dummy_rgv_0[] = { 'V', 'e', 'r', 't', 'e', 'x', '_', 'a', 'p', 'p', '\0' };
        static char* argv_dummy[2] = { dummy_rgv_0, NULL };
        *argc = 1;
        *argv = argv_dummy;
    }
}

int call_main_function(int argc, char* argv[], _priv::main_t main_fn)
{
    check_default_args(&argc, &argv);
    return main_fn(argc, argv);
}

} // namespace app
} // namespace vx
