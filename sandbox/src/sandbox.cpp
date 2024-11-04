#include "sandbox/sandbox.hpp"
#include "vertex/os/time.hpp"
#include "vertex/util/system/timer.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    using tp = time::time_point;

    timer t;
    t.start();

    os::sleep(time::seconds(5));

    t.stop();

    VX_LOG_INFO << t.elapsed().as_float_seconds();

    return 0;
}