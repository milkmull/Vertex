#include <unordered_set>

#include "vertex_test/test.hpp"
#include "vertex/os/file.hpp"
#include "vertex/os/filesystem.hpp"
#include "vertex/util/memory/memory.hpp"

using namespace vx;

static const os::path nonexistent_paths[] = {
    "C:/This/Path/Should/Not/Exist",
    "//this_path_does_not_exist_on_the_network_e9da301701f70ead24c65bd30f600d15/docs",
};

static const os::path bad_path = "// ?? ?? ///// ?? ?? ? ////";

struct temp_directory
{
    explicit temp_directory(const str::str_arg_t& name) : path(name)
    {
        err::clear();
        os::filesystem::remove_all(path);
        if (err::get())
        {
            VX_WARNING("failed to clean up ", name, " before test");
        }
        else if (!os::filesystem::create_directories(path))
        {
            VX_WARNING("failed to create directoy ", path, " before test");
        }
    }

    temp_directory(const temp_directory&) = delete;
    temp_directory& operator=(temp_directory&) = delete;

    ~temp_directory() noexcept
    {
        err::clear();
        os::filesystem::remove_all(path);
        if (err::get())
        {
            VX_WARNING("failed to clean up ", path, " after test");
        }
    }

    bool exists() const
    {
        return os::filesystem::exists(path);
    }

    os::path path;
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
            VX_WARNING("User Folder ", pair.second, ": not found");
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

VX_TEST_CASE(test_create_directory)
{
    const os::path directory = "create_directory.dir";
    VX_EXPECT_NO_ERROR(os::filesystem::remove(directory));

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

    VX_EXPECT_NO_ERROR(os::filesystem::remove(directory));
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_create_directories_and_remove_all)
{
    temp_directory temp_dir("create_directories_and_remove_all.dir");
    VX_CHECK(temp_dir.exists());

    const os::path directories = temp_dir.path / "a/b/c/d/e/f/g/h/i/j";

    VX_SECTION("create and remove")
    {
        VX_CHECK(os::filesystem::create_directories(directories));
        VX_CHECK(os::filesystem::exists(directories));

        VX_CHECK_AND_EXPECT_NO_ERROR(os::filesystem::remove_all(directories) > 0);
        VX_CHECK_AND_EXPECT_NO_ERROR(os::filesystem::remove_all(directories) == 0);
    }

    VX_SECTION("bad path")
    {
        VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::create_directories(bad_path));
        VX_CHECK_AND_EXPECT_NO_ERROR(os::filesystem::remove_all(bad_path) == 0);
    }

    VX_SECTION("create directory empty path")
    {
        VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::create_directories(os::path{}));
    }

    VX_SECTION("create directories with dots")
    {
        const os::path p = temp_dir.path / "a/../b/../c";

        VX_CHECK(os::filesystem::create_directories(p));
        VX_CHECK(os::filesystem::exists(temp_dir.path / "a"));
        VX_CHECK(os::filesystem::exists(temp_dir.path / "b"));
        VX_CHECK(os::filesystem::exists(temp_dir.path / "c"));
    }

#if defined(VX_PLATFORM_WINDOWS)

    VX_SECTION("long path support")
    {
        // The long path prefix ("\\?\") requires the path to be absolute
        const os::path long_root = R"(\\?\)" + os::filesystem::absolute(temp_dir.path).string();
        const os::path long_full = long_root / std::string(255, 'a');

        VX_CHECK(os::filesystem::create_directories(long_full));
        VX_CHECK_AND_EXPECT_NO_ERROR(os::filesystem::remove(long_full));
        VX_CHECK(!os::filesystem::exists(long_full));
    }

