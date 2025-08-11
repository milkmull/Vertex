#pragma once

#include "vertex/config/language_config.hpp"

#include <type_traits>
#include <iterator>
#include <limits>

namespace vx {
namespace type_traits {

template <typename...>
using void_t = void;

template<bool v>
using bool_constant = std::integral_constant<bool, v>;

template <typename T>
struct type_identity
{
    using type = T;
};

///////////////////////////////////////////////////////////////////////////////
// enable if
///////////////////////////////////////////////////////////////////////////////

template <bool Test, class T = void>
struct enable_if {};

template <class T>
struct enable_if<true, T> { using type = T; };

#define VX_REQUIRES(condition) typename ::vx::type_traits::enable_if<(condition), int>::type = 0

///////////////////////////////////////////////////////////////////////////////
// conjunction disjunction
///////////////////////////////////////////////////////////////////////////////

// https://github.com/gcc-mirror/gcc/blob/da8aaa7784810e23123639c06c22eb6d37ec945c/libstdc%2B%2B-v3/include/std/type_traits#L172

namespace _priv {

// A variadic alias template that resolves to its first argument.
template<typename T, typename...>
using first_t = T;

// These are deliberately not defined.
template <typename... Bn>
first_t<std::false_type, typename enable_if<!bool(Bn::value)>::type...> or_fn(int);

template <typename... Bn>
std::true_type or_fn(...);

template <typename... Bn>
first_t<std::true_type, typename enable_if<bool(Bn::value)>::type...> and_fn(int);

template <typename... Bn>
std::false_type and_fn(...);

} // namespace detail

template <typename... Bn>
struct conjunction : decltype(_priv::and_fn<Bn...>(0)) {};

template <typename... Bn>
struct disjunction : decltype(_priv::or_fn<Bn...>(0)) {};

template<typename Bn>
struct negation : bool_constant<!bool(Bn::value)> {};

///////////////////////////////////////////////////////////////////////////////
// is any of
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename... Types>
struct is_any_of : disjunction<std::is_same<T, Types>...> {};

///////////////////////////////////////////////////////////////////////////////
// is iterator
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename = void>
struct is_iterator : std::false_type {};

template <typename T>
struct is_iterator<T, void_t<typename std::iterator_traits<T>::iterator_category>> : std::true_type {};

///////////////////////////////////////////////////////////////////////////////
// element type
///////////////////////////////////////////////////////////////////////////////

namespace _priv {

template <typename T, typename = void>
struct value_type_impl { using type = void; };

template <typename T>
struct value_type_impl<T, void_t<typename T::value_type>>
{
    using type = typename T::value_type;
};

template <typename T>
struct value_type_impl<T*> { using type = T; };

template <typename T, size_t N>
struct value_type_impl<T[N]> { using type = T; };

} // namespace _priv

template <typename T>
struct value_type
{
    using type = typename _priv::value_type_impl<typename std::remove_cv<T>::type>::type;
};

///////////////////////////////////////////////////////////////////////////////
// strippers
///////////////////////////////////////////////////////////////////////////////

template <typename T>
struct remove_cvref
{
    using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
};

///////////////////////////////////////////////////////////////////////////////
// invoke helpers
///////////////////////////////////////////////////////////////////////////////

#if defined(__cpp_lib_is_invocable)

template <typename Fn, typename... Args>
using invoke_result = std::invoke_result<Fn, Args...>;

template <typename Fn, typename... Args>
using is_invocable = std::is_invocable<Fn, Args...>;

template<typename Fn, typename... Args>
using is_nothrow_invocable = std::is_nothrow_invocable<Fn, Args...>;

#else

namespace _priv {

// https://github.com/gcc-mirror/gcc/blob/da8aaa7784810e23123639c06c22eb6d37ec945c/libstdc%2B%2B-v3/include/bits/invoke.h#L55

template <typename T>
struct success_type
{
    using type = T;
};

struct failure_type {};

struct invoke_memfun_ref {};
struct invoke_memfun_deref {};
struct invoke_memobj_ref {};
struct invoke_memobj_deref {};
struct invoke_other {};

// Associate a tag type with a specialization of success_type.
template <typename T, typename Tag>
struct result_of_success : success_type<T>
{
    using invoke_type = Tag;
};

///////////////////////////////////////////////////////////////////////////////
// result_of_memobj
///////////////////////////////////////////////////////////////////////////////

// https://github.com/gcc-mirror/gcc/blob/3014f8787196d7c0d15d24195c8f07167968ff55/libstdc%2B%2B-v3/include/std/type_traits#L2675

