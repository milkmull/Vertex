#pragma once

#include "vertex_test/test.hpp"
#include "vertex/os/path.hpp"

namespace vx {
namespace test {

///////////////////////////////////////////////////////////////////////////////
// decomposition
///////////////////////////////////////////////////////////////////////////////

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

inline bool run_decomposition_test_case(const decomposition_test_case& test_case)
{
#   define failure_message(input, test, actual, expected) \
        std::cout << "  Decomposition failed for " << input << "\n  " << \
        "Test: " << test << " Result: " << actual << " Expected: " << expected << '\n'

    const os::path p(test_case.input);

    // root name
    const auto root_name = p.root_name().string();
    if (root_name != test_case.root_name)
    {
        failure_message(test_case.input, "root_name", root_name, test_case.root_name);
        return false;
    }
    const bool has_root_name_actual = p.has_root_name();
    const bool has_root_name_expected = !test_case.root_name.empty();
    if (has_root_name_actual != has_root_name_expected)
    {
        failure_message(test_case.input, "has_root_name", has_root_name_actual, has_root_name_expected);
        return false;
    }

    // root directory
    const auto root_directory = p.root_directory().string();
    if (root_directory != test_case.root_directory)
    {
        failure_message(test_case.input, "root_directory", root_directory, test_case.root_directory);
        return false;
    }
    const bool has_root_directory_actual = p.has_root_directory();
    const bool has_root_directory_expected = !test_case.root_directory.empty();
    if (has_root_directory_actual != has_root_directory_expected)
    {
        failure_message(test_case.input, "has_root_directory", has_root_directory_actual, has_root_directory_expected);
        return false;
    }
    
    // root path
    const auto actual_root_path = p.root_path().string();
    const auto expected_root_path = std::string(test_case.root_name) + std::string(test_case.root_directory);
    if (actual_root_path != expected_root_path)
    {
        failure_message(test_case.input, "root_path", actual_root_path, expected_root_path);
        return false;
    }
    const bool has_root_path_actual = p.has_root_path();
    const bool has_root_path_expected = !(test_case.root_name.empty() && test_case.root_directory.empty());
    if (has_root_path_actual != has_root_path_expected)
    {
        failure_message(test_case.input, "has_root_path", has_root_path_actual, has_root_path_expected);
        return false;
    }

    // relative path
    const auto relative_path = p.relative_path().string();
    if (relative_path != test_case.relative_path)
    {
        failure_message(test_case.input, "relative_path", relative_path, test_case.relative_path);
        return false;
    }
    const bool has_relative_path_actual = p.has_relative_path();
    const bool has_relative_path_expected = !test_case.relative_path.empty();
    if (has_relative_path_actual != has_relative_path_expected)
    {
        failure_message(test_case.input, "has_relative_path", has_relative_path_actual, has_relative_path_expected);
        return false;
    }

    // parent path
    const auto parent_path = p.parent_path().string();
    if (parent_path != test_case.parent_path)
    {
        failure_message(test_case.input, "parent_path", parent_path, test_case.parent_path);
        return false;
    }
    const bool has_parent_path_actual = p.has_parent_path();
    const bool has_parent_path_expected = !test_case.parent_path.empty();
    if (has_parent_path_actual != has_parent_path_expected)
    {
        failure_message(test_case.input, "has_parent_path", has_parent_path_actual, has_parent_path_expected);
        return false;
    }

    // filename
    const auto filename = p.filename().string();
    if (filename != test_case.filename)
    {
        failure_message(test_case.input, "filename", filename, test_case.filename);
        return false;
    }
    const bool has_filename_actual = p.has_filename();
    const bool has_filename_expected = !test_case.filename.empty();
    if (has_filename_actual != has_filename_expected)
    {
        failure_message(test_case.input, "has_filename", has_filename_actual, has_filename_expected);
        return false;
    }

    // absolute
    const bool is_absolute_actual = p.is_absolute();
    const bool is_absolute_expected = test_case.absolute;
    if (is_absolute_actual != is_absolute_expected)
    {
        failure_message(test_case.input, "is_absolute", is_absolute_actual, is_absolute_expected);
        return false;
    }

    return true;

#   undef failure_message
}

struct stem_test_case
{
    str::str_arg_t input;
    str::str_arg_t stem;
    str::str_arg_t extension;
};

inline bool run_stem_test_case(const stem_test_case& test_case)
{
#   define failure_message(input, test, actual, expected) \
        std::cout << "  Stem decomposition failed for " << input << "\n  " << \
        "Test: " << test << " Result: " << actual << " Expected: " << expected << '\n'

    const os::path p(test_case.input);

    // stem
    const auto stem = p.stem().string();
    if (stem != test_case.stem)
    {
        failure_message(test_case.input, "stem", stem, test_case.stem);
        return false;
    }

    // extension
    const auto extension = p.extension().string();
    if (extension != test_case.extension)
    {
        failure_message(test_case.input, "extension", extension, test_case.extension);
        return false;
    }

#   undef failure_message

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// comparison
///////////////////////////////////////////////////////////////////////////////

enum class compare_result
{
    less,
    greater,
    equal
};

inline constexpr const char* compare_result_string(compare_result r)
{
    switch (r)
    {
        default:
        case compare_result::less:      return "<";
        case compare_result::greater:   return ">";
        case compare_result::equal:     return "==";
    }
}

struct compare_test_case
{
    str::str_arg_t lhs;
    str::str_arg_t rhs;
    compare_result expected;
};

inline bool run_compare_test_case(const compare_test_case& test_case)
{
#   define failure_message(lhs, rhs, actual, expected) \
        std::cout << "  Comparison failed for " << lhs << " and " << rhs << '\n' << \
        "  Result: " << actual << " Expected: " << expected << '\n'

    const os::path lhs(test_case.lhs);
    const os::path rhs(test_case.rhs);

    const auto lhs_hash = std::hash<os::path>{}(lhs);
    const auto rhs_hash = std::hash<os::path>{}(rhs);

    const int comparison = lhs.compare(rhs);
    const bool equal_hashes = (lhs_hash == rhs_hash);

    if (comparison < 0)
    {
        if (test_case.expected != compare_result::less)
        {
            failure_message(
                lhs, rhs,
                compare_result_string(compare_result::less),
                compare_result_string(test_case.expected)
            );
            return false;
        }

        if (equal_hashes)
        {
            return false;
        }
    }
    else if (comparison > 0)
    {
        if (test_case.expected != compare_result::greater)
        {
            failure_message(
                lhs, rhs,
                compare_result_string(compare_result::greater),
                compare_result_string(test_case.expected)
            );
            return false;
        }

        if (equal_hashes)
        {
            return false;
        }
    }
    else // comparison == 0
    {
        if (test_case.expected != compare_result::equal)
        {
            failure_message(
                lhs, rhs,
                compare_result_string(compare_result::equal),
                compare_result_string(test_case.expected)
            );
            return false;
        }

        if (!equal_hashes)
        {
            return false;
        }
    }

#   undef failure_message

    return true;
}

} // namespace test
} // namespace vx