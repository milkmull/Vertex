#define VX_FORMAT_ESCAPED_SUPPORT
#define VX_STRING_CONVERT_IND_NAN_SUPPORT
#define VX_STRING_CONVERT_SNAN_SUPPORT

#include "vertex/std/scan.hpp"
#include "vertex/std/array.hpp"
#include "vertex/std/io.hpp"
#include "vertex/std/string.hpp"
#include "vertex/std/string_view.hpp"
#include "vertex_test/test.hpp"

#define LIT(x) VX_LIT(C, x)

using namespace vx;

//==============================================================================

namespace fmt_test_types {

struct point
{
    int x;
    int y;
};

constexpr bool operator==(const point& a, const point& b) noexcept
{
    return a.x == b.x && a.y == b.y;
}

} // namespace fmt_test_types

namespace vx {
namespace fmt {

template <typename C>
struct scanner<fmt_test_types::point, C> : basic_scanner<fmt_test_types::point, C>
{
private:

    using base = basic_scanner<fmt_test_types::point, C>;
    using P = fmt_test_types::point;

    static constexpr scan_error scan_int(basic_scan_context<C>& ctx, int& out) noexcept
    {
        const C* ptr = ctx.in();
        const size_t remaining = ctx.remaining();

        const auto res = strconv::_strconv_priv::parse_integer_impl<int, C, true, true>(
            ptr, remaining, out, 10);

        ctx.consume(res.count);

        if (res.err == strconv::from_string_error::out_of_range)
        {
            return scan_error::result_out_of_range;
        }

        return (res.err == strconv::from_string_error::none)
            ? scan_error::none
            : scan_error::invalid_scaned_field;
    }

public:

    constexpr bool parse(const basic_parse_context<C>& ctx) noexcept
    {
        if (!base::parse(ctx))
        {
            return false;
        }

        return (base::type == C('\0')) || (base::type == C('p'));
    }

    constexpr scan_error scan_field(
        basic_scan_context<C>& ctx,
        P& value) const noexcept
    {
        return ctx.scan_field(*this, value, alignment::right);
    }

    constexpr scan_error scan(
        basic_scan_context<C>& ctx,
        P& value) const noexcept
    {
        if (base::width == 0)
        {
            ctx.consume_whitespace();
        }

        if (!ctx.consume_character(C('{')))
        {
            return scan_error::invalid_scaned_field;
        }

        int x = 0;
        if (auto err = scan_int(ctx, x); err != scan_error::none)
        {
            return err;
        }

        if (!ctx.consume_character(C(',')))
        {
            return scan_error::invalid_scaned_field;
        }

        int y = 0;
        if (auto err = scan_int(ctx, y); err != scan_error::none)
        {
            return err;
        }

        if (!ctx.consume_character(C('}')))
        {
            return scan_error::invalid_scaned_field;
        }

        value.x = x;
        value.y = y;

        return scan_error::none;
    }
};

} // namespace fmt
} // namespace vx

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

template <typename T>
constexpr bool values_equal(const T& a, const T& b)
{
    if constexpr (std::is_floating_point<typename type_traits::remove_cvref<T>::type>::value)
    {
        return (std::isnan(a) && std::isnan(b)) || (a == b);
    }
    else
    {
        return a == b;
    }
}

