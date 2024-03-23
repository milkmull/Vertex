#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../window.h"
#include "vertex/tools/debug/logger.h"

// https://www.glfw.org/docs/3.3/window_guide.html
// https://www.glfw.org/docs/3.3/window_guide.html#window_creation

// TODO: fullscreen with monitors, creation flags (center cursor), mouse passthrough

namespace vx {
namespace app {

static void glfwErrorCallback(int error, const char* description)
{
    VX_LOG_ERROR << "GLFW Error (" << error << "): " << description;
}

window::window(const window_specs& specs)
{
    open(specs);
}

window::~window()
{
    close();
}

void window::open(const window_specs& specs)
{
    // initialize glfw

    static bool GLFW_initialized = false;

    if (!GLFW_initialized)
    {
        int success = glfwInit();
        VX_ASSERT(success, "Could not initialize GLFW\n");

        GLFW_initialized = true;
        glfwSetErrorCallback(glfwErrorCallback);
    }

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

}
}