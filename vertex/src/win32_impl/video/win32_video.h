#pragma once

#include "win32_impl/win32_header.h"
#include "vertex/app/video/video.h"

namespace vx {
namespace app {

class video::video_impl
{
public:

    friend video;
    friend display_mode;
    friend display;
    friend window;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // display_mode
    ///////////////////////////////////////////////////////////////////////////////

    static bool get_display_mode(const std::wstring& device_name, display_mode& mode, DWORD index, display_orientation* orientation);

public:

    ///////////////////////////////////////////////////////////////////////////////
    // display
    ///////////////////////////////////////////////////////////////////////////////

    struct display_state
    {
        using type = uint32_t;

        enum : type
        {
            NONE    = 0,
            ADDED   = (1 << 0),
            REMOVED = (1 << 1)
        };
    };

    static math::vec2 get_display_content_scale(const HMONITOR handle);

    static bool create_display(size_t& index, std::vector<display>& displays, HMONITOR hMonitor, const MONITORINFOEX* info);
    static void update_displays(std::vector<display>& displays);
    static BOOL CALLBACK enum_displays_callback(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam);

public:

    ///////////////////////////////////////////////////////////////////////////////
    // video
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

    struct shcore
    {
        HMODULE dll;

        HRESULT(WINAPI* GetDpiForMonitor)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
        HRESULT(WINAPI* SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);
    };

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

public:

    static bool load_libraries();
    static void free_libraries();

    static bool init();
    static void quit();

    static bool set_dpi_awareness(process_dpi_awareness awareness);
    static process_dpi_awareness get_dpi_awareness();

    static system_theme get_system_theme();

    static bool register_window_class(WNDPROC proc);
    static void unregister_window_class();

    static bool set_default_window_icon(const uint8_t* pixels, const math::vec2i& size);

    static bool make_custom_cursor(HCURSOR& cursor, const uint8_t* pixels, const math::vec2i& size, const math::vec2i& hotspot);
    static bool make_blank_cursor();

    static video_driver_data s_driver_data;
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

    display_mode* m_owner;
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

    void list_display_modes();
    bool set_display_mode(display_mode& mode) const;

    bool get_bounds(math::recti& bounds) const;
    bool get_work_area(math::recti& work_area) const;

private:

    display* m_owner;
    HMONITOR m_handle;
    std::wstring m_device_name;
    video_impl::display_state::type m_state;
    math::recti m_last_bounds;
};

#define VIDEO_DRIVER_DATA ::vx::app::video::video_impl::s_driver_data

}
}