template <typename Tuple, size_t... I>
bool tuple_values_equal(const Tuple& a, const Tuple& b, std::index_sequence<I...>)
{
    bool ok = true;
    ((ok = ok && values_equal(std::get<I>(a), std::get<I>(b))), ...);
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
        // We only need to check the actual values
        // in cases that parsing should fully succeed.
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
    VX_SECTION("empty")
    {
        constexpr scan_test_case<C, int> empty_cases[] = {
            { LIT(""),   LIT(""),         fmt::scan_error::none,         0, {} },
            { LIT("42"), LIT(""),         fmt::scan_error::none,         0, {} },
            { LIT(""),   LIT("42"),       fmt::scan_error::end_of_input, 0, {} },
            { LIT(""),   LIT(" \n\t242"), fmt::scan_error::end_of_input, 0, {} },
            { LIT(""),   LIT(" \n\t"),    fmt::scan_error::none,         0, {} },
        };

        run_test_batch(empty_cases);
    }

    VX_SECTION("escaped braces")
    {
        constexpr scan_test_case<C, int> escape_cases[] = {
            { LIT("{"),    LIT("{{"),     fmt::scan_error::none, 1, {}     },
            { LIT("}"),    LIT("}}"),     fmt::scan_error::none, 1, {}     },
            { LIT("{}"),   LIT("{{}}"),   fmt::scan_error::none, 2, {}     },

            { LIT("{42}"), LIT("{{{}}}"), fmt::scan_error::none, 4, { 42 } },
            { LIT("{42"),  LIT("{{{}"),   fmt::scan_error::none, 3, { 42 } },
            { LIT("42}"),  LIT("{}}}"),   fmt::scan_error::none, 3, { 42 } },
        };

        run_test_batch(escape_cases);
    }

    VX_SECTION("bad format")
    {
        constexpr scan_test_case<C, int> bad_format_cases[] = {
            { LIT("{"),  LIT("{{{"),                              fmt::scan_error::invalid_format, 0, {} },
            { LIT("}"),  LIT("}}}"),                              fmt::scan_error::invalid_format, 0, {} },

            { LIT("42"), LIT("{0"),                               fmt::scan_error::invalid_format, 0, {} },
            { LIT("42"), LIT("{0:"),                              fmt::scan_error::invalid_format, 0, {} },
            { LIT("42"), LIT("{:"),                               fmt::scan_error::invalid_format, 0, {} },

            { LIT("42"), LIT("{0}}"),                             fmt::scan_error::invalid_format, 0, {} },
            { LIT("42"), LIT("{}}"),                              fmt::scan_error::invalid_format, 0, {} },

            { LIT("42"), LIT("{-1}"),                             fmt::scan_error::invalid_format, 0, {} },
            { LIT("42"), LIT("{+1}"),                             fmt::scan_error::invalid_format, 0, {} },
            { LIT("42"), LIT("{a}"),                              fmt::scan_error::invalid_format, 0, {} },
            { LIT("42"), LIT("{1a}"),                             fmt::scan_error::invalid_format, 0, {} },

            { LIT("42"), LIT("{::}"),                             fmt::scan_error::invalid_format, 0, {} },
            { LIT("42"), LIT("{0::}"),                            fmt::scan_error::invalid_format, 0, {} },
            { LIT("42"), LIT("{0:x:}"),                           fmt::scan_error::invalid_format, 0, {} },
            { LIT("42"), LIT("{0:x}}"),                           fmt::scan_error::invalid_format, 0, {} },

            { LIT("42"), LIT("{999999999999999999999999999999}"), fmt::scan_error::invalid_format, 0, {} },

            // Fill requires an alignment specifier.
            { LIT("42"), LIT("{:*}"),                             fmt::scan_error::invalid_format, 0, {} },
        };

        run_test_batch(bad_format_cases);
    }

    VX_SECTION("argument index errors")
    {
        constexpr scan_test_case<C, int> arg_index_cases[] = {
            { LIT("42"), LIT("{} {}"), fmt::scan_error::end_of_input,     0, {} },
            { LIT("42"), LIT("{1}"),   fmt::scan_error::invalid_argument, 0, {} },
            { LIT("42"), LIT("{42}"),  fmt::scan_error::invalid_argument, 0, {} },
        };

        run_test_batch(arg_index_cases);
    }

    VX_SECTION("mismatched auto/manual indexing")
    {
        constexpr scan_test_case<C, int> mixed_indexing_cases[] = {
            { LIT("42 43"), LIT("{} {0}"), fmt::scan_error::index_mode_mismatch, 0, {} },
            { LIT("42 43"), LIT("{0} {}"), fmt::scan_error::index_mode_mismatch, 0, {} },
        };

        run_test_batch(mixed_indexing_cases);
    }

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
        constexpr scan_test_case<C, int, str::basic_string_view<C>> multiple_arg_cases[] = {
            { LIT("1 2"),   LIT("{}{}"),      fmt::scan_error::none, 3, { 1, str::basic_string_view<C>{ LIT("2") } } },
            { LIT("1 2"),   LIT("{} {}"),     fmt::scan_error::none, 3, { 1, str::basic_string_view<C>{ LIT("2") } } },
            { LIT("2 1"),   LIT("{1}{0}"),    fmt::scan_error::none, 3, { 1, str::basic_string_view<C>{ LIT("2") } } },
            { LIT("2 1 2"), LIT("{1}{1}{0}"), fmt::scan_error::none, 5, { 2, str::basic_string_view<C>{ LIT("1") } } },
        };

        run_test_batch(multiple_arg_cases);
    }

    VX_SECTION("width, alignment, padding")
    {
        constexpr scan_test_case<C, int> width_basic_cases[] = {
            { LIT("12345"), LIT("{:1}"),  fmt::scan_error::none,           1, { 1 }     },
            { LIT("12345"), LIT("{:2}"),  fmt::scan_error::none,           2, { 12 }    },
            { LIT("12345"), LIT("{:3}"),  fmt::scan_error::none,           3, { 123 }   },
            { LIT("12345"), LIT("{:5}"),  fmt::scan_error::none,           5, { 12345 } },
            { LIT("42"),    LIT("{:2}"),  fmt::scan_error::none,           2, { 42 }    },

            // Invalid width specifications.
            { LIT("123"),   LIT("{:0}"),  fmt::scan_error::invalid_format, 0, {}        },
            { LIT("123"),   LIT("{:-1}"), fmt::scan_error::invalid_format, 0, {}        },
            { LIT("123"),   LIT("{:*3}"), fmt::scan_error::invalid_format, 0, {}        },
        };

        run_test_batch(width_basic_cases);

        constexpr scan_test_case<C, int> width_vs_available_input_cases[] = {
            // Shortfall with and without remaining input.
            { LIT("123"),    LIT("{:10}"), fmt::scan_error::end_of_input,         3, {} },
            { LIT("123abc"), LIT("{:6}"),  fmt::scan_error::invalid_scaned_field, 3, {} },
        };

        run_test_batch(width_vs_available_input_cases);

        constexpr scan_test_case<C, int> width_default_align_whitespace_cases[] = {
            { LIT("   42"),    LIT("{:5}"), fmt::scan_error::none,                 5, { 42 } },

            // Padding consumes the field before the value.
            { LIT("   12345"), LIT("{:2}"), fmt::scan_error::invalid_scaned_field, 0, {}     },
            { LIT("   12345"), LIT("{:3}"), fmt::scan_error::invalid_scaned_field, 0, {}     },
            { LIT("   "),      LIT("{:3}"), fmt::scan_error::end_of_input,         0, {}     },
        };

        run_test_batch(width_default_align_whitespace_cases);

        constexpr scan_test_case<C, int> width_explicit_align_right_cases[] = {
            { LIT("***42"), LIT("{:*>5}"), fmt::scan_error::none,                 5, { 42 } },

            // Width truncates into the value.
            { LIT("***42"), LIT("{:*>4}"), fmt::scan_error::none,                 4, { 4 }  },

            // Missing required padding.
            { LIT("42xxx"), LIT("{:*>5}"), fmt::scan_error::invalid_scaned_field, 0, {}     },
            { LIT("42"),    LIT("{:*>5}"), fmt::scan_error::end_of_input,         0, {}     },
        };

        run_test_batch(width_explicit_align_right_cases);

        constexpr scan_test_case<C, int> width_explicit_align_left_cases[] = {
            { LIT("42***"),  LIT("{:*<5}"), fmt::scan_error::none,                 5, { 42 } },

            // Missing required padding.
            { LIT("42*xxx"), LIT("{:*<5}"), fmt::scan_error::invalid_scaned_field, 0, {}     },
            { LIT("42*"),    LIT("{:*<5}"), fmt::scan_error::end_of_input,         0, {}     },
        };

        run_test_batch(width_explicit_align_left_cases);

        constexpr scan_test_case<C, int> width_explicit_align_center_cases[] = {
            { LIT("**42**"), LIT("{:*^6}"), fmt::scan_error::none,                 6, { 42 } },

            // Odd padding puts the extra character on the right.
            { LIT("*42**"),  LIT("{:*^5}"), fmt::scan_error::none,                 5, { 42 } },

            // Center alignment requires the expected split.
            { LIT("**42*"),  LIT("{:*^5}"), fmt::scan_error::invalid_scaned_field, 0, {}     },
            { LIT("**42*"),  LIT("{:*^6}"), fmt::scan_error::end_of_input,         0, {}     },

            { LIT("42"),     LIT("{:*^2}"), fmt::scan_error::none,                 2, { 42 } },
        };

        run_test_batch(width_explicit_align_center_cases);

        constexpr scan_test_case<C, int> width_negative_value_cases[] = {
            { LIT("**-42**"), LIT("{:*^7}"), fmt::scan_error::none, 7, { -42 } },
            { LIT("***-42"),  LIT("{:*>6}"), fmt::scan_error::none, 6, { -42 } },
            { LIT("-42***"),  LIT("{:*<6}"), fmt::scan_error::none, 6, { -42 } },
        };

        run_test_batch(width_negative_value_cases);

        constexpr scan_test_case<C, int> width_base_prefix_cases[] = {
            { LIT("0x2abc"),    LIT("{:4x}"),   fmt::scan_error::none, 4, { 0x2a } },
            { LIT("**0x2a***"), LIT("{:*^9x}"), fmt::scan_error::none, 9, { 0x2a } },
        };

        run_test_batch(width_base_prefix_cases);
    }

    VX_SECTION("alignment without width (unbounded)")
    {
        constexpr scan_test_case<C, int> unbounded_align_cases[] = {
            // Left/right consume any amount of fill.
            { LIT("***42"),    LIT("{:*>}"),  fmt::scan_error::none,         5, { 42 }   },
            { LIT("42***"),    LIT("{:*<}"),  fmt::scan_error::none,         5, { 42 }   },
            { LIT("42"),       LIT("{:*>}"),  fmt::scan_error::none,         2, { 42 }   },
            { LIT("42"),       LIT("{:*<}"),  fmt::scan_error::none,         2, { 42 }   },
            { LIT("***42xyz"), LIT("{:*>}"),  fmt::scan_error::none,         5, { 42 }   },
            { LIT("42**x"),    LIT("{:*<}"),  fmt::scan_error::none,         4, { 42 }   },

            // Center requires balanced padding.
            { LIT("*42*"),     LIT("{:*^}"),  fmt::scan_error::none,         4, { 42 }   },
            { LIT("42"),       LIT("{:*^}"),  fmt::scan_error::none,         2, { 42 }   },
            { LIT("**42"),     LIT("{:*^}"),  fmt::scan_error::end_of_input, 0, {}       },
            { LIT("**42*"),    LIT("{:*^}"),  fmt::scan_error::end_of_input, 0, {}       },

            { LIT("   42"),    LIT("{:>}"),   fmt::scan_error::none,         5, { 42 }   },
            { LIT("42   "),    LIT("{:<}"),   fmt::scan_error::none,         5, { 42 }   },
            { LIT(" 42 "),     LIT("{:^}"),   fmt::scan_error::none,         4, { 42 }   },

            { LIT("*-42*"),    LIT("{:*^}"),  fmt::scan_error::none,         5, { -42 }  },
            { LIT("***0x2a"),  LIT("{:*>x}"), fmt::scan_error::none,         7, { 0x2a } },
        };

        run_test_batch(unbounded_align_cases);
    }
}

//==============================================================================