 // [Fn.require] paragraph 1 bullet 3:
struct result_of_memobj_ref_impl
{
    template <typename FnPtr, typename T>
    static result_of_success<decltype(
        std::declval<T>().*std::declval<FnPtr>()
        ), invoke_memobj_ref> test(int);

    template <typename, typename>
    static failure_type test(...);
};

template <typename MemPtr, typename Arg>
struct result_of_memobj_ref : private result_of_memobj_ref_impl
{
    using type = decltype(test<MemPtr, Arg>(0));
};

// [Fn.require] paragraph 1 bullet 4:
struct result_of_memobj_deref_impl
{
    template <typename FnPtr, typename T>
    static result_of_success<decltype(
        (*std::declval<T>()).*std::declval<FnPtr>()
        ), invoke_memobj_deref> test(int);

    template <typename, typename>
    static failure_type test(...);
};

template <typename MemPtr, typename Arg>
struct result_of_memobj_deref : private result_of_memobj_deref_impl
{
    using type = decltype(test<MemPtr, Arg>(0));
};

template <typename MemPtr, typename Arg>
struct result_of_memobj {};

template <typename Res, typename T, typename Arg>
struct result_of_memobj<Res T::*, Arg>
{
    using Argval = type_traits::remove_cvref<Arg>;
    using MemPtr = Res T::*;

    using type = typename std::conditional<
        type_traits::disjunction<
        std::is_same<Argval, T>,
        std::is_base_of<T, Argval>
        >::value,
        result_of_memobj_ref<MemPtr, Arg>,
        result_of_memobj_deref<MemPtr, Arg>
    >::type;
};

///////////////////////////////////////////////////////////////////////////////
// result_of_memfun
///////////////////////////////////////////////////////////////////////////////

// [Fn.require] paragraph 1 bullet 1:
struct result_of_memfun_ref_impl
{
    template <typename FnPtr, typename T, typename... Args>
    static result_of_success<decltype(
        (std::declval<T>().*std::declval<FnPtr>())(std::declval<Args>()...)
        ), invoke_memfun_ref> test(int);

    template <typename...>
    static failure_type test(...);
};

template <typename MemPtr, typename Arg, typename... Args>
struct result_of_memfun_ref : private result_of_memfun_ref_impl
{
    using type = decltype(test<MemPtr, Arg, Args...>(0));
};

// [Fn.require] paragraph 1 bullet 2:
struct result_of_memfun_deref_impl
{
    template <typename FnPtr, typename T, typename... Args>
    static result_of_success<decltype(
        ((*std::declval<T>()).*std::declval<FnPtr>())(std::declval<Args>()...)
        ), invoke_memfun_deref> test(int);

    template <typename...>
    static failure_type test(...);
};

template <typename MemPtr, typename Arg, typename... Args>
struct result_of_memfun_deref : private result_of_memfun_deref_impl
{
    using type = decltype(test<MemPtr, Arg, Args...>(0));
};

template <typename MemPtr, typename Arg, typename... Args>
struct result_of_memfun;

template <typename Res, typename T, typename Arg, typename... Args>
struct result_of_memfun<Res T::*, Arg, Args...>
{
    using Argval = typename std::remove_reference<Arg>::type;
    using MemPtr = Res T::*;

