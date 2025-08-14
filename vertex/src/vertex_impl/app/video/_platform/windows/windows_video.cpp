#include "vertex_impl/app/video/_platform/windows/windows_window.hpp"
#include "vertex/util/string/string_cast.hpp"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// library loading
///////////////////////////////////////////////////////////////////////////////

static bool load_libraries()
{
#   define set_fn_ptr(lib, fn) s_driver_data.lib.fn = s_driver_data.lib.dll.get<decltype(s_driver_data.lib.fn)>(#fn)

    // user32
    {
        if (!s_driver_data.user32.dll.load("user32.dll"))
        {
            return false;
        }

        set_fn_ptr(user32, SetProcessDPIAware);
        set_fn_ptr(user32, SetProcessDpiAwarenessContext);
        set_fn_ptr(user32, SetThreadDpiAwarenessContext);
        set_fn_ptr(user32, GetThreadDpiAwarenessContext);
        set_fn_ptr(user32, GetAwarenessFromDpiAwarenessContext);
        set_fn_ptr(user32, EnableNonClientDpiScaling);
        set_fn_ptr(user32, AdjustWindowRectExForDpi);
        set_fn_ptr(user32, GetDpiForWindow);
        set_fn_ptr(user32, AreDpiAwarenessContextsEqual);
        set_fn_ptr(user32, IsValidDpiAwarenessContext);
        set_fn_ptr(user32, GetDisplayConfigBufferSizes);
        set_fn_ptr(user32, QueryDisplayConfig);
        set_fn_ptr(user32, DisplayConfigGetDeviceInfo);
    }

    // shcore
    {
        if (!s_driver_data.shcore.dll.load("shcore.dll"))
        {
            return false;
        }

        set_fn_ptr(shcore, GetDpiForMonitor);
        set_fn_ptr(shcore, SetProcessDpiAwareness);
    }

#   undef set_fn_ptr
    return true;
}

static void free_libraries()
{
    s_driver_data.user32.dll.free();
    s_driver_data.shcore.dll.free();
}

///////////////////////////////////////////////////////////////////////////////
// dpi
///////////////////////////////////////////////////////////////////////////////

