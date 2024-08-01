#include "win32_window.h"
#include "vertex/app/event/event.h"
#include "vertex/system/string/string_fn.h"
#include "vertex/system/error.h"

namespace vx {
namespace app {

#define IS_RECT_EMPTY(rect) ((rect.right <= rect.left) || (rect.bottom <= rect.top))

// =============== window init helpers ===============

// DWM setting support

static void update_window_handle_theme(HWND hwnd)
{
    HMODULE dwmapi_dll = LoadLibrary(L"dwmapi.dll");
    if (dwmapi_dll)
    {
        DwmSetWindowAttribute_t DwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttribute_t>(
            GetProcAddress(dwmapi_dll, "DwmSetWindowAttribute"));
        if (DwmSetWindowAttribute)
        {
            BOOL value = (video::get_system_theme() == video::system_theme::DARK) ? TRUE : FALSE;
            DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
        }

        FreeLibrary(dwmapi_dll);
    }
}

void video::window::window_impl::create(window& w)
{
    w.m_impl = std::make_unique<window_impl>(&w);
    if (w.m_impl->m_handle == NULL)
    {
        w.m_impl.reset();
    }
}

video::window::window_impl::window_impl(window* w)
    : m_owner(w)
    , m_handle(NULL)
    , m_icon(NULL)
    , m_expected_resize(false)
    , m_floating_rect_pending(false)
    , m_windowed_mode_was_maximized(false)
    , m_in_window_deactivation(false)
    , m_cleared(false)
{
    if (!m_owner)
    {
        // critical error
        return;
    }

    // Adjust window size and position to match requested area

    DWORD style = get_window_style();
    DWORD ex_style = get_window_ex_style();

    RECT rect{};
    adjust_rect_with_style(rect, window_rect_type::FLOATING, style, ex_style);

    // Create window
    m_handle = CreateWindowEx(
        ex_style,
        VIDEO_DRIVER_DATA.window_class_name,
        str::string_to_wstring(m_owner->m_title).c_str(),
        style,
        rect.left, rect.top,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, // no parent window
        NULL, // no window menu
        VIDEO_DRIVER_DATA.instance,
        m_owner
    );

    if (m_handle == NULL)
    {
        // critical error
        return;
    }

    // Set window theme to match system theme
    update_window_handle_theme(m_handle);
    event::pump_events(true);

    // set up current flags based on the internal style of the window
    {
        style = GetWindowLong(m_handle, GWL_STYLE);

        if (style & WS_VISIBLE)
        {
            m_owner->m_flags &= ~flags::HIDDEN;
        }
        else
        {
            m_owner->m_flags |= flags::HIDDEN;
        }
        if (style & WS_POPUP)
        {
            m_owner->m_flags |= flags::BORDERLESS;
        }
        else
        {
            m_owner->m_flags &= ~flags::BORDERLESS;
        }
        if (style & WS_THICKFRAME)
        {
            m_owner->m_flags |= flags::RESIZABLE;
        }
        else
        {
            m_owner->m_flags &= ~flags::RESIZABLE;
        }
        if (style & WS_MAXIMIZE)
        {
            m_owner->m_flags |= flags::MAXIMIZED;
        }
        else
        {
            m_owner->m_flags &= ~flags::MAXIMIZED;
        }
        if (style & WS_MINIMIZE)
        {
            m_owner->m_flags |= flags::MINIMIZED;
        }
        else
        {
            m_owner->m_flags &= ~flags::MINIMIZED;
        }
    }

    // Windows imposes a size limit for windows that prevents them from being
    // created in a size larger than the display they are on. If this happens
    // we can catch and override it.

    if (!(m_owner->m_flags & flags::MINIMIZED))
    {
        if (GetClientRect(m_handle, &rect) && !IS_RECT_EMPTY(rect))
        {
            if ((m_owner->m_windowed_rect.size.x && m_owner->m_windowed_rect.size.x != rect.right) ||
                (m_owner->m_windowed_rect.size.y && m_owner->m_windowed_rect.size.y != rect.bottom))
            {
                // Window size is currently not what we are expecting it to be
                adjust_rect(rect, window_rect_type::WINDOWED);

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
                m_owner->m_position.x = rect.left;
                m_owner->m_position.y = rect.top;
                m_owner->m_size.x = rect.right - rect.left;
                m_owner->m_size.y = rect.bottom - rect.top;
            }
        }
    }

    if (GetFocus() == m_handle)
    {
        m_owner->m_flags |= flags::FOCUSSED;
        //SDL_SetKeyboardFocus(window);
        //WIN_UpdateClipCursor(window);
    }

    // Update the position to allow topmost flag to be set if desired
    set_position_internal(SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE, window_rect_type::CURRENT);
}

video::window::window_impl::~window_impl()
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

bool video::window::window_impl::validate() const
{
    return m_owner && m_handle;
}

// =============== style ===============

// define a mask of styles that we want to be able to control ourselves

#define STYLE_MASK (WS_POPUP | WS_MINIMIZEBOX | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME)

DWORD video::window::window_impl::get_window_style() const
{
    DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    if (m_owner->m_flags & flags::FULLSCREEN)
    {
        style |= WS_POPUP | WS_MINIMIZEBOX;
    }
    else
    {
        style |= WS_SYSMENU | WS_MINIMIZEBOX;

        if (m_owner->m_flags & flags::BORDERLESS)
        {
            style |= WS_POPUP;
        }
        else
        {
            style |= WS_CAPTION;

            if (m_owner->m_flags & flags::RESIZABLE)
            {
                style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
            }
        }
    }

    return style;
}

DWORD video::window::window_impl::get_window_ex_style() const
{
    DWORD style = 0;
    return style;
}

void video::window::window_impl::sync_window_style()
{
    DWORD style;
    style = GetWindowLong(m_handle, GWL_STYLE);
    style &= ~STYLE_MASK;
    style |= get_window_style();
    SetWindowLong(m_handle, GWL_STYLE, style);
}

// =============== events ===============

LRESULT CALLBACK video::window::window_impl::window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    window* win = nullptr;
    LRESULT return_code = -1;

