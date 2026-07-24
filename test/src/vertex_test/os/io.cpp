#include "vertex_test/test.hpp"
#include "vertex/os/io.hpp"
#include "vertex/std/memory.hpp"

using namespace vx;

//=============================================================================

VX_TEST_CASE(os_write_raw_stdout)
{
    const char* message = "vx test stdout\n";
    const size_t size = std::strlen(message);

    const size_t written = os::write_raw(
        os::stream::out,
        message,
        size);

    VX_CHECK(written == size);
}


VX_TEST_CASE(os_write_raw_stderr)
{
    const char* message = "vx test stderr\n";
    const size_t size = std::strlen(message);

    const size_t written = os::write_raw(
        os::stream::err,
        message,
        size);

    VX_CHECK(written == size);
}


VX_TEST_CASE(os_write_raw_zero_bytes)
{
    const char* message = "ignored";

    const size_t written = os::write_raw(
        os::stream::out,
        message,
        0);

    VX_CHECK(written == 0);
}


VX_TEST_CASE(os_write_raw_binary_data)
{
    const char data[] = {
        'A',
        '\0',
        'B',
        '\xff'
    };

    const size_t written = os::write_raw(
        os::stream::out,
        data,
        sizeof(data));

    VX_CHECK(written == sizeof(data));
}


VX_TEST_CASE(os_read_raw_invalid_size)
{
    const char* prompt = "Type 'hello': ";
    os::write_raw(
        os::stream::out,
        prompt,
        std::strlen(prompt));

    while (true)
    {
        char buffer[5] = {};

        const size_t read = os::read_raw(
            os::stream::in,
            buffer,
            sizeof(buffer));

        if (read == sizeof(buffer) &&
            std::memcmp(buffer, "hello", 5) == 0)
        {
            break;
        }

        const char* retry = "\nTry again: ";
        os::write_raw(
            os::stream::out,
            retry,
            std::strlen(retry));
    }
}

//=============================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
