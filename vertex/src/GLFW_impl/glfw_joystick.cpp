#include <GLFW/glfw3.h>

#include "vertex/app/input/joystick.h"
#include "vertex_impl/app/init_internal.h"

namespace vx {
namespace app {
namespace joystick {

bool is_present(int id)
{
    VX_REQUIRE_INIT_OR_RETURN(false);
    return glfwJoystickIsGamepad(id);
}

const char* get_name(int id)
{
    VX_REQUIRE_INIT_OR_RETURN(nullptr);
    return glfwGetGamepadName(id);
}

state get_state(int id)
{
    VX_REQUIRE_INIT_OR_RETURN(state{});
    GLFWgamepadstate glfw_state;
    glfwGetGamepadState(id, &glfw_state);
    return *(state*)(&glfw_state);
}

}
}
}