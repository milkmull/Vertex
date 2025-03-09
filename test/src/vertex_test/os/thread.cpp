#include "vertex_test/test.hpp"
#include "vertex/os/thread.hpp"
#include "vertex/os/atomic.hpp"
#include "vertex/os/time.hpp"

using namespace vx;

static void simple_task(std::atomic<bool>& flag)
{
    os::sleep(time::milliseconds(5000));
    flag = true;
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_basic_process)
{
    os::thread t;
    std::atomic<bool> flag = false;

    // check status of default constructed thread
    VX_CHECK(!t.is_valid());
    VX_CHECK(!t.is_alive());
    VX_CHECK(!t.is_joinable());
    VX_CHECK(!t.join());
    VX_CHECK(t.get_id() == 0);

    VX_CHECK(t.start(simple_task, std::ref(flag)));
    VX_CHECK(t.is_valid());
    VX_CHECK(t.is_alive());
    VX_CHECK(t.is_joinable());
    VX_CHECK(t.get_id() != 0);

    VX_CHECK(t.join());
    VX_CHECK(!t.is_alive());
    VX_CHECK(flag);
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}