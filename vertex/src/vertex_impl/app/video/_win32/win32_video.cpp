#include <unordered_set>

#include "win32_video.h"
#include "win32_window.h"
#include "vertex/system/log.h"
#include "vertex/stdlib/string/string.h"

namespace vx {
namespace app {

///////////////////////////////////////////////////////////////////////////////
// init stuff
///////////////////////////////////////////////////////////////////////////////

const char* video::video_data::driver_name = "win32";
const video::caps::type video::video_data::video_caps = video::caps::SENDS_FULLSCREEN_DIMENSIONS;

const LPCWSTR video::video_impl::video_driver_data::window_class_name = L"Vertex_Window";
video::video_impl::video_driver_data video::video_impl::s_driver_data = video::video_impl::video_driver_data{};

static bool load_libraries(user32& user32_data, shcore& shcore_data)
{
    {
        user32_data.dll = LoadLibrary(L"user32.dll");
        if (!user32_data.dll)
        {
            VX_ERROR(error::error_code::SYSTEM_ERROR) << "Failed to load user32.dll";
            return false;
        }

        user32_data.SetProcessDPIAware = reinterpret_cast<decltype(user32_data.SetProcessDPIAware)>(
            GetProcAddress(user32_data.dll, "SetProcessDPIAware"));
        user32_data.SetProcessDpiAwarenessContext = reinterpret_cast<decltype(user32_data.SetProcessDpiAwarenessContext)>(
            GetProcAddress(user32_data.dll, "SetProcessDpiAwarenessContext"));
        user32_data.SetThreadDpiAwarenessContext = reinterpret_cast<decltype(user32_data.SetThreadDpiAwarenessContext)>(
            GetProcAddress(user32_data.dll, "SetThreadDpiAwarenessContext"));
        user32_data.GetThreadDpiAwarenessContext = reinterpret_cast<decltype(user32_data.GetThreadDpiAwarenessContext)>(
            GetProcAddress(user32_data.dll, "GetThreadDpiAwarenessContext"));
        user32_data.GetAwarenessFromDpiAwarenessContext = reinterpret_cast<decltype(user32_data.GetAwarenessFromDpiAwarenessContext)>(
            GetProcAddress(user32_data.dll, "GetAwarenessFromDpiAwarenessContext"));
        user32_data.EnableNonClientDpiScaling = reinterpret_cast<decltype(user32_data.EnableNonClientDpiScaling)>(
            GetProcAddress(user32_data.dll, "EnableNonClientDpiScaling"));
        user32_data.AdjustWindowRectExForDpi = reinterpret_cast<decltype(user32_data.AdjustWindowRectExForDpi)>(
            GetProcAddress(user32_data.dll, "AdjustWindowRectExForDpi"));
        user32_data.GetDpiForWindow = reinterpret_cast<decltype(user32_data.GetDpiForWindow)>(
            GetProcAddress(user32_data.dll, "GetDpiForWindow"));
        user32_data.AreDpiAwarenessContextsEqual = reinterpret_cast<decltype(user32_data.AreDpiAwarenessContextsEqual)>(
            GetProcAddress(user32_data.dll, "AreDpiAwarenessContextsEqual"));
        user32_data.IsValidDpiAwarenessContext = reinterpret_cast<decltype(user32_data.IsValidDpiAwarenessContext)>(
            GetProcAddress(user32_data.dll, "IsValidDpiAwarenessContext"));
    }

    {
        shcore_data.dll = LoadLibrary(L"shcore.dll");
        if (!shcore_data.dll)
        {
            VX_ERROR(error::error_code::SYSTEM_ERROR) << "Failed to load shcore.dll";
            return false;
        }

        shcore_data.GetDpiForMonitor = reinterpret_cast<decltype(shcore_data.GetDpiForMonitor)>(
            GetProcAddress(shcore_data.dll, "GetDpiForMonitor"));
        shcore_data.SetProcessDpiAwareness = reinterpret_cast<decltype(shcore_data.SetProcessDpiAwareness)>(
            GetProcAddress(shcore_data.dll, "SetProcessDpiAwareness"));
    }

    return true;
}

static void free_libraries(user32& user32_data, shcore& shcore_data)
{
    if (user32_data.dll)
    {
        FreeLibrary(user32_data.dll);
        user32_data.dll = NULL;
    }
    if (shcore_data.dll)
    {
        FreeLibrary(shcore_data.dll);
        shcore_data.dll = NULL;
    }
}

static ATOM register_window_class(WNDPROC proc, HINSTANCE instance, LPCWSTR class_name)
{
    WNDCLASSW wc{};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = proc;
    wc.hInstance = instance; // needed for dll
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.lpszClassName = class_name;

    return RegisterClass(&wc);
}

static void unregister_window_class(LPCWSTR class_name, HINSTANCE instance)
{
    UnregisterClass(class_name, instance);
}

bool video::video_impl::set_dpi_awareness(process_dpi_awareness awareness)
{
    // https://learn.microsoft.com/en-us/windows/win32/hidpi/setting-the-default-dpi-awareness-for-a-process

    switch (awareness)
    {
        case process_dpi_awareness::UNAWARE:
        {
            if (s_driver_data.user32.SetProcessDpiAwarenessContext)
            {
                // Windows 10, version 1607
                return s_driver_data.user32.SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
            }
            if (s_driver_data.shcore.SetProcessDpiAwareness)
            {
                // Windows 8.1
                return SUCCEEDED(s_driver_data.shcore.SetProcessDpiAwareness(PROCESS_DPI_UNAWARE));
            }
            break;
        }
        case process_dpi_awareness::SYSTEM:
        {
            if (s_driver_data.user32.SetProcessDpiAwarenessContext)
            {
                // Windows 10, version 1607
                return s_driver_data.user32.SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
            }
            if (s_driver_data.shcore.SetProcessDpiAwareness)
            {
                // Windows 8.1
                return SUCCEEDED(s_driver_data.shcore.SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE));
            }
            if (s_driver_data.user32.SetProcessDPIAware)
            {
                // Windows Vista
                return s_driver_data.user32.SetProcessDPIAware();
            }
            break;
        }
        case process_dpi_awareness::PER_MONITOR:
        {
            if (s_driver_data.user32.SetProcessDpiAwarenessContext)
            {
                // Windows 10, version 1607
                return s_driver_data.user32.SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
            }
            if (s_driver_data.shcore.SetProcessDpiAwareness)
            {
                // Windows 8.1
                return SUCCEEDED(s_driver_data.shcore.SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE));
            }
            else
            {
                // Older OS: fall back to system DPI aware
                return set_dpi_awareness(process_dpi_awareness::SYSTEM);
            }
            break;
        }
    }

