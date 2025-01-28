#include "vertex_test/test.hpp"
#include "vertex_test/util/random/random.hpp"

using namespace vx;

using RNG = random::pcg32;

VX_TEST_CASE(pcg32)
{
    VX_SECTION("seeding")
    {
        constexpr size_t n = 20;
        typename random::pcg32::result_type results[n]{};

        ///////////////////////////////////////////////////////////////////////////////

        // These values are obtained from: https://github.com/imneme/pcg-cpp/blob/master/include/
        const typename random::pcg32::result_type expected1[n] = {
             676697322,  420258633, 3418632178, 3595600211, 3265791279,
             257272927, 3607051826, 1330014364, 1691133457, 2692391003,
            1436966076, 3405603488, 3196723772, 2037651542, 1789776910,
            3642929604, 3134326335, 2746793161, 2907548636, 3720053141
        };

        random::pcg32 rng;
        for (auto& x : results)
        {
            x = rng();
        }

        VX_CHECK(std::equal(std::begin(results), std::end(results), std::begin(expected1), std::end(expected1)));

        ///////////////////////////////////////////////////////////////////////////////

        // These values are obtained from: https://github.com/imneme/pcg-cpp/blob/master/include/
        const typename random::pcg32::result_type expected2[n] = {
            1411482639, 3165192603, 3360792183, 2433038347,  628889468,
            3778631550, 2430531221, 2504758782, 1116223725, 3013600377,
            1421996083, 3614188191, 3874189094, 1043933343, 1500908903,
             640134120, 1028161106, 4145427323, 1123590394, 3905095026
        };

        rng.seed(12345);
        for (auto& x : results)
        {
            x = rng();
        }

        VX_CHECK(std::equal(std::begin(results), std::end(results), std::begin(expected2), std::end(expected2)));

        ///////////////////////////////////////////////////////////////////////////////

        // These values are obtained from: https://github.com/imneme/pcg-cpp/blob/master/include/
        const typename random::pcg32::result_type expected3[n] = {
            1607305410, 1357112869, 3673902475, 3050389573, 2926792163,
            4067116395, 2792760830,  539878933, 2038219788, 1432098220,
            4115853849, 2289597163, 3730708165, 2025820233, 4079274970,
             974451971, 3617634205, 1973318027,  174393280, 1530913870
        };

        rng.seed(67890);
        for (auto& x : results)
        {
            x = rng();
        }

        VX_CHECK(std::equal(std::begin(results), std::end(results), std::begin(expected3), std::end(expected3)));
    }

    VX_SECTION("chi squared")
    {
        using Dist = random::uniform_int_distribution<typename RNG::result_type>;

        RNG rng;
        Dist dist;

        constexpr size_t bins = 20;
        constexpr size_t samples = 10000;
        VX_CHECK((test::test_uniform_int_distribution<RNG, Dist, bins, samples>(rng, dist)));
    }
}

VX_TEST_CASE(uniform_int_distribution)
{
    RNG rng;

    VX_SECTION("small range")
    {
        using Dist = random::uniform_int_distribution<uint8_t>;
        Dist dist(1, 10); // range size == 10
    
        constexpr size_t bins = 10;
        constexpr size_t samples = 10000;
        VX_CHECK((test::test_uniform_int_distribution<RNG, Dist, bins, samples>(rng, dist)));
    }
    
    VX_SECTION("larger range")
    {
        using Dist = random::uniform_int_distribution<int>;
        Dist dist(100, 199); // range size == 100
    
        constexpr size_t bins = 20;
        constexpr size_t samples = 100000;
        VX_CHECK((test::test_uniform_int_distribution<RNG, Dist, bins, samples>(rng, dist)));
    }

    VX_SECTION("negative range")
    {
        using Dist = random::uniform_int_distribution<int>;
        Dist dist(-199, -100); // range size == 100

        constexpr size_t bins = 20;
        constexpr size_t samples = 100000;
        VX_CHECK((test::test_uniform_int_distribution<RNG, Dist, bins, samples>(rng, dist)));
    }

    VX_SECTION("extreme range")
    {
        using Dist = random::uniform_int_distribution<int64_t>;
        Dist dist; // max range for int64
    
        constexpr size_t bins = 20;
        constexpr size_t samples = 50000;
        VX_CHECK((test::test_uniform_int_distribution<RNG, Dist, bins, samples>(rng, dist)));
    }

    VX_SECTION("single value range")
    {
        using Dist = random::uniform_int_distribution<int>;
        Dist dist(42, 42); // range size == 1

        constexpr size_t samples = 1000;
        for (size_t i = 0; i < samples; ++i)
        {
            VX_CHECK(dist(rng) == 42);
        }
    }
}