    using type = typename std::conditional<
        std::is_base_of<T, Argval>::value,
        result_of_memfun_ref<MemPtr, Arg, Args...>,
        result_of_memfun_deref<MemPtr, Arg, Args...>
    >::type;
};

///////////////////////////////////////////////////////////////////////////////
// inv_unwrap
///////////////////////////////////////////////////////////////////////////////

// Used by result_of, invoke etc. to unwrap a reference_wrapper.
template <typename T, typename U = type_traits::remove_cvref<T>>
struct inv_unwrap
{
    using type = T;
};

template <typename T, typename U>
struct inv_unwrap<T, std::reference_wrapper<U>>
{
    using type = U&;
};

///////////////////////////////////////////////////////////////////////////////
// result_of
///////////////////////////////////////////////////////////////////////////////

template <bool, bool, typename Fn, typename... Args>
struct result_of_impl {};

template <typename MemPtr, typename Arg>
struct result_of_impl<true, false, MemPtr, Arg>
    : public result_of_memobj<typename std::decay<MemPtr>::type, typename inv_unwrap<Arg>::type> {};

template <typename MemPtr, typename Arg, typename... Args>
struct result_of_impl<false, true, MemPtr, Arg, Args...>
    : public result_of_memfun<typename std::decay<MemPtr>::type, typename inv_unwrap<Arg>::type, Args...> {};

// [Fn.require] paragraph 1 bullet 5:
struct result_of_other_impl
{
    template <typename Fn, typename... Args>
    static result_of_success<decltype(
        std::declval<Fn>()(std::declval<Args>()...)
        ), invoke_other> test(int);

    template <typename...>
    static failure_type test(...);
};

template <typename Fn, typename... Args>
struct result_of_impl<false, false, Fn, Args...> : private result_of_other_impl
{
    using type = decltype(test<Fn, Args...>(0));
};

} // namespace _priv

///////////////////////////////////////////////////////////////////////////////
// invoke_result
///////////////////////////////////////////////////////////////////////////////

// https://github.com/gcc-mirror/gcc/blob/c207dcf393b864adc8eb41bbbcd630a6cfdc145a/libstdc%2B%2B-v3/include/std/type_traits#L2796

// invoke_result (std::invoke_res for C++11)
template <typename Fn, typename... Args>
struct invoke_result : public _priv::result_of_impl<
    std::is_member_object_pointer<typename std::remove_reference<Fn>::type>::value,
    std::is_member_function_pointer<typename std::remove_reference<Fn>::type>::value,
    Fn, Args...
>::type {};

namespace _priv {

///////////////////////////////////////////////////////////////////////////////
// is_invocable
///////////////////////////////////////////////////////////////////////////////

template <typename Res, typename Ret, bool = std::is_void<Ret>::value, typename = void>
struct is_invocable_impl : std::false_type
{
    using nothrow_conv = std::false_type; // For is_nothrow_invocable_r
};

// Used for valid INVOKE and INVOKE<void> expressions.
template <typename Res, typename Ret>
struct is_invocable_impl<Res, Ret, /* std::is_void<Ret> = */ true, type_traits::void_t<typename Res::type>> : std::true_type
{
    using nothrow_conv = std::true_type; // For is_nothrow_invocable_r
};

// https://github.com/gcc-mirror/gcc/blob/c207dcf393b864adc8eb41bbbcd630a6cfdc145a/libstdc%2B%2B-v3/include/std/type_traits#L3171

VX_DISABLE_GCC_WARNING_PUSH()
VX_DISABLE_GCC_WARNING("-Wctor-dtor-privacy")

// Used for INVOKE<R> expressions to check the implicit conversion to R.
template <typename Res, typename Ret>
struct is_invocable_impl<Res, Ret, /* std::is_void<Ret> = */ false, type_traits::void_t<typename Res::type>>
{
private:
    // The type of the INVOKE expression.
    using Res_t = typename Res::type;

    // Unlike declval, this doesn't add_rvalue_reference, so it respects
    // guaranteed copy elision.
    static Res_t get() noexcept;

    // Used to check if Res_t can implicitly convert to T.
    template <typename T>
    static void conv(typename type_traits::type_identity<T>::type) noexcept;

    // This overload is viable if INVOKE(f, args...) can convert to T.
    template <typename T, bool nothrow = noexcept(conv<T>(get())), typename = decltype(conv<T>(get())),

#if VX_HAS_BUILTIN(__reference_converts_from_temporary)
        bool dangle = __reference_converts_from_temporary(T, Res_t)
#else
        bool dangle = false
#endif
    >
    static type_traits::bool_constant<nothrow && !dangle> test(int);

