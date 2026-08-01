#include <cassert>
#include <cstring>

#include "vertex/std/format.hpp"
#include "vertex/std/io.hpp"

using namespace vx;

template <size_t InN, size_t FmtN, typename T>
static fmt::format_result run_scan(const char (&in)[InN], const char (&fmt)[FmtN], T& out)
{
    return fmt::scan(
        in, mem::array_size(in),
        fmt, mem::array_size(fmt),
        out);
}

static void test_no_align_no_width()
{
    const char in[] = "   42";
    const char fmt[] = "{}";
    int value = 0;

    fmt::format_result r = run_scan(in, fmt, value);
    assert(r.err == fmt::format_error::none);
    assert(value == 42);
}

static void test_width_only_default_align_ok()
{
    const char in[] = "    42";
    const char fmt[] = "{:6}";
    int value = 0;

    fmt::format_result r = run_scan(in, fmt, value);
    assert(r.err == fmt::format_error::none);
    assert(value == 42);
    assert(r.count == 7); // full field consumed
}

static void test_width_only_default_align_error()
{
    const char in[] = "42";
    const char fmt[] = "{:6}";
    int value = 0;

    fmt::format_result r = run_scan(in, fmt, value);
    assert(r.err != fmt::format_error::none); // width=6 requires exact field, no fill present
}

static void test_align_right_no_width()
{
    const char in[] = "***42";
    const char fmt[] = "{:*>}";
    int value = 0;

    fmt::format_result r = run_scan(in, fmt, value);
    assert(r.err == fmt::format_error::none);
    assert(value == 42);
    assert(r.count == 6);
}

static void test_align_left_no_width()
{
    const char in[] = "42***";
    const char fmt[] = "{:*<}";
    int value = 0;

    fmt::format_result r = run_scan(in, fmt, value);
    assert(r.err == fmt::format_error::none);
    assert(value == 42);
    assert(r.count == 6);
}

static void test_align_center_no_width_ok()
{
    const char in[] = "*42*";
    const char fmt[] = "{:*^}";
    int value = 0;

    fmt::format_result r = run_scan(in, fmt, value);
    assert(r.err == fmt::format_error::none);
    assert(value == 42);
    assert(r.count == 5);
}

static void test_align_center_no_width_error()
{
    const char in[] = "**42*";
    const char fmt[] = "{:*^}";
    int value = 0;

    fmt::format_result r = run_scan(in, fmt, value);
    assert(r.err != fmt::format_error::none); // pre=2, post=1, mismatch
}

static void test_align_right_width_ok()
{
    const char in[] = "***42";
    const char fmt[] = "{:*>5}";
    int value = 0;

    fmt::format_result r = run_scan(in, fmt, value);
    assert(r.err == fmt::format_error::none);
    assert(value == 42);
    assert(r.count == 6);
}

static void test_align_right_width_truncates_value()
{
    const char in[] = "***42";
    const char fmt[] = "{:*>4}";
    int value = 0;

    fmt::format_result r = run_scan(in, fmt, value);
    //assert(r.err == fmt::format_error::none);
    assert(value == 4);   // gotcha: width chops into the value itself
    assert(r.count == 4); // "2" left unconsumed
}

static void test_align_center_width_ok()
{
    const char in[] = "*42**";
    const char fmt[] = "{:*^5}";
    int value = 0;

    fmt::format_result r = run_scan(in, fmt, value);
    assert(r.err == fmt::format_error::none);
    assert(value == 42);
    assert(r.count == 6);
}

static void test_align_center_width_error()
{
    const char in[] = "**42*";
    const char fmt[] = "{:*^5}";
    int value = 0;

    fmt::format_result r = run_scan(in, fmt, value);
    assert(r.err != fmt::format_error::none); // n=3 wants pre=1/post=2, got pre=2/post=1
}

static void test_align_left_width_ok()
{
    const char in[] = "42***";
    const char fmt[] = "{:*<5}";
    int value = 0;

    fmt::format_result r = run_scan(in, fmt, value);
    assert(r.err == fmt::format_error::none);
    assert(value == 42);
    assert(r.count == 6);
}

static void test_align_left_width_error()
{
    const char in[] = "42";
    const char fmt[] = "{:*<5}";
    int value = 0;

    fmt::format_result r = run_scan(in, fmt, value);
    assert(r.err != fmt::format_error::none); // width=5 requires 3 trailing '*', none present
}

static void test_width_equals_value_len_left()
{
    const char in[] = "42";
    const char fmt[] = "{:*<2}";
    int value = 0;

    fmt::format_result r = run_scan(in, fmt, value);
    assert(r.err == fmt::format_error::none);
    assert(value == 42);
    assert(r.count == 3);
}

static void test_width_equals_value_len_center()
{
    const char in[] = "42";
    const char fmt[] = "{:*^2}";
    int value = 0;

    fmt::format_result r = run_scan(in, fmt, value);
    assert(r.err == fmt::format_error::none);
    assert(value == 42);
    assert(r.count == 3);
}

int main()
{
    test_no_align_no_width();
    test_width_only_default_align_ok();
    test_width_only_default_align_error();
    test_align_right_no_width();
    test_align_left_no_width();
    test_align_center_no_width_ok();
    test_align_center_no_width_error();
    test_align_right_width_ok();
    test_align_right_width_truncates_value();
    test_align_center_width_ok();
    test_align_center_width_error();
    test_align_left_width_ok();
    test_align_left_width_error();
    test_width_equals_value_len_left();
    test_width_equals_value_len_center();

    io::print("all scan tests passed");

    return 0;
}
