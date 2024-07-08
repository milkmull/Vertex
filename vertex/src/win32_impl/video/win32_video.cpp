#include <unordered_set>

#include "win32_video.h"
#include "vertex/system/error.h"
#include "vertex/system/log.h"
#include "vertex/system/string/string_fn.h"
#include "vertex_impl/app/event/display_events.h"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// init stuff
///////////////////////////////////////////////////////////////////////////////

const LPCWSTR video::video_driver_data::window_class_name = L"Vertex_Window";
video_driver_data s_driver_data = video_driver_data{};

static bool load_libraries()
{
    {
        s_driver_data.user32.dll = LoadLibrary(L"user32.dll");
        if (!s_driver_data.user32.dll)
        {
            VX_ERROR(error::error_code::PLATFORM_ERROR) << "Failed to load user32.dll";
            return false;
        }

        s_driver_data.user32.SetProcessDPIAware = reinterpret_cast<decltype(s_driver_data.user32.SetProcessDPIAware)>(
            reinterpret_cast<void*>(GetProcAddress(s_driver_data.user32.dll, "SetProcessDPIAware")));
        s_driver_data.user32.SetProcessDpiAwarenessContext = reinterpret_cast<decltype(s_driver_data.user32.SetProcessDpiAwarenessContext)>(
            reinterpret_cast<void*>(GetProcAddress(s_driver_data.user32.dll, "SetProcessDpiAwarenessContext")));
        s_driver_data.user32.SetThreadDpiAwarenessContext = reinterpret_cast<decltype(s_driver_data.user32.SetThreadDpiAwarenessContext)>(
            reinterpret_cast<void*>(GetProcAddress(s_driver_data.user32.dll, "SetThreadDpiAwarenessContext")));
        s_driver_data.user32.GetThreadDpiAwarenessContext = reinterpret_cast<decltype(s_driver_data.user32.GetThreadDpiAwarenessContext)>(
            reinterpret_cast<void*>(GetProcAddress(s_driver_data.user32.dll, "GetThreadDpiAwarenessContext")));
        s_driver_data.user32.GetAwarenessFromDpiAwarenessContext = reinterpret_cast<decltype(s_driver_data.user32.GetAwarenessFromDpiAwarenessContext)>(
            reinterpret_cast<void*>(GetProcAddress(s_driver_data.user32.dll, "GetAwarenessFromDpiAwarenessContext")));
        s_driver_data.user32.EnableNonClientDpiScaling = reinterpret_cast<decltype(s_driver_data.user32.EnableNonClientDpiScaling)>(
            reinterpret_cast<void*>(GetProcAddress(s_driver_data.user32.dll, "EnableNonClientDpiScaling")));
        s_driver_data.user32.AdjustWindowRectExForDpi = reinterpret_cast<decltype(s_driver_data.user32.AdjustWindowRectExForDpi)>(
            reinterpret_cast<void*>(GetProcAddress(s_driver_data.user32.dll, "AdjustWindowRectExForDpi")));
        s_driver_data.user32.GetDpiForWindow = reinterpret_cast<decltype(s_driver_data.user32.GetDpiForWindow)>(
            reinterpret_cast<void*>(GetProcAddress(s_driver_data.user32.dll, "GetDpiForWindow")));
        s_driver_data.user32.AreDpiAwarenessContextsEqual = reinterpret_cast<decltype(s_driver_data.user32.AreDpiAwarenessContextsEqual)>(
            reinterpret_cast<void*>(GetProcAddress(s_driver_data.user32.dll, "AreDpiAwarenessContextsEqual")));
        s_driver_data.user32.IsValidDpiAwarenessContext = reinterpret_cast<decltype(s_driver_data.user32.IsValidDpiAwarenessContext)>(
            reinterpret_cast<void*>(GetProcAddress(s_driver_data.user32.dll, "IsValidDpiAwarenessContext")));
    }

    {
        s_driver_data.shcore.dll = LoadLibrary(L"shcore.dll");
        if (!s_driver_data.shcore.dll)
        {
            VX_ERROR(error::error_code::PLATFORM_ERROR) << "Failed to load shcore.dll";
            return false;
        }

        s_driver_data.shcore.GetDpiForMonitor = reinterpret_cast<decltype(s_driver_data.shcore.GetDpiForMonitor)>(
            reinterpret_cast<void*>(GetProcAddress(s_driver_data.shcore.dll, "GetDpiForMonitor")));
        s_driver_data.shcore.SetProcessDpiAwareness = reinterpret_cast<decltype(s_driver_data.shcore.SetProcessDpiAwareness)>(
            reinterpret_cast<void*>(GetProcAddress(s_driver_data.shcore.dll, "SetProcessDpiAwareness")));
    }

    return true;
}

