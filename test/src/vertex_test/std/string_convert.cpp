#include <cstdint>

#define VX_STRING_CONVERT_IND_NAN
#define VX_STRING_CONVERT_SNAN

#include "vertex/std/string.hpp"
#include "vertex/std/string_convert.hpp"
#include "vertex/std/string_view.hpp"
#include "vertex_test/test.hpp"

#include "string_convert/integer_to_string_test_data.hpp"

#include "string_convert/float_fixed_to_string_test_cases.hpp"
#include "string_convert/float_from_string_test_cases.hpp"
#include "string_convert/float_hex_to_string_test_cases.hpp"
#include "string_convert/float_scientific_to_string_test_cases.hpp"
#include "string_convert/float_to_string_test_cases.hpp"

#include "string_convert/double_fixed_to_string_test_cases_1.hpp"
#include "string_convert/double_fixed_to_string_test_cases_2.hpp"
#include "string_convert/double_fixed_to_string_test_cases_3.hpp"
#include "string_convert/double_fixed_to_string_test_cases_4.hpp"
#include "string_convert/double_from_string_test_cases.hpp"
#include "string_convert/double_hex_to_string_test_cases.hpp"
#include "string_convert/double_scientific_to_string_test_cases_1.hpp"
#include "string_convert/double_scientific_to_string_test_cases_2.hpp"
#include "string_convert/double_scientific_to_string_test_cases_3.hpp"
#include "string_convert/double_scientific_to_string_test_cases_4.hpp"
#include "string_convert/double_to_string_test_cases.hpp"

#define LIT(x)          VX_LIT(C, x)
#define NUM(x)          static_cast<I>(x)
#define CHECK_STR(a, b) VX_CHECK(::vx::str::compare(a, b) == 0)

#define PRINT_CASE 0

// https://github.com/microsoft/STL/blob/020513e211529e7be30cb3e0ca310869701286da/tests/std/tests/P0067R5_charconv/test.cpp#L1012

//==============================================================================

constexpr str::from_string_error fse_inv_arg = str::from_string_error::invalid_argument;
constexpr str::from_string_error fse_out_ran = str::from_string_error::out_of_range;
constexpr str::from_string_error fse_none = str::from_string_error::none;
constexpr str::to_string_error tse_none = str::to_string_error::none;

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

template <typename T, typename C, typename FMT>
void test_common_to_string(const T value, const FMT& fmt, const str::basic_string_view<C> correct)
{
    // Important: Test every effective buffer size from 0 through correct.size() and slightly beyond. For the sizes
    // less than correct.size(), this verifies that the too-small buffer is correctly detected, and that we don't
    // attempt to write outside of it, even by a single char. (This exhaustive validation is necessary because the
    // implementation must check whenever it attempts to write. Sometimes we can calculate the total size and perform
    // a single check, but sometimes we need to check when writing each part of the res.) Testing correct.size()
    // verifies that we can succeed without overrunning, and testing slightly larger sizes verifies that we can succeed
    // without attempting to write to extra chars even when they're available. Finally, we also verify that we aren't
    // underrunning the buffer. This is a concern because sometimes we walk backwards when rounding.

    // detect buffer underruns (specific value isn't important)
    constexpr size_t buf_prefix = 20;
    // detect buffer overruns (specific value isn't important)
    constexpr size_t buf_suffix = 30;

    constexpr size_t space = std::is_integral<T>::value
        ? 1 + 64 // worst case: -2^63 in binary
        : std::is_same<T, float>::value
        ? 1 + 151   // worst case: negative min subnormal float, fixed notation
        : 1 + 1076; // worst case: negative min subnormal double, fixed notation

#if PRINT_CASE

    VX_IF_CONSTEXPR (std::is_same<C, char>::value)
    {
        std::cout << "to string: " << value << std::endl;
    }

#endif

    if (value == 9.9999f)
    {
        const char x = 0;
    }

    constexpr size_t buf_size = buf_prefix + space + buf_suffix;
    vx::array<C, buf_size> buf;

    C* const buf_begin = buf.data();
    C* const buf_end = buf_begin + buf_size;
    C* const first = buf_begin + buf_prefix;

    constexpr size_t extra_chars = 3;
    VX_STATIC_ASSERT_MSG(extra_chars + 10 < buf_suffix, "The specific values aren't important, but there should be plenty of room to detect buffer overruns.");

    constexpr C fill_char = '@';

    for (size_t n = 0; n <= correct.size() + extra_chars; ++n)
    {
        VX_CHECK(n <= static_cast<size_t>(buf_end - first));
        buf.fill(fill_char);

        str::to_string_result res = str::to_string(value, first, n, fmt);
        C* const last = first + n;

        if (n < correct.size())
        {
            VX_CHECK(res.count == 0);
            VX_CHECK(res.err == str::to_string_error::buffer_too_small);
            VX_CHECK(all_of(buf_begin, buf_prefix, fill_char));
            // [first, last) is unspecified
            VX_CHECK(all_of(last, buf_suffix, fill_char));
        }
        else
        {
            VX_CHECK(res.count == correct.size());
            VX_CHECK(res.err == str::to_string_error::none);
            VX_CHECK(all_of(buf_begin, buf_prefix, fill_char));
            do
            {
                if (!(str::compare(first, res.count, correct.data(), correct.size()) == 0))
                {
                    ::vx::test::fail_test("str::compare(first, res.count, correct.data(), correct.size()) == 0", __func__, 127);
                }
            } while ((0, 0));
            VX_CHECK(all_of(first + res.count, buf_suffix, fill_char));
        }
    }
}

//==============================================================================

template <typename I, typename C>
void test_integer_to_string(const I value, const str::integer_format_options& fmt, const str::basic_string_view<C> correct)
{
    test_common_to_string(value, fmt, correct);

    // Also test successful from_string() scenarios.
    {
        I out = 0;
        const str::from_string_result res = str::from_string(correct.data(), correct.size(), out, fmt);

        VX_CHECK(res.count == correct.size());
        VX_CHECK(res.err == fse_none);
        VX_CHECK(out == value);
    }
}

