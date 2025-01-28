#pragma once

#include "vertex/util/memory/memory.hpp"

#include "vertex/util/random/pcg.hpp"
#include "vertex/util/random/uniform_int_distribution.hpp"
#include "vertex/util/random/uniform_real_distribution.hpp"
#include "vertex/util/random/bernoulli_distribution.hpp"
#include "vertex/util/random/normal_distribution.hpp"
#include "vertex/util/random/discrete_distribution.hpp"
#include "vertex/util/random/algorithm.hpp"

namespace vx {
namespace test {

// https://www.scribbr.com/statistics/chi-square-distribution-table/

// Chi-squared critical value for confidence level 0.05 (95% confidence) with
// k-1 degrees of freedom The degrees of freedom depend on the range size (k = range_size)
constexpr double chi_squared_critical_values[] = {
    3.841,  5.991,  7.815,  9.488,  11.070, 12.592, 14.067, 15.507, 16.919, 18.307,
    19.675, 21.026, 22.362, 23.685, 24.996, 26.296, 27.587, 28.869, 30.144, 31.410,
    32.671, 33.924, 35.172, 36.415, 37.652, 38.885, 40.113, 41.337, 42.557, 43.773,
    44.985, 46.194, 47.400, 48.602, 49.802, 50.998, 52.192, 53.384, 54.572, 55.758
};

// https://github.com/microsoft/STL/blob/9b38fb9ccca90fd8b31fd37d4512b190aeef4fe7/tests/std/tests/GH_000178_uniform_int/test.cpp

template <typename RNG, typename Dist, size_t BINS, size_t SAMPLES>
inline bool test_uniform_int_distribution(RNG& rng, Dist& dist)
{
    // Degrees of freedom (df) for the chi-squared test:
    // In this case, df = (number of bins - 1).
    // This comes from the chi-squared test formula, where df is the number of independent categories
    // (bins) minus one, since the total number of samples constrains the last bin's frequency.
    constexpr size_t df = BINS - 1;
    static_assert(df <= mem::array_size(chi_squared_critical_values));
    constexpr double threshold = chi_squared_critical_values[df - 1];

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

template <typename RNG, typename Dist, size_t BINS, size_t SAMPLES>
inline bool test_uniform_real_distribution(RNG& rng, Dist& dist)
{
    // Degrees of freedom (df) for the chi-squared test:
    // In this case, df = (number of bins - 1).
    // This comes from the chi-squared test formula, where df is the number of independent categories
    // (bins) minus one, since the total number of samples constrains the last bin's frequency.
    constexpr size_t df = BINS - 1;
    static_assert(df <= mem::array_size(chi_squared_critical_values));
    constexpr double threshold = chi_squared_critical_values[df - 1];

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

template <typename RNG, typename Dist, size_t SAMPLES>
inline bool test_bernoulli_distribution(RNG& rng, Dist& dist)
{
    // Bernoulli distribution has 1 degree of freedom because the outcome probabilities
    // (success and failure) are determined by a single parameter p.
    constexpr size_t df = 1;
    constexpr double threshold = chi_squared_critical_values[df];
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

template <typename RNG, typename Dist, size_t SAMPLES>
inline bool test_normal_distribution(RNG& rng, Dist& dist)
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

template <typename RNG, typename Dist, size_t N, size_t SAMPLES>
inline bool test_discrete_distribution(RNG& rng, Dist& dist)
{
    static_assert(N <= mem::array_size(chi_squared_critical_values));
    constexpr double threshold = chi_squared_critical_values[N - 1];

    using result_type = typename Dist::result_type;
    const std::vector<double>& probabilities = dist.probabilities();
    VX_ASSERT(probabilities.size() == N);

    size_t freq[N]{};
    for (size_t i = 0; i < SAMPLES; ++i)
    {
        const result_type x = dist(rng);
        ++freq[static_cast<size_t>(x)];
    }

    // Expected frequencies
    double expected[N]{};
    for (size_t i = 0; i < N; ++i)
    {
        expected[i] = SAMPLES * probabilities[i];
    }

    // Calculate chi-squared statistic
    double chi_squared = 0.0;
    for (size_t i = 0; i < N; ++i)
    {
        if (expected[i] == 0.0)
        {
            if (freq[i] != 0.0) return false;
            continue;
        }

        const double diff = freq[i] - expected[i];
        chi_squared += (diff * diff) / expected[i];
    }

    return chi_squared <= threshold;
}

template <typename RNG, size_t N, size_t SAMPLES>
inline bool test_shuffle(RNG& rng, const int* data)
{
    // Degrees of freedom (df) for the chi-squared test:
    // Each of the N values can occupy N positions, giving N^2 cells in the NxN grid.
    // However, there are N constraints (one for each value, ensuring its total count across all positions equals SAMPLES).
    // This reduces the total degrees of freedom from N^2 to N * (N - 1).
    constexpr size_t df = N * (N - 1);
    static_assert(df <= mem::array_size(chi_squared_critical_values));
    constexpr double threshold = chi_squared_critical_values[df - 1];

    size_t counts[N][N]{};
    int shuffled[N]{};

    // Track how often each value ends up in each position
    for (size_t i = 0; i < SAMPLES; ++i)
    {
        // Reset the data and shuffle
        std::memcpy(shuffled, data, N * sizeof(int));
        random::shuffle(std::begin(shuffled), std::end(shuffled), rng);

        // Track where each value ends up
        for (size_t j = 0; j < N; ++j)
        {
            const int x = shuffled[j];
            ++(counts[x][j]);
        }
    }

    // Each value should have the same expected value for each position
    constexpr double expected = static_cast<double>(SAMPLES) / N;

    // Calculate chi-squared statistic
    double chi_squared = 0.0;
    for (size_t value = 0; value < N; ++value)
    {
        for (size_t position = 0; position < N; ++position)
        {
            const double diff = counts[value][position] - expected;
            chi_squared += (diff * diff) / expected;
        }
    }

    return chi_squared <= threshold;
}

template <typename RNG, size_t N, size_t SAMPLES>
inline bool test_sample(RNG& rng, const int* data)
{
    constexpr size_t df = N - 1;
    static_assert(df <= mem::array_size(chi_squared_critical_values));
    constexpr double threshold = chi_squared_critical_values[df - 1];

    size_t freq[N]{};
    for (size_t i = 0; i < SAMPLES; ++i)
    {
        int x;
        random::sample(data, data + N, &x, 1, rng);
        ++freq[x];
    }

    // Expected frequency
    constexpr double expected = static_cast<double>(SAMPLES) / N;

    // Calculate chi-squared statistic
    double chi_squared = 0.0;
    for (size_t i = 0; i < N; ++i)
    {
        const double diff = freq[i] - expected;
        chi_squared += (diff * diff) / expected;
    }

    return chi_squared <= threshold;
}

template <typename RNG, typename Dist, size_t N, size_t SAMPLES>
inline bool test_discrete_sample(RNG& rng, Dist& dist, const int* data)
{
    constexpr size_t df = N - 1;
    static_assert(df <= mem::array_size(chi_squared_critical_values));
    constexpr double threshold = chi_squared_critical_values[df - 1];

    using result_type = typename Dist::result_type;
    const std::vector<double>& probabilities = dist.probabilities();
    VX_ASSERT(probabilities.size() == N);

    size_t freq[N]{};
    for (size_t i = 0; i < SAMPLES; ++i)
    {
        int x;
        random::sample(data, data + N, &x, 1, dist, rng);
        ++freq[x];
    }

    // Expected frequency
    // Expected frequencies
    double expected[N]{};
    for (size_t i = 0; i < N; ++i)
    {
        expected[i] = SAMPLES * probabilities[i];
    }

    // Calculate chi-squared statistic
    double chi_squared = 0.0;
    for (size_t i = 0; i < N; ++i)
    {
        if (expected[i] == 0.0)
        {
            if (freq[i] != 0.0) return false;
            continue;
        }

        const double diff = freq[i] - expected[i];
        chi_squared += (diff * diff) / expected[i];
    }

    return chi_squared <= threshold;
}

} // namespace test
} // namespace vx