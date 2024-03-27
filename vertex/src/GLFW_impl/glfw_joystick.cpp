#include <GLFW/glfw3.h>

#include "vertex/app/input/joystick.h"
#include "vertex_impl/app/init_internal.h"

namespace vx {
namespace app {
namespace joystick {

bool is_present(joystick id)
{
    VX_REQUIRE_INIT_OR_RETURN(false);
    return glfwJoystickIsGamepad(id);
}

const char* get_name(joystick id)
{
    VX_REQUIRE_INIT_OR_RETURN(nullptr);
    return glfwGetGamepadName(id);
}

state get_state(joystick id)
{
    VX_REQUIRE_INIT_OR_RETURN(state{});

    GLFWgamepadstate glfw_state;
    glfwGetGamepadState(id, &glfw_state);

    state vx_state{};
    std::copy(std::begin(glfw_state.buttons), std::end(glfw_state.buttons), std::begin(vx_state.buttons));
    std::copy(std::begin(glfw_state.axes), std::end(glfw_state.axes), std::begin(vx_state.axes));

    return vx_state;
}

}
}
}