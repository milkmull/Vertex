#include "vertex/_test/test.hpp"

#include "vertex/util/bit/bit.hpp"

///////////////////////////////////////////////////////////////////////////////
// byteswap
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(byteswap)
{
    VX_SECTION("byteswap8")
    {
        constexpr uint16_t value = 0x1234;
        constexpr uint16_t expected = 0x3412;

        VX_CHECK(vx::bit::byteswap(value) == expected);
    }

    VX_SECTION("byteswap_16")
    {
        constexpr uint16_t value = 0x1234;
        constexpr uint16_t expected = 0x3412;

        VX_CHECK(vx::bit::byteswap(value) == expected);
    }

    VX_SECTION("byteswap_32")
    {
        constexpr uint32_t value = 0x12345678;
        constexpr uint32_t expected = 0x78563412;

        VX_CHECK(vx::bit::byteswap(value) == expected);
    }

    VX_SECTION("byteswap_64")
    {
        constexpr uint64_t value = 0x123456789ABCDEF0;
        constexpr uint64_t expected = 0xF0DEBC9A78563412;

        VX_CHECK(vx::bit::byteswap(value) == expected);
    }
}


///////////////////////////////////////////////////////////////////////////////
// rotl
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(rotl)
{
    VX_SECTION("rotl_8")
    {
        // Alternating bits
        VX_CHECK(vx::bit::rotl(static_cast<uint8_t>(0b10101010), 3) == static_cast<uint8_t>(0b01010101));

        // Single bit
        VX_CHECK(vx::bit::rotl(static_cast<uint8_t>(0b00000001), 1) == static_cast<uint8_t>(0b00000010));
        VX_CHECK(vx::bit::rotl(static_cast<uint8_t>(0b00000001), 7) == static_cast<uint8_t>(0b10000000));

        // All bits set
        VX_CHECK(vx::bit::rotl(static_cast<uint8_t>(0xFF), 4) == static_cast<uint8_t>(0xFF));

        // Edge rotation (shift by 0 and 8)
        VX_CHECK(vx::bit::rotl(static_cast<uint8_t>(0b10101010), 0) == static_cast<uint8_t>(0b10101010));
        VX_CHECK(vx::bit::rotl(static_cast<uint8_t>(0b10101010), 8) == static_cast<uint8_t>(0b10101010));
    }

    VX_SECTION("rotl_16")
    {
        // Alternating bits
        VX_CHECK(vx::bit::rotl(static_cast<uint16_t>(0b1010101010101010), 5) == static_cast<uint16_t>(0b0101010101010101));

        // Single bit
        VX_CHECK(vx::bit::rotl(static_cast<uint16_t>(0b0000000000000001), 1) == static_cast<uint16_t>(0b0000000000000010));
        VX_CHECK(vx::bit::rotl(static_cast<uint16_t>(0b0000000000000001), 15) == static_cast<uint16_t>(0b1000000000000000));

        // All bits set
        VX_CHECK(vx::bit::rotl(static_cast<uint16_t>(0xFFFF), 8) == static_cast<uint16_t>(0xFFFF));

        // Edge rotation (shift by 0 and 16)
        VX_CHECK(vx::bit::rotl(static_cast<uint16_t>(0b1010101010101010), 0) == static_cast<uint16_t>(0b1010101010101010));
        VX_CHECK(vx::bit::rotl(static_cast<uint16_t>(0b1010101010101010), 16) == static_cast<uint16_t>(0b1010101010101010));
    }

    VX_SECTION("rotl_32")
    {
        // Alternating bits
        VX_CHECK(vx::bit::rotl(static_cast<uint32_t>(0xAAAAAAAA), 5) == static_cast<uint32_t>(0x55555555));

        // Single bit
        VX_CHECK(vx::bit::rotl(static_cast<uint32_t>(0x00000001), 1) == static_cast<uint32_t>(0x00000002));
        VX_CHECK(vx::bit::rotl(static_cast<uint32_t>(0x00000001), 31) == static_cast<uint32_t>(0x80000000));

        // All bits set
        VX_CHECK(vx::bit::rotl(static_cast<uint32_t>(0xFFFFFFFF), 16) == static_cast<uint32_t>(0xFFFFFFFF));

        // Edge rotation (shift by 0 and 32)
        VX_CHECK(vx::bit::rotl(static_cast<uint32_t>(0xAAAAAAAA), 0) == static_cast<uint32_t>(0xAAAAAAAA));
        VX_CHECK(vx::bit::rotl(static_cast<uint32_t>(0xAAAAAAAA), 32) == static_cast<uint32_t>(0xAAAAAAAA));
    }

    VX_SECTION("rotl_64")
    {
        // Alternating bits
        VX_CHECK(vx::bit::rotl(static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA), 5) == static_cast<uint64_t>(0x5555555555555555));

        // Single bit
        VX_CHECK(vx::bit::rotl(static_cast<uint64_t>(0x0000000000000001), 1) == static_cast<uint64_t>(0x0000000000000002));
        VX_CHECK(vx::bit::rotl(static_cast<uint64_t>(0x0000000000000001), 63) == static_cast<uint64_t>(0x8000000000000000));

        // All bits set
        VX_CHECK(vx::bit::rotl(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), 32) == static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF));

        // Edge rotation (shift by 0 and 64)
        VX_CHECK(vx::bit::rotl(static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA), 0) == static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA));
        VX_CHECK(vx::bit::rotl(static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA), 32) == static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA));
    }
}