template <typename I, typename C>
bool test_integer_to_string()
{
    str::integer_format_options fmt;

    for (int base = 2; base <= 36; ++base)
    {
        fmt.base = base;

        test_integer_to_string<I, C>(static_cast<I>(0), fmt, LIT("0"));
        test_integer_to_string<I, C>(static_cast<I>(1), fmt, LIT("1"));

        // tests [3, 71]
        {
            const auto tmp = str::string_cast<C>(output_max_digit[base]);
            test_integer_to_string<I, C>(static_cast<I>(base * 2 - 1), fmt, tmp);
        }

        for (const auto& p : output_positive)
        {
            if (p.first <= static_cast<uint64_t>(std::numeric_limits<I>::max()))
            {
                const auto tmp = str::string_cast<C>(p.second[static_cast<size_t>(base)]);
                test_integer_to_string<I, C>(static_cast<I>(p.first), fmt, tmp);
            }
        }

        VX_IF_CONSTEXPR (std::is_signed<I>::value)
        {
            test_integer_to_string<I, C>(static_cast<I>(-1), fmt, LIT("-1"));

            for (const auto& p : output_negative)
            {
                if (p.first >= static_cast<int64_t>(std::numeric_limits<I>::min()))
                {
                    const auto tmp = str::string_cast<C>(p.second[static_cast<size_t>(base)]);
                    test_integer_to_string<I, C>(static_cast<I>(p.first), fmt, tmp);
                }
            }
        }
    }

    fmt.base = 10;
    test_integer_to_string<I, C>(static_cast<I>(42), fmt, LIT("42"));
    return true;
}

enum class test_from_string_mode
{
    normal,
    signaling_nan
};

template <typename T>
struct optional
{
    optional()
        : has_value(false), value{}
    {}

    optional(const T& x)
        : has_value(true), value{ x }
    {}

    bool has_value;
    T value;
};

template <typename T, typename C, typename FMT>
void test_from_string(
    const str::basic_string_view<C> input,
    const FMT& fmt,
    const size_t correct_count,
    const str::from_string_error correct_err,
    optional<T> correct_value = {},
    const test_from_string_mode mode = test_from_string_mode::normal)
{
    VX_IF_CONSTEXPR (std::is_integral<T>::value)
    {
        VX_CHECK(mode == test_from_string_mode::normal);
    }

#if PRINT_CASE

    VX_IF_CONSTEXPR (std::is_same<C, char>::value)
    {
        std::cout << "from string: " << input << std::endl;
    }

#endif

    constexpr T unmodified = 111;
    T out = unmodified;
    const str::from_string_result res = str::from_string(input.data(), input.size(), out, fmt);

    VX_CHECK(res.count == correct_count);
    VX_CHECK(res.err == correct_err);

    if (correct_err == fse_none || (std::is_floating_point<T>::value && correct_err == str::from_string_error::out_of_range))
    {
        VX_IF_CONSTEXPR (std::is_floating_point<T>::value)
        {
            if (mode == test_from_string_mode::normal)
            {
                using uint_type = typename float_traits<T>::uint_type;
                VX_CHECK(correct_value.has_value);

#if PRINT_CASE

                VX_IF_CONSTEXPR (std::is_same<C, char>::value)
                {
                    std::cout << "from string result: " << out << std::endl;
                }

#endif

                VX_CHECK(bit::bit_cast<uint_type>(out) == bit::bit_cast<uint_type>(correct_value.value));
            }
            else
            {
                VX_CHECK(mode == test_from_string_mode::signaling_nan);
                VX_CHECK(!correct_value.has_value);
                using bits_t = float_bits<T>;
                const bits_t bits{ out };
                VX_CHECK(bits.is_nan());
            }
        }
        else
        {
            VX_CHECK(correct_value.has_value);
            VX_CHECK(out == correct_value.value);
        }
    }
    else
    {
        VX_CHECK(!correct_value.has_value);
        VX_CHECK(out == unmodified);
    }
}

