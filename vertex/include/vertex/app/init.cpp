#include "init.h"

#include "detail/init_video.h"

namespace vx {
namespace app {

bool s_is_init = false;

bool init()
{
    s_is_init = true;
}

bool is_init()
{
    return s_is_init;
}

void quit()
{
    s_is_init = false;
}

}
}