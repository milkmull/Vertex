#include "vertex_impl/app/event/_platform/windows/windows_event.hpp"

namespace vx {
namespace app {
namespace event {

// https://github.com/libsdl-org/SDL/blob/e699f3dca1c03051a2736cde6fb6d45839f33062/src/video/windows/SDL_windowsevents.c#L2487

void pump_events_impl(bool process_all)
{

}

void wait_events_impl()
{
    ::WaitMessage();
    pump_events_impl(false);
}

void wait_events_timeout_impl(unsigned int timeout_ms)
{
    ::MsgWaitForMultipleObjects(0, NULL, FALSE, static_cast<DWORD>(timeout_ms), QS_ALLINPUT);
    pump_events_impl(false);
}

///////////////////////////////////////////////////////////////////////////////
// event process
///////////////////////////////////////////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/main/src/video/windows/SDL_windowsevents.c#L1150C1-L1150C83

LRESULT CALLBACK window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    return CallWindowProc(DefWindowProc, hWnd, Msg, wParam, lParam);
}

} // namespace event
} // namespace app
} // namespace vx