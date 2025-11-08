#include "vertex/util/string/string_cast.hpp"
#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/hints/hints_internal.hpp"
#include "vertex_impl/app/video/_platform/windows/windows_window.hpp"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// hints
///////////////////////////////////////////////////////////////////////////////

static void use_raw_keyboard_hint_watcher(const hint::hint_t name, const char* old_value, const char* new_value, void* user_data)
{
    video_instance_impl* this_ = static_cast<video_instance_impl*>(user_data);
    const bool enabled = hint::parse_boolean(new_value, false);
    //WIN_SetRawKeyboardEnabled(_this, enabled);
}

static void enable_message_loop_hint_watcher(const hint::hint_t name, const char* old_value, const char* new_value, void* user_data)
{
    video_instance_impl* this_ = static_cast<video_instance_impl*>(user_data);
    this_->data.enable_message_loop_hint_cache = hint::parse_boolean(new_value, true);
}

static void enable_menu_mnemonics_hint_watcher(const hint::hint_t name, const char* old_value, const char* new_value, void* user_data)
{
    video_instance_impl* this_ = static_cast<video_instance_impl*>(user_data);
    this_->data.enable_menu_mnemonics_hint_cache = hint::parse_boolean(new_value, false);
}

static void window_frame_usable_while_cursor_hidden_hint_watcher(const hint::hint_t name, const char* old_value, const char* new_value, void* user_data)
{
    video_instance_impl* this_ = static_cast<video_instance_impl*>(user_data);
    this_->data.frame_usable_while_cursor_hidden_hint_cache = hint::parse_boolean(new_value, true);
}

///////////////////////////////////////////////////////////////////////////////
// library loading
///////////////////////////////////////////////////////////////////////////////

bool video_instance_impl::load_libraries()
{
#   define set_fn_ptr(lib, fn) data.lib.fn = data.lib.dll.get<decltype(data.lib.fn)>(#fn)

    // user32
    {
        if (!data.user32.dll.load("user32.dll"))
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
        if (!data.shcore.dll.load("shcore.dll"))
        {
            return false;
        }

        set_fn_ptr(shcore, GetDpiForMonitor);
        set_fn_ptr(shcore, SetProcessDpiAwareness);
    }

#   undef set_fn_ptr
    return true;
}

void video_instance_impl::free_libraries()
{
    data.user32 = user32{};
    data.shcore = shcore{};
}

///////////////////////////////////////////////////////////////////////////////
// dpi
///////////////////////////////////////////////////////////////////////////////

process_dpi_awareness video_instance_impl::get_dpi_awareness() const
{
    if (data.user32.GetAwarenessFromDpiAwarenessContext && data.user32.AreDpiAwarenessContextsEqual)
    {
        DPI_AWARENESS_CONTEXT context = data.user32.GetThreadDpiAwarenessContext();

        if (data.user32.AreDpiAwarenessContextsEqual(context, DPI_AWARENESS_CONTEXT_UNAWARE))
        {
            return process_dpi_awareness::unaware;
        }
        if (data.user32.AreDpiAwarenessContextsEqual(context, DPI_AWARENESS_CONTEXT_SYSTEM_AWARE))
        {
            return process_dpi_awareness::system;
        }
        if (data.user32.AreDpiAwarenessContextsEqual(context, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE))
        {
            return process_dpi_awareness::per_monitor;
        }
    }

    return process_dpi_awareness::unaware;
}

bool video_instance_impl::set_dpi_awareness()
{
    const char* hint = video->app->data.hints_ptr->get_hint_string(hint::video_windows_dpi_awareness, nullptr);

    if (!hint || std::strcmp(hint, "permonitor") == 0)
    {
        return set_dpi_awareness_internal(process_dpi_awareness::per_monitor);
    }
    else if (std::strcmp(hint, "system"))
    {
        return set_dpi_awareness_internal(process_dpi_awareness::system);
    }
    else if (std::strcmp(hint, "unaware"))
    {
        return set_dpi_awareness_internal(process_dpi_awareness::unaware);
    }

    return true;
}

