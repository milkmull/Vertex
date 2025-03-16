#include "vertex_test/os/path.hpp"

#if defined(VX_PLATFORM_WINDOWS)
#   define VX_TESTING_WINDOWS_PATH
#endif // VX_PLATFORM_WINDOWS

using namespace vx;

// https://github.com/microsoft/STL/blob/fc15609a0f2ae2a134c34e7c9a13977994f37367/tests/std/tests/P0218R1_filesystem/test.cpp#L217

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(native)
{
#if defined(VX_TESTING_WINDOWS_PATH)
    static_assert(std::is_same<typename os::path::string_type, std::wstring>::value);
    static_assert(std::is_same<typename os::path::value_type, wchar_t>::value);
    static_assert(os::path::preferred_separator == L'\\');
#else
    static_assert(std::is_same<typename os::path::string_type, std::string>::value);
    static_assert(std::is_same<typename os::path::value_type, char>::value);
    static_assert(os::path::preferred_separator == '/');
#endif // VX_TESTING_WINDOWS_PATH
}

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
    VX_SECTION("body")
    {
        constexpr test::decomposition_test_case decomp_test_cases[] = {
            // basic empty path
            { "", "", "", "", "", "", false },
            // basic short relative path
            { "h", "", "", "h", "", "h", false },
            // basic relative path
            { "hi", "", "", "hi", "", "hi", false },

            // empty drive-relative path
#       if defined(VX_TESTING_WINDOWS_PATH)
            { "c:", "c:", "", "", "c:", "", false },
            { "C:", "C:", "", "", "C:", "", false },
#       else
            { "c:", "", "", "c:", "", "c:", false },
            { "C:", "", "", "C:", "", "C:", false },
#       endif // VX_TESTING_WINDOWS_PATH

            // non-empty drive-relative path
#       if defined(VX_TESTING_WINDOWS_PATH)
            { "c:dog", "c:", "", "dog", "c:", "dog", false },
#       else
            { "c:dog", "", "", "c:dog", "", "c:dog", false },
#       endif // VX_TESTING_WINDOWS_PATH

            // malformed drive
            { "cc:dog", "", "", "cc:dog", "", "cc:dog", false },
            { "::dog", "", "", "::dog", "", "::dog", false },
            { " :dog", "", "", " :dog", "", " :dog", false },
            { ":dog", "", "", ":dog", "", ":dog", false },
#       if defined(VX_TESTING_WINDOWS_PATH)
            { R"(\:dog)", "", R"(\)", ":dog", R"(\)", ":dog", false },
            { R"(\:\dog)", "", R"(\)", R"(:\dog)", R"(\:)", "dog", false },
            { R"(1:\dog)", "", "", R"(1:\dog)", R"(1:)", "dog", false },
#       else
            { R"(\:dog)", "", "", R"(\:dog)", "", R"(\:dog)", false },
            { R"(\:\dog)", "", "", R"(\:\dog)", "", R"(\:\dog)", false },
            { R"(1:\dog)", "", "", R"(1:\dog)", "", R"(1:\dog)", false },
#       endif // VX_TESTING_WINDOWS_PATH

            // current drive root relative path
            { "Dev/msvc", "", "", "Dev/msvc", "Dev", "msvc", false },
#       if defined(VX_TESTING_WINDOWS_PATH)
            { R"(\Dev\msvc)", "", R"(\)", R"(Dev\msvc)", R"(\Dev)", "msvc", false },
            { R"(\Dev\msvc\)", "", R"(\)", R"(Dev\msvc\)", R"(\Dev\msvc)", "", false },
#       else
            { "Dev/msvc/", "", "", "Dev/msvc/", "Dev/msvc", "", false },
#       endif // VX_TESTING_WINDOWS_PATH

            // absolute path
#       if defined(VX_TESTING_WINDOWS_PATH)
            { R"(c:\)", "c:", R"(\)", "", R"(c:\)", "", true },
            { "c:/", "c:", "/", "", "c:/", "", true },
#       else
            { "/", "", "/", "", "/", "", true },
            { R"(\)", "", "", R"(\)", "", R"(\)", false },
#       endif // VX_TESTING_WINDOWS_PATH

            // usual path form
#       if defined(VX_TESTING_WINDOWS_PATH)
            { R"(c:\cat\dog\elk)", "c:", R"(\)", R"(cat\dog\elk)", R"(c:\cat\dog)", "elk", true },
            { R"(c:\cat\dog\elk\)", "c:", R"(\)", R"(cat\dog\elk\)", R"(c:\cat\dog\elk)", "", true },
            { R"(c:/cat\dog\elk)", "c:", R"(/)", R"(cat\dog\elk)", R"(c:/cat\dog)", "elk", true },
            { R"(c:\cat\dog/elk/)", "c:", R"(\)", R"(cat\dog/elk/)", R"(c:\cat\dog/elk)", "", true },
#       else
            { "/cat/dog/elk", "", "/", "cat/dog/elk", "/cat/dog", "elk", true},
            { "/cat/dog/elk/", "", "/", "cat/dog/elk/", "/cat/dog/elk", "", true },
#       endif // VX_TESTING_WINDOWS_PATH

             // extra long root-directory
#       if defined(VX_TESTING_WINDOWS_PATH)
            { R"(c:\\\\\cat\dog\elk)", "c:", R"(\\\\\)", R"(cat\dog\elk)", R"(c:\\\\\cat\dog)", "elk", true },
#       else
            { "//////cat/dog/elk", "", "//////", "cat/dog/elk", "//////cat/dog", "elk", true },
#       endif // VX_TESTING_WINDOWS_PATH

            // Special cases
#       if defined(VX_TESTING_WINDOWS_PATH)
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

#       else
            { R"(\\?\)", "", "", R"(\\?\)", "", R"(\\?\)", false },
            { R"(\\.\)", "", "", R"(\\.\)", "", R"(\\.\)", false },
            { R"(\??\)", "", "", R"(\??\)", "", R"(\??\)", false },
#       endif // VX_TESTING_WINDOWS_PATH

            // network path
#       if defined(VX_TESTING_WINDOWS_PATH)
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
#       else
            { R"(\\server)", "", "", R"(\\server)", "", R"(\\server)", false },
#       endif // VX_TESTING_WINDOWS_PATH

            // document that we don't treat \\?\UNC special even if MSDN does:
#       if defined(VX_TESTING_WINDOWS_PATH)
            { R"(\\?\UNC\server\share)", R"(\\?)", R"(\)", R"(UNC\server\share)", R"(\\?\UNC\server)", "share", true },
            // other similar cases
            { R"(\\?\UNC)", R"(\\?)", R"(\)", R"(UNC)", R"(\\?\)", "UNC", true },
            { R"(\\?\UNC\server)", R"(\\?)", R"(\)", R"(UNC\server)", R"(\\?\UNC)", "server", true },
            { R"(\\?\UNC\server\)", R"(\\?)", R"(\)", R"(UNC\server\)", R"(\\?\UNC\server)", "", true },
            { R"(\\?\UNC\\)", R"(\\?)", R"(\)", R"(UNC\\)", R"(\\?\UNC)", "", true },
#       endif // VX_TESTING_WINDOWS_PATH

            // driver letter after special prefix
            // https://cplusplus.github.io/LWG/issue3699
#       if defined(VX_TESTING_WINDOWS_PATH)

            { R"(\\.\C:\attempt_at_relative)", R"(\\.\C:)", R"(\)", R"(attempt_at_relative)", R"(\\.\C:\)", "attempt_at_relative", true },
            { R"(\\.\C:attempt_at_relative)", R"(\\.\C:)", R"()", R"(attempt_at_relative)", R"(\\.\C:)", "attempt_at_relative", true },

            { R"(\\?\C:\attempt_at_relative)", R"(\\?\C:)", R"(\)", R"(attempt_at_relative)", R"(\\?\C:\)", "attempt_at_relative", true },
            { R"(\\?\C:attempt_at_relative)", R"(\\?\C:)", R"()", R"(attempt_at_relative)", R"(\\?\C:)", "attempt_at_relative", true },

            { R"(\??\C:\attempt_at_relative)", R"(\??\C:)", R"(\)", R"(attempt_at_relative)", R"(\??\C:\)", "attempt_at_relative", true },
            { R"(\??\C:attempt_at_relative)", R"(\??\C:)", R"()", R"(attempt_at_relative)", R"(\??\C:)", "attempt_at_relative", true },

#       endif // VX_TESTING_WINDOWS_PATH

        // other interesting user-submitted test cases:
#       if defined(VX_TESTING_WINDOWS_PATH)
            { R"(\)", "", R"(\)", "", R"(\)", "", false },
            { R"(\\)", "", R"(\\)", "", R"(\\)", "", false },
            { R"(\\\)", "", R"(\\\)", "", R"(\\\)", "", false },
            { R"(\\\.)", "", R"(\\\)", ".", R"(\\\)", ".", false },
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
#       else
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
#       endif // VX_TESTING_WINDOWS_PATH
        };

        for (const auto& t : decomp_test_cases)
        {
            VX_CHECK(test::run_decomposition_test_case(t));
        }
    }

    VX_SECTION("stem")
    {
        constexpr test::stem_test_case stem_test_cases[] = {
            // . and .. special cases
            { ".", ".", "" },
            { "..", "..", "" },
            // filenames with 1 leading period are special
            { ".config", ".config", "" },
            // 2 periods aren't special anymore
            { "..config", ".", ".config" },
            { "...config", "..", ".config" },
            { "classic_textfile.txt", "classic_textfile", ".txt" },
            { "cat.dog.elk.tar", "cat.dog.elk", ".tar" },
            { "cat.dog.elk", "cat.dog", ".elk" },
            { "cat.dog", "cat", ".dog" },
            { "cat", "cat", "" },
            { "cat.", "cat", "." },

            // all of the above with alternate data streams
#       if defined(VX_TESTING_WINDOWS_PATH)
            { ".:alternate_meow", ".", "" },
            { "..:alternate_dog", "..", "" },
            { ".config:alternate_elk", ".config", "" },
            { "..config:alternate_other_things", ".", ".config" },
            { "...config:alternate_other_things", "..", ".config" },
            { "classic_textfile.txt:$DATA", "classic_textfile", ".txt" },
            { "cat.dog.elk.tar:pay_no_attention", "cat.dog.elk", ".tar" },
            { "cat.dog.elk:to behind this curtain", "cat.dog", ".elk" },
            { "cat.dog:even:if:this:curtain:is:malformed", "cat", ".dog" },
            { "cat:what?", "cat", "" },
            { "cat.:alternate_fun", "cat", "." }
#       else
            { ".:alternate_meow", ".:alternate_meow", "" },
            { "..:alternate_dog", ".", ".:alternate_dog" },
            { ".config:alternate_elk", ".config:alternate_elk", "" },
            { "..config:alternate_other_things", ".", ".config:alternate_other_things" },
            { "...config:alternate_other_things", "..", ".config:alternate_other_things" },
            { "classic_textfile.txt:$DATA", "classic_textfile", ".txt:$DATA" },
            { "cat.dog.elk.tar:pay_no_attention", "cat.dog.elk", ".tar:pay_no_attention" },
            { "cat.dog.elk:to behind this curtain", "cat.dog", ".elk:to behind this curtain" },
            { "cat.dog:even:if:this:curtain:is:malformed", "cat", ".dog:even:if:this:curtain:is:malformed" },
            { "cat:what?", "cat:what?", "" },
            { "cat.:alternate_fun", "cat", ".:alternate_fun" }
#       endif // VX_TESTING_WINDOWS_PATH
        };

        for (const auto& t : stem_test_cases)
        {
            VX_CHECK(test::run_stem_test_case(t));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(compare)
{
    constexpr test::compare_test_case compare_test_cases[] = {
        // basic tests:
        { "", "", test::compare_result::equal },
        { "", "anything", test::compare_result::less },
        { "anything", "", test::compare_result::greater },
        { "c:", "c:", test::compare_result::equal },
        
        // different root_directory values compare equal:
        { "/", "//", test::compare_result::equal },
        { "//////", "//", test::compare_result::equal },
        
        // different counts of /s and different kinds compare:
#   if defined(VX_TESTING_WINDOWS_PATH)
        { "c://a/b", "c:/a//b", test::compare_result::equal },
        { "c://a/b", R"(c:/a/\b)", test::compare_result::equal },
        { "c:/a/b///c", "c:/a///b/c", test::compare_result::equal },
        { R"(c:/a/b\//c)", R"(c:/a//\b/c)", test::compare_result::equal },
#   else
        { "//a/b", "/a//b", test::compare_result::equal },
        { "//a/b", R"(/a/\b)", test::compare_result::greater },
        { "/a/b///c", "/a///b/c", test::compare_result::equal },
        { R"(/a/b\//c)", R"(/a//\b/c)", test::compare_result::greater },
#   endif // VX_TESTING_WINDOWS_PATH
        
        // comparing root_name:
#   if defined(VX_TESTING_WINDOWS_PATH)
        { R"(\\server\share)", "C:/a", test::compare_result::greater },
        { R"(//server\share)", "C:/a", test::compare_result::less },
        { "//server/share", "\\\\server/share", test::compare_result::equal },
        // doesn't actually get to has_root_name test, since root_name comparison differs:
        { R"(c:a)", R"(C:\a)", test::compare_result::greater },
#   endif // VX_TESTING_WINDOWS_PATH

        // different values of has_root_directory():
#   if defined(VX_TESTING_WINDOWS_PATH)
        { "c:/", "c:", test::compare_result::greater },
        { "c:", "c:/", test::compare_result::less },
        { "c:////", "c://", test::compare_result::equal },
        { R"(c:\a)", R"(c:a)", test::compare_result::greater },
        { R"(c:a)", R"(c:\a)", test::compare_result::less },
#   else
        { "/", "", test::compare_result::greater },
        { "", "/", test::compare_result::less },
        { "/a", "a", test::compare_result::greater},
        { "a", "/a", test::compare_result::less},
#   endif // VX_TESTING_WINDOWS_PATH
        
        // fewer path elements compare less:
        { "/a", "/a/b", test::compare_result::less },
        { "/a/b", "/a", test::compare_result::greater },
        { "/a/", "/a/b", test::compare_result::less },
        { "/a/b", "/a/", test::compare_result::greater },
        // trailing empty element
        { "/a/b", "/a/b/", test::compare_result::less },
        { "/a/b/", "/a/b", test::compare_result::greater },

        // comparisons inside path elements, note assumption L'A' < L'a':
        { "/CASEMATTERS", "/casematters", test::compare_result::less },
        { "/abcdefg", "/hijklmn", test::compare_result::less },
        { "/casematters", "/CASEMATTERS", test::compare_result::greater },
        { "/hijklmn", "/abcdefg", test::compare_result::greater },

        // differ only in where the / goes:
        { "/ap/ple", "/app/le", test::compare_result::less },
#   if defined(VX_TESTING_WINDOWS_PATH)
        { R"(c:/ap\ple)", R"(c:/app\le)", test::compare_result::less },
        { "c:/ap/ple", R"(c:/app\le)", test::compare_result::less },
        { R"(c:/ap\ple)", "c:/app/le", test::compare_result::less }
#   endif // VX_TESTING_WINDOWS_PATH
    };

    for (const auto& t : compare_test_cases)
    {
        VX_CHECK(test::run_compare_test_case(t));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(slash)
{
    constexpr test::slash_test_case slash_test_cases[] = {
        { "", "", "" },
#   if defined(VX_TESTING_WINDOWS_PATH)
        { "relative", "other", R"(relative\other)" },
        { "//server", "share", R"(//server\share)" },
        { "//server", "share", R"(//server\share)" },
        { "//server/", "share", R"(//server/share)" },
        { "cat", "c:/dog", "c:/dog" },
        { "cat", "c:", "c:" },
        { "cat", "", R"(cat\)"}, // note addition of "magic empty path" in decomposition
#   else
        { "relative", "other", "relative/other" },
        { "//server", "share", "//server/share" },
        { "//server/", "share", "//server/share" },
        { "cat", "/dog", "/dog" },
        { "cat", "/", "/" },
        { "cat", "", "cat/"}, // note addition of "magic empty path" in decomposition
#   endif // VX_TESTING_WINDOWS_PATH
        { "", "cat", "cat" },
        { "./", "cat", "./cat" },
#   if defined(VX_TESTING_WINDOWS_PATH)
        { "c:", "", "c:" },
        { "c:", "dog", "c:dog" },
        { "c:cat", "/dog", "c:/dog" },
        { "c:/cat", "/dog", "c:/dog" },
        { "c:cat", "c:dog", R"(c:cat\dog)" },
        { "c:cat", "d:dog", "d:dog" },
        { "c:abc", "def", R"(c:abc\def)" },
        { "c:abc", "dog/def", R"(c:abc\dog/def)" },
        { "//?/frobinate", R"(\that's strange)", R"(//?\that's strange)" },
        { "//?/", R"(\that's strange)", R"(//?\that's strange)" }
#   endif // VX_TESTING_WINDOWS_PATH
    };

    for (const auto& t : slash_test_cases)
    {
        VX_CHECK(test::run_slash_test_case(t));
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(iterators)
{
    static_assert(std::is_same<os::path::iterator::iterator_category, std::input_iterator_tag>::value);
    static_assert(std::is_same<os::path::iterator::value_type, os::path>::value);
    static_assert(std::is_same<os::path::iterator::difference_type, ptrdiff_t>::value);
    static_assert(std::is_same<os::path::iterator::pointer, const os::path*>::value);
    static_assert(std::is_same<os::path::iterator::reference, const os::path&>::value);

    VX_SECTION("comparison")
    {
        const os::path::iterator a{};
        const os::path::iterator b{};

        VX_CHECK(a == b);
        VX_CHECK(!(a != b));
    }

    VX_SECTION("empty")
    {
        const os::path empty;
        VX_CHECK(empty.begin() == empty.end());
    }

    VX_SECTION("empty")
    {
        const os::path empty;
        VX_CHECK(empty.begin() == empty.end());
    }

    VX_SECTION("dereference")
    {
        const char* s = "hello";
        const os::path p(s);

        const auto it = p.begin();
        VX_CHECK(it->string() == s);
        VX_CHECK((*it).string() == s);
    }

#if defined(VX_TESTING_WINDOWS_PATH)

    VX_SECTION("root name")
    {
        const os::path p("c:/cat");
        auto it = p.begin();

        VX_CHECK(it->string() == "c:");
        ++it;
        VX_CHECK(it->string() == "/");
        ++it;
        VX_CHECK(it->string() == "cat");
        ++it;
        VX_CHECK(it == p.end());
    }

    VX_SECTION("no root directory")
    {
        const os::path p("c:cat/dog");
        auto it = p.begin();

        VX_CHECK(it->string() == "c:");
        ++it;
        VX_CHECK(it->string() == "cat");
        ++it;
        VX_CHECK(it->string() == "dog");
        ++it;
        VX_CHECK(it == p.end());
    }

#endif // VX_TESTING_WINDOWS_PATH

    VX_SECTION("root directory")
    {
        const os::path p("/cat");
        auto it = p.begin();

        VX_CHECK(it->string() == "/");
        ++it;
        VX_CHECK(it->string() == "cat");
        ++it;
        VX_CHECK(it == p.end());
    }

#if defined(VX_TESTING_WINDOWS_PATH)

    VX_SECTION("long root directory")
    {
        const os::path p("c://///cat");
        auto it = p.begin();

        VX_CHECK(it->string() == "c:");
        ++it;
        VX_CHECK(it->string() == "/");
        ++it;
        VX_CHECK(it->string() == "cat");
        ++it;
        VX_CHECK(it == p.end());
}

#else

    VX_SECTION("long root directory")
    {
        const os::path p("/////cat");
        auto it = p.begin();

        VX_CHECK(it->string() == "/");
        ++it;
        VX_CHECK(it->string() == "cat");
        ++it;
        VX_CHECK(it == p.end());
    }

#endif // VX_TESTING_WINDOWS_PATH

    VX_SECTION("relative")
    {
        const os::path p("hello world");
        auto it = p.begin();

        VX_CHECK(it->string() == "hello world");
        ++it;
        VX_CHECK(it == p.end());
    }

    VX_SECTION("slashes")
    {
        const os::path p("a/b//c////d////e///");
        auto it = p.begin();

        VX_CHECK(it->string() == "a");
        ++it;
        VX_CHECK(it->string() == "b");
        ++it;
        VX_CHECK(it->string() == "c");
        ++it;
        VX_CHECK(it->string() == "d");
        ++it;
        VX_CHECK(it->string() == "e");
        ++it;
        VX_CHECK(it->string() == ""); // empty path because it ends with a slash
        ++it;
        VX_CHECK(it == p.end());
    }

    VX_SECTION("single slash")
    {
        const os::path p("/");
        auto it = p.begin();

        VX_CHECK(it->string() == "/");
        ++it;
        VX_CHECK(it == p.end());
    }

#if defined(VX_TESTING_WINDOWS_PATH)

    VX_SECTION("windows slash root")
    {
        const os::path p("//?/x:/a");
        auto it = p.begin();

        VX_CHECK(it->string() == "//?/x:");
        ++it;
        VX_CHECK(it->string() == "/");
        ++it;
        VX_CHECK(it->string() == "a");
        ++it;
        VX_CHECK(it == p.end());
    }

#endif // VX_TESTING_WINDOWS_PATH
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(append)
{
    os::path p;

    p += os::path("abc");
    VX_CHECK(p.string() == "abc");
    p += "def";
    VX_CHECK(p.string() == "abcdef");
    p += std::string("ghi");
    VX_CHECK(p.string() == "abcdefghi");
    p += str::str_arg_t("jkl");
    VX_CHECK(p.string() == "abcdefghijkl");
    p += 'm';
    VX_CHECK(p.string() == "abcdefghijklm");

    VX_CHECK(!p.empty());
    p.clear();
    VX_CHECK(p.empty());
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(pop_back)
{
    VX_CHECK(os::path().pop_back() == "");
    VX_CHECK(os::path("cat").pop_back() == "");
    VX_CHECK(os::path("cat/dog").pop_back() == "cat");

    VX_CHECK(os::path("//").pop_back() == "/");
    VX_CHECK(os::path("//share/").pop_back() == "//share/");
    VX_CHECK(os::path("//share//////").pop_back() == "//share/");
    VX_CHECK(os::path("//share/cat").pop_back() == "//share/");
    VX_CHECK(os::path("//share/cat/dog").pop_back() == "//share/cat");

#if defined(VX_TESTING_WINDOWS_PATH)

    VX_CHECK(os::path("c:cat").pop_back() == "c:");
    VX_CHECK(os::path("c:/").pop_back() == "c:/");
    VX_CHECK(os::path("c:/cat").pop_back() == "c:/");
    VX_CHECK(os::path("c:/cat/").pop_back() == "c:/cat");
    VX_CHECK(os::path("c:/cat/dog").pop_back() == "c:/cat");
    VX_CHECK(os::path("c:/cat///////dog").pop_back() == "c:/cat");
    VX_CHECK(os::path("c:///////dog").pop_back() == "c:/");

#else

#endif // VX_TESTING_WINDOWS_PATH
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(make_preferred)
{
#if defined(VX_TESTING_WINDOWS_PATH)

    os::path p("//?/a/b/c");
    VX_CHECK(p.make_preferred().string() == R"(\\?\a\b\c)");

#else

    os::path p(R"(///a/b\c)");
    VX_CHECK(p.make_preferred().string() == R"(///a/b\c)");

#endif // VX_TESTING_WINDOWS_PATH
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(filename)
{
    VX_SECTION("remove_filename")
    {
        os::path p;
        VX_CHECK(p.remove_filename().string() == "");

        p = "/";
        VX_CHECK(p.remove_filename().string() == "/");

        p = "hello/there/";
        VX_CHECK(p.remove_filename().string() == "hello/there/");

        p = "hello/there";
        VX_CHECK(p.remove_filename().string() == "hello/");

        p = "hello/there.txt";
        VX_CHECK(p.remove_filename().string() == "hello/");

#if defined(VX_TESTING_WINDOWS_PATH)

        p = "hello/there:extension";
        VX_CHECK(p.remove_filename().string() == "hello/");

        p = "c:";
        VX_CHECK(p.remove_filename().string() == "c:");

        p = "c:my_file.";
        VX_CHECK(p.remove_filename().string() == "c:");

        p = "c:/";
        VX_CHECK(p.remove_filename().string() == "c:/");

#endif // VX_TESTING_WINDOWS_PATH
    }

    VX_SECTION("remove_extension")
    {
        os::path p;
        VX_CHECK(p.remove_extension() == "");

        p = ".";
        VX_CHECK(p.remove_extension() == ".");

        p = "..";
        VX_CHECK(p.remove_extension() == "..");

        p = "...";
        VX_CHECK(p.remove_extension() == "..");

        // Files that start with a dot are special

        p = "/.config";
        VX_CHECK(p.remove_extension() == "/.config");

        p = ".txt";
        VX_CHECK(p.remove_extension() == ".txt");

        p = "hello";
        VX_CHECK(p.remove_extension() == "hello");

        p = "hello.txt";
        VX_CHECK(p.remove_extension() == "hello");

        p = "/hello.txt:bonus";
        VX_CHECK(p.remove_extension() == "/hello");

#if defined(VX_TESTING_WINDOWS_PATH)

        p = "c:.";
        VX_CHECK(p.remove_filename().string() == "c:");

        p = "c:hello.txt";
        VX_CHECK(p.remove_filename().string() == "c:");

#endif // VX_TESTING_WINDOWS_PATH
    }

    VX_SECTION("replace_extension")
    {
        os::path p;
        VX_CHECK(p.replace_extension(".txt") == ".txt");

        p = ".";
        VX_CHECK(p.replace_extension(".txt") == "..txt");

        p = "..";
        VX_CHECK(p.replace_extension(".txt") == "...txt");

        p = "...";
        VX_CHECK(p.replace_extension(".txt") == "...txt");

        // Files that start with a dot are special

        p = "/.config";
        VX_CHECK(p.replace_extension(".txt") == "/.config.txt");

        p = "hello.txt";
        VX_CHECK(p.replace_extension(".rgb") == "hello.rgb");

        p = "/hello.txt:bonus";
        VX_CHECK(p.replace_extension(".rgb") == "/hello.rgb");
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(lexically_normal)
{
    VX_CHECK(os::path().lexically_normal() == "");

    VX_CHECK(os::path("cat/./dog/..").lexically_normal() == "cat/");
    VX_CHECK(os::path("cat/.///dog/../").lexically_normal() == "cat/");

#if defined(VX_TESTING_WINDOWS_PATH)

    VX_CHECK(os::path("C:").lexically_normal() == "C:");
    VX_CHECK(os::path("C:drive_relative").lexically_normal() == "C:drive_relative");

    VX_CHECK(os::path("C:/").lexically_normal() == "C:/");
    VX_CHECK(os::path("C:\\").lexically_normal() == "C:\\");

    VX_CHECK(os::path("C:////").lexically_normal() == "C:/");
    VX_CHECK(os::path("C:\\\\\\\\").lexically_normal() == "C:\\");

    VX_CHECK(os::path("C:/absolute").lexically_normal() == "C:/absolute");
    VX_CHECK(os::path("C://///absolute").lexically_normal() == "C://///absolute");

#endif // VX_TESTING_WINDOWS_PATH

    VX_CHECK(os::path("/root_relative").lexically_normal() == "/root_relative");
    VX_CHECK(os::path("////root_relative").lexically_normal() == "/root_relative");

#if defined(VX_TESTING_WINDOWS_PATH)

    VX_CHECK(os::path("//server/share").lexically_normal() == "//server/share");
    VX_CHECK(os::path("//server///share").lexically_normal() == "//server/share");

#else

    VX_CHECK(os::path("//server/share").lexically_normal() == "/server/share");
    VX_CHECK(os::path("//server///share").lexically_normal() == "/server/share");

#endif // VX_TESTING_WINDOWS_PATH

#if defined(VX_TESTING_WINDOWS_PATH)

    VX_CHECK(os::path("//?/device").lexically_normal() == "//?/device");
    VX_CHECK(os::path("/??/device").lexically_normal() == "/??/device");
    VX_CHECK(os::path("//./device").lexically_normal() == "//./device");
    VX_CHECK(os::path("//?/UNC/server/share").lexically_normal() == "//?/UNC/server/share");

#endif // VX_TESTING_WINDOWS_PATH

    VX_CHECK(os::path("/a/b//c//d//e//f").lexically_normal() == "/a/b/c/d/e/f");

    VX_CHECK(os::path("/meow/").lexically_normal() == "/meow/");
    VX_CHECK(os::path("/meow//").lexically_normal() == "/meow/");

    VX_CHECK(os::path("/a/./b/././c/././.").lexically_normal() == "/a/b/c/");
    VX_CHECK(os::path("/a/./b/././c/./././").lexically_normal() == "/a/b/c/");

    VX_CHECK(os::path(".").lexically_normal() == ".");
    VX_CHECK(os::path("./").lexically_normal() == ".");
    VX_CHECK(os::path("./.").lexically_normal() == ".");
    VX_CHECK(os::path("././").lexically_normal() == ".");

    VX_CHECK(os::path("/a/b/c/d/e/../f/../../../g/h").lexically_normal() == "/a/b/g/h");
    VX_CHECK(os::path("/a/b/c/d/e/../f/../../../g/h/..").lexically_normal() == "/a/b/g/");
    VX_CHECK(os::path("/a/b/c/d/e/../f/../../../g/h/../").lexically_normal() == "/a/b/g/");

    VX_CHECK(os::path("../../..").lexically_normal() == "../../..");
    VX_CHECK(os::path("../../../").lexically_normal() == "../../..");
    VX_CHECK(os::path("../../../a/b/c").lexically_normal() == "../../../a/b/c");

    VX_CHECK(os::path("/../../..").lexically_normal() == "/");
    VX_CHECK(os::path("/../../../").lexically_normal() == "/");

    VX_CHECK(os::path("/../../../a/b/c").lexically_normal() == "/a/b/c");

    VX_CHECK(os::path("a/..").lexically_normal() == ".");
    VX_CHECK(os::path("a/../").lexically_normal() == ".");

    VX_CHECK(os::path("/////hello//world//a//b/c/././d/../../../..//..///other/x/y//z/.././../file.txt").lexically_normal() == "/hello/other/x/file.txt");
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(lexically_relative)
{
    VX_CHECK(os::path("").lexically_relative("") == ".");
    
    VX_CHECK(os::path("/a/d").lexically_relative("/a/b/c") == "../../d");
    VX_CHECK(os::path("/a/b/c").lexically_relative("/a/d") == "../b/c");
    VX_CHECK(os::path("a/b/c").lexically_relative("a") == "b/c");
    VX_CHECK(os::path("a/b/c").lexically_relative("a/b/c/x/y") == "../..");
    VX_CHECK(os::path("a/b/c").lexically_relative("a/b/c") == ".");
    VX_CHECK(os::path("a/b").lexically_relative("c/d") == "../../a/b");

#if defined(VX_TESTING_WINDOWS_PATH)

    VX_CHECK(os::path("C:/Temp").lexically_relative("D:/Temp") == "");
    VX_CHECK(os::path("C:/Temp").lexically_relative("Temp") == "");
    VX_CHECK(os::path("Temp").lexically_relative("C:/Temp") == "");
    VX_CHECK(os::path("C:Temp1").lexically_relative("C:Temp2") == "../Temp1");

#else

    VX_CHECK(os::path("/Temp").lexically_relative("Temp") == "");
    VX_CHECK(os::path("Temp").lexically_relative("/Temp") == "");
    VX_CHECK(os::path("/Temp1").lexically_relative("/Temp2") == "../Temp1");

#endif // VX_TESTING_WINDOWS_PATH

    VX_CHECK(os::path("one").lexically_relative("/two") == "");
    
    VX_CHECK(os::path("cat").lexically_relative("../../../meow") == "");
    VX_CHECK(os::path("cat").lexically_relative("../../../meow/././././.") == "");
    
    VX_CHECK(os::path("a/b/c/x/y/z").lexically_relative("a/b/c/d/./e/../f/g") == "../../../x/y/z");
    VX_CHECK(os::path("a/b/c/x/y/z").lexically_relative("a/b/c/d/./e/../f/g/../../..") == "x/y/z");

    // https://cplusplus.github.io/LWG/issue3070
    VX_CHECK(os::path("/a:/b:").lexically_relative("/a:/c:") == "../b:");

#if defined(VX_TESTING_WINDOWS_PATH)

    VX_CHECK(os::path(R"(\\?\a:\meow)").lexically_relative(R"(\\?\a:\meow)") == ".");
    VX_CHECK(os::path(R"(\\?\a:\meow\a\b)").lexically_relative(R"(\\?\a:\meow)") == "a/b");
    VX_CHECK(os::path(R"(\\?\a:\meow)").lexically_relative(R"(\\?\a:\meow\a\b)") == "../..");

    // UNC/DOS together should return an empty path
    VX_CHECK(os::path(R"(a:\meow)").lexically_relative(R"(\\?\a:\meow)") == "");
    VX_CHECK(os::path(R"(\\?\a:\meow)").lexically_relative(R"(a:\meow)") == "");

#endif // VX_TESTING_WINDOWS_PATH
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(lexically_proximate)
{
    VX_CHECK(os::path("").lexically_proximate("") == ".");

    VX_CHECK(os::path("/a/d").lexically_proximate("/a/b/c") == "../../d");
    VX_CHECK(os::path("/a/b/c").lexically_proximate("/a/d") == "../b/c");
    VX_CHECK(os::path("a/b/c").lexically_proximate("a") == "b/c");
    VX_CHECK(os::path("a/b/c").lexically_proximate("a/b/c/x/y") == "../..");
    VX_CHECK(os::path("a/b/c").lexically_proximate("a/b/c") == ".");
    VX_CHECK(os::path("a/b").lexically_proximate("c/d") == "../../a/b");

#if defined(VX_TESTING_WINDOWS_PATH)

    VX_CHECK(os::path("C:/Temp").lexically_proximate("D:/Temp") == "C:/Temp");
    VX_CHECK(os::path("C:/Temp").lexically_proximate("Temp") == "C:/Temp");
    VX_CHECK(os::path("Temp").lexically_proximate("C:/Temp") == "Temp");
    VX_CHECK(os::path("C:Temp1").lexically_proximate("C:Temp2") == "../Temp1");

#else

    VX_CHECK(os::path("/Temp").lexically_proximate("Temp") == "/Temp");
    VX_CHECK(os::path("Temp").lexically_proximate("/Temp") == "Temp");
    VX_CHECK(os::path("/Temp1").lexically_proximate("/Temp2") == "../Temp1");

#endif // VX_TESTING_WINDOWS_PATH

    VX_CHECK(os::path("one").lexically_proximate("/two") == "one");

    VX_CHECK(os::path("cat").lexically_proximate("../../../meow") == "cat");
    VX_CHECK(os::path("cat").lexically_proximate("../../../meow/././././.") == "cat");

    VX_CHECK(os::path("a/b/c/x/y/z").lexically_proximate("a/b/c/d/./e/../f/g") == "../../../x/y/z");
    VX_CHECK(os::path("a/b/c/x/y/z").lexically_proximate("a/b/c/d/./e/../f/g/../../..") == "x/y/z");

    // https://cplusplus.github.io/LWG/issue3070
    VX_CHECK(os::path("/a:/b:").lexically_proximate("/a:/c:") == "../b:");

#if defined(VX_TESTING_WINDOWS_PATH)

    VX_CHECK(os::path(R"(\\?\a:\meow)").lexically_proximate(R"(\\?\a:\meow)") == ".");
    VX_CHECK(os::path(R"(\\?\a:\meow\a\b)").lexically_proximate(R"(\\?\a:\meow)") == "a/b");
    VX_CHECK(os::path(R"(\\?\a:\meow)").lexically_proximate(R"(\\?\a:\meow\a\b)") == "../..");

    VX_CHECK(os::path(R"(a:\meow)").lexically_proximate(R"(\\?\a:\meow)") == R"(a:\meow)");
    VX_CHECK(os::path(R"(\\?\a:\meow)").lexically_proximate(R"(a:\meow)") == R"(\\?\a:\meow)");

#endif // VX_TESTING_WINDOWS_PATH
}

int main()
{
    VX_RUN_TESTS();
    return 0;
}