#endif // VX_PLATFORM_WINDOWS

    VX_SECTION("create directory over file")
    {
        // Try to create a sub directory where one of the parents is already a file
        const os::path file = temp_dir.path / "file.txt";
        VX_CHECK(os::filesystem::create_file(file));

        const os::path sub = file / "sub.dir";
        VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::create_directories(sub));

        // Try to create a sub directory where the last directory is already a file
        VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::create_directories(file));
    }

}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_create_file)
{
    temp_directory temp_dir("test_create_file.dir");
    VX_CHECK(temp_dir.exists());
    const os::path file = temp_dir.path / "file.txt";

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

VX_TEST_CASE(test_create_symlink)
{
    temp_directory temp_dir("test_create_symlink.dir");
    VX_CHECK(temp_dir.exists());

    VX_SECTION("create symlink")
    {
        const os::path file = temp_dir.path / "symlink.txt";
        const os::path symlink = temp_dir.path / "symlink.link";

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
        
        // creating a symlink that exists already should throw
        VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::create_symlink(file, symlink));
    }

    VX_SECTION("create symlink to nonexistent file")
    {
        const os::path file = temp_dir.path / "nonexistent_file.txt";
        const os::path symlink = temp_dir.path / "symlink_to_nonexistent_file.link";

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
        const os::path symlink = temp_dir.path / "symlink_to_malformed_source.link";

        for (const auto& p : nonexistent_paths)
        {
            VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::create_symlink(symlink, p));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_create_directory_symlink)
{
    temp_directory temp_dir("test_create_directory_symlink.dir");
    VX_CHECK(temp_dir.exists());

    VX_SECTION("create directory symlink")
    {
        const os::path directory = temp_dir.path / "directory_target.dir";
        const os::path directory_symlink = temp_dir.path / "directory_symlink.dirlink";

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

        // creating a symlink that exists already should throw
        VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::create_symlink(directory, directory_symlink));
    }

    VX_SECTION("create directory symlink to nonexistent directory")
    {
        const os::path directory = temp_dir.path / "nonexistent_directory.dir";
        const os::path directory_symlink = temp_dir.path / "directory_symlink_to_nonexistent_directory.dirlink";

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
        const os::path directory_symlink = temp_dir.path / "directory_symlink_to_malformed_source.dirlink";

        for (const auto& p : nonexistent_paths)
        {
            VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::create_directory_symlink(directory_symlink, p));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_create_hard_link)
{
    temp_directory temp_dir("test_create_hard_link.dir");
    VX_CHECK(temp_dir.exists());

    const os::path file = temp_dir.path / "file.txt";
    const os::path hard_link = temp_dir.path / "file.hlink";

    VX_CHECK(os::filesystem::create_file(file));
    VX_CHECK(os::filesystem::create_hard_link(file, hard_link));
    VX_CHECK(os::filesystem::equivalent(file, hard_link));

    // creating hard link to self should fail
    VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::create_hard_link(file, file));
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_hard_link_count)
{
    temp_directory temp_dir("test_hard_link_count.dir");
    VX_CHECK(temp_dir.exists());

    const os::path file = temp_dir.path / "file.txt";
    const os::path link1 = temp_dir.path / "link1.hlink";
    const os::path link2 = temp_dir.path / "link2.hlink";

    VX_CHECK_AND_EXPECT_ERROR(os::filesystem::hard_link_count(file) == 0);

    VX_CHECK(os::filesystem::create_file(file));
    VX_CHECK(os::filesystem::hard_link_count(file) == 1);

    VX_CHECK(os::filesystem::create_hard_link(file, link1));
    VX_CHECK(os::filesystem::hard_link_count(link1) == 2);

    VX_CHECK(os::filesystem::create_hard_link(link1, link2));
    VX_CHECK(os::filesystem::hard_link_count(link2) == 3);

    VX_CHECK(os::filesystem::remove(link2));
    VX_CHECK(os::filesystem::hard_link_count(link1) == 2);

    VX_CHECK(os::filesystem::remove(link1));
    VX_CHECK(os::filesystem::hard_link_count(file) == 1);

    VX_CHECK(os::filesystem::remove(file));

    VX_CHECK_AND_EXPECT_ERROR(os::filesystem::hard_link_count(file) == 0);
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_modify_time)
{
    temp_directory temp_dir("test_create_hard_link.dir");
    VX_CHECK(temp_dir.exists());

    VX_SECTION("existing file")
    {
        const os::path file = temp_dir.path / "file.txt";
        VX_CHECK(os::filesystem::create_file(file));

        time::time_point modify_time = os::filesystem::get_modify_time(file);
        const time::time_point now = os::system_time();
        VX_CHECK(now - time::minutes(30) <= modify_time && modify_time <= now + time::minutes(30));

        const time::time_point test_time = now;
        VX_CHECK(os::filesystem::set_modify_time(file, test_time));
        VX_CHECK(os::filesystem::get_modify_time(file) == test_time);
    }

    VX_SECTION("existing directory")
    {
        const os::path directory = temp_dir.path / "directory.dir";
        VX_CHECK(os::filesystem::create_directory(directory));

        time::time_point modify_time = os::filesystem::get_modify_time(directory);
        const time::time_point now = os::system_time();
        VX_CHECK(now - time::minutes(30) <= modify_time && modify_time <= now + time::minutes(30));

        const time::time_point test_time = now;
        VX_CHECK(os::filesystem::set_modify_time(directory, test_time));
        VX_CHECK(os::filesystem::get_modify_time(directory) == test_time);
    }

    VX_SECTION("nonexistent path")
    {
        VX_CHECK_AND_EXPECT_ERROR(os::filesystem::get_modify_time(os::path{}).as_nanoseconds() == 0);
        VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::set_modify_time(os::path{}, time::time_point{1}));

        for (const auto& p : nonexistent_paths)
        {
            VX_CHECK_AND_EXPECT_ERROR(os::filesystem::get_modify_time(p).as_nanoseconds() == 0);
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

        os::path long_path = R"(\\?\x:\some\)";
        long_path /= std::string(255, 'a');
        VX_CHECK(long_path.size() > MAX_PATH);
        VX_CHECK(os::filesystem::absolute(long_path) == long_path);

        // Path should be way too long
        long_path /= std::string(40000, 'b');
        VX_CHECK_AND_EXPECT_ERROR(os::filesystem::absolute(long_path) != long_path);

        long_path = os::path(long_path.c_str(), long_path.c_str() + MAX_PATH);
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
    temp_directory temp_dir("test_equivalent.dir");
    VX_CHECK(temp_dir.exists());

    const os::path file1 = temp_dir.path / "file1.txt";
    const os::path file2 = temp_dir.path / "file2.txt";
    VX_CHECK(os::filesystem::create_file(file1));
    VX_CHECK(os::filesystem::create_file(file2));

    VX_SECTION("failure cases")
    {
        const os::path nonexistent = temp_dir.path / "nonexistent.txt";

        VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::equivalent("", ""));
        VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::equivalent(nonexistent, nonexistent));
        VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::equivalent(nonexistent, file1));
        VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::equivalent(file1, nonexistent));
    }

    VX_SECTION("success cases")
    {
        VX_CHECK(os::filesystem::equivalent(file1, file1));
        VX_CHECK(!os::filesystem::equivalent(file1, file2));
        VX_CHECK(os::filesystem::equivalent(temp_dir.path, temp_dir.path));
        VX_CHECK(!os::filesystem::equivalent(temp_dir.path, temp_dir.path / ".."));

        const os::path same_as_file1 = temp_dir.path / "./.././." / temp_dir.path.filename() / file1.filename();
        VX_CHECK(os::filesystem::equivalent(file1, same_as_file1));
    }

    VX_SECTION("symlink resolution")
    {
        const os::path symlink = temp_dir.path / "test_equivalent.link";
        const os::path target = os::filesystem::absolute(file1) / os::path(".///////..////..\\\\\\//") / temp_dir.path / file1.filename();

        VX_CHECK(os::filesystem::create_symlink(target, symlink));
        VX_CHECK(os::filesystem::exists(symlink));

        VX_CHECK(os::filesystem::equivalent(symlink, symlink));
        VX_CHECK(os::filesystem::equivalent(file1, symlink));
        VX_CHECK(!os::filesystem::equivalent(symlink, file2));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_canonical)
{
    temp_directory temp_dir("test_canonical.dir");
    VX_CHECK(temp_dir.exists());

    VX_SECTION("nonexistent file")
    {
        VX_CHECK_AND_EXPECT_ERROR(os::filesystem::canonical("nonexistent.txt").empty());
    }

    VX_SECTION("directory")
    {
        // test that canonical on a directory is not an error
        VX_CHECK(!os::filesystem::canonical(".").empty());
    }

#if defined(VX_PLATFORM_WINDOWS)

    VX_SECTION("canonical DOS path")
    {
        // test that canonical on an ordinary file returns that file's DOS path
        const os::path file = temp_dir.path / "test_canonical.txt";
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

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_read_symlink)
{
    temp_directory temp_dir("test_read_symlink.dir");
    VX_CHECK(temp_dir.exists());

    const os::path file = temp_dir.path / "file.txt";
    VX_CHECK(os::filesystem::create_file(file));

    VX_SECTION("success")
    {
        const os::path symlink = temp_dir.path / "symlink.link";
        VX_CHECK(os::filesystem::create_symlink(file.filename(), symlink));
        VX_CHECK(os::filesystem::equivalent(file, symlink));

        const os::path read_symlink = os::filesystem::read_symlink(symlink);
        VX_CHECK(file.filename() == read_symlink);
    }

    VX_SECTION("failure")
    {
        // file is not a symlink
        VX_CHECK_AND_EXPECT_ERROR(os::filesystem::read_symlink(file).empty());
        // file does not exist
        const os::path fake_symlink = "fake_symlink.link";
        VX_CHECK_AND_EXPECT_ERROR(os::filesystem::read_symlink(fake_symlink).empty());
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_update_permissions)
{
    temp_directory temp_dir("test_update_permissions.dir");
    VX_CHECK(temp_dir.exists());

#if defined(VX_PLATFORM_WINDOWS)

    // https://github.com/microsoft/STL/blob/fc15609a0f2ae2a134c34e7c9a13977994f37367/tests/std/tests/P0218R1_filesystem/test.cpp#L3483

    VX_SECTION("directory")
    {
        const os::path directory = os::filesystem::get_temp_path();
        VX_CHECK(!directory.empty());
        VX_CHECK(os::filesystem::get_file_info(directory).permissions == os::filesystem::file_permissions::ALL_READ_WRITE);
    }

    VX_SECTION("file")
    {
        const os::path file = temp_dir.path / "file.txt";
        VX_CHECK(os::filesystem::create_file(file));

        VX_CHECK(os::filesystem::update_permissions(file, os::filesystem::file_permissions::ALL_READ_WRITE));

        // should have all read and write permissions
        VX_CHECK(os::filesystem::get_file_info(file).permissions == os::filesystem::file_permissions::ALL_READ_WRITE);

        // has no effect because some write bits still set
        VX_CHECK(os::filesystem::update_permissions(file, os::filesystem::file_permissions::OWNER_WRITE));
        VX_CHECK(os::filesystem::get_file_info(file).permissions == os::filesystem::file_permissions::ALL_READ_WRITE);

        // make file read only
        VX_CHECK(os::filesystem::update_permissions(file, os::filesystem::file_permissions::ALL_READ, os::filesystem::file_permission_operator::REPLACE));
        VX_CHECK(os::filesystem::get_file_info(file).permissions == os::filesystem::file_permissions::ALL_READ);

        // adding any write bit removes FILE_ATTRIBUTE_READONLY
        VX_CHECK(os::filesystem::update_permissions(file, os::filesystem::file_permissions::OWNER_WRITE, os::filesystem::file_permission_operator::ADD));
        VX_CHECK(os::filesystem::get_file_info(file).permissions == os::filesystem::file_permissions::ALL_READ_WRITE);

        // removing any write bit but not all has no effect
        VX_CHECK(os::filesystem::update_permissions(file, os::filesystem::file_permissions::OWNER_WRITE, os::filesystem::file_permission_operator::REMOVE));
        VX_CHECK(os::filesystem::get_file_info(file).permissions == os::filesystem::file_permissions::ALL_READ_WRITE);

        // removing all write bits sets FILE_ATTRIBUTE_READONLY
        VX_CHECK(os::filesystem::update_permissions(file, os::filesystem::file_permissions::ALL_WRITE, os::filesystem::file_permission_operator::REMOVE));
        VX_CHECK(os::filesystem::get_file_info(file).permissions == os::filesystem::file_permissions::ALL_READ);
    }

#else



#endif // VX_PLATFORM_WINDOWS
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_directory_iterator)
{
    temp_directory temp_dir("test_directory_iterator.dir");
    VX_CHECK(temp_dir.exists());

    static_assert(std::is_same<typename os::filesystem::directory_iterator::iterator_category, std::input_iterator_tag>::value);
    static_assert(std::is_same<typename os::filesystem::directory_iterator::value_type, os::filesystem::directory_entry>::value);
    static_assert(std::is_same<typename os::filesystem::directory_iterator::difference_type, ptrdiff_t>::value);
    static_assert(std::is_same<typename os::filesystem::directory_iterator::pointer, const os::filesystem::directory_entry*>::value);
    static_assert(std::is_same<typename os::filesystem::directory_iterator::reference, const os::filesystem::directory_entry&>::value);

    VX_SECTION("default constructor")
    {
        os::filesystem::directory_iterator it;
    }

    VX_SECTION("nonexistent path")
    {
        for (const auto& p : nonexistent_paths)
        {
            VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::directory_iterator(p).is_valid());
        }
    }

#if defined(VX_PLATFORM_WINDOWS)

    VX_SECTION("edge cases")
    {
        // Test VSO-844835 "directory_iterator constructed with empty path iterates over the current directory"
        VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::directory_iterator(os::path{}).is_valid());
        // Test VSO-583725 "recursive_directory_iterator blows up (memory leak + infinite loop) with embedded nulls"
        VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::directory_iterator(std::wstring(L".\0", 2)).is_valid());
    }

