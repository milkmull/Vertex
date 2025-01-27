#pragma once

#include "vertex/util/memory/memory.hpp"

namespace vx {
namespace test {

// https://www.scribbr.com/statistics/chi-square-distribution-table/

// Chi-squared critical value for confidence level 0.05 (95% confidence) with
// k-1 degrees of freedom The degrees of freedom depend on the range size (k = range_size)
constexpr double chi_squared_critical_values[] = {
    3.841, 5.991, 7.815, 9.488, 11.070, 12.592, 14.067, 15.507, 16.919, 18.307,
    19.675, 21.026, 22.362, 23.685, 24.996, 26.296, 27.587, 28.869, 30.144, 31.410
};

// https://github.com/microsoft/STL/blob/9b38fb9ccca90fd8b31fd37d4512b190aeef4fe7/tests/std/tests/GH_000178_uniform_int/test.cpp

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

    const long double bin_width = static_cast<long double>(range) / BINS;

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

template <typename RNG, typename Dist, size_t SAMPLES = 100000>
inline bool test_bernoulli_distribution(RNG& rng, Dist& dist)
{
    constexpr double threshold = chi_squared_critical_values[1];
    using result_type = typename Dist::result_type;

    size_t freq[2]{};
    for (size_t i = 0; i < SAMPLES; ++i)
    {
        const result_type x = dist(rng);
        ++freq[static_cast<size_t>(x)];
    }

    // Get the probability `p` from the distribution
    const double p = dist.p();

    // Expected frequency per bin
    const double expected_true = SAMPLES * p;
    const double expected_false = SAMPLES * (1.0 - p);

    // Calculate chi-squared statistic
    const double diff0 = freq[0] - expected_false;
    const double diff1 = freq[1] - expected_true;
    const double chi_squared = ((diff0 * diff0) / expected_false) + ((diff1 * diff1) / expected_true);

    return chi_squared <= threshold;
}

template <typename RNG, typename Dist, size_t SAMPLES = 100000>
inline bool test_real_distribution(RNG& rng, Dist& dist)
{
    using result_type = typename Dist::result_type;

    result_type sum = 0.0;
    result_type sq_sum = 0.0;
    result_type mean = 0.0;
    result_type stddev = 0.0;

    for (size_t i = 0; i < SAMPLES; ++i)
    {
        const result_type x = dist(rng);
        sum += x;
        sq_sum += (x * x);
    }

    // Calculate the mean and stddev
    mean = sum / SAMPLES;
    stddev = std::sqrt(sq_sum / SAMPLES - mean * mean);

    const result_type expected_mean = dist.mean();
    const result_type expected_stddev = dist.stddev();

    // Confidence level constants (for 95% confidence)
    constexpr result_type Z = 1.96;
    // Epsilon for mean
    const result_type epsilon_mean = Z * expected_stddev / std::sqrt(SAMPLES);
    // Epsilon for standard deviation (approximation)
    const result_type epsilon_stddev = Z * expected_stddev / std::sqrt(2 * SAMPLES);

    // Compare against dynamically computed tolerances
    const bool mean_ok = std::abs(mean - expected_mean) <= epsilon_mean;
    const bool stddev_ok = std::abs(stddev - expected_stddev) <= epsilon_stddev;

    return (mean_ok && stddev_ok);
}

} // namespace test
} // namespace vx