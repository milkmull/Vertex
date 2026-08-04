#include "vertex/std/array.hpp"
#include "vertex/std/format.hpp"
#include "vertex/std/io.hpp"
#include "vertex/std/string.hpp"
#include "vertex/std/string_view.hpp"
#include "vertex_test/test.hpp"

#define LIT(x) VX_LIT(C, x)

using namespace vx;

//==============================================================================
// Test harness
//==============================================================================

template <typename C, typename... Args>
struct scan_test_case
{
    const C* in;
    const C* fmt;
    fmt::scan_error err;
    size_t count; // only checked when err == none
    std::tuple<Args...> expected;
};

template <typename C, typename Tuple, size_t... I>
fmt::scan_result call_scan(
    const C* in,
    size_t in_size,
    const C* fmt,
    size_t fmt_size,
    Tuple& values,
    std::index_sequence<I...>)
{
    return fmt::scan(in, in_size, fmt, fmt_size, std::get<I>(values)...);
}

template <typename Tuple, size_t... I>
bool tuple_values_equal(const Tuple& a, const Tuple& b, std::index_sequence<I...>)
{
    bool ok = true;
    ((ok = ok && (std::get<I>(a) == std::get<I>(b))), ...);
    return ok;
}

template <typename C, typename... Args>
void run_scan_test(
    const C* in,
    const C* fmt,
    fmt::scan_error expected_err,
    size_t expected_count,
    std::tuple<Args...> expected_values)
{
    constexpr size_t N = sizeof...(Args);
    const size_t in_size = str::length(in);
    const size_t fmt_size = str::length(fmt);

    std::tuple<Args...> values{}; // value-initialized, per proposal semantics

    VX_IF_CONSTEXPR (sizeof(C) == sizeof(char))
    {
        println("scan testing ", fmt, " <- ", in);
    }

    auto result = call_scan<C>(
        in, in_size, fmt, fmt_size,
        values, std::make_index_sequence<N>{});

    VX_CHECK(result.err == expected_err);

    if (expected_err == fmt::scan_error::none)
    {
        VX_CHECK(result.count == expected_count);
        VX_CHECK(tuple_values_equal(values, expected_values, std::make_index_sequence<N>{}));
    }
}

template <typename C, typename... Args, size_t N>
void run_test_batch(const scan_test_case<C, Args...> (&cases)[N])
{
    for (const auto& tc : cases)
    {
        run_scan_test<C>(tc.in, tc.fmt, tc.err, tc.count, tc.expected);
    }
}

//==============================================================================

