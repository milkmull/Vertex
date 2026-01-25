#include "vertex/std/char_traits.hpp"
#include "vertex_test/test.hpp"

//=============================================================================

VX_TEST_CASE(compare)
{
    VX_SECTION("char")
    {
        VX_CHECK(vx::str::compare("hello", "hello") == 0);
        VX_CHECK(vx::str::compare("hello1", "hello") > 0);
        VX_CHECK(vx::str::compare("hello", "hello1") < 0);
        VX_CHECK(vx::str::compare("", "") == 0);
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        VX_CHECK(vx::str::compare(u8"hello", u8"hello") == 0);
        VX_CHECK(vx::str::compare(u8"hello1", u8"hello") > 0);
        VX_CHECK(vx::str::compare(u8"hello", u8"hello1") < 0);
        VX_CHECK(vx::str::compare(u8"", u8"") == 0);
    }
#endif

    VX_SECTION("wchar_t")
    {
        VX_CHECK(vx::str::compare(L"hello", L"hello") == 0);
        VX_CHECK(vx::str::compare(L"hello1", L"hello") > 0);
        VX_CHECK(vx::str::compare(L"hello", L"hello1") < 0);
        VX_CHECK(vx::str::compare(L"", L"") == 0);
    }

    VX_SECTION("char32_t")
    {
        VX_CHECK(vx::str::compare(U"hello", U"hello") == 0);
        VX_CHECK(vx::str::compare(U"hello1", U"hello") > 0);
        VX_CHECK(vx::str::compare(U"hello", U"hello1") < 0);
        VX_CHECK(vx::str::compare(U"", U"") == 0);
    }
}

//=============================================================================

VX_TEST_CASE(compare_n)
{
    VX_SECTION("char")
    {
        VX_CHECK(vx::str::compare_n("hello", "hello", 5) == 0);
        VX_CHECK(vx::str::compare_n("hello1", "hello2", 5) == 0); // first 5 equal
        VX_CHECK(vx::str::compare_n("hello1", "hello", 6) > 0);   // '1' > '\0'
        VX_CHECK(vx::str::compare_n("hello", "hello1", 6) < 0);   // '\0' < '1'
        VX_CHECK(vx::str::compare_n("", "", 1) == 0);
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        VX_CHECK(vx::str::compare_n(u8"hello", u8"hello", 5) == 0);
        VX_CHECK(vx::str::compare_n(u8"hello1", u8"hello2", 5) == 0);
        VX_CHECK(vx::str::compare_n(u8"hello1", u8"hello", 6) > 0);
        VX_CHECK(vx::str::compare_n(u8"hello", u8"hello1", 6) < 0);
        VX_CHECK(vx::str::compare_n(u8"", u8"", 1) == 0);
    }
#endif

    VX_SECTION("wchar_t")
    {
        VX_CHECK(vx::str::compare_n(L"hello", L"hello", 5) == 0);
        VX_CHECK(vx::str::compare_n(L"hello1", L"hello2", 5) == 0);
        VX_CHECK(vx::str::compare_n(L"hello1", L"hello", 6) > 0);
        VX_CHECK(vx::str::compare_n(L"hello", L"hello1", 6) < 0);
        VX_CHECK(vx::str::compare_n(L"", L"", 1) == 0);
    }

    VX_SECTION("char32_t")
    {
        VX_CHECK(vx::str::compare_n(U"hello", U"hello", 5) == 0);
        VX_CHECK(vx::str::compare_n(U"hello1", U"hello2", 5) == 0);
        VX_CHECK(vx::str::compare_n(U"hello1", U"hello", 6) > 0);
        VX_CHECK(vx::str::compare_n(U"hello", U"hello1", 6) < 0);
        VX_CHECK(vx::str::compare_n(U"", U"", 1) == 0);
    }
}

//=============================================================================

