#pragma once

#include "../win32_header.h"
#include "vertex/app/video/video.h"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// dpi
///////////////////////////////////////////////////////////////////////////////

enum PROCESS_DPI_AWARENESS
{
    PROCESS_DPI_UNAWARE = 0,
    PROCESS_SYSTEM_DPI_AWARE = 1,
    PROCESS_PER_MONITOR_DPI_AWARE = 2
};

enum MONITOR_DPI_TYPE
{
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT
};

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
// display_mode_data
///////////////////////////////////////////////////////////////////////////////

struct display_mode_data
{
    display_mode mode;

    DEVMODE device_mode;
};

///////////////////////////////////////////////////////////////////////////////
// display_data
///////////////////////////////////////////////////////////////////////////////

struct display_data
{
    display display;

    HMONITOR monitor_handle;
    std::string device_name;
};

///////////////////////////////////////////////////////////////////////////////
// video_driver_data
///////////////////////////////////////////////////////////////////////////////

struct video_driver_data
{
    user32 user32;
    shcore shcore;

    std::vector<display_data> displays;
    size_t primary_display_index = 0;
};

static video_driver_data driver;

}
}
}