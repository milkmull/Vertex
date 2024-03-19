#include "sandbox/sandbox.h"

#include "vertex/tools/debug/logger.h"
#include "vertex/tools/debug/profiler.h"
#include "vertex/math/math/util/to_string.h"

#include "vertex/math/math/transform3d.h"

int main()
{
    using namespace vx;

    profiler::begin_session("invert", "../../assets/results.txt");

    math::mat4 m = math::make_trs_3d({ 1, 2, 3 }, { 1, 1, 1 }, 0.5f, { 0.5f, 1.0f, 2 });

    for (int i = 0; i < 100000; ++i)
    {
        {
            VX_PROFILE_SCOPE("invert1");

            for (int j = 0; j < 100; ++j)
            {
                math::inverse(m);
            }
        }
    }

    return 0;
}