    if (Msg == WM_CREATE)
    {
        // From the data passed into CreateWindow, we can extract the last
        // argument which was a pointer to the window instance. We can then
        // set the user data of the win32 window to a pointer to our window.
        CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lParam);
        win = reinterpret_cast<window*>(create_struct->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(win));
    }
    else
    {
        // Get the pointer to our associated window
        win = reinterpret_cast<window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (!win || !win->validate())
    {
        return CallWindowProc(DefWindowProc, hWnd, Msg, wParam, lParam);
    }

    window_impl* win_impl = win->m_impl.get();

    switch (Msg)
    {
        ///////////////////////////////////////////////////////////////////////////////
        // window events
        ///////////////////////////////////////////////////////////////////////////////

        case WM_CLOSE:
        {
            win->post_window_close_requested();
            return_code = 0;
            break;
        }

        case WM_SHOWWINDOW:
        {
            if (wParam)
            {
                win->post_window_shown();
            }
            else
            {
                win->post_window_hidden();
            }

            break;
        }

        case WM_WINDOWPOSCHANGING:
        {
            if (win_impl->m_expected_resize)
            {
                return_code = 0;
            }

            // If we are going from a fixed size state to a floating state, we
            // update the position to reflect the cached floating rect position
            // and size.
            if (win_impl->m_floating_rect_pending &&
                !IsIconic(hWnd) &&
                !IsZoomed(hWnd) &&
                (win->m_flags & (flags::MINIMIZED | flags::MAXIMIZED)) &&
                !(win->m_flags & flags::FULLSCREEN))
            {
                WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);

                RECT rect{};
                win_impl->adjust_rect(rect, window_rect_type::FLOATING);

                wp->x = rect.left;
                wp->y = rect.top;
                wp->cx = rect.right - rect.left;
                wp->cy = rect.bottom - rect.top;
                wp->flags &= ~(SWP_NOSIZE | SWP_NOMOVE);

                win_impl->m_floating_rect_pending = false;
            }

            break;
        }

        case WM_WINDOWPOSCHANGED:
        {
            bool minimized = IsIconic(hWnd);
            bool maximized = IsZoomed(hWnd);

            const WINDOWPOS* wp = reinterpret_cast<const WINDOWPOS*>(lParam);

            if (wp->flags & SWP_SHOWWINDOW)
            {
                win->post_window_shown();
            }

            if (minimized)
            {
                win->post_window_minimized();
            }
            else if (maximized)
            {
                // If we are going from a minimized state to maximized, restore
                // first
                if (win->m_flags & flags::MINIMIZED)
                {
                    win->post_window_restored();
                }

                win->post_window_maximized();
            }
            else if (win->m_flags & (flags::MINIMIZED | flags::MAXIMIZED))
            {
                win->post_window_restored();
            }
            
            if (wp->flags & SWP_HIDEWINDOW)
            {
                win->post_window_hidden();
            }

            /* When the window is minimized it's resized to the dock icon size, ignore this */
            if (minimized)
            {
                break;
            }

            if (win->m_initializing)
            {
                break;
            }

            RECT rect{};

            if (GetClientRect(hWnd, &rect) && !IS_RECT_EMPTY(rect))
            {
                ClientToScreen(hWnd, reinterpret_cast<LPPOINT>(&rect));
                ClientToScreen(hWnd, reinterpret_cast<LPPOINT>(&rect) + 1);

                win->post_window_moved(rect.left, rect.top);
            }

            /* Moving the window from one display to another can change the size of the window (in the handling of SDL_EVENT_WINDOW_MOVED), so we need to re-query the bounds */
            if (GetClientRect(hWnd, &rect) && !IS_RECT_EMPTY(rect))
            {
                win->post_window_resized(rect.right, rect.bottom);
            }

            // update clip cursor

            /* Forces a WM_PAINT event */
            InvalidateRect(hWnd, NULL, FALSE);

            break;
        }

        case WM_SIZING:
        {
            // If aspect is not locked, skip
            if (win->m_locked_aspect <= 0.0f)
            {
                break;
            }
        
            // Retrieve the rectangle being dragged by the user, which represents
            // the full window's new size and position, including non-client areas.
            RECT* drag_rect = reinterpret_cast<RECT*>(lParam);
            RECT client_drag_rect = *drag_rect;
            RECT rect{};
        
            if (!win_impl->adjust_rect(rect, window_rect_type::INPUT))
            {
                break;
            }
        
            // Subtract the non-client area dimensions (borders, title bar, etc.)
            // from the drag rectangle to calculate the client area size.
            // This ensures that we can maintain the correct client area size or
            // aspect ratio by focusing on the actual drawable area within the window.
            client_drag_rect.left -= rect.left;
            client_drag_rect.top -= rect.top;
            client_drag_rect.right -= rect.right;
            client_drag_rect.bottom -= rect.bottom;
        
            int w = client_drag_rect.right - client_drag_rect.left;
            int h = client_drag_rect.bottom - client_drag_rect.top;
            float new_aspect = static_cast<float>(w) / static_cast<float>(h);
        
            switch (wParam)
            {
                case WMSZ_LEFT:
                case WMSZ_RIGHT:
                {
                    h = static_cast<int>(math::round(w / win->m_locked_aspect));
                    break;
                }
                default:
                {
                    // Resizing by corners or top or bottom
                    w = static_cast<int>(math::round(h * win->m_locked_aspect));
                    break;
                }
            }
        
            switch (wParam)
            {
                case WMSZ_LEFT:
                {
                    client_drag_rect.left = client_drag_rect.right - w;
                    client_drag_rect.top = (client_drag_rect.bottom + client_drag_rect.top - h) / 2;
                    client_drag_rect.bottom = h + client_drag_rect.top;
                    break;
                }
                case WMSZ_BOTTOMLEFT:
                {
                    client_drag_rect.left = client_drag_rect.right - w;
                    client_drag_rect.bottom = h + client_drag_rect.top;
                    break;
                }
                case WMSZ_RIGHT:
                {
                    client_drag_rect.right = w + client_drag_rect.left;
                    client_drag_rect.top = (client_drag_rect.bottom + client_drag_rect.top - h) / 2;
                    client_drag_rect.bottom = h + client_drag_rect.top;
                    break;
                }
                case WMSZ_TOPRIGHT:
                {
                    client_drag_rect.right = w + client_drag_rect.left;
                    client_drag_rect.top = client_drag_rect.bottom - h;
                    break;
                }
                case WMSZ_TOP:
                {
                    client_drag_rect.left = (client_drag_rect.right + client_drag_rect.left - w) / 2;
                    client_drag_rect.right = w + client_drag_rect.left;
                    client_drag_rect.top = client_drag_rect.bottom - h;
                    break;
                }
                case WMSZ_TOPLEFT:
                {
                    client_drag_rect.left = client_drag_rect.right - w;
                    client_drag_rect.top = client_drag_rect.bottom - h;
                    break;
                }
                case WMSZ_BOTTOM:
                {
                    client_drag_rect.left = (client_drag_rect.right + client_drag_rect.left - w) / 2;
                    client_drag_rect.right = w + client_drag_rect.left;
                    client_drag_rect.bottom = h + client_drag_rect.top;
                    break;
                }
                case WMSZ_BOTTOMRIGHT:
                {
                    client_drag_rect.right = w + client_drag_rect.left;
                    client_drag_rect.bottom = h + client_drag_rect.top;
                    break;
                }
            }
        
            /* convert the client rect to a window rect */
            if (!win_impl->adjust_rect(client_drag_rect, window_rect_type::INPUT))
            {
                break;
            }
            
            *drag_rect = client_drag_rect;
        
            break;
        }

        // Fix violations of minimum or maximum size
        case WM_GETMINMAXINFO:
        {
            if (win_impl->m_expected_resize)
            {
                break;
            }

            // Get the size of the window including the frame
            MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
            
            RECT frame{};
            GetWindowRect(hWnd, &frame);

            int x = frame.left;
            int y = frame.top;
            int w = win->m_size.x;
            int h = win->m_size.y;

            // If the window has a border, it must be accounted for in the size
            if (!(win->m_flags & flags::BORDERLESS))
            {
                frame.top = 0;
                frame.left = 0;
                frame.bottom = h;
                frame.right = w;

                win_impl->adjust_rect(frame, window_rect_type::INPUT);
                w = frame.right - frame.left;
                h = frame.bottom - frame.top;
            }
            
            if (win->m_flags & flags::RESIZABLE)
            {
                if (win->m_flags & flags::BORDERLESS)
                {
                    const int screen_w = GetSystemMetrics(SM_CXSCREEN);
                    const int screen_h = GetSystemMetrics(SM_CYSCREEN);
            
                    mmi->ptMaxSize.x = math::max(w, screen_w);
                    mmi->ptMaxSize.y = math::max(h, screen_h);
                    mmi->ptMaxPosition.x = math::min(0, (screen_w - w) / 2);
                    mmi->ptMaxPosition.y = math::max(0, (screen_h - h) / 2);
                }
            
                if (win->m_min_size.x)
                {
                    mmi->ptMinTrackSize.x = win->m_min_size.x + w;
                }
                else
                {
                    mmi->ptMinTrackSize.x = w;
                }
                if (win->m_min_size.y)
                {
                    mmi->ptMinTrackSize.y = win->m_min_size.y + h;
                }
                else
                {
                    mmi->ptMinTrackSize.y = h;
                }
            
                if (win->m_max_size.x)
                {
                    mmi->ptMaxTrackSize.x = win->m_max_size.x + w;
                }
                else
                {
                    mmi->ptMaxTrackSize.x = w;
                }
                if (win->m_max_size.y)
                {
                    mmi->ptMaxTrackSize.y = win->m_max_size.y + h;
                }
                else
                {
                    mmi->ptMaxTrackSize.y = h;
                }
            }
            else
            {
                mmi->ptMaxSize.x = w;
                mmi->ptMaxSize.y = h;
                mmi->ptMaxPosition.x = x;
                mmi->ptMaxPosition.y = y;
                mmi->ptMinTrackSize.x = w;
                mmi->ptMinTrackSize.y = h;
                mmi->ptMaxTrackSize.x = w;
                mmi->ptMaxTrackSize.y = h;
            }
            
            return_code = 0;
            break;
        }

        case WM_NCCALCSIZE:
        {
            if (wParam == TRUE && (win->m_flags & flags::BORDERLESS) && !(win->m_flags & flags::FULLSCREEN))
            {
                // If the window is not resizable, we need to manually adjustthe client area size.
                // This is necessary because, without a border, the default behavior might not
                // correctly calculate the size, leading to layout issues or incorrect rendering.
                if (!(win->m_flags & flags::RESIZABLE))
                {
                    NCCALCSIZE_PARAMS* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);

                    params->rgrc[0].right = params->rgrc[0].left + win->m_floating_rect.size.x;
                    params->rgrc[0].bottom = params->rgrc[0].top + win->m_floating_rect.size.y;
                }

                return 0;
            }

            break;
        }

