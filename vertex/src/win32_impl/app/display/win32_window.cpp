#include "win32_window.h"
#include "vertex/tools/string/string_fn.h"

namespace vx {
namespace app {

// =============== win32 class stuff ===============

static bool make_process_dpi_aware()
{
    return SetProcessDPIAware();
}

size_t window::window_impl::s_window_count = 0;

void window::window_impl::register_window_class()
{
    WNDCLASSW windowClass;
    windowClass.style = 0;
    windowClass.lpfnWndProc = window_proc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandleW(NULL);
    windowClass.hIcon = NULL;
    windowClass.hCursor = NULL;
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = s_class_name;

    RegisterClassW(&windowClass);
}

// =============== window init helpers ===============

window::window_impl::window_impl(const std::string& title, const math::vec2i& size, const math::vec2i& position)
    : m_handle(NULL)
    , m_resizing_or_moving(false)
    , m_last_size(size)
    , m_last_position(position)
    , m_min_size(s_default_min_size)
    , m_max_size(s_default_max_size)
    , m_icon(NULL)
{
    make_process_dpi_aware();

    // If this is our first window, we register our window class
    if (s_window_count == 0)
    {
        register_window_class();
    }

    // Configure window style (https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles)
    const DWORD win32_style = (
        WS_VISIBLE     | // window starts as visible
        WS_CAPTION     | // window has a title bar
        WS_MINIMIZEBOX | // window has minimize button
        WS_MAXIMIZEBOX | // window has maximize button
        WS_SIZEBOX     | // window has a sizing border
        WS_SYSMENU       // window has window menu on titme bar
    );

    // Adjust window size to match requested area
    RECT rect = { 0, 0, size.x, size.y };
    AdjustWindowRect(&rect, win32_style, FALSE);

    const int frame_x = position.x + rect.left;
    const int frame_y = position.y + rect.top;

    const int frame_width = rect.right - rect.left;
    const int frame_height = rect.bottom - rect.top;

    // Create window
    m_handle = CreateWindowW(
        s_class_name,
        tools::str::string_to_wstring(title).c_str(),
        win32_style,
        frame_x, frame_y,
        frame_width, frame_height,
        NULL, // no parent window
        NULL, // no window menu
        GetModuleHandleW(NULL),
        this
    );

    show();

    // Increment the window count
    ++s_window_count;
}

window::window_impl::~window_impl()
{
    // Destroy the custom icon
    if (m_icon)
    {
        DestroyIcon(m_icon);
    }

    // Destroy the window
    if (m_handle)
    {
        DestroyWindow(m_handle);
    }

    // Decrement the window count
    --s_window_count;

    // Unregister window class if we were the last window
    if (s_window_count == 0)
    {
        UnregisterClassW(s_class_name, GetModuleHandleW(NULL));
    }
}

void window::window_impl::on_destroy()
{

}

// =============== open ===============

bool window::window_impl::is_open() const
{
    return m_handle;
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
        
            break;
        }
        
