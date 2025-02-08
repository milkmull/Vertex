#include "vertex_test/test.hpp"
#include "vertex/os/file.hpp"
#include "vertex/os/filesystem.hpp"

using namespace vx;

static bool create_file_containing(const os::path& filename, const char* text)
{
    return os::file::write_file(filename, reinterpret_cast<const uint8_t*>(text), std::strlen(text));
}

static const os::path nonexistent_paths[] = {
#if defined(VX_PLATFORM_WINDOWS)
    "C:/This/Path/Should/Not/Exist",
    "//this_path_does_not_exist_on_the_network_e9da301701f70ead24c65bd30f600d15/docs"
#else
    "//This/Path/Should/Not/Exist"
#endif // VX_PLATFORM_WINDOWS
};

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_current_path_and_temp_path)
{
    os::path current_path;
    os::path temp_path;

    VX_SECTION("current path")
    {
        current_path = os::filesystem::get_current_path();
        VX_NO_ERROR();
        VX_CHECK(os::filesystem::exists(current_path));
        VX_CHECK(os::filesystem::is_directory(current_path));
    }

    VX_SECTION("temp path")
    {
        temp_path = os::filesystem::get_temp_path();
        VX_NO_ERROR();
        VX_CHECK(os::filesystem::exists(temp_path));
        VX_CHECK(os::filesystem::is_directory(temp_path));
    }

    VX_SECTION("set current path")
    {
        VX_CHECK(os::filesystem::set_current_path(temp_path));
        VX_CHECK(os::filesystem::get_current_path() == temp_path.parent_path());
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_directory_entry)
{
    const os::path file = "test_directory_entry.txt";
    const os::path file2 = "test_directory_entry2.txt";
    const os::path directory = "test_directory_entry.dir";
    const os::path symlink = "test_directory_entry.link";
    const os::path cache = "test_directory_entry_cache.txt";

    VX_CHECK(create_file_containing(file, "hello world"));
    VX_CHECK(create_file_containing(file, "goodbye world"));
    VX_CHECK(os::filesystem::create_directory(directory));

    // Check default state
    os::filesystem::directory_entry default_entry;
    default_entry.refresh();
    VX_CHECK(default_entry.path.empty());
    VX_CHECK(!default_entry.exists());

    // Check nonexistent entries
    for (const auto& p : nonexistent_paths)
    {
        os::filesystem::directory_entry entry{ p };
        entry.refresh();
        VX_CHECK(!entry.exists());
    }

    // regular file
    {
        os::filesystem::directory_entry file_entry{ file };
        file_entry.refresh();

        VX_CHECK(file_entry.info.type == os::filesystem::file_type::REGULAR);
        VX_CHECK(file_entry.info.permissions == os::filesystem::file_permissions::ALL_READ_WRITE);
        VX_CHECK(file_entry.info.size != 0);
        VX_CHECK(file_entry.info.create_time.as_nanoseconds() != 0);
        VX_CHECK(file_entry.info.modify_time.as_nanoseconds() != 0);

        VX_CHECK(file_entry.exists());
        VX_CHECK(file_entry.is_regular_file());
        VX_CHECK(!file_entry.is_directory());
        VX_CHECK(!file_entry.is_symlink());
        VX_CHECK(!file_entry.is_other());
    }

    // directory
    {
        os::filesystem::directory_entry directory_entry{ directory };
        directory_entry.refresh();

        VX_CHECK(directory_entry.info.type == os::filesystem::file_type::DIRECTORY);
        VX_CHECK(directory_entry.info.permissions == os::filesystem::file_permissions::ALL_READ_WRITE);
        VX_CHECK(directory_entry.info.size == 0);
        VX_CHECK(directory_entry.info.create_time.as_nanoseconds() != 0);
        VX_CHECK(directory_entry.info.modify_time.as_nanoseconds() != 0);

        VX_CHECK(directory_entry.exists());
        VX_CHECK(!directory_entry.is_regular_file());
        VX_CHECK(directory_entry.is_directory());
        VX_CHECK(!directory_entry.is_symlink());
        VX_CHECK(!directory_entry.is_other());
    }

    // symlink
    {
        VX_CHECK(os::filesystem::create_directory_symlink(directory, symlink));

        os::filesystem::directory_entry symlink_entry{ symlink };
        symlink_entry.refresh();

        VX_CHECK(symlink_entry.info.type == os::filesystem::file_type::SYMLINK);
        VX_CHECK(symlink_entry.info.permissions == os::filesystem::file_permissions::ALL_READ_WRITE);
        VX_CHECK(symlink_entry.info.size == 0);
        VX_CHECK(symlink_entry.info.create_time.as_nanoseconds() != 0);
        VX_CHECK(symlink_entry.info.modify_time.as_nanoseconds() != 0);

        VX_CHECK(symlink_entry.exists());
        VX_CHECK(!symlink_entry.is_regular_file());
        VX_CHECK(!symlink_entry.is_directory());
        VX_CHECK(symlink_entry.is_symlink());
        VX_CHECK(!symlink_entry.is_other());
    }
}

int main()
{
    VX_RUN_TESTS();
    return 0;
}