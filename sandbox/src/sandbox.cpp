#define VX_ENABLE_PROFILING 1

#include "vertex/system/profiler.hpp"

#include "vertex/math/math.hpp"
#include "vertex/math/core/util.hpp"

#include "vertex/math/core/types/mat4x4.hpp"
#include "vertex/math/simd/mat4f.hpp"

#include <iostream>

using namespace vx;

static auto test_norm()
{
    math::mat4 m(
        2.0f, 3.0f, 4.0f, 5.0f,
        -1.0f, -21.0f, -3.0f, -4.0f,
        6.0f, 7.0f, 8.0f, 10.0f,
        -8.0f, -9.0f, -10.0f, -12.0f
    );

    math::mat4 x;

    {
        VX_PROFILE_SCOPE("norm");
        x = m * 5.0f;
    }

    //std::cout << x << std::endl;

    return x;
}

static auto test_simd()
{
    math::mat4 m(
        2.0f, 3.0f, 4.0f, 5.0f,
        -1.0f, -21.0f, -3.0f, -4.0f,
        6.0f, 7.0f, 8.0f, 10.0f,
        -8.0f, -9.0f, -10.0f, -12.0f
    );

    using simd_data_type = typename math::simd::mat4f::vec_type;

    math::mat4 x;
    auto simd_m = (simd_data_type*)(&m.columns[0].x);
    auto simd_x = (simd_data_type*)(&x.columns[0].x);

    {
        VX_PROFILE_SCOPE("simd");
        math::simd::mat4f::mul(simd_m, 5.0f, simd_x);
    }

    //std::cout << x << std::endl;

    math::vec4 v;

    constexpr auto z = sizeof(math::mat4);
    constexpr auto y = sizeof(math::mat4::simd_data_type);

    return x;
}

int main(int argc, char* argv[])
{
    VX_PROFILE_START("../../assets/simd_test.csv");

    for (int i = 0; i < 10000; ++i)
    {
        test_norm();
    }

    for (int i = 0; i < 10000; ++i)
    {
        test_simd();
    }
}
