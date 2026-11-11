#pragma once

#include "vertex/config/language_config.hpp"

#if VX_HAVE_STD_INVOKE
    #include <functional>
#endif // VX_HAVE_STD_INVOKE

#include "vertex/config/type_traits.hpp"

namespace vx {
namespace fn {

#if VX_HAVE_STD_INVOKE

template <typename Callable, typename... Args>
constexpr auto invoke(Callable&& fn, Args&&... args) noexcept(type_traits::is_nothrow_invocable<Callable, Args...>::value)
{
    return std::invoke(std::forward<Callable>(fn), std::forward<Args>(args)...);
}

#else

// https://github.com/gcc-mirror/gcc/blob/e8c2f3a427a96a8626f005d4b38492b5d0a96c03/libstdc%2B%2B-v3/include/bits/invoke.h#L92

//=============================================================================
// invoke
//=============================================================================

namespace _priv {

// Used by invoke_impl instead of std::forward<T> so that a
// reference_wrapper is converted to an lvalue-reference.
template <typename T, typename U = typename type_traits::_priv::inv_unwrap<T>::type>
constexpr U&& invfwd(typename std::remove_reference<T>::type& t) noexcept
{
    return static_cast<U&&>(t);
}

template <typename Res, typename Fn, typename... Args>
constexpr Res invoke_impl(type_traits::_priv::invoke_other, Fn&& fn, Args&&... args)
{
    return std::forward<Fn>(fn)(std::forward<Args>(args)...);
}

template <typename Res, typename MemFn, typename T, typename... Args>
constexpr Res invoke_impl(type_traits::_priv::invoke_memfun_ref, MemFn&& fn, T&& t, Args&&... args)
{
    return (invfwd<T>(t).*Fn)(std::forward<Args>(args)...);
}

template <typename Res, typename MemFn, typename T, typename... Args>
constexpr Res invoke_impl(type_traits::_priv::invoke_memfun_deref, MemFn&& fn, T&& t, Args&&... args)
{
    return ((*std::forward<T>(t)).*fn)(std::forward<Args>(args)...);
}

template <typename Res, typename MemPtr, typename T>
constexpr Res invoke_impl(type_traits::_priv::invoke_memobj_ref, MemPtr&& fn, T&& t)
{
    return invfwd<T>(t).*fn;
}

template <typename Res, typename MemPtr, typename T>
constexpr Res invoke_impl(type_traits::_priv::invoke_memobj_deref, MemPtr&& fn, T&& t)
{
    return (*std::forward<T>(t)).*fn;
}

} // namespace _priv

/// Invoke a callable object.
template <typename Callable, typename... Args>
constexpr typename type_traits::invoke_result<Callable, Args...>::type
    invoke(Callable&& fn, Args&&... args) noexcept(type_traits::is_nothrow_invocable<Callable, Args...>::value)
{
    using Res = type_traits::invoke_result<Callable, Args...>;
    using T = typename Res::type;
    using tag = typename Res::invoke_type;
    return type_traits::_priv::invoke_impl<T>(
        tag{},
        std::forward<Callable>(fn),
        std::forward<Args>(args)...);
}

#endif // VX_HAVE_STD_INVOKE

//==============================================================================
// function pass helper
//==============================================================================

namespace _priv {

// pass function object by value as a reference
template <typename Func>
struct ref_func
{

    template <typename... Args>
    constexpr decltype(auto) operator()(Args&&... args)
    {
        VX_IF_CONSTEXPR (std::is_member_pointer<Func>::value)
        {
            return invoke(func, std::forward<Args>(args)...);
        }
        else
        {
            return func(std::forward<Args>(args)...);
        }
    }

    Func& func;
};

} // namespace _priv

template <typename Func>
VX_NO_DISCARD constexpr auto pass_func(Func& func) noexcept
{
    constexpr bool pass_by_value = ((sizeof(Func) <= sizeof(void*)) &&
        std::is_trivially_copy_constructible<Func>::value &&
        std::is_trivially_destructible<Func>::value);

    VX_IF_CONSTEXPR (pass_by_value)
    {
        return func;
    }
    else
    {
        // pass functor by "reference"
        return _priv::ref_func<Func>{ func };
    }
}

} // namespace fn
} // namespace vx
