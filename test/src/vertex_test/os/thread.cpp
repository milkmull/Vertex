#include "vertex_test/test.hpp"
#include "vertex/os/thread.hpp"
#include "vertex/os/time.hpp"

#include <thread>
#include <atomic>

using namespace vx;

static void simple_task(std::atomic<bool>& flag)
{
    os::sleep(time::milliseconds(100));
    flag = true;
}

static void long_task(std::atomic<int>& counter)
{
    for (int i = 0; i < 10; ++i)
    {
        os::sleep(time::milliseconds(50));
        ++counter;
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_basic_thread)
{
    os::thread t;
    std::atomic<bool> flag = false;

    // Check status of default constructed thread
    VX_CHECK(!t.is_valid());
    VX_CHECK(!t.is_joinable());
    VX_CHECK(!t.join());
    VX_CHECK(t.get_id() == os::thread::id{});

    // Start a simple task
    VX_CHECK(t.start(simple_task, std::ref(flag)));
    VX_CHECK(t.is_valid());
    VX_CHECK(t.is_joinable());
    VX_CHECK(t.get_id() != os::thread::id{});

    // Wait for the thread to complete
    VX_CHECK(t.join());
    VX_CHECK(!t.is_valid());
    VX_CHECK(!t.is_joinable());
    VX_CHECK(flag);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_thread_move_operations)
{
    os::thread t1;
    std::atomic<bool> flag = false;

    VX_CHECK(t1.start(simple_task, std::ref(flag)));
    VX_CHECK(t1.is_valid());

    const os::thread::id id = t1.get_id();
    VX_CHECK(id != os::thread::id{});

    VX_DISABLE_MSVC_WARNING_PUSH();
    VX_DISABLE_MSVC_WARNING(26800); // disable use after move warning

    // Move construction
    os::thread t2(std::move(t1));

    VX_CHECK(!t1.is_valid()); // Original thread should be invalidated
    VX_CHECK(t1.get_id() == os::thread::id{});

    VX_CHECK(t2.is_valid());
    VX_CHECK(t2.get_id() == id);

    // Move assignment
    os::thread t3;
    t3 = std::move(t2);

    VX_CHECK(!t2.is_valid()); // Moved-from thread should be invalidated
    VX_CHECK(t2.get_id() == os::thread::id{});

    VX_CHECK(t3.is_valid());
    VX_CHECK(t3.get_id() == id);

    VX_DISABLE_MSVC_WARNING_POP();

    // Ensure thread completion
    VX_CHECK(t3.join());
    VX_CHECK(flag);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_multiple_threads)
{
    constexpr size_t count = 5;
    os::thread threads[count]{};
    std::atomic<int> counters[count]{};

    for (size_t i = 0; i < count; ++i)
    {
        counters[i] = 0;
        VX_CHECK(threads[i].start(long_task, std::ref(counters[i])));
    }

    for (size_t i = 0; i < count; ++i)
    {
        VX_CHECK(threads[i].join());
        VX_CHECK(counters[i] == 10);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_error_handling)
{
    os::thread t;

    // Starting a thread twice without joining/detaching should fail
    std::atomic<bool> flag = false;
    VX_CHECK(t.start(simple_task, std::ref(flag)));
    VX_CHECK_AND_EXPECT_ERROR(!t.start(simple_task, std::ref(flag)));  // Should return false

    // Join the thread to clean up
    VX_CHECK(t.join());
    VX_CHECK(flag);

    // Attempt to join a thread that is already joined
    VX_CHECK(!t.join());
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_this_thread_id)
{
    VX_CHECK(os::this_thread::get_id() != os::thread::id{});
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_thread_guard_basic)
{
    std::atomic<bool> flag = false;
    os::thread t;
    VX_CHECK(t.start(simple_task, std::ref(flag)));

    {
        os::thread_guard guard(std::move(t));
        VX_CHECK(guard.get_thread().is_valid());
    } // Thread should be joined automatically here

    VX_CHECK(flag); // Verify the task completed
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_thread_guard_release)
{
    std::atomic<bool> flag = false;
    os::thread t;
    VX_CHECK(t.start(simple_task, std::ref(flag)));

    os::thread_guard guard(std::move(t));
    os::thread released_thread = guard.release();

    // Check that the thread was successfully released
    VX_CHECK(released_thread.is_valid());
    VX_CHECK(released_thread.is_joinable());

    // Join the released thread manually
    VX_CHECK(released_thread.join());
    VX_CHECK(flag);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_thread_guard_move_operations)
{
    std::atomic<bool> flag = false;
    os::thread t;
    VX_CHECK(t.start(simple_task, std::ref(flag)));

    os::thread_guard guard1(std::move(t));
    VX_CHECK(guard1.get_thread().is_valid());

    VX_DISABLE_MSVC_WARNING_PUSH();
    VX_DISABLE_MSVC_WARNING(26800); // disable use after move warning

    // Move construction
    os::thread_guard guard2(std::move(guard1));
    VX_CHECK(!guard1.get_thread().is_valid()); // Original guard is now empty
    VX_CHECK(guard2.get_thread().is_valid());

    // Move assignment
    os::thread_guard guard3 = std::move(guard2);
    VX_CHECK(!guard2.get_thread().is_valid()); // Moved-from guard is now empty
    VX_CHECK(guard3.get_thread().is_valid());

    VX_DISABLE_MSVC_WARNING_POP();

    // Ensure thread completion via destructor
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_thread_guard_with_long_task)
{
    std::atomic<int> counter = 0;
    os::thread t;
    VX_CHECK(t.start(long_task, std::ref(counter)));

    {
        os::thread_guard guard(std::move(t));
        VX_CHECK(guard.get_thread().is_valid());
    } // Thread should be joined automatically here

    VX_CHECK(counter == 10); // Verify the long task completed correctly
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_thread_local_storage)
{
    thread_local int thread_local_var = 0;

    constexpr size_t count = 5;
    os::thread threads[count]{};
    std::atomic<int> results[count]{};

    auto thread_func = [](std::atomic<int>& result)
    {
        // Each thread increments its own thread-local variable
        for (int i = 0; i < 10; ++i)
        {
            ++thread_local_var;
            os::sleep(time::milliseconds(10));
        }
        result = thread_local_var; // Store the final value
    };

    for (size_t i = 0; i < count; ++i)
    {
        results[i] = 0;
        VX_CHECK(threads[i].start(thread_func, std::ref(results[i])));
    }

    for (size_t i = 0; i < count; ++i)
    {
        VX_CHECK(threads[i].join());
        VX_CHECK(results[i] == 10); // Each thread should have its own independent thread_local_var
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_thread_deadlock)
{
    //os::thread thread;
    //std::atomic<bool> result = false;
    //
    //auto thread_func = [&]()
    //{
    //    // make sure that joining a thread from inside itsself causes a system error
    //    result = (!thread.join() && err::get().err == err::SYSTEM_ERROR);
    //};
    //
    //VX_CHECK(thread.start(thread_func));
    //VX_CHECK(thread.join());
    //VX_CHECK(result.load());
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}