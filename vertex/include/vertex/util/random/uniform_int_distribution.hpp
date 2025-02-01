#pragma once

#include "vertex/system/assert.hpp"

namespace vx {
namespace random {

// https://github.com/gcc-mirror/gcc/blob/e97179bacd067ccd3ee765632e0c034df152ccb6/libstdc%2B%2B-v3/include/bits/uniform_int_dist.h

template <typename T = int>
class uniform_int_distribution
{
    static_assert(std::is_integral<T>::value, "T must be an integral type");

public:

    using result_type = T;

    struct param_type
    {
        using distribution_type = uniform_int_distribution<T>;

        param_type() noexcept : param_type(0) {}

        explicit param_type(T min_value, T max_value = std::numeric_limits<T>::max()) noexcept
            : m_a(min_value), m_b(max_value)
        {
            VX_ASSERT(m_a <= m_b);
        }

        result_type a() const noexcept
        {
            return m_a;
        }

        result_type b() const noexcept
        {
            return m_b;
        }

        friend bool operator==(const param_type& lhs, const param_type& rhs) noexcept
        {
            return lhs.m_a == rhs.m_a && lhs.m_b == rhs.m_b;
        }

        friend bool operator!=(const param_type& lhs, const param_type& rhs) noexcept
        {
            return !(lhs == rhs);
        }

    private:

        result_type m_a;
        result_type m_b;
    };

public:

    uniform_int_distribution() noexcept : uniform_int_distribution(0) {}

    explicit uniform_int_distribution(T min_value, T max_value = std::numeric_limits<T>::max()) noexcept
        : m_param(min_value, max_value) {}

    explicit uniform_int_distribution(const param_type& p) noexcept : m_param(p) {}

    result_type a() const noexcept { return m_param.a(); }
    result_type b() const noexcept { return m_param.b(); }

    param_type param() const noexcept
    {
        return m_param;
    }

    void param(const param_type& p) noexcept
    {
        m_param = p;
    }

    result_type min() const noexcept { return a(); }
    result_type max() const noexcept { return b(); }

    auto range() const noexcept
    {
        using uresult_type = typename std::make_unsigned<result_type>::type;
        const uresult_type range_min = static_cast<uresult_type>(a()) + std::numeric_limits<result_type>::max() + 1;
        const uresult_type range_max = static_cast<uresult_type>(b()) + std::numeric_limits<result_type>::max() + 1;
        return (range_max - range_min);
    }

    void reset() noexcept {}

    friend bool operator==(
        const uniform_int_distribution& lhs,
        const uniform_int_distribution& rhs
    ) noexcept
    {
        return lhs.m_param == rhs.m_param;
    }

    friend bool operator!=(
        const uniform_int_distribution& lhs,
        const uniform_int_distribution& rhs
    ) noexcept
    {
        return !(lhs == rhs);
    }

    template <typename RNG>
    result_type operator()(RNG& rng)
    {
        return operator()(rng, m_param);
    }

    template <typename RNG>
    result_type operator()(RNG& rng, const param_type& p);

private:

    param_type m_param;
};

namespace __detail {

template <bool upscale>
struct upscaler;

template <>
struct upscaler<false> // rng range is greater than or equal to dist max range
{
    template <typename Dist, typename RNG, typename common_type>
    static VX_FORCE_INLINE common_type upscale(Dist& dist, RNG& rng, const common_type urange) noexcept
    {
        return 0;
    }
};

template <>
struct upscaler<true> // rng range is smaller than dist max range
{
    template <typename Dist, typename RNG, typename common_type>
    static inline common_type upscale(Dist& dist, RNG& rng, const common_type urange)
    {
        using param_type = typename Dist::param_type;

        constexpr common_type urng_min = static_cast<common_type>((RNG::min)());
        constexpr common_type urng_max = static_cast<common_type>((RNG::max)());
        constexpr common_type urng_range = urng_max - urng_min;

        // Note that every value in [0, urange]
        // can be written uniquely as
        // 
        // (urngrange + 1) * high + low
        // 
        // where
        // 
        // high in [0, urange / (urngrange + 1)]
        // 
        // and
        // 
        // low in [0, urngrange].
    
        // The while loop ensures uniformity by rejecting values outside the target range [0, urange].
        // Upscaling may produce values that exceed urange (overshoot) or cause wraparound 
        // if the sum overflows the type’s maximum. The condition 'ret > urange' handles overshoot, 
        // discarding any values that exceed the target range, while 'ret < tmp' checks for wraparound, 
        // where overflow would make ret unexpectedly smaller than tmp. By rejecting these out-of-bounds 
        // or wrapped values, the loop preserves an unbiased distribution across [0, urange].

        constexpr common_type uerng_range = urng_range + 1;
        
        common_type ret = 0;
        common_type tmp = 0; // wraparound control
        do
        {
            tmp = uerng_range * dist(rng, param_type(0, urange / uerng_range));
            ret = tmp + (static_cast<common_type>(rng()) - urng_min);
    
        } while (ret > urange || ret < tmp);

        return ret;
    }
};

} // namespace __detail

template <typename T>
template <typename RNG>
typename uniform_int_distribution<T>::result_type uniform_int_distribution<T>::operator()(RNG& rng, const param_type& p)
{
    using range_type = T;
    using urange_type = typename std::make_unsigned<T>::type;
    using rng_type = typename RNG::result_type;

    static_assert(
        std::is_integral<rng_type>::value && std::is_unsigned<rng_type>::value,
        "RNG::result_type must be an unsigned integral type"
    );

    using common_type = typename std::common_type<rng_type, urange_type>::type;

    constexpr common_type urng_min = static_cast<common_type>((RNG::min)());
    constexpr common_type urng_max = static_cast<common_type>((RNG::max)());
    static_assert(urng_min < urng_max, "RNG must define min() < max()");
    constexpr common_type urng_range = urng_max - urng_min;

    // The full range of the distribution type is just
    // the max value of the unsigned version of the type
    constexpr common_type udist_range = static_cast<common_type>(std::numeric_limits<urange_type>::max());
    constexpr bool possible_upscale = (urng_range < udist_range);

    const common_type urange = static_cast<common_type>(p.b()) - static_cast<common_type>(p.a());
    common_type ret = 0;

    // downscaling
    if (urng_range > urange)
    {
        // urange is the maximum desired range value (exclusive upper bound),
        // so we add 1 to make it inclusive (to allow the range from 0 to urange).
        // There is no danger of overflow here because we already know urng_range
        // is wider than urange, meaning there is room to spare given the type.
        const common_type uerange = urange + 1;

        // Calculate the scaling factor to map the RNG's full range
        // to the desired range [0, urange]. This will help map
        // the RNG's output values uniformly to our target range.
        const common_type scaling = urng_range / uerange;

        // Calculate the last acceptable value in the RNG's range
        // that will map properly into [0, urange]. Values above this
        // threshold will be rejected to maintain uniform distribution.
        const common_type end = uerange * scaling;

        // Use rejection sampling
        do
        {
            // Generate a random number within the RNG's range and shift it down
            // by urng_min to normalize it into the range [0, urng_range].
            ret = static_cast<common_type>(rng()) - urng_min;

            // If the generated value is greater than or equal to end, discard it and
            // try again. This keeps values within the mapped target range.
        } while (ret >= end);

        // Scale down the return value to fit within [0, urange].
        // This gives us a value that uniformly spans the desired range.
        ret /= scaling;
    }

    // same range
    else if (urng_range == urange)
    {
        ret = common_type(rng()) - urng_min;
    }

    // upscaling
    else // (urng_range < urange)
    {
        // Upscaling is only required if the range of the RNG (e.g., uint32_t) is smaller 
        // than the range of the distribution (e.g., uint64_t). In such cases, the RNG 
        // cannot directly generate values across the entire distribution range. 
        // To handle this, we use template specialization to implement upscaling only 
        // when this condition is met. If the RNG's range is large enough to cover 
        // the distribution, this branch is effectively a no-op and never executed.

        VX_ASSERT(possible_upscale);
        ret = __detail::upscaler<possible_upscale>::upscale(*this, rng, urange);
    }

    return static_cast<range_type>(ret + p.a());
}

} // namespace random
} // namespace vx