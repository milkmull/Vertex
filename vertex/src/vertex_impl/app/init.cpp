#include "vertex_impl/app/init_internal.h"

namespace vx {
namespace app {

static bool s_is_init = false;

bool init()
{
    if (!init_video())
    {
        terminate();
        return false;
    }

    s_is_init = true;
    return true;
}

bool is_init()
{
    return s_is_init;
}

void terminate()
{
    terminate_video();

    s_is_init = false;
}

error_fn s_error_callback = nullptr;

void set_error_callback(error_fn callback)
{
    s_error_callback = callback;
}

void throw_error(error_code code)
{
    if (s_error_callback)
    {
        s_error_callback(code);
    }
}

}
}