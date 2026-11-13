#pragma once

#include <iterator>
#include <memory>
#include <type_traits>

#include "vertex/config/feature_detection.hpp"
#include "vertex/config/language_config.hpp"

// clang-format off

namespace vx {
namespace traits {

//==============================================================================
// groups
//==============================================================================

using std::pointer_traits;
using std::iterator_traits;

//==============================================================================
// 1. fundamentals
//==============================================================================

using std::integral_constant;
using std::true_type;
using std::false_type;
using std::declval;

template <typename...>
using void_t = void;

template <bool V>
using bool_constant = std::integral_constant<bool, V>;

// Struct (not alias) so the argument stays dependent; safe for lazy static_assert.
template <typename T>
struct always_false : std::false_type
{};

template <typename T>
struct type_identity
{
    using type = T;
};

template <typename T>
using identity_t = typename type_identity<T>::type;

template <typename T, typename...>
struct first
{
    using type = T;
};

template <typename T, typename... Ts>
using first_t = typename first<T, Ts...>::type;

//==============================================================================
// 2. type modifications
//==============================================================================

// --- cv ---
using std::remove_const;
using std::remove_volatile;
using std::remove_cv;
using std::add_const;
using std::add_volatile;
using std::add_cv;

// --- references ---
using std::remove_reference;
using std::add_lvalue_reference;
using std::add_rvalue_reference;

// --- pointers ---
using std::remove_pointer;
using std::add_pointer;

// --- sign ---
using std::make_signed;
using std::make_unsigned;

// --- arrays ---
using std::remove_extent;
using std::remove_all_extents;

// --- misc ---
using std::decay;
using std::enable_if;
using std::conditional;
using std::common_type;
using std::underlying_type;
using std::aligned_storage;

// --- _t helpers (alias templates so they work regardless of language level) ---
template <typename T> using remove_const_t          = typename std::remove_const<T>::type;
template <typename T> using remove_volatile_t       = typename std::remove_volatile<T>::type;
template <typename T> using remove_cv_t             = typename std::remove_cv<T>::type;
template <typename T> using add_const_t             = typename std::add_const<T>::type;
template <typename T> using add_volatile_t          = typename std::add_volatile<T>::type;
template <typename T> using add_cv_t                = typename std::add_cv<T>::type;
template <typename T> using remove_reference_t      = typename std::remove_reference<T>::type;
template <typename T> using add_lvalue_reference_t  = typename std::add_lvalue_reference<T>::type;
template <typename T> using add_rvalue_reference_t  = typename std::add_rvalue_reference<T>::type;
template <typename T> using remove_pointer_t        = typename std::remove_pointer<T>::type;
template <typename T> using add_pointer_t           = typename std::add_pointer<T>::type;
template <typename T> using make_signed_t           = typename std::make_signed<T>::type;
template <typename T> using make_unsigned_t         = typename std::make_unsigned<T>::type;
template <typename T> using remove_extent_t         = typename std::remove_extent<T>::type;
template <typename T> using remove_all_extents_t    = typename std::remove_all_extents<T>::type;
template <typename T> using decay_t                 = typename std::decay<T>::type;
template <typename T> using underlying_type_t       = typename std::underlying_type<T>::type;
template <typename... T> using common_type_t        = typename std::common_type<T...>::type;
template <bool B, typename T = void> using enable_if_t = typename std::enable_if<B, T>::type;
template <bool B, typename T, typename F> using conditional_t = typename std::conditional<B, T, F>::type;

// --- remove_cvref (custom; std::remove_cvref is C++20) ---
// Note: this is a struct with ::type, like the other traits. Use remove_cvref_t for the type.
template <typename T>
using remove_cvref = std::remove_cv<typename std::remove_reference<T>::type>;

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

#define VX_STRIP_T(T) typename ::vx::traits::remove_cvref<T>::type

//==============================================================================
// 3. enable if
//==============================================================================

#define VX_REQUIRES_TYPE(...)   typename ::vx::traits::enable_if<(__VA_ARGS__), void>::type
#define VX_REQUIRES(...)        typename ::vx::traits::enable_if<(__VA_ARGS__), int>::type = 0
#define VX_FN_REQUIRES(...)     template <typename _dummy = void, VX_REQUIRES(::vx::traits::is_same<_dummy, _dummy>::value && (__VA_ARGS__))>

//==============================================================================
// 4. logical operations (conjunction / disjunction / negation / is_any_of)
//==============================================================================

// https://github.com/gcc-mirror/gcc/blob/da8aaa7784810e23123639c06c22eb6d37ec945c/libstdc%2B%2B-v3/include/std/traits#L172

namespace _traits_priv {

// These are deliberately not defined.
template <typename... Bn>
first_t<std::false_type, typename std::enable_if<!bool(Bn::value)>::type...> or_fn(int);

template <typename... Bn>
std::true_type or_fn(...);

template <typename... Bn>
first_t<std::true_type, typename std::enable_if<bool(Bn::value)>::type...> and_fn(int);

template <typename... Bn>
std::false_type and_fn(...);

} // namespace _traits_priv

template <typename... Bn>
struct conjunction : decltype(_traits_priv::and_fn<Bn...>(0))
{};

template <typename... Bn>
struct disjunction : decltype(_traits_priv::or_fn<Bn...>(0))
{};

template <typename Bn>
struct negation : bool_constant<!bool(Bn::value)>
{};

template <typename T, typename... Types>
struct is_any_of : disjunction<std::is_same<T, Types>...>
{};

//==============================================================================
// 5. type categories
//==============================================================================

// --- primary ---
using std::is_void;
using std::is_integral;
using std::is_floating_point;
using std::is_array;
using std::is_enum;
using std::is_union;
using std::is_class;
using std::is_function;
using std::is_pointer;
using std::is_lvalue_reference;
using std::is_rvalue_reference;
using std::is_member_object_pointer;
using std::is_member_function_pointer;

#if VX_CPP_STANDARD >= 14
using std::is_null_pointer;
#endif

// --- composite ---
using std::is_reference;
using std::is_arithmetic;
using std::is_fundamental;
using std::is_object;
using std::is_scalar;
using std::is_compound;
using std::is_member_pointer;

//==============================================================================
// 6. type properties
//==============================================================================

using std::is_const;
using std::is_volatile;
using std::is_trivial;
using std::is_trivially_copyable;
using std::is_standard_layout;
using std::is_empty;
using std::is_polymorphic;
using std::is_abstract;
using std::is_signed;
using std::is_unsigned;

#if VX_CPP_STANDARD >= 14
using std::is_final;
#endif

#if VX_CPP_STANDARD >= 17
using std::is_aggregate;
#endif

// --- numeric helpers (custom) ---
template <typename T>
struct is_signed_integral : bool_constant<std::is_integral<T>::value && std::is_signed<T>::value>
{};

template <typename T>
struct is_unsigned_integral : bool_constant<std::is_integral<T>::value && std::is_unsigned<T>::value>
{};

//==============================================================================
// 7. supported operations
//==============================================================================

// --- construction ---
using std::is_constructible;
using std::is_trivially_constructible;
using std::is_nothrow_constructible;

using std::is_default_constructible;
using std::is_trivially_default_constructible;
using std::is_nothrow_default_constructible;

using std::is_copy_constructible;
using std::is_trivially_copy_constructible;
using std::is_nothrow_copy_constructible;

using std::is_move_constructible;
using std::is_trivially_move_constructible;
using std::is_nothrow_move_constructible;

// --- assignment ---
using std::is_assignable;
using std::is_trivially_assignable;
using std::is_nothrow_assignable;

using std::is_copy_assignable;
using std::is_trivially_copy_assignable;
using std::is_nothrow_copy_assignable;

using std::is_move_assignable;
using std::is_trivially_move_assignable;
using std::is_nothrow_move_assignable;

// --- destruction ---
using std::is_destructible;
using std::is_trivially_destructible;
using std::is_nothrow_destructible;
using std::has_virtual_destructor;

// --- swap ---
#if VX_CPP_STANDARD >= 17
using std::is_swappable;
using std::is_swappable_with;
using std::is_nothrow_swappable;
using std::is_nothrow_swappable_with;
#endif

//==============================================================================
// 8. property queries
//==============================================================================

using std::alignment_of;
using std::rank;
using std::extent;

//==============================================================================
// 9. type relationships
//==============================================================================

using std::is_same;
using std::is_base_of;
using std::is_convertible;

// Whether T is a (cv-ref-stripped) pointer to (cv-stripped) U. (custom)
template <typename T, typename U>
struct is_pointer_to
    : bool_constant<
          std::is_pointer<typename remove_cvref<T>::type>::value &&
          std::is_same<
              typename std::remove_cv<
                  typename std::remove_pointer<
                      typename remove_cvref<T>::type
                  >::type
              >::type,
              typename std::remove_cv<U>::type
          >::value
      >
{};

//==============================================================================
// 10. invoke helpers
//==============================================================================

#if VX_HAVE_STD_IS_INVOKABLE

template <typename Fn, typename... Args>
using invoke_result = std::invoke_result<Fn, Args...>;

template <typename Fn, typename... Args>
using is_invocable = std::is_invocable<Fn, Args...>;

template <typename Fn, typename... Args>
using is_nothrow_invocable = std::is_nothrow_invocable<Fn, Args...>;

#else

namespace _traits_priv {

// https://github.com/gcc-mirror/gcc/blob/da8aaa7784810e23123639c06c22eb6d37ec945c/libstdc%2B%2B-v3/include/bits/invoke.h#L55

template <typename T>
struct success_type
{
    using type = T;
};

struct failure_type
{};

struct invoke_memfun_ref
{};
struct invoke_memfun_deref
{};
struct invoke_memobj_ref
{};
struct invoke_memobj_deref
{};
struct invoke_other
{};

// Associate a tag type with a specialization of success_type.
template <typename T, typename Tag>
struct result_of_success : success_type<T>
{
    using invoke_type = Tag;
};

//------------------------------------------------------------------------------
// result_of_memobj
//------------------------------------------------------------------------------

// https://github.com/gcc-mirror/gcc/blob/3014f8787196d7c0d15d24195c8f07167968ff55/libstdc%2B%2B-v3/include/std/traits#L2675

// [Fn.require] paragraph 1 bullet 3:
struct result_of_memobj_ref_impl
{
    template <typename FnPtr, typename T>
    static result_of_success<decltype(std::declval<T>().*std::declval<FnPtr>()), invoke_memobj_ref> test(int);

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
    static result_of_success<decltype((*std::declval<T>()).*std::declval<FnPtr>()), invoke_memobj_deref> test(int);

