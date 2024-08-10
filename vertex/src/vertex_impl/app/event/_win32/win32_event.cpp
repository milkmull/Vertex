#include "win32_event.h"

namespace vx {
namespace app {

void event::event_impl::pump_events(bool process_all)
{
    MSG msg;
    
    ULONGLONG end_ticks = GetTickCount64() + 1;
    constexpr size_t max_new_messages = 3;
    size_t new_messages = 0;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // Don't sit in here for too long if there are a ton of messages
        if (!process_all && (end_ticks - static_cast<decltype(end_ticks)>(msg.time)) <= 0)
        {
            // We may get some messages from other application hooks which are
            // processed before any input events. We allow some leeway here so
            // we can continue processing into some of the input messages.
            if (++new_messages > max_new_messages)
            {
                break;
            }
        }
    }
}

void event::event_impl::wait_events()
{
    WaitMessage();
    pump_events(false);
}

void event::event_impl::wait_events_timeout(unsigned int timeout_ms)
{
    MsgWaitForMultipleObjects(0, NULL, FALSE, static_cast<DWORD>(timeout_ms), QS_ALLINPUT);
    pump_events(false);
}

}
}