    return false;
}

video::process_dpi_awareness video::video_impl::get_dpi_awareness()
{
    if (s_driver_data.user32.dll && s_driver_data.user32.GetAwarenessFromDpiAwarenessContext && s_driver_data.user32.AreDpiAwarenessContextsEqual)
    {
        DPI_AWARENESS_CONTEXT context = s_driver_data.user32.GetThreadDpiAwarenessContext();

        if (s_driver_data.user32.AreDpiAwarenessContextsEqual(context, DPI_AWARENESS_CONTEXT_UNAWARE))
        {
            return process_dpi_awareness::UNAWARE;
        }
        if (s_driver_data.user32.AreDpiAwarenessContextsEqual(context, DPI_AWARENESS_CONTEXT_SYSTEM_AWARE))
        {
            return process_dpi_awareness::SYSTEM;
        }
        if (s_driver_data.user32.AreDpiAwarenessContextsEqual(context, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE))
        {
            return process_dpi_awareness::PER_MONITOR;
        }
    }

    return process_dpi_awareness::UNAWARE;
}

bool video::video_impl::init()
{
    if (!load_libraries(s_driver_data.user32, s_driver_data.shcore))
    {
        return false;
    }
    if (!set_dpi_awareness(process_dpi_awareness::SYSTEM))
    {
        return false;
    }

    if (!s_driver_data.window_class)
    {
        s_driver_data.window_class = register_window_class(
            window::window_impl::window_proc,
            s_driver_data.instance,
            s_driver_data.window_class_name
        );

        if (!s_driver_data.window_class)
        {
            return false;
        }
    }

    return true;
}

