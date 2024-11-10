#include "sandbox/sandbox.hpp"

#include <vector>
#include <map>

using namespace vx;

static void test()
{
    rng rng;

    std::vector<int> pop;
    std::vector<double> weights;

    for (int i = 0; i < 5; ++i)
    {
        pop.push_back(i);
        weights.push_back(i);
    }

    size_t trials = 1000000;
    size_t sample_size = 10;
    std::map<int, size_t> counts;
    rand::discrete_distribution<> dist(weights.begin(), weights.end());

    for (size_t i = 0; i < trials; ++i)
    {
        std::vector<int> samples;
        rand::sample(pop.begin(), pop.end(), std::back_inserter(samples), sample_size, dist, rng);

        for (const auto& x : samples)
        {
            ++counts[x];
        }
    }

    // Calculate and display histogram
    VX_LOG_INFO << "Histogram of sample selections (each '*' represents ~1%):\n";
    for (const auto& [value, count] : counts)
    {
        double percentage = (static_cast<double>(count) / (trials * sample_size)) * 100;
        int num_stars = static_cast<int>(percentage);
        VX_LOG_INFO << value << ": " << std::string(num_stars, '*') << " (" << percentage << "%)\n";
    }
}

int main(int argc, char* argv[])
{
    test();
    return 0;
}