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
    // consistency
    {
        VX_SECTION("int8_t consistency")
        {
            RNG rng;

            using type = int8_t;
            using Dist = random::uniform_int_distribution<type>;
            Dist dist(-100, 100);

            constexpr size_t count = 20;
            const type expected[count] = {
                -69, -81, 59, 68,  52, -88, 68, -38, -21, 26,
                -33,  59, 49, -5, -17,  70, 46,  28,  36, 74,
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        VX_SECTION("uint8_t consistency")
        {
            RNG rng;

            using type = uint8_t;
            using Dist = random::uniform_int_distribution<type>;
            Dist dist(0, 100);

            constexpr size_t count = 20;
            const type expected[count] = {
                15,  9, 80, 84, 76,  6, 84, 31, 39, 63,
                33, 80, 75, 47, 42, 85, 73, 64, 68, 87
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        VX_SECTION("int32_t consistency")
        {
            RNG rng;

            using type = int32_t;
            using Dist = random::uniform_int_distribution<type>;
            Dist dist(static_cast<type>(INT8_MIN) - 100, static_cast<type>(INT8_MAX) + 100);

            constexpr size_t count = 20;
            const type expected[count] = {
                -157, -184, 134, 153, 118, -201, 154, -87, -49,  57,
                 -76,  133, 111, -12, -38,  158, 104,  63,  80, 166
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        VX_SECTION("uint32_t consistency")
        {
            RNG rng;

            using type = uint32_t;
            using Dist = random::uniform_int_distribution<type>;
            Dist dist(0, static_cast<type>(UINT8_MAX) + 100);

            constexpr size_t count = 20;
            const type expected[count] = {
                 56,  34, 283, 298, 270,  21, 298, 110, 140, 223,
                119, 282, 264, 168, 148, 301, 259, 227, 241, 308
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        VX_SECTION("int64_t consistency")
        {
            RNG rng;

            using type = int64_t;
            using Dist = random::uniform_int_distribution<type>;
            Dist dist(static_cast<type>(INT32_MIN) - 100, static_cast<type>(INT32_MAX) + 100);

            constexpr size_t count = 20;
            const type expected[count] = {
                -1727225115,   544907255,  1258119740,  1495445856,  -399259835,
                 2085735633, -1493636802,  -179674110, -1223994334, -1167072243,
                 1039660707, -1825088267, -1644326292,  -419335858,   937084249,
                -1142696948, -1989447188,  -118315441,   490115782,  1130696810
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        VX_SECTION("uint64_t consistency")
        {
            RNG rng;

            using type = uint64_t;
            using Dist = random::uniform_int_distribution<type>;
            Dist dist(0, static_cast<type>(INT32_MAX) + 100);

            constexpr size_t count = 20;
            const type expected[count] = {
                676697322,   420258633,  257272927, 1330014364, 1691133457,
                1436966076, 2037651542, 1789776910, 1238309885, 1748223913,
                 923246893,  287986548, 1171138885, 1019579271, 1954310128, 
                 653846946,  385723332, 1967809638, 1982820798,  923489414
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }
    }

    // quality
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
}

VX_TEST_CASE(uniform_real_distribution)
{
    // consistency
    {
        VX_SECTION("float consistency 0-1")
        {
            RNG rng;

            using type = float;
            using Dist = random::uniform_real_distribution<type>;
            Dist dist(0.0f, 1.0f);

            constexpr size_t count = 20;
            const type expected[count] = {
                0x1.42aca80000000p-3f, 0x1.90ca300000000p-4f, 0x1.9788600000000p-1f, 0x1.aca1020000000p-1f, 0x1.85500a0000000p-1f,
                0x1.eab5800000000p-5f, 0x1.adfe7c0000000p-1f, 0x1.3d19a00000000p-2f, 0x1.9332980000000p-2f, 0x1.40f5400000000p-1f,
                0x1.5699700000000p-2f, 0x1.95fac60000000p-1f, 0x1.7d14440000000p-1f, 0x1.e5d0600000000p-2f, 0x1.aab7500000000p-2f,
                0x1.b245640000000p-1f, 0x1.75a40c0000000p-1f, 0x1.47717a0000000p-1f, 0x1.5a9b580000000p-1f, 0x1.bb77040000000p-1f
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        VX_SECTION("float consistency")
        {
            RNG rng;

            using type = float;
            using Dist = random::uniform_real_distribution<type>;
            Dist dist(-100.0f, 100.0f);

            constexpr size_t count = 20;
            const type expected[count] = {
                -0x1.11f48e0000000p+6f, -0x1.41b8820000000p+6f,  0x1.d98a300000000p+5f,  0x1.0dbb940000000p+6f,  0x1.a09a200000000p+5f,
                -0x1.6014460000000p+6f,  0x1.0fdda00000000p+6f, -0x1.3087f60000000p+5f, -0x1.5401e80000000p+4f,  0x1.95fcd00000000p+4f,
                -0x1.08b0400000000p+5f,  0x1.d4afa80000000p+5f,  0x1.86df580000000p+5f, -0x1.4753400000000p+2f, -0x1.0a83280000000p+4f,
                 0x1.168c6c0000000p+6f,  0x1.6fa0a80000000p+5f,  0x1.be85380000000p+4f,  0x1.1b25700000000p+5f,  0x1.24e9f80000000p+6f
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        VX_SECTION("double consistency 0-1")
        {
            RNG rng;

            using type = double;
            using Dist = random::uniform_real_distribution<type>;
            Dist dist(0.0, 1.0);

            constexpr size_t count = 20;
            const type expected[count] = {
                0x1.90ca349285590p-4, 0x1.aca102a797886p-1, 0x1.eab58bf855000p-5, 0x1.3d19a2735bfd0p-2, 0x1.40f540b6c9995p-1,
                0x1.95fac540ab4ccp-1, 0x1.e5d0615afa288p-2, 0x1.b2456388d55bbp-1, 0x1.4771799375a41p-1, 0x1.bb77032b5a9b5p-1,
                0x1.a0cf1ea5273c6p-2, 0x1.66e2d811711c6p-1, 0x1.b83d096f9b670p-3, 0x1.d484e8694b0f4p-1, 0x1.12a5374ae1b78p-4,
                0x1.1738a51728ddep-2, 0x1.f8a39a6a798b1p-1, 0x1.37c74d13a3e34p-3, 0x1.d52991985bf6ap-2, 0x1.b85aa433b17b8p-3
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        VX_SECTION("double consistency")
        {
            RNG rng;

            using type = double;
            using Dist = random::uniform_real_distribution<type>;
            Dist dist(-1e10, 1e10);

            constexpr size_t count = 20;
            const type expected[count] = {
                -0x1.df66b7fed10ddp+32,  0x1.91eeded8b05a4p+32, -0x1.0651c65a592e4p+33, -0x1.c5c95f469e334p+31,  0x1.2e7bfe1b85accp+31,
                 0x1.5d32ca946043ap+32, -0x1.e7c0a5b35fe00p+28,  0x1.9f11d37ba4408p+32,  0x1.4caf01192efecp+31,  0x1.b479c87410a10p+32,
                -0x1.bb44711170874p+30,  0x1.df19ccd3d90b0p+31, -0x1.53cb40d499805p+32,  0x1.eecf4c117e438p+32, -0x1.020e9b991250ap+33,
                -0x1.0efd77c444ea1p+32,  0x1.217437fc9d88ep+33, -0x1.9e9145541d7d6p+32, -0x1.8ef4d99c0ab90p+29, -0x1.53ba056877957p+32
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }
    }

    // quality
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
}

VX_TEST_CASE(bernoulli_distribution)
{
    using Dist = random::bernoulli_distribution;

    VX_SECTION("default probability")
    {
        Dist dist;

        // consistency
        {
            RNG rng;
            using type = typename Dist::result_type;

            constexpr size_t count = 100;
            const type expected[count] = {
                1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0,
                0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1,
                0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1,
                0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        // quality
        {
            RNG rng;
            constexpr size_t samples = 50000;
            VX_CHECK((test::test_bernoulli_distribution<RNG, Dist, samples>(rng, dist)));
        }
    }

    VX_SECTION("skew true")
    {
        Dist dist(0.75);

        // consistency
        {
            RNG rng;
            using type = typename Dist::result_type;

            constexpr size_t count = 100;
            const type expected[count] = {
                1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
                0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1,
                1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        // quality
        {
            RNG rng;
            constexpr size_t samples = 50000;
            VX_CHECK((test::test_bernoulli_distribution<RNG, Dist, samples>(rng, dist)));
        }
    }

    VX_SECTION("skew false")
    {
        Dist dist(0.25);

        // consistency
        {
            RNG rng;
            using type = typename Dist::result_type;

            constexpr size_t count = 100;
            const type expected[count] = {
                1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0,
                0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1,
                0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        // quality
        {
            RNG rng;
            constexpr size_t samples = 50000;
            VX_CHECK((test::test_bernoulli_distribution<RNG, Dist, samples>(rng, dist)));
        }
    }

    VX_SECTION("all true")
    {
        RNG rng;
        Dist dist(1.0);

        constexpr size_t samples = 50000;
        size_t results[2]{};
        for (size_t i = 0; i < samples; ++i)
        {
            const bool x = dist(rng);
            ++results[static_cast<size_t>(x)];
        }

        VX_CHECK(results[0] == 0);
        VX_CHECK(results[1] == samples);
    }

    VX_SECTION("all false")
    {
        RNG rng;
        Dist dist(0.0);

        constexpr size_t samples = 50000;
        size_t results[2]{};
        for (size_t i = 0; i < samples; ++i)
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

    VX_SECTION("default probability")
    {
        // consistency
        {
            RNG rng;
            Dist dist;
            using type = typename Dist::result_type;

            constexpr size_t count = 20;
            const type expected[count] = {
                -0x1.4cba4092a43dep-3, -0x1.80ad9ca152514p-2,  0x1.3ab82be233c78p+0,  0x1.109e27a213f25p-1,  0x1.3206d90ebe376p+0,
                -0x1.679d79466a362p-4,  0x1.d871a88cf09b9p-1,  0x1.6819490a34ec5p-2,  0x1.a366fb494378cp+0, -0x1.8408ac27ef7dbp-1,
                -0x1.1c049b22735d4p-3, -0x1.0e7693716c054p-2, -0x1.783383e7a1339p+0, -0x1.b9ca4b3f5c58ap-3, -0x1.70dfacebf5844p-1,
                -0x1.6b6ad779e011fp-1, -0x1.bd59cdc3774d2p-1, -0x1.eaf22c89c0e67p-4,  0x1.998388b3aedc1p-1,  0x1.3cd38286f61b3p-3
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        // quality
        {
            RNG rng;
            Dist dist;
            constexpr size_t samples = 50000;
            VX_CHECK((test::test_normal_distribution<RNG, Dist, samples>(rng, dist)));
        }
    }

    VX_SECTION("custom mean and standard deviation")
    {
        // consistency
        {
            RNG rng;
            Dist dist(10.0, 2.0); // mean = 10.0, stddev = 2.0
            using type = typename Dist::result_type;

            constexpr size_t count = 20;
            const type expected[count] = {
                0x1.359a2dfb6ade1p+3, 0x1.27f52635eadafp+3, 0x1.8eae0af88cf1ep+3, 0x1.6213c4f4427e5p+3, 0x1.8c81b643af8dep+3,
                0x1.3a618a1ae6572p+3, 0x1.7b0e35119e137p+3, 0x1.56819490a34ecp+3, 0x1.a8d9bed250de3p+3, 0x1.0f7eea7b02105p+3,
                0x1.371fdb26ec651p+3, 0x1.2f1896c8e93fbp+3, 0x1.c3e63e0c2f664p+2, 0x1.3231ada6051d4p+3, 0x1.11e40a62814f8p+3,
                0x1.1292a510c3fdcp+3, 0x1.0854c64791166p+3, 0x1.3854374dd8fc6p+3, 0x1.7330711675db8p+3, 0x1.49e69c1437b0ep+3
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        // quality
        {
            RNG rng;
            Dist dist(10.0, 2.0); // mean = 10.0, stddev = 2.0
            constexpr size_t samples = 50000;
            VX_CHECK((test::test_normal_distribution<RNG, Dist, samples>(rng, dist)));
        }
    }

    VX_SECTION("small standard deviation")
    {
        // consistency
        {
            RNG rng;
            Dist dist(0.0, 0.0001); // mean = 0.0, stddev = 0.0001 (very narrow)
            using type = typename Dist::result_type;

            constexpr size_t count = 20;
            const type expected[count] = {
                -0x1.10920634229acp-16, -0x1.3b20dc0fd405fp-15,  0x1.01d1722edc2f2p-13,  0x1.bea8308d07b1ep-15,  0x1.f564ec0621f11p-14,
                -0x1.2698c2602fafdp-17,  0x1.8306aef0cba8dp-14,  0x1.26fe2f8bd7cbdp-15,  0x1.579308044fd95p-13, -0x1.3de09a232c3acp-14,
                -0x1.d155c7b2185a9p-17, -0x1.bb207b9b1c4bdp-16, -0x1.342f1dd2a4015p-13, -0x1.69ea1f55152c6p-16, -0x1.2e2e6db56b543p-14,
                -0x1.29b6246f4f1c4p-14, -0x1.6cd4d4cc5cd10p-14, -0x1.922ed64083cf4p-17,  0x1.4f7944fa6a619p-14,  0x1.038b45a77ea75p-16
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        // quality
        {
            RNG rng;
            Dist dist(0.0, 0.0001); // mean = 0.0, stddev = 0.0001 (very narrow)
            constexpr size_t samples = 300000; // Larger sample size for precision
            VX_CHECK((test::test_normal_distribution<RNG, Dist, samples>(rng, dist)));
        }
    }

    VX_SECTION("large standard deviation")
    {
        // consistency
        {
            RNG rng;
            Dist dist(0.0, 10000.0); // mean = 0.0, stddev = 10000 (very wide)
            using type = typename Dist::result_type;

            constexpr size_t count = 20;
            const type expected[count] = {
                -0x1.96295bd3017d8p+10, -0x1.d593edb2ecfc3p+11,  0x1.802dd191a0350p+13,  0x1.4cc90f6159595p+12,  0x1.75915bf67f32ap+13,
                -0x1.b6fbba8a74a71p+9,   0x1.205b5f2005dafp+13,  0x1.b792dda8f59a7p+11,  0x1.fff735beeedcep+13, -0x1.d9ac9626bfd8fp+12,
                -0x1.5ab39f5f8dd35p+10, -0x1.4a27befbf45c7p+11, -0x1.cb3ae28440477p+13, -0x1.0da5b86d6c1d1p+11, -0x1.c2490a960933fp+12,
                -0x1.bb9fec084605ep+12, -0x1.0fd20fd68d90dp+13, -0x1.2ba64faf13fcap+10,  0x1.f3e5105f56f3ap+12,  0x1.82c030d5bf6c3p+10
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        // quality
        {
            RNG rng;
            Dist dist(0.0, 10000.0); // mean = 0.0, stddev = 10000 (very wide)
            constexpr size_t samples = 300000; // Larger sample size to capture variability
            VX_CHECK((test::test_normal_distribution<RNG, Dist, samples>(rng, dist)));
        }
    }

    VX_SECTION("large positive mean")
    {
        // consistency
        {
            RNG rng;
            Dist dist(1e6, 1.0); // mean = 1000000, stddev = 1.0
            using type = typename Dist::result_type;

            constexpr size_t count = 20;
            const type expected[count] = {
                0x1.e847facd16fdbp+19, 0x1.e847f3fa931afp+19, 0x1.e8482757057c4p+19, 0x1.e8481109e27a2p+19, 0x1.e8482640db21dp+19,
                0x1.e847fd30c50d7p+19, 0x1.e8481d871a88dp+19, 0x1.e8480b40ca485p+19, 0x1.e848346cdf693p+19, 0x1.e847e7bf753d8p+19,
                0x1.e847fb8fed937p+19, 0x1.e847f78c4b647p+19, 0x1.e847d0f98f831p+19, 0x1.e847f918d6d30p+19, 0x1.e847e8f205314p+19,
                0x1.e847e94952886p+19, 0x1.e847e42a6323dp+19, 0x1.e847fc2a1ba6fp+19, 0x1.e8481998388b4p+19, 0x1.e84804f34e0a2p+19
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        // quality
        {
            RNG rng;
            Dist dist(1e6, 1.0); // mean = 1000000, stddev = 1.0
            constexpr size_t samples = 300000;
            VX_CHECK((test::test_normal_distribution<RNG, Dist, samples>(rng, dist)));
        }
    }

    VX_SECTION("large negative mean")
    {
        // consistency
        {
            RNG rng;
            Dist dist(-1e6, 1.0); // mean = -1000000, stddev = 1.0
            using type = typename Dist::result_type;

            constexpr size_t count = 20;
            const type expected[count] = {
                -0x1.e8480532e9025p+19, -0x1.e8480c056ce51p+19, -0x1.e847d8a8fa83cp+19, -0x1.e847eef61d85ep+19, -0x1.e847d9bf24de3p+19,
                -0x1.e84802cf3af29p+19, -0x1.e847e278e5773p+19, -0x1.e847f4bf35b7bp+19, -0x1.e847cb932096dp+19, -0x1.e84818408ac28p+19,
                -0x1.e8480470126c9p+19, -0x1.e8480873b49b9p+19, -0x1.e8482f06707cfp+19, -0x1.e84806e7292d0p+19, -0x1.e848170dfacecp+19,
                -0x1.e84816b6ad77ap+19, -0x1.e8481bd59cdc3p+19, -0x1.e84803d5e4591p+19, -0x1.e847e667c774cp+19, -0x1.e847fb0cb1f5ep+19
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        // quality
        {
            RNG rng;
            Dist dist(-1e6, 1.0); // mean = -1000000, stddev = 1.0
            constexpr size_t samples = 300000;
            VX_CHECK((test::test_normal_distribution<RNG, Dist, samples>(rng, dist)));
        }
    }

    VX_SECTION("zero variance")
    {
        // consistency
        {
            RNG rng;
            Dist dist(5.0, 0.0); // mean = 5.0, stddev = 0.0 (constant distribution)
            using type = typename Dist::result_type;

            constexpr size_t count = 20;
            const type expected[count] = {
                0x1.4000000000000p+2, 0x1.4000000000000p+2, 0x1.4000000000000p+2, 0x1.4000000000000p+2, 0x1.4000000000000p+2,
                0x1.4000000000000p+2, 0x1.4000000000000p+2, 0x1.4000000000000p+2, 0x1.4000000000000p+2, 0x1.4000000000000p+2,
                0x1.4000000000000p+2, 0x1.4000000000000p+2, 0x1.4000000000000p+2, 0x1.4000000000000p+2, 0x1.4000000000000p+2,
                0x1.4000000000000p+2, 0x1.4000000000000p+2, 0x1.4000000000000p+2, 0x1.4000000000000p+2, 0x1.4000000000000p+2
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        // quality
        {
            RNG rng;
            Dist dist(5.0, 0.0); // mean = 5.0, stddev = 0.0 (constant distribution)
            constexpr size_t samples = 100; // Small sample size is enough for zero variance
            VX_CHECK((test::test_normal_distribution<RNG, Dist, samples>(rng, dist)));
        }
    }
}

VX_TEST_CASE(discrete_distribution)
{
    using Dist = random::discrete_distribution<>;

    VX_SECTION("equal probabilities")
    {
        // consistency
        {
            RNG rng;
            using type = typename Dist::result_type;
            Dist dist({ 1.0, 1.0, 1.0, 1.0 });

            constexpr size_t count = 20;
            const type expected[count] = {
                0, 3, 0, 1, 2, 3, 1, 3, 2, 3,
                1, 2, 0, 3, 0, 1, 3, 0, 1, 0
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        // quality
        {
            RNG rng;
            Dist dist({ 1.0, 1.0, 1.0, 1.0 });

            constexpr size_t samples = 50000;
            VX_CHECK((test::test_discrete_distribution<RNG, Dist, 4, samples>(rng, dist)));
        }
    }

    VX_SECTION("custom probabilities")
    {
        // consistency
        {
            RNG rng;
            using type = typename Dist::result_type;
            Dist dist({ 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 });

            constexpr size_t count = 20;
            const type expected[count] = {
                2, 9, 2, 5, 7, 8, 6, 9, 7, 9,
                6, 8, 4, 9, 2, 4, 9, 3, 6, 4
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        // quality
        {
            RNG rng;
            Dist dist({ 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 });

            constexpr size_t samples = 50000;
            VX_CHECK((test::test_discrete_distribution<RNG, Dist, 10, samples>(rng, dist)));
        }
    }

    VX_SECTION("custom probabilities with zero weights")
    {
        // consistency
        {
            RNG rng;
            using type = typename Dist::result_type;
            Dist dist({ 0.0, 1.0, 0.0, 1.0 });

            constexpr size_t count = 20;
            const type expected[count] = {
                1, 3, 1, 1, 3, 3, 1, 3, 3, 3,
                1, 3, 1, 3, 1, 1, 3, 1, 1, 1
            };

            for (size_t i = 0; i < count; ++i)
            {
                const auto x = dist(rng);
                VX_CHECK(x == expected[i]);
            }
        }

        // quality
        {
            RNG rng;
            Dist dist({ 0.0, 1.0, 0.0, 1.0 });

            constexpr size_t samples = 50000;
            VX_CHECK((test::test_discrete_distribution<RNG, Dist, 4, samples>(rng, dist)));
        }
    }
}

VX_TEST_CASE(algorithms)
{

    VX_SECTION("shuffle")
    {
        constexpr size_t N = 6;
        int data[N]{};
        std::iota(std::begin(data), std::end(data), 0);

        // consistency
        {
            RNG rng;

            constexpr size_t count = 10;
            const int expected[count][N] = {
                { 4, 1, 2, 3, 5, 0},
                { 5, 2, 3, 1, 4, 0},
                { 4, 0, 1, 5, 3, 2},
                { 0, 1, 4, 2, 3, 5},
                { 0, 5, 3, 4, 2, 1},
                { 5, 0, 2, 4, 3, 1},
                { 5, 3, 2, 0, 1, 4},
                { 2, 4, 3, 1, 5, 0},
                { 3, 4, 0, 2, 5, 1},
                { 4, 1, 5, 0, 3, 2}
            };

            for (size_t i = 0; i < count; ++i)
            {
                int shuffled[N]{};
                std::memcpy(shuffled, data, N * sizeof(int));
                random::shuffle(std::begin(shuffled), std::end(shuffled), rng);
                VX_CHECK(std::equal(std::begin(shuffled), std::end(shuffled), std::begin(expected[i]), std::end(expected[i])));
            }
        }

        // quality
        {
            RNG rng;

            constexpr size_t samples = 100000;
            VX_CHECK((test::test_shuffle<RNG, 2, samples>(rng, data)));
            VX_CHECK((test::test_shuffle<RNG, 3, samples>(rng, data)));
            VX_CHECK((test::test_shuffle<RNG, 4, samples>(rng, data)));
            VX_CHECK((test::test_shuffle<RNG, 5, samples>(rng, data)));
            VX_CHECK((test::test_shuffle<RNG, 6, samples>(rng, data)));
        }
    }

    VX_SECTION("sample")
    {
        int data[20]{};
        std::iota(std::begin(data), std::end(data), 0);

        // consistency
        {
            RNG rng;

            constexpr size_t count = 20;
            const int expected[count] = {
                3,  1, 15, 16, 15,  1, 16,  6,  7, 12,
                6, 15, 14,  9,  8, 16, 14, 12, 13, 17
            };

            for (size_t i = 0; i < count; ++i)
            {
                int x{};
                random::sample(std::begin(data), std::end(data), &x, 1, rng);
                VX_CHECK(x == expected[i]);
            }
        }

        // quality
        {
            RNG rng;

            constexpr size_t samples = 100000;
            VX_CHECK((test::test_sample<RNG, 5, samples>(rng, data)));
            VX_CHECK((test::test_sample<RNG, 10, samples>(rng, data)));
            VX_CHECK((test::test_sample<RNG, 15, samples>(rng, data)));
            VX_CHECK((test::test_sample<RNG, 20, samples>(rng, data)));
        }
    }

    VX_SECTION("discrete sample")
    {
        int data[10]{};
        std::iota(std::begin(data), std::end(data), 0);

        // consistency
        {
            RNG rng;
            using Dist = random::discrete_distribution<>;
            Dist dist({ 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 });

            constexpr size_t count = 20;
            const int expected[count] = {
                2, 9, 2, 5, 7, 8, 6, 9, 7, 9,
                6, 8, 4, 9, 2, 4, 9, 3, 6, 4
            };

            for (size_t i = 0; i < count; ++i)
            {
                int x{};
                random::sample(std::begin(data), std::end(data), &x, 1, dist, rng);
                VX_CHECK(x == expected[i]);
            }
        }

        // quality
        {
            RNG rng;
            using Dist = random::discrete_distribution<>;
            Dist dist({ 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 });

            constexpr size_t samples = 100000;
            VX_CHECK((test::test_discrete_sample<RNG, Dist, 10, samples>(rng, dist, data)));
        }
    }
}

int main()
{
    VX_RUN_TESTS();
    return 0;
}