    template <typename, typename>
    static failure_type test(...);
};

template <typename MemPtr, typename Arg>
struct result_of_memobj_deref : private result_of_memobj_deref_impl
{
    using type = decltype(test<MemPtr, Arg>(0));
};

template <typename MemPtr, typename Arg>
struct result_of_memobj
{};

template <typename Res, typename T, typename Arg>
struct result_of_memobj<Res T::*, Arg>
{
    using Argval = remove_cvref_t<Arg>;
    using MemPtr = Res T::*;

    using type = typename std::conditional<
        disjunction<
            std::is_same<Argval, T>,
            std::is_base_of<T, Argval>>::value,
        result_of_memobj_ref<MemPtr, Arg>,
        result_of_memobj_deref<MemPtr, Arg>>::type;
};

//------------------------------------------------------------------------------
// result_of_memfun
//------------------------------------------------------------------------------

// [Fn.require] paragraph 1 bullet 1:
struct result_of_memfun_ref_impl
{
    template <typename FnPtr, typename T, typename... Args>
    static result_of_success<decltype((std::declval<T>().*std::declval<FnPtr>())(std::declval<Args>()...)), invoke_memfun_ref> test(int);

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
    static result_of_success<decltype(((*std::declval<T>()).*std::declval<FnPtr>())(std::declval<Args>()...)), invoke_memfun_deref> test(int);

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
        result_of_memfun_deref<MemPtr, Arg, Args...>>::type;
};

//------------------------------------------------------------------------------
// inv_unwrap
//------------------------------------------------------------------------------

// Used by result_of, invoke etc. to unwrap a reference_wrapper.
template <typename T, typename U = remove_cvref_t<T>>
struct inv_unwrap
{
    using type = T;
};

template <typename T, typename U>
struct inv_unwrap<T, std::reference_wrapper<U>>
{
    using type = U&;
};

//------------------------------------------------------------------------------
// result_of
//------------------------------------------------------------------------------

template <bool, bool, typename Fn, typename... Args>
struct result_of_impl
{};

template <typename MemPtr, typename Arg>
struct result_of_impl<true, false, MemPtr, Arg> : result_of_memobj<typename std::decay<MemPtr>::type, typename inv_unwrap<Arg>::type>
{};

template <typename MemPtr, typename Arg, typename... Args>
struct result_of_impl<false, true, MemPtr, Arg, Args...> : result_of_memfun<typename std::decay<MemPtr>::type, typename inv_unwrap<Arg>::type, Args...>
{};

// [Fn.require] paragraph 1 bullet 5:
struct result_of_other_impl
{
    template <typename Fn, typename... Args>
    static result_of_success<decltype(std::declval<Fn>()(std::declval<Args>()...)), invoke_other> test(int);

