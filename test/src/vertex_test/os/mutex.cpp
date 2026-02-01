#include "vertex_test/test.hpp"
#include "vertex/os/mutex.hpp"
#include "vertex/os/thread.hpp"
#include "vertex/os/time.hpp"

using namespace vx;

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mutex_basic_lock_unlock)
{
    os::mutex mtx;

    VX_CHECK(mtx.lock());
    VX_CHECK(!mtx.try_lock());
    mtx.unlock();
    VX_CHECK(mtx.try_lock());
    mtx.unlock();
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mutex_deadlock)
{
    os::mutex mtx;
    
    VX_CHECK(mtx.lock());
    // would result in deadlock
    VX_CHECK_AND_EXPECT_ERROR(!mtx.lock());
    mtx.unlock();
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mutex_multiple_threads)
{
    constexpr size_t thread_count = 5;
    os::mutex mtx;
    int counter = 0;

    auto task = [&]()
    {
        for (int i = 0; i < 10; ++i)
        {
            mtx.lock();
            ++counter;
            mtx.unlock();
        }
    };

    os::thread threads[thread_count];
    for (size_t i = 0; i < thread_count; ++i)
    {
        VX_CHECK(threads[i].start(task));
    }

    for (size_t i = 0; i < thread_count; ++i)
    {
        VX_CHECK(threads[i].join());
    }

    // Each thread should have incremented the counter 10 times
    VX_CHECK(counter == thread_count * 10);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_recursive_mutex)
{
    os::recursive_mutex rec_mtx;

    VX_CHECK(rec_mtx.lock());
    VX_CHECK(rec_mtx.lock());
    VX_CHECK(rec_mtx.try_lock());
    rec_mtx.unlock();
    rec_mtx.unlock();
    rec_mtx.unlock();
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_mutex_error_handling)
{
    os::mutex mtx;
    os::thread thread;
    bool thread_flag = false;

    auto thread_func = [&]()
    {
        mtx.lock();
        os::sleep(time::milliseconds(100)); // Simulate some work
        thread_flag = true;
        mtx.unlock();
    };

    VX_CHECK(thread.start(thread_func));

    // Wait for thread to start
    os::sleep(time::milliseconds(10));

    // Try locking the mutex while it is held by another thread
    VX_CHECK(!mtx.try_lock());

    // Lock it now after the other thread has unlocked
    mtx.lock();                
    mtx.unlock();

    VX_CHECK(thread.join());

    // Ensure the thread executed and unlocked the mutex
    VX_CHECK(thread_flag);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_lock_guard)
{
    constexpr size_t thread_count = 5;
    os::mutex mtx;
    int counter = 0;

    auto thread_func = [&]()
    {
        os::lock_guard<os::mutex> guard(mtx);
        ++counter;
    };

    os::thread threads[thread_count];
    for (size_t i = 0; i < thread_count; ++i)
    {
        VX_CHECK(threads[i].start(thread_func));
    }

    for (size_t i = 0; i < thread_count; ++i)
    {
        VX_CHECK(threads[i].join());
    }

    // Each thread should have incremented the counter once
    VX_CHECK(counter == thread_count);
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}