template <typename C>
void test_integer_cases()
{
    VX_SECTION("integer")
    {
        constexpr scan_test_case<C, int> basic_cases[] = {
            // whitespace skipping
            { LIT("42"),                             LIT("{}"),    fmt::scan_error::none,                 2, { 42 }  },
            { LIT(" 42"),                            LIT("{}"),    fmt::scan_error::none,                 3, { 42 }  },
            { LIT("\t42"),                           LIT("{}"),    fmt::scan_error::none,                 3, { 42 }  },
            { LIT("\n42"),                           LIT("{}"),    fmt::scan_error::none,                 3, { 42 }  },
            { LIT(" \t\n42"),                        LIT("{}"),    fmt::scan_error::none,                 5, { 42 }  },

            // stop at first invalid digit
            { LIT("42abc"),                          LIT("{}"),    fmt::scan_error::none,                 2, { 42 }  },
            { LIT(" 42xyz"),                         LIT("{}"),    fmt::scan_error::none,                 3, { 42 }  },
            { LIT("+42xyz"),                         LIT("{}"),    fmt::scan_error::none,                 3, { 42 }  },
            { LIT("-42xyz"),                         LIT("{}"),    fmt::scan_error::none,                 3, { -42 } },
            { LIT("1010102"),                        LIT("{:b}"),  fmt::scan_error::none,                 6, { 42 }  },
            { LIT("529"),                            LIT("{:o}"),  fmt::scan_error::none,                 2, { 42 }  },
            { LIT("2ag"),                            LIT("{:x}"),  fmt::scan_error::none,                 2, { 42 }  },

            // base 10
            { LIT("42"),                             LIT("{}"),    fmt::scan_error::none,                 2, { 42 }  },
            { LIT("42"),                             LIT("{:d}"),  fmt::scan_error::none,                 2, { 42 }  },
            { LIT("-42"),                            LIT("{:d}"),  fmt::scan_error::none,                 3, { -42 } },
            { LIT("+42"),                            LIT("{:d}"),  fmt::scan_error::none,                 3, { 42 }  }, // '+' always allowed
            { LIT("0"),                              LIT("{:d}"),  fmt::scan_error::none,                 1, { 0 }   },

            // base 2
            { LIT("101010"),                         LIT("{:b}"),  fmt::scan_error::none,                 6, { 42 }  },
            { LIT("101010"),                         LIT("{:B}"),  fmt::scan_error::none,                 6, { 42 }  },
            { LIT("+101010"),                        LIT("{:b}"),  fmt::scan_error::none,                 7, { 42 }  },
            { LIT("-101010"),                        LIT("{:B}"),  fmt::scan_error::none,                 7, { -42 } },
            { LIT("0b101010"),                       LIT("{:b}"),  fmt::scan_error::none,                 8, { 42 }  },
            { LIT("0B101010"),                       LIT("{:B}"),  fmt::scan_error::none,                 8, { 42 }  },
            { LIT("+0b101010"),                      LIT("{:b}"),  fmt::scan_error::none,                 9, { 42 }  },
            { LIT("-0B101010"),                      LIT("{:B}"),  fmt::scan_error::none,                 9, { -42 } },
            { LIT("-0B101010"),                      LIT("{:B}"),  fmt::scan_error::none,                 9, { -42 } },

            // base 8
            { LIT("52"),                             LIT("{:o}"),  fmt::scan_error::none,                 2, { 42 }  },
            { LIT("+52"),                            LIT("{:o}"),  fmt::scan_error::none,                 3, { 42 }  },
            { LIT("052"),                            LIT("{:o}"),  fmt::scan_error::none,                 3, { 42 }  },
            { LIT("+052"),                           LIT("{:o}"),  fmt::scan_error::none,                 4, { 42 }  },
            { LIT("-052"),                           LIT("{:o}"),  fmt::scan_error::none,                 4, { -42 } },
            { LIT("52"),                             LIT("{:O}"),  fmt::scan_error::invalid_format,       0, {}      },

            // base 16
            { LIT("2a"),                             LIT("{:x}"),  fmt::scan_error::none,                 2, { 42 }  },
            { LIT("2A"),                             LIT("{:X}"),  fmt::scan_error::none,                 2, { 42 }  },
            { LIT("+2a"),                            LIT("{:x}"),  fmt::scan_error::none,                 3, { 42 }  },
            { LIT("-2A"),                            LIT("{:X}"),  fmt::scan_error::none,                 3, { -42 } },
            { LIT("0x2a"),                           LIT("{:x}"),  fmt::scan_error::none,                 4, { 42 }  },
            { LIT("0X2A"),                           LIT("{:X}"),  fmt::scan_error::none,                 4, { 42 }  },
            { LIT("+0x2a"),                          LIT("{:x}"),  fmt::scan_error::none,                 5, { 42 }  },
            { LIT("-0X2A"),                          LIT("{:X}"),  fmt::scan_error::none,                 5, { -42 } },
            // hex is case-insensitive
            { LIT("0x0aF"),                          LIT("{:x}"),  fmt::scan_error::none,                 5, { 175 } },

            // default alignment (right)
            { LIT("    42"),                         LIT("{:6}"),  fmt::scan_error::none,                 6, { 42 }  },
            { LIT("42    "),                         LIT("{:6}"),  fmt::scan_error::invalid_scaned_field, 0, {}      },
            { LIT("    42"),                         LIT("{:6d}"), fmt::scan_error::none,                 6, { 42 }  },
            { LIT("42    "),                         LIT("{:6d}"), fmt::scan_error::invalid_scaned_field, 0, {}      },

            { LIT(" 101010"),                        LIT("{:7b}"), fmt::scan_error::none,                 7, { 42 }  },
            { LIT("101010 "),                        LIT("{:7B}"), fmt::scan_error::invalid_scaned_field, 0, { 42 }  },
            { LIT(" 101010"),                        LIT("{:7b}"), fmt::scan_error::none,                 7, { 42 }  },
            { LIT("101010 "),                        LIT("{:7b}"), fmt::scan_error::invalid_scaned_field, 0, {}      },

            { LIT("    52"),                         LIT("{:6o}"), fmt::scan_error::none,                 6, { 42 }  },
            { LIT("52    "),                         LIT("{:6o}"), fmt::scan_error::invalid_scaned_field, 0, {}      },

            { LIT("  0x2a"),                         LIT("{:6x}"), fmt::scan_error::none,                 6, { 42 }  },
            { LIT("0x2a  "),                         LIT("{:6x}"), fmt::scan_error::invalid_scaned_field, 0, {}      },
            { LIT("  0X2A"),                         LIT("{:6X}"), fmt::scan_error::none,                 6, { 42 }  },
            { LIT("0X2A  "),                         LIT("{:6X}"), fmt::scan_error::invalid_scaned_field, 0, {}      },

            // character type
            { LIT("A"),                              LIT("{:c}"),  fmt::scan_error::none,                 1, { 'A' } },
            // character type does not shave whitespace
            { LIT("   A"),                           LIT("{:c}"),  fmt::scan_error::none,                 1, { ' ' } },
            // character type is default left aligned (correct order: width before type)
            { LIT("A     "),                         LIT("{:6c}"), fmt::scan_error::none,                 6, { 'A' } },
            { LIT("     A"),                         LIT("{:6c}"), fmt::scan_error::invalid_scaned_field, 0, {}      },

            // invalid type specifiers for int
            { LIT("42"),                             LIT("{:s}"),  fmt::scan_error::invalid_format,       0, {}      },
            { LIT("42"),                             LIT("{:f}"),  fmt::scan_error::invalid_format,       0, {}      },
            { LIT("42"),                             LIT("{:z}"),  fmt::scan_error::invalid_format,       0, {}      },

            // overflow
            { LIT("999999999999999999999999999999"), LIT("{}"),    fmt::scan_error::result_out_of_range,  0, {}      },
        };

        run_test_batch(basic_cases);
    }
}

//==============================================================================

