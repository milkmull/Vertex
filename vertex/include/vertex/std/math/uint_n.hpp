#pragma once

#include "vertex/std/array.hpp"
#include "vertex/util/bit/bit.hpp"

namespace vx {

// ============================================================================
//  Platform detection
// ============================================================================

#if defined(__SIZEOF_INT128__)
    #define __HAS_INT128 1
#endif

#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_AMD64))
    #include <intrin.h>
    #define _HAS_UMUL128  1 // _umul128
    #define _HAS_ADDCARRY 1 // _addcarry_u64 / _subborrow_u64
#endif

// ============================================================================
//  best_limb<N> - choose the widest native limb type that evenly
//  divides N bits, preferring wider limbs for better throughput.
//
//  Priority (highest first):
//    uint64_t  - when __int128 or MSVC _umul128 is available
//    uint32_t  - always available; carry fits in uint64_t
//    uint16_t  - fallback for tiny N (e.g. N=16)
//    uint8_t   - last resort
//
//  If N is not divisible by 64 we step down accordingly.
// ============================================================================

namespace _big_num_priv {

template <size_t N>
struct best_limb_impl
{
    using type =
#if defined(_HAS_INT128) || defined(_HAS_UMUL128)

        typename std::conditional<
            (N % 64 == 0),
            uint64_t,
            typename std::conditional<
                (N % 32 == 0),
                uint32_t,
                typename std::conditional<
                    (N % 16 == 0),
                    uint16_t,
                    uint8_t>::type>::type>::type;

#else

        // Without a widening 64-bit multiply, use 32-bit limbs; the
        // carry always fits in a plain uint64_t.
        typename std::conditional<
            (N % 32 == 0),
            uint32_t,
            typename std::conditional<
                (N % 16 == 0),
                uint16_t,
                uint8_t>::type>::type;

#endif
};

} // namespace _big_num_priv

// Public alias - query the default limb for any N.
template <size_t N>
using best_limb_t = typename _big_num_priv::best_limb_impl<N>::type;

// ============================================================================
//  limb_traits<Limb> - per-limb primitive operations
//
//  Every specialisation must provide:
//
//    static limb_type add_carry (a, b, cin, out&) -> carry-out  (0 or 1)
//    static limb_type sub_borrow(a, b, bin, out&) -> borrow-out (0 or 1)
//    static void      mul_wide  (a, b, lo&, hi&)  - full product
//    static int       clz       (a)               - count leading zeros
//    static constexpr int      bits
//    static constexpr limb_type zero, one, max_val
//
//  Users may define additional specialisations for custom integer types
//  (e.g. __m256i lanes, SIMD limbs, Montgomery form) and pass them as
//  the Traits parameter of uint_n without touching this header.
// ============================================================================

template <typename Limb>
struct limb_traits; // intentionally undefined - missing spec -> clear error

namespace _big_num_priv {

template <typename T, typename Wide>
constexpr T generic_adc(T a, T b, T cin, T& out) noexcept
{
    const Wide r = static_cast<Wide>(a) + static_cast<Wide>(b) + static_cast<Wide>(cin);
    out = static_cast<T>(r);
    return static_cast<T>(r >> (8 * sizeof(T)));
}

template <typename T, typename Wide>
constexpr T generic_sbb(T a, T b, T bin, T& out) noexcept
{
    const Wide r = static_cast<Wide>(a) - static_cast<Wide>(b) - static_cast<Wide>(bin);
    out = static_cast<T>(r);
    return static_cast<T>((r >> (8 * sizeof(T))) & 1);
}

template <typename T, typename Wide>
constexpr void generic_mul_wide(T a, T b, T& lo, T& hi) noexcept
{
    const Wide r = static_cast<Wide>(a) * static_cast<Wide>(b);
    lo = static_cast<T>(r);
    hi = static_cast<T>(r >> (8 * sizeof(T)));
}

} // namespace _big_num_priv

//==============================================================================
// uint8_t
//==============================================================================

template <>
struct limb_traits<uint8_t>
{
    using limb_type = uint8_t;
    static constexpr int bits = 8;
    static constexpr limb_type zero = 0;
    static constexpr limb_type one = 1;
    static constexpr limb_type max_val = 0xFFu;

    static constexpr limb_type add_carry(limb_type a, limb_type b, limb_type cin, limb_type& out) noexcept
    {
        return _big_num_priv::generic_adc<uint8_t, uint16_t>(a, b, cin, out);
    }

