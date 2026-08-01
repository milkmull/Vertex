#include <cassert>

#include "vertex/std/io.hpp"
#include "vertex/std/format.hpp"

using namespace vx;

int main()
{
    fmt::printf(
        "precision: {1} {0:10}",
        true,
        42);

    return 0;
}
