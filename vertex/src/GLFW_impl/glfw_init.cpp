#include <GLFW/glfw3.h>

#include "vertex_impl/app/init_internal.h"
#include "vertex/system/debug/logger.h"

namespace vx {
namespace app {

static bool s_is_video_init = false;

static void glfwErrorCallback(int error, const char* description)
{
    VX_LOG_ERROR << "GLFW Error (" << error << "): " << description;
    throw_error(error_code::INTERNAL);
}

bool init_video()
{
    glfwSetErrorCallback(glfwErrorCallback);
    s_is_video_init = glfwInit();
    return s_is_video_init;
}

bool is_video_init()
{
    return s_is_video_init;
}

void terminate_video()
{
    glfwTerminate();
    s_is_video_init = false;
}

}
}