    static constexpr limb_type sub_borrow(limb_type a, limb_type b, limb_type bin, limb_type& out) noexcept
    {
        return _big_num_priv::generic_sbb<uint8_t, uint16_t>(a, b, bin, out);
    }

    static constexpr void mul_wide(limb_type a, limb_type b, limb_type& lo, limb_type& hi) noexcept
    {
        _big_num_priv::generic_mul_wide<uint8_t, uint16_t>(a, b, lo, hi);
    }

    static constexpr int clz(limb_type a) noexcept
    {
        return a ? (bit::countl_zero(static_cast<uint32_t>(a)) - 24) : 8;
    }
};

//==============================================================================
// uint16_t
//==============================================================================

template <>
struct limb_traits<uint16_t>
{
    using limb_type = uint16_t;
    static constexpr int bits = 16;
    static constexpr limb_type zero = 0;
    static constexpr limb_type one = 1;
    static constexpr limb_type max_val = 0xFFFFu;

    static constexpr limb_type add_carry(limb_type a, limb_type b, limb_type cin, limb_type& out) noexcept
    {
        return _big_num_priv::generic_adc<uint16_t, uint32_t>(a, b, cin, out);
    }

    static constexpr limb_type sub_borrow(limb_type a, limb_type b, limb_type bin, limb_type& out) noexcept
    {
        return _big_num_priv::generic_sbb<uint16_t, uint32_t>(a, b, bin, out);
    }

    static constexpr void mul_wide(limb_type a, limb_type b, limb_type& lo, limb_type& hi) noexcept
    {
        _big_num_priv::generic_mul_wide<uint16_t, uint32_t>(a, b, lo, hi);
    }

    static constexpr int clz(limb_type a) noexcept
    {
        return a ? (bit::countl_zero(static_cast<uint32_t>(a)) - 16) : 16;
    }
};

//==============================================================================
// uint32_t
//==============================================================================

template <>
struct limb_traits<uint32_t>
{
    using limb_type = uint32_t;
    static constexpr int bits = 32;
    static constexpr limb_type zero = 0;
    static constexpr limb_type one = 1;
    static constexpr limb_type max_val = 0xFFFF'FFFFu;

    static constexpr limb_type add_carry(limb_type a, limb_type b, limb_type cin, limb_type& out) noexcept
    {
        return _big_num_priv::generic_adc<uint32_t, uint64_t>(a, b, cin, out);
    }

    static constexpr limb_type sub_borrow(limb_type a, limb_type b, limb_type bin, limb_type& out) noexcept
    {
        return _big_num_priv::generic_sbb<uint32_t, uint64_t>(a, b, bin, out);
    }

    static constexpr void mul_wide(limb_type a, limb_type b, limb_type& lo, limb_type& hi) noexcept
    {
        _big_num_priv::generic_mul_wide<uint32_t, uint64_t>(a, b, lo, hi);
    }

    static constexpr int clz(limb_type a) noexcept
    {
        return a ? bit::countl_zero(a) : 32;
    }
};

//==============================================================================
// uint64_t
//==============================================================================

template <>
struct limb_traits<uint64_t>
{
    using limb_type = uint64_t;
    static constexpr int bits = 64;
    static constexpr limb_type zero = 0;
    static constexpr limb_type one = 1;
    static constexpr limb_type max_val = 0xFFFF'FFFF'FFFF'FFFFull;

    static constexpr int clz(limb_type a) noexcept
    {
        return a ? bit::countl_zero(a) : 64;
    }

private:

    //==============================================================================
    // portable 32-bit-half decomposition
    //==============================================================================

    static constexpr limb_type add_carry_generic(limb_type a, limb_type b, limb_type cin, limb_type& out) noexcept
    {
        constexpr uint64_t mask = 0xFFFFFFFFull;
        const uint64_t lo = (a & mask) + (b & mask) + cin;
        const uint64_t hi = (a >> 32) + (b >> 32) + (lo >> 32);
        out = (hi << 32) | (lo & mask);
        return static_cast<limb_type>(hi >> 32);
    }

    static constexpr limb_type sub_borrow_genric(limb_type a, limb_type b, limb_type bin, limb_type& out) noexcept
    {
        constexpr int64_t mask = 0xFFFF'FFFFll;
        const int64_t lo = (int64_t)(a & (uint64_t)mask) - (int64_t)(b & (uint64_t)mask) - (int64_t)bin;
        const int64_t hi = (int64_t)(a >> 32) - (int64_t)(b >> 32) - (lo < 0 ? 1 : 0);
        out = (static_cast<uint64_t>(hi & mask) << 32) | static_cast<uint64_t>(lo & mask);
        return static_cast<limb_type>(hi < 0 ? 1 : 0);
    }

