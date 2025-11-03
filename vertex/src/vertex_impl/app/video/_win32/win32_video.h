#pragma once

#include "vertex_impl/_platform/_win32/win32_header.h"
#include "vertex/app/video/video.h"

namespace vx {
namespace app {

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

struct shcore
{
    HMODULE dll;

    HRESULT(WINAPI* GetDpiForMonitor)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
    HRESULT(WINAPI* SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);
};

class video::video_impl
{
private:

    friend video;
    friend display_mode;
    friend display;
    friend window;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // video
    ///////////////////////////////////////////////////////////////////////////////

    static bool init();
    static void quit();

    static bool set_dpi_awareness(process_dpi_awareness awareness);
    static process_dpi_awareness get_dpi_awareness();

    static system_theme get_system_theme();

public:

    struct video_driver_data
    {
        user32 user32;
        shcore shcore;

        HMODULE instance;
        static const LPCWSTR window_class_name;
        ATOM window_class;

        // Should have default window icon in here
    };

    static video_driver_data s_driver_data;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // display polling
    ///////////////////////////////////////////////////////////////////////////////

    struct display_state
    {
        using type = uint32_t;

        enum : type
        {
            NONE    = 0,
            ADDED   = VX_BIT(0),
            REMOVED = VX_BIT(1)
        };
    };

    static math::vec2 get_display_content_scale(const HMONITOR handle);
    static bool get_display_mode(const std::wstring& device_name, display_mode& mode, DWORD index, display_orientation* orientation);

    static bool create_display(size_t& index, std::vector<std::unique_ptr<display>>& displays, HMONITOR hMonitor, const MONITORINFOEX* info);
    static void update_displays(std::vector<std::unique_ptr<display>>& displays);
    static BOOL CALLBACK enum_displays_callback(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam);
};

///////////////////////////////////////////////////////////////////////////////
// display_mode_impl
///////////////////////////////////////////////////////////////////////////////

class video::display_mode::display_mode_impl
{
private:

    friend video_impl;
    friend display_mode;
    friend display::display_impl;

private:

    DEVMODE m_devmode;
};

///////////////////////////////////////////////////////////////////////////////
// display_impl
///////////////////////////////////////////////////////////////////////////////

class video::display::display_impl
{
private:

    friend video_impl;
    friend display;

public:

    HMONITOR get_handle() const;

    void list_display_modes(std::vector<display_mode>& modes);
    bool set_display_mode(display_mode& mode) const;

    bool get_bounds(math::recti& bounds) const;
    bool get_work_area(math::recti& work_area) const;

private:

    HMONITOR m_handle = NULL;
    std::wstring m_device_name;
    video_impl::display_state::type m_state = video_impl::display_state::NONE;
    math::recti m_last_bounds;
};

#define VIDEO_DRIVER_DATA ::vx::app::video::video_impl::s_driver_data

}
}