template <typename I, typename C>
bool test_integer_from_string()
{
    str::integer_format_options fmt;
    using string_t = str::basic_string<C>;

    const string_t hundred_zeroes(100, '0');
    const string_t hundred_zeroes_and_11 = hundred_zeroes + LIT("11");
    const string_t minus_hundred_zeroes = string_t(LIT("-")) + hundred_zeroes;
    const string_t minus_hundred_zeroes_and_11 = LIT("-") + hundred_zeroes_and_11;
    const string_t plus_hundred_zeroes = string_t(LIT("+")) + hundred_zeroes;
    const string_t plus_hundred_zeroes_and_11 = LIT("+") + hundred_zeroes_and_11;

    const string_t hundred_ones(100, '1');
    const string_t hundred_ones_and_atatat = hundred_ones + LIT("@@@");
    const string_t minus_hundred_ones = string_t(LIT("-")) + hundred_ones;
    const string_t minus_hundred_ones_and_atatat = string_t(LIT("-")) + hundred_ones_and_atatat;
    const string_t plus_hundred_ones = string_t(LIT("+")) + hundred_ones;
    const string_t plus_hundred_ones_and_atatat = string_t(LIT("+")) + hundred_ones_and_atatat;

    for (int base = 2; base <= 36; ++base)
    {
        fmt.base = base;

        test_from_string<I, C>(LIT(""), fmt, 0, fse_inv_arg);   // no characters
        test_from_string<I, C>(LIT("@1"), fmt, 0, fse_inv_arg); // '@' is bogus
        test_from_string<I, C>(LIT(".1"), fmt, 0, fse_inv_arg); // '.' is bogus, for integers
                                                                // "a minus sign is the only sign that may appear"
        test_from_string<I, C>(LIT(" 1"), fmt, 0, fse_inv_arg); // ' ' is bogus, no whitespace in subject sequence

        VX_IF_CONSTEXPR (std::is_unsigned<I>::value)
        {                                                           // N4713 23.20.3 [charconv.from.chars]/3
            test_from_string<I, C>(LIT("-1"), fmt, 0, fse_inv_arg); // "and only if value has a signed type"
        }

        // N4713 23.20.3 [charconv.from.chars]/1 "[ Note: If the pattern allows for an optional sign,
        // but the string has no digit characters following the sign, no characters match the pattern. -end note ]"
        test_from_string<I, C>(LIT("-"), fmt, 0, fse_inv_arg);   // '-' followed by no characters
        test_from_string<I, C>(LIT("-@1"), fmt, 0, fse_inv_arg); // '-' followed by bogus '@'
        test_from_string<I, C>(LIT("-.1"), fmt, 0, fse_inv_arg); // '-' followed by bogus '.'
        test_from_string<I, C>(LIT("-+1"), fmt, 0, fse_inv_arg); // '-' followed by bogus '+'
        test_from_string<I, C>(LIT("- 1"), fmt, 0, fse_inv_arg); // '-' followed by bogus ' '
        test_from_string<I, C>(LIT("--1"), fmt, 0, fse_inv_arg); // '-' can't be repeated

        // Leading '+' is valid in this implementation (unlike std::from_string), so the same
        // "no digits following the sign" / "sign can't repeat or combine" rules apply to it.
        test_from_string<I, C>(LIT("+"), fmt, 0, fse_inv_arg);   // '+' followed by no characters
        test_from_string<I, C>(LIT("+@1"), fmt, 0, fse_inv_arg); // '+' followed by bogus '@'
        test_from_string<I, C>(LIT("+.1"), fmt, 0, fse_inv_arg); // '+' followed by bogus '.'
        test_from_string<I, C>(LIT("+-1"), fmt, 0, fse_inv_arg); // '+' followed by bogus '-'
        test_from_string<I, C>(LIT("+ 1"), fmt, 0, fse_inv_arg); // '+' followed by bogus ' '
        test_from_string<I, C>(LIT("++1"), fmt, 0, fse_inv_arg); // '+' can't be repeated

        vx::array<C, 3> bogus_digits;
        size_t bogus_digit_count;

        if (base < 10)
        {
            bogus_digits = { static_cast<C>('0' + base), 'A', 'a' };
            bogus_digit_count = 3;
        }
        else
        {
            // '[' and '{' are bogus for base 36
            bogus_digits = { static_cast<C>('A' + (base - 10)), static_cast<C>('a' + (base - 10)) };
            bogus_digit_count = 2;
        }

        for (size_t i = 0; i < bogus_digit_count; ++i)
        {
            const C bogus = bogus_digits[i];
            test_from_string<I, C>(string_t(1, bogus) + LIT("1"), fmt, 0, fse_inv_arg);         // bogus digit (for this base)
            test_from_string<I, C>(string_t(LIT("-")) + bogus + LIT("1"), fmt, 0, fse_inv_arg); // '-' followed by bogus digit
            test_from_string<I, C>(string_t(LIT("+")) + bogus + LIT("1"), fmt, 0, fse_inv_arg); // '+' followed by bogus digit
        }

        // Test leading zeroes.
        test_from_string<I, C>(hundred_zeroes, fmt, 100, fse_none, static_cast<I>(0));
        test_from_string<I, C>(hundred_zeroes_and_11, fmt, 102, fse_none, static_cast<I>(base + 1));

        // Test negative zero and negative leading zeroes.
        VX_IF_CONSTEXPR (std::is_signed<I>::value)
        {
            test_from_string<I, C>(LIT("-0"), fmt, 2, fse_none, static_cast<I>(0));
            test_from_string<I, C>(minus_hundred_zeroes, fmt, 101, fse_none, static_cast<I>(0));
            test_from_string<I, C>(minus_hundred_zeroes_and_11, fmt, 103, fse_none, static_cast<I>(-base - 1));
        }
        else
        {
            test_from_string<I, C>(LIT("-0"), fmt, 0, fse_inv_arg);
            test_from_string<I, C>(minus_hundred_zeroes, fmt, 0, fse_inv_arg);
            test_from_string<I, C>(minus_hundred_zeroes_and_11, fmt, 0, fse_inv_arg);
        }

        // Test explicit positive zero and positive leading zeroes. Valid for both signed and
        // unsigned types, since '+' never changes the sign of the parsed value.
        test_from_string<I, C>(LIT("+0"), fmt, 2, fse_none, static_cast<I>(0));
        test_from_string<I, C>(plus_hundred_zeroes, fmt, 101, fse_none, static_cast<I>(0));
        test_from_string<I, C>(plus_hundred_zeroes_and_11, fmt, 103, fse_none, static_cast<I>(base + 1));


        // N4713 23.20.3 [charconv.from.chars]/1 "The member ptr of the return value points to the
        // first character not matching the pattern, or has the value last if all characters match."
        test_from_string<I, C>(LIT("11"), fmt, 2, fse_none, static_cast<I>(base + 1));
        test_from_string<I, C>(LIT("11@@@"), fmt, 2, fse_none, static_cast<I>(base + 1));
        test_from_string<I, C>(LIT("+11"), fmt, 3, fse_none, static_cast<I>(base + 1));
        test_from_string<I, C>(LIT("+11@@@"), fmt, 3, fse_none, static_cast<I>(base + 1));

        // When overflowing, we need to keep consuming valid digits, in order to return ptr correctly.
        test_from_string<I, C>(hundred_ones, fmt, 100, fse_out_ran);
        test_from_string<I, C>(hundred_ones_and_atatat, fmt, 100, fse_out_ran);
        test_from_string<I, C>(plus_hundred_ones, fmt, 101, fse_out_ran);
        test_from_string<I, C>(plus_hundred_ones_and_atatat, fmt, 101, fse_out_ran);

        VX_IF_CONSTEXPR (std::is_signed<I>::value)
        {
            test_from_string<I, C>(minus_hundred_ones, fmt, 101, fse_out_ran);
            test_from_string<I, C>(minus_hundred_ones_and_atatat, fmt, 101, fse_out_ran);
        }
        else
        {
            test_from_string<I, C>(minus_hundred_ones, fmt, 0, fse_inv_arg);
            test_from_string<I, C>(minus_hundred_ones_and_atatat, fmt, 0, fse_inv_arg);
        }
    }

    // N4713 23.20.3 [charconv.from.chars]/3 "The pattern is the expected form of the subject sequence
    // in the "C" locale for the given nonzero base, as described for strtol"
    // C11 7.22.1.4/3 "The letters from a (or A) through z (or Z) are ascribed the values 10 through 35"
    fmt.base = 36;
    for (int i = 0; i < 26; ++i)
    {
        test_from_string<I, C>(string_t(1, static_cast<C>('A' + i)), fmt, 1, fse_none, static_cast<I>(10 + i));
        test_from_string<I, C>(string_t(1, static_cast<C>('a' + i)), fmt, 1, fse_none, static_cast<I>(10 + i));
    }

    // N4713 23.20.3 [charconv.from.chars]/3 "no "0x" or "0X" prefix shall appear if the value of base is 16"
    fmt.base = 16;
    test_from_string<I, C>(LIT("0x1729"), fmt, 1, fse_none, static_cast<I>(0));  // reads '0', stops at 'x'
    test_from_string<I, C>(LIT("0X1729"), fmt, 1, fse_none, static_cast<I>(0));  // reads '0', stops at 'X'
    test_from_string<I, C>(LIT("+0x1729"), fmt, 2, fse_none, static_cast<I>(0)); // reads "+0", stops at 'x'
    test_from_string<I, C>(LIT("+0X1729"), fmt, 2, fse_none, static_cast<I>(0)); // reads "+0", stops at 'X'

    VX_IF_CONSTEXPR (std::is_signed<I>::value)
    {
        test_from_string<I, C>(LIT("-0x1729"), fmt, 2, fse_none, static_cast<I>(0)); // reads "-0", stops at 'x'
        test_from_string<I, C>(LIT("-0X1729"), fmt, 2, fse_none, static_cast<I>(0)); // reads "-0", stops at 'X'
    }
    else
    {
        test_from_string<I, C>(LIT("-0x1729"), fmt, 0, fse_inv_arg);
        test_from_string<I, C>(LIT("-0X1729"), fmt, 0, fse_inv_arg);
    }

    return true;
}

