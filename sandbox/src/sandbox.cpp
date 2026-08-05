#include <cassert>
#include <cstring>

#include "vertex/std/format.hpp"
#include "vertex/std/io.hpp"

using namespace vx;

int main()
{
    constexpr size_t size = 2048;
    char buffer[size] = {};

    const char fmt[] = "seth {:*>10} x {:e}";

    const auto res = fmt::format(
        buffer,
        size,
        fmt,
        mem::array_size(fmt),
        "sucks",
        0.123);

    buffer[res.count] = '\0';

    string_view s;
    float f;
    const auto res2 = fmt::scan(buffer, res.count, fmt, mem::array_size(fmt), s, f);

    io::print(buffer);

    return 0;
}
