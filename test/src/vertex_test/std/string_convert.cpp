#include <cstring>

#include "vertex/std/string_convert.hpp"
#include "vertex_test/test.hpp"
#include "vertex_test/test.hpp"

using namespace vx;

#define LIT(x)          VX_LIT(C, x)
#define NUM(x)          static_cast<T>(x)
#define CHECK_STR(a, b) VX_CHECK(::vx::str::compare(a, b) == 0)

#define PRINT_RESULT 1

template <typename T, typename C>
struct test_case
{
    T value;
    str::numeric_format_options<C> fmt;
    const C* expected;
};

//==============================================================================

template <typename T, typename C>
bool run_test_case(const test_case<T, C>& tc)
{
    const str::basic_string<C> s = str::to_string<T, C>(tc.value, tc.fmt);

#if PRINT_RESULT

    VX_IF_CONSTEXPR (std::is_same<C, char>::value)
    {
        std::cout << "Value: " << tc.value << ", String: " << s << ", Expected: " << tc.expected << std::endl;
    }
#endif

    return (str::compare(s, tc.expected) == 0);
}

//==============================================================================

template <typename C>
void test_to_string_impl()
{
    VX_SECTION("int")
    {
        using T = int;
        str::numeric_format_options<C> fmt;

        // base 10 and default options
        VX_CHECK(run_test_case<T, C>({ NUM(0), fmt, LIT("0") }));
        VX_CHECK(run_test_case<T, C>({ NUM(123), fmt, LIT("123") }));
        VX_CHECK(run_test_case<T, C>({ NUM(-456), fmt, LIT("-456") }));

        // max and min
        VX_CHECK(run_test_case<T, C>({ std::numeric_limits<T>::max(), fmt, LIT("2147483647") }));
        VX_CHECK(run_test_case<T, C>({ std::numeric_limits<T>::min(), fmt, LIT("-2147483648") }));

        // hexadecimal
        fmt.base = 16;
        VX_CHECK(run_test_case<T, C>({ NUM(255), fmt, LIT("ff") }));

        // uppercase hex
        fmt.uppercase = true;
        VX_CHECK(run_test_case<T, C>({ NUM(255), fmt, LIT("FF") }));

        // force sign
        fmt.force_sign = true;
        VX_CHECK(run_test_case<T, C>({ NUM(-255), fmt, LIT("-FF") }));
        VX_CHECK(run_test_case<T, C>({ NUM(255), fmt, LIT("+FF") }));
        fmt.force_sign = false;

        // max output size
        fmt.base = 2;
        VX_CHECK(run_test_case<T, C>({ std::numeric_limits<T>::max(), fmt, LIT("1111111111111111111111111111111") }));
        VX_CHECK(run_test_case<T, C>({ std::numeric_limits<T>::min(), fmt, LIT("-10000000000000000000000000000000") }));

        // Check null buffer outputs correct size
        {
            fmt = {};
            const size_t n = str::write_integer<T, C>(NUM(12345), nullptr, 0, fmt);
            VX_CHECK(n == 5);
        }

        // Check partial buffer handling and null termination
        {
            fmt = {};
            constexpr size_t buf_size = 4;
            C buf[buf_size + 1];

            const size_t n = str::write_integer<T, C>(NUM(12345), buf, buf_size, fmt, true);
            VX_CHECK(n == buf_size);
            buf[buf_size] = C('\0');
            VX_CHECK(str::compare(buf, LIT("1234")) == 0);
        }
    }

    VX_SECTION("unsigned int")
    {
        using T = unsigned int;
        str::numeric_format_options<C> fmt;

        // base 10 and default options
        VX_CHECK(run_test_case<T, C>({ NUM(0), fmt, LIT("0") }));
        VX_CHECK(run_test_case<T, C>({ NUM(123), fmt, LIT("123") }));

        // max and min
        VX_CHECK(run_test_case<T, C>({ std::numeric_limits<T>::max(), fmt, LIT("4294967295") }));
        VX_CHECK(run_test_case<T, C>({ std::numeric_limits<T>::min(), fmt, LIT("0") }));

        // hexadecimal
        fmt.base = 16;
        VX_CHECK(run_test_case<T, C>({ NUM(255), fmt, LIT("ff") }));

        // uppercase hex
        fmt.uppercase = true;
        VX_CHECK(run_test_case<T, C>({ NUM(255), fmt, LIT("FF") }));

        // force sign
        fmt.force_sign = true;
        VX_CHECK(run_test_case<T, C>({ NUM(255), fmt, LIT("+FF") }));
        fmt.force_sign = false;

        // max output size
        fmt.base = 2;
        VX_CHECK(run_test_case<T, C>({ std::numeric_limits<T>::max(), fmt, LIT("11111111111111111111111111111111") }));
        VX_CHECK(run_test_case<T, C>({ std::numeric_limits<T>::min(), fmt, LIT("0") }));

        // Check null buffer outputs correct size
        {
            fmt = {};
            const size_t n = str::write_integer<T, C>(NUM(12345), nullptr, 0, fmt);
            VX_CHECK(n == 5);
        }

        // Check partial buffer handling
        {
            fmt = {};
            constexpr size_t buf_size = 4;
            C buf[buf_size + 1];

            const size_t n = str::write_integer<T, C>(NUM(12345), buf, buf_size, fmt);
            VX_CHECK(n == buf_size);
            buf[buf_size] = C('\0');
            VX_CHECK(str::compare(buf, LIT("1234")) == 0);
        }
    }

    VX_SECTION("float")
    {
        using T = float;
        str::numeric_format_options<C> fmt;

        // -------------------------------------------------------------------------
        // fixed format
        // -------------------------------------------------------------------------

        fmt.format = str::numeric_format::fixed;

        // basic values
        VX_CHECK(run_test_case<T, C>({ NUM(0.0f), fmt, LIT("0.000000") }));
        VX_CHECK(run_test_case<T, C>({ NUM(1.0f), fmt, LIT("1.000000") }));
        VX_CHECK(run_test_case<T, C>({ NUM(-1.0f), fmt, LIT("-1.000000") }));
        VX_CHECK(run_test_case<T, C>({ NUM(3.14f), fmt, LIT("3.140000") }));
        VX_CHECK(run_test_case<T, C>({ NUM(123.456f), fmt, LIT("123.456001") })); // float precision artifact

        // precision
        fmt.precision = 0;
        VX_CHECK(run_test_case<T, C>({ NUM(3.7f), fmt, LIT("4") }));
        VX_CHECK(run_test_case<T, C>({ NUM(-3.7f), fmt, LIT("-4") }));
        fmt.precision = 2;
        VX_CHECK(run_test_case<T, C>({ NUM(3.14159f), fmt, LIT("3.14") }));
        fmt.precision = 10;
        VX_CHECK(run_test_case<T, C>({ NUM(1.0f), fmt, LIT("1.0000000000") }));
        fmt.precision = str::default_float_precision;

        // force sign
        fmt.force_sign = true;
        VX_CHECK(run_test_case<T, C>({ NUM(1.0f), fmt, LIT("+1.000000") }));
        VX_CHECK(run_test_case<T, C>({ NUM(-1.0f), fmt, LIT("-1.000000") }));
        fmt.force_sign = false;

        // uppercase (no effect on fixed, but should not break)
        fmt.uppercase = true;
        VX_CHECK(run_test_case<T, C>({ NUM(1.0f), fmt, LIT("1.000000") }));
        fmt.uppercase = false;

        // special values
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::infinity()), fmt, LIT("inf") }));
        VX_CHECK(run_test_case<T, C>({ NUM(-std::numeric_limits<T>::infinity()), fmt, LIT("-inf") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::quiet_NaN()), fmt, LIT("nan") }));
        fmt.uppercase = true;
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::infinity()), fmt, LIT("INF") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::quiet_NaN()), fmt, LIT("NAN") }));
        fmt.uppercase = false;
        fmt.force_sign = true;
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::infinity()), fmt, LIT("+inf") }));
        VX_CHECK(run_test_case<T, C>({ NUM(-std::numeric_limits<T>::infinity()), fmt, LIT("-inf") }));
        // if we do our own implementation, nan should not have a sign even when forcing a sign
        //VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::quiet_NaN()), fmt, LIT("nan") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::quiet_NaN()), fmt, LIT("+nan") }));
        fmt.force_sign = false;

        // negative zero
        VX_CHECK(run_test_case<T, C>({ NUM(-0.0f), fmt, LIT("-0.000000") }));

        // max and min
        fmt.precision = 0; // full integer part only, no decimal noise at this magnitude
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::max()), fmt, LIT("340282346638528859811704183484516925440") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::lowest()), fmt, LIT("-340282346638528859811704183484516925440") }));

        // max and min with decimals — buffer must be large enough
        fmt.precision = str::default_float_precision;
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::max()), fmt, LIT("340282346638528859811704183484516925440.000000") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::lowest()), fmt, LIT("-340282346638528859811704183484516925440.000000") }));

        // max precision (should cap to 32)
        fmt.precision = 1000;
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::denorm_min()), fmt, LIT("0.00000000000000000000000000000000") }));
        fmt.precision = str::default_float_precision;

        // -------------------------------------------------------------------------
        // scientific format
        // -------------------------------------------------------------------------

        fmt.format = str::numeric_format::scientific;
        fmt.precision = str::default_float_precision;

        // basic values
        VX_CHECK(run_test_case<T, C>({ NUM(0.0f), fmt, LIT("0.000000e+00") }));
        VX_CHECK(run_test_case<T, C>({ NUM(1.0f), fmt, LIT("1.000000e+00") }));
        VX_CHECK(run_test_case<T, C>({ NUM(-1.0f), fmt, LIT("-1.000000e+00") }));
        VX_CHECK(run_test_case<T, C>({ NUM(3.14f), fmt, LIT("3.140000e+00") }));
        VX_CHECK(run_test_case<T, C>({ NUM(0.001f), fmt, LIT("1.000000e-03") }));
        VX_CHECK(run_test_case<T, C>({ NUM(123456.0f), fmt, LIT("1.234560e+05") }));

        // precision
        fmt.precision = 0;
        VX_CHECK(run_test_case<T, C>({ NUM(3.14f), fmt, LIT("3e+00") }));
        fmt.precision = 2;
        VX_CHECK(run_test_case<T, C>({ NUM(3.14f), fmt, LIT("3.14e+00") }));
        fmt.precision = str::default_float_precision;

        // uppercase
        fmt.uppercase = true;
        VX_CHECK(run_test_case<T, C>({ NUM(3.14f), fmt, LIT("3.140000E+00") }));
        VX_CHECK(run_test_case<T, C>({ NUM(0.001f), fmt, LIT("1.000000E-03") }));
        fmt.uppercase = false;

        // force sign
        fmt.force_sign = true;
        VX_CHECK(run_test_case<T, C>({ NUM(1.0f), fmt, LIT("+1.000000e+00") }));
        VX_CHECK(run_test_case<T, C>({ NUM(-1.0f), fmt, LIT("-1.000000e+00") }));
        fmt.force_sign = false;

        // special values
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::infinity()), fmt, LIT("inf") }));
        VX_CHECK(run_test_case<T, C>({ NUM(-std::numeric_limits<T>::infinity()), fmt, LIT("-inf") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::quiet_NaN()), fmt, LIT("nan") }));
        fmt.uppercase = true;
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::infinity()), fmt, LIT("INF") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::quiet_NaN()), fmt, LIT("NAN") }));
        fmt.uppercase = false;
        fmt.force_sign = true;
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::infinity()), fmt, LIT("+inf") }));
        VX_CHECK(run_test_case<T, C>({ NUM(-std::numeric_limits<T>::infinity()), fmt, LIT("-inf") }));
        // if we do our own implementation, nan should not have a sign even when forcing a sign
        //VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::quiet_NaN()), fmt, LIT("nan") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::quiet_NaN()), fmt, LIT("+nan") }));
        fmt.force_sign = false;

        // negative zero
        VX_CHECK(run_test_case<T, C>({ NUM(-0.0f), fmt, LIT("-0.000000e+00") }));

        // special magnitudes
        fmt.precision = str::default_float_precision;
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::max()), fmt, LIT("3.402823e+38") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::lowest()), fmt, LIT("-3.402823e+38") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::min()), fmt, LIT("1.175494e-38") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::denorm_min()), fmt, LIT("1.401298e-45") }));

        // -------------------------------------------------------------------------
        // general format
        // -------------------------------------------------------------------------

        fmt.format = str::numeric_format::general;
        fmt.precision = str::default_float_precision;

        // uses fixed or scientific depending on magnitude (matches %g)
        VX_CHECK(run_test_case<T, C>({ NUM(0.0f), fmt, LIT("0") }));
        VX_CHECK(run_test_case<T, C>({ NUM(1.0f), fmt, LIT("1") }));
        VX_CHECK(run_test_case<T, C>({ NUM(-1.0f), fmt, LIT("-1") }));
        VX_CHECK(run_test_case<T, C>({ NUM(3.14f), fmt, LIT("3.14") }));
        VX_CHECK(run_test_case<T, C>({ NUM(0.0001f), fmt, LIT("0.0001") }));         // boundary: still fixed
        VX_CHECK(run_test_case<T, C>({ NUM(0.00001f), fmt, LIT("1e-05") }));         // boundary: switches to sci
        VX_CHECK(run_test_case<T, C>({ NUM(123456.0f), fmt, LIT("123456") }));       // still fixed
        VX_CHECK(run_test_case<T, C>({ NUM(1234567.0f), fmt, LIT("1.23457e+06") })); // switches to sci

        // precision (controls significant digits in general, not decimal places)
        fmt.precision = 3;
        VX_CHECK(run_test_case<T, C>({ NUM(3.14159f), fmt, LIT("3.14") }));
        VX_CHECK(run_test_case<T, C>({ NUM(12345.0f), fmt, LIT("1.23e+04") }));
        fmt.precision = 1;
        VX_CHECK(run_test_case<T, C>({ NUM(3.14f), fmt, LIT("3") }));
        fmt.precision = str::default_float_precision;

        // trailing zeros stripped (matches %g behaviour)
        VX_CHECK(run_test_case<T, C>({ NUM(1.5f), fmt, LIT("1.5") }));
        VX_CHECK(run_test_case<T, C>({ NUM(1.0f), fmt, LIT("1") }));

        // uppercase
        fmt.uppercase = true;
        VX_CHECK(run_test_case<T, C>({ NUM(0.00001f), fmt, LIT("1E-05") }));
        VX_CHECK(run_test_case<T, C>({ NUM(1234567.0f), fmt, LIT("1.23457E+06") }));
        fmt.uppercase = false;

        // force sign
        fmt.force_sign = true;
        VX_CHECK(run_test_case<T, C>({ NUM(3.14f), fmt, LIT("+3.14") }));
        VX_CHECK(run_test_case<T, C>({ NUM(-3.14f), fmt, LIT("-3.14") }));
        fmt.force_sign = false;

        // special values
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::infinity()), fmt, LIT("inf") }));
        VX_CHECK(run_test_case<T, C>({ NUM(-std::numeric_limits<T>::infinity()), fmt, LIT("-inf") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::quiet_NaN()), fmt, LIT("nan") }));
        fmt.uppercase = true;
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::infinity()), fmt, LIT("INF") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::quiet_NaN()), fmt, LIT("NAN") }));
        fmt.uppercase = false;
        fmt.force_sign = true;
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::infinity()), fmt, LIT("+inf") }));
        VX_CHECK(run_test_case<T, C>({ NUM(-std::numeric_limits<T>::infinity()), fmt, LIT("-inf") }));
        // if we do our own implementation, nan should not have a sign even when forcing a sign
        //VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::quiet_NaN()), fmt, LIT("nan") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::quiet_NaN()), fmt, LIT("+nan") }));
        fmt.force_sign = false;

        // negative zero
        VX_CHECK(run_test_case<T, C>({ NUM(-0.0f), fmt, LIT("-0") }));

        // special magnitudes
        fmt.precision = str::default_float_precision;
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::max()), fmt, LIT("3.40282e+38") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::lowest()), fmt, LIT("-3.40282e+38") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::min()), fmt, LIT("1.17549e-38") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::denorm_min()), fmt, LIT("1.4013e-45") }));

        // -------------------------------------------------------------------------
        // hex format
        // -------------------------------------------------------------------------

        fmt.format = str::numeric_format::hex;
        fmt.precision = str::default_float_precision;

        // basic values (matches %a — platform exact values may vary slightly)
        VX_CHECK(run_test_case<T, C>({ NUM(0.0f), fmt, LIT("0x0.000000p+0") }));
        VX_CHECK(run_test_case<T, C>({ NUM(1.0f), fmt, LIT("0x1.000000p+0") }));
        VX_CHECK(run_test_case<T, C>({ NUM(-1.0f), fmt, LIT("-0x1.000000p+0") }));
        VX_CHECK(run_test_case<T, C>({ NUM(0.5f), fmt, LIT("0x1.000000p-1") }));
        VX_CHECK(run_test_case<T, C>({ NUM(3.14f), fmt, LIT("0x1.91eb86p+1") }));

        // precision
        fmt.precision = 0;
        VX_CHECK(run_test_case<T, C>({ NUM(1.0f), fmt, LIT("0x1p+0") }));
        fmt.precision = 2;
        VX_CHECK(run_test_case<T, C>({ NUM(3.14f), fmt, LIT("0x1.92p+1") }));
        fmt.precision = str::default_float_precision;

        // uppercase
        fmt.uppercase = true;
        VX_CHECK(run_test_case<T, C>({ NUM(1.0f), fmt, LIT("0X1.000000P+0") }));
        VX_CHECK(run_test_case<T, C>({ NUM(3.14f), fmt, LIT("0X1.91EB86P+1") }));
        fmt.uppercase = false;

        // force sign
        fmt.force_sign = true;
        VX_CHECK(run_test_case<T, C>({ NUM(1.0f), fmt, LIT("+0x1.000000p+0") }));
        VX_CHECK(run_test_case<T, C>({ NUM(-1.0f), fmt, LIT("-0x1.000000p+0") }));
        fmt.force_sign = false;

        // special values
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::infinity()), fmt, LIT("inf") }));
        VX_CHECK(run_test_case<T, C>({ NUM(-std::numeric_limits<T>::infinity()), fmt, LIT("-inf") }));
        VX_CHECK(run_test_case<T, C>({ NUM(std::numeric_limits<T>::quiet_NaN()), fmt, LIT("nan") }));

        // -------------------------------------------------------------------------
        // null buffer / partial buffer
        // -------------------------------------------------------------------------
        {
            fmt = {};
            const size_t n = str::write_float<T, C>(NUM(3.14f), nullptr, 0, fmt);
            VX_CHECK(n == 4); // "3.14" in general format
        }
        {
            fmt = {};
            constexpr size_t buf_size = 3;
            C buf[buf_size + 1];
            const size_t n = str::write_float<T, C>(NUM(3.14f), buf, buf_size, fmt);
            VX_CHECK(n == buf_size);
            buf[buf_size] = C('\0');
            VX_CHECK(str::compare(buf, LIT("3.1")) == 0);
        }
    }
}

//==============================================================================

VX_TEST_CASE(to_string)
{
    //test_to_string_impl<char>();
    //test_to_string_impl<wchar_t>();

    str::write_float_fixed(std::numeric_limits<float>::max());
}

//==============================================================================

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}
