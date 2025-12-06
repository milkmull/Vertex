#include "vertex/app/event/event.hpp"
#include "vertex/app/hints/hints.hpp"
#include "vertex/util/time/time.hpp"
#include "vertex_impl/app/main/main_internal.hpp"

namespace vx {
namespace app {

struct runner_impl_data
{
    time::time_point callback_rate_increment;
    bool iterate_after_wait_event = false;
};

class runner_instance_impl
{
public:

    runner_instance_impl() = default;
    ~runner_instance_impl() = default;

    runner_instance_impl(const runner_instance_impl&) = delete;
    runner_instance_impl& operator=(const runner_instance_impl&) = delete;

    runner_instance_impl(runner_instance_impl&&) noexcept = delete;
    runner_instance_impl& operator=(runner_instance_impl&&) noexcept = delete;

public:

    app_result init(
        int argc,
        char* argv[],
        init_callback_t init_fn,
        iterate_callback_t iterate_fn,
        event_callback_t event_fn,
        quit_callback_t quit_fn)
    {
        return base.init(argc, argv, init_fn, iterate_fn, event_fn, quit_fn);
    }

    void quit(app_result result)
    {
        base.quit(result);
    }

    app_result iterate()
    {
        if (data.iterate_after_wait_event)
        {
            event::wait_event(nullptr);
        }

        return base.iterate(!data.iterate_after_wait_event);
    }

    void dispatch_events()
    {
        base.dispatch_events();
    }

    void dispatch_event(event::event& e)
    {
        base.dispatch_event(e);
    }

public:

    runner_instance base;
    runner_impl_data data;
};

//=============================================================================
// hints
//=============================================================================

static void rate_hint_watcher(const hint::hint_t, const char*, const char* new_value, void* user_data)
{
    runner_instance_impl* this_ = static_cast<runner_instance_impl*>(user_data);

    this_->data.iterate_after_wait_event = new_value && (std::strcmp(new_value, "wait_event") == 0);

    if (this_->data.iterate_after_wait_event)
    {
        this_->data.callback_rate_increment.zero();
    }
    else
    {
        const double callback_rate = hint::parse_float(new_value, 0.0);

        if (callback_rate > 0.0)
        {
            this_->data.callback_rate_increment = time::nanoseconds(static_cast<int64_t>(
                static_cast<double>(time::nanoseconds_per_second) / callback_rate));
        }
        else
        {
            this_->data.callback_rate_increment.zero();
        }
    }
}

int _priv::enter_callbacks(
    int argc,
    char* argv[],
    init_callback_t init_fn,
    iterate_callback_t iterate_fn,
    event_callback_t event_fn,
    quit_callback_t quit_fn)
{
    runner_instance_impl runner;

    app_result result = runner.init(argc, argv, init_fn, iterate_fn, event_fn, quit_fn);
    if (result == app_result::continue_)
    {
        hint::add_hint_callback(hint::app_main_callback_rate, rate_hint_watcher, &runner);

        time::time_point next_iteration;
        if (runner.data.callback_rate_increment.is_positive())
        {
            next_iteration = os::get_ticks() + runner.data.callback_rate_increment;
        }

        while ((result = runner.iterate()) == app_result::continue_)
        {
            // Try to run at whatever rate the hint requested. This makes this
            // not eat all the CPU in simple things like loopwave. By
            // default, we run as fast as possible, which means we'll clamp to
            // vsync in common cases, and won't be restrained to vsync if the
            // app is doing a benchmark or doesn't want to be, based on how
            // they've set up that window.

            if (runner.data.callback_rate_increment.is_zero())
            {
                // just clear the timer and run at the pace the video subsystem allows.
                next_iteration.zero();
            }
            else
            {
                const time::time_point now = os::get_ticks();
                if (next_iteration > now)
                {
                    // Running faster than the limit, sleep a little.
                    os::sleep_precise(next_iteration - now);
                }
                else
                {
                    // if running behind, reset the timer. If right on time, `next_iteration` already equals `now`.
                    next_iteration = now;
                }

                next_iteration += runner.data.callback_rate_increment;
            }
        }

        hint::remove_hint_callback(hint::app_main_callback_rate, rate_hint_watcher, &runner);
    }

    runner.quit(result);
    return (result == app_result::terminate_failure) ? 1 : 0;
}

} // namespace app
} // namespace vx