    template <typename...>
    static failure_type test(...);
};

template <typename Fn, typename... Args>
struct result_of_impl<false, false, Fn, Args...> : private result_of_other_impl
{
    using type = decltype(test<Fn, Args...>(0));
};

} // namespace _traits_priv

//------------------------------------------------------------------------------
// invoke_result
//------------------------------------------------------------------------------

// https://github.com/gcc-mirror/gcc/blob/c207dcf393b864adc8eb41bbbcd630a6cfdc145a/libstdc%2B%2B-v3/include/std/traits#L2796

template <typename Fn, typename... Args>
struct invoke_result : _traits_priv::result_of_impl<std::is_member_object_pointer<typename std::remove_reference<Fn>::type>::value, std::is_member_function_pointer<typename std::remove_reference<Fn>::type>::value, Fn, Args...>::type
{};

namespace _traits_priv {

//------------------------------------------------------------------------------
// is_invocable
//------------------------------------------------------------------------------

template <typename Res, typename Ret, bool = std::is_void<Ret>::value, typename = void>
struct is_invocable_impl : std::false_type
{
    using nothrow_conv = std::false_type; // For is_nothrow_invocable_r
};

// Used for valid INVOKE and INVOKE<void> expressions.
template <typename Res, typename Ret>
struct is_invocable_impl<Res, Ret, /* std::is_void<Ret> = */ true, void_t<typename Res::type>> : std::true_type
{
    using nothrow_conv = std::true_type; // For is_nothrow_invocable_r
};

// https://github.com/gcc-mirror/gcc/blob/c207dcf393b864adc8eb41bbbcd630a6cfdc145a/libstdc%2B%2B-v3/include/std/traits#L3171

VX_DISABLE_GCC_WARNING_PUSH()
VX_DISABLE_GCC_WARNING("-Wctor-dtor-privacy")

// Used for INVOKE<R> expressions to check the implicit conversion to R.
template <typename Res, typename Ret>
struct is_invocable_impl<Res, Ret, /* std::is_void<Ret> = */ false, void_t<typename Res::type>>
{
private:

    // The type of the INVOKE expression.
    using Res_t = typename Res::type;

    // Unlike declval, this doesn't add_rvalue_reference, so it respects
    // guaranteed copy elision.
    static Res_t get() noexcept;

    // Used to check if Res_t can implicitly convert to T.
    template <typename T>
    static void conv(typename type_identity<T>::type) noexcept;

    // This overload is viable if INVOKE(f, args...) can convert to T.
    template <typename T, bool nothrow = noexcept(conv<T>(get())), typename = decltype(conv<T>(get())),

    #if VX_HAS_BUILTIN(__reference_converts_from_temporary)
        bool dangle = __reference_converts_from_temporary(T, Res_t)
    #else
        bool dangle = false
    #endif
        >
    static bool_constant<nothrow && !dangle> test(int);

    template <typename T, bool = false>
    static std::false_type test(...);

public:

    // For is_invocable_r
    using type = decltype(test<Ret, /* Nothrow = */ true>(1));

    // For is_nothrow_invocable_r
    using nothrow_conv = decltype(test<Ret>(1));
};

VX_DISABLE_GCC_WARNING_POP()

} // namespace _traits_priv

template <typename Fn, typename... Args>
struct is_invocable : _traits_priv::is_invocable_impl<invoke_result<Fn, Args...>, void>::type
{};

namespace _traits_priv {

//------------------------------------------------------------------------------
// call_is_nothrow
//------------------------------------------------------------------------------

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
struct call_is_nothrow_impl : bool_constant<call_is_nt<Fn, Args...>(typename Res::invoke_type{})>
{};

template <typename Fn, typename... Args>
using call_is_nothrow = call_is_nothrow_impl<invoke_result<Fn, Args...>, Fn, Args...>;

} // namespace _traits_priv

template <typename Fn, typename... Args>
struct is_nothrow_invocable : conjunction<is_invocable<Fn, Args...>, _traits_priv::call_is_nothrow<Fn, Args...>>::type
{};

#endif // VX_HAVE_STD_IS_INVOKABLE

//==============================================================================
// 11. integer_sequence
//==============================================================================

template <typename T, T... In>
struct integer_sequence
{
    static_assert(std::is_integral<T>::value, "integer_sequence<T, I...> requires T to be an integral type.");
    using value_type = T;

