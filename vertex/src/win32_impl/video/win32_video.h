#pragma once

#include "win32_impl/win32_header.h"
#include "vertex/app/video/video.h"

namespace vx {
namespace app {

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
// driver data
///////////////////////////////////////////////////////////////////////////////

class video::display::display_impl
{
public:

    static bool create_display(size_t index, display& display, HMONITOR hMonitor, const MONITORINFOEX* info);

public:

    bool get_mode(display_mode& mode) const;
    bool get_mode(display_mode& mode, DWORD index) const;
    const std::vector<display_mode> list_modes() const;

    display_orientation get_orientation() const;
    math::vec2 get_content_scale() const;

    bool get_bounds(math::recti& bounds) const;
    bool get_work_area(math::recti& work_area) const;

public:

    HMONITOR monitor_handle;
    LPCWSTR device_name;
};

struct video_data
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

    // display stuff

    std::vector<video::display> displays;
    size_t primary_display_index = 0;

};

extern video_data driver_data;

class video::video_impl
{
public:

    static bool init();
    static void quit();

    static bool set_dpi_awareness(process_dpi_awareness awareness);
    static process_dpi_awareness get_dpi_awareness();

    static BOOL CALLBACK enum_display_monitors_callback(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam);
    static void update_displays();
    static const display* get_primary_display();
    static const std::vector<const display*> list_displays();

};

// extra functions

void get_content_scale(HMONITOR hMonitor, float& xscale, float& yscale);

}
}