bool video_instance_impl::set_dpi_awareness_internal(process_dpi_awareness awareness)
{
    // https://learn.microsoft.com/en-us/windows/win32/hidpi/setting-the-default-dpi-awareness-for-a-process

    switch (awareness)
    {
        case process_dpi_awareness::unaware:
        {
            if (data.user32.SetProcessDpiAwarenessContext)
            {
                // Windows 10, version 1607
                return data.user32.SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
            }
            if (data.shcore.SetProcessDpiAwareness)
            {
                // Windows 8.1
                return SUCCEEDED(data.shcore.SetProcessDpiAwareness(PROCESS_DPI_UNAWARE));
            }
            break;
        }
        case process_dpi_awareness::system:
        {
            if (data.user32.SetProcessDpiAwarenessContext)
            {
                // Windows 10, version 1607
                return data.user32.SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
            }
            if (data.shcore.SetProcessDpiAwareness)
            {
                // Windows 8.1
                return SUCCEEDED(data.shcore.SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE));
            }
            if (data.user32.SetProcessDPIAware)
            {
                // Windows Vista
                return data.user32.SetProcessDPIAware();
            }
            break;
        }
        case process_dpi_awareness::per_monitor:
        {
            if (data.user32.SetProcessDpiAwarenessContext)
            {
                // Windows 10, version 1607
                return data.user32.SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
            }
            if (data.shcore.SetProcessDpiAwareness)
            {
                // Windows 8.1
                return SUCCEEDED(data.shcore.SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE));
            }
            else
            {
                // Older OS: fall back to system DPI aware
                return set_dpi_awareness_internal(process_dpi_awareness::system);
            }
            break;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
// system theme
///////////////////////////////////////////////////////////////////////////////

system_theme video_instance_impl::get_system_theme() const
{
    LPCWSTR subkey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
    LPCWSTR value = L"AppsUseLightTheme";

    DWORD data = 0;
    DWORD size = sizeof(data);
    DWORD type = 0;

    if (::RegGetValue(HKEY_CURRENT_USER, subkey, value, RRF_RT_REG_DWORD, &type, &data, &size) == ERROR_SUCCESS)
    {
        // Dark mode if 0, light mode if 1
        switch (data)
        {
            case 0:  return system_theme::dark;
            case 1:  return system_theme::light;
            default: break;
        }
    }

    return system_theme::unknown;
}

///////////////////////////////////////////////////////////////////////////////
// app registration
///////////////////////////////////////////////////////////////////////////////

static bool register_app(LPCWSTR name, HINSTANCE hInstance)
{
    VX_ASSERT(name);

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
    wc.lpfnWndProc = window_instance_impl::window_proc;
    wc.hInstance = hInstance; // needed for dll
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.lpszClassName = name;

    return (RegisterClass(&wc) != NULL);
}

static void unregister_app(LPCWSTR name, HINSTANCE hInstance)
{
    VX_ASSERT(name);

    if (!hInstance)
    {
        hInstance = GetModuleHandle(NULL);
    }

    UnregisterClass(name, hInstance);
}

///////////////////////////////////////////////////////////////////////////////
// video_impl
///////////////////////////////////////////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/main/src/video/windows/SDL_windowsvideo.c#L219
// https://github.com/libsdl-org/SDL/blob/main/src/video/windows/SDL_windowsvideo.c#L579

bool video_instance_impl::init(video_instance* owner)
{
    VX_ASSERT(!video);
    VX_ASSERT(owner);
    video = owner;

    // register app
    {
        const char* app_name = video->app->data.metadata.name;
        data.app_name = str::string_cast<wchar_t>(app_name);

        if (!register_app(data.app_name.c_str(), NULL))
        {
            quit();
            return false;
        }

        data.registered_app = true;
    }

    video->data.theme = get_system_theme();

    if (!load_libraries())
    {
        quit();
        return false;
    }

    ///////////////////////////////////////

    // needed for drag and drop support
    if (data.com.initialize())
    {
        data.ole.initialize();
    }

    if (!set_dpi_awareness())
    {
        quit();
        return false;
    }

    if (!init_displays())
    {
        quit();
        return false;
    }

    data._VX_WAKEUP = ::RegisterWindowMessageA("_VX_WAKEUP");

    // hints
    {
        video->app->data.hints_ptr->add_hint_callback_and_default_value(
            hint::video_windows_use_raw_keyboard,
            use_raw_keyboard_hint_watcher,
            this,
            "0"
        );

        video->app->data.hints_ptr->add_hint_callback_and_default_value(
            hint::video_windows_enable_message_loop,
            enable_message_loop_hint_watcher,
            this,
            "1"
        );

        video->app->data.hints_ptr->add_hint_callback_and_default_value(
            hint::video_windows_enable_menu_mnemonics,
            enable_menu_mnemonics_hint_watcher,
            this,
            "0"
        );

        video->app->data.hints_ptr->add_hint_callback_and_default_value(
            hint::video_window_frame_usable_while_cursor_hidden,
            window_frame_usable_while_cursor_hidden_hint_watcher,
            this,
            "1"
        );
    }

    return true;
}

// https://github.com/libsdl-org/SDL/blob/main/src/video/windows/SDL_windowsvideo.c#L647
// https://github.com/libsdl-org/SDL/blob/main/src/video/windows/SDL_windowsvideo.c#L190

void video_instance_impl::quit()
{
    // remove hint callbacks
    if (video)
    {
        video->app->data.hints_ptr->remove_hint_callback(
            hint::video_windows_use_raw_keyboard,
            use_raw_keyboard_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::video_windows_enable_message_loop,
            enable_message_loop_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::video_windows_enable_menu_mnemonics,
            enable_menu_mnemonics_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::video_window_frame_usable_while_cursor_hidden,
            window_frame_usable_while_cursor_hidden_hint_watcher,
            this
        );
    }

    quit_displays();

    data.ole.uninitialize();
    data.com.uninitialize();

    ///////////////////////////////////////

    if (data.registered_app)
    {
        unregister_app(data.app_name.c_str(), NULL);
        data.registered_app = false;
    }

    free_libraries();

    video = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// screen saver
///////////////////////////////////////////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/main/src/video/windows/SDL_windowsvideo.c#L168

bool video_instance_impl::suspend_screen_saver()
{
    DWORD res;

    if (video->data.suspend_screen_saver)
    {
        res = ::SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);
    }
    else
    {
        res = ::SetThreadExecutionState(ES_CONTINUOUS);
    }

    if (res == 0)
    {
        os::windows::error_message("SetThreadExecutionState()");
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// display name
///////////////////////////////////////////////////////////////////////////////

static bool get_display_name_vista(const user32& user32, const WCHAR* device_name, std::string& display_name)
{
    if (!user32.GetDisplayConfigBufferSizes ||
        !user32.QueryDisplayConfig ||
        !user32.DisplayConfigGetDeviceInfo)
    {
        return false;
    }

    LONG rc;

    UINT32 path_count = 0;
    UINT32 mode_count = 0;

    std::vector<DISPLAYCONFIG_PATH_INFO> paths;
    std::vector<DISPLAYCONFIG_MODE_INFO> modes;

    rc = user32.GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &path_count, &mode_count);
    if (rc != ERROR_SUCCESS)
    {
        return false;
    }

    paths.resize(path_count);
    modes.resize(mode_count);

    rc = user32.QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &path_count, paths.data(), &mode_count, modes.data(), 0);

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
        rc = user32.DisplayConfigGetDeviceInfo(&source_name.header);
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

        rc = user32.DisplayConfigGetDeviceInfo(&target_name.header);
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

static math::vec2 get_display_content_scale(const shcore& shcore, const HMONITOR handle)
{
    UINT xdpi = 1;
    UINT ydpi = 1;
    bool dpi_set = false;

    if (shcore.GetDpiForMonitor)
    {
        if (shcore.GetDpiForMonitor(handle, MDT_EFFECTIVE_DPI, &xdpi, &ydpi) == S_OK)
        {
            dpi_set = true;
        }
    }

    if (!dpi_set)
    {
        // Safe default
        xdpi = ydpi = USER_DEFAULT_SCREEN_DPI;

        // Window 8.0 and below: same DPI for all monitors 
        HDC hdc = ::GetDC(NULL);
        if (hdc)
        {
            xdpi = ::GetDeviceCaps(hdc, LOGPIXELSX);
            ydpi = ::GetDeviceCaps(hdc, LOGPIXELSY);
            ::ReleaseDC(NULL, hdc);
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

static display_orientation get_natural_orientation(const PDEVMODE mode)
{
    int w = mode->dmPelsWidth;
    int h = mode->dmPelsHeight;

    if (mode->dmDisplayOrientation == DMDO_90 || mode->dmDisplayOrientation == DMDO_270)
    {
        std::swap(w, h);
    }

    // The best we can do is guess
    return (w >= h) ? video::display_orientation::landscape : video::display_orientation::portrait;
}

static display_orientation get_display_orientation(const PDEVMODE mode, display_orientation* natural_orientation_ptr)
{
    const display_orientation natural_orientation = get_natural_orientation(mode);

    if (natural_orientation_ptr)
    {
        *natural_orientation_ptr = natural_orientation;
    }

    switch (natural_orientation)
    {
        default:
        case display_orientation::landscape:
        {
            switch (mode->dmDisplayOrientation)
            {
                case DMDO_DEFAULT: return display_orientation::landscape;
                case DMDO_90:      return display_orientation::portrait;
                case DMDO_180:     return display_orientation::landscape_flipped;
                case DMDO_270:     return display_orientation::portrait_flipped;
                default:           return display_orientation::unknown;
            }
        }
        case display_orientation::portrait:
        {
            switch (mode->dmDisplayOrientation)
            {
                case DMDO_DEFAULT: return display_orientation::portrait;
                case DMDO_90:      return display_orientation::landscape_flipped;
                case DMDO_180:     return display_orientation::portrait_flipped;
                case DMDO_270:     return display_orientation::landscape;
                default:           return display_orientation::unknown;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// pixel format
///////////////////////////////////////////////////////////////////////////////

static pixel::pixel_format get_pixel_format(DEVMODE dm, LPCWSTR device_name, bool current_mode)
{
    if (current_mode)
    {
        HDC hdc = ::CreateDCW(device_name, NULL, NULL, NULL);
        if (hdc == NULL)
        {
            return pixel::pixel_format::unknown;
        }

        char bmi_data[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)]{};

        LPBITMAPINFO bmi = reinterpret_cast<LPBITMAPINFO>(bmi_data);
        bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

        HBITMAP hbm = ::CreateCompatibleBitmap(hdc, 1, 1);
        ::GetDIBits(hdc, hbm, 0, 1, NULL, bmi, DIB_RGB_COLORS);
        ::GetDIBits(hdc, hbm, 0, 1, NULL, bmi, DIB_RGB_COLORS);

        ::DeleteObject(hbm);
        ::DeleteDC(hdc);

        if (bmi->bmiHeader.biCompression == BI_BITFIELDS)
        {
            switch (*reinterpret_cast<uint32_t*>(bmi->bmiColors))
            {
                case 0x00FF0000: return pixel::pixel_format::argb_8888;
                case 0x000000FF: return pixel::pixel_format::xbgr_8888;
                case 0x0000F800: return pixel::pixel_format::rgb_565;
                case 0x00007C00: return pixel::pixel_format::xrgb_1555;
            }
        }
        else if (bmi->bmiHeader.biCompression == BI_RGB)
        {
            switch (bmi->bmiHeader.biBitCount)
            {
                case 24: return pixel::pixel_format::rgb_8;
                default: break; // palette format (unsupported) 
            }
        }
    }
    else
    {
        if ((dm.dmFields & DM_BITSPERPEL) == DM_BITSPERPEL)
        {
            // not sure this is the best way
            switch (dm.dmBitsPerPel)
            {
                case 32:    return pixel::pixel_format::xrgb_8888;
                case 24:    return pixel::pixel_format::rgb_8;
                case 16:    return pixel::pixel_format::rgb_565;
                case 15:    return pixel::pixel_format::xrgb_1555;
                default:    break; // palette format (unsupported) 
            }
        }
    }

    return pixel::pixel_format::unknown;
}

///////////////////////////////////////////////////////////////////////////////
// display mode
///////////////////////////////////////////////////////////////////////////////

enum class mode_result
{
    add_mode,
    skip_mode,
    exit
};

static mode_result get_display_mode(LPCWSTR device_name, display_mode_instance& mode, DWORD index, display_orientation* orientation, display_orientation* natural_orientation)
{
    DEVMODE dm{ sizeof(dm) };

    if (!::EnumDisplaySettings(device_name, index, &dm))
    {
        return mode_result::exit;
    }

    if (dm.dmBitsPerPel == 0)
    {
        return mode_result::skip_mode;
    }

    const bool is_current_mode = (index == ENUM_CURRENT_SETTINGS);
    const pixel::pixel_format format = get_pixel_format(dm, device_name, is_current_mode);
    if (format == pixel::pixel_format::unknown)
    {
        return mode_result::skip_mode;
    }

    mode.data.mode.resolution.x = dm.dmPelsWidth;
    mode.data.mode.resolution.y = dm.dmPelsHeight;
    mode.data.mode.bpp = dm.dmBitsPerPel;
    mode.data.mode.pixel_format = format;
    mode.data.mode.pixel_density = 1.0f;
    mode.data.mode.refresh_rate = get_refresh_rate(dm.dmDisplayFrequency);

    mode.impl_ptr.reset(new display_mode_instance_impl);
    if (!mode.impl_ptr)
    {
        return mode_result::exit;
    }

    mode.impl_ptr->data.devmode = dm;

    if (orientation)
    {
        *orientation = get_display_orientation(&dm, natural_orientation);
    }

    return mode_result::add_mode;
}

///////////////////////////////////////////////////////////////////////////////
// display polling
///////////////////////////////////////////////////////////////////////////////

struct poll_display_data
{
    video_instance_impl* video = nullptr;
    size_t index; // track the index of the displays we add to our list
    bool find_primary;
    std::vector<display_instance>* displays;
};

bool video_instance_impl::create_display(
    size_t index,
    std::vector<display_instance>& displays,
    HMONITOR hMonitor,
    const MONITORINFOEX* info
) const
{
    display_mode_instance current_mode;
    display_orientation natural_orientation;
    display_orientation current_orientation;

    const mode_result result = get_display_mode(info->szDevice, current_mode, ENUM_CURRENT_SETTINGS, &current_orientation, &natural_orientation);
    if (result != mode_result::add_mode)
    {
        return false;
    }

    const math::vec2 current_content_scale = get_display_content_scale(data.shcore, hMonitor);

    // Prevent adding duplicate displays. Do this after we know the display is
    // ready to be added to allow any displays that we can't fully query to be
    // removed

    bool found = false;
    for (size_t i = 0; i < displays.size(); ++i)
    {
        display_instance& d = displays[i];
        display_instance_impl* d_impl = d.impl_ptr.get();

        if (d_impl->data.device_name == info->szDevice)
        {
            if (d_impl->data.state != display_state::removed)
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
            d_impl->data.handle = hMonitor;
            d_impl->data.state = display_state::none;

            if (!video->data.setting_display_mode)
            {
                d.reset_modes();
                d.set_desktop_mode(current_mode);

                bool changed_bounds = false;
                const math::recti current_bounds = d.get_bounds();
                if (current_bounds != d_impl->data.last_bounds)
                {
                    // moved
                    d_impl->data.last_bounds = current_bounds;
                    changed_bounds = true;
                }

                if (moved || changed_bounds)
                {
                    video->post_display_moved(d.data.id);
                }

                d.set_orientation(current_orientation);
                d.set_content_scale(current_content_scale);
            }

            found = true;
            break;
        }
    }

    if (!found)
    {
        display_instance d;

        d.impl_ptr.reset(new display_instance_impl);
        if (!d.impl_ptr)
        {
            return false;
        }

        display_instance_impl* d_impl = d.impl_ptr.get();
        d_impl->data.handle = hMonitor;
        d_impl->data.device_name = info->szDevice; // Unique identifier for the monitor determined by graphics card
        d_impl->data.state = display_state::added; // Mark as added
        d_impl->data.last_bounds = d.get_bounds();

        // Get the display device name (printable)
        if (!get_display_name_vista(data.user32, info->szDevice, d.data.name))
        {
            DISPLAY_DEVICE dev{ sizeof(dev) };
            if (EnumDisplayDevices(info->szDevice, 0, &dev, NULL))
            {
                d.data.name = str::string_cast<char>(dev.DeviceString);
            }
        }

        d.data.desktop_mode = std::move(current_mode);
        d.data.natural_orientation = natural_orientation;
        d.data.orientation = current_orientation;
        d.data.content_scale = current_content_scale;

        if (!video->add_display(d, false))
        {
            return false;
        }
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
            data.video->create_display(data.index, *data.displays, hMonitor, &info);
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

void video_instance_impl::refresh_displays()
{
    std::vector<display_instance>& displays = video->data.displays;

    poll_display_data poll_data{};
    poll_data.video = this;
    poll_data.displays = &displays;

    // mark all displays as invalid to detect
    // entries that have actually been removed
    for (const auto& d : displays)
    {
        d.impl_ptr->data.state = display_state::removed;
    }

    // first locate the primary display
    poll_data.find_primary = true;
    poll_displays_internal(poll_data, enum_displays_callback);

    // second pass for secondary monitors
    poll_data.find_primary = false;
    poll_displays_internal(poll_data, enum_displays_callback);

    // remove any unaccounted for displays
    for (size_t i = 0; i < displays.size();)
    {
        if (displays[i].impl_ptr->data.state == display_state::removed)
        {
            video->remove_display(displays[i].data.id, true);
        }
        else
        {
            ++i;
        }
    }

    // add new displays
    for (const auto& d : displays)
    {
        if (d.impl_ptr->data.state == display_state::added)
        {
            video->post_display_added(d.data.id);
        }
    }
}

bool video_instance_impl::init_displays()
{
    refresh_displays();
    return true;
}

void video_instance_impl::quit_displays()
{

}

///////////////////////////////////////////////////////////////////////////////
// display modes
///////////////////////////////////////////////////////////////////////////////

void display_instance_impl::list_display_modes(const display_instance* display) const
{
    DWORD display_mode_index = 0;
    display_mode_instance mode;

    while (true)
    {
        const mode_result result = get_display_mode(data.device_name.c_str(), mode, display_mode_index++, nullptr, nullptr);

        if (result == mode_result::add_mode)
        {
            display->add_mode(mode);
        }
        else if (result == mode_result::exit)
        {
            break;
        }
    }
}

// https://github.com/libsdl-org/SDL/blob/main/src/video/windows/SDL_windowsmodes.c#L826

bool display_instance_impl::set_display_mode(display_mode_instance& mode, bool is_desktop_mode)
{
    LONG status;

    if (is_desktop_mode)
    {
        // NOTE: Passing NULL for DEVMODE* here tells Windows to restore the OS’s saved
        // desktop mode (resolution + DPI scaling). This avoids unintended DPI changes
        // that can occur if you pass a copied DEVMODE, even if the pixel dimensions match.
        status = ::ChangeDisplaySettingsExW(data.device_name.c_str(), NULL, NULL, CDS_FULLSCREEN, NULL);
    }
    else
    {
        status = ::ChangeDisplaySettingsExW(data.device_name.c_str(), &mode.impl_ptr->data.devmode, NULL, CDS_FULLSCREEN, NULL);
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

        VX_ERR(vx::err::system_error) << "ChangeDisplaySettingsEx(): " << reason;
        return false;
    }

    if (is_desktop_mode)
    {
        // Refresh stored DEVMODE with the actual settings now active
        ::EnumDisplaySettingsW(data.device_name.c_str(), ENUM_CURRENT_SETTINGS, &mode.impl_ptr->data.devmode);
    }

    return true;
}

bool display_instance_impl::get_bounds(math::recti& bounds) const
{
    MONITORINFO info{ sizeof(info) };

    if (!::GetMonitorInfo(data.handle, &info))
    {
        os::windows::error_message("GetMonitorInfo()");
        return false;
    }

    bounds.position.x = info.rcMonitor.left;
    bounds.position.y = info.rcMonitor.top;
    bounds.size.x = info.rcMonitor.right - info.rcMonitor.left;
    bounds.size.y = info.rcMonitor.bottom - info.rcMonitor.top;

    return true;
}

bool display_instance_impl::get_work_area(math::recti& work_area) const
{
    MONITORINFO info{ sizeof(info) };

    if (!::GetMonitorInfo(data.handle, &info))
    {
        os::windows::error_message("GetMonitorInfo()");
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