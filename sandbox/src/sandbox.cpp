#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/system/time.hpp"
#include "vertex/os/process.hpp"
#include "vertex/os/thread.hpp"
#include "vertex/stdlib/system/time.hpp"

using namespace vx;

static bool thread_test(int x)
{
    time::sleep(5000);
    VX_LOG_INFO << "thread: " << x;
    return true;
}

int main(int argc, char* argv[])
{
    os::thread threads[10]{};

    for (int i = 0; i < 10; ++i)
    {
        threads[i].start(thread_test, i);
    }

    for (int i = 0; i < 10; ++i)
    {
        threads[i].join();
    }

    return 0;
}