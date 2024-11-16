#pragma once

#include "vertex/system/assert.hpp"

namespace vx {
namespace rand {

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

        param_type() : param_type(0) {}

        explicit param_type(T min_value, T max_value = std::numeric_limits<T>::max())
            : m_a(min_value), m_b(max_value)
        {
            assert(m_a <= m_b);
        }

        result_type a() const
        {
            return m_a;
        }

        result_type b() const
        {
            return m_b;
        }

        friend bool operator==(const param_type& lhs, const param_type& rhs)
        {
            return lhs.m_a == rhs.m_a && lhs.m_b == rhs.m_b;
        }

        friend bool operator!=(const param_type& lhs, const param_type& rhs)
        {
            return !(lhs == rhs);
        }

    private:

        result_type m_a;
        result_type m_b;
    };

public:

    uniform_int_distribution() : uniform_int_distribution(0) {}

    explicit uniform_int_distribution(T min_value, T max_value = std::numeric_limits<T>::max())
        : m_param(min_value, max_value) {}

    explicit uniform_int_distribution(const param_type& p) : m_param(p) {}

    result_type a() const { return m_param.a(); }
    result_type b() const { return m_param.b(); }

    param_type param() const
    {
        return m_param;
    }

    void param(const param_type& p)
    {
        m_param = p;
    }

    result_type min() const { return a(); }
    result_type max() const { return b(); }

    void reset() {}

    friend bool operator==(
        const uniform_int_distribution& lhs,
        const uniform_int_distribution& rhs
    )
    {
        return lhs.m_param == rhs.m_param;
    }

    friend bool operator!=(
        const uniform_int_distribution& lhs,
        const uniform_int_distribution& rhs
    )
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

    // upscaling
    else if (urng_range < urange)
    {
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

        const common_type uerng_range = urng_range + 1;

        common_type tmp = 0; // wraparound control
        do
        {
            tmp = uerng_range * operator()(rng, param_type(0, urange / uerng_range));
            ret = tmp + (static_cast<common_type>(rng()) - urng_min);

        } while (ret > urange || ret < tmp);
    }

    // same range
    else
    {
        ret = common_type(rng()) - urng_min;
    }

    return ret + p.a();
}

} // namespace rand
} // namespace vx