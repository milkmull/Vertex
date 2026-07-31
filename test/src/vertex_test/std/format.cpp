#include "vertex/std/format.hpp"
#include "vertex/std/array.hpp"
#include "vertex/std/io.hpp"
#include "vertex/std/string_view.hpp"
#include "vertex_test/test.hpp"

#define LIT(x)          VX_LIT(C, x)
#define NUM(x)          static_cast<I>(x)
#define CHECK_STR(a, b) VX_CHECK(vx::str::compare(a, b) == 0)

#define PRINT_CASE 0

using namespace vx;

// https://github.com/microsoft/STL/blob/020513e211529e7be30cb3e0ca310869701286da/tests/std/tests/P0067R5_charconv/test.cpp#L1012

//==============================================================================

template <typename C, typename... Args>
struct format_test_case
{
    const C* fmt;
    const C* expected;
    fmt::format_error err;
    std::tuple<Args...> args;
};

template <typename C, typename Tuple, size_t... I>
fmt::format_result call_format(
    C* buffer,
    size_t buffer_size,
    const C* fmt,
    size_t fmt_size,
    const Tuple& args,
    std::index_sequence<I...>)
{
    return fmt::format(
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
    fmt::format_error expected_err,
    const Tuple& args)
{
    constexpr size_t N = std::tuple_size<Tuple>::value;

    const size_t fmt_size = str::length(fmt);
    const str::basic_string_view<C> correct(expected);

    constexpr size_t buf_prefix = 20;
    constexpr size_t buf_suffix = 30;
    constexpr size_t space = 1024;

    constexpr size_t buf_size = buf_prefix + space + buf_suffix;
    array<C, buf_size> buf;

    VX_IF_CONSTEXPR (sizeof(C) == sizeof(char))
    {
        println("testing ", fmt, ' ', expected);
    }

    if (expected_err != fmt::format_error::none)
    {
        auto res = call_format(
            buf.data(),
            buf.size(),
            fmt,
            fmt_size,
            args,
            std::make_index_sequence<N>{});

        VX_CHECK(res.err == expected_err);
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
                VX_CHECK(res.err == fmt::format_error::buffer_too_small || res.err == expected_err);
                VX_CHECK(res.count == 0);
                VX_CHECK(all_of(buf_begin, buf_prefix, fill_char));
                // [first, last) is unspecified
                VX_CHECK(all_of(last, buf_suffix, fill_char));
            }
            else
            {
                VX_CHECK(res.err == fmt::format_error::none);
                VX_CHECK(res.count == correct.size());
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
    VX_SECTION("empty")
    {
        constexpr format_test_case<C> empty_cases1[] = {
            { LIT(""), LIT(""), fmt::format_error::none, {} },
        };
    
        run_test_batch(empty_cases1);
    
        constexpr format_test_case<C, int> empty_cases2[] = {
            { LIT(""), LIT(""), fmt::format_error::none, { 42 } },
        };
    
        run_test_batch(empty_cases2);
    }
    
    VX_SECTION("escaped braces")
    {
        constexpr format_test_case<C, int> escape_cases[] = {
            { LIT("{{"),        LIT("{"),       fmt::format_error::none,           { 42 } }, // -> "{"
            { LIT("}}"),        LIT("}"),       fmt::format_error::none,           { 42 } }, // -> "}"
            { LIT("{{}}"),      LIT("{}"),      fmt::format_error::none,           { 42 } }, // -> "{}"
    
            { LIT("{{{"),       LIT(""),        fmt::format_error::invalid_format, { 42 } },
            { LIT("}}}"),       LIT(""),        fmt::format_error::invalid_format, { 42 } },
    
            { LIT("{{{0}}}"),   LIT("{42}"),    fmt::format_error::none,           { 42 } },
            { LIT("{{{0}"),     LIT("{42"),     fmt::format_error::none,           { 42 } },
            { LIT("{0}}}"),     LIT("42}"),     fmt::format_error::none,           { 42 } },
    
            { LIT("{{0}}"),     LIT("{0}"),     fmt::format_error::none,           {}     },
            { LIT("{{:}}"),     LIT("{:}"),     fmt::format_error::none,           {}     },
            { LIT("{{:+f.5}}"), LIT("{:+f.5}"), fmt::format_error::none,           {}     },
        };
    
        run_test_batch(escape_cases);
    }
    
    VX_SECTION("bad format")
    {
        constexpr format_test_case<C, int> bad_format_cases[] = {
            { LIT("{"),                                LIT(""), fmt::format_error::invalid_format, { 42 } },
            { LIT("}"),                                LIT(""), fmt::format_error::invalid_format, { 42 } },
    
            { LIT("{0"),                               LIT(""), fmt::format_error::invalid_format, { 42 } },
            { LIT("{0:"),                              LIT(""), fmt::format_error::invalid_format, { 42 } },
            { LIT("{:"),                               LIT(""), fmt::format_error::invalid_format, { 42 } },
    
            { LIT("{0}}"),                             LIT(""), fmt::format_error::invalid_format, { 42 } },
            { LIT("{}}"),                              LIT(""), fmt::format_error::invalid_format, { 42 } },
    
            { LIT("{-1}"),                             LIT(""), fmt::format_error::invalid_format, { 42 } },
            { LIT("{+1}"),                             LIT(""), fmt::format_error::invalid_format, { 42 } },
            { LIT("{a}"),                              LIT(""), fmt::format_error::invalid_format, { 42 } },
            { LIT("{1a}"),                             LIT(""), fmt::format_error::invalid_format, { 42 } },
    
            { LIT("{::}"),                             LIT(""), fmt::format_error::invalid_format, { 42 } },
            { LIT("{0::}"),                            LIT(""), fmt::format_error::invalid_format, { 42 } },
            { LIT("{0:x:}"),                           LIT(""), fmt::format_error::invalid_format, { 42 } },
            { LIT("{0:x}}"),                           LIT(""), fmt::format_error::invalid_format, { 42 } },
    
            { LIT("{999999999999999999999999999999}"), LIT(""), fmt::format_error::invalid_format, { 42 } },
        };
    
        run_test_batch(bad_format_cases);
    }
    
    VX_SECTION("argument index errors")
    {
        constexpr format_test_case<C, int> arg_index_cases[] = {
            { LIT("{} {}"), LIT(""), fmt::format_error::invalid_argument, { 42 } },
            { LIT("{1}"),   LIT(""), fmt::format_error::invalid_argument, { 42 } },
            { LIT("{42}"),  LIT(""), fmt::format_error::invalid_argument, { 42 } },
        };
    
        run_test_batch(arg_index_cases);
    }
    
    VX_SECTION("mismatched auto/manual indexing")
    {
        constexpr format_test_case<C, int> mixed_indexing_cases[] = {
            { LIT("{} {0}"), LIT(""), fmt::format_error::mode_mismatch, { 42 } },
            { LIT("{0} {}"), LIT(""), fmt::format_error::mode_mismatch, { 42 } },
        };
    
        run_test_batch(mixed_indexing_cases);
    }
    
    VX_SECTION("valid cases")
    {
        constexpr format_test_case<C, int> valid_indexing_cases[] = {
            { LIT("{}"),             LIT("42"),             fmt::format_error::none, { 42 } },
            { LIT("hello {}"),       LIT("hello 42"),       fmt::format_error::none, { 42 } },
            { LIT("{} world"),       LIT("42 world"),       fmt::format_error::none, { 42 } },
            { LIT("hello {} world"), LIT("hello 42 world"), fmt::format_error::none, { 42 } }
        };
    
        run_test_batch(valid_indexing_cases);
    }
    
    VX_SECTION("valid manual indexing")
    {
        constexpr format_test_case<C, int> valid_indexing_cases[] = {
            { LIT("{0}"),             LIT("42"),             fmt::format_error::none, { 42 } },
            { LIT("{0} {0}"),         LIT("42 42"),          fmt::format_error::none, { 42 } },
            { LIT("hello {0}"),       LIT("hello 42"),       fmt::format_error::none, { 42 } },
            { LIT("{0} world"),       LIT("42 world"),       fmt::format_error::none, { 42 } },
            { LIT("hello {0} world"), LIT("hello 42 world"), fmt::format_error::none, { 42 } },
            { LIT("a{0}b{0}c"),       LIT("a42b42c"),        fmt::format_error::none, { 42 } },
        };
    
        run_test_batch(valid_indexing_cases);
    }
    
    VX_SECTION("whitespace")
    {
        constexpr format_test_case<C, int> whitespace_cases[] = {
            { LIT(" {0} "),   LIT(" 42 "),   fmt::format_error::none,           { 42 } },
            { LIT("\t{0}\n"), LIT("\t42\n"), fmt::format_error::none,           { 42 } },
    
            { LIT("{ 0}"),    LIT(""),       fmt::format_error::invalid_format, { 42 } },
            { LIT("{0 }"),    LIT(""),       fmt::format_error::invalid_format, { 42 } },
        };
    
        run_test_batch(whitespace_cases);
    }
    
    VX_SECTION("multiple arguments")
    {
        constexpr format_test_case<C, int, const C*> multiple_arg_cases1[] = {
            { LIT("{} {}"),     LIT("1 2"), fmt::format_error::none, { 1, LIT("2") } },
            { LIT("{}-{}"),     LIT("1-2"), fmt::format_error::none, { 1, LIT("2") } },
            { LIT("{1} {0}"),   LIT("2 1"), fmt::format_error::none, { 1, LIT("2") } },
            { LIT("{1}{1}{0}"), LIT("221"), fmt::format_error::none, { 1, LIT("2") } },
        };
    
        run_test_batch(multiple_arg_cases1);
    }
    
    VX_SECTION("argument index boundaries")
    {
        constexpr format_test_case<C, int, int, int> index_boundary_cases[] = {
            { LIT("{0}"), LIT("1"), fmt::format_error::none, { 1, 2, 3 } },
            { LIT("{1}"), LIT("2"), fmt::format_error::none, { 1, 2, 3 } },
            { LIT("{2}"), LIT("3"), fmt::format_error::none, { 1, 2, 3 } },
        };
    
        run_test_batch(index_boundary_cases);
    }
    
    VX_SECTION("repeated automatic indexing")
    {
        constexpr format_test_case<C, int, const C*> auto_index_cases[] = {
            { LIT("{} {}"),    LIT("1 2"), fmt::format_error::none,             { 1, LIT("2") } },
            { LIT("{} {} {}"), LIT(""),    fmt::format_error::invalid_argument, { 1, LIT("2") } },
        };
    
        run_test_batch(auto_index_cases);
    }

    VX_SECTION("integer formatting")
    {
        constexpr format_test_case<C, int> integer_cases[] = {
            // Basic decimal formatting
            { LIT("{}"),         LIT("42"),           fmt::format_error::none,           { 42 }  },
            { LIT("{:d}"),       LIT("42"),           fmt::format_error::none,           { 42 }  },
            
            // Base conversions
            { LIT("{:x}"),       LIT("2a"),           fmt::format_error::none,           { 42 }  },
            { LIT("{:X}"),       LIT("2A"),           fmt::format_error::none,           { 42 }  },
            { LIT("{:o}"),       LIT("52"),           fmt::format_error::none,           { 42 }  },
            { LIT("{:b}"),       LIT("101010"),       fmt::format_error::none,           { 42 }  },
            { LIT("{:B}"),       LIT("101010"),       fmt::format_error::none,           { 42 }  },
            
            // Signs
            { LIT("{:+d}"),      LIT("+42"),          fmt::format_error::none,           { 42 }  },
            { LIT("{:+d}"),      LIT("-42"),          fmt::format_error::none,           { -42 } },
            { LIT("{: d}"),      LIT(" 42"),          fmt::format_error::none,           { 42 }  },
            { LIT("{: d}"),      LIT("-42"),          fmt::format_error::none,           { -42 } },
            
            // Sign handling for non-decimal
            { LIT("{:+x}"),      LIT("+2a"),          fmt::format_error::none,           { 42 }  },
            { LIT("{:+x}"),      LIT("-2a"),          fmt::format_error::none,           { -42 } },
            { LIT("{: x}"),      LIT(" 2a"),          fmt::format_error::none,           { 42 }  },
            { LIT("{: x}"),      LIT("-2a"),          fmt::format_error::none,           { -42 } },
            
            // Negative base conversions
            { LIT("{:x}"),       LIT("-2a"),          fmt::format_error::none,           { -42 } },
            { LIT("{:X}"),       LIT("-2A"),          fmt::format_error::none,           { -42 } },
            { LIT("{:o}"),       LIT("-52"),          fmt::format_error::none,           { -42 } },
            { LIT("{:b}"),       LIT("-101010"),      fmt::format_error::none,           { -42 } },
            { LIT("{:B}"),       LIT("-101010"),      fmt::format_error::none,           { -42 } },
            
            // Width and padding
            { LIT("{:8d}"),      LIT("      42"),     fmt::format_error::none,           { 42 }  },
            { LIT("{:08d}"),     LIT("00000042"),     fmt::format_error::none,           { 42 }  },
            { LIT("{:+08d}"),    LIT("+0000042"),     fmt::format_error::none,           { 42 }  },
            { LIT("{: 08d}"),    LIT(" 0000042"),     fmt::format_error::none,           { 42 }  },
            
            { LIT("{:8d}"),      LIT("     -42"),     fmt::format_error::none,           { -42 } },
            { LIT("{:08d}"),     LIT("-0000042"),     fmt::format_error::none,           { -42 } },
            { LIT("{:+08d}"),    LIT("-0000042"),     fmt::format_error::none,           { -42 } },
            
            // Width and padding (non-decimal)
            { LIT("{:8x}"),      LIT("      2a"),     fmt::format_error::none,           { 42 }  },
            { LIT("{:08x}"),     LIT("0000002a"),     fmt::format_error::none,           { 42 }  },
            { LIT("{:+08x}"),    LIT("+000002a"),     fmt::format_error::none,           { 42 }  },
            { LIT("{:08x}"),     LIT("-000002a"),     fmt::format_error::none,           { -42 } },

            // Alignment
            { LIT("{:<8d}"),     LIT("42      "),     fmt::format_error::none,           { 42 }  },
            { LIT("{:^8d}"),     LIT("   42   "),     fmt::format_error::none,           { 42 }  },
            { LIT("{:>8d}"),     LIT("      42"),     fmt::format_error::none,           { 42 }  },
            { LIT("{:*^7d}"),    LIT("**42***"),      fmt::format_error::none,           { 42 }  },
            { LIT("{:*^8d}"),    LIT("***42***"),     fmt::format_error::none,           { 42 }  },

            // Alignment (negative)
            { LIT("{:<8d}"),     LIT("-42     "),     fmt::format_error::none,           { -42 } },
            { LIT("{:^8d}"),     LIT("  -42   "),     fmt::format_error::none,           { -42 } },
            { LIT("{:>8d}"),     LIT("     -42"),     fmt::format_error::none,           { -42 } },

            // Alternate forms
            { LIT("{:#x}"),      LIT("0x2a"),         fmt::format_error::none,           { 42 }  },
            { LIT("{:#X}"),      LIT("0X2A"),         fmt::format_error::none,           { 42 }  },
            { LIT("{:#o}"),      LIT("052"),          fmt::format_error::none,           { 42 }  },
            { LIT("{:#b}"),      LIT("0b101010"),     fmt::format_error::none,           { 42 }  },
            { LIT("{:#B}"),      LIT("0B101010"),     fmt::format_error::none,           { 42 }  },

            // Alternate forms with sign (hexadecimal)
            { LIT("{:+#x}"),     LIT("+0x2a"),        fmt::format_error::none,           { 42 }  },
            { LIT("{:+#x}"),     LIT("-0x2a"),        fmt::format_error::none,           { -42 } },
            { LIT("{: #x}"),     LIT(" 0x2a"),        fmt::format_error::none,           { 42 }  },

            // Alternate forms with sign (uppercase hexadecimal)
            { LIT("{:+#X}"),     LIT("+0X2A"),        fmt::format_error::none,           { 42 }  },
            { LIT("{:+#X}"),     LIT("-0X2A"),        fmt::format_error::none,           { -42 } },
            { LIT("{: #X}"),     LIT(" 0X2A"),        fmt::format_error::none,           { 42 }  },

            // Alternate forms with sign (octal)
            { LIT("{:+#o}"),     LIT("+052"),         fmt::format_error::none,           { 42 }  },
            { LIT("{:+#o}"),     LIT("-052"),         fmt::format_error::none,           { -42 } },
            { LIT("{: #o}"),     LIT(" 052"),         fmt::format_error::none,           { 42 }  },

            // Alternate forms with sign (binary)
            { LIT("{:+#b}"),     LIT("+0b101010"),    fmt::format_error::none,           { 42 }  },
            { LIT("{:+#b}"),     LIT("-0b101010"),    fmt::format_error::none,           { -42 } },
            { LIT("{: #b}"),     LIT(" 0b101010"),    fmt::format_error::none,           { 42 }  },

            // Alternate forms with sign (uppercase binary)
            { LIT("{:+#B}"),     LIT("+0B101010"),    fmt::format_error::none,           { 42 }  },
            { LIT("{:+#B}"),     LIT("-0B101010"),    fmt::format_error::none,           { -42 } },
            { LIT("{: #B}"),     LIT(" 0B101010"),    fmt::format_error::none,           { 42 }  },

            // Zero padding with sign and alternate forms (hexadecimal)
            { LIT("{:+#08x}"),   LIT("+0x0002a"),     fmt::format_error::none,           { 42 }  },
            { LIT("{:+#08x}"),   LIT("-0x0002a"),     fmt::format_error::none,           { -42 } },

            // Zero padding with sign and alternate forms (uppercase hexadecimal)
            { LIT("{:+#08X}"),   LIT("+0X0002A"),     fmt::format_error::none,           { 42 }  },
            { LIT("{:+#08X}"),   LIT("-0X0002A"),     fmt::format_error::none,           { -42 } },

            // Zero padding with sign and alternate forms (octal)
            { LIT("{:+#08o}"),   LIT("+0000052"),     fmt::format_error::none,           { 42 }  },
            { LIT("{:+#08o}"),   LIT("-0000052"),     fmt::format_error::none,           { -42 } },

            // Zero padding with sign and alternate forms (binary)
            { LIT("{:+#012b}"),  LIT("+0b000101010"), fmt::format_error::none,           { 42 }  },
            { LIT("{:+#012b}"),  LIT("-0b000101010"), fmt::format_error::none,           { -42 } },

            // Zero padding with sign and alternate forms (uppercase binary)
            { LIT("{:+#012B}"),  LIT("+0B000101010"), fmt::format_error::none,           { 42 }  },
            { LIT("{:+#012B}"),  LIT("-0B000101010"), fmt::format_error::none,           { -42 } },

            // Zero padding with space sign and alternate forms
            { LIT("{: #08x}"),   LIT(" 0x0002a"),     fmt::format_error::none,           { 42 }  },
            { LIT("{: #08X}"),   LIT(" 0X0002A"),     fmt::format_error::none,           { 42 }  },
            { LIT("{: #012b}"),  LIT(" 0b000101010"), fmt::format_error::none,           { 42 }  },

            // Zero padding left align
            { LIT("{:0<8d}"),    LIT("42000000"),     fmt::format_error::none,           { 42 }  },
            { LIT("{:0<8d}"),    LIT("-4200000"),     fmt::format_error::none,           { -42 } },
            { LIT("{:0< #12b}"), LIT(" 0b101010000"), fmt::format_error::none,           { 42 }  },

            // Zero handling
            { LIT("{}"),         LIT("0"),            fmt::format_error::none,           { 0 }   },
            { LIT("{:x}"),       LIT("0"),            fmt::format_error::none,           { 0 }   },
            { LIT("{:+d}"),      LIT("+0"),           fmt::format_error::none,           { 0 }   },

            // Zero alternate forms
            { LIT("{:#x}"),      LIT("0x0"),          fmt::format_error::none,           { 0 }   },
            { LIT("{:#X}"),      LIT("0X0"),          fmt::format_error::none,           { 0 }   },
            { LIT("{:#b}"),      LIT("0b0"),          fmt::format_error::none,           { 0 }   },
            { LIT("{:#B}"),      LIT("0B0"),          fmt::format_error::none,           { 0 }   },
            { LIT("{:#o}"),      LIT("00"),           fmt::format_error::none,           { 0 }   },

            // Invalid type specifiers
            { LIT("{:D}"),       LIT(""),             fmt::format_error::invalid_format, { 42 }  },
            { LIT("{:O}"),       LIT(""),             fmt::format_error::invalid_format, { 42 }  },
            { LIT("{:f}"),       LIT(""),             fmt::format_error::invalid_format, { 42 }  },
            { LIT("{:g}"),       LIT(""),             fmt::format_error::invalid_format, { 42 }  },
            { LIT("{:.2d}"),     LIT(""),             fmt::format_error::invalid_format, { 42 }  },
            { LIT("{:z}"),       LIT(""),             fmt::format_error::invalid_format, { 42 }  },
            { LIT("{:1.0d}"),    LIT(""),             fmt::format_error::invalid_format, { 42 }  },
        };

        run_test_batch(integer_cases);
    }

    VX_SECTION("character formatting")
    {
        constexpr format_test_case<C, char> character_cases[] = {
            // Default / character formatting
            { LIT("{}"),      LIT("A"),         fmt::format_error::none,           { 'A' }  },
            { LIT("{:c}"),    LIT("A"),         fmt::format_error::none,           { 'A' }  },
            { LIT("{:c}"),    LIT(" "),         fmt::format_error::none,           { ' ' }  },
            { LIT("{:c}"),    LIT("\n"),        fmt::format_error::none,           { '\n' } },

            // Width and alignment
            { LIT("{:4c}"),   LIT("A   "),      fmt::format_error::none,           { 'A' }  },
            { LIT("{:<4c}"),  LIT("A   "),      fmt::format_error::none,           { 'A' }  },
            { LIT("{:^4c}"),  LIT(" A  "),      fmt::format_error::none,           { 'A' }  },
            { LIT("{:>4c}"),  LIT("   A"),      fmt::format_error::none,           { 'A' }  },
            { LIT("{:*^4c}"), LIT("*A**"),      fmt::format_error::none,           { 'A' }  },
            { LIT("{:*^5c}"), LIT("**A**"),     fmt::format_error::none,           { 'A' }  },
            { LIT("{:0>4c}"), LIT("000A"),      fmt::format_error::none,           { 'A' }  },

            // Integer formatting fallback
            { LIT("{:d}"),    LIT("65"),        fmt::format_error::none,           { 'A' }  },
            { LIT("{:x}"),    LIT("41"),        fmt::format_error::none,           { 'A' }  },
            { LIT("{:X}"),    LIT("41"),        fmt::format_error::none,           { 'A' }  },
            { LIT("{:o}"),    LIT("101"),       fmt::format_error::none,           { 'A' }  },
            { LIT("{:b}"),    LIT("1000001"),   fmt::format_error::none,           { 'A' }  },
            { LIT("{:B}"),    LIT("1000001"),   fmt::format_error::none,           { 'A' }  },

            // Integer formatting with width
            { LIT("{:4d}"),   LIT("  65"),      fmt::format_error::none,           { 'A' }  },
            { LIT("{:04x}"),  LIT("0041"),      fmt::format_error::none,           { 'A' }  },
            { LIT("{:+d}"),   LIT("+65"),       fmt::format_error::none,           { 'A' }  },
            { LIT("{: d}"),   LIT(" 65"),       fmt::format_error::none,           { 'A' }  },

            // Alternate forms
            { LIT("{:#x}"),   LIT("0x41"),      fmt::format_error::none,           { 'A' }  },
            { LIT("{:#X}"),   LIT("0X41"),      fmt::format_error::none,           { 'A' }  },
            { LIT("{:#o}"),   LIT("0101"),      fmt::format_error::none,           { 'A' }  },
            { LIT("{:#b}"),   LIT("0b1000001"), fmt::format_error::none,           { 'A' }  },
            { LIT("{:#B}"),   LIT("0B1000001"), fmt::format_error::none,           { 'A' }  },

            // Zero character
            //{ LIT("{:c}"),    LIT("\0"),        fmt::format_error::none,           { '\0' } },
            { LIT("{:d}"),    LIT("0"),         fmt::format_error::none,           { '\0' } },
            { LIT("{:#x}"),   LIT("0x0"),       fmt::format_error::none,           { '\0' } },

            // Invalid character format
            { LIT("{:.2c}"),  LIT(""),          fmt::format_error::invalid_format, { 'A' }  },

            // Invalid integer format (forwarded to integer formatter)
            { LIT("{:D}"),    LIT(""),          fmt::format_error::invalid_format, { 'A' }  },
            { LIT("{:O}"),    LIT(""),          fmt::format_error::invalid_format, { 'A' }  },
            { LIT("{:f}"),    LIT(""),          fmt::format_error::invalid_format, { 'A' }  },
            { LIT("{:g}"),    LIT(""),          fmt::format_error::invalid_format, { 'A' }  },
            { LIT("{:.2d}"),  LIT(""),          fmt::format_error::invalid_format, { 'A' }  },
            { LIT("{:z}"),    LIT(""),          fmt::format_error::invalid_format, { 'A' }  },
            { LIT("{:1.0d}"), LIT(""),          fmt::format_error::invalid_format, { 'A' }  },
        };

        run_test_batch(character_cases);
    }

    VX_SECTION("literal formatting")
    {
        constexpr format_test_case<C, const C*> literal_cases[] = {
            // Default literal formatting
            { LIT("{}"),      LIT("hello"),     fmt::format_error::none,           { LIT("hello") } },
            { LIT("{}"),      LIT(""),          fmt::format_error::none,           { LIT("") }      },
            { LIT("{}"),      LIT("A"),         fmt::format_error::none,           { LIT("A") }     },

            // Explicit string formatting
            { LIT("{:s}"),    LIT("hello"),     fmt::format_error::none,           { LIT("hello") } },
            { LIT("{:s}"),    LIT(""),          fmt::format_error::none,           { LIT("") }      },
            { LIT("{:s}"),    LIT("A"),         fmt::format_error::none,           { LIT("A") }     },

            // Width and alignment
            { LIT("{:8}"),    LIT("hello   "),  fmt::format_error::none,           { LIT("hello") } },
            { LIT("{:<8}"),   LIT("hello   "),  fmt::format_error::none,           { LIT("hello") } },
            { LIT("{:>8}"),   LIT("   hello"),  fmt::format_error::none,           { LIT("hello") } },
            { LIT("{:^8}"),   LIT(" hello  "),  fmt::format_error::none,           { LIT("hello") } },
            { LIT("{:*^9}"),  LIT("**hello**"), fmt::format_error::none,           { LIT("hello") } },

            // String formatting with width and alignment
            { LIT("{:8s}"),   LIT("hello   "),  fmt::format_error::none,           { LIT("hello") } },
            { LIT("{:<8s}"),  LIT("hello   "),  fmt::format_error::none,           { LIT("hello") } },
            { LIT("{:>8s}"),  LIT("   hello"),  fmt::format_error::none,           { LIT("hello") } },
            { LIT("{:^8s}"),  LIT(" hello  "),  fmt::format_error::none,           { LIT("hello") } },

            // Precision / truncation
            { LIT("{:.0}"),   LIT(""),          fmt::format_error::none,           { LIT("hello") } },
            { LIT("{:.1}"),   LIT("h"),         fmt::format_error::none,           { LIT("hello") } },
            { LIT("{:.3}"),   LIT("hel"),       fmt::format_error::none,           { LIT("hello") } },
            { LIT("{:.5}"),   LIT("hello"),     fmt::format_error::none,           { LIT("hello") } },
            { LIT("{:.10}"),  LIT("hello"),     fmt::format_error::none,           { LIT("hello") } },

            // String formatting with precision
            { LIT("{:.0s}"),  LIT(""),          fmt::format_error::none,           { LIT("hello") } },
            { LIT("{:.3s}"),  LIT("hel"),       fmt::format_error::none,           { LIT("hello") } },
            { LIT("{:.5s}"),  LIT("hello"),     fmt::format_error::none,           { LIT("hello") } },

            // Width combined with precision
            { LIT("{:8.3}"),  LIT("hel     "),  fmt::format_error::none,           { LIT("hello") } },
            { LIT("{:<8.3}"), LIT("hel     "),  fmt::format_error::none,           { LIT("hello") } },
            { LIT("{:>8.3}"), LIT("     hel"),  fmt::format_error::none,           { LIT("hello") } },
            { LIT("{:^8.3}"), LIT("  hel   "),  fmt::format_error::none,           { LIT("hello") } },

            // Empty literal
            { LIT("{}"),      LIT(""),          fmt::format_error::none,           { LIT("") }      },
            { LIT("{:4}"),    LIT("    "),      fmt::format_error::none,           { LIT("") }      },

            // Invalid literal format specs
            { LIT("{:+}"),    LIT(""),          fmt::format_error::invalid_format, { LIT("hello") } },
            { LIT("{:#}"),    LIT(""),          fmt::format_error::invalid_format, { LIT("hello") } },
            { LIT("{:d}"),    LIT(""),          fmt::format_error::invalid_format, { LIT("hello") } },
            { LIT("{:x}"),    LIT(""),          fmt::format_error::invalid_format, { LIT("hello") } },
            { LIT("{:S}"),    LIT(""),          fmt::format_error::invalid_format, { LIT("hello") } },
        };

        run_test_batch(literal_cases);
    }

    VX_SECTION("float formatting")
    {
        constexpr format_test_case<C, float> float_cases[] = {

            // Basic fixed formatting
            { LIT("{}"),         LIT("42.000000"),            fmt::format_error::none,           { 42.0f }     },
            { LIT("{:f}"),       LIT("42.000000"),            fmt::format_error::none,           { 42.0f }     },
            { LIT("{:.2f}"),     LIT("42.00"),                fmt::format_error::none,           { 42.0f }     },

            // Signs
            { LIT("{:+.2f}"),    LIT("+42.00"),               fmt::format_error::none,           { 42.0f }     },
            { LIT("{:+.2f}"),    LIT("-42.00"),               fmt::format_error::none,           { -42.0f }    },
            { LIT("{: .2f}"),    LIT(" 42.00"),               fmt::format_error::none,           { 42.0f }     },
            { LIT("{: .2f}"),    LIT("-42.00"),               fmt::format_error::none,           { -42.0f }    },

            // Fixed width and padding
            { LIT("{:10.2f}"),   LIT("     42.00"),           fmt::format_error::none,           { 42.0f }     },
            { LIT("{:010.2f}"),  LIT("0000042.00"),           fmt::format_error::none,           { 42.0f }     },
            { LIT("{:+010.2f}"), LIT("+000042.00"),           fmt::format_error::none,           { 42.0f }     },
            { LIT("{: 010.2f}"), LIT(" 000042.00"),           fmt::format_error::none,           { 42.0f }     },

            { LIT("{:010.2f}"),  LIT("-000042.00"),           fmt::format_error::none,           { -42.0f }    },

            // Fixed alignment
            { LIT("{:<10.2f}"),  LIT("42.00     "),           fmt::format_error::none,           { 42.0f }     },
            { LIT("{:^10.2f}"),  LIT("  42.00   "),           fmt::format_error::none,           { 42.0f }     },
            { LIT("{:>10.2f}"),  LIT("     42.00"),           fmt::format_error::none,           { 42.0f }     },
            { LIT("{:*^10.2f}"), LIT("**42.00***"),           fmt::format_error::none,           { 42.0f }     },

            // Scientific notation (short exponent)
            { LIT("{:e}"),       LIT("4.200000e1"),           fmt::format_error::none,           { 42.0f }     },
            { LIT("{:E}"),       LIT("4.200000E1"),           fmt::format_error::none,           { 42.0f }     },
            { LIT("{:.2e}"),     LIT("4.20e1"),               fmt::format_error::none,           { 42.0f }     },
            { LIT("{:.2E}"),     LIT("4.20E1"),               fmt::format_error::none,           { 42.0f }     },

            // Scientific negative exponent
            { LIT("{:e}"),       LIT("1.000000e-6"),          fmt::format_error::none,           { 0.000001f } },
            { LIT("{:.2e}"),     LIT("1.00e-6"),              fmt::format_error::none,           { 0.000001f } },

            // Scientific positive exponent with alternate form
            { LIT("{:#e}"),      LIT("4.200000e+1"),          fmt::format_error::none,           { 42.0f }     },
            { LIT("{:#.0e}"),    LIT("4e+1"),                 fmt::format_error::none,           { 42.0f }     },
            { LIT("{:#.2e}"),    LIT("4.20e+1"),              fmt::format_error::none,           { 42.0f }     },

            // Scientific signs
            { LIT("{:+e}"),      LIT("+4.200000e1"),          fmt::format_error::none,           { 42.0f }     },
            { LIT("{:+e}"),      LIT("-4.200000e1"),          fmt::format_error::none,           { -42.0f }    },
            { LIT("{: e}"),      LIT(" 4.200000e1"),          fmt::format_error::none,           { 42.0f }     },

            // Scientific width/padding
            { LIT("{:12e}"),     LIT("  4.200000e1"),         fmt::format_error::none,           { 42.0f }     },
            { LIT("{:012e}"),    LIT("004.200000e1"),         fmt::format_error::none,           { 42.0f }     },
            { LIT("{:+012e}"),   LIT("+04.200000e1"),         fmt::format_error::none,           { 42.0f }     },
            { LIT("{:+012e}"),   LIT("-04.200000e1"),         fmt::format_error::none,           { -42.0f }    },
            { LIT("{: 012e}"),   LIT(" 04.200000e1"),         fmt::format_error::none,           { 42.0f }     },

            // Scientific alignment
            { LIT("{:<12.2e}"),  LIT("4.20e1      "),         fmt::format_error::none,           { 42.0f }     },
            { LIT("{:^12.2e}"),  LIT("   4.20e1   "),         fmt::format_error::none,           { 42.0f }     },
            { LIT("{:>12.2e}"),  LIT("      4.20e1"),         fmt::format_error::none,           { 42.0f }     },

            // Hexadecimal floating point
            { LIT("{:a}"),       LIT("0x1.500000p5"),         fmt::format_error::none,           { 42.0f }     },
            { LIT("{:A}"),       LIT("0X1.500000P5"),         fmt::format_error::none,           { 42.0f }     },
            { LIT("{:.2a}"),     LIT("0x1.50p5"),             fmt::format_error::none,           { 42.0f }     },
            { LIT("{:.2A}"),     LIT("0X1.50P5"),             fmt::format_error::none,           { 42.0f }     },

            // Hex float signs
            { LIT("{:+a}"),      LIT("+0x1.500000p5"),        fmt::format_error::none,           { 42.0f }     },
            { LIT("{:+a}"),      LIT("-0x1.500000p5"),        fmt::format_error::none,           { -42.0f }    },
            { LIT("{: a}"),      LIT(" 0x1.500000p5"),        fmt::format_error::none,           { 42.0f }     },

            // Hex float alternate form
            { LIT("{:#a}"),      LIT("0x1.500000p+5"),        fmt::format_error::none,           { 42.0f }     },
            { LIT("{:#.0a}"),    LIT("0x1p+6"),               fmt::format_error::none,           { 64.0f }     },
            { LIT("{:+#a}"),     LIT("+0x1.500000p+5"),       fmt::format_error::none,           { 42.0f }     },

            // Hex float width/padding
            { LIT("{:20a}"),     LIT("        0x1.500000p5"), fmt::format_error::none,           { 42.0f }     },
            { LIT("{:020a}"),    LIT("0x000000001.500000p5"), fmt::format_error::none,           { 42.0f }     },
            { LIT("{:+020a}"),   LIT("+0x00000001.500000p5"), fmt::format_error::none,           { 42.0f }     },

            // General formatting
            { LIT("{:g}"),       LIT("42.000000"),            fmt::format_error::none,           { 42.0f }     },

            // Zero
            { LIT("{}"),         LIT("0.000000"),             fmt::format_error::none,           { 0.0f }      },
            { LIT("{:+f}"),      LIT("+0.000000"),            fmt::format_error::none,           { 0.0f }      },

            // Invalid types
            { LIT("{:d}"),       LIT(""),                     fmt::format_error::invalid_format, { 42.0f }     },
            { LIT("{:x}"),       LIT(""),                     fmt::format_error::invalid_format, { 42.0f }     },
            { LIT("{:b}"),       LIT(""),                     fmt::format_error::invalid_format, { 42.0f }     },
            { LIT("{:.2d}"),     LIT(""),                     fmt::format_error::invalid_format, { 42.0f }     },
        };

        run_test_batch(float_cases);
    }

    VX_SECTION("bool formatting")
    {
        constexpr format_test_case<C, bool> bool_cases[] = {
            // Default / boolean formatting
            { LIT("{}"),     LIT("true"),    fmt::format_error::none,           { true }  },
            { LIT("{}"),     LIT("false"),   fmt::format_error::none,           { false } },

            // Explicit boolean formatting
            { LIT("{:s}"),   LIT("true"),    fmt::format_error::none,           { true }  },
            { LIT("{:s}"),   LIT("false"),   fmt::format_error::none,           { false } },

            // Width and alignment
            { LIT("{:6}"),   LIT("true  "),  fmt::format_error::none,           { true }  },
            { LIT("{:6}"),   LIT("false "),  fmt::format_error::none,           { false } },
            { LIT("{:<6}"),  LIT("true  "),  fmt::format_error::none,           { true }  },
            { LIT("{:<6}"),  LIT("false "),  fmt::format_error::none,           { false } },
            { LIT("{:^6}"),  LIT(" true "),  fmt::format_error::none,           { true }  },
            { LIT("{:^7}"),  LIT(" false "), fmt::format_error::none,           { false } },
            { LIT("{:>6}"),  LIT("  true"),  fmt::format_error::none,           { true }  },
            { LIT("{:>7}"),  LIT("  false"), fmt::format_error::none,           { false } },
            { LIT("{:*^6}"), LIT("*true*"),  fmt::format_error::none,           { true }  },
            { LIT("{:*^7}"), LIT("*false*"), fmt::format_error::none,           { false } },

            // Alternate boolean representations (if supported)
            { LIT("{:d}"),   LIT("1"),       fmt::format_error::none,           { true }  },
            { LIT("{:d}"),   LIT("0"),       fmt::format_error::none,           { false } },

            // Invalid boolean formats
            { LIT("{:S}"),   LIT(""),        fmt::format_error::invalid_format, { true }  },
            { LIT("{:c}"),   LIT(""),        fmt::format_error::invalid_format, { true }  },
            { LIT("{:x}"),   LIT(""),        fmt::format_error::invalid_format, { true }  },
            { LIT("{:X}"),   LIT(""),        fmt::format_error::invalid_format, { true }  },
            { LIT("{:f}"),   LIT(""),        fmt::format_error::invalid_format, { true }  },
            { LIT("{:.2s}"), LIT(""),        fmt::format_error::invalid_format, { true }  },
            { LIT("{:z}"),   LIT(""),        fmt::format_error::invalid_format, { true }  },
        };

        run_test_batch(bool_cases);
    }

    VX_SECTION("pointer formatting")
    {
        // Use fixed, known addresses instead of real pointers so the hex
        // digits are fully predictable — avoids needing the masked-compare
        // trick used for real/opaque pointer values.
        void* const null_ptr = nullptr;
        void* const p_2a = reinterpret_cast<void*>(static_cast<uintptr_t>(0x2a));
        void* const p_dead = reinterpret_cast<void*>(static_cast<uintptr_t>(0xdeadbeef));

        const format_test_case<C, void* const> pointer_cases[] = {
            // Basic formatting
            { LIT("{}"),       LIT("0x2a"),         fmt::format_error::none,           { p_2a }     },
            { LIT("{:p}"),     LIT("0x2a"),         fmt::format_error::none,           { p_2a }     },

            // Null pointer
            { LIT("{}"),       LIT("0x0"),          fmt::format_error::none,           { null_ptr } },
            { LIT("{:p}"),     LIT("0x0"),          fmt::format_error::none,           { null_ptr } },

            // Larger address
            { LIT("{}"),       LIT("0xdeadbeef"),   fmt::format_error::none,           { p_dead }   },

            // Alignment
            { LIT("{:<12p}"),  LIT("0x2a        "), fmt::format_error::none,           { p_2a }     },
            { LIT("{:^12p}"),  LIT("    0x2a    "), fmt::format_error::none,           { p_2a }     },
            { LIT("{:>12p}"),  LIT("        0x2a"), fmt::format_error::none,           { p_2a }     },
            { LIT("{:*^12p}"), LIT("****0x2a****"), fmt::format_error::none,           { p_2a }     },
            { LIT("{:*^11p}"), LIT("***0x2a****"),  fmt::format_error::none,           { p_2a }     },

            // Invalid type specifiers (pointers shouldn't accept numeric-style specs)
            { LIT("{:P}"),     LIT(""),             fmt::format_error::invalid_format, { p_2a }     },
            { LIT("{:d}"),     LIT(""),             fmt::format_error::invalid_format, { p_2a }     },
            { LIT("{:x}"),     LIT(""),             fmt::format_error::invalid_format, { p_2a }     },
            { LIT("{:+p}"),    LIT(""),             fmt::format_error::invalid_format, { p_2a }     },
            { LIT("{:#p}"),    LIT(""),             fmt::format_error::invalid_format, { p_2a }     },
            { LIT("{:.2p}"),   LIT(""),             fmt::format_error::invalid_format, { p_2a }     },
            { LIT("{:012p}"),  LIT("0x000000002a"), fmt::format_error::invalid_format, { p_2a }     },
        };

        run_test_batch(pointer_cases);
    }

    VX_SECTION("string formatting")
    {
        const format_test_case<C, str::basic_string<C>> vx_string_cases[] = {
            // Default literal formatting
            { LIT("{}"),   LIT("hello"), fmt::format_error::none,           { str::basic_string<C>(LIT("hello")) } },
            { LIT("{}"),   LIT(""),      fmt::format_error::none,           { str::basic_string<C>(LIT("")) }      },
            { LIT("{}"),   LIT("A"),     fmt::format_error::none,           { str::basic_string<C>(LIT("A")) }     },

            // Explicit string formatting
            { LIT("{:s}"), LIT("hello"), fmt::format_error::none,           { str::basic_string<C>(LIT("hello")) } },
            { LIT("{:s}"), LIT(""),      fmt::format_error::none,           { str::basic_string<C>(LIT("")) }      },
            { LIT("{:s}"), LIT("A"),     fmt::format_error::none,           { str::basic_string<C>(LIT("A")) }     },

            // Invalid literal format specs
            { LIT("{:+}"), LIT(""),      fmt::format_error::invalid_format, { str::basic_string<C>(LIT("hello")) } },
            { LIT("{:#}"), LIT(""),      fmt::format_error::invalid_format, { str::basic_string<C>(LIT("hello")) } },
            { LIT("{:d}"), LIT(""),      fmt::format_error::invalid_format, { str::basic_string<C>(LIT("hello")) } },
            { LIT("{:x}"), LIT(""),      fmt::format_error::invalid_format, { str::basic_string<C>(LIT("hello")) } },
            { LIT("{:S}"), LIT(""),      fmt::format_error::invalid_format, { str::basic_string<C>(LIT("hello")) } },
        };

        run_test_batch(vx_string_cases);

        const format_test_case<C, str::basic_string_view<C>> vx_string_view_cases[] = {
            // Default literal formatting
            { LIT("{}"),   LIT("hello"), fmt::format_error::none,           { str::basic_string_view<C>(LIT("hello")) } },
            { LIT("{}"),   LIT(""),      fmt::format_error::none,           { str::basic_string_view<C>(LIT("")) }      },
            { LIT("{}"),   LIT("A"),     fmt::format_error::none,           { str::basic_string_view<C>(LIT("A")) }     },

            // Explicit string formatting
            { LIT("{:s}"), LIT("hello"), fmt::format_error::none,           { str::basic_string_view<C>(LIT("hello")) } },
            { LIT("{:s}"), LIT(""),      fmt::format_error::none,           { str::basic_string_view<C>(LIT("")) }      },
            { LIT("{:s}"), LIT("A"),     fmt::format_error::none,           { str::basic_string_view<C>(LIT("A")) }     },

            // Invalid literal format specs
            { LIT("{:+}"), LIT(""),      fmt::format_error::invalid_format, { str::basic_string_view<C>(LIT("hello")) } },
            { LIT("{:#}"), LIT(""),      fmt::format_error::invalid_format, { str::basic_string_view<C>(LIT("hello")) } },
            { LIT("{:d}"), LIT(""),      fmt::format_error::invalid_format, { str::basic_string_view<C>(LIT("hello")) } },
            { LIT("{:x}"), LIT(""),      fmt::format_error::invalid_format, { str::basic_string_view<C>(LIT("hello")) } },
            { LIT("{:S}"), LIT(""),      fmt::format_error::invalid_format, { str::basic_string_view<C>(LIT("hello")) } },
        };

        run_test_batch(vx_string_view_cases);
    }
}

//==============================================================================

void test_char_conversions()
{
#if defined(__cpp_char8_t)

    VX_SECTION("char to char8_t")
    {
        constexpr format_test_case<char, char8_t> char8_cases[] = {
            { "{}",     "A",     fmt::format_error::none,           { u8'A' } },
            { "{:c}",   "A",     fmt::format_error::none,           { u8'A' } },
            { "{:4c}",  "A   ",  fmt::format_error::none,           { u8'A' } },
            { "{:<4c}", "A   ",  fmt::format_error::none,           { u8'A' } },
            { "{:^5c}", "  A  ", fmt::format_error::none,           { u8'A' } },
            { "{:d}",   "65",    fmt::format_error::none,           { u8'A' } },
            { "{:x}",   "41",    fmt::format_error::none,           { u8'A' } },
            { "{:#x}",  "0x41",  fmt::format_error::none,           { u8'A' } },
            { "{:.2c}", "",      fmt::format_error::invalid_format, { u8'A' } },
        };

        run_test_batch(char8_cases);
    }

#endif
}

#if defined(__cpp_char8_t)

void test_char8_conversions()
{
    VX_SECTION("char to char8_t")
    {
        constexpr format_test_case<char8_t, char> char_to_char8_cases[] = {
            { u8"{}",     u8"A",     fmt::format_error::none,           { 'A' } },
            { u8"{:c}",   u8"A",     fmt::format_error::none,           { 'A' } },
            { u8"{:4c}",  u8"A   ",  fmt::format_error::none,           { 'A' } },
            { u8"{:<4c}", u8"A   ",  fmt::format_error::none,           { 'A' } },
            { u8"{:^5c}", u8"  A  ", fmt::format_error::none,           { 'A' } },
            { u8"{:d}",   u8"65",    fmt::format_error::none,           { 'A' } },
            { u8"{:x}",   u8"41",    fmt::format_error::none,           { 'A' } },
            { u8"{:#x}",  u8"0x41",  fmt::format_error::none,           { 'A' } },
            { u8"{:.2c}", u8"",      fmt::format_error::invalid_format, { 'A' } },
        };

        run_test_batch(char_to_char8_cases);
    }
}

#endif

void test_char16_conversions()
{
    VX_SECTION("char to char16_t")
    {
        constexpr format_test_case<char16_t, char> char_to_char16_cases[] = {
            { u"{}",     u"A",     fmt::format_error::none,           { 'A' } },
            { u"{:c}",   u"A",     fmt::format_error::none,           { 'A' } },
            { u"{:4c}",  u"A   ",  fmt::format_error::none,           { 'A' } },
            { u"{:<4c}", u"A   ",  fmt::format_error::none,           { 'A' } },
            { u"{:^5c}", u"  A  ", fmt::format_error::none,           { 'A' } },
            { u"{:d}",   u"65",    fmt::format_error::none,           { 'A' } },
            { u"{:x}",   u"41",    fmt::format_error::none,           { 'A' } },
            { u"{:#x}",  u"0x41",  fmt::format_error::none,           { 'A' } },
            { u"{:.2c}", u"",      fmt::format_error::invalid_format, { 'A' } },
        };

        run_test_batch(char_to_char16_cases);
    }

#if defined(__cpp_char8_t)

    VX_SECTION("char8_t to char16_t")
    {
        constexpr format_test_case<char16_t, char8_t> char8_to_char16_cases[] = {
            { u"{}",     u"A",     fmt::format_error::none,           { u8'A' } },
            { u"{:c}",   u"A",     fmt::format_error::none,           { u8'A' } },
            { u"{:4c}",  u"A   ",  fmt::format_error::none,           { u8'A' } },
            { u"{:<4c}", u"A   ",  fmt::format_error::none,           { u8'A' } },
            { u"{:^5c}", u"  A  ", fmt::format_error::none,           { u8'A' } },
            { u"{:d}",   u"65",    fmt::format_error::none,           { u8'A' } },
            { u"{:x}",   u"41",    fmt::format_error::none,           { u8'A' } },
            { u"{:#x}",  u"0x14",  fmt::format_error::none,           { u8'A' } },
            { u"{:.2c}", u"",      fmt::format_error::invalid_format, { u8'A' } },
        };

        run_test_batch(char8_to_char16_cases);
    }

#endif
}

void test_char32_conversions()
{
    VX_SECTION("char to char32_t")
    {
        constexpr format_test_case<char32_t, char> char_to_char32_cases[] = {
            { U"{}",     U"A",     fmt::format_error::none,           { 'A' } },
            { U"{:c}",   U"A",     fmt::format_error::none,           { 'A' } },
            { U"{:4c}",  U"A   ",  fmt::format_error::none,           { 'A' } },
            { U"{:<4c}", U"A   ",  fmt::format_error::none,           { 'A' } },
            { U"{:^5c}", U"  A  ", fmt::format_error::none,           { 'A' } },
            { U"{:d}",   U"65",    fmt::format_error::none,           { 'A' } },
            { U"{:x}",   U"41",    fmt::format_error::none,           { 'A' } },
            { U"{:#x}",  U"0x41",  fmt::format_error::none,           { 'A' } },
            { U"{:.2c}", U"",      fmt::format_error::invalid_format, { 'A' } },
        };

        run_test_batch(char_to_char32_cases);
    }

#if defined(__cpp_char8_t)

    VX_SECTION("char8_t to char32_t")
    {
        constexpr format_test_case<char32_t, char8_t> char8_to_char32_cases[] = {
            { U"{}",     U"A",     fmt::format_error::none,           { u8'A' } },
            { U"{:c}",   U"A",     fmt::format_error::none,           { u8'A' } },
            { U"{:4c}",  U"A   ",  fmt::format_error::none,           { u8'A' } },
            { U"{:<4c}", U"A   ",  fmt::format_error::none,           { u8'A' } },
            { U"{:^5c}", U"  A  ", fmt::format_error::none,           { u8'A' } },
            { U"{:d}",   U"65",    fmt::format_error::none,           { u8'A' } },
            { U"{:x}",   U"41",    fmt::format_error::none,           { u8'A' } },
            { U"{:#x}",  U"0x41",  fmt::format_error::none,           { u8'A' } },
            { U"{:.2c}", U"",      fmt::format_error::invalid_format, { u8'A' } },
        };

        run_test_batch(char8_to_char32_cases);
    }

#endif

    VX_SECTION("char16_t to char32_t")
    {
        constexpr format_test_case<char32_t, char16_t> char16_to_char32_cases[] = {
            { U"{}",     U"\u03A9",     fmt::format_error::none,           { u'\u03A9' } },
            { U"{:c}",   U"\u03A9",     fmt::format_error::none,           { u'\u03A9' } },
            { U"{:4c}",  U"\u03A9   ",  fmt::format_error::none,           { u'\u03A9' } },
            { U"{:<4c}", U"\u03A9   ",  fmt::format_error::none,           { u'\u03A9' } },
            { U"{:^5c}", U"  \u03A9  ", fmt::format_error::none,           { u'\u03A9' } },
            { U"{:d}",   U"937",        fmt::format_error::none,           { u'\u03A9' } },
            { U"{:x}",   U"3a9",        fmt::format_error::none,           { u'\u03A9' } },
            { U"{:#x}",  U"0x3a9",      fmt::format_error::none,           { u'\u03A9' } },
            { U"{:.2c}", U"",           fmt::format_error::invalid_format, { u'\u03A9' } },
        };

        run_test_batch(char16_to_char32_cases);
    }
}

//==============================================================================

void test_character_conversions()
{
    test_char_conversions();
#if defined(__cpp_char8_t)
    test_char8_conversions();
#endif
    test_char16_conversions();
    test_char32_conversions();
}

//==============================================================================

VX_TEST_CASE(test_common)
{
    test_common_cases<char>();
    test_common_cases<wchar_t>();
#if defined(__cpp_char8_t)
    test_common_cases<char8_t>();
#endif
    test_common_cases<char16_t>();
    test_common_cases<char32_t>();

    test_character_conversions();
}

//==============================================================================

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}