template <typename C>
void test_character_cases()
{
    VX_SECTION("character")
    {
        constexpr scan_test_case<C, char> basic_cases[] = {
            // raw cases
            { LIT("A"),         LIT("{}"),    fmt::scan_error::none,                 1, { 'A' }  },
            { LIT("A"),         LIT("{:c}"),  fmt::scan_error::none,                 1, { 'A' }  },

            // characters do not skip whitespace
            { LIT(" A"),        LIT("{}"),    fmt::scan_error::none,                 1, { ' ' }  },
            { LIT(" A"),        LIT("{:c}"),  fmt::scan_error::none,                 1, { ' ' }  },
            { LIT("42"),        LIT("{}"),    fmt::scan_error::none,                 1, { '4' }  },
            { LIT("\n42"),      LIT("{}"),    fmt::scan_error::none,                 1, { '\n' } },

            // whitespace skipping (only under integer-style specifiers)
            { LIT("42"),        LIT("{:d}"),  fmt::scan_error::none,                 2, { 42 }   },
            { LIT(" 42"),       LIT("{:d}"),  fmt::scan_error::none,                 3, { 42 }   },
            { LIT("\t42"),      LIT("{:d}"),  fmt::scan_error::none,                 3, { 42 }   },
            { LIT("\n42"),      LIT("{:d}"),  fmt::scan_error::none,                 3, { 42 }   },
            { LIT(" \t\n42"),   LIT("{:d}"),  fmt::scan_error::none,                 5, { 42 }   },

            // stop at first invalid digit
            { LIT("42abc"),     LIT("{}"),    fmt::scan_error::none,                 2, { 42 }   },
            { LIT(" 42xyz"),    LIT("{}"),    fmt::scan_error::none,                 3, { 42 }   },
            { LIT("+42xyz"),    LIT("{}"),    fmt::scan_error::none,                 3, { 42 }   },
            { LIT("-42xyz"),    LIT("{}"),    fmt::scan_error::none,                 3, { -42 }  },
            { LIT("1010102"),   LIT("{:b}"),  fmt::scan_error::none,                 6, { 42 }   },
            { LIT("529"),       LIT("{:o}"),  fmt::scan_error::none,                 2, { 42 }   },
            { LIT("2ag"),       LIT("{:x}"),  fmt::scan_error::none,                 2, { 42 }   },

            // base 10
            { LIT("42"),        LIT("{}"),    fmt::scan_error::none,                 2, { 42 }   },
            { LIT("42"),        LIT("{:d}"),  fmt::scan_error::none,                 2, { 42 }   },
            { LIT("-42"),       LIT("{:d}"),  fmt::scan_error::none,                 3, { -42 }  },
            { LIT("+42"),       LIT("{:d}"),  fmt::scan_error::none,                 3, { 42 }   }, // '+' always allowed
            { LIT("0"),         LIT("{:d}"),  fmt::scan_error::none,                 1, { 0 }    },

            // base 2
            { LIT("101010"),    LIT("{:b}"),  fmt::scan_error::none,                 6, { 42 }   },
            { LIT("101010"),    LIT("{:B}"),  fmt::scan_error::none,                 6, { 42 }   },
            { LIT("+101010"),   LIT("{:b}"),  fmt::scan_error::none,                 7, { 42 }   },
            { LIT("-101010"),   LIT("{:B}"),  fmt::scan_error::none,                 7, { -42 }  },
            { LIT("0b101010"),  LIT("{:b}"),  fmt::scan_error::none,                 8, { 42 }   },
            { LIT("0B101010"),  LIT("{:B}"),  fmt::scan_error::none,                 8, { 42 }   },
            { LIT("+0b101010"), LIT("{:b}"),  fmt::scan_error::none,                 9, { 42 }   },
            { LIT("-0B101010"), LIT("{:B}"),  fmt::scan_error::none,                 9, { -42 }  },
            { LIT("-0B101010"), LIT("{:B}"),  fmt::scan_error::none,                 9, { -42 }  },

            // base 8
            { LIT("52"),        LIT("{:o}"),  fmt::scan_error::none,                 2, { 42 }   },
            { LIT("+52"),       LIT("{:o}"),  fmt::scan_error::none,                 3, { 42 }   },
            { LIT("052"),       LIT("{:o}"),  fmt::scan_error::none,                 3, { 42 }   },
            { LIT("+052"),      LIT("{:o}"),  fmt::scan_error::none,                 4, { 42 }   },
            { LIT("-052"),      LIT("{:o}"),  fmt::scan_error::none,                 4, { -42 }  },
            { LIT("52"),        LIT("{:O}"),  fmt::scan_error::invalid_format,       0, {}       },

            // base 16
            { LIT("2a"),        LIT("{:x}"),  fmt::scan_error::none,                 2, { 42 }   },
            { LIT("2A"),        LIT("{:X}"),  fmt::scan_error::none,                 2, { 42 }   },
            { LIT("+2a"),       LIT("{:x}"),  fmt::scan_error::none,                 3, { 42 }   },
            { LIT("-2A"),       LIT("{:X}"),  fmt::scan_error::none,                 3, { -42 }  },
            { LIT("0x2a"),      LIT("{:x}"),  fmt::scan_error::none,                 4, { 42 }   },
            { LIT("0X2A"),      LIT("{:X}"),  fmt::scan_error::none,                 4, { 42 }   },
            { LIT("+0x2a"),     LIT("{:x}"),  fmt::scan_error::none,                 5, { 42 }   },
            { LIT("0x41"),      LIT("{:x}"),  fmt::scan_error::none,                 4, { 'A' }  },
            // hex digits are case-insensitive
            { LIT("0x2A"),      LIT("{:x}"),  fmt::scan_error::none,                 4, { 42 }   },

            // default alignment (left)
            { LIT("A     "),    LIT("{:6}"),  fmt::scan_error::none,                 6, { 'A' }  },
            { LIT("     A"),    LIT("{:6}"),  fmt::scan_error::invalid_scaned_field, 0, {}       },
            { LIT("A     "),    LIT("{:6c}"), fmt::scan_error::none,                 6, { 'A' }  },
            { LIT("     A"),    LIT("{:6c}"), fmt::scan_error::invalid_scaned_field, 0, {}       },

            // default alignment (right)
            { LIT("    42"),    LIT("{:6}"),  fmt::scan_error::none,                 6, { 42 }   },
            { LIT("42    "),    LIT("{:6}"),  fmt::scan_error::invalid_scaned_field, 0, {}       },
            { LIT("    42"),    LIT("{:6d}"), fmt::scan_error::none,                 6, { 42 }   },
            { LIT("42    "),    LIT("{:6d}"), fmt::scan_error::invalid_scaned_field, 0, {}       },

            { LIT(" 101010"),   LIT("{:7b}"), fmt::scan_error::none,                 7, { 42 }   },
            { LIT("101010 "),   LIT("{:7B}"), fmt::scan_error::invalid_scaned_field, 0, { 42 }   },
            { LIT(" 101010"),   LIT("{:7b}"), fmt::scan_error::none,                 7, { 42 }   },
            { LIT("101010 "),   LIT("{:7b}"), fmt::scan_error::invalid_scaned_field, 0, {}       },

            { LIT("    52"),    LIT("{:6o}"), fmt::scan_error::none,                 6, { 42 }   },
            { LIT("52    "),    LIT("{:6o}"), fmt::scan_error::invalid_scaned_field, 0, {}       },

            { LIT("  0x2a"),    LIT("{:6x}"), fmt::scan_error::none,                 6, { 42 }   },
            { LIT("0x2a  "),    LIT("{:6x}"), fmt::scan_error::invalid_scaned_field, 0, {}       },
            { LIT("  0X2A"),    LIT("{:6X}"), fmt::scan_error::none,                 6, { 42 }   },
            { LIT("0X2A  "),    LIT("{:6X}"), fmt::scan_error::invalid_scaned_field, 0, {}       },

            // invalid type specifiers for int
            { LIT("42"),        LIT("{:s}"),  fmt::scan_error::invalid_format,       0, {}       },
            { LIT("42"),        LIT("{:f}"),  fmt::scan_error::invalid_format,       0, {}       },
            { LIT("42"),        LIT("{:z}"),  fmt::scan_error::invalid_format,       0, {}       },

            // overflow
            { LIT("999"),       LIT("{:d}"),  fmt::scan_error::result_out_of_range,  0, {}       },
        };

        run_test_batch(basic_cases);
    }
}

//==============================================================================