template <typename C>
void test_common_cases()
{
    //VX_SECTION("empty")
    //{
    //    constexpr scan_test_case<C, int> empty_casess[] = {
    //        { LIT(""),   LIT(""),      fmt::scan_error::none,         0, {} },
    //        { LIT("42"), LIT(""),      fmt::scan_error::none,         0, {} },
    //        { LIT(""),   LIT("42"),    fmt::scan_error::end_of_input, 0, {} },
    //        { LIT(""),   LIT(" \n\t"), fmt::scan_error::end_of_input, 0, {} },
    //    };
    //
    //    run_test_batch(empty_casess);
    //}
    //
    //VX_SECTION("escaped braces")
    //{
    //    constexpr scan_test_case<C, int> escape_cases[] = {
    //        { LIT("{"),    LIT("{{"),     fmt::scan_error::none, 1, {}     },
    //        { LIT("}"),    LIT("}}"),     fmt::scan_error::none, 1, {}     },
    //        { LIT("{}"),   LIT("{{}}"),   fmt::scan_error::none, 2, {}     },
    //
    //        { LIT("{42}"), LIT("{{{}}}"), fmt::scan_error::none, 4, { 42 } },
    //        { LIT("{42"),  LIT("{{{}"),   fmt::scan_error::none, 3, { 42 } },
    //        { LIT("42}"),  LIT("{}}}"),   fmt::scan_error::none, 3, { 42 } },
    //    };
    //
    //    run_test_batch(escape_cases);
    //}
    //
    //VX_SECTION("bad format")
    //{
    //    constexpr scan_test_case<C, int> bad_format_cases[] = {
    //        { LIT("{"),  LIT("{{{"),                              fmt::scan_error::invalid_format, 1, {}     },
    //        { LIT("}"),  LIT("}}}"),                              fmt::scan_error::invalid_format, 1, {}     },
    //
    //        { LIT("42"), LIT("{0"),                               fmt::scan_error::invalid_format, 0, {}     },
    //        { LIT("42"), LIT("{0:"),                              fmt::scan_error::invalid_format, 0, {}     },
    //        { LIT("42"), LIT("{:"),                               fmt::scan_error::invalid_format, 0, {}     },
    //
    //        { LIT("42"), LIT("{0}}"),                             fmt::scan_error::invalid_format, 2, { 42 } },
    //        { LIT("42"), LIT("{}}"),                              fmt::scan_error::invalid_format, 2, { 42 } },
    //
    //        { LIT("42"), LIT("{-1}"),                             fmt::scan_error::invalid_format, 0, {}     },
    //        { LIT("42"), LIT("{+1}"),                             fmt::scan_error::invalid_format, 0, {}     },
    //        { LIT("42"), LIT("{a}"),                              fmt::scan_error::invalid_format, 0, {}     },
    //        { LIT("42"), LIT("{1a}"),                             fmt::scan_error::invalid_format, 0, {}     },
    //
    //        { LIT("42"), LIT("{::}"),                             fmt::scan_error::invalid_format, 0, {}     },
    //        { LIT("42"), LIT("{0::}"),                            fmt::scan_error::invalid_format, 0, {}     },
    //        { LIT("42"), LIT("{0:x:}"),                           fmt::scan_error::invalid_format, 0, {}     },
    //        { LIT("42"), LIT("{0:x}}"),                           fmt::scan_error::invalid_format, 0, {}     },
    //
    //        { LIT("42"), LIT("{999999999999999999999999999999}"), fmt::scan_error::invalid_format, 0, {}     },
    //    };
    //
    //    run_test_batch(bad_format_cases);
    //}
    //
    //VX_SECTION("argument index errors")
    //{
    //    constexpr scan_test_case<C, int> arg_index_cases[] = {
    //        { LIT("42"), LIT("{} {}"), fmt::scan_error::end_of_input,     2, { 42 } },
    //        { LIT("42"), LIT("{1}"),   fmt::scan_error::invalid_argument, 0, {}     },
    //        { LIT("42"), LIT("{42}"),  fmt::scan_error::invalid_argument, 0, {}     },
    //    };
    //
    //    run_test_batch(arg_index_cases);
    //}
    //
    //VX_SECTION("mismatched auto/manual indexing")
    //{
    //    constexpr scan_test_case<C, int> mixed_indexing_cases[] = {
    //        { LIT("42 43"), LIT("{} {0}"), fmt::scan_error::index_mode_mismatch, 3, { 42 } },
    //        { LIT("42 43"), LIT("{0} {}"), fmt::scan_error::index_mode_mismatch, 3, { 42 } },
    //    };
    //
    //    run_test_batch(mixed_indexing_cases);
    //}

    VX_SECTION("valid cases")
    {
        constexpr scan_test_case<C, int> valid_indexing_cases[] = {
            { LIT("42"),           LIT("{}"),           fmt::scan_error::none, 2, { 42 } },
            { LIT(" \n\t42\t\n "), LIT("{}"),           fmt::scan_error::none, 5, { 42 } },
            { LIT("42"),           LIT(" \n\t{}\t\n "), fmt::scan_error::none, 2, { 42 } },
        };

        run_test_batch(valid_indexing_cases);
    }

    VX_SECTION("multiple arguments")
    {
        constexpr scan_test_case<C, int, str::basic_string_view<C>> multiple_arg_cases1[] = {
            { LIT("1 2"),   LIT("{}{}"),      fmt::scan_error::none, 3, { 1, str::basic_string_view<C>{ LIT("2") } } },
            { LIT("1 2"),   LIT("{} {}"),     fmt::scan_error::none, 3, { 1, str::basic_string_view<C>{ LIT("2") } } },
            { LIT("2 1"),   LIT("{1}{0}"),    fmt::scan_error::none, 3, { 1, str::basic_string_view<C>{ LIT("2") } } },
            { LIT("2 1 2"), LIT("{1}{1}{0}"), fmt::scan_error::none, 5, { 2, str::basic_string_view<C>{ LIT("1") } } },
        };

        run_test_batch(multiple_arg_cases1);
    }

    VX_SECTION("argument index boundaries")
    {
        constexpr scan_test_case<C, int, int, int> index_boundary_cases[] = {
            { LIT("123"), LIT("{0:1}{1:1}{2:1}"), fmt::scan_error::none, { 1, 2, 3 } },
            { LIT("123"), LIT("{1:1}{0:1}{2:1}"), fmt::scan_error::none, { 1, 2, 3 } },
            { LIT("123"), LIT("{2:1}{0:1}{0:1}"), fmt::scan_error::none, { 1, 2, 3 } },
        };
    
        run_test_batch(index_boundary_cases);
    }
    //
    //VX_SECTION("repeated automatic indexing")
    //{
    //    constexpr scan_test_case<C, int, const C*> auto_index_cases[] = {
    //        { LIT("{} {}"),    LIT("1 2"), fmt::scan_error::none,             { 1, LIT("2") } },
    //        { LIT("{} {} {}"), LIT(""),    fmt::scan_error::invalid_argument, { 1, LIT("2") } },
    //    };
    //
    //    run_test_batch(auto_index_cases);
    //}
}

//==============================================================================

VX_TEST_CASE(test_scan)
{
    test_common_cases<char>();
    //test_scan_common_cases<wchar_t>();
}

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}
