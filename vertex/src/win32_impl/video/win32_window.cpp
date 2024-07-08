#include "win32_window.h"
#include "vertex/system/string/string_fn.h"
#include "vertex/system/error.h"

namespace vx {
namespace app {
namespace video {

//
// TODO:
// - hide cursor
// - grab cursor
// - raw mouse motion?
// - moving or resizing while minimzed or maximized
// - borderless and transparent window
// - popup windows
//

// =============== win32 class stuff ===============

static bool register_window_class(WNDPROC proc)
{
    WNDCLASSW wc{};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = proc;
    wc.hInstance = s_driver_data.instance; // needed for dll
    wc.hIcon = s_driver_data.window_default_icon ? s_driver_data.window_default_icon : NULL;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.lpszClassName = s_driver_data.window_class_name;

    s_driver_data.window_class = RegisterClassW(&wc);

    return s_driver_data.window_class != NULL;
}

static void unregister_window_class()
{
    UnregisterClassW(s_driver_data.window_class_name, s_driver_data.instance);
    s_driver_data.window_class = NULL;
}

static bool set_default_window_icon(const uint8_t* pixels, const math::vec2i& size)
{
    // Destroy existing icon
    if (s_driver_data.window_default_icon)
    {
        DestroyIcon(s_driver_data.window_default_icon);
        s_driver_data.window_default_icon = NULL;
    }

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
    s_driver_data.window_default_icon = CreateIcon(
        s_driver_data.instance,
        size.x, size.y,
        1,
        32,
        NULL,
        formatted_pixels.data()
    );

    VX_DISABLE_WARNING_POP();

    return s_driver_data.window_default_icon != NULL;
}

static bool make_custom_cursor(HCURSOR& cursor, const uint8_t* pixels, const math::vec2i& size, const math::vec2i& hotspot)
{
    const size_t image_size = static_cast<size_t>(size.x * size.y * 4);

    // Format is expected to have 4 8-bit channels in RGBA format
    if (image_size % 4)
    {
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

    // Create empty bitmap for monochrome mask
    HBITMAP mask = CreateBitmap(size.x, size.y, 1, 1, NULL);
    if (!mask)
    {
        return false;
    }

    // Create color bitmap
    HBITMAP color = CreateBitmap(size.x, size.y, 1, 32, formatted_pixels.data());
    if (!color)
    {
        DeleteObject(mask);
        return false;
    }

    // Create icon info
    ICONINFO info{};
    info.fIcon = FALSE;
    info.xHotspot = hotspot.x;
    info.yHotspot = hotspot.y;
    info.hbmMask = mask;
    info.hbmColor = color;

    cursor = CreateIconIndirect(&info);

    DeleteObject(color);
    DeleteObject(mask);

    return cursor != NULL;
}

static bool make_blank_cursor()
{
    if (s_driver_data.blank_cursor != NULL)
    {
        return true;
    }

    const int w = GetSystemMetrics(SM_CXCURSOR);
    const int h = GetSystemMetrics(SM_CYCURSOR);
    std::vector<uint8_t> pixels(w * h * 4);

    // Windows checks whether the image is fully transparent and if so just
    // ignores the alpha channel and makes the whole cursor opaque. To fix this
    // we make one pixel slightly less transparent.
    pixels[3] = 1;

    return make_custom_cursor(s_driver_data.blank_cursor, pixels.data(), math::vec2i(w, h), math::vec2i(0));
}

// =============== window init helpers ===============

window::window_impl::window_impl(window* window, const state_data& state)
    : m_window(window)
    , m_handle(NULL)
    , m_state(state)
    , m_borderless(false)
    , m_visible(false)
    , m_focussed(false)
    , m_floating(false)
    , m_resizable(false)
    , m_minimized(false)
    , m_maximized(false)
    , m_fullscreen(false)
    , m_scale_to_monitor(false)
    , m_scale_framebuffer(false)
    , m_mouse_passthrough(false)
    , m_icon(s_driver_data.window_default_icon)
{
    // If this is our first window, we register our window class
    if (s_driver_data.window_class == NULL)
    {
        register_window_class(window_proc); // TODO: check return value
    }

    // On Remote Desktop, setting the cursor to NULL does not hide it. To fix
    // this we create a transparent cursor and always set that instead of NULL.
    // When not on Remote Desktop, this handle is NULL and normal hiding is
    // used.
    if (GetSystemMetrics(SM_REMOTESESSION))
    {
        make_blank_cursor(); // TODO: check return value
    }

    create_window(); // TODO: check return value
}

bool window::window_impl::create_window()
{
    // Adjust window size and position to match requested area

    RECT rect;
    adjust_rect(rect, window_rect_type::FLOATING);

    // Create window
    m_handle = CreateWindowEx(
        get_window_ex_style(),
        s_driver_data.window_class_name,
        str::string_to_wstring(m_state.title).c_str(),
        get_window_style(),
        rect.left, rect.top,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, // no parent window
        NULL, // no window menu
        s_driver_data.instance,
        this
    );

    if (m_handle == NULL)
    {
        return false;
    }

    // set up out current flags based on the internal style of the window
    {
        DWORD style = GetWindowLong(m_handle, GWL_STYLE);

        if (style & WS_VISIBLE)
        {
            m_state.flags &= ~flags::HIDDEN;
        }
        else
        {
            m_state.flags |= flags::HIDDEN;
        }
        if (style & WS_POPUP)
        {
            m_state.flags |= flags::BORDERLESS;
        }
        else
        {
            m_state.flags &= ~flags::BORDERLESS;
        }
        if (style & WS_THICKFRAME)
        {
            m_state.flags |= flags::RESIZABLE;
        }
        else
        {
            m_state.flags &= ~flags::RESIZABLE;
        }
        if (style & WS_MAXIMIZE)
        {
            m_state.flags |= flags::MAXIMIZED;
        }
        else
        {
            m_state.flags &= ~flags::MAXIMIZED;
        }
        if (style & WS_MINIMIZE)
        {
            m_state.flags |= flags::MINIMIZED;
        }
        else
        {
            m_state.flags &= ~flags::MINIMIZED;
        }
    }

    // Windows imposes a size limit for windows that prevents them from being
    // created in a size larger than the display they are on. If this happens
    // we can catch and override it.

    if (!(m_state.flags & flags::MINIMIZED))
    {
        RECT rect;

        if (GetClientRect(m_handle, &rect))
        {
            if ((m_state.windowed.size.x && m_state.windowed.size.x != rect.right) ||
                (m_state.windowed.size.y && m_state.windowed.size.y != rect.bottom))
            {
                adjust_rect(rect, window_rect_type::WINDOWED);

                m_state.moving_or_resizing = true;
                SetWindowPos(
                    m_handle,
                    NULL,
                    rect.left, rect.top,
                    rect.right - rect.left, rect.bottom - rect.top,
                    SWP_NOCOPYBITS | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE
                );
                m_state.moving_or_resizing = false;
            }
            else
            {
                m_state.position.x = rect.left;
                m_state.position.y = rect.top;
                m_state.size.x = rect.right - rect.left;
                m_state.size.y = rect.bottom - rect.top;
            }
        }
    }

    if (GetFocus() == m_handle)
    {
        m_state.flags |= flags::FOCUSSED;
        //SDL_SetKeyboardFocus(window);
        //WIN_UpdateClipCursor(window);
    }

    //if (IsWindows7OrGreater())
    //{
    //    ChangeWindowMessageFilterEx(m_handle, WM_DROPFILES, MSGFLT_ALLOW, NULL);
    //    ChangeWindowMessageFilterEx(m_handle, WM_COPYDATA, MSGFLT_ALLOW, NULL);
    //}

    //if (true)//!display)
    //{
    //    RECT rect = { 0, 0, config.size.x, config.size.y };
    //    WINDOWPLACEMENT wp = { sizeof(wp) };
    //    const HMONITOR mh = MonitorFromWindow(m_handle, MONITOR_DEFAULTTONEAREST);
    //
    //    const video::display* 
    //
    //    // Obtain the content scale of the monitor that contains the window
    //
    //    float xscale = 0.0f;
    //    float yscale = 0.0f;
    //    get_content_scale(mh, xscale, yscale);
    //
    //    // Optionally scale the window content area to account for dpi scaling
    //
    //    if (m_scale_to_monitor)
    //    {
    //        if (xscale > 0.0f && yscale > 0.0f)
    //        {
    //            rect.right = static_cast<LONG>(rect.right * xscale);
    //            rect.bottom = static_cast<LONG>(rect.bottom * yscale);
    //        }
    //    }
    //
    //    // Scale the window size to account for dpi scaling
    //
    //    if (driver_data.user32.AdjustWindowRectExForDpi)
    //    {
    //        driver_data.user32.AdjustWindowRectExForDpi(&rect, style, FALSE, ex_style, GetDpiForWindow(m_handle));
    //    }
    //    else
    //    {
    //        AdjustWindowRectEx(&rect, style, FALSE, ex_style);
    //    }
    //
    //    // Move the rect to the restored window position
    //
    //    GetWindowPlacement(m_handle, &wp);
    //    OffsetRect(&rect, wp.rcNormalPosition.left - rect.left, wp.rcNormalPosition.top - rect.top);
    //
    //    // Set the restored window rect to the new adjusted rect
    //
    //    wp.rcNormalPosition = rect;
    //    wp.showCmd = SW_HIDE;
    //    SetWindowPlacement(m_handle, &wp);
    //
    //    // Adjust rect of maximized undecorated window, because by default Windows will
    //    // make such a window cover the whole monitor instead of its workarea
    //
    //    if (m_maximized && m_borderless)
    //    {
    //        MONITORINFO mi = { sizeof(mi) };
    //        GetMonitorInfoW(mh, &mi);
    //
    //        SetWindowPos(
    //            m_handle,
    //            HWND_TOP,
    //            mi.rcWork.left,
    //            mi.rcWork.top,
    //            mi.rcWork.right - mi.rcWork.left,
    //            mi.rcWork.bottom - mi.rcWork.top,
    //            SWP_NOACTIVATE | SWP_NOZORDER
    //        );
    //    }
    //}

    // Set initial mouse position
    //m_last_mouse_position = get_mouse_position();
    //m_mouse_inside_window = is_hovered();

    // Set the default cursor
    //set_cursor(m_last_cursor_object);

    // Finally show the window
    show();

    return true;
}

void window::window_impl::setup_data()
{
    {
        DWORD style = GetWindowLong(m_handle, GWL_STYLE);

        if (style & WS_VISIBLE)
        {
            m_state.flags &= ~flags::HIDDEN;
        }
        else 
        {
            m_state.flags |= flags::HIDDEN;
        }
        if (style & WS_POPUP) 
        {
            m_state.flags |= flags::BORDERLESS;
        }
        else 
        {
            m_state.flags &= ~flags::BORDERLESS;
        }
        if (style & WS_THICKFRAME) 
        {
            m_state.flags |= flags::RESIZABLE;
        }
        else 
        {
            m_state.flags &= ~flags::RESIZABLE;
        }
        if (style & WS_MAXIMIZE) 
        {
            m_state.flags |= flags::MAXIMIZED;
        }
        else
        {
            m_state.flags &= ~flags::MAXIMIZED;
        }
        if (style & WS_MINIMIZE) 
        {
            m_state.flags |= flags::MINIMIZED;
        }
        else
        {
            m_state.flags &= ~flags::MINIMIZED;
        }
    }

    if (!(m_state.flags & flags::MINIMIZED))
    {
        RECT rect;

        if (GetClientRect(m_handle, &rect))// && !WIN_IsRectEmpty(&rect))
        {
            if ((m_state.windowed.size.x && m_state.windowed.size.x != rect.right) || 
                (m_state.windowed.size.y && m_state.windowed.size.y != rect.bottom))
            {
                rect.left = 0;
                rect.top = 0;
                rect.right = m_state.windowed.size.x;
                rect.bottom = m_state.windowed.size.y;

                AdjustWindowRectEx(&rect, get_window_style(), FALSE, get_window_ex_style()); // TODO: check return value

                m_state.moving_or_resizing = true;
                SetWindowPos(
                    m_handle,
                    NULL,
                    rect.left, rect.top,
                    rect.right - rect.left, rect.bottom - rect.top,
                    SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE
                );
                m_state.moving_or_resizing = false;
            }
        }
    }

    auto z = get_size();
}

window::window_impl::~window_impl()
{
    // Destroy the custom icon
    clear_icon();

    // Destroy the window
    if (m_handle)
    {
        DestroyWindow(m_handle);
    }

    // Decrement the window count
    //--s_window_count;
    //
    //// If we were the last window...
    //if (s_window_count == 0)
    //{
    //    // Unregister window class
    //    unregister_window_class();
    //}
}

void window::window_impl::on_destroy()
{
    // Restore cursor visibility
    set_cursor_visibility(true);

    // Remove tracking
    set_mouse_tracking(false);
}

// =============== open ===============

bool window::window_impl::is_open() const
{
    return m_handle;
}

// =============== style ===============

DWORD window::window_impl::get_window_style() const
{
    DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    if (m_state.flags & flags::FULLSCREEN)
    {
        style |= WS_POPUP | WS_MINIMIZEBOX;
    }
    else
    {
        style |= WS_SYSMENU | WS_MINIMIZEBOX;

        if (m_state.flags & flags::BORDERLESS)
        {
            style |= WS_POPUP;
        }
        else
        {
            style |= WS_CAPTION;

            if (m_state.flags & flags::RESIZABLE)
            {
                style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
            }
        }
    }

    return style;
}

DWORD window::window_impl::get_window_ex_style() const
{
    DWORD style = 0;

    //if (
    //{
    //    style |= WS_EX_TOPMOST;
    //}

    return style;
}

void window::window_impl::update_style(int flags, bool enable)
{
    LONG_PTR style = GetWindowLongPtrW(m_handle, GWL_STYLE);

    if (enable)
    {
        style |= flags;
    }
    else
    {
        style &= ~flags;
    }

    SetWindowLongPtrW(m_handle, GWL_STYLE, style);

    // Redraw the window
    SetWindowPos(m_handle, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

bool window::window_impl::has_style(int flag) const
{
    LONG_PTR style = GetWindowLongPtrW(m_handle, GWL_STYLE);
    return style & flag;
}

// =============== events ===============

LRESULT CALLBACK window::window_impl::window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    window::window_impl* window = nullptr;
    LRESULT return_code = -1;

    if (Msg == WM_CREATE)
    {
        // From the data passed into CreateWindow, we can extract the last
        // argument which was a pointer to the window instance. We can then
        // set the user data of the win32 window to a pointer to our window.
        auto window_ptr = reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, window_ptr);
    }

    // Get the pointer to our associated window
    window = reinterpret_cast<window::window_impl*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

    if (!window || !window->m_handle)
    {
        return DefWindowProcW(hWnd, Msg, wParam, lParam);
    }

    switch (Msg)
    {
        // =============== window events ===============

        // Destroy window
        case WM_DESTROY:
        {
            window->on_destroy();
            break;
        }

        // Close window
        case WM_CLOSE:
        {
            event e;
            e.type = event_type::WINDOW_CLOSE_REQUESTED;
            e.window_event.window_id = window->m_window->m_window->m_state.id;
            event::post_event(e);

            break;
        }

        // Resize or move window start
        //case WM_ENTERSIZEMOVE:
        //{
        //    window->m_resizing_or_moving = true;
        //    break;
        //}
        //
        //// Resize or move window end
        //case WM_EXITSIZEMOVE:
        //{
        //    window->m_resizing_or_moving = false;
        //
        //    // Check if the size changed
        //    const math::vec2i new_size = window->get_size();
        //
        //    if (window->m_last_size != new_size)
        //    {
        //        window->m_last_size = new_size;
        //
        //        event e;
        //        e.type = event_type::WINDOW_RESIZE;
        //        e.window_resize.width = new_size.x;
        //        e.window_resize.height = new_size.y;
        //        window->post_event(e);
        //    }
        //
        //    // Check if the position changed
        //    const math::vec2i new_position = window->get_position();
        //
        //    if (window->m_last_position != new_position)
        //    {
        //        window->m_last_position = new_position;
        //
        //        event e;
        //        e.type = event_type::WINDOW_MOVE;
        //        e.window_move.x = new_position.x;
        //        e.window_move.y = new_position.y;
        //        window->post_event(e);
        //    }
        //
        //    break;
        //}

        case WM_WINDOWPOSCHANGING:
        {
            if (window->m_state.moving_or_resizing)
            {
                return_code = 0;
            }

            // If we are going from a fixed size state to a floating state, we
            // update the position to reflect the cached floating rect position
            // and size.
            if (window->m_state.adjust_floating_rect &&
                !IsIconic(window->m_handle) &&
                !IsZoomed(window->m_handle) &&
                (window->m_state.flags & (flags::MINIMIZED | flags::MAXIMIZED)) &&
                !(window->m_state.flags & flags::FULLSCREEN))
            {
                WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);

                RECT rect;
                window->adjust_rect(rect, window_rect_type::FLOATING);

                wp->x = rect.left;
                wp->y = rect.top;
                wp->cx = rect.right - rect.left;
                wp->cy = rect.bottom - rect.top;
                wp->flags &= ~(SWP_NOSIZE | SWP_NOMOVE);

                window->m_state.adjust_floating_rect = false;
            }

            break;
        }

        case WM_WINDOWPOSCHANGED:
        {
            const bool minimized = IsIconic(window->m_handle);
            const bool maximized = IsZoomed(window->m_handle);

            const WINDOWPOS* wp = reinterpret_cast<const WINDOWPOS*>(lParam);

            if (wp->flags & SWP_SHOWWINDOW)
            {
                window->m_window->on_show();
            }

            //if (minimize)
            //{
            //
            //}

            break;
        }

        // Maximize & minimize window
        case WM_SIZE:
        {
            //const math::vec2i new_size = get_size();
            //
            //if (m_last_size != new_size)
            //{
            //    if (wParam == SIZE_MINIMIZED && !m_minimized)
            //    {
            //        m_maximized = false;
            //        m_minimized = true;
            //
            //        event e;
            //        e.type = event_type::WINDOW_MINIMIZE;
            //        post_event(e);
            //    }
            //    if (wParam == SIZE_MAXIMIZED && !m_maximized)
            //    {
            //        m_minimized = false;
            //        m_maximized = true;
            //
            //        event e;
            //        e.type = event_type::WINDOW_MAXIMIZE;
            //        post_event(e);
            //    }
            //    if (wParam == SIZE_RESTORED)
            //    {
            //        m_maximized = m_minimized = false;
            //    }
            //
            //    // Only publish a resize event if we are not currently moving or resizing the window.
            //    // If we are resizing or moving, a resize or move event will be published when the
            //    // process finishes anyways.
            //
            //    if (!m_resizing_or_moving)
            //    {
            //        m_last_size = new_size;
            //
            //        event e;
            //        e.type = event_type::WINDOW_RESIZE;
            //        e.window_resize.width = new_size.x;
            //        e.window_resize.height = new_size.y;
            //        post_event(e);
            //    }
            //
            //    // Update the cursor tracking incase the window moved away from the cursor.
            //    update_mouse_tracking();
            //}

            break;
        }

        // Move window
        case WM_MOVE:
        {
            // We only want to process move messages if the window actually moved.
            // We don't count minimizing or restoring from a minimized state as 
            // moving.
            // 
            // When minimizing:
            // - is_minimized will return true, we will not post move event.
            // 
            // When restoring from minimized state:
            // - While minimized, the position of the window is not allowed to be
            //   changed.
            // - When we get the move messages, new_position should be the same as
            //   the last recorded position, we will not post move event.

            if (!window->is_minimized())
            {
                const math::vec2i new_position = window->get_position();

                if (!window->m_resizing_or_moving && window->m_last_position != new_position)
                {
                    window->m_last_position = new_position;

                    event e;
                    e.type = event_type::WINDOW_MOVED;
                    e.window_event.window_id = window->m_window->m_window->m_state.id;
                    e.window_event.data1 = new_position.x;
                    e.window_event.data2 = new_position.y;
                    window->post_event(e);

                    // Update the cursor tracking incase the window moved away from the cursor.
                    window->update_mouse_tracking();
                }
            }

            break;
        }

        // Fix violations of minimum or maximum size
        case WM_GETMINMAXINFO:
        {
            if (window->m_state.moving_or_resizing)
            {
                break;
            }

            // Get the size of the window including the frame

            MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);

            RECT frame{};
            AdjustWindowRectEx(&frame, window->get_window_style(), FALSE, window->get_window_ex_style());

            int w = frame.right - frame.left;
            int h = frame.bottom - frame.top;

            if (window->m_state.flags && flags::RESIZABLE)
            {
                if (window->m_state.flags && flags::BORDERLESS)
                {
                    const int screen_w = GetSystemMetrics(SM_CXSCREEN);
                    const int screen_h = GetSystemMetrics(SM_CYSCREEN);

                    mmi->ptMaxSize.x = std::max(w, screen_w);
                    mmi->ptMaxSize.y = std::max(h, screen_h);
                    mmi->ptMaxPosition.x = std::min(0, (screen_w - w) / 2);
                    mmi->ptMaxPosition.y = std::max(0, (screen_h - h) / 2);
                }

                if (window->m_state.min_size.x && window->m_state.min_size.y)
                {
                    mmi->ptMinTrackSize.x = window->m_state.min_size.x + w;
                    mmi->ptMinTrackSize.y = window->m_state.min_size.y + h;
                }

                if (window->m_state.max_size.x && window->m_state.max_size.y)
                {
                    mmi->ptMaxTrackSize.x = window->m_state.max_size.x + w;
                    mmi->ptMaxTrackSize.y = window->m_state.max_size.y + h;
                }
            }
            else
            {
                int x = frame.left;
                int y = frame.top;

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

        // Focus window
        case WM_SETFOCUS:
        {
            if (!window->m_focussed)
            {
                window->m_focussed = true;

                event e;
                e.type = event_type::WINDOW_GAINED_FOCUS;
                e.window_event.window_id = window->m_window->m_window->m_state.id;
                window->post_event(e);
            }

            break;
        }

        // Un-focus window
        case WM_KILLFOCUS:
        {
            if (window->m_focussed)
            {
                window->m_focussed = false;

                event e;
                e.type = event_type::WINDOW_LOST_FOCUS;
                e.window_event.window_id = window->m_window->m_window->m_state.id;
                window->post_event(e);
            }

            break;
        }

        // Show or hide window
        case WM_SHOWWINDOW:
        {
            //if (window->m_visible != static_cast<bool>(wParam))
            //{
            //    window->m_visible = wParam;
            //
            //    event e;
            //    e.type = event_type::WINDOW_SHOW;
            //    e.window_show.value = wParam;
            //    window->post_event(e);
            //
            //    // Update the cursor tracking incase the cursor moved outside the window.
            //    window->update_mouse_tracking();
            //}

            break;
        }

        case WM_PAINT:
        {
            RECT rect;

            if (GetUpdateRect(window->m_handle, &rect, FALSE))
            {
                const LONG ex_style = GetWindowLong(window->m_handle, GWL_EXSTYLE);

                // Composited windows will continue to receive WM_PAINT
                // messages for update regions until the window is actually
                // painted through Begin/EndPaint.
                if (ex_style & WS_EX_COMPOSITED)
                {
                    PAINTSTRUCT ps;
                    BeginPaint(window->m_handle, &ps);
                    EndPaint(window->m_handle, &ps);
                }

                ValidateRect(window->m_handle, NULL);
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
            if (LOWORD(lParam) == HTCLIENT && window->is_hovered())
            {
                SetCursor(window->m_cursor_visible ? window->m_last_cursor : NULL);
                return_code = 0;
            }

            break;
        }

        // Left mouse button down
        case WM_LBUTTONDOWN:
        {
            event e;
            e.type = event_type::MOUSE_BUTTON_DOWN;
            e.mouse_button_event.window_id = window->m_window->m_window->m_state.id;
            e.mouse_button_event.button = mouse::BUTTON_LEFT;
            e.mouse_button_event.x = static_cast<int>(LOWORD(lParam));
            e.mouse_button_event.y = static_cast<int>(HIWORD(lParam));
            window->post_event(e);

            break;
        }

        // Left mouse button up
        case WM_LBUTTONUP:
        {
            event e;
            e.type = event_type::MOUSE_BUTTON_UP;
            e.mouse_button_event.window_id = window->m_window->m_window->m_state.id;
            e.mouse_button_event.button = mouse::BUTTON_LEFT;
            e.mouse_button_event.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_button_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
            window->post_event(e);

            break;
        }

        // Right mouse button down
        case WM_RBUTTONDOWN:
        {
            event e;
            e.type = event_type::MOUSE_BUTTON_DOWN;
            e.mouse_button_event.window_id = window->m_window->m_window->m_state.id;
            e.mouse_button_event.button = mouse::BUTTON_RIGHT;
            e.mouse_button_event.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_button_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
            window->post_event(e);

            break;
        }

        // Right mouse button up
        case WM_RBUTTONUP:
        {
            event e;
            e.type = event_type::MOUSE_BUTTON_UP;
            e.mouse_button_event.window_id = window->m_window->m_window->m_state.id;
            e.mouse_button_event.button = mouse::BUTTON_RIGHT;
            e.mouse_button_event.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_button_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
            window->post_event(e);

            break;
        }

        // Mouse wheel button down
        case WM_MBUTTONDOWN:
        {
            event e;
            e.type = event_type::MOUSE_BUTTON_DOWN;
            e.mouse_button_event.window_id = window->m_window->m_window->m_state.id;
            e.mouse_button_event.button = mouse::BUTTON_MIDDLE;
            e.mouse_button_event.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_button_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
            window->post_event(e);

            break;
        }

        // Mouse wheel button up
        case WM_MBUTTONUP:
        {
            event e;
            e.type = event_type::MOUSE_BUTTON_UP;
            e.mouse_button_event.window_id = window->m_window->m_window->m_state.id;
            e.mouse_button_event.button = mouse::BUTTON_MIDDLE;
            e.mouse_button_event.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_button_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
            window->post_event(e);

            break;
        }

        // Extra mouse button down
        case WM_XBUTTONDOWN:
        {
            event e;
            e.type = event_type::MOUSE_BUTTON_DOWN;
            e.mouse_button_event.window_id = window->m_window->m_window->m_state.id;
            e.mouse_button_event.button = (HIWORD(wParam) == XBUTTON1) ? mouse::BUTTON_EXTRA_1 : mouse::button::BUTTON_EXTRA_2;
            e.mouse_button_event.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_button_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
            window->post_event(e);

            break;
        }

        // Extra mouse button up
        case WM_XBUTTONUP:
        {
            event e;
            e.type = event_type::MOUSE_BUTTON_UP;
            e.mouse_button_event.window_id = window->m_window->m_window->m_state.id;
            e.mouse_button_event.button = (HIWORD(wParam) == XBUTTON1) ? mouse::BUTTON_EXTRA_1 : mouse::button::BUTTON_EXTRA_2;
            e.mouse_button_event.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_button_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
            window->post_event(e);

            break;
        }

        // Vertical mouse scroll
        case WM_MOUSEWHEEL:
        {
            const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            const math::vec2i mouse_position = window->get_mouse_position();

            event e;
            e.type = event_type::MOUSE_WHEEL;
            e.mouse_wheel_event.window_id = window->m_window->m_window->m_state.id;
            e.mouse_wheel_event.wheel = mouse::wheel::VERTICAL;
            e.mouse_wheel_event.delta = static_cast<float>(delta) / static_cast<float>(WHEEL_DELTA);
            e.mouse_wheel_event.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_wheel_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
            window->post_event(e);

            break;
        }

        // Horizontal mouse scroll
        case WM_MOUSEHWHEEL:
        {
            const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            const math::vec2i mouse_position = window->get_mouse_position();

            event e;
            e.type = event_type::MOUSE_WHEEL;
            e.mouse_wheel_event.window_id = window->m_window->m_window->m_state.id;
            e.mouse_wheel_event.wheel = mouse::wheel::HORIZONTAL;
            e.mouse_wheel_event.delta = static_cast<float>(delta) / static_cast<float>(WHEEL_DELTA);
            e.mouse_wheel_event.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_wheel_event.y = static_cast<int>(GET_Y_LPARAM(lParam));
            window->post_event(e);

            break;
        }

        // Mouse move
        case WM_MOUSEMOVE:
        {
            const math::vec2i new_mouse_position = window->get_mouse_position();

            if (window->m_last_mouse_position != new_mouse_position)
            {
                window->m_last_mouse_position = new_mouse_position;

                event e;
                e.type = event_type::MOUSE_MOVED;
                e.mouse_motion_event.window_id = window->m_window->m_window->m_state.id;
                e.mouse_motion_event.x = new_mouse_position.x;
                e.mouse_motion_event.y = new_mouse_position.y;
                window->post_event(e);

                // Update the cursor tracking incase the cursor moved outside the window.
                window->update_mouse_tracking();
            }

            break;
        }

        // Mouse leave
        case WM_MOUSELEAVE:
        {
            // Update the cursor tracking incase the cursor moved outside the window.
            window->update_mouse_tracking();
            break;
        }
    }

    if (return_code >= 0)
    {
        return return_code;
    }

    return DefWindowProcW(hWnd, Msg, wParam, lParam);
}

void window::window_impl::process_events()
{
    MSG Msg;

    while (PeekMessageW(&Msg, NULL, 0u, 0u, PM_REMOVE))
    {
        TranslateMessage(&Msg);
        DispatchMessageW(&Msg);
    }
}

void window::window_impl::post_event(const event& e)
{
    m_events.push(e);
}

bool window::window_impl::pop_event(event& e, bool block)
{
    if (m_events.empty())
    {
        process_events();

        if (block)
        {
            // While blocking, sleep in periods of 10 ms while checking for new events
            while (m_events.empty())
            {
                Sleep(10);
                process_events();
            }
        }
    }

    // If there are any events in the queue return the next one
    if (!m_events.empty())
    {
        e = m_events.front();
        m_events.pop();

        return true;
    }

    return false;
}

// =============== title ===============

std::string window::window_impl::get_title() const
{
    // Check the title length
    int length = GetWindowTextLengthW(m_handle);

    // Create a buffer to hold the wide title
    std::wstring wtitle(static_cast<size_t>(length), 0);
    GetWindowTextW(m_handle, reinterpret_cast<LPWSTR>(wtitle.data()), length);

    // Convert the wide string
    return str::wstring_to_string(wtitle);
}

void window::window_impl::set_title(const std::string& title)
{
    SetWindowTextW(m_handle, str::string_to_wstring(title).c_str());
}

// =============== position and size ===============

math::vec2i window::window_impl::content_position_to_window_position(const math::vec2i& position) const
{
    // We specify a rect with the desired position, then call AdjustWindowRect to
    // which will return a new rect adjusted by the window frame position and size.
    RECT rect = { position.x, position.y, 0, 0 };
    AdjustWindowRect(&rect, static_cast<DWORD>(GetWindowLongPtrW(m_handle, GWL_STYLE)), false);
    return math::vec2i(rect.left, rect.top);
}

math::vec2i window::window_impl::content_size_to_window_size(const math::vec2i& size) const
{
    // We specify a rect with the desired size, then call AdjustWindowRect to
    // which will return a new rect adjusted by the window frame position and size.
    RECT rect = { 0, 0, size.x, size.y };
    AdjustWindowRect(&rect, static_cast<DWORD>(GetWindowLongPtrW(m_handle, GWL_STYLE)), false);
    return math::vec2i(rect.right - rect.left, rect.bottom - rect.top);
}

void window::window_impl::adjust_rect(RECT& rect, window_rect_type rect_type) const
{
    switch (rect_type)
    {
        case window_rect_type::CURRENT:
        {
            rect.left = m_state.position.x;
            rect.top = m_state.position.y;
            rect.right = m_state.position.x + m_state.size.x;
            rect.bottom = m_state.position.y + m_state.size.y;

            break;
        }
        case window_rect_type::WINDOWED:
        {
            rect.left = m_state.windowed.position.x;
            rect.top = m_state.windowed.position.y;
            rect.right = m_state.windowed.position.x + m_state.windowed.size.x;
            rect.bottom = m_state.windowed.position.y + m_state.windowed.size.y;

            break;
        }
        case window_rect_type::FLOATING:
        {
            rect.left = m_state.floating.position.x;
            rect.top = m_state.floating.position.y;
            rect.right = m_state.floating.position.x + m_state.floating.size.x;
            rect.bottom = m_state.floating.position.y + m_state.floating.size.y;

            break;
        }
    }

    AdjustWindowRectEx(&rect, GetWindowLong(m_handle, GWL_STYLE), FALSE, GetWindowLong(m_handle, GWL_EXSTYLE));
}

void window::window_impl::set_position_internal(UINT flags, window_rect_type rect_type)
{
    RECT rect;
    adjust_rect(rect, rect_type);

    HWND top = (m_state.flags & flags::TOPMOST) ? HWND_TOPMOST : HWND_NOTOPMOST;

    m_state.moving_or_resizing = true;
    const bool result = SetWindowPos(m_handle, top, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, flags);
    m_state.moving_or_resizing = false;

    assert(result);
}

math::vec2i window::window_impl::get_position() const
{
    POINT pos{};
    ClientToScreen(m_handle, &pos);
    return math::vec2i(pos.x, pos.y);
}

void window::window_impl::set_position(const math::vec2i& position)
{
    if (!(m_state.flags & flags::FULLSCREEN))
    {
        if (!(m_state.flags & (flags::MAXIMIZED | flags::MINIMIZED)))
        {
            set_position_internal(SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE, window_rect_type::FLOATING);
        }
        else
        {
            // window::set_position already set the new floating rect position.
            // We will wait to apply the changes until we are in a valid mode.
            m_state.adjust_floating_rect = true;
        }
    }
    else
    {
        // updatewindowfullscreenmode
    }
}

math::vec2i window::window_impl::get_size() const
{
    RECT rect;
    GetClientRect(m_handle, &rect);
    return math::vec2i(rect.right, rect.bottom);
}

void window::window_impl::set_size(const math::vec2i& size)
{
    const math::vec2i new_size = content_size_to_window_size(size);
    SetWindowPos(m_handle, NULL, 0, 0, new_size.x, new_size.y, SWP_NOMOVE | SWP_NOZORDER);
}

math::vec2i window::window_impl::get_min_size() const
{
    return m_min_size;
}

math::vec2i window::window_impl::get_max_size() const
{
    return m_max_size;
}

void window::window_impl::set_min_size(const math::vec2i& size)
{
    m_min_size = size;

    // Adjust the size to trigger WM_GETMINMAXINFO
    SetWindowPos(m_handle, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

void window::window_impl::set_max_size(const math::vec2i& size)
{
    m_max_size = size;

    // Adjust the size to trigger WM_GETMINMAXINFO
    SetWindowPos(m_handle, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

void window::window_impl::set_resizable(bool resizable)
{
    update_style(WS_THICKFRAME | WS_MAXIMIZEBOX, resizable);
}

bool window::window_impl::is_resizable() const
{
    return has_style(WS_THICKFRAME | WS_MAXIMIZEBOX);
}

// =============== window ops ===============

void window::window_impl::show()
{
    ShowWindow(m_handle, SW_SHOWNA);
}

void window::window_impl::minimize()
{
    // We need to call show or else we will not receive a WM_SHOWWINDOW message
    ShowWindow(m_handle, SW_MINIMIZE);
}

bool window::window_impl::is_minimized() const
{
    return IsIconic(m_handle);
}

void window::window_impl::maximize()
{
    // We need to call show or else we will not receive a WM_SHOWWINDOW message
    ShowWindow(m_handle, SW_MAXIMIZE);
}

bool window::window_impl::is_maximized() const
{
    return IsZoomed(m_handle);
}

void window::window_impl::restore()
{
    ShowWindow(m_handle, SW_RESTORE);
}

bool window::window_impl::is_fullscreen() const
{
    return m_fullscreen;
}

void window::window_impl::focus()
{
    BringWindowToTop(m_handle);
    SetForegroundWindow(m_handle);
    SetFocus(m_handle);
}

bool window::window_impl::is_focused() const
{
    return m_handle == GetActiveWindow();
}

void window::window_impl::request_attention()
{
    FlashWindow(m_handle, TRUE);
}

void window::window_impl::set_topmost(bool enabled)
{
    set_position_internal(SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE, window_rect_type::CURRENT);
}

bool window::window_impl::is_topmost() const
{
    return false;
}

// =============== icon ===============

bool window::window_impl::set_icon(const uint8_t* pixels, const math::vec2i& size)
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
        GetModuleHandleW(NULL),
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
    SendMessageW(m_handle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(m_icon));
    SendMessageW(m_handle, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(m_icon));

    return true;
}

void window::window_impl::clear_icon()
{
    if (m_icon)
    {
        DestroyIcon(m_icon);
        m_icon = NULL;
    }
}

// =============== mouse ===============

void window::window_impl::set_mouse_tracking(bool enabled)
{
    TRACKMOUSEEVENT e{ sizeof(e) };
    e.dwFlags = enabled ? TME_LEAVE : TME_CANCEL;
    e.hwndTrack = m_handle;
    e.dwHoverTime = 0;
    TrackMouseEvent(&e);
}

void window::window_impl::update_mouse_tracking()
{
    if (!is_hovered())
    {
        if (m_mouse_inside_window)
        {
            m_mouse_inside_window = false;
            set_mouse_tracking(false);

            //event e;
            //e.type = event_type::MOUSE_HOVER;
            //e.mouse_hover.value = false;
            //post_event(e);
        }
    }
    else
    {
        if (!m_mouse_inside_window)
        {
            m_mouse_inside_window = true;
            set_mouse_tracking(true);

            //event e;
            //e.type = event_type::MOUSE_HOVER;
            //e.mouse_hover.value = true;
            //post_event(e);
        }
    }
}

math::vec2i window::window_impl::get_mouse_position() const
{
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(m_handle, &point);
    return math::vec2i(point.x, point.y);
}

void window::window_impl::set_mouse_position(const math::vec2i& position)
{
    POINT point = { position.x, position.y };
    ClientToScreen(m_handle, &point);
    SetCursorPos(point.x, point.y);
}

bool window::window_impl::is_hovered() const
{
    if (!m_visible)
    {
        return false;
    }

    POINT point;
    if (!GetCursorPos(&point))
    {
        return false;
    }

    RECT rect;
    ScreenToClient(m_handle, &point);
    GetClientRect(m_handle, &rect);

    return PtInRect(&rect, point);
}

bool window::window_impl::get_cursor_visibility() const
{
    return m_cursor_visible;
}

void window::window_impl::set_cursor_visibility(bool visible)
{
    m_cursor_visible = visible;
    SetCursor(m_cursor_visible ? m_last_cursor : NULL);
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

bool window::window_impl::is_cursor_grabbed() const
{
    return m_cursor_grabbed;
}

void window::window_impl::set_cursor_grabbed(bool grabbed)
{
    if (grabbed)
    {
        RECT rect;
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
}