///////////////////////////////////////////////////////////////////////////////
// rotr
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(rotr)
{
    VX_SECTION("rotr_8")
    {
        // Alternating bits
        VX_CHECK(vx::bit::rotr(static_cast<uint8_t>(0b10101010), 3) == static_cast<uint8_t>(0b01010101));

        // Single bit
        VX_CHECK(vx::bit::rotr(static_cast<uint8_t>(0b00000001), 1) == static_cast<uint8_t>(0b10000000));
        VX_CHECK(vx::bit::rotr(static_cast<uint8_t>(0b00000001), 7) == static_cast<uint8_t>(0b00000010));

        // All bits set
        VX_CHECK(vx::bit::rotr(static_cast<uint8_t>(0xFF), 4) == static_cast<uint8_t>(0xFF));

        // Edge rotation (shift by 0 and 8)
        VX_CHECK(vx::bit::rotr(static_cast<uint8_t>(0b10101010), 0) == static_cast<uint8_t>(0b10101010));
        VX_CHECK(vx::bit::rotr(static_cast<uint8_t>(0b10101010), 8) == static_cast<uint8_t>(0b10101010));
    }

    VX_SECTION("rotr_16")
    {
        // Alternating bits
        VX_CHECK(vx::bit::rotr(static_cast<uint16_t>(0b1010101010101010), 5) == static_cast<uint16_t>(0b0101010101010101));

        // Single bit
        VX_CHECK(vx::bit::rotr(static_cast<uint16_t>(0b0000000000000001), 1) == static_cast<uint16_t>(0b1000000000000000));
        VX_CHECK(vx::bit::rotr(static_cast<uint16_t>(0b0000000000000001), 15) == static_cast<uint16_t>(0b0000000000000010));

        // All bits set
        VX_CHECK(vx::bit::rotr(static_cast<uint16_t>(0xFFFF), 8) == static_cast<uint16_t>(0xFFFF));

        // Edge rotation (shift by 0 and 16)
        VX_CHECK(vx::bit::rotr(static_cast<uint16_t>(0b1010101010101010), 0) == static_cast<uint16_t>(0b1010101010101010));
        VX_CHECK(vx::bit::rotr(static_cast<uint16_t>(0b1010101010101010), 16) == static_cast<uint16_t>(0b1010101010101010));
    }

    VX_SECTION("rotr_32")
    {
        // Alternating bits
        VX_CHECK(vx::bit::rotr(static_cast<uint32_t>(0xAAAAAAAA), 5) == static_cast<uint32_t>(0x55555555));

        // Single bit
        VX_CHECK(vx::bit::rotr(static_cast<uint32_t>(0x00000001), 1) == static_cast<uint32_t>(0x80000000));
        VX_CHECK(vx::bit::rotr(static_cast<uint32_t>(0x00000001), 31) == static_cast<uint32_t>(0x00000002));

        // All bits set
        VX_CHECK(vx::bit::rotr(static_cast<uint32_t>(0xFFFFFFFF), 16) == static_cast<uint32_t>(0xFFFFFFFF));

        // Edge rotation (shift by 0 and 32)
        VX_CHECK(vx::bit::rotr(static_cast<uint32_t>(0xAAAAAAAA), 0) == static_cast<uint32_t>(0xAAAAAAAA));
        VX_CHECK(vx::bit::rotr(static_cast<uint32_t>(0xAAAAAAAA), 32) == static_cast<uint32_t>(0xAAAAAAAA));
    }

    VX_SECTION("rotr_64")
    {
        // Alternating bits
        VX_CHECK(vx::bit::rotr(static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA), 5) == static_cast<uint64_t>(0x5555555555555555));

        // Single bit
        VX_CHECK(vx::bit::rotr(static_cast<uint64_t>(0x0000000000000001), 1) == static_cast<uint64_t>(0x8000000000000000));
        VX_CHECK(vx::bit::rotr(static_cast<uint64_t>(0x0000000000000001), 63) == static_cast<uint64_t>(0x0000000000000002));

        // All bits set
        VX_CHECK(vx::bit::rotr(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF), 32) == static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF));

        // Edge rotation (shift by 0 and 64)
        VX_CHECK(vx::bit::rotr(static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA), 0) == static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA));
        VX_CHECK(vx::bit::rotr(static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA), 32) == static_cast<uint64_t>(0xAAAAAAAAAAAAAAAA));
    }
}

