#include <GLFW/glfw3.h>

#include <unordered_map>

#include "vertex/app/display/window.h"
#include "vertex_impl/app/init_internal.h"

// https://www.glfw.org/docs/3.3/window_guide.html
// https://www.glfw.org/docs/3.3/window_guide.html#window_creation

// TODO: fullscreen with monitors, creation flags (center cursor), mouse passthrough
// TODO: lock aespect, get min and max sizes, content scale values, video modes

namespace vx {
namespace app {

int window::s_window_count = 0;

window::window(const window_specs& specs)
{
    open(specs);

    ++s_window_count;
}

window::~window()
{   
    close();

    --s_window_count;
    if (s_window_count == 0)
    {
        glfwTerminate();
    }
}

void window::open(const window_specs& specs)
{
    // initialize glfw

    VX_REQUIRE_INIT();

    // set window hints

    //glfwWindowHint(GLFW_RESIZABLE, specs.flags & window_flag::RESIZEABLE);
    //glfwWindowHint(GLFW_DECORATED, ~(specs.flags & window_flag::BORDERLESS));
    //glfwWindowHint(GLFW_FOCUSED, specs.flags & window_flag::FOCUSSED);
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, specs.flags & window_flag::TRANSPARENT);

    // create window

    m_window = glfwCreateWindow(specs.size.x, specs.size.y, specs.title, nullptr, nullptr);
    glfwSetWindowUserPointer(static_cast<GLFWwindow*>(m_window), this);

    // window close callback

    glfwSetWindowCloseCallback(static_cast<GLFWwindow*>(m_window), [](GLFWwindow* window)
    {
        ::vx::app::window* vx_window = static_cast<::vx::app::window*>(glfwGetWindowUserPointer(window));

        if (vx_window->m_event_callback)
        {
            event e;
            e.type = event_type::WINDOW_CLOSE;
            
            vx_window->m_event_callback(e);
        }
    });

    // window size callback

    glfwSetWindowSizeCallback(static_cast<GLFWwindow*>(m_window), [](GLFWwindow* window, int width, int height)
    {
        ::vx::app::window* vx_window = static_cast<::vx::app::window*>(glfwGetWindowUserPointer(window));

        if (vx_window->m_event_callback)
        {
            event e;
            e.type = event_type::WINDOW_RESIZE;
            e.window_resize.width = width;
            e.window_resize.height = height;

            vx_window->m_event_callback(e);
        }
    });

    // window position callback

    glfwSetWindowPosCallback(static_cast<GLFWwindow*>(m_window), [](GLFWwindow* window, int xpos, int ypos)
    {
        ::vx::app::window* vx_window = static_cast<::vx::app::window*>(glfwGetWindowUserPointer(window));

        if (vx_window->m_event_callback)
        {
            event e;
            e.type = event_type::WINDOW_MOVE;
            e.window_move.x = xpos;
            e.window_move.y = ypos;

            vx_window->m_event_callback(e);
        }
    });

    // window iconify callback

    glfwSetWindowIconifyCallback(static_cast<GLFWwindow*>(m_window), [](GLFWwindow* window, int iconified)
    {
        ::vx::app::window* vx_window = static_cast<::vx::app::window*>(glfwGetWindowUserPointer(window));

        if (vx_window->m_event_callback)
        {
            event e;
            e.type = event_type::WINDOW_ICONIFY;
            e.window_iconify.value = iconified;

            vx_window->m_event_callback(e);
        }
    });

    // window maximize callback

    glfwSetWindowMaximizeCallback(static_cast<GLFWwindow*>(m_window), [](GLFWwindow* window, int maximized)
    {
        ::vx::app::window* vx_window = static_cast<::vx::app::window*>(glfwGetWindowUserPointer(window));

        if (vx_window->m_event_callback)
        {
            event e;
            e.type = event_type::WINDOW_MAXIMIZE;
            e.window_maximize.value = maximized;

            vx_window->m_event_callback(e);
        }
    });

    // window focus callback

    glfwSetWindowFocusCallback(static_cast<GLFWwindow*>(m_window), [](GLFWwindow* window, int focused)
    {
        ::vx::app::window* vx_window = static_cast<::vx::app::window*>(glfwGetWindowUserPointer(window));

        if (vx_window->m_event_callback)
        {
            event e;
            e.type = event_type::WINDOW_FOCUS;
            e.window_focus.value = focused;

            vx_window->m_event_callback(e);
        }
    });

    // key callback

