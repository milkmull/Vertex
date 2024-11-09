#include "sandbox/sandbox.hpp"

#include <map>

using namespace vx;

static void test_discrete_distribution()
{
    // Define a set of weights for the discrete distribution
    std::initializer_list<double> weights = { 0.0, 1.0, 2.0, 3.0, 4.0 };
    vx::rand::discrete_distribution<int> dist(weights);

    // Display the probabilities of each category
    std::cout << "Probabilities: ";
    for (double p : dist.probabilities())
    {
        std::cout << std::fixed << std::setprecision(2) << p << " ";
    }
    std::cout << "\n";

    // Set up a random number generator
    std::random_device rd;
    rand::pcg32<> rng(rd());

    // Generate samples and count the occurrences of each outcome
    std::map<int, int> histogram;
    const int num_samples = 10000000;
    for (int i = 0; i < num_samples; ++i)
    {
        int sample = dist(rng);
        ++histogram[sample];
    }

    // Display results as a histogram
    std::cout << "Histogram of generated samples:\n";
    for (const auto& [outcome, count] : histogram)
    {
        double probability = static_cast<double>(count) / num_samples;
        int bar_width = static_cast<int>(probability * 50);  // Scale for visualization
        std::cout << outcome << ": " << std::string(bar_width, '*')
            << " (" << probability * 100 << "%)\n";
    }
}

int main(int argc, char* argv[])
{
    test_discrete_distribution();
    return 0;
}