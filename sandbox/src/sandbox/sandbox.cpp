#include "sandbox/sandbox.h"

#include "vertex/tools/debug/log.h"
#include "vertex/math/math/util/to_string.h"

using namespace vx::math;

int main()
{
    using namespace vx;

    logger::start_file_output("../../assets/log.txt");

    VX_LOG_DEBUG << "hello world" << 1 << 2;

    VX_LOG_CRITICAL << math::vec2(1, 2);

    return 0;
}