    glfwSetKeyCallback(static_cast<GLFWwindow*>(m_window), [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        ::vx::app::window* vx_window = static_cast<::vx::app::window*>(glfwGetWindowUserPointer(window));

        if (vx_window->m_event_callback)
        {
            event e;

            switch (action)
            {
                case GLFW_PRESS:   e.type = event_type::KEY_DOWN;   break;
                case GLFW_RELEASE: e.type = event_type::KEY_UP;     break;
                case GLFW_REPEAT:  e.type = event_type::KEY_REPEAT; break;
                default:                                            return;
            }

            e.key.key = static_cast<keyboard::key>(key);
            e.key.scancode = scancode;

            vx_window->m_event_callback(e);
        }
    });

    // text callback

    glfwSetCharCallback(static_cast<GLFWwindow*>(m_window), [](GLFWwindow* window, unsigned int codepoint)
    {
        ::vx::app::window* vx_window = static_cast<::vx::app::window*>(glfwGetWindowUserPointer(window));

        if (vx_window->m_event_callback)
        {
            event e;
            e.type = event_type::TEXT_INPUT;
            e.text_input.codepoint = codepoint;

            vx_window->m_event_callback(e);
        }
    });

    // mouse position callback

    glfwSetCursorPosCallback(static_cast<GLFWwindow*>(m_window), [](GLFWwindow* window, double xpos, double ypos)
    {
        ::vx::app::window* vx_window = static_cast<::vx::app::window*>(glfwGetWindowUserPointer(window));

        if (vx_window->m_event_callback)
        {
            event e;
            e.type = event_type::CURSOR_MOVE;
            e.cursor_move.x = static_cast<int>(xpos);
            e.cursor_move.y = static_cast<int>(ypos);

            vx_window->m_event_callback(e);
        }
    });

    // mouse enter/leave callback

    glfwSetCursorEnterCallback(static_cast<GLFWwindow*>(m_window), [](GLFWwindow* window, int entered)
    {
        ::vx::app::window* vx_window = static_cast<::vx::app::window*>(glfwGetWindowUserPointer(window));

        if (vx_window->m_event_callback)
        {
            event e;
            e.type = event_type::CURSOR_ENTER;
            e.cursor_enter.value = entered;

            vx_window->m_event_callback(e);
        }
    });

    // mouse button callback

    glfwSetMouseButtonCallback(static_cast<GLFWwindow*>(m_window), [](GLFWwindow* window, int button, int action, int mods)
    {
        ::vx::app::window* vx_window = static_cast<::vx::app::window*>(glfwGetWindowUserPointer(window));

        if (vx_window->m_event_callback)
        {
            event e;

            switch (action)
            {
                case GLFW_PRESS:   e.type = event_type::MOUSE_BUTTON_DOWN; break;
                case GLFW_RELEASE: e.type = event_type::MOUSE_BUTTON_UP;   break;
                default:                                                   return;
            }

            e.mouse_button.button = static_cast<mouse::button>(button);

            vx_window->m_event_callback(e);
        }
    });

    // scroll callback

