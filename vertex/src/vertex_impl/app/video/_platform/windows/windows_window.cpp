#include "vertex_impl/app/video/_platform/windows/windows_window.hpp"
#include "vertex_impl/app/event/_platform/windows/windows_event.hpp"
#include "vertex/util/string/string_cast.hpp"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// DWM support
///////////////////////////////////////////////////////////////////////////////

static void update_window_handle_theme(HWND hwnd)
{
    os::shared_library dwmapi;
    if (dwmapi.load("dwmapi.dll"))
    {
        auto DwmSetWindowAttribute = dwmapi.get<DwmSetWindowAttribute_t>("DwmSetWindowAttribute");
        if (DwmSetWindowAttribute)
        {
            const BOOL value = (video::get_system_theme() == video::system_theme::DARK) ? TRUE : FALSE;
            DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
        }
    }
}

_priv::window_impl::window_impl()
    : m_owner(nullptr)
    , m_handle(NULL)
    , m_icon(NULL)
    , m_expected_resize(false)
    , m_floating_rect_pending(false)
    , m_windowed_mode_was_maximized(false)
    , m_losing_focus(false)
    , m_cleared(false)
    , m_windowed_mode_corner_rounding(DWMWCP_DEFAULT)
    , m_dwma_border_color(DWMWA_COLOR_DEFAULT) {}

_priv::window_impl::~window_impl()
{
    destroy();
}

bool _priv::window_impl::create(window* w)
{
    if (!w)
    {
        // critical error
        return false;
    }

    m_owner = w;

    // Get the window style
    DWORD style = get_window_style();
    DWORD ex_style = get_window_ex_style();

    // Adjust window size and position to match requested area
    RECT rect{};
    adjust_rect_with_style(rect, window::rect_type::FLOATING, style, ex_style);

    HINSTANCE hInstance = GetModuleHandle(NULL);

    // Create window
    m_handle = CreateWindowEx(
        ex_style,
        s_driver_data.app_name,
        NULL, // We will set the title later
        style,
        rect.left, rect.top,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, // no parent window
        NULL, // no window menu
        hInstance,
        m_owner
    );

    if (!m_handle)
    {
        // critical error
        return false;
    }

    // Set window theme to match system theme
    update_window_handle_theme(m_handle);

    // Why doesn't this dispatch WM_CREATE?
    event::pump_events(true);

    // set up current flags based on the internal style of the window
    {
        style = GetWindowLong(m_handle, GWL_STYLE);

        if (style & WS_VISIBLE)
        {
            m_owner->m_flags &= ~window::flags::HIDDEN;
        }
        else
        {
            m_owner->m_flags |= window::flags::HIDDEN;
        }
        if (style & WS_POPUP)
        {
            m_owner->m_flags |= window::flags::BORDERLESS;
        }
        else
        {
            m_owner->m_flags &= ~window::flags::BORDERLESS;
        }
        if (style & WS_THICKFRAME)
        {
            m_owner->m_flags |= window::flags::RESIZABLE;
        }
        else
        {
            m_owner->m_flags &= ~window::flags::RESIZABLE;
        }
        if (style & WS_MAXIMIZE)
        {
            m_owner->m_flags |= window::flags::MAXIMIZED;
        }
        else
        {
            m_owner->m_flags &= ~window::flags::MAXIMIZED;
        }
        if (style & WS_MINIMIZE)
        {
            m_owner->m_flags |= window::flags::MINIMIZED;
        }
        else
        {
            m_owner->m_flags &= ~window::flags::MINIMIZED;
        }
    }

    // Windows imposes a size limit for windows that prevents them from being
    // created in a size larger than the display they are on. If this happens
    // we can catch and override it.

    if (!(m_owner->m_flags & window::flags::MINIMIZED))
    {
        if (GetClientRect(m_handle, &rect) && !IS_RECT_EMPTY(rect))
        {
            if ((m_owner->m_windowed_rect.size.x && m_owner->m_windowed_rect.size.x != rect.right) ||
                (m_owner->m_windowed_rect.size.y && m_owner->m_windowed_rect.size.y != rect.bottom))
            {
                // Window size is currently not what we are expecting it to be
                adjust_rect(rect, window::rect_type::WINDOWED);

                m_expected_resize = true;
                SetWindowPos(
                    m_handle,
                    NULL,
                    rect.left, rect.top,
                    rect.right - rect.left, rect.bottom - rect.top,
                    SWP_NOCOPYBITS | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE
                );
                m_expected_resize = false;
            }
            else
            {
                m_owner->m_size.x = rect.right - rect.left;
                m_owner->m_size.y = rect.bottom - rect.top;

                POINT position{};
                if (ClientToScreen(m_handle, &position))
                {
                    m_owner->m_position.x = position.x;
                    m_owner->m_position.y = position.y;
                }
            }
        }
    }

    if (GetFocus() == m_handle)
    {
        m_owner->m_flags |= window::flags::FOCUSSED;
        //SDL_SetKeyboardFocus(window);
        //WIN_UpdateClipCursor(window);
    }

    set_always_on_top();
    set_bordered();

    return true;
}

