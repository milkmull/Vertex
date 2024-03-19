#include "sandbox/sandbox.h"

#define VX_ENABLE_LOGGING
#define VX_ENABLE_PROFILING

#include "vertex/tools/debug/logger.h"
#include "vertex/tools/debug/profiler.h"
#include "vertex/math/math/util/to_string.h"

#include "vertex/math/math/transform3d.h"

void profile_test()
{
    using namespace vx::math;

    mat4 m;
    m = make_rotation_3d(radians(30), radians(-175), radians(330));

    {
        VX_PROFILE_SCOPE("invert");
        m = invert(m);
    }
}

int main()
{
    using namespace vx;

    logger::start_file_output("../../assets/log.txt");

    

    return 0;
}