#include "win32_window.h"
#include "vertex/tools/string/string_fn.h"

namespace vx {
namespace app {

// =============== win32 class stuff ===============

static bool make_process_dpi_aware()
{
    return SetProcessDPIAware();
}

size_t win32_window::s_window_count = 0;

void win32_window::register_window_class()
{
    WNDCLASSW windowClass;
    windowClass.style = 0;
    windowClass.lpfnWndProc = window_proc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandleW(nullptr);
    windowClass.hIcon = nullptr;
    windowClass.hCursor = nullptr;
    windowClass.hbrBackground = nullptr;
    windowClass.lpszMenuName = nullptr;
    windowClass.lpszClassName = s_class_name;

    RegisterClassW(&windowClass);
}

// =============== window init helpers ===============

win32_window::win32_window(const std::string& title, const math::vec2i& size, const math::vec2i& position)
    : m_last_size(size), m_last_position(position)
{
    make_process_dpi_aware();

    // if this is our first window, we register our window class
    if (s_window_count == 0)
    {
        register_window_class();
    }

    // configure window style (https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles)
    const DWORD win32_style = (
        WS_VISIBLE     | // window starts as visible
        WS_CAPTION     | // window has a title bar
        WS_MINIMIZEBOX | // window has minimize button
        WS_MAXIMIZEBOX | // window has maximize button
        WS_SIZEBOX     | // window has a sizing border
        WS_SYSMENU       // window has window menu on titme bar
    );

    // adjust window size to match requested area
    RECT rect = { 0, 0, size.x, size.y };
    AdjustWindowRect(&rect, win32_style, false);

    const int frame_x = position.x + rect.left;
    const int frame_y = position.y + rect.top;

    const int frame_width = rect.right - rect.left;
    const int frame_height = rect.bottom - rect.top;

    // create window
    m_handle = CreateWindowW(
        s_class_name,
        tools::str::string_to_wstring(title).c_str(),
        win32_style,
        frame_x, frame_y,
        frame_width, frame_height,
        nullptr, // no parent window
        nullptr, // no window menu
        GetModuleHandleW(nullptr),
        this
    );

    // increment the window count
    ++s_window_count;
}

win32_window::~win32_window()
{
    // destroy the window
    if (m_handle)
    {
        DestroyWindow(m_handle);
    }

    // decrement the window count
    --s_window_count;

    // unregister window class if we were the last window
    if (s_window_count == 0)
    {
        UnregisterClassW(s_class_name, GetModuleHandleW(nullptr));
    }
}

void win32_window::on_destroy()
{

}

// =============== open ===============

bool win32_window::is_open() const
{
    return m_handle;
}

// =============== events ===============

LRESULT CALLBACK win32_window::window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (Msg == WM_CREATE)
    {
        // from the data passed into CreateWindow, we can extract the last
        // argument which was a pointer to the window instance we can then
        // set the user data of the win32 window to a pointer to our window
        auto window_ptr = reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, window_ptr);
    }

    if (hWnd == nullptr)
    {
        return 0;
    }

    // get the pointer to our associated window
    win32_window* window = reinterpret_cast<win32_window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

    if (window)
    {
        window->process_event(Msg, wParam, lParam);
    }

    // prevent os from automatically closing the window
    if (Msg == WM_CLOSE)
    {
        return 0;
    }

    return DefWindowProcW(hWnd, Msg, wParam, lParam);
}