///////////////////////////////////////////////////////////////////////////////
// countl_zero
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(countl_zero)
{
    VX_SECTION("countl_zero_8")
    {
        // No bits set
        VX_CHECK(vx::bit::countl_zero(static_cast<uint8_t>(0b00000000)) == 8);
        // All bits set
        VX_CHECK(vx::bit::countl_zero(static_cast<uint8_t>(0b11111111)) == 0);
        // LSB set
        VX_CHECK(vx::bit::countl_zero(static_cast<uint8_t>(0b00000001)) == 7);
        // MSB set
        VX_CHECK(vx::bit::countl_zero(static_cast<uint8_t>(0b10000000)) == 0);
    }

    VX_SECTION("countl_zero_16")
    {
        // No bits set
        VX_CHECK(vx::bit::countl_zero(static_cast<uint16_t>(0x0000)) == 16);
        // All bits set
        VX_CHECK(vx::bit::countl_zero(static_cast<uint16_t>(0xFFFF)) == 0);
        // LSB set
        VX_CHECK(vx::bit::countl_zero(static_cast<uint16_t>(0x0001)) == 15);
        // MSB set
        VX_CHECK(vx::bit::countl_zero(static_cast<uint16_t>(0x8000)) == 0);
    }

    VX_SECTION("countl_zero_32")
    {
        // No bits set
        VX_CHECK(vx::bit::countl_zero(static_cast<uint32_t>(0x00000000)) == 32);
        // All bits set
        VX_CHECK(vx::bit::countl_zero(static_cast<uint32_t>(0xFFFFFFFF)) == 0);
        // LSB set
        VX_CHECK(vx::bit::countl_zero(static_cast<uint32_t>(0x00000001)) == 31);
        // MSB set
        VX_CHECK(vx::bit::countl_zero(static_cast<uint32_t>(0x80000000)) == 0);
    }

    VX_SECTION("countl_zero_64")
    {
        // No bits set
        VX_CHECK(vx::bit::countl_zero(static_cast<uint64_t>(0x0000000000000000)) == 64);
        // All bits set
        VX_CHECK(vx::bit::countl_zero(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF)) == 0);
        // LSB set
        VX_CHECK(vx::bit::countl_zero(static_cast<uint64_t>(0x0000000000000001)) == 63);
        // MSB set
        VX_CHECK(vx::bit::countl_zero(static_cast<uint64_t>(0x8000000000000000)) == 0);
    }
}