template <typename C>
void test_integer_overflow_scenarios()
{
    str::integer_format_options fmt;

    // Test overflow scenarios.
    test_from_string<unsigned int, C>(LIT("4294967289"), fmt, 10, fse_none, 4294967289U); // not risky
    test_from_string<unsigned int, C>(LIT("4294967294"), fmt, 10, fse_none, 4294967294U); // risky with good digit
    test_from_string<unsigned int, C>(LIT("4294967295"), fmt, 10, fse_none, 4294967295U); // risky with max digit
    test_from_string<unsigned int, C>(LIT("4294967296"), fmt, 10, fse_out_ran);           // risky with bad digit
    test_from_string<unsigned int, C>(LIT("4294967300"), fmt, 10, fse_out_ran);           // beyond risky

    test_from_string<int, C>(LIT("2147483639"), fmt, 10, fse_none, 2147483639); // not risky
    test_from_string<int, C>(LIT("2147483646"), fmt, 10, fse_none, 2147483646); // risky with good digit
    test_from_string<int, C>(LIT("2147483647"), fmt, 10, fse_none, 2147483647); // risky with max digit
    test_from_string<int, C>(LIT("2147483648"), fmt, 10, fse_out_ran);          // risky with bad digit
    test_from_string<int, C>(LIT("2147483650"), fmt, 10, fse_out_ran);          // beyond risky

    test_from_string<int, C>(LIT("-2147483639"), fmt, 11, fse_none, -2147483639);     // not risky
    test_from_string<int, C>(LIT("-2147483647"), fmt, 11, fse_none, -2147483647);     // risky with good digit
    test_from_string<int, C>(LIT("-2147483648"), fmt, 11, fse_none, -2147483647 - 1); // risky with max digit
    test_from_string<int, C>(LIT("-2147483649"), fmt, 11, fse_out_ran);               // risky with bad digit
    test_from_string<int, C>(LIT("-2147483650"), fmt, 11, fse_out_ran);               // beyond risky
}

//==============================================================================

template <typename I, typename C>
void test_integer_type()
{
    test_integer_to_string<I, C>();
    test_integer_from_string<I, C>();
    test_integer_overflow_scenarios<C>();
}

template <typename I>
void test_integer()
{
    test_integer_type<I, char>();
    test_integer_type<I, wchar_t>();
#if defined(__cpp_lib_char8_t)
    test_integer_type<I, char8_t>();
#endif
    test_integer_type<I, char16_t>();
    test_integer_type<I, char32_t>();
}

VX_TEST_CASE(test_all_integer)
{
    //test_integer<char>();
    //test_integer<signed char>();
    //test_integer<unsigned char>();
    //test_integer<short>();
    //test_integer<unsigned short>();
    //test_integer<int>();
    //test_integer<unsigned int>();
    //test_integer<long>();
    //test_integer<unsigned long>();
    //test_integer<long long>();
    //test_integer<unsigned long long>();
}

//==============================================================================