#endif // VX_PLATFORM_WINDOWS

    VX_SECTION("simple test")
    {
        const os::path file = temp_dir.path / "file.txt";
        VX_CHECK(os::filesystem::create_file(file));

        os::filesystem::directory_iterator it(temp_dir.path);
        VX_CHECK(it.is_valid());
        VX_CHECK(it != os::filesystem::directory_iterator{});

        VX_CHECK(it == it);
        VX_CHECK(!(it != it));

        const auto& first_entry = *it;
        VX_CHECK(first_entry.path == it->path);

        bool found_directory = false;
        do
        {
            found_directory = found_directory || (it->path == file);
            VX_EXPECT_NO_ERROR(++it);

        } while (it != os::filesystem::directory_iterator{});

        VX_CHECK(found_directory);
    }

    VX_SECTION("copy and assignment")
    {
        const os::filesystem::directory_iterator it1(temp_dir.path);
        VX_CHECK(it1.is_valid());
        VX_CHECK(it1 != os::filesystem::directory_iterator{});

        const os::filesystem::directory_iterator it2(it1);
        VX_CHECK(it2.is_valid());
        VX_CHECK(it2 != os::filesystem::directory_iterator{});

        VX_CHECK(it1 == it2);
        VX_CHECK(!(it1 != it2));

        os::filesystem::directory_iterator it3;
        it3 = it2;
        VX_CHECK(it3.is_valid());
        VX_CHECK(it3 != os::filesystem::directory_iterator{});

        VX_CHECK(it2 == it3);
        VX_CHECK(!(it2 != it3));

        // self assignment
        it3 = it3;
        VX_CHECK(it1 == it3);
    }

    VX_SECTION("move")
    {
        os::filesystem::directory_iterator it1(temp_dir.path);
        VX_CHECK(it1.is_valid());
        VX_CHECK(it1 != os::filesystem::directory_iterator{});
        const os::path first_entry = it1->path;

        VX_DISABLE_WARNING("", 26800); // disable use after move warning
        VX_DISABLE_WARNING_PUSH();

        os::filesystem::directory_iterator it2(std::move(it1));
        VX_CHECK(it2.is_valid());
        VX_CHECK(!it1.is_valid());
        VX_CHECK(it2 != os::filesystem::directory_iterator{});
        VX_CHECK(it1 == os::filesystem::directory_iterator{});
        VX_CHECK(first_entry == it2->path);

        os::filesystem::directory_iterator it3;
        it3 = std::move(it2);
        VX_CHECK(it3.is_valid());
        VX_CHECK(!it2.is_valid());
        VX_CHECK(it3 != os::filesystem::directory_iterator{});
        VX_CHECK(it2 == os::filesystem::directory_iterator{});
        VX_CHECK(first_entry == it3->path);

        // self move assignment
        it3 = std::move(it3);
        VX_CHECK(it3.is_valid());
        VX_CHECK(it3 != os::filesystem::directory_iterator{});

        VX_DISABLE_WARNING_POP();
    }

    VX_SECTION("operator++")
    {
        os::path file_a = temp_dir.path / "a.txt";
        os::path file_b = temp_dir.path / "b.txt";

        VX_CHECK(os::filesystem::create_file(file_a));
        VX_CHECK(os::filesystem::create_file(file_b));

        os::filesystem::directory_iterator it(temp_dir.path);
        VX_CHECK(it.is_valid());
        VX_CHECK(it != os::filesystem::directory_iterator{});

        const os::path first_path = it->path;

        while (++it != os::filesystem::directory_iterator{})
        {
            VX_CHECK(it->path != first_path);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_recursive_directory_iterator)
{
    temp_directory temp_dir("test_recursive_directory_iterator.dir");
    VX_CHECK(temp_dir.exists());

    static_assert(std::is_same<typename os::filesystem::recursive_directory_iterator::iterator_category, std::input_iterator_tag>::value);
    static_assert(std::is_same<typename os::filesystem::recursive_directory_iterator::value_type, os::filesystem::directory_entry>::value);
    static_assert(std::is_same<typename os::filesystem::recursive_directory_iterator::difference_type, ptrdiff_t>::value);
    static_assert(std::is_same<typename os::filesystem::recursive_directory_iterator::pointer, const os::filesystem::directory_entry*>::value);
    static_assert(std::is_same<typename os::filesystem::recursive_directory_iterator::reference, const os::filesystem::directory_entry&>::value);

    VX_SECTION("default constructor")
    {
        os::filesystem::recursive_directory_iterator it;
    }

    VX_SECTION("nonexistent path")
    {
        for (const auto& p : nonexistent_paths)
        {
            VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::recursive_directory_iterator(p).is_valid());
        }
    }

#if defined(VX_PLATFORM_WINDOWS)

    VX_SECTION("edge cases")
    {
        // Test VSO-844835 "recursive_directory_iterator constructed with empty path iterates over the current directory"
        VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::recursive_directory_iterator(os::path{}).is_valid());
        // Test VSO-583725 "recursive_recursive_directory_iterator blows up (memory leak + infinite loop) with embedded nulls"
        VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::recursive_directory_iterator(std::wstring(L".\0", 2)).is_valid());
    }