///////////////////////////////////////////////////////////////////////////////
// countl_one
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(countl_one)
{
    VX_SECTION("countl_one_8")
    {
        // No bits set
        VX_CHECK(vx::bit::countl_one(static_cast<uint8_t>(0b00000000)) == 0);
        // All bits set
        VX_CHECK(vx::bit::countl_one(static_cast<uint8_t>(0b11111111)) == 8);
        // LSB set
        VX_CHECK(vx::bit::countl_one(static_cast<uint8_t>(0b00000001)) == 0);
        // MSB set
        VX_CHECK(vx::bit::countl_one(static_cast<uint8_t>(0b10000000)) == 1);
    }

    VX_SECTION("countl_one_16")
    {
        // No bits set
        VX_CHECK(vx::bit::countl_one(static_cast<uint16_t>(0x0000)) == 0);
        // All bits set
        VX_CHECK(vx::bit::countl_one(static_cast<uint16_t>(0xFFFF)) == 16);
        // LSB set
        VX_CHECK(vx::bit::countl_one(static_cast<uint16_t>(0x0001)) == 0);
        // MSB set
        VX_CHECK(vx::bit::countl_one(static_cast<uint16_t>(0x8000)) == 1);
    }

    VX_SECTION("countl_one_32")
    {
        // No bits set
        VX_CHECK(vx::bit::countl_one(static_cast<uint32_t>(0x00000000)) == 0);
        // All bits set
        VX_CHECK(vx::bit::countl_one(static_cast<uint32_t>(0xFFFFFFFF)) == 32);
        // LSB set
        VX_CHECK(vx::bit::countl_one(static_cast<uint32_t>(0x00000001)) == 0);
        // MSB set
        VX_CHECK(vx::bit::countl_one(static_cast<uint32_t>(0x80000000)) == 1);
    }

    VX_SECTION("countl_one_64")
    {
        // No bits set
        VX_CHECK(vx::bit::countl_one(static_cast<uint64_t>(0x0000000000000000)) == 0);
        // All bits set
        VX_CHECK(vx::bit::countl_one(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF)) == 64);
        // LSB set
        VX_CHECK(vx::bit::countl_one(static_cast<uint64_t>(0x0000000000000001)) == 0);
        // MSB set
        VX_CHECK(vx::bit::countl_one(static_cast<uint64_t>(0x8000000000000000)) == 1);
    }
}

///////////////////////////////////////////////////////////////////////////////
// countr_zero
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(countr_zero)
{
    VX_SECTION("countr_zero_8")
    {
        // No bits set
        VX_CHECK(vx::bit::countr_zero(static_cast<uint8_t>(0b00000000)) == 8);
        // All bits set
        VX_CHECK(vx::bit::countr_zero(static_cast<uint8_t>(0b11111111)) == 0);
        // LSB set
        VX_CHECK(vx::bit::countr_zero(static_cast<uint8_t>(0b00000001)) == 0);
        // MSB set
        VX_CHECK(vx::bit::countr_zero(static_cast<uint8_t>(0b10000000)) == 7);
    }

    VX_SECTION("countr_zero_16")
    {
        // No bits set
        VX_CHECK(vx::bit::countr_zero(static_cast<uint16_t>(0x0000)) == 16);
        // All bits set
        VX_CHECK(vx::bit::countr_zero(static_cast<uint16_t>(0xFFFF)) == 0);
        // LSB set
        VX_CHECK(vx::bit::countr_zero(static_cast<uint16_t>(0x0001)) == 0);
        // MSB set
        VX_CHECK(vx::bit::countr_zero(static_cast<uint16_t>(0x8000)) == 15);
    }

    VX_SECTION("countr_zero_32")
    {
        // No bits set
        VX_CHECK(vx::bit::countr_zero(static_cast<uint32_t>(0x00000000)) == 32);
        // All bits set
        VX_CHECK(vx::bit::countr_zero(static_cast<uint32_t>(0xFFFFFFFF)) == 0);
        // LSB set
        VX_CHECK(vx::bit::countr_zero(static_cast<uint32_t>(0x00000001)) == 0);
        // MSB set
        VX_CHECK(vx::bit::countr_zero(static_cast<uint32_t>(0x80000000)) == 31);
    }

    VX_SECTION("countr_zero_64")
    {
        // No bits set
        VX_CHECK(vx::bit::countr_zero(static_cast<uint64_t>(0x0000000000000000)) == 64);
        // All bits set
        VX_CHECK(vx::bit::countr_zero(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF)) == 0);
        // LSB set
        VX_CHECK(vx::bit::countr_zero(static_cast<uint64_t>(0x0000000000000001)) == 0);
        // MSB set
        VX_CHECK(vx::bit::countr_zero(static_cast<uint64_t>(0x8000000000000000)) == 63);
    }
}

