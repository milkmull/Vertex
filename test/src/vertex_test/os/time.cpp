#include "vertex_test/test.hpp"

#include "vertex/std/time/datetime.hpp"
#include "vertex/std/time/time.hpp"
#include "vertex/std/time/timer.hpp"

#include <cstring>
#include <limits>

using namespace vx;

//=============================================================================
// helpers
//=============================================================================

namespace {

// datetime's month field is a plain int32_t and its constructor has no weekday,
// so this keeps the tests readable by letting us pass the month enum.
constexpr time::datetime make_dt(
    int32_t year,
    time::month mo,
    int32_t day,
    int32_t hour = 0,
    int32_t minute = 0,
    int32_t second = 0,
    int32_t nanosecond = 0,
    int32_t utc_offset = 0) noexcept
{
    return time::datetime(year, static_cast<int32_t>(mo), day, hour, minute, second, nanosecond, utc_offset);
}

// datetime::operator== compares the *instant* (via to_time_point), so it can't
// tell whether a conversion actually changed the fields. Use this when the
// individual fields matter (e.g. to_utc()).
constexpr bool same_fields(const time::datetime& a, const time::datetime& b) noexcept
{
    return a.year == b.year &&
        a.month == b.month &&
        a.day == b.day &&
        a.hour == b.hour &&
        a.minute == b.minute &&
        a.second == b.second &&
        a.nanosecond == b.nanosecond &&
        a.utc_offset_seconds == b.utc_offset_seconds;
}

bool str_equals(const char* buf, size_t len, const char* expected) noexcept
{
    return len == std::strlen(expected) && std::memcmp(buf, expected, len) == 0;
}

// Formats dt and compares against the expected ISO 8601 string
bool formats_as(const time::datetime& dt, const char* expected) noexcept
{
    char buf[time::datetime::iso8601_max_size];
    const auto result = dt.to_string(buf);
    return result.has_value() && str_equals(buf, result.value(), expected);
}

} // namespace

//=============================================================================
// duration
//=============================================================================

