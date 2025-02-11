#include "vertex_test/test.hpp"
#include "vertex/os/file.hpp"
#include "vertex/os/filesystem.hpp"

using namespace vx;

/*

- get/set current path
- temp directory
- user folders

- creation and info (file, directory, symlink, directory symlink, directories)

- absolute
- relative
- equivalent

- read symlink
- update permissions

- directory iterators

- copy
- remove
- rename

- space
- is_empty

*/

static bool create_file_containing(const os::path& filename, const char* text)
{
    return os::file::write_file(filename, reinterpret_cast<const uint8_t*>(text), std::strlen(text));
}

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

VX_TEST_CASE(test_user_folders)
{
    std::pair<os::filesystem::user_folder, const char*> folders[] = {
        { os::filesystem::user_folder::HOME, "Home" },
        { os::filesystem::user_folder::DESKTOP, "Desktop" },
        { os::filesystem::user_folder::DOCUMENTS, "Documents" },
        { os::filesystem::user_folder::DOWNLOADS, "Downloads" },
        { os::filesystem::user_folder::MUSIC, "Music" },
        { os::filesystem::user_folder::PICTURES, "Pictures" },
        { os::filesystem::user_folder::VIDEOS, "Videos" }
    };

    for (const auto& pair : folders)
    {
        const os::path folder = os::filesystem::get_user_folder(pair.first);

        if (!folder.empty())
        {
            VX_MESSAGE("  User Folder ", pair.second, ": ", folder);
        }
        else
        {
            VX_WARNING("  User Folder ", pair.second, ": not found");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_default_file_info)
{
    os::filesystem::file_info info{};

    VX_CHECK(info.type == os::filesystem::file_type::NONE);
    VX_CHECK(info.permissions == os::filesystem::file_permissions::NONE);
    VX_CHECK(info.size == 0);
    VX_CHECK(info.create_time.as_nanoseconds() == 0);
    VX_CHECK(info.modify_time.as_nanoseconds() == 0);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_create)
{
    const std::string filename = std::to_string(os::system_time().as_nanoseconds());

    const os::path file = filename + ".txt";
    const os::path directory = filename + ".dir";
    const os::path symlink = filename + ".link";
    const os::path directory_symlink = filename + ".dirlink";
    const os::path nested_directories = filename + ".dir/subdir1/subdir2";

    VX_SECTION("create file")
    {
        VX_CHECK(!os::filesystem::exists(file));
        VX_CHECK(os::filesystem::create_file(file));

        os::filesystem::directory_entry file_entry{ file };
        file_entry.refresh();

        VX_CHECK(file_entry.info.type == os::filesystem::file_type::REGULAR);
        VX_CHECK(file_entry.info.permissions == os::filesystem::file_permissions::ALL_READ_WRITE);
        VX_CHECK(file_entry.info.create_time.as_nanoseconds() != 0);
        VX_CHECK(file_entry.info.modify_time.as_nanoseconds() != 0);

        VX_CHECK(file_entry.exists());
        VX_CHECK(file_entry.is_regular_file());
        VX_CHECK(!file_entry.is_directory());
        VX_CHECK(!file_entry.is_symlink());
        VX_CHECK(!file_entry.is_other());
    }

    VX_SECTION("create directory")
    {
        VX_CHECK(!os::filesystem::exists(directory));
        VX_CHECK(os::filesystem::create_directory(directory));

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

    VX_SECTION("create symlink")
    {
        VX_CHECK(!os::filesystem::exists(symlink));
        VX_CHECK(os::filesystem::create_symlink(file, symlink));

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

    VX_SECTION("create directory symlink")
    {
        VX_CHECK(!os::filesystem::exists(directory_symlink));
        VX_CHECK(os::filesystem::create_directory_symlink(directory, directory_symlink));

        os::filesystem::directory_entry directory_symlink_entry{ directory_symlink };
        directory_symlink_entry.refresh();

        VX_CHECK(directory_symlink_entry.info.type == os::filesystem::file_type::SYMLINK);
        VX_CHECK(directory_symlink_entry.info.permissions == os::filesystem::file_permissions::ALL_READ_WRITE);
        VX_CHECK(directory_symlink_entry.info.size == 0);
        VX_CHECK(directory_symlink_entry.info.create_time.as_nanoseconds() != 0);
        VX_CHECK(directory_symlink_entry.info.modify_time.as_nanoseconds() != 0);

        VX_CHECK(directory_symlink_entry.exists());
        VX_CHECK(!directory_symlink_entry.is_regular_file());
        VX_CHECK(!directory_symlink_entry.is_directory()); // directory symlinks should only be symlink and not directory
        VX_CHECK(directory_symlink_entry.is_symlink());
        VX_CHECK(!directory_symlink_entry.is_other());
    }

    VX_SECTION("create directories")
    {
        VX_CHECK(!os::filesystem::exists(nested_directories));
        VX_CHECK(os::filesystem::create_directories(nested_directories));

        os::filesystem::directory_entry nested_entry{ nested_directories };
        nested_entry.refresh();

        VX_CHECK(nested_entry.exists());
        VX_CHECK(nested_entry.is_directory());
        VX_CHECK(!nested_entry.is_regular_file());
        VX_CHECK(!nested_entry.is_symlink());
        VX_CHECK(!nested_entry.is_other());
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_absolute)
{
    VX_CHECK(os::filesystem::absolute({}) == os::path());

#if defined(VX_PLATFORM_WINDOWS)

    VX_CHECK(os::filesystem::absolute("x:/cat/dog/../elk") == "x:/cat/elk");

    VX_SECTION("windows long path")
    {
        // On windows MAX_PATH is 260
        constexpr size_t MAX_PATH = 260;

        std::wstring long_path = LR"(\\?\x:\some\)";
        long_path.resize(MAX_PATH + 10, L'a');
        VX_CHECK(long_path.size() > MAX_PATH);
        VX_CHECK(os::filesystem::absolute(long_path) == long_path);

        long_path.push_back(L'\\');
        long_path.resize(40000, L'b');
        // Path should be way too long
        VX_CHECK_ERROR(
            os::filesystem::absolute(long_path) == long_path,
            err::PLATFORM_ERROR
        );

        long_path.resize(MAX_PATH);
        VX_CHECK(os::filesystem::absolute(long_path) == long_path);
    }

#else

#endif // VX_PLATFORM_WINDOWS
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_relative)
{
    const os::path directory = "test_relative/a/b/c";
    const os::path file1 = "test_relative/a/b/c/file1.txt";
    const os::path file2 = "test_relative/a/b/x/y/file2.txt";

    VX_CHECK(os::filesystem::relative("", "") == ".");
    VX_CHECK(os::filesystem::relative(file2, file1) == "../../x/y/file2.txt");
    VX_CHECK(os::filesystem::relative(file1, file2) == "../../../c/file1.txt");
    VX_CHECK(os::filesystem::relative(file1) == file1);
    VX_CHECK(os::filesystem::relative(file2) == file2);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_equivalent)
{
    VX_CHECK(os::filesystem::equivalent("", ""));
    VX_CHECK(os::filesystem::equivalent("file1", "file1"));
    VX_CHECK(!os::filesystem::equivalent("file1", "file2"));
    VX_CHECK(!os::filesystem::equivalent("C:file1", "D:file1"));
}

int main()
{
    VX_RUN_TESTS();
    return 0;
}