#include <iostream>

#define VX_ENABLE_PROFILING 1
#include "vertex/system/profiler.hpp"

#include "vertex/math/math.hpp"
#include "vertex/math/core/util.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    math::mat4 m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    for (auto& x : m)
    {
        for (auto& y : x)
        {
            std::cout << y << std::endl;
        }
    }
}
