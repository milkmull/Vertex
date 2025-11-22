#include "vertex/config/flags.hpp"
#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/hints/hints_internal.hpp"
#include "vertex_impl/app/event/event_internal.hpp"

#if defined(HAVE_SIGNAL_H)
#   include <signal.h>
#endif // HAVE_SIGNAL_H

#if defined(HAVE_SIGNAL_H) || defined(HAVE_SIGACTION)
#   define HAVE_SIGNAL_SUPPORT
#endif

namespace vx {
namespace app {
namespace event {

///////////////////////////////////////////////////////////////////////////////
// signal flags
///////////////////////////////////////////////////////////////////////////////

VX_FLAGS_DECLARE_BEGIN(signal_flags)
{
    none                        = 0,
    disable_signals             = VX_BIT(0),
    send_quit_pending           = VX_BIT(1),
    send_background_pending     = VX_BIT(2),
    send_foreground_pending     = VX_BIT(3)
}
VX_FLAGS_DECLARE_END(signal_flags)

///////////////////////////////////////////////////////////////////////////////

#if defined(HAVE_SIGNAL_SUPPORT)

static signal_flags sig_state = signal_flags::none;

static void handle_sig(int sig)
{
    VX_UNUSED(::signal(sig, handle_sig));

    // Send a quit event next time the event loop pumps.
    // We can't send it in signal handler; SDL_malloc() might be interrupted!
    if ((sig == SIGINT) || (sig == SIGTERM))
    {
        sig_state |= signal_flags::send_quit_pending;
    }

#if defined(VX_BACKGROUNDING_SIGNAL)

    else if (sig == VX_BACKGROUNDING_SIGNAL)
    {
        sig_state |= signal_flags::send_background_pending;
    }

#endif // VX_BACKGROUNDING_SIGNAL

#if defined(VX_FOREGROUNDING_SIGNAL)

    else if (sig == VX_FOREGROUNDING_SIGNAL)
    {
        sig_state |= signal_flags::send_foreground_pending;
    }

#endif // VX_FOREGROUNDING_SIGNAL
}

static void signal_init(const int sig)
{
#if defined(HAVE_SIGACTION)

    struct sigacttion action {};
    ::sigaction(sig, NULL, &action);

    if (action.sa_handler == SIG_DFL
#if defined(HAVE_SA_SIGACTION)
        && (void (*)(int))action.sa_sigaction == SIG_DFL
#endif
        )
    {
        action.sa_handler = handle_sig;
        sigaction(sig, &action, NULL);
    }

#elif defined(HAVE_SIGNAL_H)

    void (*ohandler)(int) = ::signal(sig, handle_sig);
    if (ohandler != SIG_DFL)
    {
        ::signal(sig, ohandler);
    }

#endif // HAVE_SIGACTION
}

static void signal_quit(const int sig)
{
#if defined(HAVE_SIGACTION)

    struct sigaction action {};
    ::sigaction(sig, NULL, &action);

    if (action.sa_handler == handle_sig)
    {
        action.sa_handler = SIG_DFL;
        ::sigaction(sig, &action, NULL);
    }

#elif defined(HAVE_SIGNAL_H)

    void (*ohandler)(int) = ::signal(sig, SIG_DFL);
    if (ohandler != handle_sig)
    {
        ::signal(sig, ohandler);
    }

#endif // HAVE_SIGACTION
}

static bool init_internal()
{
    // Both SIGINT and SIGTERM are translated into quit interrupts
    // and SDL can be built to simulate iOS/Android semantics with arbitrary signals.
    signal_init(SIGINT);
    signal_init(SIGTERM);

#if defined(VX_BACKGROUNDING_SIGNAL)
    signal_init(VX_BACKGROUNDING_SIGNAL);
#endif // VX_BACKGROUNDING_SIGNAL

#if defined(VX_FOREGROUNDING_SIGNAL)
    signal_init(VX_FOREGROUNDING_SIGNAL);
#endif // VX_FOREGROUNDING_SIGNAL

    return true;
}

static void quit_internal()
{
    signal_quit(SIGINT);
    signal_quit(SIGTERM);

#if defined(VX_BACKGROUNDING_SIGNAL)
    signal_quit(VX_BACKGROUNDING_SIGNAL);
#endif // VX_BACKGROUNDING_SIGNAL

#if defined(VX_FOREGROUNDING_SIGNAL)
    signal_quit(VX_FOREGROUNDING_SIGNAL);
#endif // VX_FOREGROUNDING_SIGNAL
}

#endif // HAVE_SIGNAL_SUPPORT

///////////////////////////////////////////////////////////////////////////////

bool events_instance::init_signal_handler()
{
#if defined(HAVE_SIGNAL_SUPPORT)

    if (app->data.hints_ptr->get_hint_boolean(hint::app_no_signal_handlers, false))
    {
        return init_internal();
    }

#endif // HAVE_SIGNAL_SUPPORT

    return true;
}

void events_instance::quit_signal_handler()
{
#if defined(HAVE_SIGNAL_SUPPORT)

    if (!(sig_state & signal_flags::disable_signals))
    {
        quit_internal();
    }

#endif // HAVE_SIGNAL_SUPPORT
}

void events_instance::send_pending_signal_events()
{
#if defined(HAVE_SIGNAL_SUPPORT)

    if (sig_state & signal_flags::send_quit_pending)
    {
        send_quit();
        VX_ASSERT(!(sig_state & signal_flags::send_quit_pending));
    }

#if defined(VX_BACKGROUNDING_SIGNAL)

    if (sig_state & signal_flags::send_background_pending)
    {
        sig_state &= ~signal_flags::send_background_pending;
        app->will_enter_background();
    }

#endif // VX_BACKGROUNDING_SIGNAL

#if defined(VX_FOREGROUNDING_SIGNAL)

    if (sig_state & signal_flags::send_foreground_pending)
    {
        sig_state &= ~signal_flags::send_foreground_pending;
        app->did_enter_foreground();
    }

#endif // VX_FOREGROUNDING_SIGNAL

#endif // HAVE_SIGNAL_SUPPORT
}

///////////////////////////////////////////////////////////////////////////////

void events_instance::send_quit()
{
#if defined(HAVE_SIGNAL_SUPPORT)

    sig_state &= ~signal_flags::send_quit_pending;

#endif // HAVE_SIGNAL_SUPPORT

    event e{ app_quit };
    push_event(e);
}

} // namespace event
} // namespace app
} // namespace vx