        // Focus window
        case WM_SETFOCUS:
        {
            //if (!window->m_focussed)
            //{
            //    window->m_focussed = true;
            //
            //    event e;
            //    e.type = event_type::WINDOW_GAINED_FOCUS;
            //    e.window_event.window_id = window->m_window->m_window->m_owner->m_id;
            //    window->post_event(e);
            //}

            break;
        }

        // Un-focus window
        case WM_KILLFOCUS:
        {
            //if (window->m_focussed)
            //{
            //    window->m_focussed = false;
            //
            //    event e;
            //    e.type = event_type::WINDOW_LOST_FOCUS;
            //    e.window_event.window_id = window->m_window->m_window->m_owner->m_id;
            //    window->post_event(e);
            //}

            break;
        }
        case WM_PAINT:
        {
            RECT rect{};

            if (GetUpdateRect(hWnd, &rect, FALSE))
            {
                const LONG ex_style = GetWindowLong(hWnd, GWL_EXSTYLE);

                // Composited windows will continue to receive WM_PAINT
                // messages for update regions until the window is actually
                // painted through Begin/EndPaint.
                if (ex_style & WS_EX_COMPOSITED)
                {
                    PAINTSTRUCT ps;
                    BeginPaint(hWnd, &ps);
                    EndPaint(hWnd, &ps);
                }

                ValidateRect(hWnd, NULL);
            }

            return_code = 0;
            break;
        }