VX_TEST_CASE(copy)
{
    VX_SECTION("char")
    {
        char dst[16] = {};
        VX_CHECK(vx::str::copy(dst, "hello") == dst);
        VX_CHECK(vx::str::compare(dst, "hello") == 0);
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        char8_t dst[16] = {};
        VX_CHECK(vx::str::copy(dst, u8"hello") == dst);
        VX_CHECK(vx::str::compare(dst, u8"hello") == 0);
    }
#endif

    VX_SECTION("wchar_t")
    {
        wchar_t dst[16] = {};
        VX_CHECK(vx::str::copy(dst, L"hello") == dst);
        VX_CHECK(vx::str::compare(dst, L"hello") == 0);
    }

    VX_SECTION("char32_t")
    {
        char32_t dst[16] = {};
        VX_CHECK(vx::str::copy(dst, U"hello") == dst);
        VX_CHECK(vx::str::compare(dst, U"hello") == 0);
    }
}

//=============================================================================

VX_TEST_CASE(copy_n)
{
    VX_SECTION("char")
    {
        char dst_n[16] = {};
        VX_CHECK(vx::str::copy_n(dst_n, "hello", 3) == dst_n);
        VX_CHECK(vx::str::compare(dst_n, "hel") == 0);
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        char8_t dst_n[16] = {};
        VX_CHECK(vx::str::copy_n(dst_n, u8"hello", 3) == dst_n);
        VX_CHECK(vx::str::compare(dst_n, u8"hel") == 0);
    }
#endif

    VX_SECTION("wchar_t")
    {
        wchar_t dst_n[16] = {};
        VX_CHECK(vx::str::copy_n(dst_n, L"hello", 3) == dst_n);
        VX_CHECK(vx::str::compare(dst_n, L"hel") == 0);
    }

    VX_SECTION("char32_t")
    {
        char32_t dst_n[16] = {};
        VX_CHECK(vx::str::copy_n(dst_n, U"hello", 3) == dst_n);
        VX_CHECK(vx::str::compare(dst_n, U"hel") == 0);
    }
}

//=============================================================================

VX_TEST_CASE(length)
{
    VX_SECTION("char")
    {
        VX_CHECK(vx::str::length("hello") == 5);
        VX_CHECK(vx::str::length("") == 0);
        VX_CHECK(vx::str::length("a") == 1);
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        VX_CHECK(vx::str::length(u8"hello") == 5);
        VX_CHECK(vx::str::length(u8"") == 0);
        VX_CHECK(vx::str::length(u8"a") == 1);
    }
#endif

    VX_SECTION("wchar_t")
    {
        VX_CHECK(vx::str::length(L"hello") == 5);
        VX_CHECK(vx::str::length(L"") == 0);
        VX_CHECK(vx::str::length(L"a") == 1);
    }

    VX_SECTION("char32_t")
    {
        VX_CHECK(vx::str::length(U"hello") == 5);
        VX_CHECK(vx::str::length(U"") == 0);
        VX_CHECK(vx::str::length(U"a") == 1);
    }
}

//=============================================================================

VX_TEST_CASE(concat)
{
    VX_SECTION("char")
    {
        char dst[32] = "hello";
        VX_CHECK(vx::str::concat(dst, " world") == dst);
        VX_CHECK(vx::str::compare(dst, "hello world") == 0);
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        char8_t dst[32] = u8"hello";
        VX_CHECK(vx::str::concat(dst, u8" world") == dst);
        VX_CHECK(vx::str::compare(dst, u8"hello world") == 0);
    }
#endif

    VX_SECTION("wchar_t")
    {
        wchar_t dst[32] = L"hello";
        VX_CHECK(vx::str::concat(dst, L" world") == dst);
        VX_CHECK(vx::str::compare(dst, L"hello world") == 0);
    }

    VX_SECTION("char32_t")
    {
        char32_t dst[32] = U"hello";
        VX_CHECK(vx::str::concat(dst, U" world") == dst);
        VX_CHECK(vx::str::compare(dst, U"hello world") == 0);
    }
}

//=============================================================================

