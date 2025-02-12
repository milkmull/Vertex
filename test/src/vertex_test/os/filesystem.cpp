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

const os::path nonexistent_paths[] = {
    "C:/This/Path/Should/Not/Exist",
    "//this_path_does_not_exist_on_the_network_e9da301701f70ead24c65bd30f600d15/docs",
};

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_current_path_and_temp_path)
{
    os::path current_path;
    os::path temp_path;

    VX_SECTION("current path")
    {
        VX_EXPECT_NO_ERROR(current_path = os::filesystem::get_current_path());
        VX_CHECK(os::filesystem::exists(current_path));
        VX_CHECK(os::filesystem::is_directory(current_path));
    }

    VX_SECTION("temp path")
    {
        VX_EXPECT_NO_ERROR(temp_path = os::filesystem::get_temp_path());
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

VX_TEST_CASE(test_create_file)
{
    const std::string filename = std::to_string(os::system_time().as_nanoseconds());
    const os::path file = filename + ".txt";

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

    VX_SECTION("create file in nonexistent directory")
    {
        for (const auto& p : nonexistent_paths)
        {
            const os::path fake_file = p / file;

            VX_CHECK(!os::filesystem::exists(fake_file));
            VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::create_file(fake_file));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_create_directory)
{
    const std::string filename = std::to_string(os::system_time().as_nanoseconds());
    const os::path directory = filename + ".dir";

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

    VX_SECTION("create directory in nonexistent directory")
    {
        for (const auto& p : nonexistent_paths)
        {
            const os::path fake_directory = p / directory;

            VX_CHECK(!os::filesystem::exists(fake_directory));
            VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::create_directory(fake_directory));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_create_directories)
{
    const std::string filename = std::to_string(os::system_time().as_nanoseconds());
    const os::path nested_directories = filename + ".dir/subdir1/subdir2";

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

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_create_symlink)
{
    const std::string filename = std::to_string(os::system_time().as_nanoseconds());

    VX_SECTION("create symlink")
    {
        const os::path file = filename + ".txt";
        const os::path symlink = filename + ".link";
        VX_CHECK(os::filesystem::create_file(file));

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

    VX_SECTION("create symlink to nonexistent file")
    {
        const os::path file = filename + "2.txt";
        const os::path symlink = filename + "2.link";
        VX_CHECK(!os::filesystem::exists(file));

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

    VX_SECTION("create symlink to malformed source")
    {
        const os::path symlink = filename + "3.link";

        for (const auto& p : nonexistent_paths)
        {
            VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::create_symlink(symlink, p));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_create_directory_symlink)
{
    const std::string filename = std::to_string(os::system_time().as_nanoseconds());

    VX_SECTION("create directory symlink")
    {
        const os::path directory = filename + ".dir";
        const os::path directory_symlink = filename + ".dirlink";
        VX_CHECK(os::filesystem::create_directory(directory));

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
        VX_CHECK(!directory_symlink_entry.is_directory());
        VX_CHECK(directory_symlink_entry.is_symlink());
        VX_CHECK(!directory_symlink_entry.is_other());
    }

    VX_SECTION("create directory symlink to nonexistent directory")
    {
        const os::path directory = filename + "2.dir";
        const os::path directory_symlink = filename + "2.dirlink";
        VX_CHECK(!os::filesystem::exists(directory));

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
        VX_CHECK(!directory_symlink_entry.is_directory());
        VX_CHECK(directory_symlink_entry.is_symlink());
        VX_CHECK(!directory_symlink_entry.is_other());
    }

    VX_SECTION("create directory symlink to malformed source")
    {
        const os::path file = filename + "3.txt";
        const os::path directory_symlink = filename + "4.dirlink";

        for (const auto& p : nonexistent_paths)
        {
            VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::create_directory_symlink(directory_symlink, p));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_absolute)
{
    VX_CHECK(os::filesystem::absolute({}) == os::path());

#if defined(VX_PLATFORM_WINDOWS)

    VX_CHECK(os::filesystem::absolute("x:/cat/dog/../elk") == "x:/cat/elk");

    const os::path current_path = os::filesystem::get_current_path();
    const os::path fake_file = "fake_file.txt";
    VX_CHECK(os::filesystem::absolute(fake_file) == current_path / fake_file);

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
        VX_CHECK_AND_EXPECT_ERROR(os::filesystem::absolute(long_path) != long_path);

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

// https://github.com/microsoft/STL/blob/fc15609a0f2ae2a134c34e7c9a13977994f37367/tests/std/tests/P0218R1_filesystem/test.cpp#L2313

VX_TEST_CASE(test_equivalent)
{
    const os::path directory = "equivalent.dir";
    VX_CHECK(os::filesystem::create_directory(directory));

    const os::path file1 = directory / "file1.txt";
    const os::path file2 = directory / "file2.txt";
    VX_CHECK(os::filesystem::create_file(file1));
    VX_CHECK(os::filesystem::create_file(file2));

    VX_SECTION("failure cases")
    {
        const os::path nonexistent = directory / "nonexistent.txt";

        VX_CHECK(!os::filesystem::equivalent("", ""));
        VX_CHECK(!os::filesystem::equivalent(nonexistent, nonexistent));
        VX_CHECK(!os::filesystem::equivalent(nonexistent, file1));
        VX_CHECK(!os::filesystem::equivalent(file1, nonexistent));
    }

    VX_SECTION("success cases")
    {
        VX_CHECK(os::filesystem::equivalent(file1, file1));
        VX_CHECK(os::filesystem::equivalent(file1, directory / ".." / directory.filename() / file1.filename()));
        VX_CHECK(!os::filesystem::equivalent(file1, file2));
        VX_CHECK(os::filesystem::equivalent(directory, directory));
        VX_CHECK(!os::filesystem::equivalent(directory, directory / ".."));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_canonical)
{
    VX_CHECK_AND_EXPECT_ERROR(os::filesystem::canonical("nonexistent.txt").empty());

    // test that canonical on a directory is not an error
    VX_CHECK(!os::filesystem::canonical(".").empty());

#if defined(VX_PLATFORM_WINDOWS)

    VX_SECTION("canonical DOS path")
    {
        // test that canonical on an ordinary file returns that file's DOS path
        const os::path file = "test_canonical.txt";
        VX_CHECK(os::filesystem::create_file(file));
        
        const os::path canonical_file = os::filesystem::canonical(file);
        VX_CHECK(!canonical_file.empty());

        const auto& filename = file.native();
        const auto& text = canonical_file.native();

        // make sure the result starts with "X:"
        VX_CHECK(text.size() > file.size() - 1);
        VX_CHECK(os::__detail::path_parser::is_letter(text[0]) && text[1] == L':');

        // make sure the result ends with "/filename"
        const auto diff = static_cast<ptrdiff_t>(file.size());
        VX_CHECK(*(text.end() - diff - 1) == L'\\');
        VX_CHECK(std::equal(text.end() - diff, text.end(), filename.begin(), filename.end()));
    }

#else

#endif // VX_PLATFORM_WINDOWS
}

int main()
{
    VX_RUN_TESTS();
    return 0;
}