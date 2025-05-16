#pragma once

#include <cmath>

#include "vertex/config/language_config.hpp"
#include "vertex/math/constants.hpp"
#include "vertex/math/types.hpp"

namespace vx {
namespace math {

//////////////////////////////////////////////////////////////////////////////
// pow
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T pow(T x, T y) noexcept
{
    return std::pow(x, y);
}

// vec scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> pow(const vec<2, T>& x, T y) noexcept
{
    return vec<2, T>(pow(x.x, y), pow(x.y, y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> pow(const vec<3, T>& x, T y) noexcept
{
    return vec<3, T>(pow(x.x, y), pow(x.y, y), pow(x.z, y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> pow(const vec<4, T>& x, T y) noexcept
{
    return vec<4, T>(pow(x.x, y), pow(x.y, y), pow(x.z, y), pow(x.w, y));
}

// vec binary

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> pow(
    const vec<2, T>& x,
    const vec<2, T>& y
) noexcept
{
    return vec<2, T>(pow(x.x, y.x), pow(x.y, y.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> pow(
    const vec<3, T>& x,
    const vec<3, T>& y
) noexcept
{
    return vec<3, T>(pow(x.x, y.x), pow(x.y, y.y), pow(x.z, y.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> pow(
    const vec<4, T>& x,
    const vec<4, T>& y
) noexcept
{
    return vec<4, T>(pow(x.x, y.x), pow(x.y, y.y), pow(x.z, y.z), pow(x.w, y.w));
}

//////////////////////////////////////////////////////////////////////////////
// exp
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T exp(T x) noexcept
{
    return std::exp(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> exp(const vec<2, T>& x) noexcept
{
    return vec<2, T>(exp(x.x), exp(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> exp(const vec<3, T>& x) noexcept
{
    return vec<3, T>(exp(x.x), exp(x.y), exp(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> exp(const vec<4, T>& x) noexcept
{
    return vec<4, T>(exp(x.x), exp(x.y), exp(x.z), exp(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// exp2
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T exp2(T x) noexcept
{
    return std::exp2(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> exp2(const vec<2, T>& x) noexcept
{
    return vec<2, T>(exp2(x.x), exp2(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> exp2(const vec<3, T>& x) noexcept
{
    return vec<3, T>(exp2(x.x), exp2(x.y), exp2(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> exp2(const vec<4, T>& x) noexcept
{
    return vec<4, T>(exp2(x.x), exp2(x.y), exp2(x.z), exp2(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// log
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T log(T x) noexcept
{
    return std::log(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> log(const vec<2, T>& x) noexcept
{
    return vec<2, T>(log(x.x), log(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> log(const vec<3, T>& x) noexcept
{
    return vec<3, T>(log(x.x), log(x.y), log(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> log(const vec<4, T>& x) noexcept
{
    return vec<4, T>(log(x.x), log(x.y), log(x.z), log(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// log2
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T log2(T x) noexcept
{
    return std::log2(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> log2(const vec<2, T>& x) noexcept
{
    return vec<2, T>(log2(x.x), log2(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> log2(const vec<3, T>& x) noexcept
{
    return vec<3, T>(log2(x.x), log2(x.y), log2(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> log2(const vec<4, T>& x) noexcept
{
    return vec<4, T>(log2(x.x), log2(x.y), log2(x.z), log2(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// sqrt
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T sqrt(T x) noexcept
{
    return std::sqrt(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> sqrt(const vec<2, T>& x) noexcept
{
    return vec<2, T>(sqrt(x.x), sqrt(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> sqrt(const vec<3, T>& x) noexcept
{
    return vec<3, T>(sqrt(x.x), sqrt(x.y), sqrt(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> sqrt(const vec<4, T>& x) noexcept
{
    return vec<4, T>(sqrt(x.x), sqrt(x.y), sqrt(x.z), sqrt(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// inverse
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T inverse(T x) noexcept
{
    return static_cast<T>(1) / x;
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> inverse(const vec<2, T>& x) noexcept
{
    return vec<2, T>(inverse(x.x), inverse(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> inverse(const vec<3, T>& x) noexcept
{
    return vec<3, T>(inverse(x.x), inverse(x.y), inverse(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> inverse(const vec<4, T>& x) noexcept
{
    return vec<4, T>(inverse(x.x), inverse(x.y), inverse(x.z), inverse(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// inverse_sqrt
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T inverse_sqrt(T x) noexcept
{
    return static_cast<T>(1) / sqrt(x);
}

// vec

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> inverse_sqrt(const vec<2, T>& x) noexcept
{
    return vec<2, T>(inverse_sqrt(x.x), inverse_sqrt(x.y));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> inverse_sqrt(const vec<3, T>& x) noexcept
{
    return vec<3, T>(inverse_sqrt(x.x), inverse_sqrt(x.y), inverse_sqrt(x.z));
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> inverse_sqrt(const vec<4, T>& x) noexcept
{
    return vec<4, T>(inverse_sqrt(x.x), inverse_sqrt(x.y), inverse_sqrt(x.z), inverse_sqrt(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// next_pow2
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr T next_pow2(T x) noexcept
{
    if (x <= 0) return 1;

    --x;

    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;

    VX_IF_CONSTEXPR(sizeof(T) == 8)
    {
        x |= x >> 32;
    }

    return ++x;
}

// vec

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr vec<2, T> next_pow2(const vec<2, T>& x) noexcept
{
    return vec<2, T>(next_pow2(x.x), next_pow2(x.y));
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr vec<3, T> next_pow2(const vec<3, T>& x) noexcept
{
    return vec<3, T>(next_pow2(x.x), next_pow2(x.y), next_pow2(x.z));
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr vec<4, T> next_pow2(const vec<4, T>& x) noexcept
{
    return vec<4, T>(next_pow2(x.x), next_pow2(x.y), next_pow2(x.z), next_pow2(x.w));
}

} // namespace math
} // namespace vx