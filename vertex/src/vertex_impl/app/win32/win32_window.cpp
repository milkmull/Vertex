#include "win32_header.h"
#include "vertex/app/display/window.h"

#if defined(VX_BUILD_WINDOW)

namespace vx {
namespace app {

static int s_window_count = 0;
static const wchar_t* s_class_name = L"Vertex_Window";

static bool make_process_dpi_aware()
{
    return SetProcessDPIAware();
}

static void register_window_class()
{
    WNDCLASSW windowClass;
    windowClass.style = 0;
    windowClass.lpfnWndProc = &global_on_event;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandleW(nullptr);
    windowClass.hIcon = nullptr;
    windowClass.hCursor = nullptr;
    windowClass.hbrBackground = nullptr;
    windowClass.lpszMenuName = nullptr;
    windowClass.lpszClassName = s_class_name;
    RegisterClassW(&windowClass);
}

static LRESULT CALLBACK global_on_event(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
}

window::window(const window_specs& specs)
{
    make_process_dpi_aware();

    // if this is our first window, we geniser our window class
    if (s_window_count == 0)
    {
        register_window_class();
    }

    // configure window style
    const DWORD win32_style = WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_SYSMENU;

    // adjust window size to match requested area
    RECT rectangle = { 0, 0, specs.size.x, specs.size.y };
    AdjustWindowRect(&rectangle, win32_style, false);

    // create window
    m_window = CreateWindowW(
        s_class_name,
        specs.title,
        win32_style,
        rectangle.left,
        rectangle.top,
        rectangle.right - rectangle.left,
        rectangle.bottom - rectangle.top,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        this
    );

}

}
}

#endif