template <typename F, typename C>
void test_float_from_string(const str::float_format format)
{
    str::float_from_string_format_options<C> fmt;
    fmt.format = format;

    using string_t = str::basic_string<C>;

    test_from_string<F, C>(LIT(""), fmt, 0, fse_inv_arg);   // no characters
    test_from_string<F, C>(LIT("@1"), fmt, 0, fse_inv_arg); // '@' is bogus
    test_from_string<F, C>(LIT("z1"), fmt, 0, fse_inv_arg); // 'z' is bogus
    test_from_string<F, C>(LIT("."), fmt, 0, fse_inv_arg);  // '.' without digits is bogus
    test_from_string<F, C>(LIT(" 1"), fmt, 0, fse_inv_arg); // ' ' is bogus
    test_from_string<F, C>(LIT("p5"), fmt, 0, fse_inv_arg); // binary-exponent-part without digits is bogus
    test_from_string<F, C>(LIT("in"), fmt, 0, fse_inv_arg); // incomplete inf is bogus
    test_from_string<F, C>(LIT("na"), fmt, 0, fse_inv_arg); // incomplete nan is bogus

    test_from_string<F, C>(LIT("-"), fmt, 0, fse_inv_arg);   // '-' followed by no characters
    test_from_string<F, C>(LIT("-@1"), fmt, 0, fse_inv_arg); // '-' followed by bogus '@'
    test_from_string<F, C>(LIT("-z1"), fmt, 0, fse_inv_arg); // '-' followed by bogus 'z'
    test_from_string<F, C>(LIT("-."), fmt, 0, fse_inv_arg);  // '-' followed by bogus '.'
    test_from_string<F, C>(LIT("-+1"), fmt, 0, fse_inv_arg); // '-' followed by bogus '+'
    test_from_string<F, C>(LIT("- 1"), fmt, 0, fse_inv_arg); // '-' followed by bogus ' '
    test_from_string<F, C>(LIT("-p5"), fmt, 0, fse_inv_arg); // '-' followed by bogus binary-exponent-part
    test_from_string<F, C>(LIT("-in"), fmt, 0, fse_inv_arg); // '-' followed by bogus incomplete inf
    test_from_string<F, C>(LIT("-na"), fmt, 0, fse_inv_arg); // '-' followed by bogus incomplete nan
    test_from_string<F, C>(LIT("--1"), fmt, 0, fse_inv_arg); // '-' can't be repeated

    // Leading '+' is valid in this implementation (unlike std::from_chars), so the same
    // "no digits following the sign" / "sign can't repeat or combine" rules apply to it.
    test_from_string<F, C>(LIT("+"), fmt, 0, fse_inv_arg);   // '+' followed by no characters
    test_from_string<F, C>(LIT("+@1"), fmt, 0, fse_inv_arg); // '+' followed by bogus '@'
    test_from_string<F, C>(LIT("+z1"), fmt, 0, fse_inv_arg); // '+' followed by bogus 'z'
    test_from_string<F, C>(LIT("+."), fmt, 0, fse_inv_arg);  // '+' followed by bogus '.'
    test_from_string<F, C>(LIT("+-1"), fmt, 0, fse_inv_arg); // '+' followed by bogus '-'
    test_from_string<F, C>(LIT("+ 1"), fmt, 0, fse_inv_arg); // '+' followed by bogus ' '
    test_from_string<F, C>(LIT("+p5"), fmt, 0, fse_inv_arg); // '+' followed by bogus binary-exponent-part
    test_from_string<F, C>(LIT("+in"), fmt, 0, fse_inv_arg); // '+' followed by bogus incomplete inf
    test_from_string<F, C>(LIT("+na"), fmt, 0, fse_inv_arg); // '+' followed by bogus incomplete nan
    test_from_string<F, C>(LIT("++1"), fmt, 0, fse_inv_arg); // '+' can't be repeated

    if (fmt.format != str::float_format::hex)
    {                                                            // "e5" are valid hexits
        test_from_string<F, C>(LIT("e5"), fmt, 0, fse_inv_arg);  // exponent-part without digits is bogus
        test_from_string<F, C>(LIT("-e5"), fmt, 0, fse_inv_arg); // '-' followed by bogus exponent-part
        test_from_string<F, C>(LIT("+e5"), fmt, 0, fse_inv_arg); // '+' followed by bogus exponent-part
    }

    constexpr F inf = std::numeric_limits<F>::infinity();
    constexpr F qnan = std::numeric_limits<F>::quiet_NaN();

    test_from_string<F, C>(LIT("InF"), fmt, 3, fse_none, inf);
    test_from_string<F, C>(LIT("infinite"), fmt, 3, fse_none, inf);
    test_from_string<F, C>(LIT("iNfInItY"), fmt, 8, fse_none, inf);
    test_from_string<F, C>(LIT("InfinityMeow"), fmt, 8, fse_none, inf);

    test_from_string<F, C>(LIT("-InF"), fmt, 4, fse_none, -inf);
    test_from_string<F, C>(LIT("-infinite"), fmt, 4, fse_none, -inf);
    test_from_string<F, C>(LIT("-iNfInItY"), fmt, 9, fse_none, -inf);
    test_from_string<F, C>(LIT("-InfinityMeow"), fmt, 9, fse_none, -inf);

    test_from_string<F, C>(LIT("+InF"), fmt, 4, fse_none, inf);
    test_from_string<F, C>(LIT("+infinite"), fmt, 4, fse_none, inf);
    test_from_string<F, C>(LIT("+iNfInItY"), fmt, 9, fse_none, inf);
    test_from_string<F, C>(LIT("+InfinityMeow"), fmt, 9, fse_none, inf);

    test_from_string<F, C>(LIT("NaN"), fmt, 3, fse_none, qnan);
    test_from_string<F, C>(LIT("nanotech"), fmt, 3, fse_none, qnan);
    test_from_string<F, C>(LIT("nan("), fmt, 3, fse_none, qnan);
    test_from_string<F, C>(LIT("nan(@)"), fmt, 3, fse_none, qnan);
    test_from_string<F, C>(LIT("nan(()"), fmt, 3, fse_none, qnan);
    test_from_string<F, C>(LIT("nan(abc"), fmt, 3, fse_none, qnan);
    test_from_string<F, C>(LIT("nan()"), fmt, 5, fse_none, qnan);
    test_from_string<F, C>(LIT("nan(abc)def"), fmt, 8, fse_none, qnan);
    test_from_string<F, C>(LIT("nan(_09AZaz)"), fmt, 12, fse_none, qnan);
    test_from_string<F, C>(LIT("nan(int)"), fmt, 8, fse_none, qnan);
    test_from_string<F, C>(LIT("nan(snap)"), fmt, 9, fse_none, qnan);

    test_from_string<F, C>(LIT("-NaN"), fmt, 4, fse_none, -qnan);
    test_from_string<F, C>(LIT("-nanotech"), fmt, 4, fse_none, -qnan);
    test_from_string<F, C>(LIT("-nan("), fmt, 4, fse_none, -qnan);
    test_from_string<F, C>(LIT("-nan(@)"), fmt, 4, fse_none, -qnan);
    test_from_string<F, C>(LIT("-nan(()"), fmt, 4, fse_none, -qnan);
    test_from_string<F, C>(LIT("-nan(abc"), fmt, 4, fse_none, -qnan);
    test_from_string<F, C>(LIT("-nan()"), fmt, 6, fse_none, -qnan);
    test_from_string<F, C>(LIT("-nan(abc)def"), fmt, 9, fse_none, -qnan);
    test_from_string<F, C>(LIT("-nan(_09AZaz)"), fmt, 13, fse_none, -qnan);
    test_from_string<F, C>(LIT("-nan(int)"), fmt, 9, fse_none, -qnan);
    test_from_string<F, C>(LIT("-nan(snap)"), fmt, 10, fse_none, -qnan);

    test_from_string<F, C>(LIT("+NaN"), fmt, 4, fse_none, qnan);
    test_from_string<F, C>(LIT("+nanotech"), fmt, 4, fse_none, qnan);
    test_from_string<F, C>(LIT("+nan("), fmt, 4, fse_none, qnan);
    test_from_string<F, C>(LIT("+nan(@)"), fmt, 4, fse_none, qnan);
    test_from_string<F, C>(LIT("+nan(()"), fmt, 4, fse_none, qnan);
    test_from_string<F, C>(LIT("+nan(abc"), fmt, 4, fse_none, qnan);
    test_from_string<F, C>(LIT("+nan()"), fmt, 6, fse_none, qnan);
    test_from_string<F, C>(LIT("+nan(abc)def"), fmt, 9, fse_none, qnan);
    test_from_string<F, C>(LIT("+nan(_09AZaz)"), fmt, 13, fse_none, qnan);
    test_from_string<F, C>(LIT("+nan(int)"), fmt, 9, fse_none, qnan);
    test_from_string<F, C>(LIT("+nan(snap)"), fmt, 10, fse_none, qnan);

    // Fhe UCRF considers indeterminate NaN to be negative quiet NaN with no payload bits set.
    // It parses "nan(ind)", "-nan(ind)", and "+nan(ind)" identically.
    test_from_string<F, C>(LIT("nan(InD)"), fmt, 8, fse_none, -qnan);
    test_from_string<F, C>(LIT("-nan(InD)"), fmt, 9, fse_none, -qnan);
    test_from_string<F, C>(LIT("+nan(InD)"), fmt, 9, fse_none, -qnan);

    test_from_string<F, C>(LIT("nan(SnAn)"), fmt, 9, fse_none, optional<F>{}, test_from_string_mode::signaling_nan);
    test_from_string<F, C>(LIT("-nan(SnAn)"), fmt, 10, fse_none, optional<F>{}, test_from_string_mode::signaling_nan);
    test_from_string<F, C>(LIT("+nan(SnAn)"), fmt, 10, fse_none, optional<F>{}, test_from_string_mode::signaling_nan);

    switch (fmt.format)
    {
        case str::float_format::general:
        {
            test_from_string<F, C>(LIT("1729"), fmt, 4, fse_none, F{ 1729 });
            test_from_string<F, C>(LIT("1729e3"), fmt, 6, fse_none, F{ 1729000 });
            test_from_string<F, C>(LIT("10"), fmt, 2, fse_none, F{ 10 });
            test_from_string<F, C>(LIT("11."), fmt, 3, fse_none, F{ 11 });
            test_from_string<F, C>(LIT("12.13"), fmt, 5, fse_none, static_cast<F>(12.13)); // avoid truncation warning
            test_from_string<F, C>(LIT(".14"), fmt, 3, fse_none, static_cast<F>(.14));     // avoid truncation warning
            test_from_string<F, C>(LIT("20e5"), fmt, 4, fse_none, F{ 2000000 });
            test_from_string<F, C>(LIT("21.e5"), fmt, 5, fse_none, F{ 2100000 });
            test_from_string<F, C>(LIT("22.23e5"), fmt, 7, fse_none, F{ 2223000 });
            test_from_string<F, C>(LIT(".24e5"), fmt, 5, fse_none, F{ 24000 });
            test_from_string<F, C>(LIT("33e+5"), fmt, 5, fse_none, F{ 3300000 });
            test_from_string<F, C>(LIT("33e-5"), fmt, 5, fse_none, static_cast<F>(.00033)); // avoid truncation warning
            test_from_string<F, C>(LIT("4E7"), fmt, 3, fse_none, F{ 40000000 });
            test_from_string<F, C>(LIT("-00123abc"), fmt, 6, fse_none, F{ -123 });
            test_from_string<F, C>(LIT("+00123abc"), fmt, 6, fse_none, F{ 123 });
            test_from_string<F, C>(LIT(".0045000"), fmt, 8, fse_none, static_cast<F>(.0045)); // avoid truncation warning
            test_from_string<F, C>(LIT("000"), fmt, 3, fse_none, F{ 0 });
            test_from_string<F, C>(LIT("0e9999"), fmt, 6, fse_none, F{ 0 });
            test_from_string<F, C>(LIT("0e-9999"), fmt, 7, fse_none, F{ 0 });
            test_from_string<F, C>(LIT("-000"), fmt, 4, fse_none, F{ -0.0 });
            test_from_string<F, C>(LIT("-0e9999"), fmt, 7, fse_none, F{ -0.0 });
            test_from_string<F, C>(LIT("-0e-9999"), fmt, 8, fse_none, F{ -0.0 });
            test_from_string<F, C>(LIT("+000"), fmt, 4, fse_none, F{ 0 });
            test_from_string<F, C>(LIT("+0e9999"), fmt, 7, fse_none, F{ 0 });
            test_from_string<F, C>(LIT("+0e-9999"), fmt, 8, fse_none, F{ 0 });
            test_from_string<F, C>(string_t(LIT("1")) + string_t(10000, '0') + LIT("e-10000"), fmt, 10008, fse_none, F{ 1.0 });
            test_from_string<F, C>(string_t(LIT("0.")) + string_t(9999, '0') + LIT("1e10000"), fmt, 10008, fse_none, F{ 1.0 });
            test_from_string<F, C>(string_t(LIT("1")) + string_t(1280000, '0') + LIT("e-1280000"), fmt, 1280010, fse_none, F{ 1.0 });
            test_from_string<F, C>(string_t(LIT("0.")) + string_t(1279999, '0') + LIT("1e1280000"), fmt, 1280010, fse_none, F{ 1.0 });
            test_from_string<F, C>(LIT("1e9999"), fmt, 6, fse_out_ran, inf);
            test_from_string<F, C>(LIT("-1e9999"), fmt, 7, fse_out_ran, -inf);
            test_from_string<F, C>(LIT("+1e9999"), fmt, 7, fse_out_ran, inf);
            test_from_string<F, C>(LIT("1e-9999"), fmt, 7, fse_out_ran, F{ 0 });
            test_from_string<F, C>(LIT("-1e-9999"), fmt, 8, fse_out_ran, F{ -0.0 });
            test_from_string<F, C>(LIT("+1e-9999"), fmt, 8, fse_out_ran, F{ 0 });
            test_from_string<F, C>(string_t(LIT("1")) + string_t(6000, '0'), fmt, 6001, fse_out_ran, inf);
            test_from_string<F, C>(string_t(LIT("-1")) + string_t(6000, '0'), fmt, 6002, fse_out_ran, -inf);
            test_from_string<F, C>(string_t(LIT("+1")) + string_t(6000, '0'), fmt, 6002, fse_out_ran, inf);
            test_from_string<F, C>(string_t(LIT(".")) + string_t(6000, '0') + LIT("1"), fmt, 6002, fse_out_ran, F{ 0 });
            test_from_string<F, C>(string_t(LIT("-.")) + string_t(6000, '0') + LIT("1"), fmt, 6003, fse_out_ran, F{ -0.0 });
            test_from_string<F, C>(string_t(LIT("+.")) + string_t(6000, '0') + LIT("1"), fmt, 6003, fse_out_ran, F{ 0 });
            test_from_string<F, C>(string_t(LIT("1")) + string_t(500, '0'), fmt, 501, fse_out_ran, inf);
            test_from_string<F, C>(string_t(LIT("-1")) + string_t(500, '0'), fmt, 502, fse_out_ran, -inf);
            test_from_string<F, C>(string_t(LIT("+1")) + string_t(500, '0'), fmt, 502, fse_out_ran, inf);
            test_from_string<F, C>(string_t(LIT(".")) + string_t(500, '0') + LIT("1"), fmt, 502, fse_out_ran, F{ 0 });
            test_from_string<F, C>(string_t(LIT("-.")) + string_t(500, '0') + LIT("1"), fmt, 503, fse_out_ran, F{ -0.0 });
            test_from_string<F, C>(string_t(LIT("+.")) + string_t(500, '0') + LIT("1"), fmt, 503, fse_out_ran, F{ 0 });
            break;
        }
        case str::float_format::scientific:
        {
            test_from_string<F, C>(LIT("1729"), fmt, 0, fse_inv_arg);
            test_from_string<F, C>(LIT("1729e3"), fmt, 6, fse_none, F{ 1729000 });
            break;
        }
        case str::float_format::fixed:
        {
            test_from_string<F, C>(LIT("1729"), fmt, 4, fse_none, F{ 1729 });
            test_from_string<F, C>(LIT("1729e3"), fmt, 4, fse_none, F{ 1729 });
            break;
        }
        case str::float_format::hex:
        {
            test_from_string<F, C>(LIT("0x123"), fmt, 1, fse_none, F{ 0 });
            test_from_string<F, C>(LIT("a0"), fmt, 2, fse_none, F{ 160 });
            test_from_string<F, C>(LIT("a1."), fmt, 3, fse_none, F{ 161 });
            test_from_string<F, C>(LIT("a2.a3"), fmt, 5, fse_none, F{ 162.63671875 });
            test_from_string<F, C>(LIT(".a4"), fmt, 3, fse_none, F{ 0.640625 });
            test_from_string<F, C>(LIT("a0p5"), fmt, 4, fse_none, F{ 5120 });
            test_from_string<F, C>(LIT("a1.p5"), fmt, 5, fse_none, F{ 5152 });
            test_from_string<F, C>(LIT("a2.a3p5"), fmt, 7, fse_none, F{ 5204.375 });
            test_from_string<F, C>(LIT(".a4p5"), fmt, 5, fse_none, F{ 20.5 });
            test_from_string<F, C>(LIT("a0p+5"), fmt, 5, fse_none, F{ 5120 });
            test_from_string<F, C>(LIT("a0p-5"), fmt, 5, fse_none, F{ 5 });
            test_from_string<F, C>(LIT("ABCDEFP3"), fmt, 8, fse_none, F{ 90075000 });
            test_from_string<F, C>(LIT("-00cdrom"), fmt, 5, fse_none, F{ -205 });
            test_from_string<F, C>(LIT("+00cdrom"), fmt, 5, fse_none, F{ 205 });
            test_from_string<F, C>(LIT(".00ef000"), fmt, 8, fse_none, F{ 0.0036468505859375 });
            test_from_string<F, C>(LIT("000"), fmt, 3, fse_none, F{ 0 });
            test_from_string<F, C>(LIT("0p9999"), fmt, 6, fse_none, F{ 0 });
            test_from_string<F, C>(LIT("0p-9999"), fmt, 7, fse_none, F{ 0 });
            test_from_string<F, C>(LIT("-000"), fmt, 4, fse_none, F{ -0.0 });
            test_from_string<F, C>(LIT("-0p9999"), fmt, 7, fse_none, F{ -0.0 });
            test_from_string<F, C>(LIT("-0p-9999"), fmt, 8, fse_none, F{ -0.0 });
            test_from_string<F, C>(LIT("+000"), fmt, 4, fse_none, F{ 0 });
            test_from_string<F, C>(LIT("+0p9999"), fmt, 7, fse_none, F{ 0 });
            test_from_string<F, C>(LIT("+0p-9999"), fmt, 8, fse_none, F{ 0 });
            test_from_string<F, C>(string_t(LIT("1")) + string_t(2500, '0') + LIT("p-10000"), fmt, 2508, fse_none, F{ 1.0 });
            test_from_string<F, C>(string_t(LIT("0.")) + string_t(2499, '0') + LIT("1p10000"), fmt, 2508, fse_none, F{ 1.0 });
            test_from_string<F, C>(string_t(LIT("1")) + string_t(320000, '0') + LIT("p-1280000"), fmt, 320010, fse_none, F{ 1.0 });
            test_from_string<F, C>(string_t(LIT("0.")) + string_t(319999, '0') + LIT("1p1280000"), fmt, 320010, fse_none, F{ 1.0 });
            test_from_string<F, C>(string_t(LIT("1p9999")), fmt, 6, fse_out_ran, inf);
            test_from_string<F, C>(string_t(LIT("-1p9999")), fmt, 7, fse_out_ran, -inf);
            test_from_string<F, C>(string_t(LIT("+1p9999")), fmt, 7, fse_out_ran, inf);
            test_from_string<F, C>(string_t(LIT("1p-9999")), fmt, 7, fse_out_ran, F{ 0 });
            test_from_string<F, C>(string_t(LIT("-1p-9999")), fmt, 8, fse_out_ran, F{ -0.0 });
            test_from_string<F, C>(string_t(LIT("+1p-9999")), fmt, 8, fse_out_ran, F{ 0 });
            test_from_string<F, C>(string_t(LIT("1")) + string_t(2000, '0'), fmt, 2001, fse_out_ran, inf);
            test_from_string<F, C>(string_t(LIT("-1")) + string_t(2000, '0'), fmt, 2002, fse_out_ran, -inf);
            test_from_string<F, C>(string_t(LIT("+1")) + string_t(2000, '0'), fmt, 2002, fse_out_ran, inf);
            test_from_string<F, C>(string_t(LIT(".")) + string_t(2000, '0') + LIT("1"), fmt, 2002, fse_out_ran, F{ 0 });
            test_from_string<F, C>(string_t(LIT("-.")) + string_t(2000, '0') + LIT("1"), fmt, 2003, fse_out_ran, F{ -0.0 });
            test_from_string<F, C>(string_t(LIT("+.")) + string_t(2000, '0') + LIT("1"), fmt, 2003, fse_out_ran, F{ 0 });
            test_from_string<F, C>(string_t(LIT("1")) + string_t(300, '0'), fmt, 301, fse_out_ran, inf);
            test_from_string<F, C>(string_t(LIT("-1")) + string_t(300, '0'), fmt, 302, fse_out_ran, -inf);
            test_from_string<F, C>(string_t(LIT("+1")) + string_t(300, '0'), fmt, 302, fse_out_ran, inf);
            test_from_string<F, C>(string_t(LIT(".")) + string_t(300, '0') + LIT("1"), fmt, 302, fse_out_ran, F{ 0 });
            test_from_string<F, C>(string_t(LIT("-.")) + string_t(300, '0') + LIT("1"), fmt, 303, fse_out_ran, F{ -0.0 });
            test_from_string<F, C>(string_t(LIT("+.")) + string_t(300, '0') + LIT("1"), fmt, 303, fse_out_ran, F{ 0 });
            break;
        }
    }
}