void _priv::window_impl::destroy()
{
    // Destroy the custom icon
    clear_icon();

    // Destroy the window
    if (m_handle)
    {
        DestroyWindow(m_handle);
        m_handle = NULL;
    }
}

bool _priv::window_impl::validate() const
{
    return m_owner && m_handle;
}

// =============== style ===============

// define a mask of styles that we want to be able to control ourselves
#define STYLE_MASK (WS_POPUP | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED | WS_THICKFRAME | WS_MAXIMIZEBOX)

DWORD _priv::window_impl::get_window_style() const
{
    DWORD style = (WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

    if (m_owner->m_flags & window::flags::FULLSCREEN)
    {
        style |= (WS_POPUP | WS_MINIMIZEBOX);
    }
    else
    {
        style |= (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);

        if (m_owner->m_flags & window::flags::BORDERLESS)
        {
            style |= WS_POPUP; // (WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
        }
        else
        {
            style |= WS_OVERLAPPED;
        }

        if (m_owner->m_flags & window::flags::RESIZABLE)
        {
            // Via SDL: disallow for borderless resizable windows
            // because Windows doesn't always draw it correctly.
            // https://bugzilla.libsdl.org/show_bug.cgi?id=4466
            if (!(m_owner->m_flags & window::flags::BORDERLESS))
            {
                style |= (WS_THICKFRAME | WS_MAXIMIZEBOX);
            }
        }

        // Via SDL: Need to set initialize minimize style, or when we
        // call ShowWindow with WS_MINIMIZE it will activate a random window.
        if (m_owner->m_flags & window::flags::MINIMIZED)
        {
            style |= WS_MINIMIZE;
        }
    }

    return style;
}

DWORD _priv::window_impl::get_window_ex_style() const
{
    DWORD style = 0;
    return style;
}

void _priv::window_impl::sync_window_style()
{
    DWORD style;
    style = GetWindowLong(m_handle, GWL_STYLE);
    style &= ~STYLE_MASK;
    style |= get_window_style();
    SetWindowLong(m_handle, GWL_STYLE, style);
}

// =============== sync ===============

void _priv::window_impl::sync()
{
    // Does nothing
}

// =============== title ===============

std::string _priv::window_impl::get_title() const
{
    // Check the title length
    int length = GetWindowTextLength(m_handle);

    // Create a buffer to hold the wide title
    std::wstring wtitle(static_cast<size_t>(length), 0);
    GetWindowText(m_handle, reinterpret_cast<LPWSTR>(const_cast<wchar_t*>(wtitle.data())), length);

    // Convert the wide string
    return str::string_cast<char>(wtitle);
}

void _priv::window_impl::set_title(const std::string& title)
{
    SetWindowText(m_handle, str::string_cast<wchar_t>(title).c_str());
}

// =============== position and size ===============

bool _priv::window_impl::adjust_rect_with_style(RECT& rect, window::rect_type rect_type, DWORD style, DWORD ex_style) const
{
    switch (rect_type)
    {
        case window::rect_type::INPUT:
        default:
        {
            break;
        }
        case window::rect_type::CURRENT:
        {
            rect.left = m_owner->m_position.x;
            rect.top = m_owner->m_position.y;
            rect.right = m_owner->m_position.x + m_owner->m_size.x;
            rect.bottom = m_owner->m_position.y + m_owner->m_size.y;

            break;
        }
        case window::rect_type::WINDOWED:
        {
            rect.left = m_owner->m_windowed_rect.position.x;
            rect.top = m_owner->m_windowed_rect.position.y;
            rect.right = m_owner->m_windowed_rect.position.x + m_owner->m_windowed_rect.size.x;
            rect.bottom = m_owner->m_windowed_rect.position.y + m_owner->m_windowed_rect.size.y;

            break;
        }
        case window::rect_type::FLOATING:
        {
            rect.left = m_owner->m_floating_rect.position.x;
            rect.top = m_owner->m_floating_rect.position.y;
            rect.right = m_owner->m_floating_rect.position.x + m_owner->m_floating_rect.size.x;
            rect.bottom = m_owner->m_floating_rect.position.y + m_owner->m_floating_rect.size.y;

            break;
        }
    }

    /* borderless windows will have WM_NCCALCSIZE return 0 for the non-client area. When this happens, it looks like windows will send a resize message
       expanding the window client area to the previous window + chrome size, so shouldn't need to adjust the window size for the set styles.
     */
    if (m_owner->m_flags & window::flags::BORDERLESS)
    {
        return true;
    }

    return AdjustWindowRectEx(&rect, style, (GetMenu(m_handle) != NULL), ex_style);
}

bool _priv::window_impl::adjust_rect(RECT& rect, window::rect_type rect_type) const
{
    return adjust_rect_with_style(
        rect,
        rect_type,
        GetWindowLong(m_handle, GWL_STYLE),
        GetWindowLong(m_handle, GWL_EXSTYLE)
    );
}

void _priv::window_impl::set_position_internal(UINT flags, window::rect_type rect_type)
{
    RECT rect{};
    adjust_rect(rect, rect_type);

    HWND top = (m_owner->m_flags & window::flags::TOPMOST) ? HWND_TOPMOST : HWND_NOTOPMOST;

    m_expected_resize = true;
    const bool result = SetWindowPos(
        m_handle,
        top,
        rect.left, rect.top,
        rect.right - rect.left, rect.bottom - rect.top,
        flags
    );
    m_expected_resize = false;

    assert(result);
}

void _priv::window_impl::set_position()
{
    if (!(m_owner->m_flags & window::flags::FULLSCREEN))
    {
        if (!(m_owner->m_flags & (window::flags::MAXIMIZED | window::flags::MINIMIZED)))
        {
            set_position_internal(SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE, window::rect_type::FLOATING);
        }
        else
        {
            // window::set_position already set the new floating rect position.
            // We will wait to apply the changes until we are in a valid mode.
            m_floating_rect_pending = true;
        }
    }
    else
    {
        m_owner->update_fullscreen_mode(window::fullscreen_op::ENTER, true);
    }
}

void _priv::window_impl::set_size()
{
    if (!(m_owner->m_flags & (window::flags::FULLSCREEN | window::flags::MAXIMIZED)))
    {
        set_position_internal(SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE, window::rect_type::FLOATING);
    }
    else
    {
        m_floating_rect_pending = true;
    }
}

void _priv::window_impl::get_border_size(int32_t& left, int32_t& right, int32_t& bottom, int32_t& top) const
{
    RECT rect{};
    if (!adjust_rect(rect, window::rect_type::INPUT))
    {
        return;
    }

    left = -rect.left;
    right = rect.right;
    bottom = rect.bottom;
    top = -rect.top;
}

void _priv::window_impl::set_resizable()
{
    sync_window_style();
}

void _priv::window_impl::set_bordered()
{
    sync_window_style();
    set_position_internal(SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE, window::rect_type::CURRENT);
}

void _priv::window_impl::set_always_on_top()
{
    sync_window_style();
    set_position_internal(SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE, window::rect_type::CURRENT);
}

// =============== window ops ===============

void _priv::window_impl::show()
{
    ShowWindow(m_handle, SW_SHOW);
}

void _priv::window_impl::hide()
{
    ShowWindow(m_handle, SW_HIDE);
}

void _priv::window_impl::minimize()
{
    ShowWindow(m_handle, SW_MINIMIZE);
}

void _priv::window_impl::maximize()
{
    if (m_owner->is_fullscreen())
    {
        m_windowed_mode_was_maximized = true;
        return;
    }

    m_expected_resize = true;
    ShowWindow(m_handle, SW_MAXIMIZE);
    m_expected_resize = false;

    // Is this needed for min size too?

    const math::vec2i& max_size = m_owner->m_max_size;
    if (max_size.x || max_size.y)
    {
        const math::vec2i& size = m_owner->m_size;
        math::recti& windowed_rect = m_owner->m_windowed_rect;

        windowed_rect.size.x = max_size.x ? math::min(size.x, max_size.x) : windowed_rect.size.x;
        windowed_rect.size.y = max_size.y ? math::min(size.y, max_size.y) : windowed_rect.size.y;

        RECT rect{};
        adjust_rect(rect, window::rect_type::WINDOWED);

        m_expected_resize = true;
        SetWindowPos(
            m_handle,
            HWND_TOP,
            rect.left,
            rect.top,
            rect.right - rect.left,
            rect.bottom - rect.top,
            SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOACTIVATE
        );
        m_expected_resize = false;
    }
}

void _priv::window_impl::restore()
{
    if (m_owner->m_flags & window::flags::FULLSCREEN)
    {
        m_windowed_mode_was_maximized = false;
        return;
    }

    m_expected_resize = true;
    ShowWindow(m_handle, SW_RESTORE);
    m_expected_resize = false;
}

static DWM_WINDOW_CORNER_PREFERENCE update_window_corner_rounding(HWND hwnd, DWM_WINDOW_CORNER_PREFERENCE new_corner_pref)
{
    DWM_WINDOW_CORNER_PREFERENCE old_corner_pref = DWMWCP_DEFAULT;

    HMODULE dwmapi_dll = LoadLibrary(L"dwmapi.dll");
    if (dwmapi_dll)
    {
        DwmGetWindowAttribute_t DwmGetWindowAttribute = reinterpret_cast<DwmGetWindowAttribute_t>(GetProcAddress(dwmapi_dll, "DwmGetWindowAttribute"));
        DwmSetWindowAttribute_t DwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttribute_t>(GetProcAddress(dwmapi_dll, "DwmSetWindowAttribute"));
        if (DwmGetWindowAttribute && DwmSetWindowAttribute)
        {
            DwmGetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &old_corner_pref, sizeof(old_corner_pref));
            DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &new_corner_pref, sizeof(new_corner_pref));
        }

        FreeLibrary(dwmapi_dll);
    }

    return old_corner_pref;
}

