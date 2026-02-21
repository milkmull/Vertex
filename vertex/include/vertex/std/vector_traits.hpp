#pragma once

#include <vector>

#include "vertex/config/type_traits.hpp"
#include "vertex/std/memory.hpp"

namespace vx {

//=========================================================================

template <typename T, typename Allocator>
class vector;

template <size_t N, typename T>
class static_vector;

//=========================================================================

template <typename T>
struct is_vector_like : std::false_type
{};

template <typename T, typename Allocator>
struct is_vector_like<vector<T, Allocator>> : std::true_type
{};

template <size_t N, typename T>
struct is_vector_like<static_vector<N, T>> : std::true_type
{};

template <typename T, typename Alloc>
struct is_vector_like<std::vector<T, Alloc>> : std::true_type
{};

//=========================================================================

template <typename S, typename T, typename = void>
struct is_vector_of : std::false_type
{};

template <typename S, typename T>
struct is_vector_of<S, T, type_traits::void_t<typename S::value_type>> : std::bool_constant<is_vector_like<S>::value && std::is_same<typename S::value_type, T>::value>
{};

//=========================================================================

template <typename V1, typename V2, typename = void>
struct is_vector_compatible : std::false_type
{};

template <typename V1, typename V2>
struct is_vector_compatible<V1, V2, type_traits::void_t<typename V1::value_type, typename V2::value_type>> : std::bool_constant<is_vector_like<V1>::value && is_vector_like<V2>::value && std::is_same<typename V1::value_type, typename V2::value_type>::value>
{};

//=========================================================================

template <typename V1, typename V2, VX_REQUIRES((is_vector_compatible<V1, V2>::value))>
bool operator==(const V1& lhs, const V2& rhs)
{
    return mem::compare(lhs.data(), lhs.size(), rhs.data(), rhs.size()) == 0;
}

template <typename V1, typename V2, VX_REQUIRES((is_vector_compatible<V1, V2>::value))>
bool operator!=(const V1& lhs, const V2& rhs)
{
    return !(lhs == rhs);
}

template <typename V1, typename V2, VX_REQUIRES((is_vector_compatible<V1, V2>::value))>
bool operator<(const V1& lhs, const V2& rhs)
{
    return mem::compare(lhs.data(), lhs.size(), rhs.data(), rhs.size()) < 0;
}

template <typename V1, typename V2, VX_REQUIRES((is_vector_compatible<V1, V2>::value))>
bool operator>(const V1& lhs, const V2& rhs)
{
    return (rhs < lhs);
}

template <typename V1, typename V2, VX_REQUIRES((is_vector_compatible<V1, V2>::value))>
bool operator<=(const V1& lhs, const V2& rhs)
{
    return !(rhs < lhs);
}

template <typename V1, typename V2, VX_REQUIRES((is_vector_compatible<V1, V2>::value))>
bool operator>=(const V1& lhs, const V2& rhs)
{
    return !(lhs < rhs);
}

} // namespace vx
