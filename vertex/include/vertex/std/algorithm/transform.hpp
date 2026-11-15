#pragma once

#include "vertex/std/_simd/simd_algorithms.hpp"
#include "vertex/std/algorithm/functional.hpp"
#include "vertex/std/iterator/iterator.hpp"
#include "vertex/std/memory/_priv/memory_core.hpp"
#include "vertex/std/type_traits.hpp"
#include "vertex/std/util.hpp"

namespace vx {
namespace range {

//=========================================================================
// for_each / transform / generate
//=========================================================================

// Calls fn on every element. Returns fn so stateful functors can be read back.
template <typename T, typename Fn>
constexpr Fn for_each(T* ptr, size_t num, Fn fn)
{
    for (T* const last = ptr + num; ptr != last; ++ptr)
    {
        fn(*ptr);
    }

    return fn;
}

template <typename IT, typename Fn, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr Fn for_each(IT first, IT last, Fn fn)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first)
    {
        fn(*first);
    }

    return fn;
}

// Iterator + count. Returns first advanced by count (as std::for_each_n does).
template <typename IT, typename Fn, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT for_each_n(IT first, size_t num, Fn fn)
{
    for (; 0 < num; --num, ++first)
    {
        fn(*first);
    }

    return first;
}

// dst[i] = fn(src[i]). dst may equal src (in-place), but must not partially
// overlap it. Returns dst + num.
template <typename T, typename U, typename Fn>
constexpr T* transform(T* dst, const U* src, size_t num, Fn fn)
{
    for (size_t i = 0; i < num; ++i)
    {
        dst[i] = fn(src[i]);
    }

    return dst + num;
}

// dst[i] = fn(a[i], b[i])
template <typename T, typename U, typename V, typename Fn>
constexpr T* transform(T* dst, const U* a, const V* b, size_t num, Fn fn)
{
    for (size_t i = 0; i < num; ++i)
    {
        dst[i] = fn(a[i], b[i]);
    }

    return dst + num;
}

// Returns dst advanced by the number of elements written.
template <typename IT1, typename IT2, typename Fn, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 transform(IT1 dst, IT2 first, IT2 last, Fn fn)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first, ++dst)
    {
        *dst = fn(*first);
    }

    return dst;
}

template <typename IT1, typename IT2, typename IT3, typename Fn, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value && traits::is_iterator<IT3>::value))>
constexpr IT1 transform(IT1 dst, IT2 first1, IT2 last1, IT3 first2, Fn fn)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);

    for (; first1 != last1; ++first1, ++first2, ++dst)
    {
        *dst = fn(*first1, *first2);
    }

    return dst;
}

// ptr[i] = fn(), called in order. Returns ptr + num.
template <typename T, typename Fn>
constexpr T* generate(T* ptr, size_t num, Fn fn)
{
    for (size_t i = 0; i < num; ++i)
    {
        ptr[i] = fn();
    }

    return ptr + num;
}

template <typename IT, typename Fn, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT generate(IT first, IT last, Fn fn)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first)
    {
        *first = fn();
    }

    return last;
}

// Iterator + count. Returns first advanced by num.
template <typename IT, typename Fn, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT generate_n(IT first, size_t num, Fn fn)
{
    for (; 0 < num; --num, ++first)
    {
        *first = fn();
    }

    return first;
}

} // namespace range
} // namespace vx