    static constexpr void mul_wide_generic(limb_type a, limb_type b, limb_type& lo, limb_type& hi) noexcept
    {
        const uint64_t al = a & 0xFFFFFFFFull, ah = a >> 32;
        const uint64_t bl = b & 0xFFFFFFFFull, bh = b >> 32;
        const uint64_t p0 = al * bl, p1 = al * bh, p2 = ah * bl, p3 = ah * bh;
        const uint64_t mid = (p0 >> 32) + (p1 & 0xFFFFFFFFull) + (p2 & 0xFFFFFFFFull);
        lo = (mid << 32) | (p0 & 0xFFFFFFFFull);
        hi = p3 + (p1 >> 32) + (p2 >> 32) + (mid >> 32);
    }

public:

#if defined(_HAS_INT128)

    //==============================================================================
    // __int128
    //==============================================================================

    static constexpr limb_type add_carry(limb_type a, limb_type b, limb_type cin, limb_type& out) noexcept
    {
        using u128 = unsigned __int128;
        const u128 r = static_cast<u128>(a) + b + cin;
        out = static_cast<limb_type>(r);
        return static_cast<limb_type>(r >> 64);
    }

    static constexpr limb_type sub_borrow(limb_type a, limb_type b, limb_type bin, limb_type& out) noexcept
    {
        using u128 = unsigned __int128;
        const u128 r = static_cast<u128>(a) - b - bin;
        out = static_cast<limb_type>(r);
        return static_cast<limb_type>((r >> 64) & 1);
    }

    static constexpr void mul_wide(limb_type a, limb_type b, limb_type& lo, limb_type& hi) noexcept
    {
        using u128 = unsigned __int128;
        const u128 r = static_cast<u128>(a) * b;
        lo = static_cast<limb_type>(r);
        hi = static_cast<limb_type>(r >> 64);
    }

#elif defined(_HAS_UMUL128)

    //==============================================================================
    // MSVC x64 intrinsics (not constexpr)
    //==============================================================================

    static constexpr limb_type add_carry(limb_type a, limb_type b, limb_type cin, limb_type& out) noexcept
    {
        if (VX_IS_CONSTANT_EVALUATED())
        {
            return add_carry_generic(a, b, cin, out);
        }

        const unsigned char c = _addcarry_u64(
            static_cast<unsigned char>(cin), a, b,
            reinterpret_cast<unsigned long long*>(&out));
        return static_cast<limb_type>(c);
    }

    static constexpr limb_type sub_borrow(limb_type a, limb_type b, limb_type bin, limb_type& out) noexcept
    {
        if (VX_IS_CONSTANT_EVALUATED())
        {
            return sub_borrow_genric(a, b, bin, out);
        }

        const unsigned char br = _subborrow_u64(
            static_cast<unsigned char>(bin), a, b,
            reinterpret_cast<unsigned long long*>(&out));

        return static_cast<limb_type>(br);
    }

    static constexpr void mul_wide(limb_type a, limb_type b, limb_type& lo, limb_type& hi) noexcept
    {
        if (VX_IS_CONSTANT_EVALUATED())
        {
            mul_wide_generic(a, b, lo, hi);
            return;
        }

        unsigned long long hi_tmp{};
        lo = _umul128(a, b, &hi_tmp);
        hi = hi_tmp;
    }

#else

    //==============================================================================
    // portable 32-bit-half decomposition
    //==============================================================================

    static constexpr limb_type add_carry(limb_type a, limb_type b, limb_type cin, limb_type& out) noexcept
    {
        return add_carry_generic(a, b, cin, out);
    }

    static constexpr limb_type sub_borrow(limb_type a, limb_type b, limb_type bin, limb_type& out) noexcept
    {
        return sub_borrow_genric(a, b, bin, out);
    }

