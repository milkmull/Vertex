#pragma once

#include "vertex_impl/app/video/video_internal.hpp"
#include "vertex_impl/app/_platform/windows/windows_header.hpp"
#include "vertex_impl/app/input/_platform/windows/windows_raw_input.hpp"
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

    BOOL(WINAPI* SetProcessDPIAware)(void) = nullptr;
    BOOL(WINAPI* SetProcessDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = nullptr;
    DPI_AWARENESS_CONTEXT(WINAPI* SetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = nullptr;
    DPI_AWARENESS_CONTEXT(WINAPI* GetThreadDpiAwarenessContext)(void) = nullptr;
    DPI_AWARENESS(WINAPI* GetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = nullptr;
    BOOL(WINAPI* EnableNonClientDpiScaling)(HWND) = nullptr;
    BOOL(WINAPI* AdjustWindowRectExForDpi)(LPRECT, DWORD, BOOL, DWORD, UINT) = nullptr;
    UINT(WINAPI* GetDpiForWindow)(HWND) = nullptr;
    BOOL(WINAPI* AreDpiAwarenessContextsEqual)(DPI_AWARENESS_CONTEXT, DPI_AWARENESS_CONTEXT) = nullptr;
    BOOL(WINAPI* IsValidDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = nullptr;
    LONG(WINAPI* GetDisplayConfigBufferSizes)(UINT32, UINT32*, UINT32*) = nullptr;
    LONG(WINAPI* QueryDisplayConfig)(UINT32, UINT32*, DISPLAYCONFIG_PATH_INFO*, UINT32*, DISPLAYCONFIG_MODE_INFO*, DISPLAYCONFIG_TOPOLOGY_ID*) = nullptr;
    LONG(WINAPI* DisplayConfigGetDeviceInfo)(DISPLAYCONFIG_DEVICE_INFO_HEADER*) = nullptr;
};

struct shcore
{
    os::shared_library dll;

    HRESULT(WINAPI* GetDpiForMonitor)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*) = nullptr;
    HRESULT(WINAPI* SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS) = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
// video_impl
///////////////////////////////////////////////////////////////////////////////

struct video_impl_data
{
    user32 user32;
    shcore shcore;

    os::windows::com_initializer com;
    os::windows::ole_initializer ole;

    std::wstring app_name;
    bool registered_app = false;
    UINT _VX_WAKEUP = 0;

    owner_ptr<input::raw_input_manager> raw_input;

    bool enable_message_loop_hint_cache = true;
    bool enable_menu_mnemonics_hint_cache = false;
    bool frame_usable_while_cursor_hidden_hint_cache = true;
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
    bool set_dpi_awareness();
    bool set_dpi_awareness_internal(process_dpi_awareness awareness);

    ///////////////////////////////////////////////////////////////////////////////
    // system theme
    ///////////////////////////////////////////////////////////////////////////////

    system_theme get_system_theme() const;

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

    bool init_displays();
    void quit_displays();
    void refresh_displays();

    ///////////////////////////////////////////////////////////////////////////////
    // events
    ///////////////////////////////////////////////////////////////////////////////

    void pump_events();
    bool wait_event_timeout(time::time_point t);
    void send_wakeup_event(window* w);

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

    void list_display_modes(const display_instance* display) const;
    bool set_display_mode(display_mode_instance& mode, bool is_desktop_mode);

    bool get_bounds(math::recti& bounds) const;
    bool get_work_area(math::recti& work_area) const;

    display_impl_data data;
};

} // namespace video
} // namespace app
} // namespace vx