//==============================================================================

template <typename F, typename C, size_t N>
void run_float_test_cases( const float_to_string_test_case<F, char> (&cases)[N])
{
    for (const auto& t : cases)
    {
        const auto tmp = str::string_cast<C>(t.correct);
        const str::float_to_string_format_options<C> fmt{ t.fmt.format, t.fmt.precision };
        test_common_to_string<F, C>(t.value, fmt, tmp);
    }
}

template <typename F>
struct float_tester_data;

template <>
struct float_tester_data<float>
{
    static constexpr float lwg_2403_correct = 0x1.fffffep0f;

    static inline constexpr auto& from_string_test_cases_1 = float_from_string_test_cases;
    static inline constexpr auto& from_string_test_cases_2 = floating_point_test_cases_float;

    static inline constexpr auto& to_string_test_cases_base = float_to_string_test_cases;

    static inline constexpr auto& to_string_test_cases_hex = float_hex_to_string_test_cases;
    static inline constexpr auto& to_string_test_cases_fixed = float_fixed_to_string_test_cases;
    static inline constexpr auto& to_string_test_cases_scientific = float_scientific_to_string_test_cases;

    template <typename C>
    static void test_to_string_fixed_cases()
    {
        run_float_test_cases<float, C>(to_string_test_cases_fixed);
    }