#endif // VX_PLATFORM_WINDOWS

    VX_SECTION("simple test")
    {
        const os::path file = temp_dir.path / "file.txt";
        VX_CHECK(os::filesystem::create_file(file));

        os::filesystem::recursive_directory_iterator it(temp_dir.path);
        VX_CHECK(it.is_valid());
        VX_CHECK(it != os::filesystem::recursive_directory_iterator{});

        VX_CHECK(it == it);
        VX_CHECK(!(it != it));

        const auto& first_entry = *it;
        VX_CHECK(first_entry.path == it->path);

        bool found_directory = false;
        do
        {
            found_directory = found_directory || (it->path == file);
            VX_EXPECT_NO_ERROR(++it);

        } while (it != os::filesystem::recursive_directory_iterator{});

        VX_CHECK(found_directory);
    }

    VX_SECTION("copy and assignment")
    {
        const os::filesystem::recursive_directory_iterator it1(temp_dir.path);
        VX_CHECK(it1.is_valid());
        VX_CHECK(it1 != os::filesystem::recursive_directory_iterator{});

        const os::filesystem::recursive_directory_iterator it2(it1);
        VX_CHECK(it2.is_valid());
        VX_CHECK(it2 != os::filesystem::recursive_directory_iterator{});

        VX_CHECK(it1 == it2);
        VX_CHECK(!(it1 != it2));

        os::filesystem::recursive_directory_iterator it3;
        it3 = it2;
        VX_CHECK(it3.is_valid());
        VX_CHECK(it3 != os::filesystem::recursive_directory_iterator{});

        VX_CHECK(it2 == it3);
        VX_CHECK(!(it2 != it3));

        // self assignment
        it3 = it3;
        VX_CHECK(it1 == it3);
    }

    VX_SECTION("move")
    {
        os::filesystem::recursive_directory_iterator it1(temp_dir.path);
        VX_CHECK(it1.is_valid());
        VX_CHECK(it1 != os::filesystem::recursive_directory_iterator{});
        const os::path first_entry = it1->path;

        VX_DISABLE_WARNING("", 26800); // disable use after move warning
        VX_DISABLE_WARNING_PUSH();

        os::filesystem::recursive_directory_iterator it2(std::move(it1));
        VX_CHECK(it2.is_valid());
        VX_CHECK(!it1.is_valid());
        VX_CHECK(it2 != os::filesystem::recursive_directory_iterator{});
        VX_CHECK(it1 == os::filesystem::recursive_directory_iterator{});
        VX_CHECK(first_entry == it2->path);

        os::filesystem::recursive_directory_iterator it3;
        it3 = std::move(it2);
        VX_CHECK(it3.is_valid());
        VX_CHECK(!it2.is_valid());
        VX_CHECK(it3 != os::filesystem::recursive_directory_iterator{});
        VX_CHECK(it2 == os::filesystem::recursive_directory_iterator{});
        VX_CHECK(first_entry == it3->path);

        // self move assignment
        it3 = std::move(it3);
        VX_CHECK(it3.is_valid());
        VX_CHECK(it3 != os::filesystem::recursive_directory_iterator{});

        VX_DISABLE_WARNING_POP();
    }

    VX_SECTION("operator++")
    {
        os::path file_a = temp_dir.path / "a.txt";
        os::path file_b = temp_dir.path / "b.txt";

        VX_CHECK(os::filesystem::create_file(file_a));
        VX_CHECK(os::filesystem::create_file(file_b));

        os::filesystem::recursive_directory_iterator it(temp_dir.path);
        VX_CHECK(it.is_valid());
        VX_CHECK(it != os::filesystem::recursive_directory_iterator{});

        const os::path first_path = it->path;

        while (++it != os::filesystem::recursive_directory_iterator{})
        {
            VX_CHECK(it->path != first_path);
        }
    }

    VX_SECTION("misc")
    {
        os::filesystem::recursive_directory_iterator it(temp_dir.path);
        VX_CHECK(it.is_valid());
        VX_CHECK(it != os::filesystem::recursive_directory_iterator{});

        VX_CHECK(it.depth() == 0);
        VX_CHECK(!it.recursion_pending());

        it.pop();
        VX_CHECK(it == os::filesystem::recursive_directory_iterator{});
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_recursive_directory_iterator_traversal)
{
    temp_directory temp_dir("test_recursive_directory_iterator_traversal.dir");
    VX_CHECK(temp_dir.exists());

    const os::path directories[] = {
        temp_dir.path / "A",
        temp_dir.path / "A/A1",
        temp_dir.path / "A/A2",
        temp_dir.path / "B",
        temp_dir.path / "B/B1",
        temp_dir.path / "C",
        temp_dir.path / "C/C1"
    };

    const os::path files[] = {
        temp_dir.path / "file_root.txt",
        temp_dir.path / "A/file_a.txt",
        temp_dir.path / "A/A1/file_a1.txt",
        temp_dir.path / "B/B1/file_b1.txt",
        temp_dir.path / "C/file_c.txt"
    };

    // Create the pyramid directory structure
    for (const os::path& p : directories)
    {
        VX_CHECK(os::filesystem::create_directory(p));
    }

    // Place files strategically
    for (const os::path& p : files)
    {
        VX_CHECK(os::filesystem::create_file(p));
    }

    VX_SECTION("traverses all entries")
    {
        constexpr size_t expected_entry_count = mem::array_size(directories) + mem::array_size(files);

        std::unordered_set<os::path> visited_directories;
        std::unordered_set<os::path> visited_files;

        os::filesystem::recursive_directory_iterator it(temp_dir.path);
        VX_CHECK(it.is_valid());

        while (it.is_valid())
        {
            if (it->is_directory())
            {
                visited_directories.insert(it->path);
            }
            else
            {
                visited_files.insert(it->path);
            }

            ++it;
        }

        VX_CHECK((visited_directories.size() + visited_files.size()) == expected_entry_count);

        const std::unordered_set<os::path> expected_directories(std::begin(directories), std::end(directories));
        VX_CHECK(visited_directories == expected_directories);

        const std::unordered_set<os::path> expected_files(std::begin(files), std::end(files));
        VX_CHECK(visited_files == expected_files);
    }

    VX_SECTION("pop method test")
    {
        os::filesystem::recursive_directory_iterator it(temp_dir.path);
        VX_CHECK(it.is_valid());

        while (it.is_valid())
        {
            if (it->path.filename() == "B1")
            {
                it.pop(); // Should jump back to the parent directory
                VX_CHECK(it.is_valid());
                VX_CHECK(it->path.filename() == "C");
                break;
            }
            else
            {
                ++it;
            }
        }
    }

    VX_SECTION("disable_pending_recursion method test")
    {
        os::filesystem::recursive_directory_iterator it(temp_dir.path);
        VX_CHECK(it.is_valid());

        while (it.is_valid())
        {
            if (it->path.filename() == "B")
            {
                it.disable_pending_recursion(); // Should skip "B/B1"
                ++it;
                VX_CHECK(it->path.filename() == "C");
                break;
            }
            else
            {
                ++it;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(test_remove)
{
    temp_directory temp_dir("test_remove.dir");
    VX_CHECK(temp_dir.exists());

    const os::path directory = temp_dir.path / "demove.dir";
    const os::path file = directory / "remove.txt";

    VX_SECTION("remove file")
    {
        VX_CHECK(os::filesystem::create_directory(directory));
        VX_CHECK(os::filesystem::exists(directory));

        VX_CHECK(os::filesystem::create_file(file));
        VX_CHECK(os::filesystem::exists(file));

        VX_CHECK(os::filesystem::remove(file));
        VX_CHECK(!os::filesystem::exists(file));
    }

    VX_SECTION("remove directory")
    {
        VX_CHECK(os::filesystem::create_directory(directory));
        VX_CHECK(os::filesystem::exists(directory));

        VX_CHECK(os::filesystem::create_file(file));
        VX_CHECK(os::filesystem::exists(file));

        // should fail for non-empty directory
        VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::remove(directory));

        VX_CHECK(os::filesystem::remove(file));
        VX_CHECK(!os::filesystem::exists(file));

        VX_CHECK(os::filesystem::remove(directory));
        VX_CHECK(!os::filesystem::exists(directory));
    }

    VX_SECTION("remove read-only file")
    {
        VX_CHECK(os::filesystem::create_directory(directory));
        VX_CHECK(os::filesystem::exists(directory));

        VX_CHECK(os::filesystem::create_file(file));
        VX_CHECK(os::filesystem::exists(file));

        // make file read only
        VX_CHECK(os::filesystem::update_permissions(file, os::filesystem::file_permissions::ALL_READ));

        VX_CHECK(os::filesystem::remove(file));
        VX_CHECK(!os::filesystem::exists(file));
    }

    VX_SECTION("remove nonexistent paths")
    {
        for (const auto& p : nonexistent_paths)
        {
            VX_CHECK_AND_EXPECT_NO_ERROR(!os::filesystem::remove(p));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

//VX_TEST_CASE(test_copy_file)
//{
//    temp_directory temp_dir("test_copy_file.dir");
//    VX_CHECK(temp_dir.exists());
//
//    VX_SECTION("source and destination are not file")
//    {
//        const os::path from_dir = temp_dir.path / "from.dir";
//        const os::path to_dir = temp_dir.path / "to.dir";
//
//        VX_CHECK(os::filesystem::create_directory(from_dir));
//        VX_CHECK(os::filesystem::create_directory(to_dir));
//
//        VX_CHECK_AND_EXPECT_ERROR(!os::filesystem::copy_file(from_dir, to_dir));
//    }
//}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    //VX_PRINT_ERRORS(true);
    VX_RUN_TESTS();
    return 0;
}