        case WM_ERASEBKGND:
        {
            return 1;
        }

        // =============== mouse events ===============

        // Set the cursor
        case WM_SETCURSOR:
        {
            //if (LOWORD(lParam) == HTCLIENT && window->is_hovered())
            //{
            //    SetCursor(window->m_cursor_visible ? window->m_last_cursor : NULL);
            //    return_code = 0;
            //}

            break;
        }

        // Left mouse button down
        //case WM_LBUTTONDOWN:
        //{
        //    event e;
        //    e.type = event_type::MOUSE_BUTTON_DOWN;
        //    e.mouse_button_event.window_id = window->m_window->m_impl->m_owner->m_id;
        //    e.mouse_button_event.button = mouse::BUTTON_LEFT;
        //    e.mouse_button_event.x = static_cast<int>(LOWORD(lParam));
        //    e.mouse_button_event.y = static_cast<int>(HIWORD(lParam));
        //    window->post_event(e);
        //
        //    break;
        //}
        //
        //// Left mouse button up
        //case WM_LBUTTONUP:
        //{
        //    event e;
        //    e.type = event_type::MOUSE_BUTTON_UP;
        //    e.mouse_button_event.window_id = window->m_window->m_impl->m_owner->m_id;
        //    e.mouse_button_event.button = mouse::BUTTON_LEFT;
        //    e.mouse_button_event.x = static_cast<int>(GET_X_LPARAM(lParam));
        //    e.mouse_button_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
        //    window->post_event(e);
        //
        //    break;
        //}
        //
        //// Right mouse button down
        //case WM_RBUTTONDOWN:
        //{
        //    event e;
        //    e.type = event_type::MOUSE_BUTTON_DOWN;
        //    e.mouse_button_event.window_id = window->m_window->m_impl->m_owner->m_id;
        //    e.mouse_button_event.button = mouse::BUTTON_RIGHT;
        //    e.mouse_button_event.x = static_cast<int>(GET_X_LPARAM(lParam));
        //    e.mouse_button_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
        //    window->post_event(e);
        //
        //    break;
        //}
        //
        //// Right mouse button up
        //case WM_RBUTTONUP:
        //{
        //    event e;
        //    e.type = event_type::MOUSE_BUTTON_UP;
        //    e.mouse_button_event.window_id = window->m_window->m_impl->m_owner->m_id;
        //    e.mouse_button_event.button = mouse::BUTTON_RIGHT;
        //    e.mouse_button_event.x = static_cast<int>(GET_X_LPARAM(lParam));
        //    e.mouse_button_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
        //    window->post_event(e);
        //
        //    break;
        //}
        //
        //// Mouse wheel button down
        //case WM_MBUTTONDOWN:
        //{
        //    event e;
        //    e.type = event_type::MOUSE_BUTTON_DOWN;
        //    e.mouse_button_event.window_id = window->m_window->m_impl->m_owner->m_id;
        //    e.mouse_button_event.button = mouse::BUTTON_MIDDLE;
        //    e.mouse_button_event.x = static_cast<int>(GET_X_LPARAM(lParam));
        //    e.mouse_button_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
        //    window->post_event(e);
        //
        //    break;
        //}
        //
        //// Mouse wheel button up
        //case WM_MBUTTONUP:
        //{
        //    event e;
        //    e.type = event_type::MOUSE_BUTTON_UP;
        //    e.mouse_button_event.window_id = window->m_window->m_impl->m_owner->m_id;
        //    e.mouse_button_event.button = mouse::BUTTON_MIDDLE;
        //    e.mouse_button_event.x = static_cast<int>(GET_X_LPARAM(lParam));
        //    e.mouse_button_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
        //    window->post_event(e);
        //
        //    break;
        //}
        //
        //// Extra mouse button down
        //case WM_XBUTTONDOWN:
        //{
        //    event e;
        //    e.type = event_type::MOUSE_BUTTON_DOWN;
        //    e.mouse_button_event.window_id = window->m_window->m_impl->m_owner->m_id;
        //    e.mouse_button_event.button = (HIWORD(wParam) == XBUTTON1) ? mouse::BUTTON_EXTRA_1 : mouse::button::BUTTON_EXTRA_2;
        //    e.mouse_button_event.x = static_cast<int>(GET_X_LPARAM(lParam));
        //    e.mouse_button_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
        //    window->post_event(e);
        //
        //    break;
        //}
        //
        //// Extra mouse button up
        //case WM_XBUTTONUP:
        //{
        //    event e;
        //    e.type = event_type::MOUSE_BUTTON_UP;
        //    e.mouse_button_event.window_id = window->m_window->m_impl->m_owner->m_id;
        //    e.mouse_button_event.button = (HIWORD(wParam) == XBUTTON1) ? mouse::BUTTON_EXTRA_1 : mouse::button::BUTTON_EXTRA_2;
        //    e.mouse_button_event.x = static_cast<int>(GET_X_LPARAM(lParam));
        //    e.mouse_button_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
        //    window->post_event(e);
        //
        //    break;
        //}
        //
        //// Vertical mouse scroll
        //case WM_MOUSEWHEEL:
        //{
        //    const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        //    const math::vec2i mouse_position = window->get_mouse_position();
        //
        //    event e;
        //    e.type = event_type::MOUSE_WHEEL;
        //    e.mouse_wheel_event.window_id = window->m_window->m_impl->m_owner->m_id;
        //    e.mouse_wheel_event.wheel = mouse::wheel::VERTICAL;
        //    e.mouse_wheel_event.delta = static_cast<float>(delta) / static_cast<float>(WHEEL_DELTA);
        //    e.mouse_wheel_event.x = static_cast<int>(GET_X_LPARAM(lParam));
        //    e.mouse_wheel_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
        //    window->post_event(e);
        //
        //    break;
        //}
        //
        //// Horizontal mouse scroll
        //case WM_MOUSEHWHEEL:
        //{
        //    const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        //    const math::vec2i mouse_position = window->get_mouse_position();
        //
        //    event e;
        //    e.type = event_type::MOUSE_WHEEL;
        //    e.mouse_wheel_event.window_id = window->m_window->m_impl->m_owner->m_id;
        //    e.mouse_wheel_event.wheel = mouse::wheel::HORIZONTAL;
        //    e.mouse_wheel_event.delta = static_cast<float>(delta) / static_cast<float>(WHEEL_DELTA);
        //    e.mouse_wheel_event.x = static_cast<int>(GET_X_LPARAM(lParam));
        //    e.mouse_wheel_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
        //    window->post_event(e);
        //
        //    break;
        //}

