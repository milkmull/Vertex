#include <cassert>

#include "vertex/std/io.hpp"
#include "vertex/std/format.hpp"

using namespace vx;

auto test_string_format()
{
    auto result = fmt::format(
        "precision: {1} {0:10}",
        true,
        42);

    return result;
}

int main()
{
    auto out1 = test_string_format();
    print(out1);

    return 0;
}
