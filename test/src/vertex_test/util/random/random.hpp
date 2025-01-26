#pragma once

#include "vertex/util/memory/memory.hpp"

namespace vx {
namespace test {

// https://www.scribbr.com/statistics/chi-square-distribution-table/

// Chi-squared critical value for confidence level 0.05 with k-1 degrees of freedom
// The degrees of freedom depend on the range size (k = range_size)
constexpr double chi_squared_critical_values[] = {
    3.841, 5.991, 7.815, 9.488, 11.070, 12.592, 14.067, 15.507, 16.919, 18.307,
    19.675, 21.026, 22.362, 23.685, 24.996, 26.296, 27.587, 28.869, 30.144, 31.410
};

template <typename RNG, typename Dist, size_t BINS, size_t SAMPLES = 100000>
inline bool test_uniform_int_distribution(RNG& rng, Dist& dist)
{
    static_assert(BINS <= mem::array_size(chi_squared_critical_values));
    constexpr double threshold = chi_squared_critical_values[BINS - 1];

    using result_type = typename Dist::result_type;
    static_assert(std::is_integral<result_type>::value);
    using uresult_type = typename std::make_unsigned<result_type>::type;

    const uresult_type range_min = static_cast<uresult_type>(dist.a()) + std::numeric_limits<result_type>::max() + 1;
    const uresult_type range_max = static_cast<uresult_type>(dist.b()) + std::numeric_limits<result_type>::max() + 1;
    const uresult_type range = (range_max - range_min);

    const double bin_width = static_cast<double>(range) / BINS;

    size_t freq[BINS]{};
    for (size_t i = 0; i < SAMPLES; ++i)
    {
        const result_type x = dist(rng);
        if (x < dist.a() || x > dist.b())
        {
            return false;
        }

        // Map x to unsigned range
        const uresult_type ux = static_cast<uresult_type>(x) + std::numeric_limits<result_type>::max() + 1;

        size_t bin = static_cast<size_t>((ux - range_min) / bin_width);
        if (bin >= BINS)
        {
            // Handle edge case for max value
            bin = BINS - 1;
        }

        ++freq[bin];
    }

    // Expected frequency per bin
    constexpr double expected = static_cast<double>(SAMPLES) / BINS;

    // Calculate chi-squared statistic
    double chi_squared = 0.0;
    for (size_t i = 0; i < BINS; ++i)
    {
        double diff = freq[i] - expected;
        chi_squared += (diff * diff) / expected;
    }

    return chi_squared <= threshold;
}

template <typename RNG, typename Dist, size_t BINS, size_t SAMPLES = 100000>
inline bool test_uniform_real_distribution(RNG& rng, Dist& dist)
{
    static_assert(BINS <= mem::array_size(chi_squared_critical_values));
    constexpr double threshold = chi_squared_critical_values[BINS - 1];

    using result_type = typename Dist::result_type;
    static_assert(std::is_floating_point<result_type>::value);

    const result_type range_min = dist.a();
    const result_type range_max = dist.b();
    const result_type range = (range_max - range_min);

    const double bin_width = static_cast<double>(range) / BINS;

    size_t freq[BINS]{};
    for (size_t i = 0; i < SAMPLES; ++i)
    {
        const result_type x = dist(rng);
        if (x < dist.a() || x > dist.b())
        {
            return false;
        }

        size_t bin = static_cast<size_t>((x - range_min) / bin_width);
        if (bin >= BINS)
        {
            // Handle edge case for max value
            bin = BINS - 1;
        }

        ++freq[bin];
    }

    // Expected frequency per bin
    constexpr double expected = static_cast<double>(SAMPLES) / BINS;

    // Calculate chi-squared statistic
    double chi_squared = 0.0;
    for (size_t i = 0; i < BINS; ++i)
    {
        double diff = freq[i] - expected;
        chi_squared += (diff * diff) / expected;
    }

    return chi_squared <= threshold;
}

} // namespace test
} // namespace vx