VX_TEST_CASE(concat_n)
{
    VX_SECTION("char")
    {
        char dst_n[32] = "hello";
        VX_CHECK(vx::str::concat_n(dst_n, " world", 3) == dst_n);
        VX_CHECK(vx::str::compare(dst_n, "hello wo") == 0);
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        char8_t dst_n[32] = u8"hello";
        VX_CHECK(vx::str::concat_n(dst_n, u8" world", 3) == dst_n);
        VX_CHECK(vx::str::compare(dst_n, u8"hello wo") == 0);
    }
#endif

    VX_SECTION("wchar_t")
    {
        wchar_t dst_n[32] = L"hello";
        VX_CHECK(vx::str::concat_n(dst_n, L" world", 3) == dst_n);
        VX_CHECK(vx::str::compare(dst_n, L"hello wo") == 0);
    }

    VX_SECTION("char32_t")
    {
        char32_t dst_n[32] = U"hello";
        VX_CHECK(vx::str::concat_n(dst_n, U" world", 3) == dst_n);
        VX_CHECK(vx::str::compare(dst_n, U"hello wo") == 0);
    }
}

//=============================================================================

VX_TEST_CASE(find_char)
{
    VX_SECTION("char")
    {
        const char* s = "hello world";

        VX_CHECK(vx::str::find(s, 'h') == s);
        VX_CHECK(vx::str::find(s, 'o') == s + 4);
        VX_CHECK(vx::str::find(s, 'z') == nullptr);
        VX_CHECK(vx::str::find(s, '\0') == s + vx::str::length(s));
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        const char8_t* s = u8"hello world";

        VX_CHECK(vx::str::find(s, u8'h') == s);
        VX_CHECK(vx::str::find(s, u8'o') == s + 4);
        VX_CHECK(vx::str::find(s, u8'z') == nullptr);
        VX_CHECK(vx::str::find(s, char8_t()) == s + vx::str::length(s));
    }
#endif

    VX_SECTION("wchar_t")
    {
        const wchar_t* s = L"hello world";

        VX_CHECK(vx::str::find(s, L'h') == s);
        VX_CHECK(vx::str::find(s, L'o') == s + 4);
        VX_CHECK(vx::str::find(s, L'z') == nullptr);
        VX_CHECK(vx::str::find(s, wchar_t()) == s + vx::str::length(s));
    }

    VX_SECTION("char32_t")
    {
        const char32_t* s = U"hello world";

        VX_CHECK(vx::str::find(s, U'h') == s);
        VX_CHECK(vx::str::find(s, U'o') == s + 4);
        VX_CHECK(vx::str::find(s, U'z') == nullptr);
        VX_CHECK(vx::str::find(s, char32_t()) == s + vx::str::length(s));
    }
}

//=============================================================================

VX_TEST_CASE(find_substring)
{
    VX_SECTION("char")
    {
        const char* s = "hello world";

        VX_CHECK(vx::str::find(s, "hello") == s);
        VX_CHECK(vx::str::find(s, "world") == s + 6);
        VX_CHECK(vx::str::find(s, "lo wo") == s + 3);
        VX_CHECK(vx::str::find(s, "") == s);             // empty needle
        VX_CHECK(vx::str::find(s, "absent") == nullptr); // not found
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        const char8_t* s = u8"hello world";

        VX_CHECK(vx::str::find(s, u8"hello") == s);
        VX_CHECK(vx::str::find(s, u8"world") == s + 6);
        VX_CHECK(vx::str::find(s, u8"lo wo") == s + 3);
        VX_CHECK(vx::str::find(s, u8"") == s);
        VX_CHECK(vx::str::find(s, u8"absent") == nullptr);
    }
#endif

    VX_SECTION("wchar_t")
    {
        const wchar_t* s = L"hello world";

        VX_CHECK(vx::str::find(s, L"hello") == s);
        VX_CHECK(vx::str::find(s, L"world") == s + 6);
        VX_CHECK(vx::str::find(s, L"lo wo") == s + 3);
        VX_CHECK(vx::str::find(s, L"") == s);
        VX_CHECK(vx::str::find(s, L"absent") == nullptr);
    }

    VX_SECTION("char32_t")
    {
        const char32_t* s = U"hello world";

        VX_CHECK(vx::str::find(s, U"hello") == s);
        VX_CHECK(vx::str::find(s, U"world") == s + 6);
        VX_CHECK(vx::str::find(s, U"lo wo") == s + 3);
        VX_CHECK(vx::str::find(s, U"") == s);
        VX_CHECK(vx::str::find(s, U"absent") == nullptr);
    }
}

