#include "vertex/std/char_traits.hpp"
#include "vertex_test/test.hpp"

//=============================================================================

VX_TEST_CASE(compare)
{
    VX_SECTION("char")
    {
        VX_CHECK(vx::str::_char_traits_priv::compare("hello", "hello") == 0);
        VX_CHECK(vx::str::_char_traits_priv::compare("hello1", "hello") > 0);
        VX_CHECK(vx::str::_char_traits_priv::compare("hello", "hello1") < 0);
        VX_CHECK(vx::str::_char_traits_priv::compare("", "") == 0);
    }

    VX_SECTION("wchar_t")
    {
        VX_CHECK(vx::str::_char_traits_priv::compare(L"hello", L"hello") == 0);
        VX_CHECK(vx::str::_char_traits_priv::compare(L"hello1", L"hello") > 0);
        VX_CHECK(vx::str::_char_traits_priv::compare(L"hello", L"hello1") < 0);
        VX_CHECK(vx::str::_char_traits_priv::compare(L"", L"") == 0);
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        VX_CHECK(vx::str::_char_traits_priv::compare(u8"hello", u8"hello") == 0);
        VX_CHECK(vx::str::_char_traits_priv::compare(u8"hello1", u8"hello") > 0);
        VX_CHECK(vx::str::_char_traits_priv::compare(u8"hello", u8"hello1") < 0);
        VX_CHECK(vx::str::_char_traits_priv::compare(u8"", u8"") == 0);
    }
#endif

    VX_SECTION("char16_t")
    {
        VX_CHECK(vx::str::_char_traits_priv::compare(u"hello", u"hello") == 0);
        VX_CHECK(vx::str::_char_traits_priv::compare(u"hello1", u"hello") > 0);
        VX_CHECK(vx::str::_char_traits_priv::compare(u"hello", u"hello1") < 0);
        VX_CHECK(vx::str::_char_traits_priv::compare(u"", u"") == 0);
    }

    VX_SECTION("char32_t")
    {
        VX_CHECK(vx::str::_char_traits_priv::compare(U"hello", U"hello") == 0);
        VX_CHECK(vx::str::_char_traits_priv::compare(U"hello1", U"hello") > 0);
        VX_CHECK(vx::str::_char_traits_priv::compare(U"hello", U"hello1") < 0);
        VX_CHECK(vx::str::_char_traits_priv::compare(U"", U"") == 0);
    }
}

//=============================================================================

