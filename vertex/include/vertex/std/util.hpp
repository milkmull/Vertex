#pragma once

#include "vertex/config/feature_detection.hpp"
#include "vertex/config/type_traits.hpp"

#if VX_HAVE_STD_MOVE || VX_HAVE_STD_FORWARD || \
    VX_HAVE_STD_SWAP || VX_HAVE_STD_EXCHANGE
    #include <utility>
#endif

#if VX_HAVE_STD_LAUNDER
    #include <new>
#else
    #include "vertex/os/compiler.hpp"
#endif

namespace vx {

//=========================================================================
// swap
//=========================================================================

template <typename T>
constexpr void swap(T& a, T& b) noexcept(
#if VX_HAVE_STD_SWAP

    noexcept(std::swap(a, b))

#else

    noexcept(T(std::move(a))) &&
    noexcept(a = std::move(b)) &&
    noexcept(b = std::move(a))

#endif
)
{
#if VX_HAVE_STD_SWAP

    std::swap(a, b);

#else

    T tmp = std::move(a);
    a = std::move(b);
    b = std::move(tmp);

#endif
}

//=========================================================================
// exchange
//=========================================================================

template <typename T, typename U = T>
constexpr VX_NO_DISCARD T exchange(T& obj, U&& new_value) noexcept(
#if VX_HAVE_STD_EXCHANGE

    noexcept(std::exchange(obj, std::forward<U>(new_value)))

#else

    noexcept(T(std::move(obj))) &&
    noexcept(obj = std::forward<U>(new_value))

#endif
)
{
#if VX_HAVE_STD_EXCHANGE

    return std::exchange(obj, std::forward<U>(new_value));

#else

    T old_value = std::move(obj);
    obj = std::forward<U>(new_value);
    return old_value;

#endif
}

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

} // namespace vx
