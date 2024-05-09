#include "win32_video.h"
#include "vertex/system/error.h"
#include "vertex/system/log.h"
#include "vertex/system/string/string_fn.h"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// dll
///////////////////////////////////////////////////////////////////////////////

static bool load_libraries()
{
    {
        driver.user32.dll = LoadLibrary(L"user32.dll");
        if (!driver.user32.dll)
        {
            VX_ERROR(error::error_code::PLATFORM_ERROR) << "Failed to load user32.dll";
            return false;
        }

        driver.user32.SetProcessDPIAware = reinterpret_cast<decltype(driver.user32.SetProcessDPIAware)>(
            reinterpret_cast<void*>(GetProcAddress(driver.user32.dll, "SetProcessDPIAware")));
        driver.user32.SetProcessDpiAwarenessContext = reinterpret_cast<decltype(driver.user32.SetProcessDpiAwarenessContext)>(
            reinterpret_cast<void*>(GetProcAddress(driver.user32.dll, "SetProcessDpiAwarenessContext")));
        driver.user32.SetThreadDpiAwarenessContext = reinterpret_cast<decltype(driver.user32.SetThreadDpiAwarenessContext)>(
            reinterpret_cast<void*>(GetProcAddress(driver.user32.dll, "SetThreadDpiAwarenessContext")));
        driver.user32.GetThreadDpiAwarenessContext = reinterpret_cast<decltype(driver.user32.GetThreadDpiAwarenessContext)>(
            reinterpret_cast<void*>(GetProcAddress(driver.user32.dll, "GetThreadDpiAwarenessContext")));
        driver.user32.GetAwarenessFromDpiAwarenessContext = reinterpret_cast<decltype(driver.user32.GetAwarenessFromDpiAwarenessContext)>(
            reinterpret_cast<void*>(GetProcAddress(driver.user32.dll, "GetAwarenessFromDpiAwarenessContext")));
        driver.user32.EnableNonClientDpiScaling = reinterpret_cast<decltype(driver.user32.EnableNonClientDpiScaling)>(
            reinterpret_cast<void*>(GetProcAddress(driver.user32.dll, "EnableNonClientDpiScaling")));
        driver.user32.AdjustWindowRectExForDpi = reinterpret_cast<decltype(driver.user32.AdjustWindowRectExForDpi)>(
            reinterpret_cast<void*>(GetProcAddress(driver.user32.dll, "AdjustWindowRectExForDpi")));
        driver.user32.GetDpiForWindow = reinterpret_cast<decltype(driver.user32.GetDpiForWindow)>(
            reinterpret_cast<void*>(GetProcAddress(driver.user32.dll, "GetDpiForWindow")));
        driver.user32.AreDpiAwarenessContextsEqual = reinterpret_cast<decltype(driver.user32.AreDpiAwarenessContextsEqual)>(
            reinterpret_cast<void*>(GetProcAddress(driver.user32.dll, "AreDpiAwarenessContextsEqual")));
        driver.user32.IsValidDpiAwarenessContext = reinterpret_cast<decltype(driver.user32.IsValidDpiAwarenessContext)>(
            reinterpret_cast<void*>(GetProcAddress(driver.user32.dll, "IsValidDpiAwarenessContext")));
    }

    {
        driver.shcore.dll = LoadLibrary(L"shcore.dll");
        if (!driver.shcore.dll)
        {
            VX_ERROR(error::error_code::PLATFORM_ERROR) << "Failed to load shcore.dll";
            return false;
        }

        driver.shcore.GetDpiForMonitor = reinterpret_cast<decltype(driver.shcore.GetDpiForMonitor)>(
            reinterpret_cast<void*>(GetProcAddress(driver.shcore.dll, "GetDpiForMonitor")));
        driver.shcore.SetProcessDpiAwareness = reinterpret_cast<decltype(driver.shcore.SetProcessDpiAwareness)>(
            reinterpret_cast<void*>(GetProcAddress(driver.shcore.dll, "SetProcessDpiAwareness")));
    }

    return true;
}

