#include "vertex_test/test.hpp"

#include "vertex/math/type_traits.hpp"

using namespace vx::math;

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_traits)
{
    VX_SECTION("sizes")
    {
        VX_STATIC_CHECK(sizeof(bool) == 1);

        VX_STATIC_CHECK(sizeof(i8) == 1);
        VX_STATIC_CHECK(sizeof(i16) == 2);
        VX_STATIC_CHECK(sizeof(i32) == 4);
        VX_STATIC_CHECK(sizeof(i64) == 8);

        VX_STATIC_CHECK(sizeof(u8) == 1);
        VX_STATIC_CHECK(sizeof(u16) == 2);
        VX_STATIC_CHECK(sizeof(u32) == 4);
        VX_STATIC_CHECK(sizeof(u64) == 8);

        VX_STATIC_CHECK(sizeof(f32) == 4);
        VX_STATIC_CHECK(sizeof(f64) == 8);
    }

    VX_SECTION("is_float")
    {
        VX_STATIC_CHECK(!is_float<bool>::value);

        VX_STATIC_CHECK(!is_float<i8>::value);
        VX_STATIC_CHECK(!is_float<i16>::value);
        VX_STATIC_CHECK(!is_float<i32>::value);
        VX_STATIC_CHECK(!is_float<i64>::value);

        VX_STATIC_CHECK(!is_float<u8>::value);
        VX_STATIC_CHECK(!is_float<u16>::value);
        VX_STATIC_CHECK(!is_float<u32>::value);
        VX_STATIC_CHECK(!is_float<u64>::value);

        VX_STATIC_CHECK(is_float<f32>::value);
        VX_STATIC_CHECK(is_float<f64>::value);
    }

    VX_SECTION("is_int")
    {
        VX_STATIC_CHECK(is_int<bool>::value);

        VX_STATIC_CHECK(is_int<i8>::value);
        VX_STATIC_CHECK(is_int<i16>::value);
        VX_STATIC_CHECK(is_int<i32>::value);
        VX_STATIC_CHECK(is_int<i64>::value);

        VX_STATIC_CHECK(is_int<u8>::value);
        VX_STATIC_CHECK(is_int<u16>::value);
        VX_STATIC_CHECK(is_int<u32>::value);
        VX_STATIC_CHECK(is_int<u64>::value);

        VX_STATIC_CHECK(!is_int<f32>::value);
        VX_STATIC_CHECK(!is_int<f64>::value);
    }

    VX_SECTION("is_signed")
    {
        VX_STATIC_CHECK(!is_signed<bool>::value);

        VX_STATIC_CHECK(is_signed<i8>::value);
        VX_STATIC_CHECK(is_signed<i16>::value);
        VX_STATIC_CHECK(is_signed<i32>::value);
        VX_STATIC_CHECK(is_signed<i64>::value);

        VX_STATIC_CHECK(!is_signed<u8>::value);
        VX_STATIC_CHECK(!is_signed<u16>::value);
        VX_STATIC_CHECK(!is_signed<u32>::value);
        VX_STATIC_CHECK(!is_signed<u64>::value);

        VX_STATIC_CHECK(is_signed<f32>::value);
        VX_STATIC_CHECK(is_signed<f64>::value);
    }

    VX_SECTION("is_signed_int")
    {
        VX_STATIC_CHECK(!is_signed_int<bool>::value);

        VX_STATIC_CHECK(is_signed_int<i8>::value);
        VX_STATIC_CHECK(is_signed_int<i16>::value);
        VX_STATIC_CHECK(is_signed_int<i32>::value);
        VX_STATIC_CHECK(is_signed_int<i64>::value);

        VX_STATIC_CHECK(!is_signed_int<u8>::value);
        VX_STATIC_CHECK(!is_signed_int<u16>::value);
        VX_STATIC_CHECK(!is_signed_int<u32>::value);
        VX_STATIC_CHECK(!is_signed_int<u64>::value);

        VX_STATIC_CHECK(!is_signed_int<f32>::value);
        VX_STATIC_CHECK(!is_signed_int<f64>::value);
    }

    VX_SECTION("is_unsigned")
    {
        VX_STATIC_CHECK(is_unsigned<bool>::value);

        VX_STATIC_CHECK(!is_unsigned<i8>::value);
        VX_STATIC_CHECK(!is_unsigned<i16>::value);
        VX_STATIC_CHECK(!is_unsigned<i32>::value);
        VX_STATIC_CHECK(!is_unsigned<i64>::value);

        VX_STATIC_CHECK(is_unsigned<u8>::value);
        VX_STATIC_CHECK(is_unsigned<u16>::value);
        VX_STATIC_CHECK(is_unsigned<u32>::value);
        VX_STATIC_CHECK(is_unsigned<u64>::value);

        VX_STATIC_CHECK(!is_unsigned<f32>::value);
        VX_STATIC_CHECK(!is_unsigned<f64>::value);
    }

    VX_SECTION("is_numeric")
    {
        VX_STATIC_CHECK(is_numeric<bool>::value);

        VX_STATIC_CHECK(is_numeric<i8>::value);
        VX_STATIC_CHECK(is_numeric<i16>::value);
        VX_STATIC_CHECK(is_numeric<i32>::value);
        VX_STATIC_CHECK(is_numeric<i64>::value);

        VX_STATIC_CHECK(is_numeric<u8>::value);
        VX_STATIC_CHECK(is_numeric<u16>::value);
        VX_STATIC_CHECK(is_numeric<u32>::value);
        VX_STATIC_CHECK(is_numeric<u64>::value);

        VX_STATIC_CHECK(is_numeric<f32>::value);
        VX_STATIC_CHECK(is_numeric<f64>::value);
    }

    VX_SECTION("is_scalar")
    {
        VX_STATIC_CHECK(is_scalar<bool>::value);

        VX_STATIC_CHECK(!is_scalar<i8>::value);
        VX_STATIC_CHECK(!is_scalar<i16>::value);
        VX_STATIC_CHECK(is_scalar<i32>::value);
        VX_STATIC_CHECK(!is_scalar<i64>::value);

        VX_STATIC_CHECK(!is_scalar<u8>::value);
        VX_STATIC_CHECK(!is_scalar<u16>::value);
        VX_STATIC_CHECK(is_scalar<u32>::value);
        VX_STATIC_CHECK(!is_scalar<u64>::value);

        VX_STATIC_CHECK(is_scalar<f32>::value);
        VX_STATIC_CHECK(is_scalar<f64>::value);
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}