    static constexpr size_t size() noexcept
    {
        return sizeof...(In);
    }
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
    #error no make integer sequence implementation
#endif

template <size_t N>
using make_index_sequence = make_integer_sequence<size_t, N>;

//==============================================================================
// 12. iterator traits
//==============================================================================

template <typename T, typename = void>
struct iterator_cat
{};

template <typename T>
struct iterator_cat<T, void_t<typename std::iterator_traits<T>::iterator_category>>
{
    using type = typename std::iterator_traits<T>::iterator_category;
};

template <typename T, typename = void>
struct is_iterator : std::false_type
{};

template <typename T>
struct is_iterator<T, void_t<typename iterator_cat<T>::type>> : std::true_type
{};

template <typename T, typename = void>
struct is_forward_iterator : std::false_type
{};

template <typename T>
struct is_forward_iterator<T, void_t<typename iterator_cat<T>::type>>
    : std::is_base_of<
        std::forward_iterator_tag,
        typename iterator_cat<T>::type>
{};

template <typename T, typename = void>
struct is_random_access_iterator : std::false_type
{};

template <typename T>
struct is_random_access_iterator<T, void_t<typename iterator_cat<T>::type>>
    : std::is_base_of<
        std::random_access_iterator_tag,
        typename iterator_cat<T>::type>
{};

template <typename T, typename = void>
struct is_non_pointer_iterator : std::false_type
{};

template <typename T>
struct is_non_pointer_iterator<T, typename enable_if<
    is_iterator<T>::value && !std::is_pointer<T>::value
>::type> : std::true_type
{};

// --- element / value type ---

namespace _traits_priv {

template <typename T, typename = void>
struct value_type_impl
{
    using type = void;
};

template <typename T>
struct value_type_impl<T, void_t<typename T::value_type>>
{
    using type = typename T::value_type;
};

template <typename T>
struct value_type_impl<T*>
{
    using type = T;
};

template <typename T, size_t N>
struct value_type_impl<T[N]>
{
    using type = T;
};

template <typename T>
struct value_type_impl<T[]>
{
    using type = T;
};

template <typename IT>
using iter_ref_t = typename std::iterator_traits<IT>::reference;

template <typename IT>
using iter_value_t = typename std::iterator_traits<IT>::value_type;

} // namespace _traits_priv

template <typename T>
struct value_type
{
    using type = typename _traits_priv::value_type_impl<typename std::remove_cv<T>::type>::type;
};

template <typename T, typename = void>
struct iter_reference
{
};

template <typename T>
struct iter_reference<T, void_t<decltype(*std::declval<T&>())>>
{
    using type = decltype(*std::declval<T&>());
};

//==============================================================================
// 13. character traits
//==============================================================================

template <typename T>
struct is_char : is_any_of<typename std::remove_cv<T>::type, char, wchar_t,
#if VX_HAVE_STD_CHAR8_T
                     char8_t,
#endif // VX_HAVE_STD_CHAR8_T
                     char16_t,
                     char32_t>
{};

template <typename T>
struct is_char_iter : is_char<typename value_type<T>::type> {};

//==============================================================================
// 14. zero constructible / unwrap enum
//==============================================================================

// Whether the default value of a type is just all-0 bytes.
// This can most commonly be exploited by using memset for these types instead of loop-construct.
// Trivially constructible types are also zero-constructible.
template <typename T>
struct is_zero_constructible : std::is_trivially_default_constructible<T>
{};

template <typename T>
struct is_zero_constructible<const T> : is_zero_constructible<T>
{};

template <typename T>
struct is_zero_constructible<volatile T> : is_zero_constructible<T>
{};

template <typename T>
struct is_zero_constructible<const volatile T> : is_zero_constructible<T>
{};

template <typename T, bool is_enum = std::is_enum<T>::value>
struct unwrap_enum
{
    using type = typename std::underlying_type<T>::type;
};

template <typename T>
struct unwrap_enum<T, false>
{
    using type = T;
};

//==============================================================================
// 15. memory-operation safety (memset / memmove / memcmp)
//==============================================================================

// --- memset ---

namespace _traits_priv {

template <typename T>
struct is_char_or_bool : std::is_same<T, char>
{};

template <>
struct is_char_or_bool<bool> : std::true_type
{};

template <typename T>
struct is_char_or_byte_or_bool : is_char_or_bool<T>
{};

#if VX_HAVE_STD_BYTE
template <>
struct is_char_or_byte_or_bool<std::byte> : std::true_type
{};
#endif // VX_HAVE_STD_BYTE

} // namespace _traits_priv

template <typename IT, typename T>
struct is_fill_memset_safe : bool_constant<std::is_scalar<T>::value && _traits_priv::is_char_or_byte_or_bool<typename unwrap_enum<remove_reference_t<_traits_priv::iter_ref_t<IT>>>::type>::value && !std::is_volatile<remove_reference_t<_traits_priv::iter_ref_t<IT>>>::value && std::is_assignable<_traits_priv::iter_ref_t<IT>, const T&>::value>
{};

template <typename IT, typename T>
struct is_fill_zero_memset_safe : bool_constant<std::is_pointer<IT>::value && std::is_scalar<T>::value && std::is_scalar<_traits_priv::iter_value_t<IT>>::value && !std::is_member_pointer<_traits_priv::iter_value_t<IT>>::value && !std::is_volatile<remove_reference_t<_traits_priv::iter_ref_t<IT>>>::value && std::is_assignable<_traits_priv::iter_ref_t<IT>, const T&>::value>
{};

// --- contiguity / pointer address conversion ---

#if (VX_CPP_STANDARD >= 20)

// When concepts are available, we can detect arbitrary contiguous iterators.
template <typename It>
constexpr bool iterator_is_contiguous = std::contiguous_iterator<It>;

#else

// When concepts aren't available, we can detect pointers. (Iterators should be unwrapped before using this.)
template <typename It>
constexpr bool iterator_is_contiguous = std::is_pointer<It>::value;

#endif // (VX_CPP_STANDARD >= 20)

template <typename It1, typename It2>
struct iterators_are_contiguous : bool_constant<iterator_is_contiguous<It1> && iterator_is_contiguous<It2>>
{};

template <typename It>
struct iterator_is_volatile : bool_constant<std::is_volatile<typename std::remove_reference<_traits_priv::iter_ref_t<It>>::type>::value>
{};

template <typename Src, typename Dst>
struct is_pointer_address_convertible : bool_constant<std::is_void<Src>::value || std::is_void<Dst>::value ||
                                            // NOTE: std::is_same is required for function pointers to work
                                            std::is_same<typename std::remove_cv<Src>::type, typename std::remove_cv<Dst>::type>::value

#if VX_HAVE_STD_IS_POINTER_INTERCONVERTIBLE
                                            || std::is_pointer_interconvertible_base_of<Dst, Src>::value
#endif // VX_HAVE_STD_IS_POINTER_INTERCONVERTIBLE