//=============================================================================

VX_TEST_CASE(rfind_char)
{
    VX_SECTION("char")
    {
        const char* s = "hello world";

        VX_CHECK(vx::str::rfind(s, 'h') == s);
        VX_CHECK(vx::str::rfind(s, 'o') == s + 7);
        VX_CHECK(vx::str::rfind(s, 'z') == nullptr);
        VX_CHECK(vx::str::rfind(s, '\0') == s + vx::str::length(s));
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        const char8_t* s = u8"hello world";

        VX_CHECK(vx::str::rfind(s, u8'h') == s);
        VX_CHECK(vx::str::rfind(s, u8'o') == s + 7);
        VX_CHECK(vx::str::rfind(s, u8'z') == nullptr);
        VX_CHECK(vx::str::rfind(s, char8_t()) == s + vx::str::length(s));
    }
#endif

    VX_SECTION("wchar_t")
    {
        const wchar_t* s = L"hello world";

        VX_CHECK(vx::str::rfind(s, L'h') == s);
        VX_CHECK(vx::str::rfind(s, L'o') == s + 7);
        VX_CHECK(vx::str::rfind(s, L'z') == nullptr);
        VX_CHECK(vx::str::rfind(s, wchar_t()) == s + vx::str::length(s));
    }

    VX_SECTION("char32_t")
    {
        const char32_t* s = U"hello world";

        VX_CHECK(vx::str::rfind(s, U'h') == s);
        VX_CHECK(vx::str::rfind(s, U'o') == s + 7);
        VX_CHECK(vx::str::rfind(s, U'z') == nullptr);
        VX_CHECK(vx::str::rfind(s, char32_t()) == s + vx::str::length(s));
    }
}

//=============================================================================

VX_TEST_CASE(rfind_substring)
{
    VX_SECTION("char")
    {
        const char* s = "hello hello world";

        VX_CHECK(vx::str::rfind(s, "hello") == s + 6);
        VX_CHECK(vx::str::rfind(s, "world") == s + 12);
        VX_CHECK(vx::str::rfind(s, "lo he") == s + 3);
        VX_CHECK(vx::str::rfind(s, "") == s + vx::str::length(s)); // empty needle
        VX_CHECK(vx::str::rfind(s, "absent") == nullptr);          // not found
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        const char8_t* s = u8"hello hello world";

        VX_CHECK(vx::str::rfind(s, u8"hello") == s + 6);
        VX_CHECK(vx::str::rfind(s, u8"world") == s + 12);
        VX_CHECK(vx::str::rfind(s, u8"lo he") == s + 3);
        VX_CHECK(vx::str::rfind(s, u8"") == s + vx::str::length(s));
        VX_CHECK(vx::str::rfind(s, u8"absent") == nullptr);
    }
#endif

    VX_SECTION("wchar_t")
    {
        const wchar_t* s = L"hello hello world";

        VX_CHECK(vx::str::rfind(s, L"hello") == s + 6);
        VX_CHECK(vx::str::rfind(s, L"world") == s + 12);
        VX_CHECK(vx::str::rfind(s, L"lo he") == s + 3);
        VX_CHECK(vx::str::rfind(s, L"") == s + vx::str::length(s));
        VX_CHECK(vx::str::rfind(s, L"absent") == nullptr);
    }

    VX_SECTION("char32_t")
    {
        const char32_t* s = U"hello hello world";

        VX_CHECK(vx::str::rfind(s, U"hello") == s + 6);
        VX_CHECK(vx::str::rfind(s, U"world") == s + 12);
        VX_CHECK(vx::str::rfind(s, U"lo he") == s + 3);
        VX_CHECK(vx::str::rfind(s, U"") == s + vx::str::length(s));
        VX_CHECK(vx::str::rfind(s, U"absent") == nullptr);
    }
}

//=============================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