///////////////////////////////////////////////////////////////////////////////
// countr_one
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(countr_one)
{
    VX_SECTION("countr_one_8")
    {
        // No bits set
        VX_CHECK(vx::bit::countr_one(static_cast<uint8_t>(0b00000000)) == 0);
        // All bits set
        VX_CHECK(vx::bit::countr_one(static_cast<uint8_t>(0b11111111)) == 8);
        // LSB set
        VX_CHECK(vx::bit::countr_one(static_cast<uint8_t>(0b00000001)) == 1);
        // MSB set
        VX_CHECK(vx::bit::countr_one(static_cast<uint8_t>(0b10000000)) == 0);
    }

    VX_SECTION("countr_one_16")
    {
        // No bits set
        VX_CHECK(vx::bit::countr_one(static_cast<uint16_t>(0x0000)) == 0);
        // All bits set
        VX_CHECK(vx::bit::countr_one(static_cast<uint16_t>(0xFFFF)) == 16);
        // LSB set
        VX_CHECK(vx::bit::countr_one(static_cast<uint16_t>(0x0001)) == 1);
        // MSB set
        VX_CHECK(vx::bit::countr_one(static_cast<uint16_t>(0x8000)) == 0);
    }

    VX_SECTION("countr_one_32")
    {
        // No bits set
        VX_CHECK(vx::bit::countr_one(static_cast<uint32_t>(0x00000000)) == 0);
        // All bits set
        VX_CHECK(vx::bit::countr_one(static_cast<uint32_t>(0xFFFFFFFF)) == 32);
        // LSB set
        VX_CHECK(vx::bit::countr_one(static_cast<uint32_t>(0x00000001)) == 1);
        // MSB set
        VX_CHECK(vx::bit::countr_one(static_cast<uint32_t>(0x80000000)) == 0);
    }

    VX_SECTION("countr_one_64")
    {
        // No bits set
        VX_CHECK(vx::bit::countr_one(static_cast<uint64_t>(0x0000000000000000)) == 0);
        // All bits set
        VX_CHECK(vx::bit::countr_one(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF)) == 64);
        // LSB set
        VX_CHECK(vx::bit::countr_one(static_cast<uint64_t>(0x0000000000000001)) == 1);
        // MSB set
        VX_CHECK(vx::bit::countr_one(static_cast<uint64_t>(0x8000000000000000)) == 0);
    }
}