    static constexpr void mul_wide(limb_type a, limb_type b, limb_type& lo, limb_type& hi) noexcept
    {
        mul_wide_generic(a, b, lo, hi);
    }

#endif // limb_traits<uint64_t>
};

// ============================================================================
//  native_uint<N> - map bit-widths to native types where they exist.
//
//  Used to provide zero-cost conversion to/from native integers when N
//  exactly matches a hardware type.  ::type is void if no native type exists.
// ============================================================================

namespace _big_num_priv {

template <size_t N>
struct native_uint
{
    using type = void;
};

template <>
struct native_uint<8>
{
    using type = uint8_t;
};

template <>
struct native_uint<16>
{
    using type = uint16_t;
};

template <>
struct native_uint<32>
{
    using type = uint32_t;
};

template <>
struct native_uint<64>
{
    using type = uint64_t;
};

#if defined(_HAS_INT128)

template <>
struct native_uint<128>
{
    using type = unsigned __int128;
};

#endif

template <size_t N>
using native_uint_t = typename native_uint<N>::type;

template <size_t N>
struct has_native : std::bool_constant<!std::is_void<native_uint_t<N>>::value>
{
};

} // namespace _big_num_priv


//==============================================================================
// generic bignum
//==============================================================================

// ============================================================================
//  uint_n<N, Limb, Traits>
//
//  Template parameters
//  -------------------
//  N      Bit width.  Required.  Must satisfy N % Traits::bits == 0.
//  Limb   Storage unit.  Default: best_limb_t<N> (platform-selected above).
//  Traits Operations on Limb.  Default: limb_traits<Limb>.
//         Replace this to inject platform-specific SIMD, Montgomery form,
//         or any other representation that preserves the add/sub/mul interface.
//
//  Limb ordering
//  -------------
//  limbs[0] holds the least-significant bits ("little-limb" order).
//  This matches the natural ordering of x86 ADC/SBB chains and makes
//  truncation (take limbs[0..k]) and zero-extension (pad with zeros) trivial.
// ============================================================================

template <
    size_t N,
    typename Limb = best_limb_t<N>,
    typename Traits = limb_traits<Limb>>
struct uint_n
{
    //=====================================
    // checks
    //=====================================

    VX_STATIC_ASSERT_MSG(N > 0, "uint_n: bit width N must be positive");
    VX_STATIC_ASSERT_MSG(std::is_unsigned<Limb>::value, "uint_n: Limb must be an unsigned integer type");
    VX_STATIC_ASSERT_MSG(N % Traits::bits == 0, "uint_n: N must be an exact multiple of the limb bit-width");

    //=====================================
    // stats
    //=====================================

    using limb_type = Limb;
    using traits_type = Traits;
    static constexpr size_t bits = N;
    static constexpr size_t limb_bits = Traits::bits;
    static constexpr size_t limb_count = N / limb_bits;

    //=====================================
    // storage
    //=====================================

    array<Limb, limb_count> limbs = {};

    //=====================================
    // constructors
    //=====================================

    constexpr uint_n() noexcept = default;

    template <typename T, VX_REQUIRES(type_traits::is_unsigned_integral<T>::value)>
    explicit constexpr uint_n(T v) noexcept
    {
        for (size_t i = 0; i < limb_count; ++i)
        {
            VX_IF_CONSTEXPR (sizeof(T) <= sizeof(Limb))
            {
                limbs[i] = (i == 0) ? static_cast<Limb>(v) : Limb{};
            }
            else
            {
                // T wider than Limb: extract limb-sized chunks.
                const size_t shift = i * limb_bits;
                limbs[i] = (shift < 8 * sizeof(T))
                    ? static_cast<Limb>(v >> shift)
                    : Limb{};
            }
        }
    }

    template <typename T, VX_REQUIRES(type_traits::is_signed_integral<T>::value)>
    explicit constexpr uint_n(T v) noexcept
    {
        using U = typename std::make_unsigned<T>::type;
        VX_ASSERT(v >= 0);
        *this = uint_n(static_cast<U>(v)); // calls the unsigned overload
    }

#if defined(_HAS_INT128)

    // From unsigned __int128 when available.
    explicit constexpr uint_n(unsigned __int128 v) noexcept
    {
        for (size_t i = 0; i < limb_count; ++i)
        {
            limbs[i] = static_cast<Limb>(v >> (i * limb_bits));
        }
    }

#endif

