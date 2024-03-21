#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "window.h"
#include "vertex/tools/debug/logger.h"

namespace vx {
namespace app {

static void glfwErrorCallback(int error, const char* description)
{
    VX_LOG_ERROR << "GLFW Error (" << error << "): " << description;
}

window::window()
{
    open();
}

window::~window()
{
    close();
}

void window::open()
{
    static bool GLFW_initialized = false;

    struct
    {
        int width = 200;
        int height = 200;
        const char* title = "vertex";
    } data;

    if (!GLFW_initialized)
    {
        int success = glfwInit();
        VX_ASSERT(success, "Could not initialize GLFW\n");

        GLFW_initialized = true;
        glfwSetErrorCallback(glfwErrorCallback);
    }

    m_window = glfwCreateWindow(data.width, data.height, data.title, nullptr, nullptr);

    glfwSetWindowUserPointer(static_cast<GLFWwindow*>(m_window), this);

    glfwSetWindowCloseCallback(static_cast<GLFWwindow*>(m_window), [](GLFWwindow* window)
    {
        ::vx::app::window* vx_window = static_cast<::vx::app::window*>(glfwGetWindowUserPointer(window));

        vx_window->close();
    });
}

void window::close()
{
    glfwDestroyWindow(static_cast<GLFWwindow*>(m_window));
    m_window = nullptr;

    glfwTerminate();
}

void window::poll_events()
{
    glfwPollEvents();
}

}
}