///////////////////////////////////////////////////////////////////////////////
// popcount
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(popcount)
{
    VX_SECTION("popcount_8")
    {
        // No bits set
        VX_CHECK(vx::bit::popcount(static_cast<uint8_t>(0b00000000)) == 0);
        // All bits set
        VX_CHECK(vx::bit::popcount(static_cast<uint8_t>(0b11111111)) == 8);
        // LSB set
        VX_CHECK(vx::bit::popcount(static_cast<uint8_t>(0b00000001)) == 1);
        // MSB set
        VX_CHECK(vx::bit::popcount(static_cast<uint8_t>(0b10000000)) == 1);
        // Alternating
        VX_CHECK(vx::bit::popcount(static_cast<uint8_t>(0b01010101)) == 4);
    }

    VX_SECTION("popcount_16")
    {
        // No bits set
        VX_CHECK(vx::bit::popcount(static_cast<uint16_t>(0x0000)) == 0);
        // All bits set
        VX_CHECK(vx::bit::popcount(static_cast<uint16_t>(0xFFFF)) == 16);
        // LSB set
        VX_CHECK(vx::bit::popcount(static_cast<uint16_t>(0x0001)) == 1);
        // MSB set
        VX_CHECK(vx::bit::popcount(static_cast<uint16_t>(0x8000)) == 1);
        // Alternating
        VX_CHECK(vx::bit::popcount(static_cast<uint16_t>(0x5555)) == 8);
    }

    VX_SECTION("popcount_32")
    {
        // No bits set
        VX_CHECK(vx::bit::popcount(static_cast<uint32_t>(0x00000000)) == 0);
        // All bits set
        VX_CHECK(vx::bit::popcount(static_cast<uint32_t>(0xFFFFFFFF)) == 32);
        // LSB set
        VX_CHECK(vx::bit::popcount(static_cast<uint32_t>(0x00000001)) == 1);
        // MSB set
        VX_CHECK(vx::bit::popcount(static_cast<uint32_t>(0x80000000)) == 1);
        // Alternating
        VX_CHECK(vx::bit::popcount(static_cast<uint32_t>(0x55555555)) == 16);
    }

    VX_SECTION("popcount_64")
    {
        // No bits set
        VX_CHECK(vx::bit::popcount(static_cast<uint64_t>(0x0000000000000000)) == 0);
        // All bits set
        VX_CHECK(vx::bit::popcount(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF)) == 64);
        // LSB set
        VX_CHECK(vx::bit::popcount(static_cast<uint64_t>(0x0000000000000001)) == 1);
        // MSB set
        VX_CHECK(vx::bit::popcount(static_cast<uint64_t>(0x8000000000000000)) == 1);
        // Alternating
        VX_CHECK(vx::bit::popcount(static_cast<uint64_t>(0x5555555555555555)) == 32);
    }
}

///////////////////////////////////////////////////////////////////////////////
// has_single_bit
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(has_single_bit)
{
    VX_SECTION("has_single_bit_8")
    {
        // No bits set
        VX_CHECK(!vx::bit::has_single_bit(static_cast<uint8_t>(0b00000000)));
        // All bits set
        VX_CHECK(!vx::bit::has_single_bit(static_cast<uint8_t>(0b11111111)));
        // LSB set
        VX_CHECK(vx::bit::has_single_bit(static_cast<uint8_t>(0b00000001)));
        // MSB set
        VX_CHECK(vx::bit::has_single_bit(static_cast<uint8_t>(0b00010000)));
    }

    VX_SECTION("has_single_bit_16")
    {
        // No bits set
        VX_CHECK(!vx::bit::has_single_bit(static_cast<uint16_t>(0x0000)));
        // All bits set
        VX_CHECK(!vx::bit::has_single_bit(static_cast<uint16_t>(0xFFFF)));
        // LSB set
        VX_CHECK(vx::bit::has_single_bit(static_cast<uint16_t>(0x0001)));
        // MSB set
        VX_CHECK(vx::bit::has_single_bit(static_cast<uint16_t>(0x0100)));
    }

    VX_SECTION("has_single_bit_32")
    {
        // No bits set
        VX_CHECK(!vx::bit::has_single_bit(static_cast<uint32_t>(0x00000000)));
        // All bits set
        VX_CHECK(!vx::bit::has_single_bit(static_cast<uint32_t>(0xFFFFFFFF)));
        // LSB set
        VX_CHECK(vx::bit::has_single_bit(static_cast<uint32_t>(0x00000001)));
        // MSB set
        VX_CHECK(vx::bit::has_single_bit(static_cast<uint32_t>(0x00010000)));
    }

    VX_SECTION("has_single_bit_64")
    {
        // No bits set
        VX_CHECK(!vx::bit::has_single_bit(static_cast<uint64_t>(0x0000000000000000)));
        // All bits set
        VX_CHECK(!vx::bit::has_single_bit(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF)));
        // LSB set
        VX_CHECK(vx::bit::has_single_bit(static_cast<uint64_t>(0x0000000000000001)));
        // MSB set
        VX_CHECK(vx::bit::has_single_bit(static_cast<uint64_t>(0x0000000100000000)));
    }
}