    // From another uint_n (any N, Limb, Traits) - zero-extends or truncates.
    // Conversion goes through a byte array so limb-size mismatches are handled
    // portably without aliasing violations.
    template <size_t M, typename L2, typename T2>
    explicit constexpr uint_n(const uint_n<M, L2, T2>& other) noexcept
    {
        constexpr size_t dst_bytes = N / 8;
        constexpr size_t src_bytes = M / 8;
        constexpr size_t copy_bytes = std::min(dst_bytes, src_bytes);

        // Flatten source to bytes.
        uint8_t src_b[src_bytes] = {};

        for (size_t i = 0; i < other.limb_count; ++i)
        {
            for (size_t b = 0; b < sizeof(L2); ++b)
            {
                src_b[i * sizeof(L2) + b] = static_cast<uint8_t>(other.limbs[i] >> (8 * b));
            }
        }

        // Build destination limbs from bytes.
        for (size_t i = 0; i < limb_count; ++i)
        {
            Limb v{};

            for (size_t b = 0; b < sizeof(Limb); ++b)
            {
                const size_t byte_idx = i * sizeof(Limb) + b;

                if (byte_idx < copy_bytes)
                {
                    v |= static_cast<Limb>(src_b[byte_idx]) << (8 * b);
                }
            }

            limbs[i] = v;
        }
    }

    //=====================================
    // named constructors
    //=====================================

    static constexpr uint_n zero() noexcept
    {
        return uint_n{};
    }

    static constexpr uint_n one() noexcept
    {
        uint_n r;
        r.limbs[0] = Traits::one;
        return r;
    }

    static constexpr uint_n min() noexcept
    {
        return zero();
    }

    static constexpr uint_n max() noexcept
    {
        uint_n r;
        r.limbs.fill(Traits::max_val);
        return r;
    }

    //=====================================
    // bit access
    //=====================================

    constexpr bool bit(size_t i) const noexcept
    {
        return (limbs[i / limb_bits] >> (i % limb_bits)) & Limb{ 1 };
    }

    constexpr void set_bit(size_t i, bool v) noexcept
    {
        auto& l = limbs[i / limb_bits];
        const Limb mk = Limb{ 1 } << (i % limb_bits);
        l = v ? (l | mk) : (l & ~mk);
    }

    template <typename T = Limb, VX_REQUIRES(std::is_unsigned<T>::value)>
    constexpr T extract_bits_at(size_t offset) const noexcept
    {
        constexpr size_t count = sizeof(T) * 8;

        const size_t i = offset / limb_bits;
        const size_t lo = offset % limb_bits;

        // fast path: aligned and T fits exactly in one limb
        VX_IF_CONSTEXPR (count == limb_bits)
        {
            if (lo == 0)
            {
                return static_cast<T>(limbs[i]);
            }

            // straddles two limbs
            const T low_part = static_cast<T>(limbs[i] >> lo);
            const T high_part = static_cast<T>(limbs[i + 1] << (limb_bits - lo));
            return low_part | high_part;
        }
        else
        {
            // fast path: T smaller than a limb, bits fit within one limb
            VX_IF_CONSTEXPR (count < limb_bits)
            {
                if (lo + count <= limb_bits)
                {
                    constexpr Limb mask = (Limb{ 1 } << count) - Limb{ 1 };
                    return static_cast<T>((limbs[i] >> lo) & mask);
                }
            }

            // general case: T wider than a limb or crosses boundary
            T result{};
            size_t bits_written = 0;
            size_t cur_i = i;
            size_t cur_lo = lo;

            while (bits_written < count)
            {
                const size_t slice = std::min(limb_bits - cur_lo, count - bits_written);

                const Limb mask = (slice == limb_bits)
                    ? Traits::max_val
                    : static_cast<Limb>((Limb{ 1 } << slice) - Limb{ 1 });

                result |= static_cast<T>((limbs[cur_i] >> cur_lo) & mask) << bits_written;

                bits_written += slice;
                cur_lo = 0;
                ++cur_i;
            }

            return result;
        }
    }

    constexpr uint_n& set_low_bits(size_t count) noexcept
    {
        const size_t full_limbs = count / limb_bits;
        const size_t remainder = count % limb_bits;

        for (size_t i = 0; i < full_limbs; ++i)
        {
            limbs[i] = Traits::max_val;
        }

        limbs[full_limbs] = static_cast<Limb>((Limb{ 1 } << remainder) - Limb{ 1 });

        for (size_t i = full_limbs + 1; i < limb_count; ++i)
        {
            limbs[i] = Limb{};
        }

        return *this;
    }

    //=====================================
    // checks
    //=====================================

    constexpr bool is_zero() const noexcept
    {
        for (auto& l : limbs)
        {
            if (l)
            {
                return false;
            }
        }

        return true;
    }