template <typename C>
void test_bool_cases()
{
    VX_SECTION("bool")
    {
        constexpr scan_test_case<C, bool> basic_cases[] = {
            // no specifier accept either form
            { LIT("true"),       LIT("{}"),    fmt::scan_error::none,                 4,  { true }  },
            { LIT("false"),      LIT("{}"),    fmt::scan_error::none,                 5,  { false } },
            { LIT("1"),          LIT("{}"),    fmt::scan_error::none,                 1,  { true }  },
            { LIT("2"),          LIT("{}"),    fmt::scan_error::none,                 1,  { true }  },
            { LIT("0"),          LIT("{}"),    fmt::scan_error::none,                 1,  { false } },

            // stop at valid match
            { LIT("trueeee"),    LIT("{}"),    fmt::scan_error::none,                 4,  { true }  },
            { LIT("falseeee"),   LIT("{}"),    fmt::scan_error::none,                 5,  { false } },
            { LIT("1xyz"),       LIT("{}"),    fmt::scan_error::none,                 1,  { true }  },
            { LIT("0xyz"),       LIT("{}"),    fmt::scan_error::none,                 1,  { false } },
            { LIT("001xyz"),     LIT("{}"),    fmt::scan_error::none,                 3,  { true }  },
            { LIT("000xyz"),     LIT("{}"),    fmt::scan_error::none,                 3,  { false } },
            { LIT("111xyz"),     LIT("{}"),    fmt::scan_error::none,                 3,  { true }  },
            { LIT("true1"),      LIT("{}"),    fmt::scan_error::none,                 4,  { true }  },
            { LIT("false0"),     LIT("{}"),    fmt::scan_error::none,                 5,  { false } },
            { LIT("true false"), LIT("{}"),    fmt::scan_error::none,                 4,  { true }  },
            { LIT("false true"), LIT("{}"),    fmt::scan_error::none,                 5,  { false } },

            // s specifier, only accept strings
            { LIT("true"),       LIT("{:s}"),  fmt::scan_error::none,                 4,  { true }  },
            { LIT("false"),      LIT("{:s}"),  fmt::scan_error::none,                 5,  { false } },
            { LIT("1"),          LIT("{:s}"),  fmt::scan_error::invalid_scaned_field, 0,  {}        },
            { LIT("2"),          LIT("{:s}"),  fmt::scan_error::invalid_scaned_field, 0,  {}        },
            { LIT("0"),          LIT("{:s}"),  fmt::scan_error::invalid_scaned_field, 0,  {}        },

            // other specifier, only accept numbers
            { LIT("true"),       LIT("{:d}"),  fmt::scan_error::invalid_scaned_field, 0,  {}        },
            { LIT("false"),      LIT("{:d}"),  fmt::scan_error::invalid_scaned_field, 0,  {}        },
            { LIT("1"),          LIT("{:d}"),  fmt::scan_error::none,                 1,  { true }  },
            { LIT("2"),          LIT("{:d}"),  fmt::scan_error::none,                 1,  { true }  },
            { LIT("0"),          LIT("{:d}"),  fmt::scan_error::none,                 1,  { false } },

            // parsing is case sensative
            { LIT("TRUE"),       LIT("{:s}"),  fmt::scan_error::none,                 4,  { true }  },
            { LIT("truE"),       LIT("{:s}"),  fmt::scan_error::none,                 4,  { true }  },
            { LIT("FALSE"),      LIT("{:s}"),  fmt::scan_error::none,                 5,  { false } },
            { LIT("falsE"),      LIT("{:s}"),  fmt::scan_error::none,                 5,  { false } },

            // whitespace skipping
            { LIT(" true"),      LIT("{}"),    fmt::scan_error::none,                 5,  { true }  },
            { LIT(" false"),     LIT("{}"),    fmt::scan_error::none,                 6,  { false } },
            { LIT("\t\ntrue"),   LIT("{}"),    fmt::scan_error::none,                 6,  { true }  },
            { LIT(" 1"),         LIT("{}"),    fmt::scan_error::none,                 2,  { true }  },
            { LIT(" 0"),         LIT("{}"),    fmt::scan_error::none,                 2,  { false } },
            { LIT("\t\n2"),      LIT("{}"),    fmt::scan_error::none,                 3,  { true }  },

            // default alignment (left)
            { LIT("  true"),     LIT("{:6s}"), fmt::scan_error::invalid_scaned_field, 0,  {}        },
            { LIT("true  "),     LIT("{:6s}"), fmt::scan_error::none,                 6,  { true }  },
            { LIT(" false"),     LIT("{:6s}"), fmt::scan_error::invalid_scaned_field, 0,  {}        },
            { LIT("false "),     LIT("{:6s}"), fmt::scan_error::none,                 6,  { false } },

            // whitespace skipping (decimal)
            { LIT(" \t\n1"),     LIT("{:d}"),  fmt::scan_error::none,                 4,  { true }  },

            // base 10
            { LIT("1234567890"), LIT("{:d}"),  fmt::scan_error::none,                 10, { true }  },
            { LIT("1"),          LIT("{:d}"),  fmt::scan_error::none,                 1,  { true }  },
            { LIT("0"),          LIT("{:d}"),  fmt::scan_error::none,                 1,  { false } },
            { LIT("+0"),         LIT("{:d}"),  fmt::scan_error::none,                 2,  { false } },
            { LIT("-0"),         LIT("{:d}"),  fmt::scan_error::none,                 2,  { false } },
            { LIT("+1"),         LIT("{:d}"),  fmt::scan_error::none,                 2,  { true }  },
            { LIT("-1"),         LIT("{:d}"),  fmt::scan_error::none,                 2,  { true }  },

            // base 2
            { LIT("1"),          LIT("{:b}"),  fmt::scan_error::none,                 1,  { true }  },
            { LIT("1"),          LIT("{:B}"),  fmt::scan_error::none,                 1,  { true }  },
            { LIT("0"),          LIT("{:b}"),  fmt::scan_error::none,                 1,  { false } },
            { LIT("+1"),         LIT("{:b}"),  fmt::scan_error::none,                 2,  { true }  },
            { LIT("-1"),         LIT("{:B}"),  fmt::scan_error::none,                 2,  { true }  },
            { LIT("-0"),         LIT("{:b}"),  fmt::scan_error::none,                 2,  { false } },
            { LIT("0b1"),        LIT("{:b}"),  fmt::scan_error::none,                 3,  { true }  },
            { LIT("0B1"),        LIT("{:B}"),  fmt::scan_error::none,                 3,  { true }  },
            { LIT("+0b1"),       LIT("{:b}"),  fmt::scan_error::none,                 4,  { true }  },
            { LIT("-0B1"),       LIT("{:B}"),  fmt::scan_error::none,                 4,  { true }  },
            { LIT("0b0"),        LIT("{:b}"),  fmt::scan_error::none,                 3,  { false } },

            // base 8
            { LIT("1"),          LIT("{:o}"),  fmt::scan_error::none,                 1,  { true }  },
            { LIT("0"),          LIT("{:o}"),  fmt::scan_error::none,                 1,  { false } },
            { LIT("+1"),         LIT("{:o}"),  fmt::scan_error::none,                 2,  { true }  },
            { LIT("-1"),         LIT("{:o}"),  fmt::scan_error::none,                 2,  { true }  },
            { LIT("01"),         LIT("{:o}"),  fmt::scan_error::none,                 2,  { true }  },
            { LIT("+01"),        LIT("{:o}"),  fmt::scan_error::none,                 3,  { true }  },
            { LIT("-01"),        LIT("{:o}"),  fmt::scan_error::none,                 3,  { true }  },
            { LIT("00"),         LIT("{:o}"),  fmt::scan_error::none,                 2,  { false } },
            { LIT("1"),          LIT("{:O}"),  fmt::scan_error::invalid_format,       0,  {}        },

            // base 16
            { LIT("1"),          LIT("{:x}"),  fmt::scan_error::none,                 1,  { true }  },
            { LIT("1"),          LIT("{:X}"),  fmt::scan_error::none,                 1,  { true }  },
            { LIT("0"),          LIT("{:x}"),  fmt::scan_error::none,                 1,  { false } },
            { LIT("+1"),         LIT("{:x}"),  fmt::scan_error::none,                 2,  { true }  },
            { LIT("-1"),         LIT("{:X}"),  fmt::scan_error::none,                 2,  { true }  },
            { LIT("0x1"),        LIT("{:x}"),  fmt::scan_error::none,                 3,  { true }  },
            { LIT("0X1"),        LIT("{:X}"),  fmt::scan_error::none,                 3,  { true }  },
            { LIT("+0x1"),       LIT("{:x}"),  fmt::scan_error::none,                 4,  { true }  },
            { LIT("-0X1"),       LIT("{:X}"),  fmt::scan_error::none,                 4,  { true }  },
            { LIT("0x0"),        LIT("{:x}"),  fmt::scan_error::none,                 3,  { false } },
            // hex is case-insensitive
            { LIT("0x0aF"),      LIT("{:x}"),  fmt::scan_error::none,                 5,  { true }  },

            // default alignment (right)
            { LIT("     1"),     LIT("{:6d}"), fmt::scan_error::none,                 6,  { true }  },
            { LIT("1     "),     LIT("{:6d}"), fmt::scan_error::invalid_scaned_field, 0,  {}        },

            { LIT("      0"),    LIT("{:7b}"), fmt::scan_error::none,                 7,  { false } },
            { LIT("0      "),    LIT("{:7B}"), fmt::scan_error::invalid_scaned_field, 0,  {}        },
            { LIT("      1"),    LIT("{:7b}"), fmt::scan_error::none,                 7,  { true }  },
            { LIT("1      "),    LIT("{:7b}"), fmt::scan_error::invalid_scaned_field, 0,  {}        },

            { LIT("    01"),     LIT("{:6o}"), fmt::scan_error::none,                 6,  { true }  },
            { LIT("01    "),     LIT("{:6o}"), fmt::scan_error::invalid_scaned_field, 0,  {}        },

            { LIT("   0x1"),     LIT("{:6x}"), fmt::scan_error::none,                 6,  { true }  },
            { LIT("0x1   "),     LIT("{:6x}"), fmt::scan_error::invalid_scaned_field, 0,  {}        },
            { LIT("   0X0"),     LIT("{:6X}"), fmt::scan_error::none,                 6,  { false } },
            { LIT("0X0   "),     LIT("{:6X}"), fmt::scan_error::invalid_scaned_field, 0,  {}        },

            // character type
            { LIT("A"),          LIT("{:c}"),  fmt::scan_error::none,                 1,  { true }  },
            // character type does not shave whitespace
            { LIT("   A"),       LIT("{:c}"),  fmt::scan_error::none,                 1,  { true }  },
            // character type is default left aligned (correct order: width before type)
            { LIT("A     "),     LIT("{:6c}"), fmt::scan_error::none,                 6,  { true }  },
            { LIT("     A"),     LIT("{:6c}"), fmt::scan_error::invalid_scaned_field, 0,  {}        },

            // invalid type specifiers for bool
            { LIT("1"),          LIT("{:f}"),  fmt::scan_error::invalid_format,       0,  {}        },
            { LIT("1"),          LIT("{:z}"),  fmt::scan_error::invalid_format,       0,  {}        },
        };

        run_test_batch(basic_cases);
    }
}

