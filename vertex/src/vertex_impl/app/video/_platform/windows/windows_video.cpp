#include "vertex_impl/app/video/_platform/windows/windows_window.hpp"
#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/hints/hints_internal.hpp"
#include "vertex/util/string/string_cast.hpp"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// hints
///////////////////////////////////////////////////////////////////////////////

static void use_raw_keyboard_hint_watcher(const hint::hint_t name, const char* old_value, const char* new_value, void* user_data)
{
    video_instance_impl* this_ = static_cast<video_instance_impl*>(user_data);
    const bool enabled = hint::parse_boolean(new_value, VX_HINT_GET_DEFAULT_VALUE(hint::HINT_VIDEO_WINDOWS_USE_RAW_KEYBOARD));
    //WIN_SetRawKeyboardEnabled(_this, enabled);
}

static void enable_message_loop_hint_watcher(const hint::hint_t name, const char* old_value, const char* new_value, void* user_data)
{
    video_instance_impl* this_ = static_cast<video_instance_impl*>(user_data);
    this_->data.enable_message_loop_hint_cache = hint::parse_boolean(new_value, VX_HINT_GET_DEFAULT_VALUE(hint::HINT_VIDEO_WINDOWS_ENABLE_MESSAGE_LOOP));
}

static void enable_menu_mnemonics_hint_watcher(const hint::hint_t name, const char* old_value, const char* new_value, void* user_data)
{
    video_instance_impl* this_ = static_cast<video_instance_impl*>(user_data);
    this_->data.enable_menu_mnemonics_hint_cache = hint::parse_boolean(new_value, VX_HINT_GET_DEFAULT_VALUE(hint::HINT_VIDEO_WINDOWS_ENABLE_MENU_MNEMONICS));
}

static void window_frame_usable_while_cursor_hidden_hint_watcher(const hint::hint_t name, const char* old_value, const char* new_value, void* user_data)
{
    video_instance_impl* this_ = static_cast<video_instance_impl*>(user_data);
    this_->data.frame_usable_while_cursor_hidden_hint_cache = hint::parse_boolean(new_value, VX_HINT_GET_DEFAULT_VALUE(hint::HINT_VIDEO_WINDOW_FRAME_USABLE_WHILE_CURSOR_HIDDEN));
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
            return process_dpi_awareness::UNAWARE;
        }
        if (data.user32.AreDpiAwarenessContextsEqual(context, DPI_AWARENESS_CONTEXT_SYSTEM_AWARE))
        {
            return process_dpi_awareness::SYSTEM;
        }
        if (data.user32.AreDpiAwarenessContextsEqual(context, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE))
        {
            return process_dpi_awareness::PER_MONITOR;
        }
    }

    return process_dpi_awareness::UNAWARE;
}