    // Low 64 bits for interop with native code.
    constexpr uint64_t low64() const noexcept
    {
        VX_IF_CONSTEXPR (limb_bits >= 64)
        {
            return static_cast<uint64_t>(limbs[0]);
        }
        else
        {
            uint64_t r{};
            constexpr size_t k = std::min(limb_count, size_t{ 64 / limb_bits });
            for (size_t i = 0; i < k; ++i)
            {
                r |= static_cast<uint64_t>(limbs[i]) << (i * limb_bits);
            }
            return r;
        }
    }

    // Conversion to native type - available only when N matches exactly.
    template <size_t M = N, VX_REQUIRES(_big_num_priv::has_native<M>::value)>
    constexpr auto to_native() const noexcept
    {
        using Native = _big_num_priv::native_uint_t<M>;

        Native r{};
        for (size_t i = 0; i < limb_count; ++i)
        {
            r |= static_cast<Native>(limbs[i]) << (i * limb_bits);
        }
        return r;
    }

    //=====================================
    // addition
    //=====================================

    // Returns carry-out (0 or 1).
    friend constexpr Limb add(const uint_n& a, const uint_n& b, uint_n& out, Limb cin = Limb{}) noexcept
    {
        Limb carry = cin;
        for (size_t i = 0; i < limb_count; ++i)
        {
            carry = Traits::add_carry(a.limbs[i], b.limbs[i], carry, out.limbs[i]);
        }
        return carry;
    }

    friend constexpr uint_n operator+(const uint_n& a, const uint_n& b) noexcept
    {
        uint_n r;
        add(a, b, r);
        return r;
    }

    constexpr uint_n& operator+=(const uint_n& rhs) noexcept
    {
        add(*this, rhs, *this);
        return *this;
    }

    constexpr uint_n& operator++() noexcept
    {
        for (size_t i = 0; i < limb_count; ++i)
        {
            if (++limbs[i])
            {
                break;
            }
        }
        return *this;
    }

    constexpr uint_n operator++(int) noexcept
    {
        auto t = *this;
        ++*this;
        return t;
    }

    //=====================================
    // subtraction
    //=====================================

    // Returns borrow-out (0 or 1).
    friend constexpr Limb sub(const uint_n& a, const uint_n& b, uint_n& out, Limb bin = Limb{}) noexcept
    {
        Limb borrow = bin;
        for (size_t i = 0; i < limb_count; ++i)
        {
            borrow = Traits::sub_borrow(a.limbs[i], b.limbs[i], borrow, out.limbs[i]);
        }
        return borrow;
    }

    friend constexpr uint_n operator-(const uint_n& a, const uint_n& b) noexcept
    {
        uint_n r;
        sub(a, b, r);
        return r;
    }

    constexpr uint_n& operator-=(const uint_n& rhs) noexcept
    {
        sub(*this, rhs, *this);
        return *this;
    }

    constexpr uint_n& operator--() noexcept
    {
        for (size_t i = 0; i < limb_count; ++i)
        {
            if (limbs[i]--)
            {
                break;
            }
        }
        return *this;
    }

    constexpr uint_n operator--(int) noexcept
    {
        auto t = *this;
        --*this;
        return t;
    }

    //=====================================
    // multiplication
    //=====================================

    //  Schoolbook O(n²) - correct and fast for N up to ~2048 bits.
    //  For larger N, provide a Karatsuba or NTT-based Traits::mul_n override.
    //
    //  Carry accounting:
    //    Each inner step computes  r[i+j] += a[i]*b[j]_lo + carry_in
    //    and forwards  a[i]*b[j]_hi + carry_out  into the next column.
    //    The outer carry is discarded - overflow wraps at 2^N.

    friend constexpr uint_n operator*(const uint_n& a, const uint_n& b) noexcept
    {
        uint_n r{};

        for (size_t i = 0; i < limb_count; ++i)
        {
            Limb carry{};

            for (size_t j = 0; j + i < limb_count; ++j)
            {
                // Full product: a[i] * b[j] = hi:lo
                Limb lo{}, hi{};
                Traits::mul_wide(a.limbs[i], b.limbs[j], lo, hi);

                // Step 1: r[i+j] += lo + carry_in  ->  produces carry c1 (0 or 1)
                Limb c1 = Traits::add_carry(r.limbs[i + j], lo, carry, r.limbs[i + j]);

                // Step 2: carry_out = hi + c1
                // hi ≤ max_val and c1 ≤ 1, so their sum fits in two limbs.
                // Since hi + 1 can itself overflow (when hi == max_val), we
                // use add_carry again.  The carry out of *that* add is always
                // 0 or 1, and it becomes a contribution to r[i+j+1] on the
                // next iteration - but that column is handled when i or j
                // advances, so we just propagate it as the new 'carry'.
                Limb c2 = Traits::add_carry(hi, Limb{}, c1, carry);
                // c2 can only be 1 when hi == max_val AND c1 == 1; in that
                // case carry wraps to 0 and c2 == 1 must be forwarded.
                // Because carry is the limb we carry into the *next* column,
                // we add c2 directly - it fits because carry is now 0.
                carry += c2; // carry was 0 from the wrap, c2 is 0 or 1
            }
            // Any remaining carry beyond the last limb is discarded -
            // modular 2^N semantics.
        }
        return r;
    }