VX_TEST_CASE(uniform_real_distribution)
{
    RNG rng;

    VX_SECTION("small range")
    {
        using Dist = random::uniform_real_distribution<float>;
        Dist dist(0.0f, 1.0f);

        constexpr size_t bins = 10;
        constexpr size_t samples = 20000; // Smaller sample size for quicker tests
        VX_CHECK((test::test_uniform_real_distribution<RNG, Dist, bins, samples>(rng, dist)));
    }

    VX_SECTION("larger range")
    {
        using Dist = random::uniform_real_distribution<double>;
        Dist dist(50.0, 150.0);

        constexpr size_t bins = 20;
        constexpr size_t samples = 100000;
        VX_CHECK((test::test_uniform_real_distribution<RNG, Dist, bins, samples>(rng, dist)));
    }

    VX_SECTION("negative range")
    {
        using Dist = random::uniform_real_distribution<float>;
        Dist dist(-150.0f, -50.0f);

        constexpr size_t bins = 20;
        constexpr size_t samples = 100000;
        VX_CHECK((test::test_uniform_real_distribution<RNG, Dist, bins, samples>(rng, dist)));
    }

    VX_SECTION("extreme range")
    {
        using Dist = random::uniform_real_distribution<long double>;
        Dist dist(std::numeric_limits<long double>::min(), std::numeric_limits<long double>::max());

        constexpr size_t bins = 20;
        constexpr size_t samples = 100000;
        VX_CHECK((test::test_uniform_real_distribution<RNG, Dist, bins, samples>(rng, dist)));
    }

    VX_SECTION("single value range")
    {
        using Dist = random::uniform_real_distribution<float>;
        Dist dist(42.0f, 42.0f);

        constexpr size_t samples = 1000;
        for (size_t i = 0; i < samples; ++i)
        {
            VX_CHECK(dist(rng) == 42.0f);
        }
    }
}

VX_TEST_CASE(bernoulli_distribution)
{
    using Dist = random::bernoulli_distribution;
    RNG rng;

    VX_SECTION("default probability")
    {
        Dist dist;

        constexpr size_t samples = 50000;
        VX_CHECK((test::test_bernoulli_distribution<RNG, Dist, samples>(rng, dist)));
    }

    VX_SECTION("skew true")
    {
        Dist dist(0.75);

        constexpr size_t samples = 50000;
        VX_CHECK((test::test_bernoulli_distribution<RNG, Dist, samples>(rng, dist)));
    }

    VX_SECTION("skew false")
    {
        Dist dist(0.25);

        constexpr size_t samples = 50000;
        VX_CHECK((test::test_bernoulli_distribution<RNG, Dist, samples>(rng, dist)));
    }

    VX_SECTION("all true")
    {
        Dist dist(1.0);

        constexpr size_t samples = 50000;
        size_t results[2]{};
        for (int i = 0; i < samples; ++i)
        {
            const bool x = dist(rng);
            ++results[static_cast<size_t>(x)];
        }

        VX_CHECK(results[0] == 0);
        VX_CHECK(results[1] == samples);
    }

    VX_SECTION("all false")
    {
        Dist dist(0.0);

        constexpr size_t samples = 50000;
        size_t results[2]{};
        for (int i = 0; i < samples; ++i)
        {
            const bool x = dist(rng);
            ++results[static_cast<size_t>(x)];
        }

        VX_CHECK(results[0] == samples);
        VX_CHECK(results[1] == 0);
    }
}

