#pragma once

#include "vertex_test/test.hpp"
#include "vertex/os/path.hpp"

namespace vx {
namespace test {

struct decomposition_test_case
{
    str::str_arg_t input;
    str::str_arg_t root_name;
    str::str_arg_t root_directory;
    str::str_arg_t relative_path;
    str::str_arg_t parent_path;
    str::str_arg_t filename;
    bool absolute;
};

struct decomposition_test_result {
    os::path root_name;
    bool has_root_name;
    os::path root_directory;
    bool has_root_directory;
    os::path root_path;
    bool has_root_path;
    os::path relative_path;
    bool has_relative_path;
    os::path parent_path;
    bool has_parent_path;
    os::path filename;
    bool has_filename;
    bool absolute;

    explicit decomposition_test_result(const os::path& p)
        : root_name(p.root_name()), has_root_name(p.has_root_name())
        , root_directory(p.root_directory()), has_root_directory(p.has_root_directory())
        , root_path(p.root_path()), has_root_path(p.has_root_path())
        , relative_path(p.relative_path()), has_relative_path(p.has_relative_path())
        , parent_path(p.parent_path()), has_parent_path(p.has_parent_path())
        , filename(p.filename()), has_filename(p.has_filename())
        , absolute(p.is_absolute())
    {
        VX_CHECK(absolute == !p.is_relative());
    }
};

inline bool run_decomposition_test_case(const decomposition_test_case& test_case)
{
#   define failure_message(input, test, actual, expected) \
        std::cout << "  Decomposition failed for " << input << "\n  " << \
        "Test: " << test << " Result: " << actual << " Expected: " << expected << '\n'

    const decomposition_test_result result(test_case.input);

    // root name
    const auto root_name = result.root_name.string();
    if (root_name != test_case.root_name)
    {
        failure_message(test_case.input, "root_name", root_name, test_case.root_name);
        return false;
    }
    const auto has_root_name_expected = !test_case.root_name.empty();
    if (result.has_root_name != has_root_name_expected)
    {
        failure_message(test_case.input, "has_root_name", result.has_root_name, has_root_name_expected);
        return false;
    }

    // root directory
    const auto root_directory = result.root_directory.string();
    if (root_directory != test_case.root_directory)
    {
        failure_message(test_case.input, "root_directory", root_directory, test_case.root_directory);
        return false;
    }
    const auto has_root_directory_expected = !test_case.root_directory.empty();
    if (result.has_root_directory != has_root_directory_expected)
    {
        failure_message(test_case.input, "has_root_directory", result.has_root_directory, has_root_directory_expected);
        return false;
    }
    
    // root path
    const auto root_path = result.root_path.string();
    const auto expected_root_path = std::string(test_case.root_name) + std::string(test_case.root_directory);
    if (root_path != expected_root_path)
    {
        failure_message(test_case.input, "root_path", root_path, expected_root_path);
        return false;
    }
    const auto has_root_path_expected = !(test_case.root_name.empty() && test_case.root_directory.empty());
    if (result.has_root_path != has_root_path_expected)
    {
        failure_message(test_case.input, "has_root_path", result.has_root_path, has_root_path_expected);
        return false;
    }

    // relative path
    const auto relative_path = result.relative_path.string();
    if (relative_path != test_case.relative_path)
    {
        failure_message(test_case.input, "relative_path", relative_path, test_case.relative_path);
        return false;
    }
    const auto has_relative_path_expected = !test_case.relative_path.empty();
    if (result.has_relative_path != has_relative_path_expected)
    {
        failure_message(test_case.input, "has_relative_path", result.has_relative_path, has_relative_path_expected);
        return false;
    }

    // parent path
    const auto parent_path = result.parent_path.string();
    if (parent_path != test_case.parent_path)
    {
        failure_message(test_case.input, "parent_path", parent_path, test_case.parent_path);
        return false;
    }
    const auto has_parent_path_expected = !test_case.parent_path.empty();
    if (result.has_parent_path != has_parent_path_expected)
    {
        failure_message(test_case.input, "has_parent_path", result.has_parent_path, has_parent_path_expected);
        return false;
    }

    // filename
    const auto filename = result.filename.string();
    if (filename != test_case.filename)
    {
        failure_message(test_case.input, "filename", filename, test_case.filename);
        return false;
    }
    const auto has_filename_expected = !test_case.filename.empty();
    if (result.has_filename != has_filename_expected)
    {
        failure_message(test_case.input, "has_filename", result.has_filename, has_filename_expected);
        return false;
    }

    // absolute
    if (result.absolute != test_case.absolute)
    {
        failure_message(test_case.input, "is_absolute", result.absolute, test_case.absolute);
        return false;
    }

    return true;
}

} // namespace test
} // namespace vx