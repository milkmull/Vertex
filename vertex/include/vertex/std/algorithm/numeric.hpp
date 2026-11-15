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
// 10. numeric
//=========================================================================

template <typename IT, typename T, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr T accumulate(IT first, IT last, T init)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first)
    {
        init = _range_priv::as_rvalue(init) + *first;
    }

    return init;
}

template <typename IT, typename T, typename Op, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr T accumulate(IT first, IT last, T init, Op op)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first)
    {
        init = op(_range_priv::as_rvalue(init), *first);
    }

    return init;
}

// first = value, value + 1, value + 2, ...
template <typename IT, typename T, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr void iota(IT first, IT last, T value)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first, ++value)
    {
        *first = value;
    }
}

template <typename IT1, typename IT2, typename T, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr T inner_product(IT1 first1, IT1 last1, IT2 first2, T init)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);

    for (; first1 != last1; ++first1, ++first2)
    {
        init = _range_priv::as_rvalue(init) + *first1 * *first2;
    }

    return init;
}

template <typename IT1, typename IT2, typename T, typename Op1, typename Op2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr T inner_product(IT1 first1, IT1 last1, IT2 first2, T init, Op1 op1, Op2 op2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);

    for (; first1 != last1; ++first1, ++first2)
    {
        init = op1(_range_priv::as_rvalue(init), op2(*first1, *first2));
    }

    return init;
}

// dst[i] = src[0] + ... + src[i]. dst may equal first (in place).
template <typename OUT, typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr OUT partial_sum(IT first, IT last, OUT dst)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    if (first == last) // needed: *first is read
    {
        return dst;
    }

    typename traits::value_type<IT>::type sum = *first;
    *dst = sum;

    for (++first, ++dst; first != last; ++first, ++dst)
    {
        sum = _range_priv::as_rvalue(sum) + *first;
        *dst = sum;
    }

    return dst;
}

template <typename OUT, typename IT, typename Op, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr OUT partial_sum(IT first, IT last, OUT dst, Op op)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    if (first == last) // needed: *first is read
    {
        return dst;
    }

    typename traits::value_type<IT>::type sum = *first;
    *dst = sum;

    for (++first, ++dst; first != last; ++first, ++dst)
    {
        sum = op(_range_priv::as_rvalue(sum), *first);
        *dst = sum;
    }

    return dst;
}

} // namespace range
} // namespace vx