    template <typename C>
    static void test_to_string_scientific_cases()
    {
        run_float_test_cases<float, C>(to_string_test_cases_scientific);
    }
};

template <>
struct float_tester_data<double>
{
    static constexpr double lwg_2403_correct = 0x1.fffffdp0;

    static inline constexpr auto& from_string_test_cases_1 = double_from_string_test_cases;
    static inline constexpr auto& from_string_test_cases_2 = floating_point_test_cases_double;

    static inline constexpr auto& to_string_test_cases_base = double_to_string_test_cases;

    static inline constexpr auto& to_string_test_cases_hex = double_hex_to_string_test_cases;

    static inline constexpr auto& to_string_test_cases_fixed_1 = double_fixed_to_string_test_cases_1;
    static inline constexpr auto& to_string_test_cases_fixed_2 = double_fixed_to_string_test_cases_2;
    static inline constexpr auto& to_string_test_cases_fixed_3 = double_fixed_to_string_test_cases_3;
    static inline constexpr auto& to_string_test_cases_fixed_4 = double_fixed_to_string_test_cases_4;

    static inline constexpr auto& to_string_test_cases_scientific_1 = double_scientific_to_string_test_cases_1;
    static inline constexpr auto& to_string_test_cases_scientific_2 = double_scientific_to_string_test_cases_2;
    static inline constexpr auto& to_string_test_cases_scientific_3 = double_scientific_to_string_test_cases_3;
    static inline constexpr auto& to_string_test_cases_scientific_4 = double_scientific_to_string_test_cases_4;

