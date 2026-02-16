#include "vertex/std/string.hpp"
#include "vertex_test/test.hpp"

// https://github.com/microsoft/STL/blob/e2ef398685f7e470dbaeaf65ff919de72bda7489/tests/tr1/tests/string1/test.cpp

//=============================================================================

#define LIT(x)          VX_LIT(T, x)
#define CHECK_STR(a, b) VX_CHECK(a == b)

//=============================================================================

template <typename T>
static void test_container()
{
    using string_view = vx::str::basic_string_view<T>;
    T carr[] = { T('a'), T('b'), T('c'), T() };

    string_view v0;
    VX_CHECK(v0.empty());
    VX_CHECK(v0.size() == 0);

    string_view v1(carr, 3);
    VX_CHECK(v1.size() == 3);
    VX_CHECK(v1.end()[-1] == T('c'));

    string_view v2(v1);
    VX_CHECK(v2.size() == 3);
    VX_CHECK(*v2.begin() == T('a'));

    string_view v3(v1.data(), v1.size());
    VX_CHECK(v3.size() == 3);
    VX_CHECK(*v3.begin() == T('a'));

    const string_view v4(v1.data(), v1.size());
    VX_CHECK(v4.size() == 3);
    VX_CHECK(*v4.begin() == T('a'));

    v0 = v4;
    VX_CHECK(v0.size() == 3);
    VX_CHECK(v0[0] == T('a'));

    VX_SECTION("iterators")
    {
        typename string_view::iterator it(v0.begin());
        typename string_view::const_iterator cit(v4.begin());
        typename string_view::reverse_iterator rit(v0.rbegin());
        typename string_view::const_reverse_iterator crit(v4.rbegin());

        VX_CHECK(*it == T('a'));
        VX_CHECK(*--(it = v0.end()) == T('c'));

        VX_CHECK(*cit == T('a'));
        VX_CHECK(*--(cit = v4.end()) == T('c'));

        VX_CHECK(*rit == T('c'));
        VX_CHECK(*--(rit = v0.rend()) == T('a'));

        VX_CHECK(*crit == T('c'));
        VX_CHECK(*--(crit = v4.rend()) == T('a'));
    }

    VX_SECTION("const iterators")
    {
        typename string_view::const_iterator it(v0.cbegin());
        typename string_view::const_iterator cit(v4.cbegin());
        typename string_view::const_reverse_iterator rit(v0.crbegin());
        typename string_view::const_reverse_iterator crit(v4.crbegin());

        VX_CHECK(*it == T('a'));
        VX_CHECK(*--(it = v0.cend()) == T('c'));

        VX_CHECK(*cit == T('a'));
        VX_CHECK(*--(cit = v4.cend()) == T('c'));

        VX_CHECK(*rit == T('c'));
        VX_CHECK(*--(rit = v0.crend()) == T('a'));

        VX_CHECK(*crit == T('c'));
        VX_CHECK(*--(crit = v4.crend()) == T('a'));
    }

    VX_CHECK(v0.front() == T('a'));
    VX_CHECK(v0.back() == T('c'));

    VX_SECTION("swap")
    {
        T carr1[] = { T('h'), T('i'), T() };
        T carr2[] = { T('b'), T('y'), T('e'), T() };

        string_view a(carr1, 2);
        string_view b(carr2, 3);

        VX_CHECK(a.size() == 2);
        VX_CHECK(b.size() == 3);
        VX_CHECK(a.front() == T('h'));
        VX_CHECK(b.front() == T('b'));

        a.swap(b);
        VX_CHECK(a.size() == 3);
        VX_CHECK(b.size() == 2);
        VX_CHECK(a.front() == T('b'));
        VX_CHECK(b.front() == T('h'));

        std::swap(a, b);
        VX_CHECK(a.size() == 2);
        VX_CHECK(b.size() == 3);
        VX_CHECK(a.front() == T('h'));
        VX_CHECK(b.front() == T('b'));
    }
}

VX_TEST_CASE(container)
{
    VX_MESSAGE("  char");
    test_container<char>();

    VX_MESSAGE("  wchar_t");
    test_container<wchar_t>();

#if defined(__cpp_lib_char8_t)
    VX_MESSAGE("  char8_t");
    test_container<char8_t>();
#endif

    VX_MESSAGE("  char16_t");
    test_container<char16_t>();

    VX_MESSAGE("  char32_t");
    test_container<char32_t>();
}

//=============================================================================

