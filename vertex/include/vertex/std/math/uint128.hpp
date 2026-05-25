#pragma once

//#include <cstdint>
//#include <cstring>
//#include <limits>
//#include <stdexcept>
//#include <string>
//#include <type_traits>
//
//#include "vertex/config/type_traits.hpp"
//#include "vertex/util/bit/bit.hpp"
//#include "vertex/util/bit/endian.hpp"
//
//namespace vx {
//
////==============================================================================
//// intrinsics
////==============================================================================
//
//#if defined(__SIZEOF_INT128__)
//    #define _VX_UINT128_HAS_INT128 1
//#endif
//
//#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_AMD64))
//    #include <intrin.h>
//    #define _VX_UINT128_HAS_MSVC_INTRINSICS 1
//#endif
//
////==============================================================================
//// base type
////==============================================================================
//
//namespace _uint128_priv {
//
//template <typename base>
//struct divmod_result_t
//{
//    base quotient;
//    base remainder;
//    bool division_by_zero;
//};
//
//#if defined(_VX_UINT128_HAS_INT128)
//
//using uint128_base = unsigned __int128;
//
//#else
//
//struct uint128_base
//{
//    #if (VX_ORDER_NATIVE_ENDIAN == VX_ORDER_BIG_ENDIAN)
//
//    uint64_t hi, lo;
//
//    #else
//
//    uint64_t lo, hi;
//
//    #endif
//};
//
//struct uint128_traits
//{
//    using base = uint128_base;
//    using divmod_result = divmod_result_t<base>;
//
//    static inline constexpr base construct(uint64_t hi, uint64_t lo) noexcept
//    {
//    #if (VX_ORDER_NATIVE_ENDIAN == VX_ORDER_BIG_ENDIAN)
//
//        return base{ hi, lo };
//
//    #else
//
//        return base{ lo, hi };
//
//    #endif
//    }
//
//    template <typename T, VX_REQUIRES(std::is_arithmetic<T>::value)>
//    static constexpr base construct(T v) noexcept
//    {
//        VX_STATIC_ASSERT_MSG(sizeof(T) <= 128, "construct: T too wide for uint128");
//        return construct(0, static_cast<uint64_t>(v));
//    }
//
//    template <typename T, VX_REQUIRES(std::is_arithmetic<T>::value)>
//    static constexpr T to_arithmetic(const base& b) noexcept
//    {
//        VX_STATIC_ASSERT_MSG(sizeof(T) <= 128, "to_arithmetic: T too wide for uint128");
//        return static_cast<T>(get_lo(b));
//    }
//
//    static inline constexpr uint64_t get_lo(const base& b) noexcept
//    {
//        return b.lo;
//    }
//
//    static inline constexpr uint64_t get_hi(const base& b) noexcept
//    {
//        return b.hi;
//    }
//
//    static inline constexpr bool is_zero(const base& b) noexcept
//    {
//        return b.lo == 0 && b.hi == 0;
//    }
//
//    static inline constexpr bool eq(const base& a, const base& b) noexcept
//    {
//        return a.lo == b.lo && a.hi == b.hi;
//    }
//
//    static inline constexpr bool lt(const base& a, const base& b) noexcept
//    {
//        return a.hi != b.hi ? a.hi < b.hi : a.lo < b.lo;
//    }
//
//    static inline constexpr base bitwise_not(const base& b) noexcept
//    {
//        return construct(~b.hi, ~b.lo);
//    }
//
//    static inline constexpr base bitwise_and(const base& a, const base& b) noexcept
//    {
//        return construct(a.hi & b.hi, a.lo & b.lo);
//    }
//
//    static inline constexpr base bitwise_or(const base& a, const base& b) noexcept
//    {
//        return construct(a.hi | b.hi, a.lo | b.lo);
//    }
//
//    static inline constexpr base bitwise_xor(const base& a, const base& b) noexcept
//    {
//        return construct(a.hi ^ b.hi, a.lo ^ b.lo);
//    }
//
//    static inline constexpr base shift_left(const base& b, unsigned n) noexcept
//    {
//        return construct((b.hi << n) | (b.lo >> (64 - n)), b.lo << n);
//    }
//
//    static inline constexpr base shift_right(const base& b, unsigned n) noexcept
//    {
//        return construct(b.hi >> n, (b.lo >> n) | (b.hi << (64 - n)));
//    }
//
//    static inline constexpr base add(const base& a, const base& b) noexcept
//    {
//    #if defined(_VX_UINT128_HAS_MSVC_INTRINSICS)
//
//        if (!VX_IS_CONSTANT_EVALUATED())
//        {
//            base r;
//            r.lo = _addcarry_u64(0, a.lo, b.lo, &r.lo);
//            // _addcarry_u64 returns the carry; use it for hi
//            const unsigned char carry = _addcarry_u64(0, a.lo, b.lo, &r.lo);
//            _addcarry_u64(carry, a.hi, b.hi, &r.hi);
//            return r;
//        }
//
//    #endif // _VX_UINT128_HAS_MSVC_INTRINSICS
//
//        const uint64_t lo = a.lo + b.lo;
//        const uint64_t hi = a.hi + b.hi + (lo < a.lo ? 1 : 0);
//        return construct(hi, lo);
//    }
//
//    static inline constexpr base subtract(const base& a, const base& b) noexcept
//    {
//    #if defined(_VX_UINT128_HAS_MSVC_INTRINSICS)
//
//        if (!VX_IS_CONSTANT_EVALUATED())
//        {
//            base r;
//            const unsigned char borrow = _subborrow_u64(0, a.lo, b.lo, &r.lo);
//            _subborrow_u64(borrow, a.hi, b.hi, &r.hi);
//            return r;
//        }
//
//    #endif // _VX_UINT128_HAS_MSVC_INTRINSICS
//
//        const uint64_t lo = a.lo - b.lo;
//        const uint64_t hi = a.hi - b.hi - (a.lo < b.lo ? 1 : 0);
//        return construct(hi, lo);
//    }
//
//    static inline constexpr base multiply(const base& a, const base& b) noexcept
//    {
//    #if defined(_VX_UINT128_HAS_MSVC_INTRINSICS)
//
//        if (!VX_IS_CONSTANT_EVALUATED())
//        {
//            uint64_t p_hi;
//            const uint64_t p_lo = _umul128(a.lo, b.lo, &p_hi);
//            p_hi += a.lo * b.hi + a.hi * b.lo;
//            return construct(p_hi, p_lo);
//        }
//
//    #endif // _VX_UINT128_HAS_MSVC_INTRINSICS
//
//        const uint64_t p0 = a.lo * b.lo;
//        const uint64_t p1 = a.lo * b.hi;
//        const uint64_t p2 = a.hi * b.lo;
//        const uint64_t p3 = a.hi * b.hi;
//        const uint64_t mid = (p0 >> 32) + (p1 & 0xFFFFFFFFu) + (p2 & 0xFFFFFFFFu);
//        const uint64_t lo = (p0 & 0xFFFFFFFFu) | (mid << 32);
//        const uint64_t hi = p3 + (p1 >> 32) + (p2 >> 32) + (mid >> 32);
//        return construct(hi, lo);
//    }
//
//    static inline constexpr int clz(const base& b) noexcept
//    {
//        if (b.hi)
//        {
//            return bit::countl_zero(b.hi);
//        }
//        else
//        {
//            return 64 + bit::countl_zero(b.lo);
//        }
//    }
//
//    static inline constexpr divmod_result divmod(const base& dividend, const base& divisor) noexcept
//    {
//        divmod_result result{ construct(0, 0), construct(0, 0), false };
//        if (is_zero(divisor))
//        {
//            result.division_by_zero = true;
//            return result;
//        }
//
//        result.remainder = dividend;
//
//    }
//};
//
//#endif
//
//} // namespace _uint128_priv
//
////==============================================================================
//// uint128
////=====================================
//
//struct uint128_t
//{
//    //=====================================
//    // data
//    //=====================================
//
//    _uint128_priv::uint128_base data;
//
//    //=====================================
//    // helpers (internal)
//    //=====================================
//
//private:
//
//    // Return the low 64 bits.
//    constexpr uint64_t lo() const noexcept
//    {
//#if defined(_VX_UINT128_HAS_INT128)
//        return static_cast<uint64_t>(data);
//#else
//        return data.lo;
//#endif
//    }
//
//    // Return the high 64 bits.
//    constexpr uint64_t hi() const noexcept
//    {
//#if defined(_VX_UINT128_HAS_INT128)
//        return static_cast<uint64_t>(data >> 64);
//#else
//        return data.hi;
//#endif
//    }
//
//    // Construct from raw hi/lo (private shorthand).
//    static constexpr uint128_t from_hi_lo(uint64_t lo, uint64_t hi) noexcept
//    {
//        return uint128_t(lo, hi);
//    }
//
//public:
//
//    //=====================================
//    // constructors
//    //=====================================
//
//    //=====================================
//    // constructors
//    //=====================================
//
//    constexpr uint128_t() noexcept
//        : data{}
//    {}
//
//    constexpr uint128_t(uint64_t lo, uint64_t hi) noexcept
//    {
//#if defined(_VX_UINT128_HAS_INT128)
//        data = (static_cast<_uint128_priv::uint128_base>(hi) << 64) | static_cast<_uint128_priv::uint128_base>(lo);
//#else
//        data.lo = lo;
//        data.hi = hi;
//#endif
//    }
//
//    // Implicit construction from any integer type
//    template <typename T, VX_REQUIRES(std::is_integral<T>::value)>
//    constexpr uint128_t(T v) noexcept
//        : lo(static_cast<uint64_t>(v))
//        , hi(std::is_signed<T>::value && v < 0 ? ~uint64_t(0) : uint64_t(0))
//    {}
//
//    // -- explicit conversions --------------------------------------------------
//
//    explicit constexpr operator bool() const noexcept
//    {
//        return hi || lo;
//    }
//
//    template <typename T, VX_REQUIRES(std::is_integral<T>::value)>
//    explicit constexpr operator T() const noexcept
//    {
//        VX_IF_CONSTEXPR (sizeof(T) <= sizeof(uint64_t))
//        {
//            return static_cast<T>(lo);
//        }
//        else
//        {
//            return static_cast<T>((static_cast<T>(hi) << 64) | lo);
//        }
//    }
//
//    // -- accessors -------------------------------------------------------------
//
//    constexpr uint64_t high() const noexcept
//    {
//        return hi;
//    }
//
//    constexpr uint64_t low() const noexcept
//    {
//        return lo;
//    }
//
//    // -------------------------------------------------------------------------
//    //  Comparison operators
//    // -------------------------------------------------------------------------
//
//    constexpr bool operator==(const uint128_t& o) const noexcept
//    {
//        return hi == o.hi && lo == o.lo;
//    }
//
//    constexpr bool operator!=(const uint128_t& o) const noexcept
//    {
//        return !(*this == o);
//    }
//
//    constexpr bool operator<(const uint128_t& o) const noexcept
//    {
//        return hi != o.hi ? hi < o.hi : lo < o.lo;
//    }
//
//    constexpr bool operator<=(const uint128_t& o) const noexcept
//    {
//        return !(o < *this);
//    }
//
//    constexpr bool operator>(const uint128_t& o) const noexcept
//    {
//        return o < *this;
//    }
//
//    constexpr bool operator>=(const uint128_t& o) const noexcept
//    {
//        return !(*this < o);
//    }
//
//    // -------------------------------------------------------------------------
//    //  Bitwise operators
//    // -------------------------------------------------------------------------
//
//    constexpr uint128_t operator~() const noexcept
//    {
//        return { ~lo, ~hi };
//    }
//
//    constexpr uint128_t operator&(const uint128_t& o) const noexcept
//    {
//        return { lo & o.lo, hi & o.hi };
//    }
//
//    constexpr uint128_t operator|(const uint128_t& o) const noexcept
//    {
//        return { lo | o.lo, hi | o.hi };
//    }
//
//    constexpr uint128_t operator^(const uint128_t& o) const noexcept
//    {
//        return { lo ^ o.lo, hi ^ o.hi };
//    }
//
//    constexpr uint128_t& operator&=(const uint128_t& o) noexcept
//    {
//        return *this = *this & o;
//    }
//
//    constexpr uint128_t& operator|=(const uint128_t& o) noexcept
//    {
//        return *this = *this | o;
//    }
//
//    constexpr uint128_t& operator^=(const uint128_t& o) noexcept
//    {
//        return *this = *this ^ o;
//    }
//
//    // -------------------------------------------------------------------------
//    //  Shift operators
//    // -------------------------------------------------------------------------
//
//    constexpr uint128_t operator<<(unsigned n) const noexcept
//    {
//        return { lo << n, (hi << n) | (lo >> (64 - n)) };
//    }
//
//    constexpr uint128_t operator>>(unsigned n) const noexcept
//    {
//        return { (lo >> n) | (hi << (64 - n)), hi >> n };
//    }
//
//    constexpr uint128_t& operator<<=(unsigned n) noexcept
//    {
//        return *this = *this << n;
//    }
//
//    constexpr uint128_t& operator>>=(unsigned n) noexcept
//    {
//        return *this = *this >> n;
//    }
//
//    // -------------------------------------------------------------------------
//    //  Addition / subtraction
//    // -------------------------------------------------------------------------
//
//private:
//
//    static constexpr uint128_t add_portable(const uint128_t& a, const uint128_t& b) noexcept
//    {
//        const uint64_t lo = a.lo + b.lo;
//        const uint64_t carry = (lo < a.lo) ? 1u : 0u;
//        return { lo, a.hi + b.hi + carry };
//    }
//
//    static constexpr uint128_t sub_portable(const uint128_t& a, const uint128_t& b) noexcept
//    {
//        const uint64_t lo = a.lo - b.lo;
//        const uint64_t borrow = (a.lo < b.lo) ? 1u : 0u;
//        return { lo, a.hi - b.hi - borrow };
//    }
//
//public:
//
//    constexpr uint128_t operator+(const uint128_t& o) const noexcept
//    {
//#if defined(_VX_HAS_MSVC_INTRINSICS)
//
//        if (!VX_IS_CONSTANT_EVALUATED())
//        {
//            uint128_t r;
//            r.lo = _addcarry_u64(0, lo, o.lo, &r.lo);
//            // _addcarry_u64 returns the carry; use it for hi
//            unsigned char c = _addcarry_u64(0, lo, o.lo, &r.lo);
//            _addcarry_u64(c, hi, o.hi, &r.hi);
//            return r;
//        }
//
//#endif // _VX_HAS_MSVC_INTRINSICS
//
//        return add_portable(*this, o);
//    }
//    constexpr uint128_t operator-(const uint128_t& o) const noexcept
//    {
//#if defined(_VX_HAS_MSVC_INTRINSICS)
//
//        if (!VX_IS_CONSTANT_EVALUATED())
//        {
//            uint128_t r;
//            unsigned char b = _subborrow_u64(0, lo, o.lo, &r.lo);
//            _subborrow_u64(b, hi, o.hi, &r.hi);
//            return r;
//        }
//
//#endif // _VX_HAS_MSVC_INTRINSICS
//
//        return sub_portable(*this, o);
//    }
//
//    constexpr uint128_t& operator+=(const uint128_t& o) noexcept
//    {
//        return *this = *this + o;
//    }
//
//    constexpr uint128_t& operator-=(const uint128_t& o) noexcept
//    {
//        return *this = *this - o;
//    }
//
//    constexpr uint128_t operator-() const noexcept
//    {
//        return (~*this) + uint128_t(1u);
//    }
//
//    constexpr uint128_t& operator++() noexcept
//    {
//        return *this += 1u;
//    }
//
//    constexpr uint128_t& operator--() noexcept
//    {
//        return *this -= 1u;
//    }
//
//    constexpr uint128_t operator++(int) noexcept
//    {
//        auto t = *this;
//        ++*this;
//        return t;
//    }
//
//    constexpr uint128_t operator--(int) noexcept
//    {
//        auto t = *this;
//        --*this;
//        return t;
//    }
//
//    // -------------------------------------------------------------------------
//    //  Multiplication
//    //  Strategy: split each 64-bit half into two 32-bit limbs and do a
//    //  schoolbook 4×4 partial-product accumulation.  With intrinsics on x64
//    //  we can use _umul128 (MSVC) or __uint128_t (GCC/Clang) for the
//    //  64×64→128-bit inner product.
//    // -------------------------------------------------------------------------
//
//private:
//
//    // 64×64 → 128 portable (always constexpr)
//    static constexpr uint128_t mul64x64_portable(uint64_t a, uint64_t b) noexcept
//    {
//        const uint64_t a_lo = a & 0xFFFFFFFFu, a_hi = a >> 32;
//        const uint64_t b_lo = b & 0xFFFFFFFFu, b_hi = b >> 32;
//        const uint64_t p0 = a_lo * b_lo;
//        const uint64_t p1 = a_lo * b_hi;
//        const uint64_t p2 = a_hi * b_lo;
//        const uint64_t p3 = a_hi * b_hi;
//        const uint64_t mid = (p0 >> 32) + (p1 & 0xFFFFFFFFu) + (p2 & 0xFFFFFFFFu);
//        const uint64_t lo = (p0 & 0xFFFFFFFFu) | (mid << 32);
//        const uint64_t hi = p3 + (p1 >> 32) + (p2 >> 32) + (mid >> 32);
//        return { lo, hi };
//    }
//
//    static uint128_t mul64x64(uint64_t a, uint64_t b) noexcept
//    {
//#if defined(_VX_HAS_MSVC_INTRINSICS)
//
//        uint64_t hi;
//        uint64_t lo = _umul128(a, b, &hi);
//        return { lo, hi };
//
//#elif defined(_VX_HAS_INT128)
//
//        // __uint128_t is available but __VX_UINT128_USE_NATIVE is 0 (user forced soft).
//        // Still useful as a fast internal helper.
//        unsigned __int128 p = (unsigned __int128)a * b;
//        return { (uint64_t)p, (uint64_t)(p >> 64) };
//
//#else
//
//        return mul64x64_portable(a, b);
//
//#endif
//    }
//
//public:
//
//    constexpr uint128_t operator*(const uint128_t& o) const noexcept
//    {
//        // Result = (hi*2^64 + lo) * (o.hi*2^64 + o.lo)
//        //        = lo*o.lo  +  (lo*o.hi + hi*o.lo)*2^64   (upper 128 bits drop)
//        if (VX_IS_CONSTANT_EVALUATED())
//        {
//            uint128_t p = mul64x64_portable(lo, o.lo);
//            p.hi += lo * o.hi + hi * o.lo;
//            return p;
//        }
//        else
//        {
//            uint128_t p = mul64x64(lo, o.lo);
//            p.hi += lo * o.hi + hi * o.lo;
//            return p;
//        }
//    }
//
//    constexpr uint128_t& operator*=(const uint128_t& o) noexcept
//    {
//        return *this = *this * o;
//    }
//
//    // -------------------------------------------------------------------------
//    //  Division and modulo (portable restoring long-division)
//    // -------------------------------------------------------------------------
//
//private:
//
//    // Returns {quotient, remainder}
//    static constexpr uint128_t divmod(const uint128_t& num, const uint128_t& den, uint128_t& rem)
//    {
//        if (!den)
//        {
//            // Division by zero — mirror undefined behaviour of unsigned int:
//            // return all-ones (implementation-defined but deterministic)
//            rem = num;
//            return ~uint128_t(0u);
//        }
//        if (num < den)
//        {
//            rem = num;
//            return {};
//        }
//        if (den.hi == 0 && den.lo == 1u)
//        {
//            rem = {};
//            return num;
//        }
//
//        // Count leading zeros for normalisation
//        auto clz128 = [](const uint128_t& v) constexpr -> unsigned
//        {
//            if (v.hi)
//            {
//#if defined(__GNUC__) || defined(__clang__)
//                return (unsigned)__builtin_clzll(v.hi);
//#else
//                unsigned n = 0;
//                uint64_t x = v.hi;
//                if (!(x >> 32))
//                {
//                    n += 32;
//                    x <<= 32;
//                }
//                if (!(x >> 48))
//                {
//                    n += 16;
//                    x <<= 16;
//                }
//                if (!(x >> 56))
//                {
//                    n += 8;
//                    x <<= 8;
//                }
//                if (!(x >> 60))
//                {
//                    n += 4;
//                    x <<= 4;
//                }
//                if (!(x >> 62))
//                {
//                    n += 2;
//                    x <<= 2;
//                }
//                if (!(x >> 63))
//                {
//                    n += 1;
//                }
//                return n;
//#endif
//            }
//            else
//            {
//#if defined(__GNUC__) || defined(__clang__)
//                return 64u + (unsigned)__builtin_clzll(v.lo);
//#else
//                unsigned n = 64;
//                uint64_t x = v.lo;
//                if (!(x >> 32))
//                {
//                    n += 32;
//                    x <<= 32;
//                }
//                if (!(x >> 48))
//                {
//                    n += 16;
//                    x <<= 16;
//                }
//                if (!(x >> 56))
//                {
//                    n += 8;
//                    x <<= 8;
//                }
//                if (!(x >> 60))
//                {
//                    n += 4;
//                    x <<= 4;
//                }
//                if (!(x >> 62))
//                {
//                    n += 2;
//                    x <<= 2;
//                }
//                if (!(x >> 63))
//                {
//                    n += 1;
//                }
//                return n;
//#endif
//            }
//        };
//
//        unsigned shift = clz128(den) - clz128(num);
//        uint128_t d = den << shift;
//        uint128_t q;
//        rem = num;
//        for (;;)
//        {
//            if (rem >= d)
//            {
//                rem -= d;
//                q |= uint128_t(1u) << shift;
//            }
//            if (shift == 0)
//                break;
//            d >>= 1;
//            --shift;
//        }
//        return q;
//    }
//
//public:
//
//    constexpr uint128_t operator/(const uint128_t& o) const
//    {
//        uint128_t rem;
//        return divmod(*this, o, rem);
//    }
//    constexpr uint128_t operator%(const uint128_t& o) const
//    {
//        uint128_t rem;
//        divmod(*this, o, rem);
//        return rem;
//    }
//    constexpr uint128_t& operator/=(const uint128_t& o)
//    {
//        return *this = *this / o;
//    }
//    constexpr uint128_t& operator%=(const uint128_t& o)
//    {
//        return *this = *this % o;
//    }
//
//    // -------------------------------------------------------------------------
//    //  Logical operators
//    // -------------------------------------------------------------------------
//    constexpr bool operator!() const noexcept
//    {
//        return !hi && !lo;
//    }
//    constexpr bool operator&&(const uint128_t& o) const noexcept
//    {
//        return bool(*this) && bool(o);
//    }
//    constexpr bool operator||(const uint128_t& o) const noexcept
//    {
//        return bool(*this) || bool(o);
//    }
//
//    // -------------------------------------------------------------------------
//    //  Utility
//    // -------------------------------------------------------------------------
//
//    /// Returns the number of set bits (popcount)
//    constexpr int popcount() const noexcept
//    {
//#if defined(__GNUC__) || defined(__clang__)
//        if (!VX_IS_CONSTANT_EVALUATED())
//            return __builtin_popcountll(hi) + __builtin_popcountll(lo);
//#elif UINT128_USE_MSVC_INTRINSICS
//        if (!VX_IS_CONSTANT_EVALUATED())
//            return (int)(__popcnt64(hi) + __popcnt64(lo));
//#endif
//        // Portable Hamming-weight
//        auto sw = [](uint64_t x) constexpr -> int
//        {
//            x -= (x >> 1) & 0x5555555555555555ull;
//            x = (x & 0x3333333333333333ull) + ((x >> 2) & 0x3333333333333333ull);
//            x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0Full;
//            return (int)((x * 0x0101010101010101ull) >> 56);
//        };
//        return sw(hi) + sw(lo);
//    }
//
//    /// Count leading zeros
//    constexpr int clz() const noexcept
//    {
//        if (hi)
//        {
//#if defined(__GNUC__) || defined(__clang__)
//            if (!VX_IS_CONSTANT_EVALUATED())
//                return __builtin_clzll(hi);
//#elif UINT128_USE_MSVC_INTRINSICS
//            if (!VX_IS_CONSTANT_EVALUATED())
//            {
//                unsigned long idx;
//                _BitScanReverse64(&idx, hi);
//                return 63 - (int)idx;
//            }
//#endif
//            int n = 0;
//            uint64_t x = hi;
//            if (!(x >> 32))
//            {
//                n += 32;
//                x <<= 32;
//            }
//            if (!(x >> 48))
//            {
//                n += 16;
//                x <<= 16;
//            }
//            if (!(x >> 56))
//            {
//                n += 8;
//                x <<= 8;
//            }
//            if (!(x >> 60))
//            {
//                n += 4;
//                x <<= 4;
//            }
//            if (!(x >> 62))
//            {
//                n += 2;
//                x <<= 2;
//            }
//            if (!(x >> 63))
//                n++;
//            return n;
//        }
//        if (!lo)
//            return 128;
//#if defined(__GNUC__) || defined(__clang__)
//        if (!VX_IS_CONSTANT_EVALUATED())
//            return 64 + __builtin_clzll(lo);
//#elif UINT128_USE_MSVC_INTRINSICS
//        if (!VX_IS_CONSTANT_EVALUATED())
//        {
//            unsigned long idx;
//            _BitScanReverse64(&idx, lo);
//            return 127 - (int)idx;
//        }
//#endif
//        int n = 64;
//        uint64_t x = lo;
//        if (!(x >> 32))
//        {
//            n += 32;
//            x <<= 32;
//        }
//        if (!(x >> 48))
//        {
//            n += 16;
//            x <<= 16;
//        }
//        if (!(x >> 56))
//        {
//            n += 8;
//            x <<= 8;
//        }
//        if (!(x >> 60))
//        {
//            n += 4;
//            x <<= 4;
//        }
//        if (!(x >> 62))
//        {
//            n += 2;
//            x <<= 2;
//        }
//        if (!(x >> 63))
//            n++;
//        return n;
//    }
//
//    /// Count trailing zeros
//    constexpr int ctz() const noexcept
//    {
//        if (lo)
//        {
//#if defined(__GNUC__) || defined(__clang__)
//            if (!VX_IS_CONSTANT_EVALUATED())
//                return __builtin_ctzll(lo);
//#elif UINT128_USE_MSVC_INTRINSICS
//            if (!VX_IS_CONSTANT_EVALUATED())
//            {
//                unsigned long idx;
//                _BitScanForward64(&idx, lo);
//                return (int)idx;
//            }
//#endif
//            int n = 0;
//            uint64_t x = lo;
//            if (!(x & 0xFFFFFFFFull))
//            {
//                n += 32;
//                x >>= 32;
//            }
//            if (!(x & 0xFFFFull))
//            {
//                n += 16;
//                x >>= 16;
//            }
//            if (!(x & 0xFFull))
//            {
//                n += 8;
//                x >>= 8;
//            }
//            if (!(x & 0xFull))
//            {
//                n += 4;
//                x >>= 4;
//            }
//            if (!(x & 0x3ull))
//            {
//                n += 2;
//                x >>= 2;
//            }
//            if (!(x & 0x1ull))
//                n++;
//            return n;
//        }
//        if (!hi)
//            return 128;
//#if defined(__GNUC__) || defined(__clang__)
//        if (!VX_IS_CONSTANT_EVALUATED())
//            return 64 + __builtin_ctzll(hi);
//#elif UINT128_USE_MSVC_INTRINSICS
//        if (!VX_IS_CONSTANT_EVALUATED())
//        {
//            unsigned long idx;
//            _BitScanForward64(&idx, hi);
//            return 64 + (int)idx;
//        }
//#endif
//        int n = 64;
//        uint64_t x = hi;
//        if (!(x & 0xFFFFFFFFull))
//        {
//            n += 32;
//            x >>= 32;
//        }
//        if (!(x & 0xFFFFull))
//        {
//            n += 16;
//            x >>= 16;
//        }
//        if (!(x & 0xFFull))
//        {
//            n += 8;
//            x >>= 8;
//        }
//        if (!(x & 0xFull))
//        {
//            n += 4;
//            x >>= 4;
//        }
//        if (!(x & 0x3ull))
//        {
//            n += 2;
//            x >>= 2;
//        }
//        if (!(x & 0x1ull))
//            n++;
//        return n;
//    }
//};
//
//// -----------------------------------------------------------------------------
////  Free-function helpers  (mirror those for the native case)
//// -----------------------------------------------------------------------------
//
//constexpr uint128_t make_uint128(uint64_t hi, uint64_t lo) noexcept
//{
//    return { lo, hi };
//}
//constexpr uint64_t uint128_hi(const uint128_t& v) noexcept
//{
//    return v.hi;
//}
//constexpr uint64_t uint128_lo(const uint128_t& v) noexcept
//{
//    return v.lo;
//}
//
//// Mixed arithmetic: allow  uint128_t OP integer  and  integer OP uint128_t
//// without polluting the global namespace with hundreds of overloads – we rely
//// on the converting constructor for implicit widening.
//// The only case we add explicitly is `integer OP uint128_t` where the LHS
//// can't see the member operator.
//template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
//constexpr uint128_t operator+(T lhs, const uint128_t& rhs) noexcept
//{
//    return uint128_t(lhs) + rhs;
//}
//template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
//constexpr uint128_t operator-(T lhs, const uint128_t& rhs) noexcept
//{
//    return uint128_t(lhs) - rhs;
//}
//template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
//constexpr uint128_t operator*(T lhs, const uint128_t& rhs) noexcept
//{
//    return uint128_t(lhs) * rhs;
//}
//template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
//constexpr uint128_t operator/(T lhs, const uint128_t& rhs)
//{
//    return uint128_t(lhs) / rhs;
//}
//template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
//constexpr uint128_t operator%(T lhs, const uint128_t& rhs)
//{
//    return uint128_t(lhs) % rhs;
//}
//template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
//constexpr uint128_t operator&(T lhs, const uint128_t& rhs) noexcept
//{
//    return uint128_t(lhs) & rhs;
//}
//template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
//constexpr uint128_t operator|(T lhs, const uint128_t& rhs) noexcept
//{
//    return uint128_t(lhs) | rhs;
//}
//template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
//constexpr uint128_t operator^(T lhs, const uint128_t& rhs) noexcept
//{
//    return uint128_t(lhs) ^ rhs;
//}
//
//} // namespace vx
//
//// -----------------------------------------------------------------------------
////  std::numeric_limits  specialisation
//// -----------------------------------------------------------------------------
//
//#if !__VX_UINT128_USE_NATIVE // native __uint128_t already has its own limits
//
//namespace std {
//template <>
//class numeric_limits<uint128_t>
//{
//public:
//
//    static constexpr bool is_specialized = true;
//    static constexpr bool is_signed = false;
//    static constexpr bool is_integer = true;
//    static constexpr bool is_exact = true;
//    static constexpr bool has_infinity = false;
//    static constexpr bool has_quiet_NaN = false;
//    static constexpr bool has_signaling_NaN = false;
//    static constexpr bool is_bounded = true;
//    static constexpr bool is_modulo = true;
//    static constexpr int digits = 128;
//    static constexpr int digits10 = 38; // floor(128*log10(2))
//    static constexpr int radix = 2;
//
//    static constexpr uint128_t min() noexcept
//    {
//        return {};
//    }
//    static constexpr uint128_t max() noexcept
//    {
//        return ~uint128_t{};
//    }
//    static constexpr uint128_t lowest() noexcept
//    {
//        return {};
//    }
//};
//} // namespace std
//
//#endif // !__VX_UINT128_USE_NATIVE
//
//// -----------------------------------------------------------------------------
////  Native-type thin wrapper  (just an alias when __uint128_t is present)
//// -----------------------------------------------------------------------------
//
//#if __VX_UINT128_USE_NATIVE
//
//using uint128_t = unsigned __int128;
//
//// Convenience helpers so user code looks the same either way
//constexpr uint128_t make_uint128(uint64_t hi, uint64_t lo) noexcept
//{
//    return (static_cast<uint128_t>(hi) << 64) | lo;
//}
//constexpr uint64_t uint128_hi(uint128_t v) noexcept
//{
//    return static_cast<uint64_t>(v >> 64);
//}
//constexpr uint64_t uint128_lo(uint128_t v) noexcept
//{
//    return static_cast<uint64_t>(v);
//}
//
//#else // -- soft implementation ------------------------------------------------
//} // namespace vx
