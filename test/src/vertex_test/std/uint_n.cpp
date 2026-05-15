#include "vertex/std/math/uint_n.hpp"
#include "vertex_test/test.hpp"

using namespace vx;

//==============================================================================
// Aliases used throughout
//==============================================================================

using u128_64 = uint_n<128, uint64_t>;
using u128_32 = uint_n<128, uint32_t>;
using u256_64 = uint_n<256, uint64_t>;
using u256_32 = uint_n<256, uint32_t>;
using u64_32 = uint_n<64, uint32_t>;

//==============================================================================

VX_TEST_CASE(test_construction)
{
    VX_SECTION("default construction is zero")
    {
        u256_64 a;
        VX_CHECK(a.is_zero());
        for (size_t i = 0; i < u256_64::limb_count; ++i)
        {
            VX_CHECK(a.limbs[i] == 0);
        }
    }

    VX_SECTION("construct from uint64_t")
    {
        u256_64 a(uint64_t{ 0xDEADBEEFull });
        VX_CHECK(a.low64() == 0xDEADBEEFull);
        VX_CHECK(a.limbs[0] == 0xDEADBEEFull);
        VX_CHECK(a.limbs[1] == 0);
    }

    VX_SECTION("construct from uint32_t")
    {
        u256_64 a(uint32_t{ 0xCAFEu });
        VX_CHECK(a.low64() == 0xCAFEull);
        VX_CHECK(!a.bit(16));
    }

    VX_SECTION("construct zero value")
    {
        u256_64 a(uint64_t{ 0 });
        VX_CHECK(a.is_zero());
    }

    VX_SECTION("widen from narrower uint_n")
    {
        u128_64 narrow(uint64_t{ 0xABCDull });
        u256_64 wide(narrow);
        VX_CHECK(wide.low64() == 0xABCDull);
        VX_CHECK(wide.limbs[2] == 0);
        VX_CHECK(wide.limbs[3] == 0);
    }

    VX_SECTION("truncate from wider uint_n")
    {
        u256_64 wide(uint64_t{ 0xFFFFull });
        wide <<= 64; // put value in limb[1]
        u128_64 narrow(wide);
        VX_CHECK(narrow.limbs[1] == 0xFFFFull);
        VX_CHECK(narrow.limbs[0] == 0);
    }

    VX_SECTION("cross limb-size construction u32 -> u64")
    {
        u256_32 src(uint64_t{ 0xCAFEBABEull });
        u256_64 dst(src);
        VX_CHECK(dst.low64() == 0xCAFEBABEull);
    }

    VX_SECTION("cross limb-size construction u64 -> u32")
    {
        u256_64 src(uint64_t{ 0x12345678ull });
        u256_32 dst(src);
        VX_CHECK(dst.low64() == 0x12345678ull);
    }
}

//==============================================================================

VX_TEST_CASE(test_named_constructors)
{
    VX_SECTION("zero")
    {
        auto z = u256_64::zero();
        VX_CHECK(z.is_zero());
    }

    VX_SECTION("one")
    {
        auto o = u256_64::one();
        VX_CHECK(o.bit(0));
        VX_CHECK(!o.bit(1));
        VX_CHECK(o.low64() == 1);
    }

    VX_SECTION("min")
    {
        auto m = u256_64::min();
        VX_CHECK(m.is_zero());
    }

    VX_SECTION("max")
    {
        auto m = u256_64::max();
        for (size_t i = 0; i < u256_64::limb_count; ++i)
        {
            VX_CHECK(m.limbs[i] == uint64_t(-1));
        }
        VX_CHECK(!m.is_zero());
    }
}

//==============================================================================