    constexpr uint_n& operator*=(const uint_n& rhs) noexcept
    {
        return *this = *this * rhs;
    }

    //=====================================
    // division + modulo
    //=====================================

    //  Binary (bit-at-a-time) long division.  Correct for all inputs.
    //  Runs in O(N²) bit operations - fine for float-to-string and crypto
    //  support routines.  For high-throughput use, implement Knuth Algorithm D
    //  or Barrett reduction in a custom Traits.

    struct divmod_result
    {
        uint_n quotient;
        uint_n remainder;
        bool division_by_zero = false;
    };

    friend constexpr divmod_result divmod(const uint_n& a, const uint_n& b) noexcept
    {
        if (b.is_zero())
        {
            return { uint_n{}, uint_n{}, true };
        }

        uint_n q{}, r{};

        for (int i = static_cast<int>(N) - 1; i >= 0; --i)
        {
            r <<= 1;
            r.set_bit(0, a.bit(static_cast<size_t>(i)));

            if (!(r < b))
            {
                r -= b;
                q.set_bit(static_cast<size_t>(i), true);
            }
        }

        return { q, r };
    }

    friend constexpr uint_n operator/(const uint_n& a, const uint_n& b) noexcept
    {
        return divmod(a, b).quotient;
    }

    friend constexpr uint_n operator%(const uint_n& a, const uint_n& b) noexcept
    {
        return divmod(a, b).remainder;
    }

    constexpr uint_n& operator/=(const uint_n& rhs) noexcept
    {
        *this = divmod(*this, rhs).quotient;
        return *this;
    }

    constexpr uint_n& operator%=(const uint_n& rhs) noexcept
    {
        *this = divmod(*this, rhs).remainder;
        return *this;
    }

    //=====================================
    // bitwise operators
    //=====================================

    friend constexpr uint_n operator&(const uint_n& a, const uint_n& b) noexcept
    {
        uint_n r;
        for (size_t i = 0; i < limb_count; ++i)
        {
            r.limbs[i] = a.limbs[i] & b.limbs[i];
        }
        return r;
    }

    friend constexpr uint_n operator|(const uint_n& a, const uint_n& b) noexcept
    {
        uint_n r;
        for (size_t i = 0; i < limb_count; ++i)
        {
            r.limbs[i] = a.limbs[i] | b.limbs[i];
        }
        return r;
    }

    friend constexpr uint_n operator^(const uint_n& a, const uint_n& b) noexcept
    {
        uint_n r;
        for (size_t i = 0; i < limb_count; ++i)
        {
            r.limbs[i] = a.limbs[i] ^ b.limbs[i];
        }
        return r;
    }

    constexpr uint_n operator~() const noexcept
    {
        uint_n r;
        for (size_t i = 0; i < limb_count; ++i)
        {
            r.limbs[i] = ~limbs[i];
        }
        return r;
    }

    constexpr uint_n& operator&=(const uint_n& rhs) noexcept
    {
        for (size_t i = 0; i < limb_count; ++i)
        {
            limbs[i] &= rhs.limbs[i];
        }
        return *this;
    }

    constexpr uint_n& operator|=(const uint_n& rhs) noexcept
    {
        for (size_t i = 0; i < limb_count; ++i)
        {
            limbs[i] |= rhs.limbs[i];
        }
        return *this;
    }

    constexpr uint_n& operator^=(const uint_n& rhs) noexcept
    {
        for (size_t i = 0; i < limb_count; ++i)
        {
            limbs[i] ^= rhs.limbs[i];
        }
        return *this;
    }

    //=====================================
    // shifts
    //=====================================