//==============================================================================
template <typename C>
void test_float_cases()
{
    using F = double;

    VX_SECTION("float")
    {
        constexpr auto inf = std::numeric_limits<F>::infinity();
        constexpr auto nan = std::numeric_limits<F>::quiet_NaN();

        constexpr scan_test_case<C, F> basic_cases[] = {
            // no specifier accept either form
            { LIT("42"),           LIT("{}"),    fmt::scan_error::none,                 2,  { 42.0 }   },
            { LIT("42"),           LIT("{:g}"),  fmt::scan_error::none,                 2,  { 42.0 }   },
            { LIT("42"),           LIT("{:f}"),  fmt::scan_error::none,                 2,  { 42.0 }   },
            { LIT("42"),           LIT("{:G}"),  fmt::scan_error::none,                 2,  { 42.0 }   },
            { LIT("42"),           LIT("{:F}"),  fmt::scan_error::none,                 2,  { 42.0 }   },
            { LIT("3.14"),         LIT("{}"),    fmt::scan_error::none,                 4,  { 3.14 }   },
            { LIT("3."),           LIT("{}"),    fmt::scan_error::none,                 2,  { 3.0 }    },
            { LIT(".5"),           LIT("{}"),    fmt::scan_error::none,                 2,  { 0.5 }    },
            { LIT("0.5"),          LIT("{}"),    fmt::scan_error::none,                 3,  { 0.5 }    },
            { LIT("+3.14"),        LIT("{}"),    fmt::scan_error::none,                 5,  { 3.14 }   },
            { LIT("-3.14"),        LIT("{}"),    fmt::scan_error::none,                 5,  { -3.14 }  },
            { LIT("-0.0"),         LIT("{}"),    fmt::scan_error::none,                 4,  { -0.0 }   },

            { LIT("42"),           LIT("{:e}"),  fmt::scan_error::invalid_scaned_field, 0,  {}         },
            { LIT("42"),           LIT("{:E}"),  fmt::scan_error::invalid_scaned_field, 0,  {}         },

            // whitespace
            { LIT(" 3.14"),        LIT("{}"),    fmt::scan_error::none,                 5,  { 3.14 }   },
            { LIT(" \t\n3.14"),    LIT("{}"),    fmt::scan_error::none,                 7,  { 3.14 }   },

            // scientific
            { LIT("1e3"),          LIT("{}"),    fmt::scan_error::none,                 3,  { 1000.0 } },
            { LIT("1E3"),          LIT("{}"),    fmt::scan_error::none,                 3,  { 1000.0 } },
            { LIT("1e+3"),         LIT("{}"),    fmt::scan_error::none,                 4,  { 1000.0 } },
            { LIT("1e-3"),         LIT("{}"),    fmt::scan_error::none,                 4,  { 0.001 }  },
            { LIT("3.14e2"),       LIT("{}"),    fmt::scan_error::none,                 6,  { 314.0 }  },
            { LIT("1e3"),          LIT("{:e}"),  fmt::scan_error::none,                 3,  { 1000.0 } },
            { LIT("1e3"),          LIT("{:E}"),  fmt::scan_error::none,                 3,  { 1000.0 } },
            { LIT("1e3"),          LIT("{:g}"),  fmt::scan_error::none,                 3,  { 1000.0 } },
            { LIT("1e3"),          LIT("{:G}"),  fmt::scan_error::none,                 3,  { 1000.0 } },

            // hexadecimal floating point
            { LIT("0x1p0"),        LIT("{:a}"),  fmt::scan_error::none,                 5,  { 1.0 }    },
            { LIT("0X1P0"),        LIT("{:A}"),  fmt::scan_error::none,                 5,  { 1.0 }    },
            { LIT("0x1p1"),        LIT("{:a}"),  fmt::scan_error::none,                 5,  { 2.0 }    },
            { LIT("0x1.8p1"),      LIT("{:a}"),  fmt::scan_error::none,                 7,  { 3.0 }    },
            { LIT("0x1.fp3"),      LIT("{:a}"),  fmt::scan_error::none,                 7,  { 15.5 }   },
            // optional prefix
            { LIT("1p0"),          LIT("{:a}"),  fmt::scan_error::none,                 3,  { 1.0 }    },
            { LIT("1.8p1"),        LIT("{:a}"),  fmt::scan_error::none,                 5,  { 3.0 }    },
            { LIT("1.fp3"),        LIT("{:a}"),  fmt::scan_error::none,                 5,  { 15.5 }   },
            { LIT("+0x1.fp3"),     LIT("{:a}"),  fmt::scan_error::none,                 8,  { 15.5 }   },
            { LIT("-0x1.fp3"),     LIT("{:A}"),  fmt::scan_error::none,                 8,  { -15.5 }  },

            // infinities
            { LIT("inf"),          LIT("{}"),    fmt::scan_error::none,                 3,  { inf }    },
            { LIT("+inf"),         LIT("{}"),    fmt::scan_error::none,                 4,  { inf }    },
            { LIT("-inf"),         LIT("{}"),    fmt::scan_error::none,                 4,  { -inf }   },
            { LIT("infinity"),     LIT("{}"),    fmt::scan_error::none,                 8,  { inf }    },

            // NaN
            { LIT("nan"),          LIT("{}"),    fmt::scan_error::none,                 3,  { nan }    },
            { LIT("nan(payload)"), LIT("{}"),    fmt::scan_error::none,                 12, { nan }    },

            // stop at invalid character
            { LIT("3.14abc"),      LIT("{}"),    fmt::scan_error::none,                 4,  { 3.14 }   },
            { LIT("1e3xyz"),       LIT("{}"),    fmt::scan_error::none,                 3,  { 1000.0 } },
            { LIT("0x1.fp3!"),     LIT("{:a}"),  fmt::scan_error::none,                 7,  { 15.5 }   },
            { LIT("3..14"),        LIT("{}"),    fmt::scan_error::none,                 2,  { 3.0 }    },
            { LIT("1e"),           LIT("{}"),    fmt::scan_error::none,                 1,  { 1.0 }    },
            { LIT("1e+"),          LIT("{}"),    fmt::scan_error::none,                 1,  { 1.0 }    },
            { LIT("0x1p"),         LIT("{:a}"),  fmt::scan_error::none,                 3,  { 1.0 }    },

            // default alignment (right)
            { LIT("  3.14"),       LIT("{:6}"),  fmt::scan_error::none,                 6,  { 3.14 }   },
            { LIT("3.14  "),       LIT("{:6}"),  fmt::scan_error::invalid_scaned_field, 0,  {}         },
            { LIT("  3.14"),       LIT("{:6f}"), fmt::scan_error::none,                 6,  { 3.14 }   },
            { LIT("3.14  "),       LIT("{:6f}"), fmt::scan_error::invalid_scaned_field, 0,  {}         },
            { LIT(" 1e3"),         LIT("{:4e}"), fmt::scan_error::none,                 4,  { 1000.0 } },
            { LIT("1e3 "),         LIT("{:4e}"), fmt::scan_error::invalid_scaned_field, 0,  {}         },
            { LIT(" 0x1p0"),       LIT("{:6a}"), fmt::scan_error::none,                 6,  { 1.0 }    },
            { LIT("0x1p0 "),       LIT("{:6a}"), fmt::scan_error::invalid_scaned_field, 0,  {}         },

            // overflow
            { LIT("1e5000"),       LIT("{}"),    fmt::scan_error::result_out_of_range,  0,  {}         },
            { LIT("-1e5000"),      LIT("{}"),    fmt::scan_error::result_out_of_range,  0,  {}         },
            // underflow
            { LIT("1e-5000"),      LIT("{}"),    fmt::scan_error::result_out_of_range,  0,  {}         },
        };

        run_test_batch(basic_cases);
    }
}

