#include "sandbox/sandbox.hpp"

#include "vertex/os/filesystem/path.hpp"

#include <filesystem>

using namespace vx;

int main(int argc, char* argv[])
{
    std::vector<const char*> test_paths = {
        "/",
        "C:\\",
        "./dir/file",
        ".\\dir\\file",
        "~/",
        "%USERPROFILE%\\",
        "\\\\Server\\Share\\Folder\\File",
        "/usr//local///bin",
        "C:\\\\Windows\\\\System32\\\\",
        "/usr/local/bin/",
        "C:\\Windows\\System32\\",
        "/usr/local/../bin",
        "C:\\Windows\\System32\\..\\",
        "/path/with space/file.txt",
        "C:\\Path\\With Space\\File.txt",
        //"/path/with/𝓾𝓷𝓲𝓬𝓸𝓭𝓮",
        //R"(C:\Path\With\𝓾𝓷𝓲𝓬𝓸𝓭𝓮)",
        "/dev/null",
        "NUL",
        "",
        "../..",
        "..\\.."
    };

    for (int i = 0; i < test_paths.size(); ++i)
    {
        const auto& path = test_paths[i];

        VX_LOG_INFO << "testing path: " << i << ": " << path;

        std::filesystem::path std_p(path);
        os::filesystem::path os_p(path);

        std::cout << "std::filesystem:\n";
        for (const auto& p : std_p)
        {
            std::cout << "  " << p << "\n";
        }

        std::cout << "os::filesystem:\n";
        for (const auto& p : os_p)
        {
            std::cout << "  " << p << "\n";
        }
    }

    return 0;
}