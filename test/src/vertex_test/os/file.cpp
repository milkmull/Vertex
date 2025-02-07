#include "vertex_test/test.hpp"
#include "vertex/os/file.hpp"
#include "vertex/os/filesystem.hpp" // only for temp path

using namespace vx;

static const os::path temp_path{ os::filesystem::get_temp_path() };

static std::string current_time_file()
{
    const time::time_point now = os::system_time();
    std::string filename = std::to_string(now.as_nanoseconds());
    filename.append(".txt");
    return filename;
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(file)
{
    const os::path filename = current_time_file();

    VX_SECTION("exists")
    {
        VX_CHECK(!os::file::exists(temp_path / filename));
    }

    VX_SECTION("create")
    {
        VX_CHECK(os::file::create(temp_path / filename));
        VX_CHECK(os::file::exists(temp_path / filename));
    }

    VX_SECTION("read_write_file")
    {
        const os::path temp_file = current_time_file();
        const char* expected = "Hello World!";

        VX_CHECK(os::file::write_text_file(temp_file, expected));

        std::string text;
        VX_CHECK(os::file::read_text_file(temp_file, text));
        VX_CHECK(text == expected);
    }
}

int main()
{
    VX_RUN_TESTS();
    return 0;
}