    glfwSetScrollCallback(static_cast<GLFWwindow*>(m_window), [](GLFWwindow* window, double xoffset, double yoffset)
    {
        ::vx::app::window* vx_window = static_cast<::vx::app::window*>(glfwGetWindowUserPointer(window));

        if (vx_window->m_event_callback)
        {
            event e;
            e.type = event_type::MOUSE_SCROLL;
            e.mouse_scroll.x = static_cast<float>(xoffset);
            e.mouse_scroll.y = static_cast<float>(yoffset);

            vx_window->m_event_callback(e);
        }
    });

}

void window::close()
{
    glfwDestroyWindow(static_cast<GLFWwindow*>(m_window));
    m_window = nullptr;

    glfwTerminate();
}

bool window::should_close() const
{
    return glfwWindowShouldClose(static_cast<GLFWwindow*>(m_window));
}

void window::set_should_close(bool should_close)
{
    glfwSetWindowShouldClose(static_cast<GLFWwindow*>(m_window), should_close);
}

void window::poll_events()
{
    glfwPollEvents();
}

// =============== title ===============

const char* window::get_title() const
{
    return glfwGetWindowTitle(static_cast<GLFWwindow*>(m_window));
}

void window::set_title(const char* title)
{
    glfwSetWindowTitle(static_cast<GLFWwindow*>(m_window), title);
}

// =============== position ===============

math::vec2i window::get_position() const
{
    math::vec2i pos;
    glfwGetWindowPos(static_cast<GLFWwindow*>(m_window), &pos.x, &pos.y);
    return pos;
}

void window::set_position(const math::vec2i& pos)
{
    glfwSetWindowPos(static_cast<GLFWwindow*>(m_window), pos.x, pos.y);
}

// =============== size ===============

math::vec2i window::get_size() const
{
    math::vec2i size;
    glfwGetWindowSize(static_cast<GLFWwindow*>(m_window), &size.x, &size.y);
    return size;
}

void window::set_size(const math::vec2i& size)
{
    glfwSetWindowPos(static_cast<GLFWwindow*>(m_window), size.x, size.y);
}

void window::set_min_size(const math::vec2i& size)
{
    glfwSetWindowSizeLimits(static_cast<GLFWwindow*>(m_window), size.x, size.y, GLFW_DONT_CARE, GLFW_DONT_CARE);
}

void window::set_max_size(const math::vec2i& size)
{
    glfwSetWindowSizeLimits(static_cast<GLFWwindow*>(m_window), GLFW_DONT_CARE, GLFW_DONT_CARE, size.x, size.y);
}

// =============== resizable ===============

void window::set_resizable(bool resizable)
{
    glfwSetWindowAttrib(static_cast<GLFWwindow*>(m_window), GLFW_RESIZABLE, resizable);
}

bool window::is_resizable() const
{
    return glfwGetWindowAttrib(static_cast<GLFWwindow*>(m_window), GLFW_RESIZABLE);
}

// =============== bordered ===============

void window::set_bordered(bool bordered)
{
    glfwSetWindowAttrib(static_cast<GLFWwindow*>(m_window), GLFW_DECORATED, bordered);
}

bool window::is_bordered() const
{
    return glfwGetWindowAttrib(static_cast<GLFWwindow*>(m_window), GLFW_DECORATED);
}

// =============== always on top ===============

void window::set_always_on_top(bool always_on_top)
{
    glfwSetWindowAttrib(static_cast<GLFWwindow*>(m_window), GLFW_FLOATING, always_on_top);
}

bool window::is_always_on_top() const
{
    return glfwGetWindowAttrib(static_cast<GLFWwindow*>(m_window), GLFW_FLOATING);
}

// =============== iconify and maximize ===============

void window::iconify()
{
    glfwIconifyWindow(static_cast<GLFWwindow*>(m_window));
}

bool window::is_iconified() const
{
    return glfwGetWindowAttrib(static_cast<GLFWwindow*>(m_window), GLFW_ICONIFIED);
}

void window::maximize()
{
    glfwMaximizeWindow(static_cast<GLFWwindow*>(m_window));
}

bool window::is_maximized() const
{
    return glfwGetWindowAttrib(static_cast<GLFWwindow*>(m_window), GLFW_MAXIMIZED);
}

void window::restore()
{
    glfwRestoreWindow(static_cast<GLFWwindow*>(m_window));
}

// =============== hide and show ===============

void window::hide()
{
    glfwHideWindow(static_cast<GLFWwindow*>(m_window));
}

void window::show()
{
    glfwShowWindow(static_cast<GLFWwindow*>(m_window));
}

bool window::is_visible() const
{
    return glfwGetWindowAttrib(static_cast<GLFWwindow*>(m_window), GLFW_VISIBLE);
}

// =============== focus ===============

void window::focus()
{
    glfwFocusWindow(static_cast<GLFWwindow*>(m_window));
}

bool window::is_focused() const
{
    return glfwGetWindowAttrib(static_cast<GLFWwindow*>(m_window), GLFW_FOCUSED);
}

// =============== attention ===============

void window::request_attention()
{
    glfwRequestWindowAttention(static_cast<GLFWwindow*>(m_window));
}

// =============== opacity ===============

float window::get_opacity() const
{
    return glfwGetWindowOpacity(static_cast<GLFWwindow*>(m_window));
}

void window::set_opacity(float opacity)
{
    glfwSetWindowOpacity(static_cast<GLFWwindow*>(m_window), opacity);
}

// =============== hovered ===============

bool window::is_hovered() const
{
    return glfwGetWindowAttrib(static_cast<GLFWwindow*>(m_window), GLFW_HOVERED);
}

// =============== icon ===============

void window::set_icon(const img::image& icon)
{
    GLFWimage image;

    image.width = static_cast<int>(icon.width());
    image.height = static_cast<int>(icon.height());
    image.pixels = (unsigned char*)icon.data();

    glfwSetWindowIcon(static_cast<GLFWwindow*>(m_window), 1, &image);
}

void window::clear_icon()
{
    glfwSetWindowIcon(static_cast<GLFWwindow*>(m_window), 0, nullptr);
}

// =============== cursor ===============

math::vec2 window::get_cursor_position() const
{
    double xpos, ypos;
    glfwGetCursorPos(static_cast<GLFWwindow*>(m_window), &xpos, &ypos);
    return math::vec2(xpos, ypos);
}

void window::set_cursor_position(const math::vec2& cursor_position)
{
    glfwSetCursorPos(static_cast<GLFWwindow*>(m_window), cursor_position.x, cursor_position.y);
}

// GLFW will deallocate GLFWcursor objects when the program terminates
static std::unordered_map<int, GLFWcursor*> s_cursor_cache;

void window::set_cursor_shape(cursor::shape shape)
{
    int glfw_shape;

    switch (shape)
    {
        case cursor::shape::ARROW:       glfw_shape = GLFW_ARROW_CURSOR;       break;
        case cursor::shape::IBEAM:       glfw_shape = GLFW_IBEAM_CURSOR;       break;
        case cursor::shape::CROSSHAIR:   glfw_shape = GLFW_CROSSHAIR_CURSOR;   break;
        case cursor::shape::HAND:        glfw_shape = GLFW_HAND_CURSOR;        break;
        case cursor::shape::HRESIZE:     glfw_shape = GLFW_HRESIZE_CURSOR;     break;
        case cursor::shape::VRESIZE:     glfw_shape = GLFW_VRESIZE_CURSOR;     break;
        case cursor::shape::ALL_RESIZE:  glfw_shape = GLFW_RESIZE_ALL_CURSOR;  break;
        case cursor::shape::NOT_ALLOWED: glfw_shape = GLFW_NOT_ALLOWED_CURSOR; break;
        default:                                                               return;
    }

    if (s_cursor_cache.find(glfw_shape) == s_cursor_cache.end())
    {
        GLFWcursor* cursor = glfwCreateStandardCursor(glfw_shape);
        if (!cursor) return;

        s_cursor_cache[glfw_shape] = cursor;
    }

    glfwSetCursor(static_cast<GLFWwindow*>(m_window), s_cursor_cache[glfw_shape]);
}

void window::set_cursor_shape(const img::image& shape, int shape_id, const math::vec2i& hotspot)
{
    assert(shape_id > static_cast<int>(cursor::shape::NOT_ALLOWED));

    if (s_cursor_cache.find(shape_id) == s_cursor_cache.end())
    {
        GLFWimage image;

        image.width = static_cast<int>(shape.width());
        image.height = static_cast<int>(shape.height());
        image.pixels = (unsigned char*)shape.data();

        GLFWcursor* cursor = glfwCreateCursor(&image, hotspot.x, hotspot.y);
        if (!cursor) return;

        s_cursor_cache[shape_id] = cursor;
    }

    glfwSetCursor(static_cast<GLFWwindow*>(m_window), s_cursor_cache[shape_id]);
}

void window::clear_cursor_shape()
{
    glfwSetCursor(static_cast<GLFWwindow*>(m_window), nullptr);
}

cursor::mode window::get_cursor_mode() const
{
    const int glfw_cursor_mode = glfwGetInputMode(static_cast<GLFWwindow*>(m_window), GLFW_CURSOR);

    switch (glfw_cursor_mode)
    {
        case GLFW_CURSOR_NORMAL:   return cursor::mode::NORMAL;
        case GLFW_CURSOR_HIDDEN:   return cursor::mode::HIDDEN;
        case GLFW_CURSOR_DISABLED: return cursor::mode::DISABLED;
        default:                   break;
    }

    return cursor::mode::NORMAL;
}

void window::set_cursor_mode(cursor::mode mode)
{
    int glfw_cursor_mode;

    switch (mode)
    {
        case cursor::mode::NORMAL:   glfw_cursor_mode = GLFW_CURSOR_NORMAL;   break;
        case cursor::mode::HIDDEN:   glfw_cursor_mode = GLFW_CURSOR_HIDDEN;   break;
        case cursor::mode::DISABLED: glfw_cursor_mode = GLFW_CURSOR_DISABLED; break;
        default:                                                              return;
    }

    glfwSetInputMode(static_cast<GLFWwindow*>(m_window), GLFW_CURSOR, glfw_cursor_mode);
}

// =============== mouse button state ===============

bool window::is_mouse_button_pressed(mouse::button button) const
{
    return glfwGetMouseButton(static_cast<GLFWwindow*>(m_window), static_cast<int>(button));
}

// =============== key state ===============

bool window::is_key_pressed(keyboard::key key) const
{
    return glfwGetKey(static_cast<GLFWwindow*>(m_window), static_cast<int>(key));
}

}
}