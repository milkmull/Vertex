#include <cassert>

#include "vertex/std/io.hpp"
#include "vertex/std/format.hpp"

using namespace vx;

int main()
{
    const char in[] = "precision: 42";
    const char fmt[] = "precision: {}";

    int value;

    fmt::scan(
        in, mem::array_size(in),
        fmt, mem::array_size(fmt),
        value);

    io::print(value);

    return 0;
}
