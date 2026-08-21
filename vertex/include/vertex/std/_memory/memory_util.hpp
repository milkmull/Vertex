#pragma once

#include "vertex/config/feature_detection.hpp"
#include "vertex/config/type_traits.hpp"

#if VX_HAVE_STD_LAUNDER
    #include <new>
#else
    #include "vertex/os/compiler.hpp"
#endif


namespace vx {
namespace mem {

//=========================================================================
// helpers
//=========================================================================

template <typename T>
constexpr VX_NO_DISCARD typename std::remove_reference<T>::type&& move(T&& a) noexcept
{
    return static_cast<typename std::remove_reference<T>::type&&>(a);
}

template <typename T>
constexpr VX_NO_DISCARD T&& forward(
    typename std::remove_reference<T>::type& a) noexcept
{
    return static_cast<T&&>(a);
}

template <typename T>
constexpr VX_NO_DISCARD T&& forward(
    typename std::remove_reference<T>::type&& a) noexcept
{
    static_assert(!std::is_lvalue_reference<T>::value,
        "cannot forward an rvalue as an lvalue");
    return static_cast<T&&>(a);
}

template <typename T>
constexpr void swap(T& a, T& b) noexcept(
    noexcept(T(move(a))) &&
    noexcept(a = move(b)) &&
    noexcept(b = move(a)))
{
    T tmp = move(a);
    a = move(b);
    b = move(tmp);
}

template <typename T, typename U = T>
constexpr VX_NO_DISCARD T exchange(T& obj, U&& new_value) noexcept(
    noexcept(T(move(obj))) &&
    noexcept(obj = forward<U>(new_value)))
{
    T old_value = move(obj);
    obj = forward<U>(new_value);
    return old_value;
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

} // namespace mem
} // namespace vx
