#include "vertex/std/format.hpp"
#include "vertex/std/array.hpp"
#include "vertex/std/io.hpp"
#include "vertex/std/string_view.hpp"
#include "vertex_test/test.hpp"

#include "vertex_test/std/format/char_test_cases.hpp"

#define LIT(x)          VX_LIT(C, x)
#define NUM(x)          static_cast<I>(x)
#define CHECK_STR(a, b) VX_CHECK(::vx::str::compare(a, b) == 0)

#define PRINT_CASE 0

// https://github.com/microsoft/STL/blob/020513e211529e7be30cb3e0ca310869701286da/tests/std/tests/P0067R5_charconv/test.cpp#L1012

//==============================================================================

template <typename C, typename... Args>
struct format_test_case
{
    const C* fmt;
    const C* expected;
    vx::format::format_error err;
    std::tuple<Args...> args;
};

template <typename C, typename Tuple, size_t... I>
format::format_result call_format(
    C* buffer,
    size_t buffer_size,
    const C* fmt,
    size_t fmt_size,
    const Tuple& args,
    std::index_sequence<I...>)
{
    return vx::format::format(
        buffer,
        buffer_size,
        fmt,
        fmt_size,
        std::get<I>(args)...);
}

//==============================================================================

template <typename C>
bool all_of(const C* ptr, size_t count, C c)
{
    for (size_t i = 0; i < count; ++i)
    {
        if (ptr[i] != c)
        {
            return false;
        }
    }

    return true;
}

template <typename C, typename Tuple>
void run_format_test(
    const C* fmt,
    const C* expected,
    format::format_error expected_err,
    const Tuple& args)
{
    constexpr size_t N = std::tuple_size<Tuple>::value;

    const size_t fmt_size = str::length(fmt);
    const str::basic_string_view<C> correct(expected);

    constexpr size_t buf_prefix = 20;
    constexpr size_t buf_suffix = 30;
    constexpr size_t space = 1024;

    constexpr size_t buf_size = buf_prefix + space + buf_suffix;
    vx::array<C, buf_size> buf;

    println("testing ", fmt, ' ', expected);

    if (expected_err != format::format_error::none)
    {
        auto res = call_format(
            buf.data(),
            buf.size(),
            fmt,
            fmt_size,
            args,
            std::make_index_sequence<N>{});

        do
        {
            if (!(res.err == expected_err))
            {
                ::vx::test::fail_test("res.err == expected_err", __func__, 89);
            }
        } while ((0, 0));
    }
    else
    {
        constexpr C fill_char = C('@');

        C* const buf_begin = buf.data();
        C* const first = buf_begin + buf_prefix;

        constexpr size_t extra_chars = 3;
        VX_STATIC_ASSERT_MSG(extra_chars + 10 < buf_suffix, "The specific values aren't important, but there should be plenty of room to detect buffer overruns.");

        for (size_t n = 0; n <= correct.size() + extra_chars; ++n)
        {
            buf.fill(fill_char);

            auto res = call_format(
                first,
                n,
                fmt,
                fmt_size,
                args,
                std::make_index_sequence<N>{});

            C* const last = first + n;

            if (n < correct.size())
            {
                do
                {
                    if (!(res.err == format::format_error::buffer_too_small || res.err == expected_err))
                    {
                        ::vx::test::fail_test("res.err == format::format_error::buffer_too_small || res.err == expected_err", __func__, 123);
                    }
                } while ((0, 0));
                do
                {
                    if (!(res.count == 0))
                    {
                        ::vx::test::fail_test("res.count == 0", __func__, 130);
                    }
                } while ((0, 0));
                VX_CHECK(all_of(buf_begin, buf_prefix, fill_char));
                // [first, last) is unspecified
                VX_CHECK(all_of(last, buf_suffix, fill_char));
            }
            else
            {
                VX_CHECK(res.err == format::format_error::none);
                do
                {
                    if (!(res.count == correct.size()))
                    {
                        ::vx::test::fail_test("res.count == correct.size()", __func__, 147);
                    }
                } while ((0, 0));
                VX_CHECK(all_of(buf_begin, buf_prefix, fill_char));
                VX_CHECK(str::compare(first, res.count, correct.data(), correct.size()) == 0);
                VX_CHECK(all_of(first + res.count, buf_suffix, fill_char));
            }
        }
    }
}

