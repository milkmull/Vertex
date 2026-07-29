
#include "vertex/std/io.hpp"
#include "vertex/std/format.hpp"

//==============================================================================

int main()
{
    char buffer[256];

    auto result = vx::format::format(
        buffer,
        sizeof(buffer),
        "Hello, {}! {:>10} {:.5} {:*^12}",
        33,
        "world",
        "Vertex",
        "Formatting",
        static_cast<void*>(buffer));

    if (result.err != vx::format::format_error::none)
    {
        vx::println("Formatting failed");
        return 1;
    }

    vx::println_raw(buffer, result.count);

    return 0;
}