        // Mouse move
        case WM_MOUSEMOVE:
        {
            //const math::vec2i new_mouse_position = window->get_mouse_position();
            //
            //if (window->m_last_mouse_position != new_mouse_position)
            //{
            //    window->m_last_mouse_position = new_mouse_position;
            //
            //    event e;
            //    e.type = event_type::MOUSE_MOVED;
            //    e.mouse_motion_event.window_id = window->m_window->m_window->m_owner->m_id;
            //    e.mouse_motion_event.x = new_mouse_position.x;
            //    e.mouse_motion_event.y = new_mouse_position.y;
            //    window->post_event(e);
            //
            //    // Update the cursor tracking incase the cursor moved outside the window.
            //    window->update_mouse_tracking();
            //}

            break;
        }

        // Mouse leave
        //case WM_MOUSELEAVE:
        //{
        //    // Update the cursor tracking incase the cursor moved outside the window.
        //    window->update_mouse_tracking();
        //    break;
        //}
    }

    if (return_code >= 0)
    {
        return return_code;
    }

    return CallWindowProc(DefWindowProc, hWnd, Msg, wParam, lParam);
}

// =============== sync ===============

void video::window::window_impl::sync()
{
    // Does nothing
}

// =============== title ===============

std::string video::window::window_impl::get_title() const
{
    // Check the title length
    int length = GetWindowTextLength(m_handle);

    // Create a buffer to hold the wide title
    std::wstring wtitle(static_cast<size_t>(length), 0);
    GetWindowText(m_handle, reinterpret_cast<LPWSTR>(wtitle.data()), length);

    // Convert the wide string
    return str::wstring_to_string(wtitle);
}

