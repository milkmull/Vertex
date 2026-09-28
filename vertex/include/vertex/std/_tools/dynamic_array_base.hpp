#pragma once

#include "vertex/std/memory.hpp"
#include "vertex/std/iterator.hpp"

namespace vx {
namespace _dynamic_array_base_priv {

//=========================================================================
// data type
//=========================================================================

template <typename T>
struct dynamic_array_data
{
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    pointer ptr;
    size_type size;
    size_type capacity;

    dynamic_array_data() noexcept
        : ptr(), size(), capacity()
    {}

    dynamic_array_data release() noexcept
    {
        dynamic_array_data old = std::move(*this);

        ptr = nullptr;
        size = 0;
        capacity = 0;

        return old;
    }

    void acquire(dynamic_array_data& other) noexcept
    {
        *this = other.release();
    }
};

//=========================================================================
// iterator helpers
//=========================================================================

//==============================================================================

template <typename P1, typename P2, typename P3, typename P4>
constexpr bool contig_range_overlaps(P1 first, P2 last, P3 begin, P4 end) noexcept
{
    // Empty range can't alias anything.
    return first != last && first < end && last > begin;
}

template <typename P1, typename P2, typename P3, typename P4>
constexpr bool contig_range_contained(P1 first, P2 last, P3 begin, P4 end) noexcept
{
    return first >= begin && first <= end && last >= begin && last <= end && first <= last;
}

template <bool include_end, typename P1, typename P2, typename P3>
constexpr bool contig_position_valid(P1 pos, P2 begin, P3 end) noexcept
{
    return pos >= begin && (include_end ? pos <= end : pos < end);
}

template <typename P1, typename P2, typename P3>
constexpr bool contig_position_insertable(P1 pos, P2 begin, P3 end) noexcept
{
    return contig_position_valid<true>(pos, begin, end);
}

template <typename P1, typename P2, typename P3>
constexpr bool contig_position_erasable(P1 pos, P2 begin, P3 end) noexcept
{
    return contig_position_valid<false>(pos, begin, end);
}

template <typename P1, typename P2, typename P3>
constexpr bool contig_ptr_outside_range(P1 addr, P2 begin, P3 end) noexcept
{
    return addr < begin || addr >= end;
}

// Used by insert/assign: true if [first,last) could alias *this.
template <typename C, typename IT1, typename IT2>
constexpr bool assert_contig_self_range(const C& container, const IT1& first, const IT2& last)
{
    using T = typename C::value_type;

    VX_IF_CONSTEXPR (
        type_traits::is_pointer_to<IT1, T>::value &&
        type_traits::is_pointer_to<IT2, T>::value)
    {
        const auto begin = container.cbegin().ptr();
        const auto end = container.cend().ptr();
        return contig_range_overlaps(first, last, begin, end);
    }
    else VX_IF_CONSTEXPR (
        _priv::is_my_pointer_iterator<IT1, C>::value &&
        _priv::is_my_pointer_iterator<IT2, C>::value)
    {
        const auto begin = container.cbegin().ptr();
        const auto end = container.cend().ptr();
        return contig_range_overlaps(first.ptr(), last.ptr(), begin, end);
    }
    else
    {
        return false;
    }
}

// Used by erase: true if [first,last) is a valid sub-range of *this.
template <typename C, typename IT1, typename IT2>
constexpr bool assert_contig_contained_range(const C& container, const IT1& first, const IT2& last)
{
    using T = typename C::value_type;

    VX_IF_CONSTEXPR (
        type_traits::is_pointer_to<IT1, T>::value &&
        type_traits::is_pointer_to<IT2, T>::value)
    {
        const auto begin = container.cbegin().ptr();
        const auto end = container.cend().ptr();
        return contig_range_contained(first, last, begin, end);
    }
    else VX_IF_CONSTEXPR (
        _priv::is_my_pointer_iterator<IT1, C>::value &&
        _priv::is_my_pointer_iterator<IT2, C>::value)
    {
        const auto begin = container.cbegin().ptr();
        const auto end = container.cend().ptr();
        return contig_range_contained(first.ptr(), last.ptr(), begin, end);
    }
    else
    {
        return true;
    }
}

template <bool include_end, typename C, typename IT>
constexpr bool assert_contig_valid_position(const C& container, const IT& pos)
{
    using T = typename C::value_type;

    VX_IF_CONSTEXPR (type_traits::is_pointer_to<IT, T>::value)
    {
        const auto begin = container.cbegin().ptr();
        const auto end = container.cend().ptr();
        return contig_position_valid<include_end>(pos, begin, end);
    }
    else VX_IF_CONSTEXPR (_priv::is_my_pointer_iterator<IT, C>::value)
    {
        const auto begin = container.cbegin().ptr();
        const auto end = container.cend().ptr();
        return contig_position_valid<include_end>(pos.ptr(), begin, end);
    }
    else
    {
        return true;
    }
}

template <typename C, typename T>
constexpr bool assert_not_aliasing_element(const C& container, const T& value) noexcept
{
    VX_IF_CONSTEXPR (!std::is_same<typename std::decay<T>::type, typename C::value_type>::value)
    {
        return true;
    }
    else
    {
        const T* addr = std::addressof(value);
        const T* begin = container.data();
        const T* end = begin + container.size();
        return contig_ptr_outside_range(addr, begin, end);
    }
}

template <typename C, typename... Args>
constexpr bool assert_not_aliasing_element_pack(const C&, const Args&...) noexcept
{
    return true;
}

template <typename C, typename Arg>
constexpr bool assert_not_aliasing_element_pack(const C& container, const Arg& value) noexcept
{
    return assert_not_aliasing_element(container, value);
}

//=========================================================================

#define VX_PRIV_ASSERT_CONTIG_SELF_RANGE(first, last) \
    VX_ASSERT(::vx::_dynamic_array_base_priv::assert_contig_self_range(*this, (first), (last)))

#define VX_PRIV_ASSERT_CONTIG_NOT_SELF_RANGE(first, last) \
    VX_ASSERT(!::vx::_dynamic_array_base_priv::assert_contig_self_range(*this, (first), (last)))

#define VX_PRIV_ASSERT_CONTIG_CONTAINED_RANGE(first, last) \
    VX_ASSERT(::vx::_dynamic_array_base_priv::assert_contig_contained_range(*this, (first), (last)))

#define VX_PRIV_ASSERT_CONTIG_INSERTABLE_POSITION(pos) \
    VX_ASSERT(::vx::_dynamic_array_base_priv::assert_contig_valid_position<true>(*this, (pos)))

#define VX_PRIV_ASSERT_CONTIG_ERASABLE_POSITION(pos) \
    VX_ASSERT(::vx::_dynamic_array_base_priv::assert_contig_valid_position<false>(*this, (pos)))

#define VX_PRIV_ASSERT_CONTIG_VALID_POSITION(pos) \
    VX_ASSERT(::vx::_dynamic_array_base_priv::assert_contig_valid_position<false>(*this, (pos)))

#define VX_PRIV_ASSERT_CONTIG_INVALID_POSITION(pos) \
    VX_ASSERT(!::vx::_dynamic_array_base_priv::assert_contig_valid_position<false>(*this, (pos)))

#define VX_PRIV_ASSERT_NOT_ALIASING_ELEMENT(value) \
    VX_ASSERT(::vx::_dynamic_array_base_priv::assert_not_aliasing_element(*this, (value)))

#define VX_PRIV_ASSERT_NOT_ALIASING_ELEMENT_PACK(...) \
    VX_ASSERT(::vx::_dynamic_array_base_priv::assert_not_aliasing_element_pack(*this, ##__VA_ARGS__))

} // namespace _dynamic_array_base_priv
} // namespace vx
