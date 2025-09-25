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

    static LPCWSTR app_name;
    bool registered_app = false;
};

class video_impl
{
public:

    static bool init();
    static bool is_init();
    static void quit();

    static process_dpi_awareness get_dpi_awareness();
    static system_theme get_system_theme();

    static inline constexpr capabilities get_capabilities() noexcept
    {
        return capabilities::SENDS_DISPLAY_CHANGES;
    }

    static inline constexpr bool has_capabilities(capabilities caps) noexcept
    {
        return get_capabilities() & caps;
    }

    static void update_displays(std::vector<owner_ptr<display>>& displays);

    static inline display* get_display_for_window(const window& w)
    {
        return nullptr;
    }
};

///////////////////////////////////////////////////////////////////////////////
// display_mode_impl
///////////////////////////////////////////////////////////////////////////////

struct display_mode_impl_data
{
    DEVMODE devmode;
};

class display_mode_impl
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

class display_impl
{
public:

    static bool get_display_mode(
        const WCHAR* device_name,
        display_mode_instance& mode, DWORD index, display_orientation* orientation
    );

    static void list_display_modes(std::vector<display_mode_instance>& modes);
    static bool set_display_mode(display_mode_instance& mode, bool is_desktop_mode);

    static bool get_bounds(math::recti& bounds);
    static bool get_work_area(math::recti& work_area);
};

} // namespace video
} // namespace app
} // namespace vx