void win32_window::process_event(UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (!m_handle)
    {
        return;
    }

    switch (Msg)
    {
        // =============== window events ===============

        // destroy window
        case WM_DESTROY:
        {
            on_destroy();

            break;
        }

        // close window
        case WM_CLOSE:
        {
            event e;
            e.type = event_type::WINDOW_CLOSE;
            post_event(e);

            break;
        }

        // resize or move window start
        case WM_ENTERSIZEMOVE:
        {
            m_resizing_or_moving = true;
            break;
        }
        
        // resize or move window end
        case WM_EXITSIZEMOVE:
        {
            m_resizing_or_moving = false;
        
            // check if the position changed
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
        
            // check if the size changed
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

        // resize window maximize & minimize
        case WM_SIZE:
        {
            const math::vec2i new_size = get_size();

            if (!m_resizing_or_moving && m_last_size != new_size)
            {
                if (wParam == SIZE_MINIMIZED)
                {
                    event e;
                    e.type = event_type::WINDOW_ICONIFY;
                    post_event(e);
                }
                else if (wParam == SIZE_MAXIMIZED)
                {
                    event e;
                    e.type = event_type::WINDOW_MAXIMIZE;
                    post_event(e);
                }

                m_last_size = new_size;

                event e;
                e.type = event_type::WINDOW_RESIZE;
                e.window_resize.width = new_size.x;
                e.window_resize.height = new_size.y;
                post_event(e);
            }
        
            break;
        }

        // focus window
        case WM_SETFOCUS:
        {
            event e;
            e.type = event_type::WINDOW_FOCUS;
            e.window_focus.value = true;
            post_event(e);

            break;
        }

        // un-focus window
        case WM_KILLFOCUS:
        {
            event e;
            e.type = event_type::WINDOW_FOCUS;
            e.window_focus.value = false;
            post_event(e);

            break;
        }

        // =============== mouse events ===============

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

        // Mouse left button up event
        case WM_LBUTTONUP:
        {
            event e;
            e.type = event_type::MOUSE_BUTTON_UP;
            e.mouse_button.button = mouse::BUTTON_LEFT;
            e.mouse_button.x = static_cast<int>(LOWORD(lParam));
            e.mouse_button.y = static_cast<int>(HIWORD(lParam));
            post_event(e);

            break;
        }
    }
}

void win32_window::process_events()
{
    MSG Msg;

    while (PeekMessageW(&Msg, nullptr, 0u, 0u, PM_REMOVE))
    {
        TranslateMessage(&Msg);
        DispatchMessageW(&Msg);
    }
}

// =============== title ===============

std::string win32_window::get_title() const
{
    // check the title length
    int length = GetWindowTextLengthW(m_handle);

    // create a buffer to hold the wide title
    std::wstring wtitle(static_cast<size_t>(length), 0);
    GetWindowTextW(m_handle, reinterpret_cast<LPWSTR>(wtitle.data()), length);

    // convert the wide string
    return tools::str::wstring_to_string(wtitle);
}

void win32_window::set_title(const std::string& title)
{
    SetWindowTextW(m_handle, tools::str::string_to_wstring(title).c_str());
}

// =============== position ===============

math::vec2i win32_window::get_position() const
{
    POINT pos = { 0, 0 };
    ClientToScreen(m_handle, &pos);
    return math::vec2i(pos.x, pos.y);
}

void win32_window::set_position(const math::vec2i& position)
{
    // we specify a rect with the desired position, then call AdjustWindowRect to
    // account for the window frame since SetWindowPos expects the frame to be included
    RECT rect = { position.x, position.y, 0, 0 };
    AdjustWindowRect(&rect, static_cast<DWORD>(GetWindowLongPtrW(m_handle, GWL_STYLE)), false);
    SetWindowPos(m_handle, nullptr, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

// =============== size ===============

math::vec2i win32_window::get_size() const
{
    RECT rect;
    GetClientRect(m_handle, &rect);
    return math::vec2i(rect.right, rect.bottom);
}

void win32_window::set_size(const math::vec2i& size)
{
    // we specify a rect with the desired size, then call AdjustWindowRect to
    // account for the window frame since SetWindowPos expects the frame to be included
    RECT rect = { 0, 0, size.x, size.y };
    AdjustWindowRect(&rect, static_cast<DWORD>(GetWindowLongPtrW(m_handle, GWL_STYLE)), false);
    SetWindowPos(m_handle, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
}

}
}