template <typename T>
static void test_basics()
{
    using string_view = vx::str::basic_string_view<T>;

    string_view s1;
    string_view s2(LIT(""), 0);
    string_view s3(LIT("\0\0\0\0"), 4);
    string_view s4(LIT("s4"));
    string_view s5(LIT("s5xxx"), 2);
    string_view s6(LIT("aaa"));
    string_view s7(LIT("bbbbb"));
    string_view s8(LIT("c"));
    string_view s9(s7);

    VX_SECTION("constructors")
    {
        VX_CHECK(s1.empty());
        CHECK_STR(s2, LIT(""));
        VX_CHECK(vx::mem::compare(s3.data(), LIT("\0\0\0\0"), sizeof(T) * 4) == 0);
        CHECK_STR(s4, LIT("s4"));
        CHECK_STR(s5, LIT("s5"));
        CHECK_STR(s6, LIT("aaa"));
        CHECK_STR(s7, LIT("bbbbb"));
        CHECK_STR(s8, LIT("c"));
        VX_CHECK(s7.length() == 5);
        CHECK_STR(s9, LIT("bbbbb"));
    }

    VX_SECTION("assignment")
    {
        s1 = string_view(LIT("hello"));
        CHECK_STR(s1, LIT("hello"));
        s1 = s4;
        CHECK_STR(s1, LIT("s4"));
        s1 = string_view(LIT("AB"));
        CHECK_STR(s1, LIT("AB"));
        s1 = string_view(LIT("C"));
        CHECK_STR(s1, LIT("C"));
        s1 = s4;
        CHECK_STR(s1, LIT("s4"));
    }

    VX_SECTION("copy")
    {
        T buf[10]{};

        string_view s(LIT("012345"));
        VX_CHECK(s.copy(buf, vx::mem::array_size(buf)) == 6);
        VX_CHECK(vx::mem::compare(buf, LIT("012345"), sizeof(T) * 6) == 0);
        VX_CHECK(s.copy(buf, 3, 2) == 3);
        VX_CHECK(vx::mem::compare(buf, LIT("234"), sizeof(T) * 3) == 0);
        VX_CHECK(s.copy(buf, 99, 4) == 2);
        VX_CHECK(vx::mem::compare(buf, LIT("45"), sizeof(T) * 2) == 0);
        VX_CHECK(s.copy(buf, 5, 6) == 0);
        VX_CHECK(s.copy(buf, 5, 99) == 0);
    }

    VX_SECTION("substr")
    {
        string_view s(LIT("ABCDE"));

        auto a = s.substr();
        VX_CHECK(a.size() == 5);
        CHECK_STR(a, LIT("ABCDE"));
        auto b = s.substr(2);
        VX_CHECK(b.size() == 3);
        CHECK_STR(b, LIT("CDE"));
        auto c = s.substr(1, 2);
        VX_CHECK(c.size() == 2);
        CHECK_STR(c, LIT("BC"));
        auto d = s.substr(3, 99);
        VX_CHECK(d.size() == 2);
        CHECK_STR(d, LIT("DE"));
        auto e = s.substr(5);
        VX_CHECK(e.size() == 0);
        VX_CHECK(e.empty());
        auto f = s.substr(99);
        VX_CHECK(f.size() == 0);
        VX_CHECK(f.empty());
    }

    s1 = LIT("s4s4");

    // find
    {
        VX_SECTION("find")
        {
            VX_CHECK(s1.find(s4) == 0);
            VX_CHECK(s1.find(s4, 1) == 2);
            VX_CHECK(s1.find(s4, 3) == string_view::npos);
            VX_CHECK(s1.find(LIT("s4")) == 0);
            VX_CHECK(s1.find(LIT("s4"), 3) == string_view::npos);
            VX_CHECK(s1.find(LIT("s4XX"), 1, 2) == 2);
            VX_CHECK(s1.find(T('s')) == 0);
            VX_CHECK(s1.find(T('s'), 1) == 2);
            VX_CHECK(s1.find(T('x')) == string_view::npos);
        }

        VX_SECTION("rfind")
        {
            VX_CHECK(s1.rfind(s4) == 2);
            VX_CHECK(s1.rfind(s4, 1) == 0);
            VX_CHECK(s1.rfind(s5, 3) == string_view::npos);
            VX_CHECK(s1.rfind(LIT("s4")) == 2);
            VX_CHECK(s1.rfind(LIT("s4"), 3) == 2);
            VX_CHECK(s1.rfind(LIT("s4XX"), 1, 3) == string_view::npos);
            VX_CHECK(s1.rfind(T('s')) == 2);
            VX_CHECK(s1.rfind(T('s'), 2) == 2);
            VX_CHECK(s1.rfind(T('x')) == string_view::npos);
        }

        VX_SECTION("find_first_of")
        {
            VX_CHECK(s1.find_first_of(s4) == 0);
            VX_CHECK(s1.find_first_of(s4, 1) == 1);
            VX_CHECK(s1.find_first_of(s4, 4) == string_view::npos);
            VX_CHECK(s1.find_first_of(LIT("s4")) == 0);
            VX_CHECK(s1.find_first_of(LIT("s4"), 3) == 3);
            VX_CHECK(s1.find_first_of(LIT("abs"), 1, 2) == string_view::npos);
            VX_CHECK(s1.find_first_of(T('s')) == 0);
            VX_CHECK(s1.find_first_of(T('s'), 1) == 2);
            VX_CHECK(s1.find_first_of(T('x')) == string_view::npos);
        }

        VX_SECTION("find_last_of")
        {
            VX_CHECK(s1.find_last_of(s4) == 3);
            VX_CHECK(s1.find_last_of(s4, 1) == 1);
            VX_CHECK(s1.find_last_of(s6) == string_view::npos);
            VX_CHECK(s1.find_last_of(LIT("s4")) == 3);
            VX_CHECK(s1.find_last_of(LIT("s4"), 2) == 2);
            VX_CHECK(s1.find_last_of(LIT("abs"), 1, 2) == string_view::npos);
            VX_CHECK(s1.find_last_of(T('s')) == 2);
            VX_CHECK(s1.find_last_of(T('s'), 1) == 0);
            VX_CHECK(s1.find_last_of(T('x')) == string_view::npos);
        }

        VX_SECTION("find_first_not_of")
        {
            VX_CHECK(s1.find_first_not_of(s5) == 1);
            VX_CHECK(s1.find_first_not_of(s5, 2) == 3);
            VX_CHECK(s1.find_first_not_of(s4) == string_view::npos);
            VX_CHECK(s1.find_first_not_of(LIT("s5")) == 1);
            VX_CHECK(s1.find_first_not_of(LIT("s5"), 2) == 3);
            VX_CHECK(s1.find_first_not_of(LIT("s4a"), 1, 2) == string_view::npos);
            VX_CHECK(s1.find_first_not_of(T('s')) == 1);
            VX_CHECK(s1.find_first_not_of(T('s'), 2) == 3);
            VX_CHECK(s1.find_first_not_of(T('s'), 4) == string_view::npos);
        }

        VX_SECTION("find_last_not_of")
        {
            VX_CHECK(s1.find_last_not_of(s5) == 3);
            VX_CHECK(s1.find_last_not_of(s5, 2) == 1);
            VX_CHECK(s1.find_last_not_of(s4) == string_view::npos);
            VX_CHECK(s1.find_last_not_of(LIT("s5")) == 3);
            VX_CHECK(s1.find_last_not_of(LIT("s5"), 2) == 1);
            VX_CHECK(s1.find_last_not_of(LIT("s4a"), 1, 2) == string_view::npos);
            VX_CHECK(s1.find_last_not_of(T('s')) == 3);
            VX_CHECK(s1.find_last_not_of(T('s'), 2) == 1);
            VX_CHECK(s1.find_last_not_of(T('s'), 0) == string_view::npos);
        }
    }

    VX_SECTION("compare")
    {
        VX_CHECK(s1.compare(s1) == 0);
        VX_CHECK(s1.compare(0, 6, s1) == 0);
        VX_CHECK(s1.compare(2, 2, s4) == 0);
        VX_CHECK(s1.compare(2, 2, s4, 0, 2) == 0);
        VX_CHECK(s1.compare(LIT("s4s4")) == 0);
        VX_CHECK(s1.compare(0, 2, LIT("s4")) == 0);
        VX_CHECK(s1.compare(0, 2, LIT("s4"), 2) == 0);
        VX_CHECK(s1.compare(2, 2, LIT("s4xx"), 0, 2) == 0);
        VX_CHECK(s1.compare(s4) != 0);
        VX_CHECK(s1.compare(1, 4, s1) != 0);
        VX_CHECK(s1.compare(0, 4, s4, 0, 2) != 0);
        VX_CHECK(s1.compare(LIT("s4s5")) != 0);
        VX_CHECK(s1.compare(0, 4, LIT("s44"), 1) != 0);
        VX_CHECK(s1.compare(1, 4, LIT("s4xx"), 0, 2) != 0);

        VX_CHECK(s1 == s1);
        VX_CHECK(s1 == LIT("s4s4"));
        VX_CHECK(LIT("s4") == s4);
        VX_CHECK(s1 != s4);
        VX_CHECK(s1 != LIT("xx"));
        VX_CHECK(LIT("s4") != s1);

        VX_CHECK(s4 < s1);
        VX_CHECK(s4 < LIT("s5"));
        VX_CHECK(LIT("s3") < s4);
        VX_CHECK(s4 <= s1);
        VX_CHECK(s4 <= LIT("s5"));
        VX_CHECK(LIT("s3") <= s4);

        VX_CHECK(s1 > s4);
        VX_CHECK(LIT("s5") > s4);
        VX_CHECK(s4 > LIT("s3"));
        VX_CHECK(s1 >= s4);
        VX_CHECK(LIT("s5") >= s4);
        VX_CHECK(s4 >= LIT("s3"));
    }

    VX_SECTION("hash")
    {
        size_t hash_val = vx::hash<string_view>()(string_view(LIT("abc")));
        VX_UNUSED(hash_val);
    }
}

VX_TEST_CASE(basics)
{
    VX_MESSAGE("  char");
    test_basics<char>();

    VX_MESSAGE("  wchar_t");
    test_basics<wchar_t>();

#if defined(__cpp_lib_char8_t)
    VX_MESSAGE("  char8_t");
    test_basics<char8_t>();
#endif

    VX_MESSAGE("  char16_t");
    test_basics<char16_t>();

    VX_MESSAGE("  char32_t");
    test_basics<char32_t>();
}

//=============================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