static COLORREF update_window_border_color(HWND hwnd, COLORREF new_color_pref)
{
    COLORREF old_color_pref = DWMWA_COLOR_DEFAULT;

    HMODULE dwmapi_dll = LoadLibrary(L"dwmapi.dll");
    if (dwmapi_dll)
    {
        DwmGetWindowAttribute_t DwmGetWindowAttribute = reinterpret_cast<DwmGetWindowAttribute_t>(GetProcAddress(dwmapi_dll, "DwmGetWindowAttribute"));
        DwmSetWindowAttribute_t DwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttribute_t>(GetProcAddress(dwmapi_dll, "DwmSetWindowAttribute"));
        if (DwmGetWindowAttribute && DwmSetWindowAttribute)
        {
            DwmGetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &old_color_pref, sizeof(old_color_pref));
            DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &new_color_pref, sizeof(new_color_pref));
        }

        FreeLibrary(dwmapi_dll);
    }

    return old_color_pref;
}

bool _priv::window_impl::set_fullscreen(window::fullscreen_op fullscreen, const display* d)
{
    // no-op
    if (!(m_owner->m_flags & window::flags::FULLSCREEN) && !fullscreen)
    {
        return true;
    }

    HWND top = is_topmost() ? HWND_TOPMOST : HWND_NOTOPMOST;

    MONITORINFO mi{ sizeof(mi) };
    if (!GetMonitorInfo(video_internal::get_impl(*d)->handle, &mi))
    {
        err::set(err::SYSTEM_ERROR, "GetMonitorInfo()");
        return false;
    }

    if (fullscreen)
    {
        m_owner->post_window_enter_fullscreen();
    }
    else
    {
        m_owner->post_window_leave_fullscreen();
    }
    
    DWORD style = GetWindowLong(m_handle, GWL_STYLE);
    DWORD ex_style = GetWindowLong(m_handle, GWL_EXSTYLE);

    // Mask out any resigual style bits and set the new style bits that reflect
    // the window style changes (to or from fullscreen)
    style &= ~STYLE_MASK;
    style |= get_window_style();

    bool maximize_on_exit = false;
    RECT rect{};

    if (fullscreen)
    {
        rect = mi.rcMonitor;

        // Via SDL: we save the maxmized state of the window to restore later.
        // This fixes https://bugzilla.libsdl.org/show_bug.cgi?id=3215
        if (style & WS_MAXIMIZE)
        {
            m_windowed_mode_was_maximized = true;
            style &= ~WS_MAXIMIZE;
        }

        // Disable corner rounding and border color (Windows 11+) so the window fills the full screen
        m_windowed_mode_corner_rounding = update_window_corner_rounding(m_handle, DWMWCP_DONOTROUND);
        m_dwma_border_color = update_window_border_color(m_handle, DWMWA_COLOR_NONE);
    }
    else
    {
        // Restore corner rounding and border color (Windows 11+)
        update_window_corner_rounding(m_handle, m_windowed_mode_corner_rounding);
        update_window_border_color(m_handle, m_dwma_border_color);

        // Via SDL: we restore the maxmized state of the window.
        // This fixes https://bugzilla.libsdl.org/show_bug.cgi?id=3215
        // Special care is taken to not do this if and when the window
        // is losing focus, which can cause the above bug on some systems.
        if (m_windowed_mode_was_maximized && !m_losing_focus)
        {
            style |= WS_MAXIMIZE;
            maximize_on_exit = true;
        }

        window::rect_type type = m_windowed_mode_was_maximized ? window::rect_type::WINDOWED : window::rect_type::FLOATING;
        adjust_rect_with_style(rect, type, style, ex_style);
        m_windowed_mode_was_maximized = false;
    }

    SetWindowLong(m_handle, GWL_STYLE, style);

    m_expected_resize = true;

    if (!maximize_on_exit)
    {
        SetWindowPos(
            m_handle,
            top,
            rect.left,
            rect.top,
            rect.right - rect.left,
            rect.bottom - rect.top,
            SWP_NOCOPYBITS | SWP_NOACTIVATE
        );
    }
    else
    {
        maximize();
    }

    m_expected_resize = false;

    return true;
}