process_dpi_awareness get_dpi_awareness_impl()
{
    if (s_driver_data.user32.GetAwarenessFromDpiAwarenessContext && s_driver_data.user32.AreDpiAwarenessContextsEqual)
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

static bool set_dpi_awareness(process_dpi_awareness awareness)
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

///////////////////////////////////////////////////////////////////////////////
// app registration
///////////////////////////////////////////////////////////////////////////////

LPCWSTR driver_data::app_name = L"Vertex_App";

static bool register_app(LPCWSTR name, HINSTANCE hInstance)
{
    if (!name)
    {
        name = driver_data::app_name;
    }

    if (!hInstance)
    {
        hInstance = GetModuleHandle(NULL);
    }

    UINT style = 0;
#if defined(CS_BYTEALIGNCLIENT) || defined(CS_OWNDC)
    style = (CS_BYTEALIGNCLIENT | CS_OWNDC);
#endif

    WNDCLASSW wc{};
    wc.style = style;
    wc.lpfnWndProc = _priv::window_impl::window_proc;
    wc.hInstance = hInstance; // needed for dll
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.lpszClassName = name;

    return (RegisterClass(&wc) != NULL);
}

static void unregister_app(LPCWSTR name, HINSTANCE hInstance)
{
    if (!name)
    {
        name = driver_data::app_name;
    }

    if (!hInstance)
    {
        hInstance = GetModuleHandle(NULL);
    }

    UnregisterClass(name, hInstance);
}

///////////////////////////////////////////////////////////////////////////////
// video_impl
///////////////////////////////////////////////////////////////////////////////

driver_data s_driver_data = driver_data{};

bool init_impl()
{
    if (!load_libraries())
    {
        return false;
    }

    if (!set_dpi_awareness(process_dpi_awareness::UNAWARE))
    {
        return false;
    }

    if (!s_driver_data.registered_app)
    {
        s_driver_data.registered_app = register_app(NULL, NULL);
        
        if (!s_driver_data.registered_app)
        {
            return false;
        }
    }

    return true;
}

void quit_impl()
{
    free_libraries();

    if (!s_driver_data.registered_app)
    {
        unregister_app(NULL, NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////
// system theme
///////////////////////////////////////////////////////////////////////////////

system_theme get_system_theme_impl()
{
    LPCWSTR subkey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
    LPCWSTR value = L"AppsUseLightTheme";

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
// display name
///////////////////////////////////////////////////////////////////////////////

static bool get_display_name_vista(const WCHAR* device_name, std::string& display_name)
{
    if (!s_driver_data.user32.GetDisplayConfigBufferSizes ||
        !s_driver_data.user32.QueryDisplayConfig ||
        !s_driver_data.user32.DisplayConfigGetDeviceInfo)
    {
        return false;
    }

    LONG rc;

    UINT32 path_count = 0;
    UINT32 mode_count = 0;

    std::vector<DISPLAYCONFIG_PATH_INFO> paths;
    std::vector<DISPLAYCONFIG_MODE_INFO> modes;

    rc = s_driver_data.user32.GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &path_count, &mode_count);
    if (rc != ERROR_SUCCESS)
    {
        return false;
    }

    paths.resize(path_count);
    modes.resize(mode_count);

    rc = s_driver_data.user32.QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &path_count, paths.data(), &mode_count, modes.data(), 0);

    if (rc != ERROR_SUCCESS)
    {
        return false;
    }

    for (size_t i = 0; i < path_count; ++i)
    {
        DISPLAYCONFIG_SOURCE_DEVICE_NAME source_name{};

        source_name.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        source_name.header.size = sizeof(source_name);
        source_name.header.adapterId = paths[i].sourceInfo.adapterId;
        source_name.header.id = paths[i].sourceInfo.id;

        // retrieve the source (GPU output) GDI device name
        rc = s_driver_data.user32.DisplayConfigGetDeviceInfo(&source_name.header);
        if (rc != ERROR_SUCCESS)
        {
            return false;
        }

        if (std::wcscmp(device_name, source_name.viewGdiDeviceName) != 0)
        {
            // we have the wrong device, skip
            continue;
        }

        DISPLAYCONFIG_TARGET_DEVICE_NAME target_name{};

        target_name.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        target_name.header.size = sizeof(target_name);
        target_name.header.adapterId = paths[i].targetInfo.adapterId;
        target_name.header.id = paths[i].targetInfo.id;

        rc = s_driver_data.user32.DisplayConfigGetDeviceInfo(&target_name.header);
        if (rc != ERROR_SUCCESS)
        {
            return false;
        }

        display_name = str::string_cast<char>(target_name.monitorFriendlyDeviceName);
        break;
    }

    return !display_name.empty();
}

///////////////////////////////////////////////////////////////////////////////
// display content scale
///////////////////////////////////////////////////////////////////////////////

static math::vec2 get_display_content_scale(const HMONITOR handle)
{
    UINT xdpi = 1;
    UINT ydpi = 1;
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
// display refresh rate
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

///////////////////////////////////////////////////////////////////////////////
// display orientation
///////////////////////////////////////////////////////////////////////////////

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
// display mode
///////////////////////////////////////////////////////////////////////////////

bool _priv::display_impl::get_display_mode(const WCHAR* device_name, display_mode& mode, DWORD index, display_orientation* orientation)
{
    DEVMODE dm{ sizeof(dm) };

    if (!EnumDisplaySettings(device_name, index, &dm))
    {
        return false;
    }

    if (dm.dmBitsPerPel == 0)
    {
        return false;
    }

    mode.resolution.x = dm.dmPelsWidth;
    mode.resolution.y = dm.dmPelsHeight;
    mode.bpp = dm.dmBitsPerPel;
    mode.pixel_format = pixel::pixel_format::UNKNOWN; // TODO: figure out how to get the pixel format
    mode.pixel_density = 1.0f;
    mode.refresh_rate = get_refresh_rate(dm.dmDisplayFrequency);

    mode.m_impl = std::make_unique<_priv::display_mode_impl>();
    mode.m_impl->devmode = dm;

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
    size_t index; // track the index of the displays we add to our list
    bool find_primary;
    std::vector<owner_ptr<video::display>>* displays;
};

bool _priv::display_impl::create_display(
    size_t index,
    std::vector<owner_ptr<display>>& displays,
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

    const math::vec2 current_content_scale = get_display_content_scale(hMonitor);

    // Prevent adding duplicate displays. Do this after we know the display is
    // ready to be added to allow any displays that we can't fully query to be
    // removed

    bool found = false;
    for (size_t i = 0; i < displays.size(); ++i)
    {
        display* d = displays[i].get();
        _priv::display_impl* d_impl = video_internal::get_impl(*d);

        if (d_impl->device_name == info->szDevice)
        {
            if (d_impl->state != display_state::REMOVED)
            {
                // This display has already been enumerated and is still valid;
                // no need to re-add or move it.
                return false;
            }

            if (index >= displays.size())
            {
                // This should never happen due to the check above, but just in case...
                return false;
            }

            const bool moved = (index != i);
            if (moved)
            {
                // Display was found at a different index than expected; it has moved.
                // Swap it into the correct position to maintain display order.
                std::swap(displays[index], displays[i]);
            }

            // Reactivate the display and update its state.
            d_impl->handle = hMonitor;
            d_impl->state = display_state::NONE;

            if (!s_video_data.setting_display_mode)
            {
                const math::recti current_bounds = d->get_bounds();
                if (moved || d->get_bounds() != d_impl->last_bounds)
                {
                    // moved
                    d_impl->last_bounds = current_bounds;
                    video_internal::post_display_moved(*d);
                }

                video_internal::post_display_orientation_changed(*d, current_orientation);
                video_internal::post_display_content_scale_changed(*d, current_content_scale);
            }

            found = true;
            break;
        }
    }

    if (!found)
    {
        displays.emplace_back(new display);
        display* d = displays.back().get();
        _priv::display_impl* d_impl = video_internal::get_impl(*d);

        d_impl->handle = hMonitor;
        d_impl->device_name = info->szDevice; // Unique identifier for the monitor determined by graphics card
        d_impl->state = display_state::ADDED; // Mark as added
        d_impl->last_bounds = d->get_bounds();

        d->m_id = generate_device_id();

        // Get the display device name (printable)
        if (!get_display_name_vista(info->szDevice, d->m_name))
        {
            DISPLAY_DEVICE dev{ sizeof(dev) };
            if (EnumDisplayDevices(info->szDevice, 0, &dev, NULL))
            {
                d->m_name = str::string_cast<char>(dev.DeviceString);
            }
        }

        current_mode.m_display_id = d->m_id;
        d->m_desktop_mode = d->m_current_mode = current_mode;
        d->m_orientation = current_orientation;
        d->m_content_scale = current_content_scale;
    }

    return true;
}

// will be called for each display
static BOOL CALLBACK enum_displays_callback(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam)
{
    poll_display_data& data = *(poll_display_data*)(lParam);

    MONITORINFOEX info{ sizeof(info) };

    if (GetMonitorInfo(hMonitor, (MONITORINFO*)(&info)))
    {
        const bool is_primary = (info.dwFlags & MONITORINFOF_PRIMARY);

        // proceed if we found what we are looking for
        if (data.find_primary == is_primary)
        {
            _priv::display_impl::create_display(data.index, *data.displays, hMonitor, &info);
            ++data.index;

            if (data.find_primary)
            {
                // stop enumeration
                return FALSE;
            }
        }
    }

    // return TRUE to continue enumerating more monitors
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

// https://github.com/libsdl-org/SDL/blob/561c99ee1171f680088ff98c773de2efe94b0f5e/src/video/windows/SDL_windowsmodes.c#L887

void update_displays_impl(std::vector<owner_ptr<display>>& displays)
{
    poll_display_data data{};
    data.displays = &displays;

    // mark all displays as invalid to detect
    // entries that have actually been removed
    for (const auto& d : displays)
    {
        _priv::video_internal::get_impl(*d)->state = display_state::REMOVED;
    }

    // first locate the primary display
    data.find_primary = true;
    poll_displays_internal(data, enum_displays_callback);

    // second pass for secondary monitors
    data.find_primary = false;
    poll_displays_internal(data, enum_displays_callback);

    // remove any unaccounted for displays
    auto it = displays.begin();
    while (it != displays.end())
    {
        const display& d = *(it->get());

        if (_priv::video_internal::get_impl(d)->state == display_state::REMOVED)
        {
            video_internal::post_display_removed(d);
            it = displays.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // add new displays
    for (const auto& d : displays)
    {
        if (_priv::video_internal::get_impl(*d.get())->state == display_state::ADDED)
        {
            video_internal::post_display_added(*d);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// display modes
///////////////////////////////////////////////////////////////////////////////

void _priv::display_impl::list_display_modes(std::vector<display_mode>& modes) const
{
    DWORD display_mode_index = 0;
    display_mode mode;

    while (true)
    {
        if (!get_display_mode(device_name.c_str(), mode, display_mode_index++, nullptr))
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

// https://github.com/libsdl-org/SDL/blob/main/src/video/windows/SDL_windowsmodes.c#L826

bool _priv::display_impl::set_display_mode(display_mode& mode, bool is_desktop_mode) const
{
    LONG status;

    if (is_desktop_mode)
    {
        // NOTE: Passing NULL for DEVMODE* here tells Windows to restore the OS’s saved
        // desktop mode (resolution + DPI scaling). This avoids unintended DPI changes
        // that can occur if you pass a copied DEVMODE, even if the pixel dimensions match.
        status = ::ChangeDisplaySettingsExW(device_name.c_str(), NULL, NULL, CDS_FULLSCREEN, NULL);
    }
    else
    {
        status = ::ChangeDisplaySettingsExW(device_name.c_str(), &mode.m_impl->devmode, NULL, CDS_FULLSCREEN, NULL);
    }

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

        VX_ERR(vx::err::SYSTEM_ERROR) << "ChangeDisplaySettingsEx(): " << reason;
        return false;
    }

    if (is_desktop_mode)
    {
        // Refresh stored DEVMODE with the actual settings now active
        ::EnumDisplaySettingsW(device_name.c_str(), ENUM_CURRENT_SETTINGS, &mode.m_impl->devmode);
    }

    return true;
}

bool _priv::display_impl::get_bounds(math::recti& bounds) const
{
    MONITORINFO info{ sizeof(info) };

    if (!GetMonitorInfo(handle, &info))
    {
        return false;
    }

    bounds.position.x = info.rcMonitor.left;
    bounds.position.y = info.rcMonitor.top;
    bounds.size.x = info.rcMonitor.right - info.rcMonitor.left;
    bounds.size.y = info.rcMonitor.bottom - info.rcMonitor.top;

    return true;
}

bool _priv::display_impl::get_work_area(math::recti& work_area) const
{
    MONITORINFO info{ sizeof(info) };

    if (!GetMonitorInfo(handle, &info))
    {
        return false;
    }

    work_area.position.x = info.rcWork.left;
    work_area.position.y = info.rcWork.top;
    work_area.size.x = info.rcWork.right - info.rcWork.left;
    work_area.size.y = info.rcWork.bottom - info.rcWork.top;

    return true;
}

} // namespace video
} // namespace app
} // namespace vx