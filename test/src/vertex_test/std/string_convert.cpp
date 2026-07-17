#include <cstdint>

#include "vertex/std/string_convert.hpp"
#include "vertex/std/string_utils.hpp"
#include "vertex_test/test.hpp"
#include "vertex_test/integer_to_string_test_data.hpp"

using namespace vx;

#define LIT(x)          VX_LIT(C, x)
#define NUM(x)          static_cast<I>(x)
#define CHECK_STR(a, b) VX_CHECK(::vx::str::compare(a, b) == 0)

#define PRINT_RESULT 1

// https://github.com/microsoft/STL/blob/020513e211529e7be30cb3e0ca310869701286da/tests/std/tests/P0067R5_charconv/test.cpp#L1012

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

template <typename I, typename C, typename FMT>
void test_common_to_string(const I value, const FMT& fmt, const C* correct)
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

    constexpr size_t space = std::is_integral<I>::value
        ? 1 + 64 // worst case: -2^63 in binary
        : std::is_same<I, float>::value
        ? 1 + 151   // worst case: negative min subnormal float, fixed notation
        : 1 + 1076; // worst case: negative min subnormal double, fixed notation

    constexpr size_t buf_size = buf_prefix + space + buf_suffix;
    vx::array<C, buf_size> buf;

    C* const buf_begin = buf.data();
    C* const buf_end = buf_begin + buf_size;

    constexpr size_t extra_chars = 3;
    VX_STATIC_ASSERT_MSG(extra_chars + 10 < buf_suffix, "The specific values aren't important, but there should be plenty of room to detect buffer overruns.");

    constexpr C fill_char = '@';

    const size_t correct_size = str::length(correct);
    for (size_t n = 0; n <= correct_size + extra_chars; ++n)
    {
        VX_CHECK(n <= static_cast<size_t>(buf_end - first));
        buf.fill('@');

        str::to_string_result res = str::to_string(value, buf.data(), buf.size(), fmt);

        if (n < correct_size)
        {
            VX_CHECK(res.count == 0);
            VX_CHECK(res.err == str::to_string_error::buffer_too_small);
            VX_CHECK(all_of(buf_begin, buf_prefix, fill_char));
            // [first, last) is unspecified
            VX_CHECK(all_of(buf_end, buf_suffix, fill_char));
        }
        else
        {
            VX_CHECK(res.count == correct_size);
            VX_CHECK(res.err == str::to_string_error::none);
            VX_CHECK(all_of(buf_begin, buf_prefix, fill_char));
            VX_CHECK(str::compare(first, res.count, correct, correct_size) == 0);
            VX_CHECK(all_of(first + res.count, buf_suffix, fill_char));
        }
    }
}

//==============================================================================

template <typename I, typename C>
void test_integer_to_string(const I value, str::integer_format_options& fmt, const C* correct)
{
    test_common_to_string(value, fmt, correct);

    // Also test successful from_chars() scenarios.
    {
        const size_t correct_size = str::length(correct);

        I out = 0;
        const str::from_string_result res = str::from_string(correct, correct_size, out, fmt);

        VX_CHECK(res.count == correct_size);
        VX_CHECK(res.err == str::from_string_error::none);
        VX_CHECK(out == value);
    }
}

template <typename I, typename C>
bool test_integer_to_string()
{
    for (int base = 2; base <= 36; ++base)
    {
        test_integer_to_string(static_cast<I>(0), base, "0");
        test_integer_to_string(static_cast<I>(1), base, "1");

        // tests [3, 71]
        test_integer_to_string(static_cast<I>(base * 2 - 1), base, output_max_digit[base]);

        for (const auto& p : output_positive)
        {
            if (p.first <= static_cast<uint64_t>(std::numeric_limits<I>::max()))
            {
                test_integer_to_string(static_cast<I>(p.first), base, p.second[static_cast<size_t>(base)]);
            }
        }

        VX_IF_CONSTEXPR (std::is_signed<I>::value)
        {
            test_integer_to_string(static_cast<I>(-1), base, "-1");

            for (const auto& p : output_negative)
            {
                if (p.first >= static_cast<int64_t>(std::numeric_limits<I>::min()))
                {
                    test_integer_to_string(static_cast<I>(p.first), base, p.second[static_cast<size_t>(base)]);
                }
            }
        }
    }

    test_integer_to_string(static_cast<I>(42), {}, "42");
    return true;
}

template <typename I>
void test_integer()
{
    test_integer_to_string<I>();
}

void all_integer_tests()
{
    test_integer<char>();
    test_integer<signed char>();
    test_integer<unsigned char>();
    test_integer<short>();
    test_integer<unsigned short>();
    test_integer<int>();
    test_integer<unsigned int>();
    test_integer<long>();
    test_integer<unsigned long>();
    test_integer<long long>();
    test_integer<unsigned long long>();
}

//==============================================================================

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}
