#include "vertex/system/log.hpp"
#include "vertex/util/fixed_array.hpp"

#define VX_ENABLE_PROFILING
#include "vertex/system/profiler.hpp"

#include <vector>
#include <numeric>

using namespace vx;

static void vector_iteration_test()
{
    constexpr size_t count = 10'000;
    VX_PROFILE_FUNCTION();

    std::vector<int> data(count);
    std::iota(data.begin(), data.end(), 0);

    volatile int sum = 0;
    for (auto v : data)
        sum += v;
}

static void fixed_array_iteration_test()
{
    constexpr size_t count = 10'000;
    VX_PROFILE_FUNCTION();

    fixed_array<int> data(count);
    std::iota(data.begin(), data.end(), 0);

    volatile int sum = 0;
    for (auto v : data)
        sum += v;
}

int main(int argc, char* argv[])
{
    VX_PROFILE_START("../../assets/profile_results.txt");

    for (int i = 0; i < 10'000; ++i)
    {
        vector_iteration_test();
        fixed_array_iteration_test();
    }

    VX_PROFILE_STOP();

    return 0;
}