VX_TEST_CASE(test_bit_access)
{
    VX_SECTION("set and read individual bits")
    {
        u256_64 a;
        a.set_bit(0, true);
        a.set_bit(63, true);
        a.set_bit(64, true);
        a.set_bit(255, true);

        VX_CHECK(a.bit(0));
        VX_CHECK(a.bit(63));
        VX_CHECK(a.bit(64));
        VX_CHECK(a.bit(255));
        VX_CHECK(!a.bit(1));
        VX_CHECK(!a.bit(62));
        VX_CHECK(!a.bit(65));
        VX_CHECK(!a.bit(254));
    }

    VX_SECTION("clear a set bit")
    {
        u256_64 a;
        a.set_bit(100, true);
        VX_CHECK(a.bit(100));
        a.set_bit(100, false);
        VX_CHECK(!a.bit(100));
        VX_CHECK(a.is_zero());
    }

    VX_SECTION("bit_length")
    {
        u256_64 z;
        VX_CHECK(z.bit_length() == 0);

        u256_64 one(uint64_t{ 1 });
        VX_CHECK(one.bit_length() == 1);

        u256_64 a(uint64_t{ 0xFF });
        VX_CHECK(a.bit_length() == 8);

        u256_64 b;
        b.set_bit(200, true);
        VX_CHECK(b.bit_length() == 201);
    }
}

//==============================================================================

VX_TEST_CASE(test_clz_crz)
{
    VX_SECTION("clz of zero is N")
    {
        u256_64 z;
        VX_CHECK(z.clz() == 256);
    }

    VX_SECTION("clz of 1 is N-1")
    {
        u256_64 a(uint64_t{ 1 });
        VX_CHECK(a.clz() == 255);
    }

    VX_SECTION("clz with high bit set")
    {
        u256_64 a;
        a.set_bit(255, true);
        VX_CHECK(a.clz() == 0);
    }

    VX_SECTION("clz mid-value")
    {
        u256_64 a;
        a.set_bit(100, true);
        VX_CHECK(a.clz() == 155);
    }

    VX_SECTION("crz of zero is N")
    {
        u256_64 z;
        VX_CHECK(z.crz() == 256);
    }

    VX_SECTION("crz of 1 is 0")
    {
        u256_64 a(uint64_t{ 1 });
        VX_CHECK(a.crz() == 0);
    }

    VX_SECTION("crz crosses limb boundary")
    {
        u256_64 a;
        a.set_bit(64, true);
        VX_CHECK(a.crz() == 64);
    }

    VX_SECTION("crz mid-value")
    {
        u256_64 a(uint64_t{ 0x10 });
        VX_CHECK(a.crz() == 4);
    }
}

//==============================================================================

