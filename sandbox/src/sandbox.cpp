
#include "vertex/std/io.hpp"

#define VX_FORMAT_DISABLE_RUNTIME_FORMAT_CHECKS
#define VX_FORMAT_DISABLE_RUNTIME_SIZE_CHECKS

#include "vertex/std/format.hpp"

//==============================================================================

int main()
{
    char buffer1[256];
    char buffer2[256];

    const char fmt[] = "precision: {{:.{}}}";

    auto result = vx::fmt::format(
        buffer1,
        vx::mem::array_size(buffer1),
        fmt,
        vx::mem::array_size(fmt),
        10);

    result = vx::fmt::format(
        buffer2,
        vx::mem::array_size(buffer2),
        buffer1,
        result.count,
        0.123);

    if (result.err != vx::fmt::format_error::none)
    {
        vx::println("Formatting failed");
        return 1;
    }

    vx::println_raw(buffer2, result.count);

    return 0;
}
