#include "vertex_test/test.hpp"
#include "vertex_test/util/random/random.hpp"

#include "vertex/util/random/pcg.hpp"
#include "vertex/util/random/uniform_int_distribution.hpp"

#include <random>

using namespace vx;

VX_TEST_CASE(pcg32)
{
    VX_SECTION("seeding")
    {
        constexpr size_t n = 20;
        typename random::pcg32::result_type results[n]{};

        ///////////////////////////////////////////////////////////////////////////////

        // These values are obtained from: https://github.com/imneme/pcg-cpp/blob/master/include/pcg_uint128.hpp
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

        // These values are obtained from: https://github.com/imneme/pcg-cpp/blob/master/include/pcg_uint128.hpp
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

        // These values are obtained from: https://github.com/imneme/pcg-cpp/blob/master/include/pcg_uint128.hpp
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

    VX_SECTION("min and max")
    {
        VX_CHECK(random::pcg32::min() == std::numeric_limits<uint32_t>::min());
        VX_CHECK(random::pcg32::max() == std::numeric_limits<uint32_t>::max());
    }
}

VX_TEST_CASE(uniform_int_distribution)
{
    VX_SECTION("Small Range (0-1) with 2 Bins using uint8_t")
    {
        random::pcg32 rng;
        random::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX - 1);
        VX_CHECK((test::test_uniform_distribution<decltype(rng), decltype(dist), 2>(rng, dist)));
    }

    //VX_SECTION("Small Range (-128 to 127) with 8 Bins using char")
    //{
    //    //random::pcg32 rng;
    //    //random::uniform_int_distribution<char> dist(0, 20);
    //    //VX_CHECK((test::test_uniform_distribution<decltype(rng), decltype(dist), 8>(rng, dist)));
    //    random::uniform_int_distribution<char> dist;
    //}
}

int main()
{
    VX_RUN_TESTS();
    return 0;
}