VX_TEST_CASE(test_addition)
{
    VX_SECTION("basic add")
    {
        u256_64 a(uint64_t{ 100 });
        u256_64 b(uint64_t{ 200 });
        u256_64 c = a + b;
        VX_CHECK(c.low64() == 300);
    }

    VX_SECTION("add with carry across limb boundary")
    {
        u256_64 a(uint64_t{ 0xFFFF'FFFF'FFFF'FFFFull });
        u256_64 b(uint64_t{ 1 });
        u256_64 c = a + b;
        VX_CHECK(c.limbs[0] == 0);
        VX_CHECK(c.limbs[1] == 1);
    }

    VX_SECTION("add carry propagates through multiple limbs")
    {
        u256_64 a = u256_64::max();
        u256_64 b(uint64_t{ 1 });
        u256_64 c = a + b;
        VX_CHECK(c.is_zero()); // wraps at 2^256
    }

    VX_SECTION("add_assign")
    {
        u256_64 a(uint64_t{ 50 });
        a += u256_64(uint64_t{ 50 });
        VX_CHECK(a.low64() == 100);
    }

    VX_SECTION("pre-increment")
    {
        u256_64 a(uint64_t{ 41 });
        ++a;
        VX_CHECK(a.low64() == 42);
    }

    VX_SECTION("post-increment")
    {
        u256_64 a(uint64_t{ 9 });
        u256_64 b = a++;
        VX_CHECK(b.low64() == 9);
        VX_CHECK(a.low64() == 10);
    }

    VX_SECTION("add with u32 limbs")
    {
        u256_32 a(uint64_t{ 0xFFFF'FFFF'FFFF'FFFFull });
        u256_32 b(uint64_t{ 1 });
        u256_32 c = a + b;
        VX_CHECK(c.limbs[0] == 0);
        VX_CHECK(c.limbs[1] == 0);
        VX_CHECK(c.low64() == 0);
        VX_CHECK(c.limbs[2] == 1);
    }
}

//==============================================================================

VX_TEST_CASE(test_subtraction)
{
    VX_SECTION("basic sub")
    {
        u256_64 a(uint64_t{ 200 });
        u256_64 b(uint64_t{ 100 });
        u256_64 c = a - b;
        VX_CHECK(c.low64() == 100);
    }

    VX_SECTION("sub with borrow across limb boundary")
    {
        u256_64 a;
        a.limbs[1] = 1; // value = 2^64
        u256_64 b(uint64_t{ 1 });
        u256_64 c = a - b;
        VX_CHECK(c.limbs[0] == 0xFFFF'FFFF'FFFF'FFFFull);
        VX_CHECK(c.limbs[1] == 0);
    }

    VX_SECTION("sub wraps (modular)")
    {
        u256_64 z;
        u256_64 one(uint64_t{ 1 });
        u256_64 r = z - one;
        VX_CHECK(r == u256_64::max());
    }

    VX_SECTION("sub_assign")
    {
        u256_64 a(uint64_t{ 100 });
        a -= u256_64(uint64_t{ 60 });
        VX_CHECK(a.low64() == 40);
    }

    VX_SECTION("pre-decrement")
    {
        u256_64 a(uint64_t{ 10 });
        --a;
        VX_CHECK(a.low64() == 9);
    }

    VX_SECTION("post-decrement")
    {
        u256_64 a(uint64_t{ 10 });
        u256_64 b = a--;
        VX_CHECK(b.low64() == 10);
        VX_CHECK(a.low64() == 9);
    }
}

//==============================================================================

VX_TEST_CASE(test_multiplication)
{
    VX_SECTION("basic multiply")
    {
        u256_64 a(uint64_t{ 12345 });
        u256_64 b(uint64_t{ 67890 });
        u256_64 c = a * b;
        VX_CHECK(c.low64() == 12345ULL * 67890ULL);
    }

    VX_SECTION("multiply by zero")
    {
        u256_64 a(uint64_t{ 0xDEADBEEFull });
        u256_64 z;
        VX_CHECK((a * z).is_zero());
        VX_CHECK((z * a).is_zero());
    }

    VX_SECTION("multiply by one")
    {
        u256_64 a(uint64_t{ 0x1234567890ABCDEFull });
        u256_64 one = u256_64::one();
        VX_CHECK(a * one == a);
        VX_CHECK(one * a == a);
    }

    VX_SECTION("multiply crosses limb boundary")
    {
        u256_64 a(uint64_t{ 0xFFFF'FFFF'FFFF'FFFFull });
        u256_64 two(uint64_t{ 2 });
        u256_64 c = a * two;
        // 2 * (2^64 - 1) = 2^65 - 2
        VX_CHECK(c.limbs[0] == 0xFFFF'FFFF'FFFF'FFFEull);
        VX_CHECK(c.limbs[1] == 1);
    }

    VX_SECTION("multiply result spans multiple limbs")
    {
        u256_64 a;
        a.set_bit(127, true); // a = 2^127
        u256_64 two(uint64_t{ 2 });
        u256_64 c = a * two; // = 2^128
        VX_CHECK(c.bit(128));
        VX_CHECK(!c.bit(127));
    }

    VX_SECTION("multiply wraps at 2^N")
    {
        u256_64 half;
        half.set_bit(255, true); // 2^255
        u256_64 two(uint64_t{ 2 });
        VX_CHECK((half * two).is_zero());
    }

    VX_SECTION("multiply_assign")
    {
        u256_64 a(uint64_t{ 7 });
        a *= u256_64(uint64_t{ 6 });
        VX_CHECK(a.low64() == 42);
    }

    VX_SECTION("multiply with u32 limbs")
    {
        u256_32 a(uint64_t{ 999 });
        u256_32 b(uint64_t{ 1000 });
        VX_CHECK((a * b).low64() == 999000ULL);
    }
}

//==============================================================================

VX_TEST_CASE(test_division)
{
    VX_SECTION("basic divmod")
    {
        u256_64 a(uint64_t{ 100 });
        u256_64 b(uint64_t{ 10 });
        auto res = divmod(a, b);
        VX_CHECK(res.quotient.low64() == 10);
        VX_CHECK(res.remainder.is_zero());
    }

    VX_SECTION("divmod with remainder")
    {
        u256_64 a(uint64_t{ 101 });
        u256_64 b(uint64_t{ 10 });
        auto res = divmod(a, b);
        VX_CHECK(res.quotient.low64() == 10);
        VX_CHECK(res.remainder.low64() == 1);
    }

    VX_SECTION("divide by one")
    {
        u256_64 a(uint64_t{ 0xABCDull });
        u256_64 one = u256_64::one();
        auto res = divmod(a, one);
        VX_CHECK(res.quotient == a);
        VX_CHECK(res.remainder.is_zero());
    }

    VX_SECTION("divide value smaller than divisor")
    {
        u256_64 a(uint64_t{ 5 });
        u256_64 b(uint64_t{ 10 });
        auto res = divmod(a, b);
        VX_CHECK(res.quotient.is_zero());
        VX_CHECK(res.remainder == a);
    }

    VX_SECTION("divide equal values")
    {
        u256_64 a(uint64_t{ 42 });
        auto res = divmod(a, a);
        VX_CHECK(res.quotient.low64() == 1);
        VX_CHECK(res.remainder.is_zero());
    }

    VX_SECTION("division crosses limb boundary")
    {
        // Build 2^64 + 1 then divide by 2
        u256_64 a;
        a.limbs[1] = 1;
        a.limbs[0] = 1;
        u256_64 two(uint64_t{ 2 });
        auto res = divmod(a, two);
        // (2^64 + 1) / 2 = 2^63,  remainder 1
        VX_CHECK(res.quotient.bit(63));
        VX_CHECK(!res.quotient.bit(64));
        VX_CHECK(res.remainder.low64() == 1);
    }

    VX_SECTION("operator/ and operator%")
    {
        u256_64 a(uint64_t{ 77 });
        u256_64 b(uint64_t{ 8 });
        VX_CHECK((a / b).low64() == 9);
        VX_CHECK((a % b).low64() == 5);
    }
}

//==============================================================================

VX_TEST_CASE(test_bitwise)
{
    VX_SECTION("AND")
    {
        u256_64 a(uint64_t{ 0b1100 });
        u256_64 b(uint64_t{ 0b1010 });
        VX_CHECK((a & b).low64() == 0b1000);
    }

    VX_SECTION("OR")
    {
        u256_64 a(uint64_t{ 0b1100 });
        u256_64 b(uint64_t{ 0b1010 });
        VX_CHECK((a | b).low64() == 0b1110);
    }

    VX_SECTION("XOR")
    {
        u256_64 a(uint64_t{ 0b1100 });
        u256_64 b(uint64_t{ 0b1010 });
        VX_CHECK((a ^ b).low64() == 0b0110);
    }

    VX_SECTION("NOT")
    {
        u256_64 z;
        VX_CHECK(~z == u256_64::max());
        VX_CHECK(~u256_64::max() == z);
    }

    VX_SECTION("assign operators")
    {
        u256_64 a(uint64_t{ 0xFF });
        a &= u256_64(uint64_t{ 0x0F });
        VX_CHECK(a.low64() == 0x0F);

        a |= u256_64(uint64_t{ 0xF0 });
        VX_CHECK(a.low64() == 0xFF);

        a ^= u256_64(uint64_t{ 0xFF });
        VX_CHECK(a.is_zero());
    }
}

//==============================================================================

VX_TEST_CASE(test_shifts)
{
    VX_SECTION("shift left within one limb")
    {
        u256_64 a(uint64_t{ 1 });
        a <<= 10;
        VX_CHECK(a.low64() == (1ull << 10));
    }

    VX_SECTION("shift left across limb boundary")
    {
        u256_64 a(uint64_t{ 1 });
        a <<= 64;
        VX_CHECK(a.limbs[0] == 0);
        VX_CHECK(a.limbs[1] == 1);
    }

    VX_SECTION("shift left to highest bit")
    {
        u256_64 a(uint64_t{ 1 });
        a <<= 255;
        VX_CHECK(a.bit(255));
        VX_CHECK(a.clz() == 0);
    }

    VX_SECTION("shift left by N zeroes result")
    {
        u256_64 a(uint64_t{ 0xDEAD });
        a <<= 256;
        VX_CHECK(a.is_zero());
    }

    VX_SECTION("shift left by zero is identity")
    {
        u256_64 a(uint64_t{ 0x1234 });
        a <<= 0;
        VX_CHECK(a.low64() == 0x1234);
    }

    VX_SECTION("shift right within one limb")
    {
        u256_64 a(uint64_t{ 0x100 });
        a >>= 4;
        VX_CHECK(a.low64() == 0x10);
    }

    VX_SECTION("shift right across limb boundary")
    {
        u256_64 a;
        a.limbs[1] = 1; // value = 2^64
        a >>= 64;
        VX_CHECK(a.low64() == 1);
        VX_CHECK(a.limbs[1] == 0);
    }

    VX_SECTION("shift right by N zeroes result")
    {
        u256_64 a(uint64_t{ 0xDEAD });
        a >>= 256;
        VX_CHECK(a.is_zero());
    }

    VX_SECTION("shift right by zero is identity")
    {
        u256_64 a(uint64_t{ 0x5678 });
        a >>= 0;
        VX_CHECK(a.low64() == 0x5678);
    }

    VX_SECTION("non-assign shift operators")
    {
        u256_64 a(uint64_t{ 1 });
        VX_CHECK((a << 8).low64() == 256);
        VX_CHECK((a << 8 >> 8).low64() == 1);
    }

    VX_SECTION("shift with partial limb overlap")
    {
        // Shift by an amount that splits a limb (e.g. 70 = 64 + 6)
        u256_64 a(uint64_t{ 0b11 });
        u256_64 b = a << 63;
        VX_CHECK(b.bit(63)); // bit 64 and 63 should be set
        VX_CHECK(b.bit(64));
        u256_64 c = b >> 63;
        VX_CHECK(c == a);
    }
}

//==============================================================================

VX_TEST_CASE(test_comparisons)
{
    VX_SECTION("equality")
    {
        u256_64 a(uint64_t{ 42 });
        u256_64 b(uint64_t{ 42 });
        u256_64 c(uint64_t{ 43 });
        VX_CHECK(a == b);
        VX_CHECK(!(a == c));
        VX_CHECK(a != c);
        VX_CHECK(!(a != b));
    }

    VX_SECTION("less than")
    {
        u256_64 lo(uint64_t{ 1 });
        u256_64 hi(uint64_t{ 2 });
        VX_CHECK(lo < hi);
        VX_CHECK(!(hi < lo));
        VX_CHECK(!(lo < lo));
    }

    VX_SECTION("greater than")
    {
        u256_64 lo(uint64_t{ 10 });
        u256_64 hi(uint64_t{ 20 });
        VX_CHECK(hi > lo);
        VX_CHECK(!(lo > hi));
    }

    VX_SECTION("less-or-equal / greater-or-equal")
    {
        u256_64 a(uint64_t{ 5 });
        u256_64 b(uint64_t{ 5 });
        u256_64 c(uint64_t{ 6 });
        VX_CHECK(a <= b);
        VX_CHECK(a <= c);
        VX_CHECK(c >= a);
        VX_CHECK(a >= b);
        VX_CHECK(!(c <= a));
    }

    VX_SECTION("comparison crosses limb boundary")
    {
        u256_64 lo(uint64_t{ 0xFFFF'FFFF'FFFF'FFFFull });
        u256_64 hi;
        hi.limbs[1] = 1; // hi = 2^64 > 2^64-1
        VX_CHECK(lo < hi);
        VX_CHECK(hi > lo);
    }

    VX_SECTION("max > everything else")
    {
        u256_64 m = u256_64::max();
        u256_64 a(uint64_t{ 0xDEADBEEFull });
        VX_CHECK(m > a);
        VX_CHECK(a < m);
        VX_CHECK(m >= m);
    }
}

//==============================================================================

VX_TEST_CASE(test_low64)
{
    VX_SECTION("low64 with u64 limbs")
    {
        u256_64 a(uint64_t{ 0xDEADBEEFCAFEull });
        VX_CHECK(a.low64() == 0xDEADBEEFCAFEull);
    }

    VX_SECTION("low64 with u32 limbs")
    {
        u256_32 a(uint64_t{ 0xAABBCCDD11223344ull });
        VX_CHECK(a.low64() == 0xAABBCCDD11223344ull);
    }

    VX_SECTION("low64 ignores upper limbs")
    {
        u256_64 a;
        a.limbs[1] = 0xFFFFFFFF;
        a.limbs[0] = 0x1234;
        VX_CHECK(a.low64() == 0x1234);
    }
}

//==============================================================================

VX_TEST_CASE(test_static_properties)
{
    VX_SECTION("u256_64 properties")
    {
        VX_STATIC_CHECK(u256_64::bits == 256);
        VX_STATIC_CHECK(u256_64::limb_bits == 64);
        VX_STATIC_CHECK(u256_64::limb_count == 4);
    }

    VX_SECTION("u256_32 properties")
    {
        VX_STATIC_CHECK(u256_32::bits == 256);
        VX_STATIC_CHECK(u256_32::limb_bits == 32);
        VX_STATIC_CHECK(u256_32::limb_count == 8);
    }

    VX_SECTION("u128_64 properties")
    {
        VX_STATIC_CHECK(u128_64::bits == 128);
        VX_STATIC_CHECK(u128_64::limb_bits == 64);
        VX_STATIC_CHECK(u128_64::limb_count == 2);
    }

    VX_SECTION("u64_32 properties")
    {
        VX_STATIC_CHECK(u64_32::bits == 64);
        VX_STATIC_CHECK(u64_32::limb_bits == 32);
        VX_STATIC_CHECK(u64_32::limb_count == 2);
    }
}

//==============================================================================

VX_TEST_CASE(test_add_free_function)
{
    VX_SECTION("add with carry-in")
    {
        u256_64 a(uint64_t{ 0xFFFF'FFFF'FFFF'FFFFull });
        u256_64 b(uint64_t{ 0 });
        u256_64 r;
        uint64_t cout = add(a, b, r, uint64_t{ 1 });
        VX_CHECK(r.limbs[0] == 0);
        VX_CHECK(r.limbs[1] == 1);
        VX_CHECK(cout == 0);
    }

    VX_SECTION("add produces carry-out")
    {
        u256_64 m = u256_64::max();
        u256_64 one(uint64_t{ 1 });
        u256_64 r;
        uint64_t cout = add(m, one, r);
        VX_CHECK(r.is_zero());
        VX_CHECK(cout == 1);
    }
}

//==============================================================================

VX_TEST_CASE(test_sub_free_function)
{
    VX_SECTION("sub with borrow-in")
    {
        u256_64 a(uint64_t{ 10 });
        u256_64 b(uint64_t{ 9 });
        u256_64 r;
        uint64_t bout = sub(a, b, r, uint64_t{ 1 }); // 10 - 9 - 1 = 0
        VX_CHECK(r.is_zero());
        VX_CHECK(bout == 0);
    }

    VX_SECTION("sub produces borrow-out")
    {
        u256_64 z;
        u256_64 one(uint64_t{ 1 });
        u256_64 r;
        uint64_t bout = sub(z, one, r);
        VX_CHECK(r == u256_64::max());
        VX_CHECK(bout == 1);
    }
}

//==============================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