void video::video_impl::quit()
{
    unregister_window_class(s_driver_data.window_class_name, s_driver_data.instance);
    s_driver_data.window_class = NULL;

    free_libraries(s_driver_data.user32, s_driver_data.shcore);
}

video::system_theme video::video_impl::get_system_theme()
{
    const wchar_t* subkey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
    const wchar_t* value = L"AppsUseLightTheme";
    DWORD data = 0;
    DWORD size = sizeof(data);
    DWORD type = 0;

    if (RegGetValue(HKEY_CURRENT_USER, subkey, value, RRF_RT_REG_DWORD, &type, &data, &size) == ERROR_SUCCESS)
    {
        // Dark mode if 0, light mode if 1
        switch (data)
        {
            case 0:  return system_theme::DARK;
            case 1:  return system_theme::LIGHT;
            default: break;
        }
    }

    return system_theme::UNKNOWN;
}

///////////////////////////////////////////////////////////////////////////////
// display mode info
///////////////////////////////////////////////////////////////////////////////

static float get_refresh_rate(DWORD rate)
{
    // Convert to NTSC timings
    switch (rate)
    {
        case 119:   return 199.88f;
        case 59:    return 59.94f;
        case 29:    return 29.97f;
        default:    return static_cast<float>(rate);
    }
}

static video::display_orientation get_natural_orientation(const PDEVMODE mode)
{
    int w = mode->dmPelsWidth;
    int h = mode->dmPelsHeight;

    if (mode->dmDisplayOrientation == DMDO_90 || mode->dmDisplayOrientation == DMDO_270)
    {
        std::swap(w, h);
    }

    // The best we can do is guess
    return (w >= h) ? video::display_orientation::LANDSCAPE : video::display_orientation::PORTRAIT;
}