VX_TEST_CASE(duration)
{
    VX_SECTION("construction")
    {
        constexpr time::duration d_default{};
        VX_STATIC_CHECK(d_default.as_nanoseconds() == 0);
        VX_STATIC_CHECK(d_default.is_zero());

        constexpr time::duration d(1234);
        VX_STATIC_CHECK(d.as_nanoseconds() == 1234);

        VX_STATIC_CHECK(time::zero().is_zero());
    }

    VX_SECTION("factory functions")
    {
        VX_STATIC_CHECK(time::nanoseconds(5).as_nanoseconds() == 5);
        VX_STATIC_CHECK(time::microseconds(5).as_nanoseconds() == 5'000);
        VX_STATIC_CHECK(time::milliseconds(5).as_nanoseconds() == 5'000'000);
        VX_STATIC_CHECK(time::seconds(5).as_nanoseconds() == 5'000'000'000);
        VX_STATIC_CHECK(time::minutes(5).as_nanoseconds() == 300'000'000'000);
        VX_STATIC_CHECK(time::hours(5).as_nanoseconds() == 18'000'000'000'000);
    }

    VX_SECTION("integer conversions")
    {
        constexpr time::duration d = time::seconds(1);

        VX_STATIC_CHECK(d.as_nanoseconds() == 1'000'000'000);
        VX_STATIC_CHECK(d.as_microseconds() == 1'000'000);
        VX_STATIC_CHECK(d.as_milliseconds() == 1'000);
        VX_STATIC_CHECK(d.as_seconds() == 1);
        VX_STATIC_CHECK(d.as_minutes() == 0);
        VX_STATIC_CHECK(d.as_hours() == 0);

        VX_STATIC_CHECK(time::minutes(90).as_hours() == 1);
        VX_STATIC_CHECK(time::hours(2).as_minutes() == 120);
        VX_STATIC_CHECK(time::hours(2).as_seconds() == 7200);
    }

    VX_SECTION("integer conversions truncate toward zero")
    {
        VX_STATIC_CHECK(time::milliseconds(1500).as_seconds() == 1);
        VX_STATIC_CHECK(time::milliseconds(-1500).as_seconds() == -1);
        VX_STATIC_CHECK(time::nanoseconds(999).as_microseconds() == 0);
        VX_STATIC_CHECK(time::nanoseconds(-999).as_microseconds() == 0);
    }

    VX_SECTION("floating point conversions")
    {
        constexpr time::duration d = time::milliseconds(1500);

        VX_STATIC_CHECK(d.as_float_nanoseconds() == 1'500'000'000.0);
        VX_STATIC_CHECK(d.as_float_microseconds() == 1'500'000.0);
        VX_STATIC_CHECK(d.as_float_milliseconds() == 1500.0);
        VX_STATIC_CHECK(d.as_float_seconds() == 1.5);

        VX_STATIC_CHECK(time::seconds(90).as_float_minutes() == 1.5);
        VX_STATIC_CHECK(time::minutes(90).as_float_hours() == 1.5);
        VX_STATIC_CHECK(time::milliseconds(-500).as_float_seconds() == -0.5);
    }

    VX_SECTION("sign queries")
    {
        VX_STATIC_CHECK(time::seconds(1).is_positive());
        VX_STATIC_CHECK(!time::seconds(1).is_negative());
        VX_STATIC_CHECK(time::seconds(-1).is_negative());
        VX_STATIC_CHECK(!time::seconds(-1).is_positive());
        VX_STATIC_CHECK(!time::zero().is_positive());
        VX_STATIC_CHECK(!time::zero().is_negative());
    }

    VX_SECTION("zero / bool")
    {
        VX_STATIC_CHECK(!time::seconds(1).is_zero());
        VX_STATIC_CHECK(static_cast<bool>(time::seconds(1)));
        VX_STATIC_CHECK(!static_cast<bool>(time::zero()));

        // zero() mutates, so this one stays a runtime check
        time::duration d = time::seconds(1);
        d.zero();
        VX_CHECK(d.is_zero());
        VX_CHECK(!static_cast<bool>(d));
        VX_CHECK(d == time::zero());
    }

    VX_SECTION("comparison")
    {
        constexpr time::duration a = time::seconds(2);
        constexpr time::duration b = time::seconds(3);
        constexpr time::duration c = time::milliseconds(2000);

        VX_STATIC_CHECK(a < b);
        VX_STATIC_CHECK(b > a);
        VX_STATIC_CHECK(a <= b);
        VX_STATIC_CHECK(b >= a);
        VX_STATIC_CHECK(a <= c);
        VX_STATIC_CHECK(a >= c);

        VX_STATIC_CHECK(a == c);
        VX_STATIC_CHECK(!(a != c));
        VX_STATIC_CHECK(a != b);
        VX_STATIC_CHECK(!(a == b));
    }

    VX_SECTION("unary operators")
    {
        constexpr time::duration d = time::seconds(2);

        VX_STATIC_CHECK(+d == d);
        VX_STATIC_CHECK(-d == time::seconds(-2));
        VX_STATIC_CHECK(-(-d) == d);
        VX_STATIC_CHECK(-time::zero() == time::zero());
    }

    VX_SECTION("addition / subtraction")
    {
        constexpr time::duration a = time::seconds(2);
        constexpr time::duration b = time::milliseconds(500);

        VX_STATIC_CHECK(a + b == time::milliseconds(2500));
        VX_STATIC_CHECK(a - b == time::milliseconds(1500));
        VX_STATIC_CHECK(b - a == time::milliseconds(-1500));
    }

    VX_SECTION("scalar multiplication")
    {
        constexpr time::duration d = time::seconds(2);

        VX_STATIC_CHECK(d * 2 == time::seconds(4));
        VX_STATIC_CHECK(2 * d == time::seconds(4));
        VX_STATIC_CHECK(d * -1 == time::seconds(-2));
        VX_STATIC_CHECK(d * 0 == time::zero());

        // floating point scalars are computed in floating point, then truncated
        VX_STATIC_CHECK(d * 0.25 == time::milliseconds(500));
        VX_STATIC_CHECK(0.25 * d == time::milliseconds(500));
        //VX_STATIC_CHECK(d * 1.5f == time::seconds(3));
        VX_STATIC_CHECK(time::nanoseconds(3) * 0.5 == time::nanoseconds(1)); // 1.5 truncates to 1
    }

    VX_SECTION("scalar division")
    {
        constexpr time::duration d = time::seconds(5);

        VX_STATIC_CHECK(d / 2 == time::milliseconds(2500));
        //VX_STATIC_CHECK(d / 2.0 == time::milliseconds(2500));
        //VX_STATIC_CHECK(d / 0.5 == time::seconds(10));
        VX_STATIC_CHECK(time::nanoseconds(7) / 2 == time::nanoseconds(3)); // truncates
    }

    VX_SECTION("duration / duration and modulo")
    {
        // the ratio of two durations is a plain integer, not a duration
        constexpr int64_t ratio = time::seconds(5) / time::seconds(2);
        VX_STATIC_CHECK(ratio == 2);
        VX_STATIC_CHECK(time::minutes(1) / time::seconds(1) == 60);
        VX_STATIC_CHECK(time::seconds(1) / time::minutes(1) == 0);

        VX_STATIC_CHECK(time::seconds(5) % time::seconds(2) == time::seconds(1));
        VX_STATIC_CHECK(time::milliseconds(2500) % time::seconds(1) == time::milliseconds(500));
        VX_STATIC_CHECK(time::seconds(4) % time::seconds(2) == time::zero());
    }

    // The remaining mutating operations aren't evaluated at compile time here
    VX_SECTION("compound assignment")
    {
        time::duration d = time::seconds(1);

        d += time::seconds(2);
        VX_CHECK(d == time::seconds(3));

        d -= time::seconds(1);
        VX_CHECK(d == time::seconds(2));

        d *= 3;
        VX_CHECK(d == time::seconds(6));

        d /= 4;
        VX_CHECK(d == time::milliseconds(1500));

        d *= 2.0;
        VX_CHECK(d == time::seconds(3));

        d /= 0.5;
        VX_CHECK(d == time::seconds(6));

        d %= time::seconds(4);
        VX_CHECK(d == time::seconds(2));
    }

    VX_SECTION("swap")
    {
        time::duration a = time::seconds(1);
        time::duration b = time::seconds(2);

        a.swap(b);
        VX_CHECK(a == time::seconds(2));
        VX_CHECK(b == time::seconds(1));

        swap(a, b);
        VX_CHECK(a == time::seconds(1));
        VX_CHECK(b == time::seconds(2));
    }

    VX_SECTION("min / max")
    {
        VX_STATIC_CHECK(time::max().as_nanoseconds() == INT64_MAX);
        VX_STATIC_CHECK(time::min().as_nanoseconds() == INT64_MIN);
        VX_STATIC_CHECK(time::min() < time::zero());
        VX_STATIC_CHECK(time::max() > time::zero());
    }

    VX_SECTION("unit constants")
    {
        VX_STATIC_CHECK(time::milliseconds_per_second == 1'000);
        VX_STATIC_CHECK(time::microseconds_per_second == 1'000'000);
        VX_STATIC_CHECK(time::nanoseconds_per_second == 1'000'000'000);
        VX_STATIC_CHECK(time::nanoseconds_per_millisecond == 1'000'000);
        VX_STATIC_CHECK(time::seconds_per_minute == 60);
        VX_STATIC_CHECK(time::seconds_per_hour == 3'600);
        VX_STATIC_CHECK(time::seconds_per_day == 86'400);
    }

    VX_SECTION("hash")
    {
        const hash<time::duration> h;

        VX_CHECK(h(time::seconds(1)) == h(time::milliseconds(1000)));
        VX_CHECK(h(time::seconds(1)) != h(time::seconds(2)));
    }

    VX_SECTION("numeric_limits")
    {
        using limits = std::numeric_limits<time::duration>;

        VX_STATIC_CHECK(limits::is_specialized);
        VX_STATIC_CHECK(limits::min() == time::min());
        VX_STATIC_CHECK(limits::max() == time::max());
        VX_STATIC_CHECK(limits::lowest() == time::min());
    }
}

//=============================================================================
// time_point
//=============================================================================

VX_TEST_CASE(time_point)
{
    VX_SECTION("construction")
    {
        constexpr time::time_point epoch{};
        VX_STATIC_CHECK(epoch.time_since_epoch().is_zero());

        constexpr time::time_point tp(time::seconds(1));
        VX_STATIC_CHECK(tp.time_since_epoch() == time::seconds(1));
    }

    VX_SECTION("time since epoch conversions")
    {
        constexpr time::time_point tp(time::seconds(1));
        constexpr time::duration since_epoch = tp.time_since_epoch();

        VX_STATIC_CHECK(since_epoch.as_nanoseconds() == 1'000'000'000);
        VX_STATIC_CHECK(since_epoch.as_microseconds() == 1'000'000);
        VX_STATIC_CHECK(since_epoch.as_milliseconds() == 1'000);
        VX_STATIC_CHECK(since_epoch.as_seconds() == 1);

        VX_STATIC_CHECK(since_epoch.as_float_seconds() == 1.0);
        VX_STATIC_CHECK(since_epoch.as_float_milliseconds() == 1000.0);
    }

    VX_SECTION("arithmetic")
    {
        constexpr time::time_point tp1(time::seconds(2));
        constexpr time::time_point tp2(time::milliseconds(3000));
        constexpr time::duration d = time::seconds(1);

        // time_point +/- duration -> time_point
        VX_STATIC_CHECK((tp1 + d).time_since_epoch() == time::seconds(3));
        VX_STATIC_CHECK((d + tp1).time_since_epoch() == time::seconds(3));
        VX_STATIC_CHECK((tp1 - d).time_since_epoch() == time::seconds(1));

        // time_point - time_point -> duration
        constexpr time::duration diff = tp2 - tp1;
        VX_STATIC_CHECK(diff == time::seconds(1));
        VX_STATIC_CHECK((tp1 - tp2) == time::seconds(-1));
        VX_STATIC_CHECK((tp1 - tp1).is_zero());

        // pre-epoch results are fine too
        VX_STATIC_CHECK((tp1 - time::seconds(3)).time_since_epoch() == time::seconds(-1));
    }

    VX_SECTION("compound assignment")
    {
        time::time_point tp(time::seconds(2));

        tp += time::seconds(3);
        VX_CHECK(tp.time_since_epoch() == time::seconds(5));

        tp -= time::seconds(1);
        VX_CHECK(tp.time_since_epoch() == time::seconds(4));
    }

    VX_SECTION("comparison")
    {
        constexpr time::time_point tp1(time::seconds(2));
        constexpr time::time_point tp2(time::seconds(3));

        VX_STATIC_CHECK(tp1 < tp2);
        VX_STATIC_CHECK(tp2 > tp1);
        VX_STATIC_CHECK(tp1 <= tp2);
        VX_STATIC_CHECK(tp2 >= tp1);
        VX_STATIC_CHECK(tp1 != tp2);

        constexpr time::time_point tp3(tp1);
        VX_STATIC_CHECK(tp1 == tp3);
        VX_STATIC_CHECK(!(tp1 != tp3));
        VX_STATIC_CHECK(tp1 <= tp3);
        VX_STATIC_CHECK(tp1 >= tp3);

        // same instant expressed in different units
        VX_STATIC_CHECK(time::time_point(time::seconds(2)) == time::time_point(time::milliseconds(2000)));
    }

    VX_SECTION("swap")
    {
        time::time_point a(time::seconds(1));
        time::time_point b(time::seconds(2));

        a.swap(b);
        VX_CHECK(a.time_since_epoch() == time::seconds(2));
        VX_CHECK(b.time_since_epoch() == time::seconds(1));

        swap(a, b);
        VX_CHECK(a.time_since_epoch() == time::seconds(1));
        VX_CHECK(b.time_since_epoch() == time::seconds(2));
    }

    VX_SECTION("hash")
    {
        const hash<time::time_point> h;

        VX_CHECK(h(time::time_point(time::seconds(1))) == h(time::time_point(time::milliseconds(1000))));
        VX_CHECK(h(time::time_point(time::seconds(1))) != h(time::time_point(time::seconds(2))));
    }

    VX_SECTION("numeric_limits")
    {
        using limits = std::numeric_limits<time::time_point>;

        VX_STATIC_CHECK(limits::is_specialized);
        VX_STATIC_CHECK(limits::min().time_since_epoch() == time::min());
        VX_STATIC_CHECK(limits::max().time_since_epoch() == time::max());
        VX_STATIC_CHECK(limits::lowest() == limits::min());
        VX_STATIC_CHECK(limits::min() < limits::max());
    }
}

//=============================================================================
// date helpers
//=============================================================================

VX_TEST_CASE(date_helpers)
{
    VX_SECTION("leap year")
    {
        VX_STATIC_CHECK(time::is_leap_year(2024));
        VX_STATIC_CHECK(!time::is_leap_year(2023));
        VX_STATIC_CHECK(time::is_leap_year(2000));
        VX_STATIC_CHECK(!time::is_leap_year(1900));
    }

    VX_SECTION("days in year")
    {
        VX_STATIC_CHECK(time::get_days_in_year(2024) == 366);
        VX_STATIC_CHECK(time::get_days_in_year(2023) == 365);
        VX_STATIC_CHECK(time::get_days_in_year(2000) == 366);
        VX_STATIC_CHECK(time::get_days_in_year(1900) == 365);
    }

    VX_SECTION("days in month")
    {
        VX_STATIC_CHECK(time::get_days_in_month(2023, time::month::january) == 31);
        VX_STATIC_CHECK(time::get_days_in_month(2023, time::month::february) == 28);
        VX_STATIC_CHECK(time::get_days_in_month(2024, time::month::february) == 29);
        VX_STATIC_CHECK(time::get_days_in_month(1900, time::month::february) == 28);
        VX_STATIC_CHECK(time::get_days_in_month(2000, time::month::february) == 29);
        VX_STATIC_CHECK(time::get_days_in_month(2023, time::month::april) == 30);
        VX_STATIC_CHECK(time::get_days_in_month(2023, time::month::september) == 30);
        VX_STATIC_CHECK(time::get_days_in_month(2023, time::month::december) == 31);
    }

    // NOTE: the implementation is zero-based (Jan 1 == 0) even though the doc
    // comment on get_day_of_year says [1-366].
    VX_SECTION("day of year")
    {
        VX_STATIC_CHECK(time::get_day_of_year(2023, time::month::january, 1) == 0);
        VX_STATIC_CHECK(time::get_day_of_year(2023, time::month::february, 28) == 58);
        VX_STATIC_CHECK(time::get_day_of_year(2023, time::month::march, 1) == 59);
        VX_STATIC_CHECK(time::get_day_of_year(2024, time::month::february, 29) == 59);
        VX_STATIC_CHECK(time::get_day_of_year(2024, time::month::march, 1) == 60);
        VX_STATIC_CHECK(time::get_day_of_year(2023, time::month::december, 31) == 364);
        VX_STATIC_CHECK(time::get_day_of_year(2024, time::month::december, 31) == 365);
    }

    VX_SECTION("day of week")
    {
        VX_STATIC_CHECK(time::get_day_of_week(2023, time::month::may, 1) == time::weekday::monday);
        VX_STATIC_CHECK(time::get_day_of_week(2023, time::month::september, 15) == time::weekday::friday);
        VX_STATIC_CHECK(time::get_day_of_week(1970, time::month::january, 1) == time::weekday::thursday);
        VX_STATIC_CHECK(time::get_day_of_week(2000, time::month::january, 1) == time::weekday::saturday);
        VX_STATIC_CHECK(time::get_day_of_week(2024, time::month::february, 29) == time::weekday::thursday);

        // before the epoch
        VX_STATIC_CHECK(time::get_day_of_week(1969, time::month::december, 31) == time::weekday::wednesday);
        VX_STATIC_CHECK(time::get_day_of_week(1969, time::month::december, 28) == time::weekday::sunday);
        VX_STATIC_CHECK(time::get_day_of_week(1900, time::month::january, 1) == time::weekday::monday);
    }

    // strcmp isn't constexpr, so these stay runtime checks
    VX_SECTION("month / weekday to string")
    {
        VX_CHECK(std::strcmp(time::weekday_to_string(time::weekday::sunday), "Sunday") == 0);
        VX_CHECK(std::strcmp(time::weekday_to_string(time::weekday::saturday), "Saturday") == 0);
        VX_CHECK(std::strcmp(time::month_to_string(time::month::january), "January") == 0);
        VX_CHECK(std::strcmp(time::month_to_string(time::month::december), "December") == 0);
    }

    VX_SECTION("civil <-> days round trip")
    {
        namespace priv = time::_datetime_priv;

        VX_STATIC_CHECK(priv::civil_to_days(1970, 1, 1) == 0);
        VX_STATIC_CHECK(priv::civil_to_days(1969, 12, 31) == -1);
        VX_STATIC_CHECK(priv::civil_to_days(2000, 3, 1) == 11017);

        // large loop: kept at runtime to avoid constexpr step limits
        bool ok = true;
        for (int32_t days = -800000; days <= 800000; days += 37)
        {
            const priv::civil c = priv::civil_from_days(days);
            ok = ok && (priv::civil_to_days(c.year, static_cast<int32_t>(c.month), static_cast<int32_t>(c.day)) == days);
        }
        VX_CHECK(ok);
    }
}

//=============================================================================
// datetime
//=============================================================================

VX_TEST_CASE(datetime)
{
    VX_SECTION("construction")
    {
        // only y/m/d are required, everything else defaults to 0
        constexpr time::datetime date(2023, 3, 15);
        VX_STATIC_CHECK(date.year == 2023);
        VX_STATIC_CHECK(date.month == 3);
        VX_STATIC_CHECK(date.day == 15);
        VX_STATIC_CHECK(date.hour == 0);
        VX_STATIC_CHECK(date.minute == 0);
        VX_STATIC_CHECK(date.second == 0);
        VX_STATIC_CHECK(date.nanosecond == 0);
        VX_STATIC_CHECK(date.utc_offset_seconds == 0);

        constexpr time::datetime full(2023, 3, 15, 12, 30, 45, 123456789, 3600);
        VX_STATIC_CHECK(full.hour == 12);
        VX_STATIC_CHECK(full.minute == 30);
        VX_STATIC_CHECK(full.second == 45);
        VX_STATIC_CHECK(full.nanosecond == 123456789);
        VX_STATIC_CHECK(full.utc_offset_seconds == 3600);

        // implicit conversion from the os fields struct
        constexpr os::datetime_fields fields = full;
        constexpr time::datetime from_fields = fields;
        VX_STATIC_CHECK(same_fields(from_fields, full));
    }

    VX_SECTION("get_month / get_weekday")
    {
        VX_STATIC_CHECK(make_dt(2023, time::month::march, 15).get_month() == time::month::march);
        VX_STATIC_CHECK(make_dt(2023, time::month::december, 1).get_month() == time::month::december);

        // weekday is derived from the date, it is no longer stored
        VX_STATIC_CHECK(make_dt(2023, time::month::march, 15).get_weekday() == time::weekday::wednesday);
        VX_STATIC_CHECK(make_dt(1970, time::month::january, 1).get_weekday() == time::weekday::thursday);
        VX_STATIC_CHECK(make_dt(1969, time::month::december, 31).get_weekday() == time::weekday::wednesday);
        VX_STATIC_CHECK(make_dt(2023, time::month::december, 31).get_weekday() == time::weekday::sunday);
    }

    VX_SECTION("valid datetime")
    {
        constexpr time::datetime valid_date = make_dt(2023, time::month::march, 15, 12, 30, 45, 500000000, 0);
        VX_STATIC_CHECK(valid_date.is_valid());
        VX_STATIC_CHECK(valid_date.find_invalid_field() == time::datetime::field::none);

        constexpr time::datetime invalid_date = make_dt(2023, time::month::february, 30, 12, 0, 0, 0, 0);
        VX_STATIC_CHECK(!invalid_date.is_valid()); // Feb 30th

        constexpr time::datetime invalid_time = make_dt(2023, time::month::january, 1, 25, 61, 61, 1000000000, 0);
        VX_STATIC_CHECK(!invalid_time.is_valid()); // hour, minute, second, nanosecond

        constexpr time::datetime max_date = make_dt(9999, time::month::december, 31, 23, 59, 59, 999999999, 0);
        VX_STATIC_CHECK(max_date.is_valid());

        // dates before the Unix epoch
        constexpr time::datetime pre_epoch = make_dt(1969, time::month::december, 31, 23, 59, 59, 999999999, 0);
        VX_STATIC_CHECK(pre_epoch.is_valid());

        constexpr time::datetime epoch = make_dt(1970, time::month::january, 1);
        VX_STATIC_CHECK(epoch.is_valid());

        // leap days
        VX_STATIC_CHECK(make_dt(2024, time::month::february, 29).is_valid());
        VX_STATIC_CHECK(!make_dt(2023, time::month::february, 29).is_valid());
        VX_STATIC_CHECK(!make_dt(1900, time::month::february, 29).is_valid());
        VX_STATIC_CHECK(make_dt(2000, time::month::february, 29).is_valid());
    }

    VX_SECTION("find_invalid_field")
    {
        using field = time::datetime::field;

        VX_STATIC_CHECK(time::datetime(-1, 1, 1).find_invalid_field() == field::year);
        VX_STATIC_CHECK(time::datetime(10000, 1, 1).find_invalid_field() == field::year);
        VX_STATIC_CHECK(time::datetime(0, 1, 1).find_invalid_field() == field::none);

        VX_STATIC_CHECK(time::datetime(2023, 0, 1).find_invalid_field() == field::month);
        VX_STATIC_CHECK(time::datetime(2023, 13, 1).find_invalid_field() == field::month);

        VX_STATIC_CHECK(time::datetime(2023, 1, 0).find_invalid_field() == field::day);
        VX_STATIC_CHECK(time::datetime(2023, 1, 32).find_invalid_field() == field::day);
        VX_STATIC_CHECK(time::datetime(2023, 4, 31).find_invalid_field() == field::day);

        VX_STATIC_CHECK(time::datetime(2023, 1, 1, -1).find_invalid_field() == field::hour);
        VX_STATIC_CHECK(time::datetime(2023, 1, 1, 24).find_invalid_field() == field::hour);

        VX_STATIC_CHECK(time::datetime(2023, 1, 1, 0, -1).find_invalid_field() == field::minute);
        VX_STATIC_CHECK(time::datetime(2023, 1, 1, 0, 60).find_invalid_field() == field::minute);

        VX_STATIC_CHECK(time::datetime(2023, 1, 1, 0, 0, -1).find_invalid_field() == field::second);
        VX_STATIC_CHECK(time::datetime(2023, 1, 1, 0, 0, 60).find_invalid_field() == field::second); // no leap seconds

        VX_STATIC_CHECK(time::datetime(2023, 1, 1, 0, 0, 0, -1).find_invalid_field() == field::nanosecond);
        VX_STATIC_CHECK(time::datetime(2023, 1, 1, 0, 0, 0, 1000000000).find_invalid_field() == field::nanosecond);

        VX_STATIC_CHECK(time::datetime(2023, 1, 1, 0, 0, 0, 0, -43201).find_invalid_field() == field::utc_offset);
        VX_STATIC_CHECK(time::datetime(2023, 1, 1, 0, 0, 0, 0, 50401).find_invalid_field() == field::utc_offset);
        VX_STATIC_CHECK(time::datetime(2023, 1, 1, 0, 0, 0, 0, -43200).find_invalid_field() == field::none); // -12:00
        VX_STATIC_CHECK(time::datetime(2023, 1, 1, 0, 0, 0, 0, 50400).find_invalid_field() == field::none);  // +14:00

        // the first invalid field in declaration order is reported
        VX_STATIC_CHECK(time::datetime(2023, 13, 40, 25).find_invalid_field() == field::month);
    }

    VX_SECTION("datetime to utc")
    {
        // No UTC offset (should remain unchanged)
        {
            constexpr time::datetime dt = make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, 0);
            VX_STATIC_CHECK(same_fields(dt.to_utc(), dt));
        }

        // Positive UTC offset (+05:30)
        {
            constexpr time::datetime dt = make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, 19800);
            constexpr time::datetime expected = make_dt(2023, time::month::march, 15, 7, 0, 45, 123456789, 0);
            VX_STATIC_CHECK(same_fields(dt.to_utc(), expected));
            VX_STATIC_CHECK(dt.to_utc().get_weekday() == time::weekday::wednesday);
        }

        // Negative UTC offset (-07:00)
        {
            constexpr time::datetime dt = make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, -25200);
            constexpr time::datetime expected = make_dt(2023, time::month::march, 15, 19, 30, 45, 123456789, 0);
            VX_STATIC_CHECK(same_fields(dt.to_utc(), expected));
        }

        // Small negative UTC offset (-00:30)
        {
            constexpr time::datetime dt = make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, -1800);
            constexpr time::datetime expected = make_dt(2023, time::month::march, 15, 13, 0, 45, 123456789, 0);
            VX_STATIC_CHECK(same_fields(dt.to_utc(), expected));
        }

        // Large negative UTC offset (-12:45) crosses into the next day
        {
            constexpr time::datetime dt = make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, -45900);
            constexpr time::datetime expected = make_dt(2023, time::month::march, 16, 1, 15, 45, 123456789, 0);
            VX_STATIC_CHECK(same_fields(dt.to_utc(), expected));
            VX_STATIC_CHECK(dt.to_utc().get_weekday() == time::weekday::thursday);
        }

        // Large positive UTC offset (+14:00) crosses into the previous day
        {
            constexpr time::datetime dt = make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, 50400);
            constexpr time::datetime expected = make_dt(2023, time::month::march, 14, 22, 30, 45, 123456789, 0);
            VX_STATIC_CHECK(same_fields(dt.to_utc(), expected));
            VX_STATIC_CHECK(dt.to_utc().get_weekday() == time::weekday::tuesday);
        }

        // Pre-epoch time with UTC offset crosses into the epoch
        {
            constexpr time::datetime dt = make_dt(1969, time::month::december, 31, 23, 59, 59, 123456789, -3600);
            constexpr time::datetime expected = make_dt(1970, time::month::january, 1, 0, 59, 59, 123456789, 0);
            VX_STATIC_CHECK(same_fields(dt.to_utc(), expected));
        }

        // Midnight conversion crosses into the previous year
        {
            constexpr time::datetime dt = make_dt(2024, time::month::january, 1, 0, 0, 0, 0, 3600);
            constexpr time::datetime expected = make_dt(2023, time::month::december, 31, 23, 0, 0, 0, 0);
            VX_STATIC_CHECK(same_fields(dt.to_utc(), expected));
            VX_STATIC_CHECK(dt.to_utc().get_weekday() == time::weekday::sunday);
        }

        // Leap day boundary
        {
            constexpr time::datetime dt = make_dt(2024, time::month::march, 1, 0, 30, 0, 0, 3600);
            constexpr time::datetime expected = make_dt(2024, time::month::february, 29, 23, 30, 0, 0, 0);
            VX_STATIC_CHECK(same_fields(dt.to_utc(), expected));
        }

        // the result always has a zero offset and represents the same instant
        {
            constexpr time::datetime dt = make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, 19800);
            VX_STATIC_CHECK(dt.to_utc().utc_offset_seconds == 0);
            VX_STATIC_CHECK(dt.to_utc().to_time_point() == dt.to_time_point());
        }
    }

    // to_string returns an expected<>, which may not be usable in constant
    // expressions, so these stay runtime checks.
    VX_SECTION("datetime to string (fixed buffer)")
    {
        // No UTC offset (appends 'Z')
        VX_CHECK(formats_as(make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, 0), "2023-03-15T12:30:45Z"));

        // Positive UTC offset (+05:30)
        VX_CHECK(formats_as(make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, 19800), "2023-03-15T12:30:45+05:30"));

        // Negative UTC offset (-07:00)
        VX_CHECK(formats_as(make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, -25200), "2023-03-15T12:30:45-07:00"));

        // Small negative UTC offset (-00:30) keeps the sign
        VX_CHECK(formats_as(make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, -1800), "2023-03-15T12:30:45-00:30"));

        // Largest negative UTC offset (-12:00) and the one before it (-12:45)
        VX_CHECK(formats_as(make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, -43200), "2023-03-15T12:30:45-12:00"));
        VX_CHECK(formats_as(make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, -45900), "2023-03-15T12:30:45-12:45"));

        // Max valid positive UTC offset (+14:00)
        VX_CHECK(formats_as(make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, 50400), "2023-03-15T12:30:45+14:00"));

        // Pre-epoch time with UTC
        VX_CHECK(formats_as(make_dt(1969, time::month::december, 31, 23, 59, 59, 123456789, 0), "1969-12-31T23:59:59Z"));

        // Midnight with UTC
        VX_CHECK(formats_as(make_dt(2024, time::month::january, 1, 0, 0, 0, 0, 0), "2024-01-01T00:00:00Z"));

        // Zero padding of small years and fields
        VX_CHECK(formats_as(make_dt(5, time::month::february, 3, 4, 5, 6, 0, 0), "0005-02-03T04:05:06Z"));

        // Extremes
        VX_CHECK(formats_as(make_dt(9999, time::month::december, 31, 23, 59, 59, 999999999, 0), "9999-12-31T23:59:59Z"));
        VX_CHECK(formats_as(make_dt(0, time::month::january, 1, 0, 0, 0, 0, 0), "0000-01-01T00:00:00Z"));
    }

    VX_SECTION("datetime to string (buffer handling)")
    {
        const time::datetime utc = make_dt(2023, time::month::march, 15, 12, 30, 45, 0, 0);
        const time::datetime offset = make_dt(2023, time::month::march, 15, 12, 30, 45, 0, 19800);

        VX_STATIC_CHECK(time::datetime::iso8601_utc_size == 20);
        VX_STATIC_CHECK(time::datetime::iso8601_max_size == 25);

        // exact-size buffers succeed and report the written length
        {
            char buf[20];
            const auto result = utc.to_string(buf);
            VX_CHECK(result.has_value());
            VX_CHECK(result.value() == 20);
            VX_CHECK(str_equals(buf, 20, "2023-03-15T12:30:45Z"));
        }
        {
            char buf[25];
            const auto result = offset.to_string(buf);
            VX_CHECK(result.has_value());
            VX_CHECK(result.value() == 25);
            VX_CHECK(str_equals(buf, 25, "2023-03-15T12:30:45+05:30"));
        }

        // too-small buffers fail and leave the buffer untouched
        {
            char buf[19];
            std::memset(buf, '#', sizeof(buf));

            const auto result = utc.to_string(buf);
            VX_CHECK(!result.has_value());
            VX_CHECK(result.error() == err::size_error);

            bool untouched = true;
            for (const char c : buf)
            {
                untouched = untouched && (c == '#');
            }
            VX_CHECK(untouched);
        }
        {
            // a UTC-sized buffer is too small for a non-UTC string
            char buf[24];
            std::memset(buf, '#', sizeof(buf));

            const auto result = offset.to_string(buf);
            VX_CHECK(!result.has_value());
            VX_CHECK(result.error() == err::size_error);

            bool untouched = true;
            for (const char c : buf)
            {
                untouched = untouched && (c == '#');
            }
            VX_CHECK(untouched);
        }

        // pointer + size overload: larger buffers are fine, and nothing is written
        // past the end of the string (no null terminator)
        {
            char buf[32];
            std::memset(buf, '#', sizeof(buf));

            const auto result = utc.to_string(buf, sizeof(buf));
            VX_CHECK(result.has_value());
            VX_CHECK(result.value() == 20);
            VX_CHECK(buf[20] == '#');
        }

        // the size check uses out_size, not the real array size
        {
            char buf[32];
            VX_CHECK(!utc.to_string(buf, 19).has_value());
            VX_CHECK(utc.to_string(buf, 20).has_value());
            VX_CHECK(!offset.to_string(buf, 24).has_value());
            VX_CHECK(offset.to_string(buf, 25).has_value());
        }
    }

    VX_SECTION("equality compares instants")
    {
        // same instant, different representation
        constexpr time::datetime utc = make_dt(2023, time::month::march, 15, 7, 0, 45, 123456789, 0);
        constexpr time::datetime plus_530 = make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, 19800);
        VX_STATIC_CHECK(utc == plus_530);
        VX_STATIC_CHECK(!(utc != plus_530));
        VX_STATIC_CHECK(!same_fields(utc, plus_530));

        // different instants
        constexpr time::datetime other = make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, 0);
        VX_STATIC_CHECK(utc != other);
        VX_STATIC_CHECK(!(utc == other));

        // nanoseconds matter
        constexpr time::datetime ns_a = make_dt(2023, time::month::march, 15, 12, 30, 45, 1, 0);
        constexpr time::datetime ns_b = make_dt(2023, time::month::march, 15, 12, 30, 45, 2, 0);
        VX_STATIC_CHECK(ns_a != ns_b);
    }
}