static void free_libraries()
{
    if (s_driver_data.user32.dll)
    {
        FreeLibrary(s_driver_data.user32.dll);
    }
    if (s_driver_data.shcore.dll)
    {
        FreeLibrary(s_driver_data.shcore.dll);
    }
}

bool video_impl::set_dpi_awareness(process_dpi_awareness awareness)
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

process_dpi_awareness video_impl::get_dpi_awareness()
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

bool video_impl::init()
{
    if (!load_libraries())
    {
        return false;
    }
    if (!set_dpi_awareness(process_dpi_awareness::SYSTEM))
    {
        return false;
    }

    return true;
}

void video_impl::quit()
{
    if (s_driver_data.window_default_icon != NULL)
    {
        DestroyIcon(s_driver_data.window_default_icon);
        s_driver_data.window_default_icon = NULL;
    }

    if (s_driver_data.blank_cursor != NULL)
    {
        DestroyCursor(s_driver_data.blank_cursor);
        s_driver_data.blank_cursor = NULL;
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

bool display::display_impl::get_display_mode(
    const std::wstring& device_name,
    std::unique_ptr<display_mode::display_mode_impl>& mode,
    DWORD index,
    display_orientation* orientation
)
{
    DEVMODE dm{ sizeof(dm) };

    if (!EnumDisplaySettingsW(device_name.c_str(), index, &dm))
    {
        return false;
    }

    if (dm.dmBitsPerPel == 0)
    {
        return false;
    }

    mode = std::make_unique<display_mode::display_mode_impl>();

    mode->data.resolution.x = dm.dmPelsWidth;
    mode->data.resolution.y = dm.dmPelsHeight;
    mode->data.bpp = dm.dmBitsPerPel;
    mode->data.pixel_format = pixel::pixel_format::PIXEL_FORMAT_UNKNOWN; // TODO: figure out how to get the pixel format
    mode->data.pixel_density = 1.0f;
    mode->data.refresh_rate = get_refresh_rate(dm.dmDisplayFrequency);

    mode->driver_data.devmode = dm;

    if (orientation)
    {
        *orientation = get_display_orientation(&dm);
    }

    return true;
}

void display::display_impl::list_display_modes()
{
    data.modes.clear();
    DWORD display_mode_index = 0;

    display_mode mode;

    while (true)
    {
        if (!get_display_mode(driver_data.device_name, mode.m_impl, display_mode_index++, nullptr))
        {
            break;
        }

        // don't add duplicates
        if (std::find(data.modes.begin(), data.modes.end(), mode) == data.modes.end())
        {
            data.modes.push_back(mode);
        }
    }
}

bool display::display_impl::set_display_mode(display_mode& mode)
{
    LONG status = ChangeDisplaySettingsExW(driver_data.device_name.c_str(), &mode.m_impl->driver_data.devmode, NULL, CDS_FULLSCREEN, NULL);

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

        VX_ERROR(error::error_code::PLATFORM_ERROR) << "ChangeDisplaySettingsEx() failed: " << reason;

        return false;
    }

    EnumDisplaySettingsW(driver_data.device_name.c_str(), ENUM_CURRENT_SETTINGS, &mode.m_impl->driver_data.devmode);
    get_display_mode(driver_data.device_name.c_str(), mode.m_impl, ENUM_CURRENT_SETTINGS, nullptr);

    return true;
}

static math::vec2 get_display_content_scale(const HMONITOR handle)
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

///////////////////////////////////////////////////////////////////////////////
// display bounds
///////////////////////////////////////////////////////////////////////////////

bool video::display::display_impl::get_bounds(math::recti& bounds) const
{
    MONITORINFO info{ sizeof(info) };

    if (!GetMonitorInfo(driver_data.handle, &info))
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

    if (!GetMonitorInfo(driver_data.handle, &info))
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
    size_t index;
    bool find_primary;
    std::vector<std::unique_ptr<video::display>>* displays;
};

bool video::display::display_impl::create_display(
    size_t& index,
    std::vector<std::unique_ptr<display>>& displays,
    HMONITOR hMonitor,
    const MONITORINFOEX* info
)
{
    display_mode current_mode;
    display_orientation current_orientation;

    if (!get_display_mode(info->szDevice, current_mode.m_impl, ENUM_CURRENT_SETTINGS, &current_orientation))
    {
        return false;
    }

    math::vec2 current_content_scale = get_display_content_scale(hMonitor);

    bool found = false;

    // Check to see if the monitor already exists
    for (size_t i = 0; i < displays.size(); ++i)
    {
        display* d = displays[i].get();
        display_data& data = d->m_impl->data;
        display_driver_data& driver_data = d->m_impl->driver_data;

        if (driver_data.device_name == info->szDevice)
        {
            bool moved = (index != i);

            if (driver_data.state != display_state::REMOVED)
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

            driver_data.handle = hMonitor;
            driver_data.state = display_state::NONE;

            {
                math::recti current_bounds = d->get_bounds();

                if (moved || d->get_bounds() != driver_data.bounds)
                {
                    // moved
                    driver_data.bounds = current_bounds;
                    driver_data.state |= display_state::MOVED;
                }
                if (current_orientation != data.orientation)
                {
                    // orientation change
                    data.orientation = current_orientation;
                    driver_data.state |= display_state::ORIENTATION_CHANGED;
                }

                data.content_scale = current_content_scale;
            }

            found = true;
            break;
        }
    }

    if (!found)
    {
        display* d = displays.emplace_back(std::make_unique<display>()).get();
        d->m_impl = std::make_unique<display_impl>();

        display_data& data = d->m_impl->data;
        display_driver_data& driver_data = d->m_impl->driver_data;

        driver_data.handle = hMonitor;
        driver_data.device_name = info->szDevice; // Unique identifier for the monitor determined by graphics card
        driver_data.state = display_state::ADDED; // Mark as added by default

        data.id = get_next_device_id();

        // Get the display device name (printable)
        {
            DISPLAY_DEVICE dev{ sizeof(dev) };

            if (EnumDisplayDevicesW(info->szDevice, 0, &dev, NULL))
            {
                data.name = str::wstring_to_string(dev.DeviceString);
            }
        }

        {
            data.current_mode = data.desktop_mode = current_mode;
            data.content_scale = current_content_scale;
            driver_data.bounds = d->get_bounds();
        }
    }

    return true;
}

BOOL CALLBACK display::display_impl::enum_displays_callback(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam)
{
    poll_display_data& data = *(poll_display_data*)(lParam);

    MONITORINFOEX info{ sizeof(info) };

    if (GetMonitorInfo(hMonitor, (MONITORINFO*)(&info)))
    {
        if (data.find_primary == static_cast<bool>(info.dwFlags & MONITORINFOF_PRIMARY))
        {
            display::display_impl::create_display(data.index, *data.displays, hMonitor, &info);
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

void display::display_impl::update_displays(std::vector<std::unique_ptr<display>>& displays)
{
    poll_display_data data{ 0 };
    data.displays = &displays;

    for (auto& d : displays)
    {
        d->m_impl->driver_data.state = display_state::REMOVED;
    }

    data.find_primary = true;
    poll_displays_internal(data, enum_displays_callback);

    data.find_primary = false;
    poll_displays_internal(data, enum_displays_callback);

    // remove displays still marked as removed
    auto it = displays.begin();
    while (it != displays.end())
    {
        auto& d = *it;

        if (d->m_impl->driver_data.state == display_state::REMOVED)
        {
            post_display_event(event_type::DISPLAY_REMOVED, *d);
            it = displays.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // update existing displays
    for (auto& d : displays)
    {
        if (d->m_impl->driver_data.state & display_state::MOVED)
        {
            post_display_event(event_type::DISPLAY_MOVED, *d);
        }
        if (d->m_impl->driver_data.state & display_state::ORIENTATION_CHANGED)
        {
            post_display_event(event_type::DISPLAY_ORIENTATION_CHANGED, *d);
        }
    }

    // add new displays
    for (auto& d : displays)
    {
        if (d->m_impl->driver_data.state == display_state::ADDED)
        {
            post_display_event(event_type::DISPLAY_ADDED, *d);
        }
    }
}

}
}
}