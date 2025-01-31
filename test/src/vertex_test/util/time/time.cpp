#include "vertex_test/test.hpp"

#include "vertex/util/time/time.hpp"
#include "vertex/util/time/datetime.hpp"
#include "vertex/os/time.hpp"

using namespace vx;

VX_TEST_CASE(time_point)
{
    VX_SECTION("conversions")
    {
        const time::time_point tp(time::seconds(1));

        VX_CHECK(tp.as_nanoseconds() == 1'000'000'000);
        VX_CHECK(tp.as_microseconds() == 1'000'000);
        VX_CHECK(tp.as_milliseconds() == 1'000);
        VX_CHECK(tp.as_seconds() == 1);

        VX_CHECK(tp.as_float_seconds() == 1.0);
        VX_CHECK(tp.as_float_milliseconds() == 1000.0);
    }

    VX_SECTION("arithmetic")
    {
        const time::time_point tp1(time::seconds(2));
        const time::time_point tp2(time::milliseconds(3000));

        VX_CHECK((tp1 + tp2).as_seconds() == 5);
        VX_CHECK((tp2 - tp1).as_seconds() == 1);
        VX_CHECK((tp1 * 2).as_seconds() == 4);
        VX_CHECK((tp1 / 2).as_seconds() == 1);
    }

    VX_SECTION("comparison")
    {
        const time::time_point tp1(time::seconds(2));
        const time::time_point tp2(time::seconds(3));

        VX_CHECK(tp1 < tp2);
        VX_CHECK(tp2 > tp1);
        VX_CHECK(tp1 <= tp2);
        VX_CHECK(tp2 >= tp1);

        const time::time_point tp3(tp1);
        VX_CHECK(tp1 == tp3);
        VX_CHECK(!(tp1 != tp3));
    }

    VX_SECTION("windows filetime conversion")
    {
        const uint32_t low = 0xD53E8000;
        const uint32_t high = 0x01D96000;
        const time::time_point tp = time::time_point::from_windows_file_time(low, high);

        uint32_t new_low, new_high;
        tp.to_windows_file_time(new_low, new_high);

        VX_CHECK(new_low == low);
        VX_CHECK(new_high == high);
    }
}

VX_TEST_CASE(datetime)
{
    VX_SECTION("leap year")
    {
        VX_CHECK(time::is_leap_year(2024));
        VX_CHECK(!time::is_leap_year(2023));
        VX_CHECK(time::is_leap_year(2000));
        VX_CHECK(!time::is_leap_year(1900));
    }

    VX_SECTION("days in month")
    {
        VX_CHECK(time::get_days_in_month(2023, time::month::JANUARY) == 31);
        VX_CHECK(time::get_days_in_month(2023, time::month::FEBRUARY) == 28);
        VX_CHECK(time::get_days_in_month(2024, time::month::FEBRUARY) == 29);
        VX_CHECK(time::get_days_in_month(2023, time::month::SEPTEMBER) == 30);
    }

    VX_SECTION("day of year")
    {
        VX_CHECK(time::get_day_of_year(2023, time::month::JANUARY, 1) == 0);
        VX_CHECK(time::get_day_of_year(2023, time::month::FEBRUARY, 28) == 58);
        VX_CHECK(time::get_day_of_year(2024, time::month::FEBRUARY, 29) == 59);
        VX_CHECK(time::get_day_of_year(2023, time::month::DECEMBER, 31) == 364);
    }

    VX_SECTION("day of week")
    {
        VX_CHECK(time::get_day_of_week(2023, time::month::MAY, 1) == time::weekday::MONDAY);
        VX_CHECK(time::get_day_of_week(2023, time::month::SEPTEMBER, 15) == time::weekday::FRIDAY);
    }

    VX_SECTION("valid datetime")
    {
        const time::datetime valid_date{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 12, 30, 45, 500000000, 0 };
        VX_CHECK(valid_date.is_valid());

        const time::datetime invalid_date{ 2023, time::month::FEBRUARY, 30, time::weekday::THURSDAY, 25, 61, 61, 1000000000, 0 };
        VX_CHECK(!invalid_date.is_valid()); // Invalid date (Feb 30th)

        const time::datetime invalid_time{ 2023, time::month::JANUARY, 1, time::weekday::SUNDAY, 25, 61, 61, 1000000000, 0 };
        VX_CHECK(!invalid_time.is_valid()); // Invalid hour, minute, second

        const time::datetime overflow_date{ 9999, time::month::DECEMBER, 31, time::weekday::SUNDAY, 23, 59, 59, 999999999, 0 };
        VX_CHECK(overflow_date.is_valid()); // Max valid date (no overflow)

        // Testing dates before the Unix epoch (negative timestamps)
        const time::datetime negative_date_1{ 1969, time::month::DECEMBER, 31, time::weekday::WEDNESDAY, 23, 59, 59, 999999999, 0 };
        VX_CHECK(negative_date_1.is_valid()); // Valid date just before the epoch

        const time::datetime negative_date_3{ 1970, time::month::JANUARY, 1, time::weekday::THURSDAY, 0, 0, 0, 0, 0 };
        VX_CHECK(negative_date_3.is_valid()); // Exactly the epoch start
    }

    VX_SECTION("datetime to utc")
    {
        // Test case with no UTC offset (should remain unchanged)
        const time::datetime date_utc{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 12, 30, 45, 123456789, 0 };
        VX_CHECK(date_utc.to_utc() == date_utc);

        // Test case with positive UTC offset (+05:30) -> Should convert to UTC
        const time::datetime date_positive{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 12, 30, 45, 123456789, 19800 };
        const time::datetime expected_positive{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 7, 0, 45, 123456789, 0 };
        VX_CHECK(date_positive.to_utc() == expected_positive);

        // Test case with negative UTC offset (-07:00) -> Should convert to UTC
        const time::datetime date_negative{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 12, 30, 45, 123456789, -25200 };
        const time::datetime expected_negative{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 19, 30, 45, 123456789, 0 };
        VX_CHECK(date_negative.to_utc() == expected_negative);

        // Test case with negative UTC offset (-00:30) -> Should convert to UTC
        const time::datetime date_small_negative{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 12, 30, 45, 123456789, -1800 };
        const time::datetime expected_small_negative{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 13, 0, 45, 123456789, 0 };
        VX_CHECK(date_small_negative.to_utc() == expected_small_negative);

        // Test case with large negative UTC offset (-12:45) -> Should convert to UTC
        const time::datetime date_large_negative{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 12, 30, 45, 123456789, -45900 };
        const time::datetime expected_large_negative{ 2023, time::month::MARCH, 16,  time::weekday::THURSDAY, 1, 15, 45, 123456789, 0 };
        VX_CHECK(date_large_negative.to_utc() == expected_large_negative);

        // Test case with large positive UTC offset (+14:00) -> Should convert to UTC
        const time::datetime date_large_positive{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 12, 30, 45, 123456789, 50400 };
        const time::datetime expected_large_positive{ 2023, time::month::MARCH, 14, time::weekday::TUESDAY, 22, 30, 45, 123456789, 0 };
        VX_CHECK(date_large_positive.to_utc() == expected_large_positive);

        // Edge case: Pre-epoch time with UTC offset
        const time::datetime pre_epoch{ 1969, time::month::DECEMBER, 31, time::weekday::WEDNESDAY, 23, 59, 59, 123456789, -3600 };
        const time::datetime expected_pre_epoch{ 1970, time::month::JANUARY, 1, time::weekday::THURSDAY, 0, 59, 59, 123456789, 0 };
        VX_CHECK(pre_epoch.to_utc() == expected_pre_epoch);

        // Edge case: Midnight conversion
        const time::datetime midnight{ 2024, time::month::JANUARY, 1, time::weekday::MONDAY, 0, 0, 0, 0, 3600 };
        const time::datetime expected_midnight{ 2023, time::month::DECEMBER, 31, time::weekday::SUNDAY, 23, 0, 0, 0, 0 };
        VX_CHECK(midnight.to_utc() == expected_midnight);
    }

    VX_SECTION("datetime to string")
    {
        // Test case with no UTC offset (should append 'Z')
        const time::datetime date_utc{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 12, 30, 45, 123456789, 0 };
        const char* utc_expected = "2023-03-15T12:30:45Z";
        VX_CHECK(date_utc.to_string() == utc_expected);

        // Test case with positive UTC offset (+05:30)
        const time::datetime date_positive{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 12, 30, 45, 123456789, 19800 };
        const char* expected_positive = "2023-03-15T12:30:45+05:30";
        VX_CHECK(date_positive.to_string() == expected_positive);

        // Test case with negative UTC offset (-07:00)
        const time::datetime date_negative{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 12, 30, 45, 123456789, -25200 };
        const char* expected_negative = "2023-03-15T12:30:45-07:00";
        VX_CHECK(date_negative.to_string() == expected_negative);

        // Test case with negative UTC offset (-00:30)
        const time::datetime date_small_negative{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 12, 30, 45, 123456789, -1800 };
        const char* expected_small_negative = "2023-03-15T12:30:45-00:30";
        VX_CHECK(date_small_negative.to_string() == expected_small_negative);

        // Test case with large negative UTC offset (-12:45)
        const time::datetime date_large_negative{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 12, 30, 45, 123456789, -45900 };
        const char* expected_large_negative = "2023-03-15T12:30:45-12:45";
        VX_CHECK(date_large_negative.to_string() == expected_large_negative);

        // Test case with large positive UTC offset (+14:00) - Max valid offset
        const time::datetime date_large_positive{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 12, 30, 45, 123456789, 50400 };
        const char* expected_large_positive = "2023-03-15T12:30:45+14:00";
        VX_CHECK(date_large_positive.to_string() == expected_large_positive);

        // Edge case: Pre-epoch time with UTC
        const time::datetime pre_epoch{ 1969, time::month::DECEMBER, 31, time::weekday::WEDNESDAY, 23, 59, 59, 123456789, 0 };
        const char* expected_pre_epoch = "1969-12-31T23:59:59Z";
        VX_CHECK(pre_epoch.to_string() == expected_pre_epoch);

        // Edge case: Midnight with UTC
        const time::datetime midnight_utc{ 2024, time::month::JANUARY, 1, time::weekday::MONDAY, 0, 0, 0, 0, 0 };
        const char* expected_midnight = "2024-01-01T00:00:00Z";
        VX_CHECK(midnight_utc.to_string() == expected_midnight);
    }
}

VX_TEST_CASE(conversions)
{
    // https://www.epochconverter.com/
    VX_SECTION("datetime to time_point")
    {
        // Epoch time: 1970-01-01T00:00:00 UTC
        const time::datetime epoch{ 1970, time::month::JANUARY, 1, time::weekday::THURSDAY };
        const time::time_point epoch_tp = epoch.to_time_point();
        VX_CHECK(epoch_tp.as_seconds() == 0);

        // Specific positive time: 2023-03-15T12:30:45.123456789 UTC+1
        const time::datetime date{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 12, 30, 45, 123456789, 3600 };
        const time::time_point date_tp = date.to_time_point();
        VX_CHECK(date_tp.as_seconds() == 1678879845);
        VX_CHECK(date_tp.as_nanoseconds() == 1678879845123456789);

        // Specific negative time: 1969-12-31T23:59:59 UTC
        const time::datetime pre_epoch{ 1969, time::month::DECEMBER, 31, time::weekday::WEDNESDAY, 23, 59, 59 };
        const time::time_point pre_epoch_tp = pre_epoch.to_time_point();
        VX_CHECK(pre_epoch_tp.as_seconds() == -1);
    }

    VX_SECTION("time_point to datetime")
    {
        // Epoch time: 1970-01-01T00:00:00 UTC
        const time::time_point epoch_tp{ 0 };
        const time::datetime epoch{ 1970, time::month::JANUARY, 1, time::weekday::THURSDAY };
        VX_CHECK(epoch_tp.to_datetime() == epoch);

        // Specific positive time: 2023-03-15T12:30:45.123456789 UTC+1
        const time::time_point date_tp{ time::nanoseconds(1678879845123456789) };
        const time::datetime date{ 2023, time::month::MARCH, 15, time::weekday::WEDNESDAY, 12, 30, 45, 123456789, 3600 };
        VX_CHECK(date_tp.to_datetime() == date);

        // Specific negative time: 1969-12-31T23:59:59 UTC
        const time::time_point pre_epoch_tp{ time::seconds(-1) };
        const time::datetime pre_epoch{ 1969, time::month::DECEMBER, 31, time::weekday::WEDNESDAY, 23, 59, 59 };
        VX_CHECK(pre_epoch_tp.to_datetime() == pre_epoch);
    }
}

int main()
{
    VX_RUN_TESTS();
    return 0;
}