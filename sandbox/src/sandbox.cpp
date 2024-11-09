#include "sandbox/sandbox.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    size_t num_samples = 100000000; // Number of random samples to generate
    constexpr long double min_value = -100;
    constexpr long double max_value = 100;

    test_pcg32_distribution(num_samples, min_value, max_value);
}