                                            >
{};

// --- trivial cat ---

namespace _traits_priv {

template <typename Src, typename Dst, typename SrcRef, typename DstRef>
struct trivial_cat
{
    using src_u = typename unwrap_enum<Src>::type;
    using dst_u = typename unwrap_enum<Dst>::type;

    static constexpr bool is_same_size_and_comparable =
        (sizeof(Src) == sizeof(Dst)) &&
        // If dst_u is bool, src_u also needs to be bool
        // Conversion from non-bool -> non-bool | bool -> bool | bool -> non-bool is fine.
        // Conversion from non-bool -> bool is not fine.
        std::is_same<bool, src_u>::value >= std::is_same<bool, dst_u>::value &&
        (std::is_same<src_u, dst_u>::value ||
            (std::is_integral<src_u>::value && std::is_integral<dst_u>::value) ||
            (std::is_floating_point<src_u>::value && std::is_floating_point<dst_u>::value));

    static constexpr bool is_bitcopy_constructible =
        is_same_size_and_comparable &&
        std::is_trivially_constructible<Dst, SrcRef>::value;

    static constexpr bool is_bitcopy_assignable =
        is_same_size_and_comparable &&
        std::is_trivially_assignable<DstRef, SrcRef>::value;
};

template <typename Src, typename Dst, typename SrcRef, typename DstRef>
struct trivial_cat<Src*, Dst*, SrcRef, DstRef>
{
    static constexpr bool is_bitcopy_constructible =
        is_pointer_address_convertible<Src, Dst>::value &&
        std::is_trivially_constructible<Dst*, SrcRef>::value;