VX_TEST_CASE(compare_n)
{
    VX_SECTION("char")
    {
        VX_CHECK(vx::str::_char_traits_priv::compare_n("hello", "hello", 5) == 0);
        VX_CHECK(vx::str::_char_traits_priv::compare_n("hello1", "hello2", 5) == 0); // first 5 equal
        VX_CHECK(vx::str::_char_traits_priv::compare_n("hello1", "hello", 6) > 0);   // '1' > '\0'
        VX_CHECK(vx::str::_char_traits_priv::compare_n("hello", "hello1", 6) < 0);   // '\0' < '1'
        VX_CHECK(vx::str::_char_traits_priv::compare_n("", "", 1) == 0);
    }

    VX_SECTION("wchar_t")
    {
        VX_CHECK(vx::str::_char_traits_priv::compare_n(L"hello", L"hello", 5) == 0);
        VX_CHECK(vx::str::_char_traits_priv::compare_n(L"hello1", L"hello2", 5) == 0);
        VX_CHECK(vx::str::_char_traits_priv::compare_n(L"hello1", L"hello", 6) > 0);
        VX_CHECK(vx::str::_char_traits_priv::compare_n(L"hello", L"hello1", 6) < 0);
        VX_CHECK(vx::str::_char_traits_priv::compare_n(L"", L"", 1) == 0);
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        VX_CHECK(vx::str::_char_traits_priv::compare_n(u8"hello", u8"hello", 5) == 0);
        VX_CHECK(vx::str::_char_traits_priv::compare_n(u8"hello1", u8"hello2", 5) == 0);
        VX_CHECK(vx::str::_char_traits_priv::compare_n(u8"hello1", u8"hello", 6) > 0);
        VX_CHECK(vx::str::_char_traits_priv::compare_n(u8"hello", u8"hello1", 6) < 0);
        VX_CHECK(vx::str::_char_traits_priv::compare_n(u8"", u8"", 1) == 0);
    }
#endif

    VX_SECTION("char16_t")
    {
        VX_CHECK(vx::str::_char_traits_priv::compare_n(u"hello", u"hello", 5) == 0);
        VX_CHECK(vx::str::_char_traits_priv::compare_n(u"hello1", u"hello2", 5) == 0);
        VX_CHECK(vx::str::_char_traits_priv::compare_n(u"hello1", u"hello", 6) > 0);
        VX_CHECK(vx::str::_char_traits_priv::compare_n(u"hello", u"hello1", 6) < 0);
        VX_CHECK(vx::str::_char_traits_priv::compare_n(u"", u"", 1) == 0);
    }

    VX_SECTION("char32_t")
    {
        VX_CHECK(vx::str::_char_traits_priv::compare_n(U"hello", U"hello", 5) == 0);
        VX_CHECK(vx::str::_char_traits_priv::compare_n(U"hello1", U"hello2", 5) == 0);
        VX_CHECK(vx::str::_char_traits_priv::compare_n(U"hello1", U"hello", 6) > 0);
        VX_CHECK(vx::str::_char_traits_priv::compare_n(U"hello", U"hello1", 6) < 0);
        VX_CHECK(vx::str::_char_traits_priv::compare_n(U"", U"", 1) == 0);
    }
}

//=============================================================================

VX_TEST_CASE(copy)
{
    VX_SECTION("char")
    {
        char dst[16] = {};
        VX_CHECK(vx::str::_char_traits_priv::copy(dst, "hello") == dst);
        VX_CHECK(vx::str::_char_traits_priv::compare(dst, "hello") == 0);
    }

    VX_SECTION("wchar_t")
    {
        wchar_t dst[16] = {};
        VX_CHECK(vx::str::_char_traits_priv::copy(dst, L"hello") == dst);
        VX_CHECK(vx::str::_char_traits_priv::compare(dst, L"hello") == 0);
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        char8_t dst[16] = {};
        VX_CHECK(vx::str::_char_traits_priv::copy(dst, u8"hello") == dst);
        VX_CHECK(vx::str::_char_traits_priv::compare(dst, u8"hello") == 0);
    }
#endif

    VX_SECTION("char16_t")
    {
        char16_t dst[16] = {};
        VX_CHECK(vx::str::_char_traits_priv::copy(dst, u"hello") == dst);
        VX_CHECK(vx::str::_char_traits_priv::compare(dst, u"hello") == 0);
    }

    VX_SECTION("char32_t")
    {
        char32_t dst[16] = {};
        VX_CHECK(vx::str::_char_traits_priv::copy(dst, U"hello") == dst);
        VX_CHECK(vx::str::_char_traits_priv::compare(dst, U"hello") == 0);
    }
}

//=============================================================================

VX_TEST_CASE(copy_n)
{
    VX_SECTION("char")
    {
        char dst_n[16] = {};
        VX_CHECK(vx::str::_char_traits_priv::copy_n(dst_n, "hello", 3) == dst_n);
        VX_CHECK(vx::str::_char_traits_priv::compare(dst_n, "hel") == 0);
    }

    VX_SECTION("wchar_t")
    {
        wchar_t dst_n[16] = {};
        VX_CHECK(vx::str::_char_traits_priv::copy_n(dst_n, L"hello", 3) == dst_n);
        VX_CHECK(vx::str::_char_traits_priv::compare(dst_n, L"hel") == 0);
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        char8_t dst_n[16] = {};
        VX_CHECK(vx::str::_char_traits_priv::copy_n(dst_n, u8"hello", 3) == dst_n);
        VX_CHECK(vx::str::_char_traits_priv::compare(dst_n, u8"hel") == 0);
    }
#endif

    VX_SECTION("char16_t")
    {
        char16_t dst_n[16] = {};
        VX_CHECK(vx::str::_char_traits_priv::copy_n(dst_n, u"hello", 3) == dst_n);
        VX_CHECK(vx::str::_char_traits_priv::compare(dst_n, u"hel") == 0);
    }

    VX_SECTION("char32_t")
    {
        char32_t dst_n[16] = {};
        VX_CHECK(vx::str::_char_traits_priv::copy_n(dst_n, U"hello", 3) == dst_n);
        VX_CHECK(vx::str::_char_traits_priv::compare(dst_n, U"hel") == 0);
    }
}