//==============================================================================

template <typename C>
void test_string_view_cases()
{
    using SV = str::basic_string_view<C>;

    VX_SECTION("string_view")
    {
        constexpr scan_test_case<C, SV> basic_cases[] = {
            // default / s specifier
            { LIT("hello"),        LIT("{}"),    fmt::scan_error::none,                 5, { SV{ LIT("hello") } } },
            { LIT("hello"),        LIT("{:s}"),  fmt::scan_error::none,                 5, { SV{ LIT("hello") } } },

            // whitespace skipping
            { LIT(" hello"),       LIT("{}"),    fmt::scan_error::none,                 6, { SV{ LIT("hello") } } },
            { LIT("\t\nhello"),    LIT("{:s}"),  fmt::scan_error::none,                 7, { SV{ LIT("hello") } } },

            // stop at whitespace
            { LIT("hello world"),  LIT("{}"),    fmt::scan_error::none,                 5, { SV{ LIT("hello") } } },
            { LIT("hello\tworld"), LIT("{}"),    fmt::scan_error::none,                 5, { SV{ LIT("hello") } } },
            { LIT("hello\nworld"), LIT("{}"),    fmt::scan_error::none,                 5, { SV{ LIT("hello") } } },

            // width
            { LIT("abcdef"),       LIT("{:3s}"), fmt::scan_error::none,                 3, { SV{ LIT("abc") } }   },
            { LIT("abc"),          LIT("{:3s}"), fmt::scan_error::none,                 3, { SV{ LIT("abc") } }   },
            { LIT("abc"),          LIT("{:5s}"), fmt::scan_error::end_of_input,         0, {}                     },

            // default alignment (left)
            { LIT("abc   "),       LIT("{:6s}"), fmt::scan_error::none,                 6, { SV{ LIT("abc") } }   },
            { LIT("   abc"),       LIT("{:6s}"), fmt::scan_error::invalid_scaned_field, 0, {}                     },

            // c specifier scans exactly width characters
            { LIT("abc"),          LIT("{:3c}"), fmt::scan_error::none,                 3, { SV{ LIT("abc") } }   },
            { LIT("abcdef"),       LIT("{:3c}"), fmt::scan_error::none,                 3, { SV{ LIT("abc") } }   },

            // preserves whitespace
            { LIT("a b"),          LIT("{:3c}"), fmt::scan_error::none,                 3, { SV{ LIT("a b") } }   },
            { LIT("   "),          LIT("{:3c}"), fmt::scan_error::none,                 3, { SV{ LIT("   ") } }   },
            { LIT("\t\n "),        LIT("{:3c}"), fmt::scan_error::none,                 3, { SV{ LIT("\t\n ") } } },

            // exact width required
            { LIT("ab"),           LIT("{:3c}"), fmt::scan_error::end_of_input,         2, {}                     },
            { LIT(""),             LIT("{:1c}"), fmt::scan_error::end_of_input,         0, {}                     },

            // invalid format
            { LIT("abc"),          LIT("{:c}"),  fmt::scan_error::invalid_format,       0, {}                     }, // c requires width
            { LIT("abc"),          LIT("{:d}"),  fmt::scan_error::invalid_format,       0, {}                     },
            { LIT("abc"),          LIT("{:b}"),  fmt::scan_error::invalid_format,       0, {}                     },
            { LIT("abc"),          LIT("{:o}"),  fmt::scan_error::invalid_format,       0, {}                     },
            { LIT("abc"),          LIT("{:x}"),  fmt::scan_error::invalid_format,       0, {}                     },
            { LIT("abc"),          LIT("{:f}"),  fmt::scan_error::invalid_format,       0, {}                     },
            { LIT("abc"),          LIT("{:p}"),  fmt::scan_error::invalid_format,       0, {}                     },
        };

        run_test_batch(basic_cases);
    }
}

//==============================================================================

template <typename C>
void test_pointer_cases()
{
    using P = void*;

    VX_SECTION("pointer")
    {
        const scan_test_case<C, P> basic_cases[] = {
            // default / p specifier
            { LIT("0x0"),                       LIT("{}"),    fmt::scan_error::none,                 3, { reinterpret_cast<P>(uintptr_t{ 0 }) }    },
            { LIT("0x42"),                      LIT("{}"),    fmt::scan_error::none,                 4, { reinterpret_cast<P>(uintptr_t{ 0x42 }) } },
            { LIT("0x2a"),                      LIT("{:p}"),  fmt::scan_error::none,                 4, { reinterpret_cast<P>(uintptr_t{ 0x2a }) } },
            { LIT("0X2A"),                      LIT("{:p}"),  fmt::scan_error::none,                 4, { reinterpret_cast<P>(uintptr_t{ 0x2a }) } },

            // required prefix
            { LIT(" 0x2a"),                     LIT("{:p}"),  fmt::scan_error::none,                 5, { reinterpret_cast<P>(uintptr_t{ 0x2a }) } },
            { LIT("\t\n0x2a"),                  LIT("{:p}"),  fmt::scan_error::none,                 6, { reinterpret_cast<P>(uintptr_t{ 0x2a }) } },

            // stop at first invalid digit
            { LIT("0x2ag"),                     LIT("{:p}"),  fmt::scan_error::none,                 4, { reinterpret_cast<P>(uintptr_t{ 0x2a }) } },
            { LIT("0x2axyz"),                   LIT("{:p}"),  fmt::scan_error::none,                 4, { reinterpret_cast<P>(uintptr_t{ 0x2a }) } },

            // default alignment (right)
            { LIT("  0x2a"),                    LIT("{:6p}"), fmt::scan_error::none,                 6, { reinterpret_cast<P>(uintptr_t{ 0x2a }) } },
            { LIT("0x2a  "),                    LIT("{:6p}"), fmt::scan_error::invalid_scaned_field, 0, {}                                         },

            // invalid input
            { LIT(""),                          LIT("{:p}"),  fmt::scan_error::end_of_input,         0, {}                                         },
            { LIT("0x"),                        LIT("{:p}"),  fmt::scan_error::invalid_scaned_field, 0, {}                                         },
            { LIT("xyz"),                       LIT("{:p}"),  fmt::scan_error::invalid_scaned_field, 0, {}                                         },

            // missing required prefix
            { LIT("0"),                         LIT("{}"),    fmt::scan_error::invalid_scaned_field, 0, {}                                         },
            { LIT("42"),                        LIT("{}"),    fmt::scan_error::invalid_scaned_field, 0, {}                                         },
            { LIT("2a"),                        LIT("{:p}"),  fmt::scan_error::invalid_scaned_field, 0, {}                                         },
            { LIT("2A"),                        LIT("{:p}"),  fmt::scan_error::invalid_scaned_field, 0, {}                                         },
            { LIT("2ag"),                       LIT("{:p}"),  fmt::scan_error::invalid_scaned_field, 0, {}                                         },

            // signs are not permitted
            { LIT("+2a"),                       LIT("{:p}"),  fmt::scan_error::invalid_scaned_field, 0, {}                                         },
            { LIT("-2a"),                       LIT("{:p}"),  fmt::scan_error::invalid_scaned_field, 0, {}                                         },
            { LIT("+0x2a"),                     LIT("{:p}"),  fmt::scan_error::invalid_scaned_field, 0, {}                                         },
            { LIT("-0x2a"),                     LIT("{:p}"),  fmt::scan_error::invalid_scaned_field, 0, {}                                         },

            // invalid presentation types
            { LIT("0x2a"),                      LIT("{:P}"),  fmt::scan_error::invalid_format,       0, {}                                         },
            { LIT("0x2a"),                      LIT("{:d}"),  fmt::scan_error::invalid_format,       0, {}                                         },
            { LIT("0x2a"),                      LIT("{:x}"),  fmt::scan_error::invalid_format,       0, {}                                         },
            { LIT("0x2a"),                      LIT("{:X}"),  fmt::scan_error::invalid_format,       0, {}                                         },
            { LIT("0x2a"),                      LIT("{:b}"),  fmt::scan_error::invalid_format,       0, {}                                         },
            { LIT("0x2a"),                      LIT("{:o}"),  fmt::scan_error::invalid_format,       0, {}                                         },
            { LIT("0x2a"),                      LIT("{:s}"),  fmt::scan_error::invalid_format,       0, {}                                         },
            { LIT("0x2a"),                      LIT("{:c}"),  fmt::scan_error::invalid_format,       0, {}                                         },
            { LIT("0x2a"),                      LIT("{:f}"),  fmt::scan_error::invalid_format,       0, {}                                         },

            // overflow
            { LIT("0xfffffffffffffffffffffff"), LIT("{:p}"),  fmt::scan_error::result_out_of_range,  0, {}                                         },
        };

        run_test_batch(basic_cases);
    }
}