    template <typename C>
    static void test_to_string_fixed_cases()
    {
        run_float_test_cases<double, C>(to_string_test_cases_fixed_1);
        run_float_test_cases<double, C>(to_string_test_cases_fixed_2);
        run_float_test_cases<double, C>(to_string_test_cases_fixed_3);
        run_float_test_cases<double, C>(to_string_test_cases_fixed_4);
    }

    template <typename C>
    static void test_to_string_scientific_cases()
    {
        run_float_test_cases<double, C>(to_string_test_cases_scientific_1);
        run_float_test_cases<double, C>(to_string_test_cases_scientific_2);
        run_float_test_cases<double, C>(to_string_test_cases_scientific_3);
        run_float_test_cases<double, C>(to_string_test_cases_scientific_4);
    }
};

template <typename F, typename C>
void test_float_type()
{
    VX_SECTION("general")
    {
        const str::float_format formats[] = {
            str::float_format::general,
            str::float_format::scientific,
            str::float_format::fixed,
            str::float_format::hex
        };

        for (const auto& fmt : formats)
        {
            test_float_from_string<F, C>(fmt);
        }
    }

    using tester_data = float_tester_data<F>;
    const str::float_from_string_format_options<C> general_fmt{ str::float_format::general };

    VX_SECTION("from string rounding")
    {
        // See float_from_chars_test_cases.hpp in this directory.
        for (const auto& t : tester_data::from_string_test_cases_1)
        {
            const auto tmp = str::string_cast<C>(t.input);
            const str::float_from_string_format_options<C> fmt{ t.fmt.format, static_cast<C>(t.fmt.decimal_point) };
            test_from_string<F, C>(tmp, fmt, t.correct_count, t.correct_err, t.correct_value);
        }

        {
            // See LWG-2403. This number (exactly 0x1.fffffd00000004 in infinite precision) behaves differently
            // when parsed as double and converted to float, versus being parsed as float directly.
            const C* const lwg_2403 = LIT("1.999999821186065729339276231257827021181583404541015625");
            constexpr F correct_value = tester_data::lwg_2403_correct;
            constexpr float twice_rounded_float = 0x1.fffffcp0f;
            test_from_string<F, C>(lwg_2403, general_fmt, 56, fse_none, correct_value);

            VX_IF_CONSTEXPR (std::is_same<F, double>::value)
            {
                VX_STATIC_ASSERT(static_cast<float>(correct_value) == twice_rounded_float);
            }
        }
    }

    VX_SECTION("from string")
    {
        for (const auto& p : tester_data::from_string_test_cases_2)
        {
            const auto tmp = str::string_cast<C>(p.first);
            test_from_string<F, C>(tmp, general_fmt, tmp.size(), fse_none, bit::bit_cast<F>(p.second));
        }
    }

    VX_SECTION("to string base")
    {
        run_float_test_cases<F, C>(tester_data::to_string_test_cases_base);
    }
    VX_SECTION("to string hex")
    {
        run_float_test_cases<F, C>(tester_data::to_string_test_cases_hex);
    }
    VX_SECTION("to string fixed")
    {
        tester_data::template test_to_string_fixed_cases<C>();
    }
    VX_SECTION("to string scientific")
    {
        tester_data::template test_to_string_scientific_cases<C>();
    }
}

template <typename F>
void test_float()
{
    test_float_type<F, char>();
    test_float_type<F, wchar_t>();
#if defined(__cpp_lib_char8_t)
    test_float_type<F, char8_t>();
#endif
    test_float_type<F, char16_t>();
    test_float_type<F, char32_t>();
}

VX_TEST_CASE(test_all_float)
{
    VX_MESSAGE("float");
    {
        test_float<float>();
    }
    VX_MESSAGE("double");
    {
        test_float<double>();
    }
}

//==============================================================================

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}