//=============================================================================

VX_TEST_CASE(assign)
{
    VX_SECTION("char")
    {
        char s1[] = "hello";
        vx::str::_char_traits_priv::assign(s1, 'x');
        VX_CHECK(vx::str::_char_traits_priv::compare(s1, "xxxxx") == 0);
    }

    VX_SECTION("wchar_t")
    {
        wchar_t s1[] = L"hello";
        vx::str::_char_traits_priv::assign(s1, L'x');
        VX_CHECK(vx::str::_char_traits_priv::compare(s1, L"xxxxx") == 0);
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        char8_t s1[] = u8"hello";
        vx::str::_char_traits_priv::assign(s1, u8'x');
        VX_CHECK(vx::str::_char_traits_priv::compare(s1, u8"xxxxx") == 0);
    }
#endif

    VX_SECTION("char16_t")
    {
        char16_t s1[] = u"hello";
        vx::str::_char_traits_priv::assign(s1, u'x');
        VX_CHECK(vx::str::_char_traits_priv::compare(s1, u"xxxxx") == 0);
    }

    VX_SECTION("char32_t")
    {
        char32_t s1[] = U"hello";
        vx::str::_char_traits_priv::assign(s1, U'x');
        VX_CHECK(vx::str::_char_traits_priv::compare(s1, U"xxxxx") == 0);
    }
}

//=============================================================================

VX_TEST_CASE(assign_char)
{
    VX_SECTION("char")
    {
        char s2[] = "hello";
        vx::str::_char_traits_priv::assign_n(s2, 3, 'y');
        VX_CHECK(vx::str::_char_traits_priv::compare_n(s2, "yyylo", 5) == 0);

        char s3[] = "hello";
        vx::str::_char_traits_priv::assign_n(s3, 0, 'z');
        VX_CHECK(vx::str::_char_traits_priv::compare(s3, "hello") == 0);
    }

    VX_SECTION("wchar_t")
    {
        wchar_t s2[] = L"hello";
        vx::str::_char_traits_priv::assign_n(s2, 3, L'y');
        VX_CHECK(vx::str::_char_traits_priv::compare_n(s2, L"yyylo", 5) == 0);

        wchar_t s3[] = L"hello";
        vx::str::_char_traits_priv::assign_n(s3, 0, L'z');
        VX_CHECK(vx::str::_char_traits_priv::compare(s3, L"hello") == 0);
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        char8_t s2[] = u8"hello";
        vx::str::_char_traits_priv::assign_n(s2, 3, u8'y');
        VX_CHECK(vx::str::_char_traits_priv::compare(s2, u8"yyylo") == 0);

        char8_t s3[] = u8"hello";
        vx::str::_char_traits_priv::assign_n(s3, 0, u8'z');
        VX_CHECK(vx::str::_char_traits_priv::compare(s3, u8"hello") == 0);
    }
#endif

    VX_SECTION("char16_t")
    {
        char16_t s2[] = u"hello";
        vx::str::_char_traits_priv::assign_n(s2, 3, u'y');
        VX_CHECK(vx::str::_char_traits_priv::compare_n(s2, u"yyylo", 5) == 0);

        char16_t s3[] = u"hello";
        vx::str::_char_traits_priv::assign_n(s3, 0, u'z');
        VX_CHECK(vx::str::_char_traits_priv::compare(s3, u"hello") == 0);
    }

    VX_SECTION("char32_t")
    {
        char32_t s2[] = U"hello";
        vx::str::_char_traits_priv::assign_n(s2, 3, U'y');
        VX_CHECK(vx::str::_char_traits_priv::compare_n(s2, U"yyylo", 5) == 0);

        char32_t s3[] = U"hello";
        vx::str::_char_traits_priv::assign_n(s3, 0, U'z');
        VX_CHECK(vx::str::_char_traits_priv::compare(s3, U"hello") == 0);
    }
}

