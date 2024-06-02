#include <set>

#include "win32_video.h"
#include "vertex/system/error.h"
#include "vertex/system/log.h"
#include "vertex/system/string/string_fn.h"

namespace vx {
namespace app {

///////////////////////////////////////////////////////////////////////////////
// init stuff
///////////////////////////////////////////////////////////////////////////////

video_data driver_data = video_data{};

const LPCWSTR video_data::window_class_name = L"Vertex_Window";

static bool load_libraries()
{
    {
        driver_data.user32.dll = LoadLibrary(L"user32.dll");
        if (!driver_data.user32.dll)
        {
            VX_ERROR(error::error_code::PLATFORM_ERROR) << "Failed to load user32.dll";
            return false;
        }

        driver_data.user32.SetProcessDPIAware = reinterpret_cast<decltype(driver_data.user32.SetProcessDPIAware)>(
            reinterpret_cast<void*>(GetProcAddress(driver_data.user32.dll, "SetProcessDPIAware")));
        driver_data.user32.SetProcessDpiAwarenessContext = reinterpret_cast<decltype(driver_data.user32.SetProcessDpiAwarenessContext)>(
            reinterpret_cast<void*>(GetProcAddress(driver_data.user32.dll, "SetProcessDpiAwarenessContext")));
        driver_data.user32.SetThreadDpiAwarenessContext = reinterpret_cast<decltype(driver_data.user32.SetThreadDpiAwarenessContext)>(
            reinterpret_cast<void*>(GetProcAddress(driver_data.user32.dll, "SetThreadDpiAwarenessContext")));
        driver_data.user32.GetThreadDpiAwarenessContext = reinterpret_cast<decltype(driver_data.user32.GetThreadDpiAwarenessContext)>(
            reinterpret_cast<void*>(GetProcAddress(driver_data.user32.dll, "GetThreadDpiAwarenessContext")));
        driver_data.user32.GetAwarenessFromDpiAwarenessContext = reinterpret_cast<decltype(driver_data.user32.GetAwarenessFromDpiAwarenessContext)>(
            reinterpret_cast<void*>(GetProcAddress(driver_data.user32.dll, "GetAwarenessFromDpiAwarenessContext")));
        driver_data.user32.EnableNonClientDpiScaling = reinterpret_cast<decltype(driver_data.user32.EnableNonClientDpiScaling)>(
            reinterpret_cast<void*>(GetProcAddress(driver_data.user32.dll, "EnableNonClientDpiScaling")));
        driver_data.user32.AdjustWindowRectExForDpi = reinterpret_cast<decltype(driver_data.user32.AdjustWindowRectExForDpi)>(
            reinterpret_cast<void*>(GetProcAddress(driver_data.user32.dll, "AdjustWindowRectExForDpi")));
        driver_data.user32.GetDpiForWindow = reinterpret_cast<decltype(driver_data.user32.GetDpiForWindow)>(
            reinterpret_cast<void*>(GetProcAddress(driver_data.user32.dll, "GetDpiForWindow")));
        driver_data.user32.AreDpiAwarenessContextsEqual = reinterpret_cast<decltype(driver_data.user32.AreDpiAwarenessContextsEqual)>(
            reinterpret_cast<void*>(GetProcAddress(driver_data.user32.dll, "AreDpiAwarenessContextsEqual")));
        driver_data.user32.IsValidDpiAwarenessContext = reinterpret_cast<decltype(driver_data.user32.IsValidDpiAwarenessContext)>(
            reinterpret_cast<void*>(GetProcAddress(driver_data.user32.dll, "IsValidDpiAwarenessContext")));
    }

    {
        driver_data.shcore.dll = LoadLibrary(L"shcore.dll");
        if (!driver_data.shcore.dll)
        {
            VX_ERROR(error::error_code::PLATFORM_ERROR) << "Failed to load shcore.dll";
            return false;
        }

        driver_data.shcore.GetDpiForMonitor = reinterpret_cast<decltype(driver_data.shcore.GetDpiForMonitor)>(
            reinterpret_cast<void*>(GetProcAddress(driver_data.shcore.dll, "GetDpiForMonitor")));
        driver_data.shcore.SetProcessDpiAwareness = reinterpret_cast<decltype(driver_data.shcore.SetProcessDpiAwareness)>(
            reinterpret_cast<void*>(GetProcAddress(driver_data.shcore.dll, "SetProcessDpiAwareness")));
    }

    return true;
}

static void free_libraries()
{
    if (driver_data.user32.dll)
    {
        FreeLibrary(driver_data.user32.dll);
    }
    if (driver_data.shcore.dll)
    {
        FreeLibrary(driver_data.shcore.dll);
    }
}

bool video::video_impl::set_dpi_awareness(process_dpi_awareness awareness)
{
    // https://learn.microsoft.com/en-us/windows/win32/hidpi/setting-the-default-dpi-awareness-for-a-process

    switch (awareness)
    {
        case process_dpi_awareness::UNAWARE:
        {
            if (driver_data.user32.SetProcessDpiAwarenessContext)
            {
                // Windows 10, version 1607
                return driver_data.user32.SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
            }
            if (driver_data.shcore.SetProcessDpiAwareness)
            {
                // Windows 8.1
                return SUCCEEDED(driver_data.shcore.SetProcessDpiAwareness(PROCESS_DPI_UNAWARE));
            }
            break;
        }
        case process_dpi_awareness::SYSTEM:
        {
            if (driver_data.user32.SetProcessDpiAwarenessContext)
            {
                // Windows 10, version 1607
                return driver_data.user32.SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
            }
            if (driver_data.shcore.SetProcessDpiAwareness)
            {
                // Windows 8.1
                return SUCCEEDED(driver_data.shcore.SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE));
            }
            if (driver_data.user32.SetProcessDPIAware)
            {
                // Windows Vista
                return driver_data.user32.SetProcessDPIAware();
            }
            break;
        }
        case process_dpi_awareness::PER_MONITOR:
        {
            if (driver_data.user32.SetProcessDpiAwarenessContext)
            {
                // Windows 10, version 1607
                return driver_data.user32.SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
            }
            if (driver_data.shcore.SetProcessDpiAwareness)
            {
                // Windows 8.1
                return SUCCEEDED(driver_data.shcore.SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE));
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
    if (driver_data.user32.dll && driver_data.user32.GetAwarenessFromDpiAwarenessContext && driver_data.user32.AreDpiAwarenessContextsEqual)
    {
        DPI_AWARENESS_CONTEXT context = driver_data.user32.GetThreadDpiAwarenessContext();

        if (driver_data.user32.AreDpiAwarenessContextsEqual(context, DPI_AWARENESS_CONTEXT_UNAWARE))
        {
            return process_dpi_awareness::UNAWARE;
        }
        if (driver_data.user32.AreDpiAwarenessContextsEqual(context, DPI_AWARENESS_CONTEXT_SYSTEM_AWARE))
        {
            return process_dpi_awareness::SYSTEM;
        }
        if (driver_data.user32.AreDpiAwarenessContextsEqual(context, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE))
        {
            return process_dpi_awareness::PER_MONITOR;
        }
    }

    return process_dpi_awareness::UNAWARE;
}

bool video::video_impl::init()
{
    if (!load_libraries())
    {
        return false;
    }
    if (!set_dpi_awareness(process_dpi_awareness::SYSTEM))
    {
        return false;
    }

    update_displays();
    if (driver_data.displays.empty())
    {
        return false;
    }

    return true;
}

void video::video_impl::quit()
{
    if (driver_data.window_default_icon != NULL)
    {
        DestroyIcon(driver_data.window_default_icon);
        driver_data.window_default_icon = NULL;
    }

    if (driver_data.blank_cursor != NULL)
    {
        DestroyCursor(driver_data.blank_cursor);
        driver_data.blank_cursor = NULL;
    }

    free_libraries();
}

///////////////////////////////////////////////////////////////////////////////
// display mode
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

bool video::display::display_impl::get_mode(video::display_mode& mode) const
{
    return get_mode(mode, ENUM_CURRENT_SETTINGS);
}

bool video::display::display_impl::get_mode(video::display_mode& mode, DWORD index) const
{
    DEVMODE dm{ sizeof(dm) };

    if (!EnumDisplaySettingsW(device_name, index, &dm))
    {
        return false;
    }

    if (dm.dmBitsPerPel == 0)
    {
        return false;
    }

    mode.width = dm.dmPelsWidth;
    mode.height = dm.dmPelsHeight;
    mode.bpp = dm.dmBitsPerPel;
    mode.pixel_format = pixel::pixel_format::PIXEL_FORMAT_UNKNOWN; // TODO: figure out how to get the pixel format
    mode.refresh_rate = get_refresh_rate(dm.dmDisplayFrequency);

    return true;
}

const std::vector<video::display_mode> video::display::display_impl::list_modes() const
{
    std::set<display_mode> modes;

    DWORD display_mode_index = 0;

    while (true)
    {
        display_mode mode;

        if (!get_mode(mode, display_mode_index++))
        {
            break;
        }

        modes.insert(mode);
    }

    return std::vector<display_mode>(modes.begin(), modes.end());
}

///////////////////////////////////////////////////////////////////////////////
// display orientation
///////////////////////////////////////////////////////////////////////////////

static video::display_orientation get_natural_orientation(const DEVMODE* mode)
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

static video::display_orientation get_display_orientation(const DEVMODE* mode)
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

video::display_orientation video::display::display_impl::get_orientation() const
{
    DEVMODE dm{ sizeof(dm) };
    return get_display_orientation(&dm);
}

math::vec2 video::display::display_impl::get_content_scale() const
{
    UINT xdpi, ydpi;
    bool dpi_set = false;

    if (driver_data.shcore.GetDpiForMonitor)
    {
        if (driver_data.shcore.GetDpiForMonitor(monitor_handle, MDT_EFFECTIVE_DPI, &xdpi, &ydpi) == S_OK)
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

///////////////////////////////////////////////////////////////////////////////
// display bounds
///////////////////////////////////////////////////////////////////////////////

bool video::display::display_impl::get_bounds(math::recti& bounds) const
{
    MONITORINFO info{ sizeof(info) };

    if (!GetMonitorInfo(monitor_handle, &info))
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

    if (!GetMonitorInfo(monitor_handle, &info))
    {
        return false;
    }

    work_area.position.x = info.rcWork.left;
    work_area.position.y = info.rcWork.top;
    work_area.size.x = info.rcWork.right - info.rcWork.left;
    work_area.size.y = info.rcWork.bottom - info.rcWork.top;

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// display polling
///////////////////////////////////////////////////////////////////////////////

struct poll_display_data
{
    size_t index = 0;
    std::vector<video::display> displays;
    size_t primary_monitor_index = 0;
    bool found_primary_monitor = false;
};

bool video::display::display_impl::create_display(size_t index, display& display, HMONITOR hMonitor, const MONITORINFOEX* info)
{
    display.id = index;

    display.m_impl = std::make_shared<display::display_impl>();
    display.m_impl->monitor_handle = hMonitor;
    display.m_impl->device_name = info->szDevice; // Unique identifier for the monitor determined by graphics card

    // Get the display device name (printable)
    {
        DISPLAY_DEVICE dev{};
        dev.cb = sizeof(dev);

        if (EnumDisplayDevicesW(info->szDevice, 0, &dev, NULL))
        {
            display.name = str::wstring_to_string(dev.DeviceString);
        }
    }

    // Get the current mode
    {
        display_mode mode;

        if (!display.get_mode(mode))
        {
            return false;
        }
    }

    return true;
}

BOOL CALLBACK video::video_impl::enum_display_monitors_callback(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam)
{
    poll_display_data& data = *(poll_display_data*)(lParam);

    MONITORINFOEX info{};
    info.cbSize = sizeof(info);

    if (GetMonitorInfo(hMonitor, (MONITORINFO*)(&info)))
    {
        if (info.dwFlags & MONITORINFOF_PRIMARY)
        {
            data.primary_monitor_index = data.index;
            data.found_primary_monitor = true;
        }

        VX_LOG_INFO << "Display: " << str::wstring_to_string(info.szDevice);

        display display;
        if (video::display::display_impl::create_display(data.index, display, hMonitor, &info))
        {
            data.displays.push_back(display);
        }
    }

    ++data.index;

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

void video::video_impl::update_displays()
{
    poll_display_data data;
    poll_displays_internal(data, enum_display_monitors_callback);

    if (!data.found_primary_monitor)
    {
        return;
    }

    driver_data.displays = std::move(data.displays);
    driver_data.primary_display_index = data.primary_monitor_index;
}

const video::display* video::video_impl::get_primary_display()
{
    if (driver_data.primary_display_index < driver_data.displays.size())
    {
        return &driver_data.displays.at(driver_data.primary_display_index);
    }

    return nullptr;
}

const std::vector<const video::display*> video::video_impl::list_displays()
{
    std::vector<const display*> displays;

    for (const display& d : driver_data.displays)
    {
        displays.push_back(&d);
    }

    return displays;
}

}
}