#include <GLFW/glfw3.h>

#include "vertex/app/input/joystick.h"

namespace vx {
namespace app {
namespace joystick {

bool is_present(int id)
{
    return glfwJoystickIsGamepad(id);
}

const char* get_name(int id)
{
    return glfwGetGamepadName(id);
}

state get_state(int id)
{
    GLFWgamepadstate glfw_state;
    glfwGetGamepadState(id, &glfw_state);
    return *(state*)(&glfw_state);
}

}
}
}