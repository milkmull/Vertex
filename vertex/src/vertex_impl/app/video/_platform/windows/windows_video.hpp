#pragma once

#include "vertex_impl/app/video/video_internal.hpp"
#include "vertex_impl/app/_platform/windows/windows_header.hpp"
#include "vertex/os/shared_library.hpp"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// external module functions
///////////////////////////////////////////////////////////////////////////////

struct user32
{
    os::shared_library dll;

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
    LONG(WINAPI* GetDisplayConfigBufferSizes)(UINT32, UINT32*, UINT32*);
    LONG(WINAPI* QueryDisplayConfig)(UINT32, UINT32*, DISPLAYCONFIG_PATH_INFO*, UINT32*, DISPLAYCONFIG_MODE_INFO*, DISPLAYCONFIG_TOPOLOGY_ID*);
    LONG(WINAPI* DisplayConfigGetDeviceInfo)(DISPLAYCONFIG_DEVICE_INFO_HEADER*);
};

struct shcore
{
    os::shared_library dll;

    HRESULT(WINAPI* GetDpiForMonitor)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
    HRESULT(WINAPI* SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);
};

///////////////////////////////////////////////////////////////////////////////
// video_impl
///////////////////////////////////////////////////////////////////////////////

struct video_impl_data
{
    user32 user32;
    shcore shcore;

    std::wstring app_name;
    bool registered_app = false;

    system_theme system_theme_cache = system_theme::UNKNOWN;
};

class video_instance_impl
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // libraries
    ///////////////////////////////////////////////////////////////////////////////

    bool load_libraries();
    void free_libraries();

    ///////////////////////////////////////////////////////////////////////////////
    // initialization
    ///////////////////////////////////////////////////////////////////////////////

    bool init(video_instance* owner);
    void quit();

    ///////////////////////////////////////////////////////////////////////////////
    // dpi
    ///////////////////////////////////////////////////////////////////////////////

    process_dpi_awareness get_dpi_awareness() const;
    bool set_dpi_awareness(process_dpi_awareness awareness);

    ///////////////////////////////////////////////////////////////////////////////
    // system theme
    ///////////////////////////////////////////////////////////////////////////////

    static system_theme get_system_theme();

    ///////////////////////////////////////////////////////////////////////////////
    // screen saver
    ///////////////////////////////////////////////////////////////////////////////

    bool suspend_screen_saver();

    ///////////////////////////////////////////////////////////////////////////////
    // displays
    ///////////////////////////////////////////////////////////////////////////////

    bool create_display(
        size_t index,
        std::vector<display_instance>& displays,
        HMONITOR hMonitor,
        const MONITORINFOEX* info
    ) const;

    void update_displays();

    inline display* get_display_for_window(const window& w)
    {
        return nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    video_instance* video = nullptr;
    video_impl_data data;
};

///////////////////////////////////////////////////////////////////////////////
// display_mode_instance_impl
///////////////////////////////////////////////////////////////////////////////

struct display_mode_impl_data
{
    DEVMODE devmode;
};

class display_mode_instance_impl
{
public:

    display_mode_impl_data data;
};

///////////////////////////////////////////////////////////////////////////////
// display_impl
///////////////////////////////////////////////////////////////////////////////

enum class display_state
{
    NONE    = 0,
    ADDED   = (1 << 0),
    REMOVED = (1 << 1)
};

struct display_impl_data
{
    HMONITOR handle = NULL;
    std::wstring device_name;
    display_state state = display_state::NONE;
    math::recti last_bounds;
};

class display_instance_impl
{
public:

    void list_display_modes(std::vector<display_mode_instance>& modes) const;
    bool set_display_mode(display_mode_instance& mode, bool is_desktop_mode);

    bool get_bounds(math::recti& bounds) const;
    bool get_work_area(math::recti& work_area) const;

    display_impl_data data;
};

} // namespace video
} // namespace app
} // namespace vx