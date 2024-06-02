#include "win32_window.h"
//#include "../input/cursor_internal.h"
#include "vertex/system/string/string_fn.h"
#include "vertex/system/error.h"

namespace vx {
namespace app {

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
    wc.hInstance = driver_data.instance; // needed for dll
    wc.hIcon = driver_data.window_default_icon ? driver_data.window_default_icon : NULL;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.lpszClassName = video_data::window_class_name;

    driver_data.window_class = RegisterClassW(&wc);

    return driver_data.window_class != NULL;
}

static void unregister_window_class()
{
    UnregisterClassW(video_data::window_class_name, driver_data.instance);
    driver_data.window_class = NULL;
}

static bool set_default_window_icon(const uint8_t* pixels, const math::vec2i& size)
{
    // Destroy existing icon
    if (driver_data.window_default_icon)
    {
        DestroyIcon(driver_data.window_default_icon);
        driver_data.window_default_icon = NULL;
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
    driver_data.window_default_icon = CreateIcon(
        driver_data.instance,
        size.x, size.y,
        1,
        32,
        NULL,
        formatted_pixels.data()
    );

    VX_DISABLE_WARNING_POP();

    return driver_data.window_default_icon != NULL;
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
    if (driver_data.blank_cursor != NULL)
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

    return make_custom_cursor(driver_data.blank_cursor, pixels.data(), math::vec2i(w, h), math::vec2i(0));
}

// =============== window init helpers ===============

window::window_impl::window_impl(const config& config)
    : m_handle(NULL)
    , m_borderless(config.hint.borderless)
    , m_visible(config.hint.visible)
    , m_focussed(config.hint.focused)
    , m_floating(config.hint.floating)
    , m_resizable(config.hint.resizable)
    , m_minimized(false)
    , m_maximized(config.hint.maximized)
    , m_fullscreen(config.hint.fullscreen)
    , m_scale_to_monitor(config.hint.scale_to_monitor)
    , m_scale_framebuffer(config.hint.scale_framebuffer)
    , m_mouse_passthrough(config.hint.mouse_passthrough)
    , m_resizing_or_moving(false)
    , m_last_size(config.size)
    , m_last_position(config.position)
    , m_min_size(s_default_min_size)
    , m_max_size(s_default_max_size)
    , m_icon(driver_data.window_default_icon)
    //, m_last_cursor_object(cursor::cursor_shape::SHAPE_ARROW)
{
    // If this is our first window, we register our window class
    if (driver_data.window_class == NULL)
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

    create_window(config); // TODO: check return value

    if (config.hint.fullscreen)
    {
        show();
        focus();
        //acquireMonitor(window);
        //fitToMonitor(window);
        //
        //if (wndconfig->centerCursor)
        //    _glfwCenterCursorInContentArea(window);
    }
    else
    {
        if (config.hint.visible)
        {
            show();

            if (config.hint.focused)
            {
                focus();
            }
        }
    }
}

bool window::window_impl::create_window(const config& config)
{
    // Get the size and position of the window as requested

    DWORD style = get_window_style();
    DWORD ex_style = get_window_ex_style();

    int frame_x, frame_y, frame_width, frame_height;

    if (config.hint.fullscreen)
    {
        frame_x = display->bounds.position.x;
        frame_y = display->bounds.position.y;
        frame_width = display->bounds.size.x;
        frame_height = display->bounds.size.y;
    }
    else
    {
        // Adjust window size to match requested area

        RECT rect = { 0, 0, config.size.x, config.size.y };

        if (m_maximized)
        {
            style |= WS_MAXIMIZE;
        }

        AdjustWindowRectEx(&rect, style, FALSE, ex_style);

        // Adjust window position to match requested position

        if (config.position.x == VX_DEFAULT_INT && config.position.y == VX_DEFAULT_INT)
        {
            frame_x = CW_USEDEFAULT;
            frame_y = CW_USEDEFAULT;
        }
        else
        {
            frame_x = config.position.x + rect.left;
            frame_y = config.position.y + rect.top;
        }

        frame_width = rect.right - rect.left;
        frame_height = rect.bottom - rect.top;
    }

    // Create window
    m_handle = CreateWindowExW(
        ex_style,
        video_data::window_class_name,
        str::string_to_wstring(config.title).c_str(),
        style,
        frame_x, frame_y,
        frame_width, frame_height,
        NULL, // no parent window
        NULL, // no window menu
        driver_data.instance,
        this
    );

    if (m_handle == NULL)
    {
        return false;
    }

    //if (IsWindows7OrGreater())
    //{
    //    ChangeWindowMessageFilterEx(m_handle, WM_DROPFILES, MSGFLT_ALLOW, NULL);
    //    ChangeWindowMessageFilterEx(m_handle, WM_COPYDATA, MSGFLT_ALLOW, NULL);
    //}

    if (!display)
    {
        RECT rect = { 0, 0, config.size.x, config.size.y };
        WINDOWPLACEMENT wp = { sizeof(wp) };
        const HMONITOR mh = MonitorFromWindow(m_handle, MONITOR_DEFAULTTONEAREST);

        // Obtain the content scale of the monitor that contains the window

        float xscale = 0.0f;
        float yscale = 0.0f;
        get_content_scale(mh, xscale, yscale);

        // Optionally scale the window content area to account for dpi scaling

        if (m_scale_to_monitor)
        {
            if (xscale > 0.0f && yscale > 0.0f)
            {
                rect.right = static_cast<LONG>(rect.right * xscale);
                rect.bottom = static_cast<LONG>(rect.bottom * yscale);
            }
        }

        // Scale the window size to account for dpi scaling

        if (driver_data.user32.AdjustWindowRectExForDpi)
        {
            driver_data.user32.AdjustWindowRectExForDpi(&rect, style, FALSE, ex_style, GetDpiForWindow(m_handle));
        }
        else
        {
            AdjustWindowRectEx(&rect, style, FALSE, ex_style);
        }

        // Move the rect to the restored window position

        GetWindowPlacement(m_handle, &wp);
        OffsetRect(&rect, wp.rcNormalPosition.left - rect.left, wp.rcNormalPosition.top - rect.top);

        // Set the restored window rect to the new adjusted rect

        wp.rcNormalPosition = rect;
        wp.showCmd = SW_HIDE;
        SetWindowPlacement(m_handle, &wp);

        // Adjust rect of maximized undecorated window, because by default Windows will
        // make such a window cover the whole monitor instead of its workarea

        if (m_maximized && m_borderless)
        {
            MONITORINFO mi = { sizeof(mi) };
            GetMonitorInfoW(mh, &mi);

            SetWindowPos(
                m_handle,
                HWND_TOP,
                mi.rcWork.left,
                mi.rcWork.top,
                mi.rcWork.right - mi.rcWork.left,
                mi.rcWork.bottom - mi.rcWork.top,
                SWP_NOACTIVATE | SWP_NOZORDER
            );
        }
    }

    // Set initial mouse position
    //m_last_mouse_position = get_mouse_position();
    //m_mouse_inside_window = is_hovered();

    // Set the default cursor
    //set_cursor(m_last_cursor_object);

    // Finally show the window
    //show();

    return true;
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

    if (false)//(window->monitor)
    {
        style |= WS_POPUP;
    }
    else
    {
        style |= WS_SYSMENU | WS_MINIMIZEBOX;

        if (!m_borderless)
        {
            style |= WS_CAPTION;

            if (m_resizable)
            {
                style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
            }
        }
        else
        {
            style |= WS_POPUP;
        }
    }

    return style;
}

DWORD window::window_impl::get_window_ex_style() const
{
    DWORD style = WS_EX_APPWINDOW;

    if (false)//window->monitor || window->floating)
    {
        style |= WS_EX_TOPMOST;
    }

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
    if (Msg == WM_CREATE)
    {
        // From the data passed into CreateWindow, we can extract the last
        // argument which was a pointer to the window instance. We can then
        // set the user data of the win32 window to a pointer to our window.
        auto window_ptr = reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, window_ptr);
    }

    if (hWnd == NULL)
    {
        return 0;
    }

    // Get the pointer to our associated window
    window::window_impl* window = reinterpret_cast<window::window_impl*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

    if (window)
    {
        if (window->process_event(Msg, wParam, lParam))
        {
            return 0;
        }
    }

    // Prevent os from automatically closing the window
    if (Msg == WM_CLOSE)
    {
        return 0;
    }

    return DefWindowProcW(hWnd, Msg, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////////////
// event checklist
// 
// window:
// - on resize and move events, resize event should post first
// - when the window is minimized or restored, no move event should be posted
// - minimized windows should not be able to change their position
// - focus should be lost when minimizing or hiding window
// - focus should be gained when restoring or showing window
//
// mouse:
// - mouse hover events should be posted only once per enter and exit of the
// window.
// - if the window moves or resizes or is minimized and the mouse leaves or
// enters the window as a result, a mouse hover event should be posted
// - have events should still be generated if the window is not in focus or is
// partially overlapped by another window
///////////////////////////////////////////////////////////////////////////////

bool window::window_impl::process_event(UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (!m_handle)
    {
        return false;
    }

    bool handled = false;

    switch (Msg)
    {
        // =============== window events ===============

        // Destroy window
        case WM_DESTROY:
        {
            on_destroy();

            break;
        }

        // Close window
        case WM_CLOSE:
        {
            event e;
            e.type = event_type::WINDOW_CLOSE;
            post_event(e);

            break;
        }

        // Resize or move window start
        case WM_ENTERSIZEMOVE:
        {
            m_resizing_or_moving = true;
            break;
        }

        // Resize or move window end
        case WM_EXITSIZEMOVE:
        {
            m_resizing_or_moving = false;

            // Check if the size changed
            const math::vec2i new_size = get_size();

            if (m_last_size != new_size)
            {
                m_last_size = new_size;

                event e;
                e.type = event_type::WINDOW_RESIZE;
                e.window_resize.width = new_size.x;
                e.window_resize.height = new_size.y;
                post_event(e);
            }

            // Check if the position changed
            const math::vec2i new_position = get_position();

            if (m_last_position != new_position)
            {
                m_last_position = new_position;

                event e;
                e.type = event_type::WINDOW_MOVE;
                e.window_move.x = new_position.x;
                e.window_move.y = new_position.y;
                post_event(e);
            }

            break;
        }

        // Maximize & minimize window
        case WM_SIZE:
        {
            const math::vec2i new_size = get_size();

            if (m_last_size != new_size)
            {
                if (wParam == SIZE_MINIMIZED && !m_minimized)
                {
                    m_maximized = false;
                    m_minimized = true;

                    event e;
                    e.type = event_type::WINDOW_MINIMIZE;
                    post_event(e);
                }
                if (wParam == SIZE_MAXIMIZED && !m_maximized)
                {
                    m_minimized = false;
                    m_maximized = true;

                    event e;
                    e.type = event_type::WINDOW_MAXIMIZE;
                    post_event(e);
                }
                if (wParam == SIZE_RESTORED)
                {
                    m_maximized = m_minimized = false;
                }

                // Only publish a resize event if we are not currently moving or resizing the window.
                // If we are resizing or moving, a resize or move event will be published when the
                // process finishes anyways.

                if (!m_resizing_or_moving)
                {
                    m_last_size = new_size;

                    event e;
                    e.type = event_type::WINDOW_RESIZE;
                    e.window_resize.width = new_size.x;
                    e.window_resize.height = new_size.y;
                    post_event(e);
                }

                // Update the cursor tracking incase the window moved away from the cursor.
                update_mouse_tracking();
            }

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

            if (!is_minimized())
            {
                const math::vec2i new_position = get_position();

                if (!m_resizing_or_moving && m_last_position != new_position)
                {
                    m_last_position = new_position;

                    event e;
                    e.type = event_type::WINDOW_MOVE;
                    e.window_move.x = new_position.x;
                    e.window_move.y = new_position.y;
                    post_event(e);

                    // Update the cursor tracking incase the window moved away from the cursor.
                    update_mouse_tracking();
                }
            }

            break;
        }

        // Fix violations of minimum or maximum size
        case WM_GETMINMAXINFO:
        {
            const math::vec2i full_min_size = content_size_to_window_size(m_min_size);
            const math::vec2i full_max_size = content_size_to_window_size(m_max_size);

            PMINMAXINFO minmax_info = reinterpret_cast<PMINMAXINFO>(lParam);

            minmax_info->ptMinTrackSize.x = full_min_size.x;
            minmax_info->ptMinTrackSize.y = full_min_size.y;

            minmax_info->ptMaxTrackSize.x = full_max_size.x;
            minmax_info->ptMaxTrackSize.y = full_max_size.y;

            // By default, the max window size is constrained to the monitor.
            // We overwrite this to allow for more flexibility.

            minmax_info->ptMaxSize.x = full_max_size.x;
            minmax_info->ptMaxSize.y = full_max_size.y;

            break;
        }

        // Focus window
        case WM_SETFOCUS:
        {
            if (!m_focussed)
            {
                m_focussed = true;

                event e;
                e.type = event_type::WINDOW_FOCUS;
                e.window_focus.value = true;
                post_event(e);
            }

            break;
        }

        // Un-focus window
        case WM_KILLFOCUS:
        {
            if (m_focussed)
            {
                m_focussed = false;

                event e;
                e.type = event_type::WINDOW_FOCUS;
                e.window_focus.value = false;
                post_event(e);
            }

            break;
        }

        // Show or hide window
        case WM_SHOWWINDOW:
        {
            if (m_visible != static_cast<bool>(wParam))
            {
                m_visible = wParam;

                event e;
                e.type = event_type::WINDOW_SHOW;
                e.window_show.value = wParam;
                post_event(e);

                // Update the cursor tracking incase the cursor moved outside the window.
                update_mouse_tracking();
            }

            break;
        }

        // =============== mouse events ===============

        // Set the cursor
        case WM_SETCURSOR:
        {
            if (LOWORD(lParam) == HTCLIENT && is_hovered())
            {
                SetCursor(m_cursor_visible ? m_last_cursor : NULL);
                handled = true;
            }

            break;
        }

        // Left mouse button down
        case WM_LBUTTONDOWN:
        {
            event e;
            e.type = event_type::MOUSE_BUTTON_DOWN;
            e.mouse_button.button = mouse::BUTTON_LEFT;
            e.mouse_button.x = static_cast<int>(LOWORD(lParam));
            e.mouse_button.y = static_cast<int>(HIWORD(lParam));
            post_event(e);

            break;
        }

        // Left mouse button up
        case WM_LBUTTONUP:
        {
            event e;
            e.type = event_type::MOUSE_BUTTON_UP;
            e.mouse_button.button = mouse::BUTTON_LEFT;
            e.mouse_button.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_button.y = static_cast<int>(GET_Y_LPARAM(lParam));
            post_event(e);

            break;
        }

        // Right mouse button down
        case WM_RBUTTONDOWN:
        {
            event e;
            e.type = event_type::MOUSE_BUTTON_DOWN;
            e.mouse_button.button = mouse::BUTTON_RIGHT;
            e.mouse_button.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_button.y = static_cast<int>(GET_Y_LPARAM(lParam));
            post_event(e);

            break;
        }

        // Right mouse button up
        case WM_RBUTTONUP:
        {
            event e;
            e.type = event_type::MOUSE_BUTTON_UP;
            e.mouse_button.button = mouse::BUTTON_RIGHT;
            e.mouse_button.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_button.y = static_cast<int>(GET_Y_LPARAM(lParam));
            post_event(e);

            break;
        }

        // Mouse wheel button down
        case WM_MBUTTONDOWN:
        {
            event e;
            e.type = event_type::MOUSE_BUTTON_DOWN;
            e.mouse_button.button = mouse::BUTTON_MIDDLE;
            e.mouse_button.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_button.y = static_cast<int>(GET_Y_LPARAM(lParam));
            post_event(e);

            break;
        }

        // Mouse wheel button up
        case WM_MBUTTONUP:
        {
            event e;
            e.type = event_type::MOUSE_BUTTON_UP;
            e.mouse_button.button = mouse::BUTTON_MIDDLE;
            e.mouse_button.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_button.y = static_cast<int>(GET_Y_LPARAM(lParam));
            post_event(e);

            break;
        }

        // Extra mouse button down
        case WM_XBUTTONDOWN:
        {
            event e;
            e.type = event_type::MOUSE_BUTTON_DOWN;
            e.mouse_button.button = (HIWORD(wParam) == XBUTTON1) ? mouse::BUTTON_EXTRA_1 : mouse::button::BUTTON_EXTRA_2;
            e.mouse_button.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_button.y = static_cast<int>(GET_Y_LPARAM(lParam));
            post_event(e);

            break;
        }

        // Extra mouse button up
        case WM_XBUTTONUP:
        {
            event e;
            e.type = event_type::MOUSE_BUTTON_UP;
            e.mouse_button.button = (HIWORD(wParam) == XBUTTON1) ? mouse::BUTTON_EXTRA_1 : mouse::button::BUTTON_EXTRA_2;
            e.mouse_button.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_button.y = static_cast<int>(GET_Y_LPARAM(lParam));
            post_event(e);

            break;
        }

        // Vertical mouse scroll
        case WM_MOUSEWHEEL:
        {
            const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            const math::vec2i mouse_position = get_mouse_position();

            event e;
            e.type = event_type::MOUSE_SCROLL;
            e.mouse_scroll.wheel = mouse::wheel::VERTICAL;
            e.mouse_scroll.delta = static_cast<float>(delta) / static_cast<float>(WHEEL_DELTA);
            e.mouse_scroll.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_scroll.y = static_cast<int>(GET_Y_LPARAM(lParam));
            post_event(e);

            break;
        }

        // Horizontal mouse scroll
        case WM_MOUSEHWHEEL:
        {
            const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            const math::vec2i mouse_position = get_mouse_position();

            event e;
            e.type = event_type::MOUSE_SCROLL;
            e.mouse_scroll.wheel = mouse::wheel::HORIZONTAL;
            e.mouse_scroll.delta = static_cast<float>(delta) / static_cast<float>(WHEEL_DELTA);
            e.mouse_scroll.x = static_cast<int>(GET_X_LPARAM(lParam));
            e.mouse_scroll.y = static_cast<int>(GET_Y_LPARAM(lParam));
            post_event(e);

            break;
        }

        // Mouse move
        case WM_MOUSEMOVE:
        {
            const math::vec2i new_mouse_position = get_mouse_position();

            if (m_last_mouse_position != new_mouse_position)
            {
                m_last_mouse_position = new_mouse_position;

                event e;
                e.type = event_type::MOUSE_MOVE;
                e.mouse_move.x = new_mouse_position.x;
                e.mouse_move.y = new_mouse_position.y;
                post_event(e);

                // Update the cursor tracking incase the cursor moved outside the window.
                update_mouse_tracking();
            }

            break;
        }

        // Mouse leave
        case WM_MOUSELEAVE:
        {
            // Update the cursor tracking incase the cursor moved outside the window.
            update_mouse_tracking();
            break;
        }
    }

    return handled;
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

math::vec2i window::window_impl::get_position() const
{
    POINT pos = { 0, 0 };
    ClientToScreen(m_handle, &pos);
    return math::vec2i(pos.x, pos.y);
}

void window::window_impl::set_position(const math::vec2i& position)
{
    const math::vec2i new_position = content_position_to_window_position(position);
    SetWindowPos(m_handle, NULL, new_position.x, new_position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
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

            event e;
            e.type = event_type::MOUSE_HOVER;
            e.mouse_hover.value = false;
            post_event(e);
        }
    }
    else
    {
        if (!m_mouse_inside_window)
        {
            m_mouse_inside_window = true;
            set_mouse_tracking(true);

            event e;
            e.type = event_type::MOUSE_HOVER;
            e.mouse_hover.value = true;
            post_event(e);
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