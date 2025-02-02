#include "vertex_test/os/path.hpp"

using namespace vx;

// https://github.com/microsoft/STL/blob/fc15609a0f2ae2a134c34e7c9a13977994f37367/tests/std/tests/P0218R1_filesystem/test.cpp#L217

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

VX_TEST_CASE(decomposition)
{
    constexpr test::decomposition_test_case decomp_test_cases[] = {

        // basic empty path
        { "", "", "", "", "", "", false },
        // basic short relative path
        { "h", "", "", "h", "", "h", false },        
        // basic relative path
        { "hi", "", "", "hi", "", "hi", false },

        // empty drive-relative path
#if defined(VX_PLATFORM_WINDOWS)
        { "c:", "c:", "", "", "c:", "", false },
        { "C:", "C:", "", "", "C:", "", false },
#else
        { "c:", "", "", "c:", "", "c:", false },
        { "C:", "", "", "C:", "", "C:", false },
#endif // VX_PLATFORM_WINDOWS

        // non-empty drive-relative path
#if defined(VX_PLATFORM_WINDOWS)
        { "c:dog", "c:", "", "dog", "c:", "dog", false },
#else
        { "c:dog", "", "", "c:dog", "", "c:dog", false },
#endif // VX_PLATFORM_WINDOWS
        
        // malformed drive
        { "cc:dog", "", "", "cc:dog", "", "cc:dog", false },
        { "::dog", "", "", "::dog", "", "::dog", false },
        { " :dog", "", "", " :dog", "", " :dog", false },
        { ":dog", "", "", ":dog", "", ":dog", false },
#if defined(VX_PLATFORM_WINDOWS)
        { R"(\:dog)", "", R"(\)", ":dog", R"(\)", ":dog", false },
        { R"(\:\dog)", "", R"(\)", R"(:\dog)", R"(\:)", "dog", false },
        { R"(1:\dog)", "", "", R"(1:\dog)", R"(1:)", "dog", false },
#else
        { R"(\:dog)", "", "", R"(\:dog)", "", R"(\:dog)", false },
        { R"(\:\dog)", "", "", R"(\:\dog)", "", R"(\:\dog)", false },
        { R"(1:\dog)", "", "", R"(1:\dog)", "", R"(1:\dog)", false },
#endif // VX_PLATFORM_WINDOWS
        
        // current drive root relative path
        { "Dev/msvc", "", "", "Dev/msvc", "Dev", "msvc", false },
#if defined(VX_PLATFORM_WINDOWS)
        { R"(\Dev\msvc)", "", R"(\)", R"(Dev\msvc)", R"(\Dev)", "msvc", false },
        { R"(\Dev\msvc\)", "", R"(\)", R"(Dev\msvc\)", R"(\Dev\msvc)", "", false },
#else
        { "Dev/msvc/", "", "", "Dev/msvc/", "Dev/msvc", "", false },
#endif // VX_PLATFORM_WINDOWS
        
        // absolute path
#if defined(VX_PLATFORM_WINDOWS)
        { R"(c:\)", "c:", R"(\)", "", R"(c:\)", "", true },
        { "c:/", "c:", "/", "", "c:/", "", true },
#else
        { "/", "", "/", "", "/", "", true },
        { R"(\)", "", "", R"(\)", "", R"(\)", false },
#endif // VX_PLATFORM_WINDOWS
      
      // usual path form
#if defined(VX_PLATFORM_WINDOWS)
        { R"(c:\cat\dog\elk)", "c:", R"(\)", R"(cat\dog\elk)", R"(c:\cat\dog)", "elk", true },
        { R"(c:\cat\dog\elk\)", "c:", R"(\)", R"(cat\dog\elk\)", R"(c:\cat\dog\elk)", "", true },
        { R"(c:/cat\dog\elk)", "c:", R"(/)", R"(cat\dog\elk)", R"(c:/cat\dog)", "elk", true },
        { R"(c:\cat\dog/elk/)", "c:", R"(\)", R"(cat\dog/elk/)", R"(c:\cat\dog/elk)", "", true },
#else
        { "/cat/dog/elk", "", "/", "cat/dog/elk", "/cat/dog", "elk", true},
        { "/cat/dog/elk/", "", "/", "cat/dog/elk/", "/cat/dog/elk", "", true },
#endif // VX_PLATFORM_WINDOWS
        
        // extra long root-directory
#if defined(VX_PLATFORM_WINDOWS)
        { R"(c:\\\\\cat\dog\elk)", "c:", R"(\\\\\)", R"(cat\dog\elk)", R"(c:\\\\\cat\dog)", "elk", true },
#else
        { "//////cat/dog/elk", "", "//////", "cat/dog/elk", "//////cat/dog", "elk", true },
#endif // VX_PLATFORM_WINDOWS

        // Special cases
#if defined(VX_PLATFORM_WINDOWS)
        // "\\?\"
        { R"(\\?\)", R"(\\?)", R"(\)", "", R"(\\?\)", "", true },
        { R"(\\?\x)", R"(\\?)", R"(\)", "x", R"(\\?\)", "x", true },
        // with extra trailing \, not the special prefix any longer, but becomes the \\server form:
        { R"(\\?\\)", R"(\\?)", R"(\\)", "", R"(\\?\\)", "", true },
        // also "\\.\" special prefix
        { R"(\\.\x)", R"(\\.)", R"(\)", "x", R"(\\.\)", "x", true },
        // also "\??\" special prefix
        { R"(\??\x)", R"(\??)", R"(\)", "x", R"(\??\)", "x", true },
        
        // adding an extra trailing \ to this one makes it become a root relative path:
        { R"(\??\\)", "", R"(\)", R"(??\\)", R"(\??)", "", false },
        // not special (more follow)
        { R"(\x?\x)", "", R"(\)", R"(x?\x)", R"(\x?)", "x", false },
        { R"(\?x\x)", "", R"(\)", R"(?x\x)", R"(\?x)", "x", false },
        { R"(\\x\x)", R"(\\x)", R"(\)", R"(x)", R"(\\x\)", "x", true },
        { R"(\??)", R"()", R"(\)", R"(??)", R"(\)", "??", false },
        { R"(\???)", R"()", R"(\)", R"(???)", R"(\)", "???", false },
        { R"(\????)", R"()", R"(\)", R"(????)", R"(\)", "????", false },
        // became \\server form
        { R"(\\.xx)", R"(\\.xx)", "", "", R"(\\.xx)", "", true },
        
#else
        { R"(\\?\)", "", "", R"(\\?\)", "", R"(\\?\)", false },
        { R"(\\.\)", "", "", R"(\\.\)", "", R"(\\.\)", false },
        { R"(\??\)", "", "", R"(\??\)", "", R"(\??\)", false },
#endif // VX_PLATFORM_WINDOWS

        // network path
#if defined(VX_PLATFORM_WINDOWS)
        { R"(\\server\share)", R"(\\server)", R"(\)", "share", R"(\\server\)", "share", true },
        { R"(/\server\share)", R"(/\server)", R"(\)", "share", R"(/\server\)", "share", true },
        { R"(/\server/share)", R"(/\server)", R"(/)", "share", R"(/\server/)", "share", true },
        { R"(//server\share)", R"(//server)", R"(\)", "share", R"(//server\)", "share", true },
        { R"(//server/share)", R"(//server)", R"(/)", "share", R"(//server/)", "share", true },
        { R"(\/server\share)", R"(\/server)", R"(\)", "share", R"(\/server\)", "share", true },
        { R"(\/server/share)", R"(\/server)", R"(/)", "share", R"(\/server/)", "share", true },
        // \\\ doesn't fall into \\server, becomes a current drive root relative path:
        { R"(\\\\)", "", R"(\\\\)", "", R"(\\\\)", "", false },
        { R"(\\\dog)", "", R"(\\\)", "dog", R"(\\\)", "dog", false },
#else
        { R"(\\server)", "", "", R"(\\server)", "", R"(\\server)", false },
#endif // VX_PLATFORM_WINDOWS

        // document that we don't treat \\?\UNC special even if MSDN does:
#if defined(VX_PLATFORM_WINDOWS)
        { R"(\\?\UNC\server\share)", R"(\\?)", R"(\)", R"(UNC\server\share)", R"(\\?\UNC\server)", "share", true },
        // other similar cases
        { R"(\\?\UNC)", R"(\\?)", R"(\)", R"(UNC)", R"(\\?\)", "UNC", true },
        { R"(\\?\UNC\server)", R"(\\?)", R"(\)", R"(UNC\server)", R"(\\?\UNC)", "server", true },
        { R"(\\?\UNC\server\)", R"(\\?)", R"(\)", R"(UNC\server\)", R"(\\?\UNC\server)", "", true },
        { R"(\\?\UNC\\)", R"(\\?)", R"(\)", R"(UNC\\)", R"(\\?\UNC)", "", true },
#endif // VX_PLATFORM_WINDOWS

        // document that drive letters aren't special with special prefixes:
#if defined(VX_PLATFORM_WINDOWS)
        { R"(\\.\C:attempt_at_relative)", R"(\\.)", R"(\)", R"(C:attempt_at_relative)", R"(\\.\)", "C:attempt_at_relative", true },
#endif // VX_PLATFORM_WINDOWS

        // other interesting user-submitted test cases:
#if defined(VX_PLATFORM_WINDOWS)
        { R"(\)", "", R"(\)", "", R"(\)", "", false },
        { R"(\\)", "", R"(\\)", "", R"(\\)", "", false },
        { R"(\\\)", "", R"(\\\)", "", R"(\\\)", "", false },
        { R"(\\\.)", "", R"(\\\)", ".", R"(\\\)", ".", true },
        { R"(\\\..)", "", R"(\\\)", "..", R"(\\\)", "..", false },
        { R"(\\\.\)", "", R"(\\\)", R"(.\)", R"(\\\.)", "", false },
        { R"(\\\..\)", "", R"(\\\)", R"(..\)", R"(\\\..)", "", false },
        { "/", "", "/", "", "/", "", false },
        { "//", "", "//", "", "//", "", false },
        { "///", "", "///", "", "///", "", false },
        { R"(\/)", "", R"(\/)", "", R"(\/)", "", false },
        { "/c", "", "/", "c", "/", "c", false },
        { "/c:", "", "/", "c:", "/", "c:", false },
        { "..", "", "", "..", "", "..", false },
        { R"(\.)", "", R"(\)", ".", R"(\)", ".", false },
        { "/.", "", "/", ".", "/", ".", false },
        { R"(\..)", "", R"(\)", "..", R"(\)", "..", false },
        { R"(\..\..)", "", R"(\)", R"(..\..)", R"(\..)", "..", false },
        { R"(c:\..)", "c:", R"(\)", "..", R"(c:\)", "..", true },
        { "c:..", "c:", "", "..", "c:", "..", false },
        { R"(c:..\..)", "c:", "", R"(..\..)", R"(c:..)", "..", false },
        { R"(c:\..\..)", "c:", R"(\)", R"(..\..)", R"(c:\..)", "..", true },
        { R"(\\server\..)", R"(\\server)", R"(\)", R"(..)", R"(\\server\)", "..", true },
        { R"(\\server\share\..\..)", R"(\\server)", R"(\)", R"(share\..\..)", R"(\\server\share\..)", "..", true },
        { R"(\\server\.\share)", R"(\\server)", R"(\)", R"(.\share)", R"(\\server\.)", "share", true },
        { R"(\\server\.\..\share)", R"(\\server)", R"(\)", R"(.\..\share)", R"(\\server\.\..)", "share", true },
        { R"(\..\../)", "", R"(\)", R"(..\../)", R"(\..\..)", "", false },
        { R"(\\..\../)", R"(\\..)", R"(\)", R"(../)", R"(\\..\..)", "", true },
        { R"(\\\..\../)", "", R"(\\\)", R"(..\../)", R"(\\\..\..)", "", false },
        { R"(\\?/)", R"(\\?)", R"(/)", "", R"(\\?/)", "", true },
        { R"(\/?/)", R"(\/?)", R"(/)", "", R"(\/?/)", "", true },
        { "//?/", "//?", "/", "", "//?/", "", true },
        { "//server", "//server", "", "", "//server", "", true },
        { "[:/abc", "", "", "[:/abc", "[:", "abc", false }
#else
        { "//", "", "//", "", "//", "", true },
        { "///", "", "///", "", "///", "", true },
        { R"(\/)", "", "", R"(\/)", R"(\)", "", false },
        { R"(/\)", "", "/", R"(\)", "/", R"(\)", true },
        { "/c", "", "/", "c", "/", "c", true },
        { "/c:", "", "/", "c:", "/", "c:", true },
        { "..", "", "", "..", "", "..", false },
        { "/.", "", "/", ".", "/", ".", true },
        { "/..", "", "/", "..", "/", "..", true },
        { "/../..", "", "/", "../..", "/..", "..", true },
        { "/../../", "", "/", "../../", "/../..", "", true },
        { "//../../", "", "//", "../../", "//../..", "", true }
#endif // VX_PLATFORM_WINDOWS
    };

    for (const auto& t : decomp_test_cases)
    {
        VX_CHECK(test::run_decomposition_test_case(t));
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}