//=============================================================================
// conversions
//=============================================================================

VX_TEST_CASE(conversions)
{
    // https://www.epochconverter.com/
    VX_SECTION("datetime to time_point")
    {
        // Epoch time: 1970-01-01T00:00:00 UTC
        constexpr time::datetime epoch = make_dt(1970, time::month::january, 1);
        VX_STATIC_CHECK(epoch.to_time_point().time_since_epoch().as_seconds() == 0);
        VX_STATIC_CHECK(epoch.to_time_point() == time::time_point{});

        // Specific positive time: 2023-03-15T12:30:45.123456789 UTC+1
        constexpr time::datetime date = make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, 3600);
        constexpr time::duration date_since_epoch = date.to_time_point().time_since_epoch();
        VX_STATIC_CHECK(date_since_epoch.as_seconds() == 1678879845);
        VX_STATIC_CHECK(date_since_epoch.as_nanoseconds() == 1678879845123456789);

        // The same instant expressed in UTC
        constexpr time::datetime date_utc = make_dt(2023, time::month::march, 15, 11, 30, 45, 123456789, 0);
        VX_STATIC_CHECK(date_utc.to_time_point() == date.to_time_point());

        // Specific negative time: 1969-12-31T23:59:59 UTC
        constexpr time::datetime pre_epoch = make_dt(1969, time::month::december, 31, 23, 59, 59);
        VX_STATIC_CHECK(pre_epoch.to_time_point().time_since_epoch().as_seconds() == -1);

        // Pre-epoch with a sub-second part: -1s + 0.5s = -0.5s
        constexpr time::datetime pre_epoch_ns = make_dt(1969, time::month::december, 31, 23, 59, 59, 500000000);
        VX_STATIC_CHECK(pre_epoch_ns.to_time_point().time_since_epoch().as_nanoseconds() == -500000000);

        // Leap day
        constexpr time::datetime leap_day = make_dt(2024, time::month::february, 29);
        VX_STATIC_CHECK(leap_day.to_time_point().time_since_epoch().as_seconds() == 1709164800);

        // Y2K
        constexpr time::datetime y2k = make_dt(2000, time::month::january, 1);
        VX_STATIC_CHECK(y2k.to_time_point().time_since_epoch().as_seconds() == 946684800);

        // Max valid date
        constexpr time::datetime max_date = make_dt(9999, time::month::december, 31, 23, 59, 59);
        //VX_STATIC_CHECK(max_date.to_time_point().time_since_epoch().as_seconds() == 253402300799);

        // Offsets at the limits of the valid range
        constexpr time::datetime plus_14 = make_dt(1970, time::month::january, 1, 14, 0, 0, 0, 50400);
        VX_STATIC_CHECK(plus_14.to_time_point() == time::time_point{});

        constexpr time::datetime minus_12 = make_dt(1969, time::month::december, 31, 12, 0, 0, 0, -43200);
        VX_STATIC_CHECK(minus_12.to_time_point() == time::time_point{});
    }

    VX_SECTION("datetime to_utc round trips through time_point")
    {
        const time::datetime inputs[] = {
            make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, 19800),
            make_dt(2023, time::month::march, 15, 12, 30, 45, 123456789, -45900),
            make_dt(1969, time::month::december, 31, 23, 59, 59, 123456789, -3600),
            make_dt(2024, time::month::january, 1, 0, 0, 0, 0, 3600),
            make_dt(2024, time::month::february, 29, 23, 59, 59, 999999999, -43200),
        };

        bool ok = true;
        for (const time::datetime& dt : inputs)
        {
            const time::datetime utc = dt.to_utc();
            ok = ok && utc.is_valid();
            ok = ok && (utc.utc_offset_seconds == 0);
            ok = ok && (utc.to_time_point() == dt.to_time_point());
            ok = ok && (utc.nanosecond == dt.nanosecond);
        }
        VX_CHECK(ok);
    }

    // TODO: time_point::to_datetime() goes through the os layer, so it isn't
    // covered here yet.
}

int main()
{
    VX_RUN_TESTS();
    return 0;
}
