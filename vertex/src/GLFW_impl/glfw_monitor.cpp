#include <GLFW/glfw3.h>

#include "vertex/app/display/monitor.h"
#include "vertex_impl/app/init_internal.h"

namespace vx {
namespace app {
namespace monitor {

// https://www.glfw.org/docs/3.3/monitor_guide.html#monitor_object

monitor get_primary_monitor()
{
    return (monitor)(glfwGetPrimaryMonitor());
}

std::vector<monitor> list_monitors()
{
    int count = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&count);
    return std::vector<monitor>(count, (monitor**)monitors);
}

const char* get_monitor_name(monitor monitor)
{
    return glfwGetMonitorName((GLFWmonitor*)monitor);
}

math::vec2i get_monitor_position(monitor monitor)
{
    math::vec2i pos;
    glfwGetMonitorPos((GLFWmonitor*)monitor, &pos.x, &pos.y);
    return pos;
}

math::vec2i get_monitor_physical_size(monitor monitor)
{
    math::vec2i size;
    glfwGetMonitorPhysicalSize((GLFWmonitor*)monitor, &size.x, &size.y);
    return size;
}

}
}
}