template <typename C, typename... Args, size_t N>
void run_test_batch(const format_test_case<C, Args...> (&cases)[N])
{
    for (const auto& tc : cases)
    {
        run_format_test(tc.fmt, tc.expected, tc.err, tc.args);
    }
}

template <typename C>
void test_common_cases()
{
    //VX_SECTION("empty")
    //{
    //    constexpr format_test_case<C> empty_cases1[] = {
    //        { LIT(""), LIT(""), format::format_error::none, {} },
    //    };
    //
    //    run_test_batch(empty_cases1);
    //
    //    constexpr format_test_case<C, int> empty_cases2[] = {
    //        { LIT(""), LIT(""), format::format_error::none, { 42 } },
    //    };
    //
    //    run_test_batch(empty_cases2);
    //}
    //
    //VX_SECTION("escaped braces")
    //{
    //    constexpr format_test_case<C, int> escape_cases[] = {
    //        { LIT("{{"),      LIT("{"),    format::format_error::none,           { 42 } }, // -> "{"
    //        { LIT("}}"),      LIT("}"),    format::format_error::none,           { 42 } }, // -> "}"
    //        { LIT("{{}}"),    LIT("{}"),   format::format_error::none,           { 42 } }, // -> "{}"
    //
    //        { LIT("{{{"),     LIT(""),     format::format_error::invalid_format, { 42 } },
    //        { LIT("}}}"),     LIT(""),     format::format_error::invalid_format, { 42 } },
    //
    //        { LIT("{{{0}}}"), LIT("{42}"), format::format_error::none,           { 42 } },
    //        { LIT("{{{0}"),   LIT("{42"),  format::format_error::none,           { 42 } },
    //        { LIT("{0}}}"),   LIT("42}"),  format::format_error::none,           { 42 } },
    //    };
    //
    //    run_test_batch(escape_cases);
    //}
    //
    //VX_SECTION("bad format")
    //{
    //    constexpr format_test_case<C, int> bad_format_cases[] = {
    //        { LIT("{"),                                LIT(""), format::format_error::invalid_format, { 42 } },
    //        { LIT("}"),                                LIT(""), format::format_error::invalid_format, { 42 } },
    //
    //        { LIT("{0"),                               LIT(""), format::format_error::invalid_format, { 42 } },
    //        { LIT("{0:"),                              LIT(""), format::format_error::invalid_format, { 42 } },
    //        { LIT("{:"),                               LIT(""), format::format_error::invalid_format, { 42 } },
    //
    //        { LIT("{0}}"),                             LIT(""), format::format_error::invalid_format, { 42 } },
    //        { LIT("{}}"),                              LIT(""), format::format_error::invalid_format, { 42 } },
    //
    //        { LIT("{-1}"),                             LIT(""), format::format_error::invalid_format, { 42 } },
    //        { LIT("{+1}"),                             LIT(""), format::format_error::invalid_format, { 42 } },
    //        { LIT("{a}"),                              LIT(""), format::format_error::invalid_format, { 42 } },
    //        { LIT("{1a}"),                             LIT(""), format::format_error::invalid_format, { 42 } },
    //
    //        { LIT("{::}"),                             LIT(""), format::format_error::invalid_format, { 42 } },
    //        { LIT("{0::}"),                            LIT(""), format::format_error::invalid_format, { 42 } },
    //        { LIT("{0:x:}"),                           LIT(""), format::format_error::invalid_format, { 42 } },
    //        { LIT("{0:x}}"),                           LIT(""), format::format_error::invalid_format, { 42 } },
    //
    //        { LIT("{999999999999999999999999999999}"), LIT(""), format::format_error::invalid_format, { 42 } },
    //    };
    //
    //    run_test_batch(bad_format_cases);
    //}
    //
    //VX_SECTION("argument index errors")
    //{
    //    constexpr format_test_case<C, int> arg_index_cases[] = {
    //        { LIT("{} {}"), LIT(""), format::format_error::invalid_argument, { 42 } },
    //        { LIT("{1}"),   LIT(""), format::format_error::invalid_argument, { 42 } },
    //        { LIT("{42}"),  LIT(""), format::format_error::invalid_argument, { 42 } },
    //    };
    //
    //    run_test_batch(arg_index_cases);
    //}
    //
    //VX_SECTION("mismatched auto/manual indexing")
    //{
    //    constexpr format_test_case<C, int> mixed_indexing_cases[] = {
    //        { LIT("{} {0}"), LIT(""), format::format_error::mode_mismatch, { 42 } },
    //        { LIT("{0} {}"), LIT(""), format::format_error::mode_mismatch, { 42 } },
    //    };
    //
    //    run_test_batch(mixed_indexing_cases);
    //}
    //
    //VX_SECTION("valid cases")
    //{
    //    constexpr format_test_case<C, int> valid_indexing_cases[] = {
    //        { LIT("{}"),             LIT("42"),             format::format_error::none, { 42 } },
    //        { LIT("hello {}"),       LIT("hello 42"),       format::format_error::none, { 42 } },
    //        { LIT("{} world"),       LIT("42 world"),       format::format_error::none, { 42 } },
    //        { LIT("hello {} world"), LIT("hello 42 world"), format::format_error::none, { 42 } }
    //    };
    //
    //    run_test_batch(valid_indexing_cases);
    //}
    //
    //VX_SECTION("valid manual indexing")
    //{
    //    constexpr format_test_case<C, int> valid_indexing_cases[] = {
    //        { LIT("{0}"),             LIT("42"),             format::format_error::none, { 42 } },
    //        { LIT("{0} {0}"),         LIT("42 42"),          format::format_error::none, { 42 } },
    //        { LIT("hello {0}"),       LIT("hello 42"),       format::format_error::none, { 42 } },
    //        { LIT("{0} world"),       LIT("42 world"),       format::format_error::none, { 42 } },
    //        { LIT("hello {0} world"), LIT("hello 42 world"), format::format_error::none, { 42 } },
    //        { LIT("a{0}b{0}c"),       LIT("a42b42c"),        format::format_error::none, { 42 } },
    //    };
    //
    //    run_test_batch(valid_indexing_cases);
    //}
    //
    //VX_SECTION("whitespace")
    //{
    //    constexpr format_test_case<C, int> whitespace_cases[] = {
    //        { LIT(" {0} "),   LIT(" 42 "),   format::format_error::none,           { 42 } },
    //        { LIT("\t{0}\n"), LIT("\t42\n"), format::format_error::none,           { 42 } },
    //
    //        { LIT("{ 0}"),    LIT(""),       format::format_error::invalid_format, { 42 } },
    //        { LIT("{0 }"),    LIT(""),       format::format_error::invalid_format, { 42 } },
    //    };
    //
    //    run_test_batch(whitespace_cases);
    //}

    //VX_SECTION("multiple arguments")
    //{
    //    constexpr format_test_case<C, int, const char*> multiple_arg_cases1[] = {
    //        { LIT("{} {}"),     LIT("1 2"), format::format_error::none, { 1, "2" } },
    //        { LIT("{}-{}"),     LIT("1-2"), format::format_error::none, { 1, "2" } },
    //        { LIT("{1} {0}"),   LIT("2 1"), format::format_error::none, { 1, "2" } },
    //        { LIT("{1}{1}{0}"), LIT("221"), format::format_error::none, { 1, "2" } },
    //    };
    //
    //    run_test_batch(multiple_arg_cases1);
    //}
    //
    //VX_SECTION("argument index boundaries")
    //{
    //    constexpr format_test_case<C, int, int, int> index_boundary_cases[] = {
    //        { LIT("{0}"), LIT("1"), format::format_error::none, { 1, 2, 3 } },
    //        { LIT("{1}"), LIT("2"), format::format_error::none, { 1, 2, 3 } },
    //        { LIT("{2}"), LIT("3"), format::format_error::none, { 1, 2, 3 } },
    //    };
    //
    //    run_test_batch(index_boundary_cases);
    //}
    //
    //VX_SECTION("repeated automatic indexing")
    //{
    //    constexpr format_test_case<C, int, const char*> auto_index_cases[] = {
    //        { LIT("{} {}"),    LIT("1 2"), format::format_error::none,             { 1, "2" } },
    //        { LIT("{} {} {}"), LIT(""),    format::format_error::invalid_argument, { 1, "2" } },
    //    };
    //
    //    run_test_batch(auto_index_cases);
    //}

    VX_SECTION("integer formatting")
    {
        constexpr format_test_case<C, int> integer_cases[] = {
            // Basic decimal formatting
            //{ LIT("{}"),      LIT("42"),        format::format_error::none,           { 42 }  },
            //{ LIT("{:d}"),    LIT("42"),        format::format_error::none,           { 42 }  },
            //
            //// Base conversions
            //{ LIT("{:x}"),    LIT("2a"),        format::format_error::none,           { 42 }  },
            //{ LIT("{:X}"),    LIT("2A"),        format::format_error::none,           { 42 }  },
            //{ LIT("{:o}"),    LIT("52"),        format::format_error::none,           { 42 }  },
            //{ LIT("{:b}"),    LIT("101010"),    format::format_error::none,           { 42 }  },
            //{ LIT("{:B}"),    LIT("101010"),    format::format_error::none,           { 42 }  },
            //
            //// Signs
            //{ LIT("{:+d}"),   LIT("+42"),       format::format_error::none,           { 42 }  },
            //{ LIT("{:+d}"),   LIT("-42"),       format::format_error::none,           { -42 } },
            //{ LIT("{: d}"),   LIT(" 42"),       format::format_error::none,           { 42 }  },
            //{ LIT("{: d}"),   LIT("-42"),       format::format_error::none,           { -42 } },
            //
            //// Negative base conversions
            //{ LIT("{:x}"),    LIT("-2a"),       format::format_error::none,           { -42 } },
            //{ LIT("{:X}"),    LIT("-2A"),       format::format_error::none,           { -42 } },
            //{ LIT("{:o}"),    LIT("-52"),       format::format_error::none,           { -42 } },
            //{ LIT("{:b}"),    LIT("-101010"),   format::format_error::none,           { -42 } },
            //{ LIT("{:B}"),    LIT("-101010"),   format::format_error::none,           { -42 } },
            //
            //// Width and padding
            //{ LIT("{:8d}"),   LIT("      42"),  format::format_error::none,           { 42 }  },
            //{ LIT("{:08d}"),  LIT("00000042"),  format::format_error::none,           { 42 }  },
            { LIT("{:+08d}"), LIT("+00000042"), format::format_error::none,           { 42 }  },

            { LIT("{:8d}"),   LIT("     -42"),  format::format_error::none,           { -42 } },
            { LIT("{:08d}"),  LIT("-0000042"),  format::format_error::none,           { -42 } },
            { LIT("{:+08d}"), LIT("-0000042"),  format::format_error::none,           { -42 } },

            // Alignment
            { LIT("{:<8d}"),  LIT("42      "),  format::format_error::none,           { 42 }  },
            { LIT("{:^8d}"),  LIT("   42   "),  format::format_error::none,           { 42 }  },
            { LIT("{:>8d}"),  LIT("      42"),  format::format_error::none,           { 42 }  },
            { LIT("{:*^8d}"), LIT("***42***"),  format::format_error::none,           { 42 }  },

            // Alternate forms
            { LIT("{:#x}"),   LIT("0x2a"),      format::format_error::none,           { 42 }  },
            { LIT("{:#X}"),   LIT("0X2A"),      format::format_error::none,           { 42 }  },
            { LIT("{:#o}"),   LIT("052"),       format::format_error::none,           { 42 }  },
            { LIT("{:#b}"),   LIT("0b101010"),  format::format_error::none,           { 42 }  },
            { LIT("{:#B}"),   LIT("0B101010"),  format::format_error::none,           { 42 }  },

            // Zero handling
            { LIT("{}"),      LIT("0"),         format::format_error::none,           { 0 }   },
            { LIT("{:x}"),    LIT("0"),         format::format_error::none,           { 0 }   },
            { LIT("{:+d}"),   LIT("+0"),        format::format_error::none,           { 0 }   },

            // Invalid type specifiers
            { LIT("{:D}"),    LIT(""),          format::format_error::invalid_format, { 42 }  },
            { LIT("{:O}"),    LIT(""),          format::format_error::invalid_format, { 42 }  },
            { LIT("{:f}"),    LIT(""),          format::format_error::invalid_format, { 42 }  },
            { LIT("{:g}"),    LIT(""),          format::format_error::invalid_format, { 42 }  },
            { LIT("{:.2d}"),  LIT(""),          format::format_error::invalid_format, { 42 }  },
            { LIT("{:z}"),    LIT(""),          format::format_error::invalid_format, { 42 }  },
            { LIT("{:1.0d}"), LIT(""),          format::format_error::invalid_format, { 42 }  },
        };

        run_test_batch(integer_cases);
    }
}

VX_TEST_CASE(test_common)
{
    test_common_cases<char>();
}

//==============================================================================

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}