//==============================================================================

template <typename C>
void test_escaped_cases()
{
    using SV = str::basic_string_view<C>;

    VX_SECTION("escaped string_view")
    {
        constexpr scan_test_case<C, SV> basic_cases[] = {
            // basic quoted string, no escapes
            { LIT("\"hi\""),        LIT("{:?}"), fmt::scan_error::none,                 4, { SV{ LIT("hi") } }        },
            { LIT("\"\""),          LIT("{:?}"), fmt::scan_error::none,                 2, { SV{ LIT("") } }          },

            // leading whitespace is skipped (unbounded field)
            { LIT("  \"hi\""),      LIT("{:?}"), fmt::scan_error::none,                 6, { SV{ LIT("hi") } }        },

            // escaped closing quote does not terminate the string early;
            // the RETURNED VIEW is the raw content, backslashes included
            { LIT("\"he\\\"llo\""), LIT("{:?}"), fmt::scan_error::none,                 9, { SV{ LIT("he\\\"llo") } } },

            // escaped backslash, raw view keeps both backslashes
            { LIT("\"a\\\\b\""),    LIT("{:?}"), fmt::scan_error::none,                 6, { SV{ LIT("a\\\\b") } }    },

            // valid \xHH escape, raw view keeps the literal escape text
            { LIT("\"\\x41\""),     LIT("{:?}"), fmt::scan_error::none,                 6, { SV{ LIT("\\x41") } }     },

            // missing opening quote
            { LIT("hi\""),          LIT("{:?}"), fmt::scan_error::invalid_scaned_field, 0, {}                         },

            // unrecognized escape sequence
            { LIT("\"a\\qb\""),     LIT("{:?}"), fmt::scan_error::invalid_scaned_field, 0, {}                         },

            // truncated \xHH escape (second hex digit missing/invalid)
            { LIT("\"a\\x1\""),     LIT("{:?}"), fmt::scan_error::invalid_scaned_field, 0, {}                         },

            // unterminated string (no closing quote at all)
            { LIT("\"hello"),       LIT("{:?}"), fmt::scan_error::invalid_scaned_field, 0, {}                         },

            // invalid type combo (string_view doesn't reject ? at parse
            // time for other reasons, but a numeric type should still fail)
            { LIT("\"hi\""),        LIT("{:d}"), fmt::scan_error::invalid_format,       0, {}                         },
        };

        run_test_batch(basic_cases);
    }

    VX_SECTION("escaped character")
    {
        constexpr scan_test_case<C, char> basic_cases[] = {
            // basic quoted character, no escape
            { LIT("'A'"),     LIT("{:?}"), fmt::scan_error::none,                 3, { 'A' }  },

            // escapes ARE decoded (owned value, not a view)
            { LIT("'\\n'"),   LIT("{:?}"), fmt::scan_error::none,                 4, { '\n' } },
            { LIT("'\\t'"),   LIT("{:?}"), fmt::scan_error::none,                 4, { '\t' } },
            { LIT("'\\r'"),   LIT("{:?}"), fmt::scan_error::none,                 4, { '\r' } },
            { LIT("'\\\\'"),  LIT("{:?}"), fmt::scan_error::none,                 4, { '\\' } },
            { LIT("'\\''"),   LIT("{:?}"), fmt::scan_error::none,                 4, { '\'' } },
            { LIT("'\\x41'"), LIT("{:?}"), fmt::scan_error::none,                 6, { 'A' }  },

            // does not skip leading whitespace before the opening quote
            { LIT(" 'A'"),    LIT("{:?}"), fmt::scan_error::invalid_scaned_field, 0, {}       },

            // missing opening quote
            { LIT("A'"),      LIT("{:?}"), fmt::scan_error::invalid_scaned_field, 0, {}       },

            // unrecognized escape sequence
            { LIT("'\\q'"),   LIT("{:?}"), fmt::scan_error::invalid_scaned_field, 0, {}       },

            // missing closing quote after an unescaped character
            { LIT("'A"),      LIT("{:?}"), fmt::scan_error::end_of_input,         0, {}       },

            // missing closing quote after a decoded multi-char escape
            { LIT("'\\x41"),  LIT("{:?}"), fmt::scan_error::end_of_input,         0, {}       },

            // empty input after the opening quote
            { LIT("'"),       LIT("{:?}"), fmt::scan_error::end_of_input,         0, {}       },
        };

        run_test_batch(basic_cases);
    }
}

//==============================================================================

template <typename C>
void test_custom_type_cases()
{
    using P = fmt_test_types::point;

    VX_SECTION("custom type (point)")
    {
        constexpr scan_test_case<C, P> basic_cases[] = {
            // basic
            { LIT("{42,43}"),    LIT("{}"),      fmt::scan_error::none,                 7,  { P{ 42, 43 } } },
            { LIT("{42,43}"),    LIT("{:p}"),    fmt::scan_error::none,                 7,  { P{ 42, 43 } } },
            { LIT("{-1,-2}"),    LIT("{}"),      fmt::scan_error::none,                 7,  { P{ -1, -2 } } },

            // leading whitespace is skipped like other unbounded fields
            { LIT("  {42,43}"),  LIT("{}"),      fmt::scan_error::none,                 9,  { P{ 42, 43 } } },

            // malformed input
            { LIT("42,43}"),     LIT("{}"),      fmt::scan_error::invalid_scaned_field, 0,  {}              },
            { LIT("{42;43}"),    LIT("{}"),      fmt::scan_error::invalid_scaned_field, 0,  {}              },
            { LIT("{42,43"),     LIT("{}"),      fmt::scan_error::end_of_input,         0,  {}              },

            // invalid presentation type
            { LIT("{42,43}"),    LIT("{:x}"),    fmt::scan_error::invalid_format,       0,  {}              },

            // width + explicit fill/alignment
            { LIT("***{42,43}"), LIT("{:*>10}"), fmt::scan_error::none,                 10, { P{ 42, 43 } } },
            { LIT("{42,43}xxx"), LIT("{:*>10}"), fmt::scan_error::invalid_scaned_field, 0,  {}              },
        };

        run_test_batch(basic_cases);
    }
}

//==============================================================================

VX_TEST_CASE(test_scan)
{
    test_common_cases<char>();
    test_integer_cases<char>();
    test_bool_cases<char>();
    test_float_cases<char>();
    test_string_view_cases<char>();
    test_pointer_cases<char>();
    test_custom_type_cases<char>();
    test_escaped_cases<char>();
}

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}
