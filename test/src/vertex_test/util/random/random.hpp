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

// WARNING: using this function with a distribution over the
// full range of 64 bit integers will fail due to wrapping
template <typename RNG, typename Dist, size_t BINS, size_t SAMPLES = 100000>
inline bool test_uniform_distribution(RNG& rng, Dist& dist)
{
    static_assert(BINS <= mem::array_size(chi_squared_critical_values));
    constexpr double threshold = chi_squared_critical_values[BINS - 1];

    using result_type = typename Dist::result_type;
    static_assert(std::is_integral<result_type>::value);

    const result_type range = (dist.b() - dist.a()) + 1; // +1 because it is inclusive
    const double bin_width = static_cast<double>(range) / BINS;

    size_t freq[BINS]{};
    for (size_t i = 0; i < SAMPLES; ++i)
    {
        const result_type x = dist(rng);
        if (x < dist.a() || x > dist.b())
        {
            return false;
        }

        size_t bin = static_cast<size_t>((x - dist.a()) / bin_width);
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