void video::window::window_impl::set_title(const std::string& title)
{
    SetWindowText(m_handle, str::string_to_wstring(title).c_str());
}

// =============== position and size ===============

bool video::window::window_impl::adjust_rect_with_style(RECT& rect, window_rect_type::type rect_type, DWORD style, DWORD ex_style) const
{
    switch (rect_type)
    {
        case window_rect_type::INPUT:
        default:
        {
            break;
        }
        case window_rect_type::CURRENT:
        {
            rect.left = m_owner->m_position.x;
            rect.top = m_owner->m_position.y;
            rect.right = m_owner->m_position.x + m_owner->m_size.x;
            rect.bottom = m_owner->m_position.y + m_owner->m_size.y;

            break;
        }
        case window_rect_type::WINDOWED:
        {
            rect.left = m_owner->m_windowed_rect.position.x;
            rect.top = m_owner->m_windowed_rect.position.y;
            rect.right = m_owner->m_windowed_rect.position.x + m_owner->m_windowed_rect.size.x;
            rect.bottom = m_owner->m_windowed_rect.position.y + m_owner->m_windowed_rect.size.y;

            break;
        }
        case window_rect_type::FLOATING:
        {
            rect.left = m_owner->m_floating_rect.position.x;
            rect.top = m_owner->m_floating_rect.position.y;
            rect.right = m_owner->m_floating_rect.position.x + m_owner->m_floating_rect.size.x;
            rect.bottom = m_owner->m_floating_rect.position.y + m_owner->m_floating_rect.size.y;

            break;
        }
    }

    return AdjustWindowRectEx(&rect, style, (GetMenu(m_handle) != NULL), ex_style);
}

