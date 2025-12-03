#include "vertex_test/test.hpp"
#include "vertex/os/time.hpp"

using namespace vx;

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_system_time)
{
    time::time_point t1, t2;

    VX_EXPECT_NO_ERROR(t1 = os::system_time());
    VX_EXPECT_NO_ERROR(os::sleep(time::milliseconds(10)));
    VX_EXPECT_NO_ERROR(t2 = os::system_time());
    VX_CHECK(t2 > t1);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_get_ticks)
{
    time::time_point t1, t2;

    VX_EXPECT_NO_ERROR(t1 = os::get_ticks());
    VX_CHECK(!t1.is_zero());
    VX_EXPECT_NO_ERROR(os::sleep(time::milliseconds(10)));
    VX_EXPECT_NO_ERROR(t2 = os::get_ticks());
    VX_CHECK(!t2.is_zero());
    VX_CHECK(t2 > t1);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_get_performance_counter)
{
    int64_t t1, t2;

    VX_EXPECT_NO_ERROR(t1 = os::get_performance_counter());
    VX_CHECK(t1 != 0);
    VX_EXPECT_NO_ERROR(os::sleep(time::milliseconds(10)));
    VX_EXPECT_NO_ERROR(t2 = os::get_performance_counter());
    VX_CHECK(t2 != 0);
    VX_CHECK(t2 > t1);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_get_performance_frequency)
{
    VX_CHECK_AND_EXPECT_NO_ERROR(os::get_performance_frequency() > 0);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_sleep)
{
    constexpr int64_t sleep_time_ms = 10;
    constexpr time::time_point sleep_ms = time::milliseconds(sleep_time_ms);
    time::time_point t1, t2;

    VX_EXPECT_NO_ERROR(t1 = os::get_ticks());
    VX_CHECK(!t1.is_zero());
    VX_EXPECT_NO_ERROR(os::sleep(sleep_ms));
    VX_EXPECT_NO_ERROR(t2 = os::get_ticks());
    VX_CHECK(!t2.is_zero());

    const auto dif = t2.as_float_milliseconds() - t1.as_float_milliseconds();
    // sometimes sleep can take a little longer, we check if we are within 30 ms
    VX_CHECK(std::abs(dif - sleep_ms.as_float_milliseconds()) < 30.0);

    VX_MESSAGE("  Expected sleep time: ", sleep_time_ms, " ms");
    VX_MESSAGE("  Actual sleep time: ", dif, " ms");
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_sleep_precise)
{
    constexpr int64_t sleep_time_ms = 10;
    constexpr time::time_point sleep_ms = time::milliseconds(sleep_time_ms);
    time::time_point t1, t2;

    // Measure time before precise sleep
    VX_EXPECT_NO_ERROR(t1 = os::get_ticks());
    VX_CHECK(!t1.is_zero());

    // Perform precise sleep
    VX_EXPECT_NO_ERROR(os::sleep_precise(sleep_ms));

    // Measure time after precise sleep
    VX_EXPECT_NO_ERROR(t2 = os::get_ticks());
    VX_CHECK(!t2.is_zero());

    const auto dif = t2.as_float_milliseconds() - t1.as_float_milliseconds();

    // sleep_precise should closely hit the target with smaller variance.
    // A 10 ms target should normally land very near 10 ms, but allow some margin
    // for scheduler behavior.
    VX_CHECK(std::abs(dif - sleep_ms.as_float_milliseconds()) < 5.0);

    VX_MESSAGE("  Expected precise sleep time: ", sleep_time_ms, " ms");
    VX_MESSAGE("  Actual precise sleep time: ", dif, " ms");
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}