//=============================================================================

VX_TEST_CASE(length)
{
    VX_SECTION("char")
    {
        VX_CHECK(vx::str::_char_traits_priv::length("hello") == 5);
        VX_CHECK(vx::str::_char_traits_priv::length("") == 0);
        VX_CHECK(vx::str::_char_traits_priv::length("a") == 1);
    }

    VX_SECTION("wchar_t")
    {
        VX_CHECK(vx::str::_char_traits_priv::length(L"hello") == 5);
        VX_CHECK(vx::str::_char_traits_priv::length(L"") == 0);
        VX_CHECK(vx::str::_char_traits_priv::length(L"a") == 1);
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        VX_CHECK(vx::str::_char_traits_priv::length(u8"hello") == 5);
        VX_CHECK(vx::str::_char_traits_priv::length(u8"") == 0);
        VX_CHECK(vx::str::_char_traits_priv::length(u8"a") == 1);
    }
#endif

    VX_SECTION("char16_t")
    {
        VX_CHECK(vx::str::_char_traits_priv::length(u"hello") == 5);
        VX_CHECK(vx::str::_char_traits_priv::length(u"") == 0);
        VX_CHECK(vx::str::_char_traits_priv::length(u"a") == 1);
    }

    VX_SECTION("char32_t")
    {
        VX_CHECK(vx::str::_char_traits_priv::length(U"hello") == 5);
        VX_CHECK(vx::str::_char_traits_priv::length(U"") == 0);
        VX_CHECK(vx::str::_char_traits_priv::length(U"a") == 1);
    }
}

//=============================================================================

VX_TEST_CASE(find_n_char)
{
    VX_SECTION("char")
    {
        const char* s = "hello world";

        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, 'h') == s);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, 'o') == s + 4);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, 'z') == nullptr);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, '\0') == s + vx::str::_char_traits_priv::length(s));

        // limited range
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 5, 'o') == s + 4);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 5, 'w') == nullptr);
    }

    VX_SECTION("wchar_t")
    {
        const wchar_t* s = L"hello world";

        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, L'h') == s);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, L'o') == s + 4);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, L'z') == nullptr);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, wchar_t()) == s + vx::str::_char_traits_priv::length(s));

        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 5, L'o') == s + 4);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 5, L'w') == nullptr);
    }

#if defined(__cpp_char8_t)
    VX_SECTION("char8_t")
    {
        const char8_t* s = u8"hello world";

        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, u8'h') == s);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, u8'o') == s + 4);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, u8'z') == nullptr);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, char8_t()) == s + vx::str::_char_traits_priv::length(s));

        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 5, u8'o') == s + 4);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 5, u8'w') == nullptr);
    }
#endif

    VX_SECTION("char16_t")
    {
        const char16_t* s = u"hello world";

        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, u'h') == s);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, u'o') == s + 4);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, u'z') == nullptr);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, char16_t()) == s + vx::str::_char_traits_priv::length(s));

        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 5, u'o') == s + 4);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 5, u'w') == nullptr);
    }

    VX_SECTION("char32_t")
    {
        const char32_t* s = U"hello world";

        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, U'h') == s);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, U'o') == s + 4);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, U'z') == nullptr);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 12, char32_t()) == s + vx::str::_char_traits_priv::length(s));

        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 5, U'o') == s + 4);
        VX_CHECK(vx::str::_char_traits_priv::find_n(s, 5, U'w') == nullptr);
    }
}

//=============================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