void _priv::window_impl::focus()
{
    BringWindowToTop(m_handle);
    SetForegroundWindow(m_handle);
    SetFocus(m_handle);
}

bool _priv::window_impl::is_focused() const
{
    return m_handle == GetActiveWindow();
}

void _priv::window_impl::flash(window::flash_op operation)
{
    FLASHWINFO fi{ sizeof(fi) };
    fi.hwnd = m_handle;

    switch (operation)
    {
        case window::flash_op::CANCEL:
        {
            fi.dwFlags = FLASHW_STOP;
            break;
        }
        case window::flash_op::BRIEF:
        {
            fi.dwFlags = FLASHW_TRAY;
            fi.uCount = 1;
            break;
        }
        case window::flash_op::UNTIL_FOCUSED:
        {
            fi.dwFlags = (FLASHW_TRAY | FLASHW_TIMERNOFG);
            break;
        }
        default:
        {
            break;
        }
    }

    FlashWindowEx(&fi);
}

void _priv::window_impl::set_topmost(bool enabled)
{
    set_position_internal(SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE, window::rect_type::CURRENT);
}

bool _priv::window_impl::is_topmost() const
{
    return false;
}

// =============== icon ===============

bool _priv::window_impl::set_icon(const pixel::surface_rgba8& surf)
{
    clear_icon();

    const size_t surf_size = surf.data_size();
    const uint8_t* pixels = surf.data();

    // Convert the surf to an BGRA
    std::vector<uint8_t> formatted_pixels(surf_size);
    for (size_t pixel = 0; pixel < surf_size; pixel += 4)
    {
        formatted_pixels[pixel + 0] = pixels[pixel + 2];
        formatted_pixels[pixel + 1] = pixels[pixel + 1];
        formatted_pixels[pixel + 2] = pixels[pixel + 0];
        formatted_pixels[pixel + 3] = pixels[pixel + 3];
    }

    // MSVC warns about arument 6 being NULL
    VX_DISABLE_MSVC_WARNING_PUSH();
    VX_DISABLE_MSVC_WARNING("", 6387);

    // Create the icon
    m_icon = CreateIcon(
        GetModuleHandle(NULL),
        surf.width(), surf.height(),
        1,
        32,
        NULL,
        formatted_pixels.data()
    );

    VX_DISABLE_MSVC_WARNING_POP();

    if (!m_icon)
    {
        return false;
    }

    // Set it as both big and small icon of the window
    SendMessage(m_handle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(m_icon));
    SendMessage(m_handle, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(m_icon));

    return true;
}

