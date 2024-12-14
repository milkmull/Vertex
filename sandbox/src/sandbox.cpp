#include "vertex/os/filesystem/path.hpp"

#include <filesystem>
#include <iostream>
#include <vector>

using namespace vx;

int main()
{
    struct TestCase {
        const char* path;
        const char* base;
    };

    std::vector<TestCase> test_cases = {
        // Basic Tests
        {"c:/hello/world/filename.txt", "c:/hello"},
        {"c:/hello/world", "c:/hello/world"},
        {"c:/hello/world/filename.txt", "d:/other/folder"},
        
        // Tests Without Root
        {"hello/world/filename.txt", "hello"},
        {"hello/world/filename.txt", "other/folder"},
        
        // Trailing Separators
        {"c:/hello/world/filename.txt", "c:/hello/"},
        //{"c:/hello/world/", "c:/hello/"},
        
        // Root Directory
        {"c:/", "c:/"},
        {"c:/hello/world/filename.txt", "c:/"},
        
        // Parent Directory
        {"c:/hello/world/filename.txt", "c:/hello/world"},
        
        // Dot and Dot-Dot
        {"c:/hello/world/../folder/./file.txt", "c:/hello"},
        {"c:/hello/./world/./file.txt", "c:/hello/world"},

        // Mixed Separators (Cross-Platform Compatibility)
        {"c:\\hello\\world\\filename.txt", "c:/hello"},
        {"c:/hello\\world/../folder/file.txt", "c:/hello"},
        
        // Case Sensitivity (Linux vs. Windows)
        {"c:/Hello/World/File.txt", "c:/hello/world"},
        {"c:/hello/world/file.txt", "c:/hello/world"},
        
        // Complex Relative Paths
        {"c:/hello/world/../folder/./../file.txt", "c:/hello"},
        {"c:/../file.txt", "c:/"},
    };

    for (const auto& test : test_cases)
    {
        std::filesystem::path std_path(test.path);
        os::filesystem::path os_path(test.path);

        std::cout << "Testing:\n"
            << "Path: " << test.path << "\n"
            << "Base: " << test.base << "\n";

        auto std_result = std_path.lexically_relative(test.base);
        auto os_result = os_path.lexically_relative(test.base);

        std::cout << "std::filesystem: " << std_result << "\n";
        std::cout << "os::filesystem:  " << os_result << "\n";

        if (std_result.native() != os_result.native()) {
            std::cout << "Test FAILED!\n";
        }
        else {
            std::cout << "Test PASSED!\n";
        }

        std::cout << "-------------------------\n";
    }

    return 0;
}