bool video_instance_impl::set_dpi_awareness(process_dpi_awareness awareness)
{
    // https://learn.microsoft.com/en-us/windows/win32/hidpi/setting-the-default-dpi-awareness-for-a-process

    switch (awareness)
    {
        case process_dpi_awareness::UNAWARE:
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
        case process_dpi_awareness::SYSTEM:
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
        case process_dpi_awareness::PER_MONITOR:
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

    data.system_theme_cache = get_system_theme();

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

    if (!set_dpi_awareness(process_dpi_awareness::UNAWARE))
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
            hint::HINT_VIDEO_WINDOWS_USE_RAW_KEYBOARD,
            use_raw_keyboard_hint_watcher,
            this,
            VX_HINT_GET_DEFAULT_VALUE(hint::HINT_VIDEO_WINDOWS_USE_RAW_KEYBOARD)
        );

        video->app->data.hints_ptr->add_hint_callback_and_default_value(
            hint::HINT_VIDEO_WINDOWS_ENABLE_MESSAGE_LOOP,
            enable_message_loop_hint_watcher,
            this,
            VX_HINT_GET_DEFAULT_VALUE(hint::HINT_VIDEO_WINDOWS_ENABLE_MESSAGE_LOOP)
        );

        video->app->data.hints_ptr->add_hint_callback_and_default_value(
            hint::HINT_VIDEO_WINDOWS_ENABLE_MENU_MNEMONICS,
            enable_menu_mnemonics_hint_watcher,
            this,
            VX_HINT_GET_DEFAULT_VALUE(hint::HINT_VIDEO_WINDOWS_ENABLE_MENU_MNEMONICS)
        );

        video->app->data.hints_ptr->add_hint_callback_and_default_value(
            hint::HINT_VIDEO_WINDOW_FRAME_USABLE_WHILE_CURSOR_HIDDEN,
            window_frame_usable_while_cursor_hidden_hint_watcher,
            this,
            VX_HINT_GET_DEFAULT_VALUE(hint::HINT_VIDEO_WINDOW_FRAME_USABLE_WHILE_CURSOR_HIDDEN)
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
            hint::HINT_VIDEO_WINDOWS_USE_RAW_KEYBOARD,
            use_raw_keyboard_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::HINT_VIDEO_WINDOWS_ENABLE_MESSAGE_LOOP,
            enable_message_loop_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::HINT_VIDEO_WINDOWS_ENABLE_MENU_MNEMONICS,
            enable_menu_mnemonics_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::HINT_VIDEO_WINDOW_FRAME_USABLE_WHILE_CURSOR_HIDDEN,
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
// system theme
///////////////////////////////////////////////////////////////////////////////

system_theme video_instance_impl::get_system_theme()
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
            case 0:  return system_theme::DARK;
            case 1:  return system_theme::LIGHT;
            default: break;
        }
    }

    return system_theme::UNKNOWN;
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
    return (w >= h) ? video::display_orientation::LANDSCAPE : video::display_orientation::PORTRAIT;
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
        case display_orientation::LANDSCAPE:
        {
            switch (mode->dmDisplayOrientation)
            {
                case DMDO_DEFAULT: return display_orientation::LANDSCAPE;
                case DMDO_90:      return display_orientation::PORTRAIT;
                case DMDO_180:     return display_orientation::LANDSCAPE_FLIPPED;
                case DMDO_270:     return display_orientation::PORTRAIT_FLIPPED;
                default:           return display_orientation::UNKNOWN;
            }
        }
        case display_orientation::PORTRAIT:
        {
            switch (mode->dmDisplayOrientation)
            {
                case DMDO_DEFAULT: return display_orientation::PORTRAIT;
                case DMDO_90:      return display_orientation::LANDSCAPE_FLIPPED;
                case DMDO_180:     return display_orientation::PORTRAIT_FLIPPED;
                case DMDO_270:     return display_orientation::LANDSCAPE;
                default:           return display_orientation::UNKNOWN;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// display mode
///////////////////////////////////////////////////////////////////////////////

static bool get_display_mode(const WCHAR* device_name, display_mode_instance& mode, DWORD index, display_orientation* orientation, display_orientation* natural_orientation)
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

    mode.data.mode.resolution.x = dm.dmPelsWidth;
    mode.data.mode.resolution.y = dm.dmPelsHeight;
    mode.data.mode.bpp = dm.dmBitsPerPel;
    mode.data.mode.pixel_format = pixel::pixel_format::UNKNOWN; // TODO: figure out how to get the pixel format
    mode.data.mode.pixel_density = 1.0f;
    mode.data.mode.refresh_rate = get_refresh_rate(dm.dmDisplayFrequency);

    mode.impl_ptr.reset(new display_mode_instance_impl);
    if (!mode.impl_ptr)
    {
        return false;
    }

    mode.impl_ptr->data.devmode = dm;

    if (orientation)
    {
        *orientation = get_display_orientation(&dm, natural_orientation);
    }

    return true;
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

    if (!get_display_mode(info->szDevice, current_mode, ENUM_CURRENT_SETTINGS, &current_orientation, &natural_orientation))
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
            if (d_impl->data.state != display_state::REMOVED)
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
            d_impl->data.state = display_state::NONE;

            if (!video->data.setting_display_mode)
            {
                d.data.modes.clear();
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
        {
            display_instance tmp;

            tmp.impl_ptr.reset(new display_instance_impl);
            if (!tmp.impl_ptr)
            {
                return false;
            }

            displays.emplace_back(std::move(tmp));
        }

        display_instance& d = displays.back();
        d.video = video;
        d.data.id = video->data.display_id_generator.next();

        display_instance_impl* d_impl = d.impl_ptr.get();
        d_impl->data.handle = hMonitor;
        d_impl->data.device_name = info->szDevice; // Unique identifier for the monitor determined by graphics card
        d_impl->data.state = display_state::ADDED; // Mark as added
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

        current_mode.data.mode.display = d.data.id;
        d.data.current_mode = current_mode.data.mode;
        d.data.desktop_mode = std::move(current_mode);
        d.data.natural_orientation = natural_orientation;
        d.data.orientation = current_orientation;
        d.data.content_scale = current_content_scale;
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
        d.impl_ptr->data.state = display_state::REMOVED;
    }

    // first locate the primary display
    poll_data.find_primary = true;
    poll_displays_internal(poll_data, enum_displays_callback);

    // second pass for secondary monitors
    poll_data.find_primary = false;
    poll_displays_internal(poll_data, enum_displays_callback);

    // remove any unaccounted for displays
    auto it = displays.begin();
    while (it != displays.end())
    {
        const display_instance& d = *it;

        if (d.impl_ptr->data.state == display_state::REMOVED)
        {
            video->post_display_removed(d.data.id);
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
        if (d.impl_ptr->data.state == display_state::ADDED)
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

void display_instance_impl::list_display_modes(std::vector<display_mode_instance>& modes) const
{
    DWORD display_mode_index = 0;
    display_mode_instance mode;

    while (true)
    {
        if (!get_display_mode(data.device_name.c_str(), mode, display_mode_index++, nullptr, nullptr))
        {
            break;
        }

        // don't add duplicates
        bool found = false;
        for (const display_mode_instance& m : modes)
        {
            if (compare_display_modes(m.data.mode, mode.data.mode))
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            modes.push_back(std::move(mode));
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

        VX_ERR(vx::err::SYSTEM_ERROR) << "ChangeDisplaySettingsEx(): " << reason;
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

    if (!GetMonitorInfo(data.handle, &info))
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

    if (!GetMonitorInfo(data.handle, &info))
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