static video::display_orientation get_display_orientation(const PDEVMODE mode)
{
    switch (get_natural_orientation(mode))
    {
        default:
        case video::display_orientation::LANDSCAPE:
        {
            switch (mode->dmDisplayOrientation)
            {
                case DMDO_DEFAULT: return video::display_orientation::LANDSCAPE;
                case DMDO_90:      return video::display_orientation::PORTRAIT;
                case DMDO_180:     return video::display_orientation::LANDSCAPE_FLIPPED;
                case DMDO_270:     return video::display_orientation::PORTRAIT_FLIPPED;
                default:           return video::display_orientation::UNKNOWN;
            }
        }
        case video::display_orientation::PORTRAIT:
        {
            switch (mode->dmDisplayOrientation)
            {
                case DMDO_DEFAULT: return video::display_orientation::PORTRAIT;
                case DMDO_90:      return video::display_orientation::LANDSCAPE_FLIPPED;
                case DMDO_180:     return video::display_orientation::PORTRAIT_FLIPPED;
                case DMDO_270:     return video::display_orientation::LANDSCAPE;
                default:           return video::display_orientation::UNKNOWN;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// display content scale
///////////////////////////////////////////////////////////////////////////////

math::vec2 video::video_impl::get_display_content_scale(const HMONITOR handle)
{
    UINT xdpi, ydpi;
    bool dpi_set = false;

    if (s_driver_data.shcore.GetDpiForMonitor)
    {
        if (s_driver_data.shcore.GetDpiForMonitor(handle, MDT_EFFECTIVE_DPI, &xdpi, &ydpi) == S_OK)
        {
            dpi_set = true;
        }
    }

    if (!dpi_set)
    {
        // Safe default
        xdpi = ydpi = USER_DEFAULT_SCREEN_DPI;

        // Window 8.0 and below: same DPI for all monitors 
        HDC hdc = GetDC(NULL);
        if (hdc)
        {
            xdpi = GetDeviceCaps(hdc, LOGPIXELSX);
            ydpi = GetDeviceCaps(hdc, LOGPIXELSY);
            ReleaseDC(NULL, hdc);
        }
    }

    return math::vec2(
        xdpi / static_cast<float>(USER_DEFAULT_SCREEN_DPI),
        ydpi / static_cast<float>(USER_DEFAULT_SCREEN_DPI)
    );
}

bool video::video_impl::get_display_mode(const std::wstring& device_name, display_mode& mode, DWORD index, display_orientation* orientation)
{
    DEVMODE dm{ sizeof(dm) };

    if (!EnumDisplaySettings(device_name.c_str(), index, &dm))
    {
        return false;
    }

    if (dm.dmBitsPerPel == 0)
    {
        return false;
    }

    mode.m_impl = std::make_unique<display_mode::display_mode_impl>();

    mode.resolution.x = dm.dmPelsWidth;
    mode.resolution.y = dm.dmPelsHeight;
    mode.bpp = dm.dmBitsPerPel;
    mode.pixel_format = pixel::pixel_format::UNKNOWN; // TODO: figure out how to get the pixel format
    mode.pixel_density = 1.0f;
    mode.refresh_rate = get_refresh_rate(dm.dmDisplayFrequency);

    mode.m_impl->m_devmode = dm;

    if (orientation)
    {
        *orientation = get_display_orientation(&dm);
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// display polling
///////////////////////////////////////////////////////////////////////////////

struct poll_display_data
{
    size_t index;
    bool find_primary;
    std::vector<std::unique_ptr<video::display>>* displays;
};

bool video::video_impl::create_display(
    size_t& index,
    std::vector<std::unique_ptr<display>>& displays,
    HMONITOR hMonitor,
    const MONITORINFOEX* info
)
{
    display_mode current_mode;
    display_orientation current_orientation;

    if (!get_display_mode(info->szDevice, current_mode, ENUM_CURRENT_SETTINGS, &current_orientation))
    {
        return false;
    }

    math::vec2 current_content_scale = get_display_content_scale(hMonitor);

    bool found = false;

    // Check to see if the monitor already exists
    for (size_t i = 0; i < displays.size(); ++i)
    {
        display* d = displays[i].get();
        display::display_impl* d_impl = d->m_impl.get();

        if (d_impl->m_device_name == info->szDevice)
        {
            bool moved = (index != i);

            if (d_impl->m_state != display_state::REMOVED)
            {
                return false;
            }

            if (index >= displays.size())
            {
                return false;
            }

            if (moved)
            {
                std::swap(displays[index], displays[i]);
            }

            d_impl->m_handle = hMonitor;
            d_impl->m_state = display_state::NONE;

            if (!s_video_data.setting_display_mode)
            {
                math::recti current_bounds = d->get_bounds();

                if (moved || d->get_bounds() != d_impl->m_last_bounds)
                {
                    // moved
                    d_impl->m_last_bounds = current_bounds;
                    post_display_moved(d);
                }

                post_display_orientation_changed(d, current_orientation);
                post_display_content_scale_changed(d, current_content_scale);
            }

            found = true;
            break;
        }
    }

    if (!found)
    {
        displays.emplace_back(std::make_unique<display>());
        display* d = displays.back().get();
        d->m_impl = std::make_unique<video::display::display_impl>();
        display::display_impl* d_impl = d->m_impl.get();

        d_impl->m_handle = hMonitor;
        d_impl->m_device_name = info->szDevice; // Unique identifier for the monitor determined by graphics card
        d_impl->m_state = display_state::ADDED; // Mark as added by default

        d->m_id = get_next_device_id();

        // Get the display device name (printable)
        {
            DISPLAY_DEVICE dev{ sizeof(dev) };

            if (EnumDisplayDevices(info->szDevice, 0, &dev, NULL))
            {
                d->m_name = str::wstring_to_string(dev.DeviceString);
            }
        }

        {
            current_mode.m_display_id = d->m_id;
            d->m_desktop_mode = d->m_current_mode = current_mode;
            d->m_orientation = current_orientation;
            d->m_content_scale = current_content_scale;
            d_impl->m_last_bounds = d->get_bounds();
        }
    }

    return true;
}

BOOL CALLBACK video::video_impl::enum_displays_callback(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam)
{
    poll_display_data& data = *(poll_display_data*)(lParam);

    MONITORINFOEX info{};
    info.cbSize = sizeof(info);

    if (GetMonitorInfo(hMonitor, (MONITORINFO*)(&info)))
    {
        if (data.find_primary == static_cast<bool>(info.dwFlags & MONITORINFOF_PRIMARY))
        {
            create_display(data.index, *data.displays, hMonitor, &info);
            ++data.index;
        }
    }

    return TRUE;
}

static void poll_displays_internal(poll_display_data& data, MONITORENUMPROC callback)
{
    // NOTE: This will enumerate all unique displays. If a monitor is being
    // extended by another monitor, each will be listed as a unique display.
    // If a monitor is being duplicated accross other monitors, only the
    // original monitor will be listed. Each unique display corresponds to a
    // display slot on the graphics card being used. Different graphics cards
    // may support different number of displays.

    EnumDisplayMonitors(NULL, NULL, callback, (LPARAM)&data);
}

void video::video_impl::update_displays(std::vector<std::unique_ptr<display>>& displays)
{
    poll_display_data data{ 0 };
    data.displays = &displays;

    for (const std::unique_ptr<display>& d : displays)
    {
        d->m_impl->m_state = display_state::REMOVED;
    }

    data.find_primary = true;
    poll_displays_internal(data, enum_displays_callback);

    data.find_primary = false;
    poll_displays_internal(data, enum_displays_callback);

    // remove displays still marked as removed
    auto it = displays.begin();
    while (it != displays.end())
    {
        const display& d = *(it->get());

        if (d.m_impl->m_state == display_state::REMOVED)
        {
            post_display_removed(&d);
            it = displays.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // add new displays
    for (const std::unique_ptr<display>& d : displays)
    {
        if (d->m_impl->m_state == display_state::ADDED)
        {
            post_display_added(d.get());
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// display_impl
///////////////////////////////////////////////////////////////////////////////

HMONITOR video::display::display_impl::get_handle() const
{
    return m_handle;
}

void video::display::display_impl::list_display_modes(std::vector<display_mode>& modes)
{
    modes.clear();

    DWORD display_mode_index = 0;
    display_mode mode;

    while (true)
    {
        if (!video_impl::get_display_mode(m_device_name, mode, display_mode_index++, nullptr))
        {
            break;
        }

        // don't add duplicates
        bool found = false;
        for (const display_mode& m : modes)
        {
            if (display_mode::compare(m, mode))
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            modes.push_back(mode);
        }
    }
}

bool video::display::display_impl::set_display_mode(display_mode& mode) const
{
    LONG status = ChangeDisplaySettingsEx(m_device_name.c_str(), &mode.m_impl->m_devmode, NULL, CDS_FULLSCREEN, NULL);

    if (status != DISP_CHANGE_SUCCESSFUL)
    {
        const char* reason = "Unknown reason";

        switch (status)
        {
            case DISP_CHANGE_BADFLAGS:
            {
                reason = "DISP_CHANGE_BADFLAGS";
                break;
            }
            case DISP_CHANGE_BADMODE:
            {
                reason = "DISP_CHANGE_BADMODE";
                break;
            }
            case DISP_CHANGE_BADPARAM:
            {
                reason = "DISP_CHANGE_BADPARAM";
                break;
            }
            case DISP_CHANGE_FAILED:
            {
                reason = "DISP_CHANGE_FAILED";
                break;
            }
        }

        VX_ERROR(error::error_code::SYSTEM_ERROR) << "ChangeDisplaySettingsEx() failed: " << reason;
        return false;
    }

    video_impl::get_display_mode(m_device_name.c_str(), mode, ENUM_CURRENT_SETTINGS, nullptr);

    return true;
}

bool video::display::display_impl::get_bounds(math::recti& bounds) const
{
    MONITORINFO info{ sizeof(info) };

    if (!GetMonitorInfo(m_handle, &info))
    {
        return false;
    }

    bounds.position.x = info.rcMonitor.left;
    bounds.position.y = info.rcMonitor.top;
    bounds.size.x = info.rcMonitor.right - info.rcMonitor.left;
    bounds.size.y = info.rcMonitor.bottom - info.rcMonitor.top;

    return true;
}

bool video::display::display_impl::get_work_area(math::recti& work_area) const
{
    MONITORINFO info{ sizeof(info) };

    if (!GetMonitorInfo(m_handle, &info))
    {
        return false;
    }

    work_area.position.x = info.rcWork.left;
    work_area.position.y = info.rcWork.top;
    work_area.size.x = info.rcWork.right - info.rcWork.left;
    work_area.size.y = info.rcWork.bottom - info.rcWork.top;

    return true;
}

}
}