void _priv::window_impl::clear_icon()
{
    if (m_icon)
    {
        DestroyIcon(m_icon);
        m_icon = NULL;
    }
}

// =============== mouse ===============

void _priv::window_impl::set_mouse_tracking(bool enabled)
{
    TRACKMOUSEEVENT e{ sizeof(e) };
    e.dwFlags = enabled ? TME_LEAVE : TME_CANCEL;
    e.hwndTrack = m_handle;
    e.dwHoverTime = 0;
    TrackMouseEvent(&e);
}

void _priv::window_impl::update_mouse_tracking()
{
    //if (!is_hovered())
    //{
    //    if (m_mouse_inside_window)
    //    {
    //        m_mouse_inside_window = false;
    //        set_mouse_tracking(false);
    //
    //        //event e;
    //        //e.type = event_type::MOUSE_HOVER;
    //        //e.mouse_hover.value = false;
    //        //post_event(e);
    //    }
    //}
    //else
    //{
    //    if (!m_mouse_inside_window)
    //    {
    //        m_mouse_inside_window = true;
    //        set_mouse_tracking(true);
    //
    //        //event e;
    //        //e.type = event_type::MOUSE_HOVER;
    //        //e.mouse_hover.value = true;
    //        //post_event(e);
    //    }
    //}
}

