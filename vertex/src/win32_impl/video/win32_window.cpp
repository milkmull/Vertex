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

#define STYLE_BASIC               (WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define STYLE_FULLSCREEN          (WS_POPUP | WS_MINIMIZEBOX)
#define STYLE_BORDERLESS          (WS_POPUP | WS_MINIMIZEBOX)
#define STYLE_BORDERLESS_WINDOWED (WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
#define STYLE_NORMAL              (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
#define STYLE_RESIZABLE           (WS_THICKFRAME | WS_MAXIMIZEBOX)
#define STYLE_MASK                (STYLE_FULLSCREEN | STYLE_BORDERLESS | STYLE_NORMAL | STYLE_RESIZABLE)

// =============== win32 class stuff ===============

static bool make_process_dpi_aware()
{
    // https://learn.microsoft.com/en-gb/windows/win32/hidpi/high-dpi-desktop-application-development-on-windows?redirectedfrom=MSDN

    bool success = false;

    // Try SetProcessDpiAwareness first
    HINSTANCE sh_core_dll = LoadLibrary(L"Shcore.dll");

    if (sh_core_dll)
    {
        // https://learn.microsoft.com/en-us/windows/win32/api/shellscalingapi/ne-shellscalingapi-process_dpi_awareness

        enum PROCESS_DPI_AWARENESS
        {
            PROCESS_DPI_UNAWARE = 0,
            PROCESS_SYSTEM_DPI_AWARE = 1,
            PROCESS_PER_MONITOR_DPI_AWARE = 2
        };

        using SetProcessDpiAwarenessFuncType = HRESULT(WINAPI*)(PROCESS_DPI_AWARENESS);
        auto SetProcessDpiAwareness = reinterpret_cast<SetProcessDpiAwarenessFuncType>(
            reinterpret_cast<void*>(GetProcAddress(sh_core_dll, "SetProcessDpiAwareness"))
        );

        // Check for E_INVALIDARG. A value of E_ACCESSDENIED incicates
        // that the dpi has already been set, and a value of S_OK
        // indicates that the process was successful.
        // 
        // We intentionally don't use Per Monitor V2 which can be
        // enabled with SetProcessDpiAwarenessContext, because that
        // would scale the title bar and thus change window size
        // by default when moving the window between monitors.

        if (SetProcessDpiAwareness && SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE) != E_INVALIDARG)
        {
            success = true;
        }

        FreeLibrary(sh_core_dll);
    }

    if (!success)
    {
        // Fall back to SetProcessDPIAware if SetProcessDpiAwareness
        // is not available on this system
        HINSTANCE user_32_dll = LoadLibrary(L"user32.dll");

        if (user_32_dll)
        {
            using SetProcessDPIAwareFuncType = BOOL(WINAPI*)();
            auto SetProcessDPIAware = reinterpret_cast<SetProcessDPIAwareFuncType>(
                reinterpret_cast<void*>(GetProcAddress(user_32_dll, "SetProcessDPIAware"))
            );

            if (SetProcessDPIAware && SetProcessDPIAware())
            {
                success = true;
            }

            FreeLibrary(user_32_dll);
        }
    }

    if (!success)
    {
        VX_ERROR(error::error_code::PLATFORM_ERROR) << "Failed to set process DPI awareness";
    }

    return success;
}

size_t window::window_impl::s_window_count = 0;

void window::window_impl::register_window_class()
{
    WNDCLASSW windowClass{};
    windowClass.style = 0;
    windowClass.lpfnWndProc = window_proc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandleW(NULL);
    windowClass.hIcon = NULL;
    windowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = s_class_name;

    RegisterClassW(&windowClass);
}

// =============== window init helpers ===============

window::window_impl::window_impl(const std::string& title, const math::vec2i& size, const math::vec2i& position, style style)
    : m_handle(NULL)
    , m_visible(true)
    , m_focussed(true)
    , m_minimized(false)
    , m_maximized(false)
    , m_fullscreen(false)
    , m_resizing_or_moving(false)
    , m_last_size(size)
    , m_last_position(position)
    , m_min_size(s_default_min_size)
    , m_max_size(s_default_max_size)
    , m_icon(NULL)
    , m_last_cursor_object(cursor::cursor_shape::SHAPE_ARROW)
{
    make_process_dpi_aware();

    // If this is our first window, we register our window class
    if (s_window_count == 0)
    {
        register_window_class();
    }

    // Configure window style (https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles)
    DWORD win32_style = STYLE_NORMAL;

    if (style & style::BORDERLESS)
    {
        win32_style |= STYLE_BORDERLESS;
    }
    if (style & style::RESIZABLE && !(style & style::BORDERLESS))
    {
        // You can have a borderless resizable window, but Windows doesn't always draw
        // it correctly, see https://bugzilla.libsdl.org/show_bug.cgi?id=4466
        win32_style |= STYLE_RESIZABLE;
    }

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
        str::string_to_wstring(title).c_str(),
        win32_style,
        frame_x, frame_y,
        frame_width, frame_height,
        NULL, // no parent window
        NULL, // no window menu
        GetModuleHandleW(NULL),
        this
    );

    // Set initial mouse position
    m_last_mouse_position = get_mouse_position();
    m_mouse_inside_window = is_hovered();

    // Set the default cursor
    set_cursor(m_last_cursor_object);

    // Finally show the window
    show();

    // Increment the window count
    ++s_window_count;
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
    --s_window_count;

    // If we were the last window...
    if (s_window_count == 0)
    {
        // Unregister window class
        UnregisterClassW(s_class_name, GetModuleHandleW(NULL));
    }
}

const window_handle window::window_impl::get_native_handle() const
{
    return static_cast<window_handle>(m_handle);
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
    TRACKMOUSEEVENT e{};
    e.cbSize = sizeof(e);
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

cursor window::window_impl::get_cursor() const
{
    return m_last_cursor_object;
}

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