VX_TEST_CASE(normal_distribution)
{
    using Dist = random::normal_distribution<>;
    RNG rng;

    VX_SECTION("default probability")
    {
        Dist dist;

        constexpr size_t samples = 50000;
        VX_CHECK((test::test_normal_distribution<RNG, Dist, samples>(rng, dist)));
    }

    VX_SECTION("custom mean and standard deviation")
    {
        Dist dist(10.0, 2.0); // mean = 10.0, stddev = 2.0

        constexpr size_t samples = 50000;
        VX_CHECK((test::test_normal_distribution<RNG, Dist, samples>(rng, dist)));
    }

    VX_SECTION("small standard deviation")
    {
        Dist dist(0.0, 0.0001); // mean = 0.0, stddev = 0.0001 (very narrow)

        constexpr size_t samples = 200000; // Larger sample size for precision
        VX_CHECK((test::test_normal_distribution<RNG, Dist, samples>(rng, dist)));
    }

    VX_SECTION("large standard deviation")
    {
        Dist dist(0.0, 10000.0); // mean = 0.0, stddev = 10000 (very wide)

        constexpr size_t samples = 200000; // Larger sample size to capture variability
        VX_CHECK((test::test_normal_distribution<RNG, Dist, samples>(rng, dist)));
    }

    VX_SECTION("large positive mean")
    {
        Dist dist(1e6, 1.0); // mean = 1000000, stddev = 1.0

        constexpr size_t samples = 50000;
        VX_CHECK((test::test_normal_distribution<RNG, Dist, samples>(rng, dist)));
    }

    VX_SECTION("large negative mean")
    {
        Dist dist(-1e6, 1.0); // mean = -1000000, stddev = 1.0

        constexpr size_t samples = 50000;
        VX_CHECK((test::test_normal_distribution<RNG, Dist, samples>(rng, dist)));
    }

    VX_SECTION("zero variance")
    {
        Dist dist(5.0, 0.0); // mean = 5.0, stddev = 0.0 (constant distribution)

        constexpr size_t samples = 100; // Small sample size is enough for zero variance
        VX_CHECK((test::test_normal_distribution<RNG, Dist, samples>(rng, dist)));
    }
}

VX_TEST_CASE(discrete_distribution)
{
    using Dist = random::discrete_distribution<>;
    RNG rng;

    VX_SECTION("equal probabilities")
    {
        Dist dist({ 1.0, 1.0, 1.0, 1.0 });

        constexpr size_t samples = 50000;
        VX_CHECK((test::test_discrete_distribution<RNG, Dist, 4, samples>(rng, dist)));
    }

    VX_SECTION("custom probabilities")
    {
        Dist dist({ 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 });

        constexpr size_t samples = 50000;
        VX_CHECK((test::test_discrete_distribution<RNG, Dist, 10, samples>(rng, dist)));
    }

    VX_SECTION("custom probabilities with zero weights")
    {
        Dist dist({ 0.0, 1.0, 0.0, 1.0 });

        constexpr size_t samples = 50000;
        VX_CHECK((test::test_discrete_distribution<RNG, Dist, 4, samples>(rng, dist)));
    }
}

VX_TEST_CASE(algorithms)
{
    RNG rng;

    VX_SECTION("shuffle")
    {
        int data[6]{};
        std::iota(std::begin(data), std::end(data), 0);

        constexpr size_t samples = 100000;
        VX_CHECK((test::test_shuffle<RNG, 2, samples>(rng, data)));
        VX_CHECK((test::test_shuffle<RNG, 3, samples>(rng, data)));
        VX_CHECK((test::test_shuffle<RNG, 4, samples>(rng, data)));
        VX_CHECK((test::test_shuffle<RNG, 5, samples>(rng, data)));
        VX_CHECK((test::test_shuffle<RNG, 6, samples>(rng, data)));
    }

    VX_SECTION("sample")
    {
        int data[20]{};
        std::iota(std::begin(data), std::end(data), 0);

        constexpr size_t samples = 100000;
        VX_CHECK((test::test_sample<RNG, 5, samples>(rng, data)));
        VX_CHECK((test::test_sample<RNG, 10, samples>(rng, data)));
        VX_CHECK((test::test_sample<RNG, 15, samples>(rng, data)));
        VX_CHECK((test::test_sample<RNG, 20, samples>(rng, data)));
    }

    VX_SECTION("discrete sample")
    {
        using Dist = random::discrete_distribution<>;
        Dist dist({ 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 });

        int data[10]{};
        std::iota(std::begin(data), std::end(data), 0);

        constexpr size_t samples = 100000;
        VX_CHECK((test::test_discrete_sample<RNG, Dist, 10, samples>(rng, dist, data)));
    }
}

int main()
{
    VX_RUN_TESTS();
    return 0;
}