///////////////////////////////////////////////////////////////////////////////
// bit_width
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(bit_width)
{
    VX_SECTION("bit_width_8")
    {
        // No bits set
        VX_CHECK(vx::bit::bit_width(static_cast<uint8_t>(0b00000000)) == 0);
        // All bits set
        VX_CHECK(vx::bit::bit_width(static_cast<uint8_t>(0b11111111)) == 8);
        // LSB set
        VX_CHECK(vx::bit::bit_width(static_cast<uint8_t>(0b00000001)) == 1);
        // MSB set
        VX_CHECK(vx::bit::bit_width(static_cast<uint8_t>(0b10000000)) == 8);
    }

    VX_SECTION("bit_width_16")
    {
        // No bits set
        VX_CHECK(vx::bit::bit_width(static_cast<uint16_t>(0x0000)) == 0);
        // All bits set
        VX_CHECK(vx::bit::bit_width(static_cast<uint16_t>(0xFFFF)) == 16);
        // LSB set
        VX_CHECK(vx::bit::bit_width(static_cast<uint16_t>(0x0001)) == 1);
        // MSB set
        VX_CHECK(vx::bit::bit_width(static_cast<uint16_t>(0x8000)) == 16);
    }

    VX_SECTION("bit_width_32")
    {
        // No bits set
        VX_CHECK(vx::bit::bit_width(static_cast<uint32_t>(0x00000000)) == 0);
        // All bits set
        VX_CHECK(vx::bit::bit_width(static_cast<uint32_t>(0xFFFFFFFF)) == 32);
        // LSB set
        VX_CHECK(vx::bit::bit_width(static_cast<uint32_t>(0x00000001)) == 1);
        // MSB set
        VX_CHECK(vx::bit::bit_width(static_cast<uint32_t>(0x80000000)) == 32);
    }

    VX_SECTION("bit_width_64")
    {
        // No bits set
        VX_CHECK(vx::bit::bit_width(static_cast<uint64_t>(0x0000000000000000)) == 0);
        // All bits set
        VX_CHECK(vx::bit::bit_width(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF)) == 64);
        // LSB set
        VX_CHECK(vx::bit::bit_width(static_cast<uint64_t>(0x0000000000000001)) == 1);
        // MSB set
        VX_CHECK(vx::bit::bit_width(static_cast<uint64_t>(0x8000000000000000)) == 64);
    }
}

///////////////////////////////////////////////////////////////////////////////
// bit_ceil
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(bit_ceil)
{
    VX_SECTION("bit_ceil_8")
    {
        // No bits set
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint8_t>(0b00000000)) == 0b00000001);
        // All bits set
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint8_t>(0b11111111)) == 0b00000000);
        // LSB set
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint8_t>(0b00000001)) == 0b00000001);
        // MSB set
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint8_t>(0b10000000)) == 0b10000000);
        // Alternating
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint8_t>(0b00010000)) == 0b00010000);
    }

    VX_SECTION("bit_ceil_16")
    {
        // No bits set
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint16_t>(0x0000)) == 0x0001);
        // All bits set
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint16_t>(0xFFFF)) == 0x0000);
        // LSB set
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint16_t>(0x0001)) == 0x0001);
        // MSB set
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint16_t>(0x8000)) == 0x8000);
        // Alternating
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint16_t>(0x0100)) == 0x0100);
    }

    VX_SECTION("bit_ceil_32")
    {
        // No bits set
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint32_t>(0x00000000)) == 0x00000001);
        // All bits set
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint32_t>(0xFFFFFFFF)) == 0x00000001);
        // LSB set
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint32_t>(0x00000001)) == 0x00000001);
        // MSB set
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint32_t>(0x80000000)) == 0x80000000);
        // Alternating
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint32_t>(0x00010000)) == 0x00010000);
    }

    VX_SECTION("bit_ceil_64")
    {
        // No bits set
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint64_t>(0x0000000000000000)) == 0x0000000000000001);
        // All bits set
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF)) == 0x0000000000000001);
        // LSB set
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint64_t>(0x0000000000000001)) == 0x0000000000000001);
        // MSB set
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint64_t>(0x8000000000000000)) == 0x8000000000000000);
        // Alternating
        VX_CHECK(vx::bit::bit_ceil(static_cast<uint64_t>(0x0000000100000000)) == 0x0000000100000000);
    }
}

