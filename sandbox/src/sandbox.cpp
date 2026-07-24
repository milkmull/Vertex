
#include "vertex/std/io.hpp"
#include "vertex/std/format.hpp"

//==============================================================================

int main()
{
    char buffer[256];

    auto result = vx::format::format(
        buffer,
        sizeof(buffer),
        "Testing {:.e} boop",
        18, 1.0f);

    if (result.err != vx::format::format_error::none)
    {
        vx::println("Formatting failed");
        return 1;
    }

    vx::println_raw(buffer, result.count);

    return 0;
}