    template <typename T, bool = false>
    static std::false_type test(...);

public:
    // For is_invocable_r
    using type = decltype(_S_test<Ret, /* Nothrow = */ true>(1));

    // For is_nothrow_invocable_r
    using nothrow_conv = decltype(test<Ret>(1));
};

VX_DISABLE_GCC_WARNING_POP()

} // namespace _priv

template <typename Fn, typename... Args>
struct is_invocable : _priv::is_invocable_impl<invoke_result<Fn, Args...>, void>::type {};

namespace _priv {

///////////////////////////////////////////////////////////////////////////////
// call_is_nothrow
///////////////////////////////////////////////////////////////////////////////

template <typename Fn, typename T, typename... Args>
constexpr bool call_is_nt(invoke_memfun_ref)
{
    using U = typename inv_unwrap<T>::type;
    return noexcept((std::declval<U>().*std::declval<Fn>())(std::declval<Args>()...));
}

template <typename Fn, typename T, typename... Args>
constexpr bool call_is_nt(invoke_memfun_deref)
{
    return noexcept(((*std::declval<T>()).*std::declval<Fn>())(std::declval<Args>()...));
}

template <typename Fn, typename T>
constexpr bool call_is_nt(invoke_memobj_ref)
{
    using U = typename inv_unwrap<T>::type;
    return noexcept(std::declval<U>().*std::declval<Fn>());
}

template <typename Fn, typename T>
constexpr bool call_is_nt(invoke_memobj_deref)
{
    return noexcept((*std::declval<T>()).*std::declval<Fn>());
}

template <typename Fn, typename... Args>
constexpr bool call_is_nt(invoke_other)
{
    return noexcept(std::declval<Fn>()(std::declval<Args>()...));
}

template <typename Res, typename Fn, typename... Args>
struct call_is_nothrow_impl : type_traits::bool_constant<call_is_nt<Fn, Args...>(typename Res::invoke_type{})> {};

template<typename Fn, typename... Args>
using call_is_nothrow = _priv::call_is_nothrow_impl<invoke_result<Fn, Args...>, Fn, Args...>;

} // namespace _priv

///////////////////////////////////////////////////////////////////////////////
// is_nothrow_invocable
///////////////////////////////////////////////////////////////////////////////

// https://github.com/gcc-mirror/gcc/blob/c207dcf393b864adc8eb41bbbcd630a6cfdc145a/libstdc%2B%2B-v3/include/std/type_traits#L3265

template <typename Fn, typename... Args>
struct is_nothrow_invocable : type_traits::conjunction<is_invocable<Fn, Args...>, _priv::call_is_nothrow<Fn, Args...>>::type {};

#endif // __cpp_lib_is_invocable

///////////////////////////////////////////////////////////////////////////////
// integer_sequence
///////////////////////////////////////////////////////////////////////////////

template <typename T, T... In>
struct integer_sequence
{
    static_assert(std::is_integral<T>::value, "integer_sequence<T, I...> requires T to be an integral type.");
    using value_type = T;

    static constexpr size_t size() noexcept { return sizeof...(In); }
};

template <size_t... In>
using index_sequence = integer_sequence<size_t, In...>;

template <typename T, T N>
using make_integer_sequence
#if defined(_MSC_VER) || VX_HAS_BUILTIN(__make_integer_seq)
    = __make_integer_seq<integer_sequence, T, N>;
#elif VX_HAS_BUILTIN(__integer_pack)
    = integer_sequence<T, __integer_pack(N)...>;
#else
#   error no make integer sequence implementation
#endif

template <size_t N>
using make_index_sequence = make_integer_sequence<size_t, N>;

///////////////////////////////////////////////////////////////////////////////
// char types
///////////////////////////////////////////////////////////////////////////////

template <typename T>
struct is_char : is_any_of<typename std::remove_cv<T>::type,
    char,
    wchar_t,
#if defined(__cpp_char8_t)
    char8_t
#endif // __cpp_char8_t
    char16_t,
    char32_t
> {};

} // namespace type_traits
} // namespace vx