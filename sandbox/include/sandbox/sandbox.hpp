#define VX_ENABLE_PROFILING

#include <random>
#include <iomanip>

#include "vertex/main/main.hpp"
#include "vertex/core/log.hpp"
#include "vertex/core/error.hpp"

#include "vertex/util/random/pcg.hpp"
#include "vertex/os/time.hpp"
#include "vertex/util/random/uniform_real_distribution.hpp"
#include "vertex/util/random/normal_distribution.hpp"

template <typename T>
inline void test_pcg32_distribution(size_t num_samples, T min_value, T max_value)
{
    rand::pcg32<> rng(os::system_time().as_nanoseconds()); // Instantiate the PCG random number generator

    // Define parameters
    double mean = 0.0;
    double stddev = 1.0;
    rand::normal_distribution<double> distribution(mean, stddev);

    // Generate values
    const int sample_size = 100000; // Number of samples
    std::vector<double> values;
    for (int i = 0; i < sample_size; ++i)
    {
        values.push_back(distribution(rng));
    }

    // Histogram parameters
    const int num_bins = 30;
    std::vector<int> histogram(num_bins, 0);

    // Determine range
    double min_val = -3.0 * stddev + mean; // Capture most of the normal distribution range
    double max_val = 3.0 * stddev + mean;
    double bin_width = (max_val - min_val) / num_bins;

    // Fill histogram
    for (const auto& value : values)
    {
        if (value >= min_val && value < max_val)
        {
            int bin = static_cast<int>((value - min_val) / bin_width);
            histogram[bin]++;
        }
    }

    // Find maximum count in histogram for scaling
    int max_count = *std::max_element(histogram.begin(), histogram.end());

    // Display histogram
    for (int i = 0; i < num_bins; ++i)
    {
        double bin_start = min_val + i * bin_width;
        double bin_end = bin_start + bin_width;

        // Print bin range with aligned formatting
        std::cout << std::fixed << std::setw(6) << std::setprecision(2) << bin_start << " - "
            << std::setw(6) << std::setprecision(2) << bin_end << " : ";

        // Print scaled number of stars
        int bar_length = static_cast<int>((histogram[i] / static_cast<double>(max_count)) * 50);
        std::cout << std::string(bar_length, '*') << '\n';
    }
}