    static constexpr bool is_bitcopy_assignable =
        is_pointer_address_convertible<Src, Dst>::value &&
        std::is_trivially_assignable<DstRef, SrcRef>::value;
};

struct false_trivial_cat
{
    static constexpr bool is_bitcopy_constructible = false;
    static constexpr bool is_bitcopy_assignable = false;
};

template <typename SrcIt, typename DstIt, bool are_contiguous = iterators_are_contiguous<SrcIt, DstIt>::value && !iterator_is_volatile<SrcIt>::value && !iterator_is_volatile<DstIt>::value>
struct iter_move_cat : trivial_cat<iter_value_t<SrcIt>, iter_value_t<DstIt>, remove_reference_t<iter_ref_t<SrcIt>>&&, iter_ref_t<DstIt>>
{};

template <typename SrcIt, typename DstIt>
struct iter_move_cat<SrcIt, DstIt, false> : false_trivial_cat
{};

template <typename SrcIt, typename DstIt>
struct iter_move_cat<std::move_iterator<SrcIt>, DstIt, false> : iter_move_cat<SrcIt, DstIt>
{};

template <typename SrcIt, typename DstIt, bool are_contiguous = iterators_are_contiguous<SrcIt, DstIt>::value && !iterator_is_volatile<SrcIt>::value && !iterator_is_volatile<DstIt>::value>
struct iter_copy_cat : trivial_cat<iter_value_t<SrcIt>, iter_value_t<DstIt>, iter_ref_t<SrcIt>, iter_ref_t<DstIt>>
{};

template <typename SrcIt, typename DstIt>
struct iter_copy_cat<SrcIt, DstIt, false> : false_trivial_cat
{};

template <typename SrcIt, typename DstIt>
struct iter_copy_cat<std::move_iterator<SrcIt>, DstIt, false> : iter_move_cat<SrcIt, DstIt>
{};

template <typename SrcIt, typename Sent, typename DstIt>
struct sent_copy_cat : std::conditional<
    std::is_same<Sent, SrcIt>::value,
    iter_copy_cat<SrcIt, DstIt>,
    false_trivial_cat
>::type
{};

} // namespace _traits_priv

// --- bitwise comparable ---

template <typename T1, typename T2, typename = void>
struct can_compare_with_operator_equal : std::false_type
{};

template <typename T1, typename T2>
struct can_compare_with_operator_equal<T1, T2, void_t<decltype(std::declval<T1&>() == std::declval<T2&>())>> : std::true_type
{};

template <typename T1, typename T2>
struct is_pointer_address_comparable : bool_constant<can_compare_with_operator_equal<T1, T2>::value && (is_pointer_address_convertible<T1, T2>::value || is_pointer_address_convertible<T2, T1>::value)>
{};

// Integral types are eligible for memcmp in very specific cases.
// * They must be the same size. (`int == long` is eligible; `int == long long` isn't.)
// * The usual arithmetic conversions must preserve bit patterns. (This is true for `int == unsigned int`,
//   but false for `short == unsigned short`.)
VX_DISABLE_MSVC_WARNING_PUSH()
VX_DISABLE_MSVC_WARNING(4806) // no value of type 'bool' promoted to type 'char' can equal the given constant
template <typename T1, typename T2, bool = ((sizeof(T1) == sizeof(T2)) && (std::is_integral<T1>::value && std::is_integral<T2>::value))>
struct is_bitwise_comparable : bool_constant<(std::is_same<T1, bool>::value || std::is_same<T2, bool>::value || (static_cast<T1>(-1) == static_cast<T2>(-1)))>
{};
VX_DISABLE_MSVC_WARNING_POP()

// Allow memcmping std::byte.
#if VX_HAVE_STD_BYTE
template <>
struct is_bitwise_comparable<std::byte, std::byte, false> : std::true_type
{};
#endif // VX_HAVE_STD_BYTE

// Pointer elements are eligible for memcmp when they point to the same type, ignoring cv-qualification.
// This handles pointers to object types, pointers to void, and pointers to function types.
template <typename T1, typename T2>
struct is_bitwise_comparable<T1*, T2*, false> : is_pointer_address_comparable<T1, T2>
{};

template <typename T1, typename T2>
struct is_bitwise_comparable<T1, T2, false> : std::false_type
{};

namespace _traits_priv {

// is_bitwise_comparable_with_pred<T1, T2, Pred> reports whether the memcmp optimization is applicable,
// given contiguously stored elements. (This avoids having to repeat the metaprogramming that finds the element types.)
// T1 and T2 aren't top-level const here.
template <typename T1, typename T2, typename Pred>
struct is_bitwise_comparable_with_pred : std::false_type
{};

// With std::equal_to<T3> we need to make sure that both T1 and T2 are convertible to T3 without changing
// object representation (we use iter_copy_cat for this task) and T3 can be safely memcmp'ed with itself
template <typename T1, typename T2, typename T3>
struct is_bitwise_comparable_with_pred<T1, T2, std::equal_to<T3>> : bool_constant<iter_copy_cat<T1*, T3*>::is_bitcopy_constructible && iter_copy_cat<T2*, T3*>::is_bitcopy_constructible && is_bitwise_comparable<remove_cv_t<T3>, remove_cv_t<T3>>::value>
{};

// equal_to<> is transparent.
template <typename T1, typename T2>
struct is_bitwise_comparable_with_pred<T1, T2, std::equal_to<>> : is_bitwise_comparable<T1, T2>
{};

#if (VX_CPP_STANDARD >= 20)
// std::ranges::equal_to is also transparent.
template <typename T1, typename T2>
struct is_bitwise_comparable_with_pred<T1, T2, std::ranges::equal_to> : is_bitwise_comparable<T1, T2>
{};
#endif // (VX_CPP_STANDARD >= 20)

// equal_memcmp_is_safe<It1, It2, Pred> reports whether we can activate the memcmp optimization
// for arbitrary iterators and predicates.
// It ignores top-level constness on the iterators and on the elements.
template <typename It1, typename It2, typename Pred>
struct equal_memcmp_is_safe_helper : bool_constant<iterators_are_contiguous<It1, It2>::value && !iterator_is_volatile<It1>::value && !iterator_is_volatile<It2>::value && is_bitwise_comparable_with_pred<iter_value_t<It1>, iter_value_t<It2>, Pred>::value>
{};

template <typename It1, typename It2, typename Pred>
struct equal_memcmp_is_safe : equal_memcmp_is_safe_helper<remove_const_t<It1>, remove_const_t<It2>, remove_const_t<Pred>>
{};

} // namespace _traits_priv

// --- memmove ---

template <typename It1, typename It2 = It1>
struct memmove_is_safe : bool_constant<_traits_priv::iter_copy_cat<It1, It2>::is_bitcopy_constructible>
{};

// --- memcmp lexicographical ---

// Reports whether memcmp's unsigned-byte ordering is equivalent to the
// value ordering of T1/T2.
//
// This is intentionally stricter than is_bitwise_comparable. A type may have
// a representation suitable for equality-by-memcmp without having a byte
// representation whose ordering matches operator<.
//
// In particular, multi-byte integral types are NOT generally eligible:
// their object representation is affected by endianness.
//
// The ordinary memcmp implementation is therefore restricted to types whose
// ordering is directly represented by an unsigned byte.
template <typename T1, typename T2, typename = void>
struct is_memcmp_lexicographically_safe : std::false_type
{};

// unsigned char has exactly the ordering required by memcmp.
template <>
struct is_memcmp_lexicographically_safe<unsigned char, unsigned char, void> : std::true_type
{};

#if VX_HAVE_STD_BYTE
// std::byte's ordering is its underlying unsigned-byte ordering.
template <>
struct is_memcmp_lexicographically_safe<std::byte, std::byte, void> : std::true_type
{};
#endif // VX_HAVE_STD_BYTE

namespace _traits_priv {

// is_memcmp_lexicographically_safe_with_pred<T1, T2, Pred> reports whether
// the memcmp optimization is applicable to contiguously stored elements for
// the specified ordering predicate.
template <typename T1, typename T2, typename Pred>
struct is_memcmp_lexicographically_safe_with_pred : std::false_type
{};

template <typename T1, typename T2>
struct is_memcmp_lexicographically_safe_with_pred<T1, T2, std::less<>>
    : is_memcmp_lexicographically_safe<typename std::remove_cv<T1>::type, typename std::remove_cv<T2>::type>
{};

template <typename T1, typename T2>
struct is_memcmp_lexicographically_safe_with_pred<T1, T2, std::greater<>>
    : is_memcmp_lexicographically_safe<typename std::remove_cv<T1>::type, typename std::remove_cv<T2>::type>
{};

#if (VX_CPP_STANDARD >= 20)

template <typename T1, typename T2>
struct is_memcmp_lexicographically_safe_with_pred<T1, T2, std::ranges::less>
    : is_memcmp_lexicographically_safe<typename std::remove_cv<T1>::type, typename std::remove_cv<T2>::type>
{};

template <typename T1, typename T2>
struct is_memcmp_lexicographically_safe_with_pred<T1, T2, std::ranges::greater>
    : is_memcmp_lexicographically_safe<typename std::remove_cv<T1>::type, typename std::remove_cv<T2>::type>
{};

#endif // (VX_CPP_STANDARD >= 20)

} // namespace _traits_priv

// memcmp_lexicographical_is_safe reports whether we can activate the memcmp
// optimization for arbitrary iterators and predicates.
//
// It ignores top-level constness on the iterators and on the elements.
template <typename It1, typename It2, typename Pred = std::less<>>
struct memcmp_lexicographical_is_safe
    : bool_constant<
          iterators_are_contiguous<It1, It2>::value &&
          !iterator_is_volatile<It1>::value &&
          !iterator_is_volatile<It2>::value &&
          _traits_priv::is_memcmp_lexicographically_safe_with_pred<
              _traits_priv::iter_value_t<It1>,
              _traits_priv::iter_value_t<It2>,
              typename std::remove_const<Pred>::type>::value>
{};

//==============================================================================
// 16. misc helper types
//==============================================================================

template <typename... Types>
struct composite_storage
{
    std::tuple<Types...> members;
};

// tag that selects a type to construct in place
template <typename>
struct in_place_type_t
{
    explicit in_place_type_t() = default;
};

struct callable_t
{
    void operator()()
    {}
};

template <typename T, typename... Args>
constexpr auto extract_first(T&& first, Args&&... args) noexcept
{
    return std::forward<T>(first);
}

//==============================================================================
// 17. is container-like
//==============================================================================

namespace _traits_priv {

template <typename T, typename = void>
struct is_container_like_impl : std::false_type
{};

template <typename T>
struct is_container_like_impl<T, void_t<
    decltype(std::begin(std::declval<T&>())),
    decltype(std::end(std::declval<T&>())),
    decltype(*std::begin(std::declval<T&>())),
    decltype(++std::declval<decltype(std::begin(std::declval<T&>()))&>())
>> : std::true_type
{};

} // namespace _traits_priv

template <typename T>
struct is_container_like
    : _traits_priv::is_container_like_impl<
          typename std::remove_cv<typename std::remove_reference<T>::type>::type>
{};

} // namespace traits
} // namespace vx

// clang-format on
