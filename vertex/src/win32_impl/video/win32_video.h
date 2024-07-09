#pragma once

#include "win32_impl/win32_header.h"
#include "vertex_impl/app/video/video.h"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// user32.dll
///////////////////////////////////////////////////////////////////////////////

struct user32
{
    HMODULE dll;

    BOOL(WINAPI* SetProcessDPIAware)(void);
    BOOL(WINAPI* SetProcessDpiAwarenessContext)(DPI_AWARENESS_CONTEXT);
    DPI_AWARENESS_CONTEXT(WINAPI* SetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT);
    DPI_AWARENESS_CONTEXT(WINAPI* GetThreadDpiAwarenessContext)(void);
    DPI_AWARENESS(WINAPI* GetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT);
    BOOL(WINAPI* EnableNonClientDpiScaling)(HWND);
    BOOL(WINAPI* AdjustWindowRectExForDpi)(LPRECT, DWORD, BOOL, DWORD, UINT);
    UINT(WINAPI* GetDpiForWindow)(HWND);
    BOOL(WINAPI* AreDpiAwarenessContextsEqual)(DPI_AWARENESS_CONTEXT, DPI_AWARENESS_CONTEXT);
    BOOL(WINAPI* IsValidDpiAwarenessContext)(DPI_AWARENESS_CONTEXT);
};

///////////////////////////////////////////////////////////////////////////////
// shcore.dll
///////////////////////////////////////////////////////////////////////////////

struct shcore
{
    HMODULE dll;

    HRESULT(WINAPI* GetDpiForMonitor)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
    HRESULT(WINAPI* SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);
};

///////////////////////////////////////////////////////////////////////////////
// display_mode
///////////////////////////////////////////////////////////////////////////////

struct display_mode_driver_data
{
    DEVMODE devmode;
};

class display_mode::display_mode_impl
{
public:

    display_mode_driver_data driver_data = display_mode_driver_data{};
};

///////////////////////////////////////////////////////////////////////////////
// display
///////////////////////////////////////////////////////////////////////////////

struct display_state
{
    using type = uint32_t;

    static const type NONE = 0;
    static const type ADDED = (1 << 0);
    static const type REMOVED = (1 << 1);
    static const type MOVED = (1 << 2);
    static const type ORIENTATION_CHANGED = (1 << 3);
};

struct display_driver_data
{
    HMONITOR handle;
    std::wstring device_name;
    display_state::type state;
    math::recti bounds;
};

class display::display_impl
{
public:

    static bool create_display(size_t& index, std::vector<std::unique_ptr<display>>& displays, HMONITOR hMonitor, const MONITORINFOEX* info);
    static void update_displays(std::vector<std::unique_ptr<display>>& displays);
    static BOOL CALLBACK enum_displays_callback(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam);

    static bool get_display_mode(const std::wstring& device_name, display_mode& mode, DWORD index, display_orientation* orientation);

public:

    void list_display_modes();
    bool set_display_mode(display_mode& mode) const;

    bool get_bounds(math::recti& bounds) const;
    bool get_work_area(math::recti& work_area) const;

public:

    display_data data = display_data{};
    display_driver_data driver_data = display_driver_data{};

};

///////////////////////////////////////////////////////////////////////////////
// video
///////////////////////////////////////////////////////////////////////////////

struct video_driver_data
{
    // library stuff

    HMODULE instance;

    user32 user32;
    shcore shcore;

    // window stuff

    static const LPCWSTR window_class_name;
    HICON window_default_icon;
    ATOM window_class;

    // cursor stuff

    HCURSOR blank_cursor;
};

namespace video_impl {

    bool init();
    void quit();

    bool set_dpi_awareness(process_dpi_awareness awareness);
    process_dpi_awareness get_dpi_awareness();
}

extern video_driver_data s_driver_data;

}
}
}