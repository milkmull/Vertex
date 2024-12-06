#include "sandbox/sandbox.hpp"

#include "vertex/os/filesystem/path.hpp"

#include <filesystem>

using namespace vx;

int main(int argc, char* argv[])
{
    const std::vector<std::string> test_cases = {
        "c:../../file///////",                    // Test redundant separators and dot-dot segments.
        "/home/user/../././folder////file.txt",   // Test absolute path with redundant segments.
        "C:\\\\..\\..\\folder\\file.txt",         // Windows-style path with dot-dots.
        "C:/a/b/../../c/./d/e/f//g/h/..",         // Complex mixed relative path.
        "../..//..//file",                        // Relative path with multiple dot-dots.
        ".",                                      // Current directory.
        "..",                                     // Parent directory.
        "C:/folder/..",                           // Path with parent directory reference at the end.
        "C:/folder/../",                          // Path ending with a parent directory reference and trailing slash.
        "/../folder/../../file.txt",              // Complex path starting at the root.
        "/",                                      // Root directory.
        "///////",                                // Path with only redundant separators.
        "C:/",                                    // Drive root.
        "C:../folder/file",                       // Relative path within a drive.
        "/folder/./subfolder/..",                 // Path with dot and dot-dot in a subfolder.
    };

    for (const auto& test_case : test_cases)
    {
        std::filesystem::path std_path(test_case);
        os::filesystem::path os_path(test_case);

        std::cout << "Test Case: " << test_case << "\n";
        std::cout << "std::filesystem: " << std_path.lexically_normal() << "\n";
        std::cout << "os::filesystem: " << os_path.lexically_normal() << "\n\n";
    }

    return 0;
}