        // Resize window maximize & minimize
        case WM_SIZE:
        {
            const math::vec2i new_size = get_size();
        
            if (m_last_size != new_size)
            {
                if (wParam == SIZE_MINIMIZED)
                {
                    event e;
                    e.type = event_type::WINDOW_MINIMIZE;
                    post_event(e);
                }
                if (wParam == SIZE_MAXIMIZED)
                {
                    event e;
                    e.type = event_type::WINDOW_MAXIMIZE;
                    post_event(e);
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
            event e;
            e.type = event_type::WINDOW_FOCUS;
            e.window_focus.value = true;
            post_event(e);

            break;
        }

        // Un-focus window
        case WM_KILLFOCUS:
        {
            event e;
            e.type = event_type::WINDOW_FOCUS;
            e.window_focus.value = false;
            post_event(e);

            break;
        }

        // Show or hide window
        case WM_SHOWWINDOW:
        {
            event e;
            e.type = event_type::WINDOW_SHOW;
            e.window_show.value = wParam;
            post_event(e);

            break;
        }

        // =============== mouse events ===============

        // Mouse left button down
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

        // Mouse left button up
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

        // Mouse right button down
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

        // Mouse right button up
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
    return tools::str::wstring_to_string(wtitle);
}

void window::window_impl::set_title(const std::string& title)
{
    SetWindowTextW(m_handle, tools::str::string_to_wstring(title).c_str());
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

    // Instruct the system to post a WM_GETMINMAXINFO so the changes can take effect.
    SetWindowPos(m_handle, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

void window::window_impl::set_max_size(const math::vec2i& size)
{
    m_max_size = size;

    // Instruct the system to post a WM_GETMINMAXINFO so the changes can take effect.
    SetWindowPos(m_handle, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

// =============== window ops ===============

void window::window_impl::show()
{
    ShowWindow(m_handle, SW_SHOW);
}

void window::window_impl::hide()
{
    ShowWindow(m_handle, SW_HIDE);
}

bool window::window_impl::is_visible() const
{
    return IsWindowVisible(m_handle);
}

void window::window_impl::minimize()
{
    // We need to call show first or else we will not receive a WM_SHOWWINDOW message
    ShowWindow(m_handle, SW_SHOW);
    ShowWindow(m_handle, SW_MINIMIZE);
}

bool window::window_impl::is_minimized() const
{
    return IsIconic(m_handle);
}

void window::window_impl::maximize()
{
    // We need to call show first or else we will not receive a WM_SHOWWINDOW message
    ShowWindow(m_handle, SW_SHOW);
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

bool window::window_impl::request_focus()
{
    // We only want to steal focus from a window if it belongs to the same process as the current window
    DWORD this_pid;
    DWORD foreground_pid;
    GetWindowThreadProcessId(m_handle, &this_pid);
    GetWindowThreadProcessId(GetForegroundWindow(), &foreground_pid);

    if (this_pid == foreground_pid)
    {
        // The window requesting focus belongs to the same process as the current window: steal focus
        SetForegroundWindow(m_handle);
        return true;
    }

    // Different process: don't steal focus, but raquest attention
    request_attention();
    return false;
}

bool window::window_impl::is_focused() const
{
    // GetForegroundWindow returns the active window of any process
    return m_handle == GetForegroundWindow();
}

void window::window_impl::request_attention()
{
    FlashWindow(m_handle, TRUE);
}

// =============== icon ===============

void window::window_impl::set_icon(const img::image& icon)
{
    clear_icon();

    // Format is expected to have 4 8-bit channels in BGRA format
    img::image formatted_icon(icon.width(), icon.height(), img::image_format::RGBA8);

    // Convert the image to an RGBA-8 format and swizzle each pixel
    for (size_t y = 0; y < icon.height(); ++y)
    {
        for (size_t x = 0; x < icon.width(); ++x)
        {
            const math::color p = icon.get_pixel(x, y);
            formatted_icon.set_pixel(x, y, math::color(p.b, p.g, p.r, p.a));
        }
    }

    // MSVC warns about arument 6 being NULL
    VX_DISABLE_WARNING("", 6387);
    VX_DISABLE_WARNING_PUSH();

    // Create the icon
    m_icon = CreateIcon(
        GetModuleHandleW(NULL),
        static_cast<int>(formatted_icon.width()),
        static_cast<int>(formatted_icon.height()),
        1,
        32,
        NULL,
        formatted_icon.data()
    );

    VX_DISABLE_WARNING_POP();

    // Set it as both big and small icon of the window
    if (m_icon)
    {
        SendMessageW(m_handle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(m_icon));
        SendMessageW(m_handle, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(m_icon));
    }
    else
    {
        throw_error(error_code::INTERNAL, "Failed to set window icon");
    }

}

void window::window_impl::clear_icon()
{
    if (m_icon)
    {
        DestroyIcon(m_icon);
        m_icon = NULL;
    }
}

}
}