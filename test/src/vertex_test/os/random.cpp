#include "vertex_test/test.hpp"
#include "vertex/os/random.hpp"

using namespace vx;

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_get_entropy)
{
    VX_SECTION("basic test")
    {
        constexpr size_t count = 16;
        uint8_t data[count]{};

        VX_CHECK_AND_EXPECT_NO_ERROR(os::get_entropy(data, count));

        // make sure the data is not all 0
        bool fount_nonzero = false;
        for (const uint8_t byte : data)
        {
            if (byte != 0)
            {
                fount_nonzero = true;
                break;
            }
        }
        VX_CHECK(fount_nonzero);
    }

    VX_SECTION("test quality")
    {
        constexpr size_t SAMPLES = 1000000;  // 1 million bytes
        constexpr size_t BINS = 256;

        // Degrees of freedom (df) for the chi-squared test:
        // In this case, df = (number of bins - 1 OR 255).
        // This comes from the chi-squared test formula, where df is the number of independent categories
        // (bins) minus one, since the total number of samples constrains the last bin's frequency.

        // Used to calculate critical chi square value for system with 255 dof and 95% certainty 
        // https://www.danielsoper.com/statcalc/calculator.aspx?id=12

        constexpr double threshold = 293.248;

        // generate bytes
        std::vector<uint8_t> data(SAMPLES);
        VX_CHECK_AND_EXPECT_NO_ERROR(os::get_entropy(data.data(), SAMPLES));

        // calculate frequencies
        size_t freq[BINS]{};
        for (uint8_t byte : data)
        {
            ++freq[byte];
        }

        // Expected frequency per bin
        constexpr double expected = static_cast<double>(SAMPLES) / BINS;

        // Calculate chi-squared statistic
        double chi_squared = 0.0;
        for (size_t i = 0; i < BINS; ++i)
        {
            const double diff = static_cast<double>(freq[i]) - expected;
            chi_squared += (diff * diff) / expected;
        }

        VX_CHECK(chi_squared <= threshold);
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}