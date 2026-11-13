#pragma once

#include "vertex/config/feature_detection.hpp"
#include "vertex/config/language_config.hpp"
#include "vertex/std/type_traits.hpp"

#include <memory>
#include <utility>

#if VX_HAVE_STD_LAUNDER
    #include <new>
#else
    #include "vertex/os/compiler.hpp"
#endif

namespace vx {

//=========================================================================
// moving and forwarding
//=========================================================================

// std::move and std::forward are kept as using-declarations rather than
// wrappers: compilers treat them specially (diagnostics, debug-build
// inlining), and a wrapper would add a real call to every debug build.
using std::forward;
using std::move;

//=========================================================================
// swap / exchange
//=========================================================================

// NOTE: like std::swap, this is meant to be called qualified (vx::swap(a, b)).
// It does not look for a user-provided swap via ADL.
template <typename T>
constexpr void swap(T& a, T& b) noexcept(
#if VX_HAVE_STD_SWAP

    noexcept(std::swap(a, b))

#else

    noexcept(T(move(a))) &&
    noexcept(a = move(b)) &&
    noexcept(b = move(a))

#endif
)
{
#if VX_HAVE_STD_SWAP

    std::swap(a, b);

#else

    T tmp = move(a);
    a = move(b);
    b = move(tmp);

#endif
}

template <typename T, typename U = T>
constexpr VX_NO_DISCARD T exchange(T& obj, U&& new_value) noexcept(
#if VX_HAVE_STD_EXCHANGE

    noexcept(std::exchange(obj, std::forward<U>(new_value)))

#else

    noexcept(T(move(obj))) &&
    noexcept(obj = std::forward<U>(new_value))

#endif
)
{
#if VX_HAVE_STD_EXCHANGE

    return std::exchange(obj, std::forward<U>(new_value));

#else

    T old_value = move(obj);
    obj = std::forward<U>(new_value);
    return old_value;

#endif
}

//=========================================================================
// constness
//=========================================================================

#if (VX_CPP_STANDARD >= 17)

using std::as_const;

#else

template <typename T>
constexpr traits::add_const_t<T>& as_const(T& x) noexcept
{
    return x;
}

// Binding a const reference to a temporary would dangle.
template <typename T>
void as_const(const T&&) = delete;

#endif // (VX_CPP_STANDARD >= 17)

//=========================================================================
// enums
//=========================================================================

#if (VX_CPP_STANDARD >= 23)

using std::to_underlying;

#else

template <typename E, VX_REQUIRES(traits::is_enum<E>::value)>
constexpr typename traits::underlying_type<E>::type to_underlying(E e) noexcept
{
    using UT = typename traits::underlying_type<E>::type;
    return static_cast<UT>(e);
}

#endif // (VX_CPP_STANDARD >= 23)

//=========================================================================
// addressof
//=========================================================================

#if VX_HAS_BUILTIN(__builtin_addressof)

template <typename T>
VX_FORCE_INLINE constexpr T* addressof(T& x) noexcept
{
    return __builtin_addressof(x);
}

// Taking the address of a temporary would dangle.
template <typename T>
const T* addressof(const T&&) = delete;

#else

// Needs <memory>; include it only on this path.
using std::addressof;

#endif

//=========================================================================
// launder
//=========================================================================

template <typename T>
VX_FORCE_INLINE constexpr T* launder(T* p) noexcept
{
#if VX_HAVE_STD_LAUNDER

    return std::launder(p);

#elif VX_HAS_BUILTIN(__builtin_launder)

    return __builtin_launder(p);

#else

    // No std::launder or builtin available: falls back to a compiler
    // barrier, which defeats common load-caching miscompilations but
    // does not carry the formal [ptr.launder] guarantee.
    return os::compiler_barrier(p);

#endif
}

template <typename T>
VX_FORCE_INLINE constexpr const T* launder(const T* p) noexcept
{
    return const_cast<const T*>(launder(const_cast<T*>(p)));
}

//=========================================================================
// pairs
//=========================================================================

using std::make_pair;
using std::pair;
using std::piecewise_construct;

} // namespace vx
