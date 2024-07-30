#include "vertex/app/app.h"
#include "vertex/app/video/video.h"

namespace vx {
namespace app {

bool init(subsystem::type systems)
{
    if (systems & subsystem::VIDEO)
    {
        return video::init();
    }

    return true;
}

}
}