static bool set_dpi_awareness(process_dpi_awareness awareness)
{
    // https://learn.microsoft.com/en-us/windows/win32/hidpi/setting-the-default-dpi-awareness-for-a-process

    switch (awareness)
    {
        case process_dpi_awareness::UNAWARE:
        {
            if (driver.user32.SetProcessDpiAwarenessContext)
            {
                // Windows 10, version 1607
                return driver.user32.SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
            }
            if (driver.shcore.SetProcessDpiAwareness)
            {
                // Windows 8.1
                return SUCCEEDED(driver.shcore.SetProcessDpiAwareness(PROCESS_DPI_UNAWARE));
            }
            break;
        }
        case process_dpi_awareness::SYSTEM:
        {
            if (driver.user32.SetProcessDpiAwarenessContext)
            {
                // Windows 10, version 1607
                return driver.user32.SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
            }
            if (driver.shcore.SetProcessDpiAwareness)
            {
                // Windows 8.1
                return SUCCEEDED(driver.shcore.SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE));
            }
            if (driver.user32.SetProcessDPIAware)
            {
                // Windows Vista
                return driver.user32.SetProcessDPIAware();
            }
            break;
        }
        case process_dpi_awareness::PER_MONITOR:
        {
            if (driver.user32.SetProcessDpiAwarenessContext)
            {
                // Windows 10, version 1607
                return driver.user32.SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
            }
            if (driver.shcore.SetProcessDpiAwareness)
            {
                // Windows 8.1
                return SUCCEEDED(driver.shcore.SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE));
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

///////////////////////////////////////////////////////////////////////////////
// dpi
///////////////////////////////////////////////////////////////////////////////

static process_dpi_awareness get_dpi_awareness()
{
    if (driver.user32.dll && driver.user32.GetAwarenessFromDpiAwarenessContext && driver.user32.AreDpiAwarenessContextsEqual)
    {
        DPI_AWARENESS_CONTEXT context = driver.user32.GetThreadDpiAwarenessContext();

        if (driver.user32.AreDpiAwarenessContextsEqual(context, DPI_AWARENESS_CONTEXT_UNAWARE))
        {
            return process_dpi_awareness::UNAWARE;
        }
        if (driver.user32.AreDpiAwarenessContextsEqual(context, DPI_AWARENESS_CONTEXT_SYSTEM_AWARE))
        {
            return process_dpi_awareness::SYSTEM;
        }
        if (driver.user32.AreDpiAwarenessContextsEqual(context, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE))
        {
            return process_dpi_awareness::PER_MONITOR;
        }
    }

    return process_dpi_awareness::UNAWARE;
}

///////////////////////////////////////////////////////////////////////////////
// displays
///////////////////////////////////////////////////////////////////////////////

struct poll_display_data
{
    size_t index = 0;
    std::vector<display_data> displays;
    size_t primary_monitor_index = 0;
    bool found_primary_monitor = false;
};

static float get_content_scale(HMONITOR hMonitor)
{
    int dpi = 0;

    if (driver.shcore.GetDpiForMonitor)
    {
        UINT hdpi_uint, vdpi_uint;
        if (driver.shcore.GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &hdpi_uint, &vdpi_uint) == S_OK)
        {
            dpi = static_cast<int>(hdpi_uint);
        }
    }
    if (dpi == 0)
    {
        // Window 8.0 and below: same DPI for all monitors 
        HDC hdc = GetDC(NULL);
        if (hdc)
        {
            dpi = GetDeviceCaps(hdc, LOGPIXELSX);
            ReleaseDC(NULL, hdc);
        }
    }
    if (dpi == 0)
    {
        // Safe default
        dpi = USER_DEFAULT_SCREEN_DPI;
    }

    return dpi / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
}

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

static bool get_display_mode(display_mode_data& data, HMONITOR hMonitor, LPCWSTR deviceName, DWORD iModeNum)
{
    DEVMODE dm{};
    dm.dmSize = sizeof(dm);

    if (!EnumDisplaySettingsW(deviceName, iModeNum, &dm))
    {
        return false;
    }

    data.mode.width = dm.dmPelsWidth;
    data.mode.height = dm.dmPelsHeight;
    data.mode.refresh_rate = get_refresh_rate(dm.dmDisplayFrequency);

    data.device_mode = dm;

}

static bool create_display(size_t index, display_data& data, HMONITOR hMonitor, const MONITORINFOEX* info)
{
    data.display.id = index;
    data.display.active = true;

    data.monitor_handle = hMonitor;
    data.device_name = str::wstring_to_string(info->szDevice);

    {
        DISPLAY_DEVICE dev{};
        dev.cb = sizeof(dev);

        DWORD physical_monitor_index = 0;
        if (EnumDisplayDevicesW(info->szDevice, physical_monitor_index++, &dev, NULL))
        {
            data.display.name = str::wstring_to_string(dev.DeviceString);
        }
    }

    {
        data.display.bounds = math::recti(
            info->rcMonitor.left,
            info->rcMonitor.top,
            info->rcMonitor.right - info->rcMonitor.left,
            info->rcMonitor.bottom - info->rcMonitor.top
        );

        data.display.work_area = math::recti(
            info->rcWork.left,
            info->rcWork.top,
            info->rcWork.right - info->rcWork.left,
            info->rcWork.bottom - info->rcWork.top
        );
    }

    return true;
}

static BOOL CALLBACK enum_display_monitors_callback(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam)
{
    poll_display_data data = *(poll_display_data*)(lParam);

    MONITORINFOEX info{};
    info.cbSize = sizeof(info);

    if (GetMonitorInfoW(hMonitor, (MONITORINFO*)(&info)))
    {
        if (info.dwFlags & MONITORINFOF_PRIMARY)
        {
            data.primary_monitor_index = data.index;
            data.found_primary_monitor = true;
        }

        VX_LOG_INFO << "Display: " << str::wstring_to_string(info.szDevice);

        display_data display;
        create_display(data.index, display, hMonitor, &info);
    }

    ++data.index;

    return TRUE;
}

static void poll_displays()
{
    // NOTE: This will enumerate all unique displays. If a monitor is being
    // extended by another monitor, each will be listed as a unique display.
    // If a monitor is being duplicated accross other monitors, only the
    // original monitor will be listed. Each unique display corresponds to a
    // display slot on the graphics card being used. Different graphics cards
    // may support different number of displays.

    poll_display_data data;
    EnumDisplayMonitors(NULL, NULL, enum_display_monitors_callback, (LPARAM)&data);
}

///////////////////////////////////////////////////////////////////////////////
// init and quit
///////////////////////////////////////////////////////////////////////////////

bool init()
{
    if (!load_libraries())
    {
        return false;
    }
    if (!set_dpi_awareness(process_dpi_awareness::SYSTEM))
    {
        return false;
    }

    poll_displays();

    return true;
}

void quit()
{
    if (driver.user32.dll)
    {
        FreeLibrary(driver.user32.dll);
    }
    if (driver.shcore.dll)
    {
        FreeLibrary(driver.shcore.dll);
    }
}

}
}
}