math::vec2i _priv::window_impl::get_mouse_position() const
{
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(m_handle, &point);
    return math::vec2i(point.x, point.y);
}

void _priv::window_impl::set_mouse_position(const math::vec2i& position)
{
    POINT point = { position.x, position.y };
    ClientToScreen(m_handle, &point);
    SetCursorPos(point.x, point.y);
}

bool _priv::window_impl::is_hovered() const
{
    //if (!m_visible)
    //{
    //    return false;
    //}
    //
    //POINT point;
    //if (!GetCursorPos(&point))
    //{
    //    return false;
    //}
    //
    //RECT rect{};
    //ScreenToClient(m_handle, &point);
    //GetClientRect(m_handle, &rect);
    //
    //return PtInRect(&rect, point);
    return false;
}

bool _priv::window_impl::get_cursor_visibility() const
{
    //return m_cursor_visible;
    return true;
}

void _priv::window_impl::set_cursor_visibility(bool visible)
{
    //m_cursor_visible = visible;
    //SetCursor(m_cursor_visible ? m_last_cursor : NULL);
}

//cursor window::window_impl::get_cursor() const
//{
//    return m_last_cursor_object;
//}

//bool window::window_impl::set_cursor(cursor cursor)
//{
//    auto it = cursor_data::s_cursor_cache.find(cursor.id());
//    if (it != cursor_data::s_cursor_cache.end())
//    {
//        m_last_cursor_object = cursor;
//        m_last_cursor = it->second.cursor;
//        SetCursor(m_cursor_visible ? m_last_cursor : NULL);
//
//        return true;
//    }
//
//    return false;
//}

bool _priv::window_impl::is_cursor_grabbed() const
{
    //return m_cursor_grabbed;
    return false;
}

void _priv::window_impl::set_cursor_grabbed(bool grabbed)
{
    if (grabbed)
    {
        RECT rect{};
        GetClientRect(m_handle, &rect);
        MapWindowPoints(m_handle, nullptr, reinterpret_cast<LPPOINT>(&rect), 2);
        ClipCursor(&rect);
    }
    else
    {
        ClipCursor(nullptr);
    }
}

} // namespace video
} // namespace app
} // namespace vx