    constexpr uint_n& operator<<=(int k) noexcept
    {
        if (k == 0)
        {
            return *this;
        }

        if (k >= N)
        {
            limbs.fill({});
            return *this;
        }

        const size_t ls = k / limb_bits; // whole-limb shift
        const size_t bs = k % limb_bits; // sub-limb bit shift

        for (size_t i = limb_count - 1; i > ls; --i)
        {
            limbs[i] = (limbs[i - ls] << bs) | (bs ? limbs[i - ls - 1] >> (limb_bits - bs) : Limb{});
        }

        limbs[ls] = limbs[0] << bs;

        for (size_t i = 0; i < ls; ++i)
        {
            limbs[i] = Limb{};
        }

        return *this;
    }

    constexpr uint_n& operator>>=(int k) noexcept
    {
        if (k == 0)
        {
            return *this;
        }

        if (k >= N)
        {
            limbs.fill({});
            return *this;
        }

        const size_t ls = k / limb_bits;
        const size_t bs = k % limb_bits;

        for (size_t i = 0; i + ls + 1 < limb_count; ++i)
        {
            limbs[i] = (limbs[i + ls] >> bs) | (bs ? limbs[i + ls + 1] << (limb_bits - bs) : Limb{});
        }

        limbs[limb_count - 1 - ls] = limbs[limb_count - 1] >> bs;
        std::fill(limbs.end() - static_cast<std::ptrdiff_t>(ls), limbs.end(), Limb{});
        return *this;
    }

    friend constexpr uint_n operator<<(uint_n a, int k) noexcept
    {
        return a <<= k;
    }

    friend constexpr uint_n operator>>(uint_n a, int k) noexcept
    {
        return a >>= k;
    }

    //=====================================
    // comparison
    //=====================================

    // Returns -1, 0, or 1 — internal helper, not exposed.
    // All six operators delegate to this so the loop runs only once.
    friend constexpr int compare(const uint_n& a, const uint_n& b) noexcept
    {
        for (size_t i = limb_count - 1; i < limb_count; --i)
        {
            if (a.limbs[i] < b.limbs[i])
            {
                return -1;
            }
            if (a.limbs[i] > b.limbs[i])
            {
                return 1;
            }
        }
        return 0;
    }

    friend constexpr bool operator==(const uint_n& a, const uint_n& b) noexcept
    {
        return compare(a, b) == 0;
    }

    friend constexpr bool operator!=(const uint_n& a, const uint_n& b) noexcept
    {
        return compare(a, b) != 0;
    }

    friend constexpr bool operator<(const uint_n& a, const uint_n& b) noexcept
    {
        return compare(a, b) < 0;
    }

    friend constexpr bool operator>(const uint_n& a, const uint_n& b) noexcept
    {
        return compare(a, b) > 0;
    }

    friend constexpr bool operator<=(const uint_n& a, const uint_n& b) noexcept
    {
        return compare(a, b) <= 0;
    }

    friend constexpr bool operator>=(const uint_n& a, const uint_n& b) noexcept
    {
        return compare(a, b) >= 0;
    }


    //=====================================
    // bit count utils
    //=====================================

    constexpr int clz() const noexcept
    {
        for (size_t i = limb_count - 1; i < limb_count; --i)
        {
            if (limbs[i])
            {
                return static_cast<int>(limb_bits * (limb_count - 1 - i)) + Traits::clz(limbs[i]);
            }
        }
        return static_cast<int>(N);
    }

    constexpr int crz() const noexcept
    {
        for (size_t i = 0; i < limb_count; ++i)
        {
            if (limbs[i])
            {
                return static_cast<int>(limb_bits * i) + bit::countr_zero(limbs[i]);
            }
        }
        return static_cast<int>(N);
    }

    // Bit length: position of the highest set bit + 1; 0 for zero.
    constexpr int bit_length() const noexcept
    {
        return static_cast<int>(N) - clz();
    }
};

// ============================================================================
//  Convenient type aliases
// ============================================================================

using uint128 = uint_n<128>;
using uint256 = uint_n<256>;
using uint512 = uint_n<512>;
using uint1024 = uint_n<1024>;

// Explicit limb-type variants:
//   using uint256_u32 = uint_n<256, uint32_t>;
//
// Custom Traits (e.g. for Montgomery multiplication):
//   struct mont_traits { ... };
//   using uint256_mont = uint_n<256, uint64_t, mont_traits>;

#undef _HAS_UINT128
#undef _HAS_ADDCARRY

} // namespace vx