///////////////////////////////////////////////////////////////////////////////
// bit_floor
///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(bit_floor)
{
    VX_SECTION("bit_floor_8")
    {
        // No bits set
        VX_CHECK(vx::bit::bit_floor(static_cast<uint8_t>(0b00000000)) == 0b00000000);
        // All bits set
        VX_CHECK(vx::bit::bit_floor(static_cast<uint8_t>(0b11111111)) == 0b10000000);
        // LSB set
        VX_CHECK(vx::bit::bit_floor(static_cast<uint8_t>(0b00000001)) == 0b00000001);
        // MSB set
        VX_CHECK(vx::bit::bit_floor(static_cast<uint8_t>(0b10000000)) == 0b10000000);
        // Alternating
        VX_CHECK(vx::bit::bit_floor(static_cast<uint8_t>(0b00010000)) == 0b00010000);
    }

    VX_SECTION("bit_floor_16")
    {
        // No bits set
        VX_CHECK(vx::bit::bit_floor(static_cast<uint16_t>(0x0000)) == 0x0000);
        // All bits set
        VX_CHECK(vx::bit::bit_floor(static_cast<uint16_t>(0xFFFF)) == 0x8000);
        // LSB set
        VX_CHECK(vx::bit::bit_floor(static_cast<uint16_t>(0x0001)) == 0x0001);
        // MSB set
        VX_CHECK(vx::bit::bit_floor(static_cast<uint16_t>(0x8000)) == 0x8000);
        // Alternating
        VX_CHECK(vx::bit::bit_floor(static_cast<uint16_t>(0x0100)) == 0x0100);
    }

    VX_SECTION("bit_floor_32")
    {
        // No bits set
        VX_CHECK(vx::bit::bit_floor(static_cast<uint32_t>(0x00000000)) == 0x00000000);
        // All bits set
        VX_CHECK(vx::bit::bit_floor(static_cast<uint32_t>(0xFFFFFFFF)) == 0x80000000);
        // LSB set
        VX_CHECK(vx::bit::bit_floor(static_cast<uint32_t>(0x00000001)) == 0x00000001);
        // MSB set
        VX_CHECK(vx::bit::bit_floor(static_cast<uint32_t>(0x80000000)) == 0x80000000);
        // Alternating
        VX_CHECK(vx::bit::bit_floor(static_cast<uint32_t>(0x00010000)) == 0x00010000);
    }

    VX_SECTION("bit_floor_64")
    {
        // No bits set
        VX_CHECK(vx::bit::bit_floor(static_cast<uint64_t>(0x0000000000000000)) == 0x0000000000000000);
        // All bits set
        VX_CHECK(vx::bit::bit_floor(static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF)) == 0x8000000000000000);
        // LSB set
        VX_CHECK(vx::bit::bit_floor(static_cast<uint64_t>(0x0000000000000001)) == 0x0000000000000001);
        // MSB set
        VX_CHECK(vx::bit::bit_floor(static_cast<uint64_t>(0x8000000000000000)) == 0x8000000000000000);
        // Alternating
        VX_CHECK(vx::bit::bit_floor(static_cast<uint64_t>(0x0000000100000000)) == 0x0000000100000000);
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    vx::test::test_runner::instance().run();
    return 0;
}