bool video::window::window_impl::adjust_rect(RECT& rect, window_rect_type::type rect_type) const
{
    return adjust_rect_with_style(
        rect,
        rect_type,
        GetWindowLong(m_handle, GWL_STYLE),
        GetWindowLong(m_handle, GWL_EXSTYLE)
    );
}

void video::window::window_impl::set_position_internal(UINT flags, window_rect_type::type rect_type)
{
    RECT rect{};
    adjust_rect(rect, rect_type);

    HWND top = (m_owner->m_flags & flags::TOPMOST) ? HWND_TOPMOST : HWND_NOTOPMOST;

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

void video::window::window_impl::set_position()
{
    if (!(m_owner->m_flags & flags::FULLSCREEN))
    {
        if (!(m_owner->m_flags & (flags::MAXIMIZED | flags::MINIMIZED)))
        {
            set_position_internal(SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE, window_rect_type::FLOATING);
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
        m_owner->update_fullscreen_mode(true, true);
    }
}

void video::window::window_impl::set_size()
{
    if (!(m_owner->m_flags & (flags::FULLSCREEN | flags::MAXIMIZED)))
    {
        set_position_internal(SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE, window_rect_type::FLOATING);
    }
    else
    {
        m_floating_rect_pending = true;
    }
}

void video::window::window_impl::get_border_size(int32_t& left, int32_t& right, int32_t& bottom, int32_t& top) const
{
    RECT client_rect{};
    RECT window_rect{};
    POINT shift{};

    if (!GetClientRect(m_handle, &client_rect) || !GetWindowRect(m_handle, &window_rect))
    {
        return;
    }

    // Find the shift needed to move the client rect to window coordinates
    if (!ClientToScreen(m_handle, &shift))
    {
        return;
    }

    left = shift.x - window_rect.left;
    top = shift.y - window_rect.top;
    right = (window_rect.right - window_rect.left) - client_rect.right - left;
    bottom = (window_rect.bottom - window_rect.top) - client_rect.bottom - top;
}

void video::window::window_impl::set_resizable()
{
    sync_window_style();
}

void video::window::window_impl::set_bordered()
{
    sync_window_style();
    set_position_internal(SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE, window_rect_type::CURRENT);
}

void video::window::window_impl::set_always_on_top()
{
    sync_window_style();
    set_position_internal(SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE, window_rect_type::CURRENT);
}

// =============== window ops ===============

void video::window::window_impl::show()
{
    ShowWindow(m_handle, SW_SHOW);
}

void video::window::window_impl::hide()
{
    ShowWindow(m_handle, SW_HIDE);
}

void video::window::window_impl::minimize()
{
    ShowWindow(m_handle, SW_MINIMIZE);
}

void video::window::window_impl::maximize()
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
        adjust_rect(rect, window_rect_type::WINDOWED);

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

void video::window::window_impl::restore()
{
    if (m_owner->is_fullscreen())
    {
        m_windowed_mode_was_maximized = false;
        return;
    }

    m_expected_resize = true;
    ShowWindow(m_handle, SW_RESTORE);
    m_expected_resize = false;
}

bool video::window::window_impl::set_fullscreen(fullscreen_op::type fullscreen, const display* d)
{
    // no-op
    if (!(m_owner->m_flags & flags::FULLSCREEN) && !fullscreen)
    {
        return true;
    }

    HWND top = is_topmost() ? HWND_TOPMOST : HWND_NOTOPMOST;

    MONITORINFO mi{ sizeof(mi) };
    if (!GetMonitorInfo(d->m_impl->get_handle(), &mi))
    {
        VX_ERROR(error::error_code::PLATFORM_ERROR) << "GetMonitorInfo() failed";
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

        // Unset the maximized flag
        // This fixes https://bugzilla.libsdl.org/show_bug.cgi?id=3215
        if (style & WS_MAXIMIZE)
        {
            m_windowed_mode_was_maximized = true;
            style &= ~WS_MAXIMIZE;
        }

        // update corner rounding
    }
    else
    {
        // restore corner rounding

        // Restore maximized state if window was maximized when entering
        // fullscreen
        if (m_windowed_mode_was_maximized && !m_in_window_deactivation)
        {
            style |= WS_MAXIMIZE;
            maximize_on_exit = true;
        }

        window_rect_type::type type = m_windowed_mode_was_maximized ? window_rect_type::WINDOWED : window_rect_type::FLOATING;
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

void video::window::window_impl::focus()
{
    BringWindowToTop(m_handle);
    SetForegroundWindow(m_handle);
    SetFocus(m_handle);
}

bool video::window::window_impl::is_focused() const
{
    return m_handle == GetActiveWindow();
}

void video::window::window_impl::request_attention()
{
    FlashWindow(m_handle, TRUE);
}

void video::window::window_impl::set_topmost(bool enabled)
{
    set_position_internal(SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE, window_rect_type::CURRENT);
}

bool video::window::window_impl::is_topmost() const
{
    return false;
}

// =============== icon ===============

bool video::window::window_impl::set_icon(const uint8_t* pixels, const math::vec2i& size)
{
    clear_icon();

    const size_t image_size = static_cast<size_t>(size.x * size.y * 4);

    // Format is expected to have 4 8-bit channels in RGBA format
    if (image_size % 4)
    {
        VX_ERROR(error::error_code::INVALID_ARGUMENT) << "Window icon pixels must be RGBA format";
        return false;
    }

    // Convert the image to an BGRA
    std::vector<uint8_t> formatted_pixels(image_size);
    for (size_t pixel = 0; pixel < image_size; pixel += 4)
    {
        formatted_pixels[pixel + 0] = pixels[pixel + 2];
        formatted_pixels[pixel + 1] = pixels[pixel + 1];
        formatted_pixels[pixel + 2] = pixels[pixel + 0];
        formatted_pixels[pixel + 3] = pixels[pixel + 3];
    }

    // MSVC warns about arument 6 being NULL
    VX_DISABLE_WARNING("", 6387);
    VX_DISABLE_WARNING_PUSH();

    // Create the icon
    m_icon = CreateIcon(
        GetModuleHandle(NULL),
        size.x, size.y,
        1,
        32,
        NULL,
        formatted_pixels.data()
    );

    VX_DISABLE_WARNING_POP();

    if (!m_icon)
    {
        return false;
    }

    // Set it as both big and small icon of the window
    SendMessage(m_handle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(m_icon));
    SendMessage(m_handle, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(m_icon));

    return true;
}

void video::window::window_impl::clear_icon()
{
    if (m_icon)
    {
        DestroyIcon(m_icon);
        m_icon = NULL;
    }
}

// =============== mouse ===============

void video::window::window_impl::set_mouse_tracking(bool enabled)
{
    TRACKMOUSEEVENT e{ sizeof(e) };
    e.dwFlags = enabled ? TME_LEAVE : TME_CANCEL;
    e.hwndTrack = m_handle;
    e.dwHoverTime = 0;
    TrackMouseEvent(&e);
}

void video::window::window_impl::update_mouse_tracking()
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

math::vec2i video::window::window_impl::get_mouse_position() const
{
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(m_handle, &point);
    return math::vec2i(point.x, point.y);
}

void video::window::window_impl::set_mouse_position(const math::vec2i& position)
{
    POINT point = { position.x, position.y };
    ClientToScreen(m_handle, &point);
    SetCursorPos(point.x, point.y);
}

bool video::window::window_impl::is_hovered() const
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

bool video::window::window_impl::get_cursor_visibility() const
{
    //return m_cursor_visible;
    return true;
}

void video::window::window_impl::set_cursor_visibility(bool visible)
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

bool video::window::window_impl::is_cursor_grabbed() const
{
    //return m_cursor_grabbed;
    return false;
}

void video::window::window_impl::set_cursor_grabbed(bool grabbed)
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

}
}