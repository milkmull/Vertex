#pragma once

#include "vertex/config/feature_detection.hpp"

#include <string>

#if VX_HAVE_STD_STRING_VIEW
    #include <string_view>
#endif

#include "vertex/config/type_traits.hpp"

namespace vx {
namespace str {

namespace _string_traits_priv {

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr const C* empty_string() noexcept
{
    static constexpr C s[1] = { C() };
    return s;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_empty_string(const C* ptr) noexcept
{
    return ptr == empty_string<C>();
}

} // namespace _string_traits_priv

//=========================================================================

template <typename T>
class basic_string_view;

template <typename T>
class basic_cstring_view;

template <typename T, typename Allocator, typename Growth>
class basic_string;

template <size_t N, typename T>
class basic_static_string;

//=========================================================================

namespace _string_traits_priv {

template <typename>
struct is_string_like_impl : std::false_type
{};

template <typename T>
struct is_string_like_impl<basic_string_view<T>> : std::true_type
{};

template <typename T>
struct is_string_like_impl<basic_cstring_view<T>> : std::true_type
{};

template <typename T, typename Allocator, typename Growth>
struct is_string_like_impl<basic_string<T, Allocator, Growth>> : std::true_type
{};

template <size_t N, typename T>
struct is_string_like_impl<basic_static_string<N, T>> : std::true_type
{};

#if VX_HAVE_STD_STRING_VIEW

template <typename T, typename Traits>
struct is_string_like_impl<std::basic_string_view<T, Traits>> : std::true_type
{};

#endif // VX_HAVE_STD_STRING_VIEW

template <typename T, typename Traits, typename Alloc>
struct is_string_like_impl<std::basic_string<T, Traits, Alloc>> : std::true_type
{};

} // namespace _string_traits_priv

template <typename T>
struct is_string_like : _string_traits_priv::is_string_like_impl<VX_STRIP_T(T)>
{};

//=========================================================================

namespace _string_traits_priv {

template <typename>
struct is_string_view_impl : std::false_type
{};

template <typename T>
struct is_string_view_impl<basic_string_view<T>> : std::true_type
{};

template <typename T>
struct is_string_view_impl<basic_cstring_view<T>> : std::true_type
{};

#if VX_HAVE_STD_STRING_VIEW

template <typename T, typename Traits>
struct is_string_view_impl<std::basic_string_view<T, Traits>> : std::true_type
{};

#endif // VX_HAVE_STD_STRING_VIEW

} // namespace _string_traits_priv

template <typename T>
struct is_string_view : _string_traits_priv::is_string_view_impl<VX_STRIP_T(T)>
{};

//=========================================================================

namespace _string_traits_priv {

template <typename>
struct is_null_terminated_string_like_impl : std::false_type
{};

template <typename T>
struct is_null_terminated_string_like_impl<basic_cstring_view<T>> : std::true_type
{};

template <typename T, typename Allocator, typename Growth>
struct is_null_terminated_string_like_impl<basic_string<T, Allocator, Growth>> : std::true_type
{};

template <size_t N, typename T>
struct is_null_terminated_string_like_impl<basic_static_string<N, T>> : std::true_type
{};

template <typename T, typename Traits, typename Alloc>
struct is_null_terminated_string_like_impl<std::basic_string<T, Traits, Alloc>> : std::true_type
{};

} // namespace _string_traits_priv

template <typename T>
struct is_null_terminated_string_like : _string_traits_priv::is_null_terminated_string_like_impl<VX_STRIP_T(T)>
{};

//=========================================================================

namespace _string_traits_priv {

template <typename>
struct is_mutable_string_like_impl : std::false_type
{};

template <typename T, typename Allocator, typename Growth>
struct is_mutable_string_like_impl<basic_string<T, Allocator, Growth>> : std::true_type
{};

template <size_t N, typename T>
struct is_mutable_string_like_impl<basic_static_string<N, T>> : std::true_type
{};

template <typename T, typename Traits, typename Alloc>
struct is_mutable_string_like_impl<std::basic_string<T, Traits, Alloc>> : std::true_type
{};

} // namespace _string_traits_priv

template <typename T>
struct is_mutable_string_like : _string_traits_priv::is_mutable_string_like_impl<VX_STRIP_T(T)>
{};

//=========================================================================

namespace _string_traits_priv {

template <typename, typename, typename = void>
struct is_string_of_impl : std::false_type
{};

template <typename S, typename T>
struct is_string_of_impl<S, T, type_traits::void_t<typename S::value_type>> : std::bool_constant<is_string_like<S>::value && std::is_same<typename S::value_type, T>::value>
{};

} // namespace _string_traits_priv

template <typename S, typename T>
struct is_string_of : _string_traits_priv::is_string_of_impl<VX_STRIP_T(S), T>
{};

//=========================================================================

namespace _string_traits_priv {

template <typename, typename, typename = void>
struct is_string_compatible_impl : std::false_type
{};

template <typename S1, typename S2>
struct is_string_compatible_impl<S1, S2, type_traits::void_t<typename S1::value_type, typename S2::value_type>> : std::bool_constant<is_string_like<S1>::value && is_string_like<S2>::value && std::is_same<typename S1::value_type, typename S2::value_type>::value>
{};

} // namespace _string_traits_priv

template <typename S1, typename S2>
struct is_string_compatible : _string_traits_priv::is_string_compatible_impl<VX_STRIP_T(S1), VX_STRIP_T(S2)>
{};

//=========================================================================

namespace _string_traits_priv {

template <typename, typename, typename = void>
struct is_string_compatible_operator_impl : std::false_type
{};

template <typename S1, typename S2>
struct is_string_compatible_operator_impl<S1, S2, type_traits::void_t<typename S1::value_type, typename S2::value_type>> : std::bool_constant<is_string_like<S1>::value && is_string_like<S2>::value && std::is_same<typename S1::value_type, typename S2::value_type>::value && !std::is_same<VX_STRIP_T(S1), VX_STRIP_T(S2)>::value>
{};

template <typename S1, typename S2>
struct is_string_compatible_operator : _string_traits_priv::is_string_compatible_operator_impl<VX_STRIP_T(S1), VX_STRIP_T(S2)>
{};

} // namespace _string_traits_priv

//=========================================================================

template <typename S1, typename S2, VX_REQUIRES((_string_traits_priv::is_string_compatible_operator<S1, S2>::value))>
S1 operator+(const S1& lhs, const S2& rhs) noexcept(noexcept(S1().append(rhs.c_str())))
{
    return S1(lhs).append(rhs.c_str());
}

template <typename S1, typename S2, VX_REQUIRES((_string_traits_priv::is_string_compatible_operator<S1, S2>::value))>
bool operator==(const S1& lhs, const S2& rhs) noexcept(noexcept(S1().compare(rhs.c_str())))
{
    return lhs.compare(rhs.c_str()) == 0;
}

template <typename S1, typename S2, VX_REQUIRES((_string_traits_priv::is_string_compatible_operator<S1, S2>::value))>
bool operator!=(const S1& lhs, const S2& rhs) noexcept(noexcept(S1().compare(rhs.c_str())))
{
    return lhs.compare(rhs.c_str()) != 0;
}

template <typename S1, typename S2, VX_REQUIRES((_string_traits_priv::is_string_compatible_operator<S1, S2>::value))>
bool operator<(const S1& lhs, const S2& rhs) noexcept(noexcept(S1().compare(rhs.c_str())))
{
    return lhs.compare(rhs.c_str()) < 0;
}

template <typename S1, typename S2, VX_REQUIRES((_string_traits_priv::is_string_compatible_operator<S1, S2>::value))>
bool operator>(const S1& lhs, const S2& rhs) noexcept(noexcept(S1().compare(rhs.c_str())))
{
    return lhs.compare(rhs.c_str()) > 0;
}

template <typename S1, typename S2, VX_REQUIRES((_string_traits_priv::is_string_compatible_operator<S1, S2>::value))>
bool operator<=(const S1& lhs, const S2& rhs) noexcept(noexcept(S1().compare(rhs.c_str())))
{
    return lhs.compare(rhs.c_str()) <= 0;
}

template <typename S1, typename S2, VX_REQUIRES((_string_traits_priv::is_string_compatible_operator<S1, S2>::value))>
bool operator>=(const S1& lhs, const S2& rhs) noexcept(noexcept(S1().compare(rhs.c_str())))
{
    return lhs.compare(rhs.c_str()) >= 0;
}

} // namespace str
} // namespace vx
