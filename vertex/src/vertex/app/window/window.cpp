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
    init();
}

window::~window()
{
    shutdown();
}

void window::init()
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
}

void window::shutdown()
{
    glfwDestroyWindow(static_cast<GLFWwindow*>(m_window));
    m_window = nullptr;

    glfwTerminate();
}

}
}