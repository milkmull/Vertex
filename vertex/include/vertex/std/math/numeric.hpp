#pragma once

#include "vertex/std/functional.hpp"
#include "vertex/std/iterator.hpp"

namespace vx {

//==============================================================================
// accumulate
//==============================================================================

template <typename IT, typename T, typename Fn>
constexpr T accumulate(const IT first, const IT last, T val, Fn reduce_op)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    auto ufirst = first;
    const auto ulast = last;

    for (; ufirst != ulast; ++ufirst)
    {
        val = reduce_op(std::move(val), *ufirst);
    }

    return val;
}

template <typename IT, typename T>
constexpr T accumulate(const IT first, const IT last, T val)
{
    return accumulate(first, last, val, plus<>{});
}

//==============================================================================
// partial sum
//==============================================================================

// compute partial noncommutative and nonassociative reductions into dst, using reduce_op
template <typename IT1, typename IT2, typename Fn>
constexpr IT2 partial_sum(const IT1 first, const IT1 last, IT2 dst, Fn reduce_op)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    auto ufirst = first;
    const auto ulast = last;

    if (ufirst != ulast)
    {
        auto val = *ufirst;

        for (;;)
        {
            *dst = val;
            ++dst;
            ++ufirst;
            if (ufirst == ulast)
            {
                break;
            }

            val = reduce_op(std::move(val), *ufirst);
        }
    }

    return dst;
}

// compute partial noncommutative and nonassociative reductions into dst
template <typename IT1, typename IT2>
constexpr IT2 partial_sum(IT1 first, IT1 last, IT2 dst)
{
    return partial_sum(first, last, dst, plus<>{});
}

//==============================================================================
// lower bound
//==============================================================================

// find first element not before val
template <typename IT, typename T, typename Fn>
constexpr IT lower_bound(const IT first, const IT last, const T& val, Fn pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    auto ufirst = first;
    const auto ulast = last;
    auto count = distance(ufirst, ulast);

    while (0 < count)
    {
        const auto count2 = count / 2;
        const auto mid = next(ufirst, count2);

        if (pred(*mid, val))
        {
            ufirst = next(mid);
            count -= count2 + 1;
        }
        else
        {
            count = count2;
        }
    }

    return ufirst;
}

// find first element not before val
template <typename IT, typename T>
constexpr IT lower_bound(const IT first, const IT last, const T& val)
{
    return lower_bound(first, last, val, less<>{});
}

//==============================================================================
// upper bound
//==============================================================================

// find first element that val is before
template <typename IT, typename T, typename Fn>
constexpr IT upper_bound(const IT first, const IT last, const T& val, Fn pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    auto ufirst = first;
    const auto ulast = last;
    auto count = distance(ufirst, ulast);

    while (0 < count)
    {
        const auto count2 = count / 2;
        const auto mid = next(ufirst, count2);

        if (pred(val, *mid))
        {
            count = count2;
        }
        else
        {
            ufirst = next(mid);
            count -= count2 + 1;
        }
    }

    return ufirst;
}

// find first element that val is before
template <typename IT, typename T>
constexpr IT upper_bound(const IT first, const IT last, const T& val)
{
    return upper_bound(first, last, val, less<>{});
}

} // namespace vx
