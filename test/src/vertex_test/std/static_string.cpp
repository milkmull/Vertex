#include "vertex/std/static_string.hpp"
#include "vertex_test/test.hpp"

// https://github.com/microsoft/STL/blob/e2ef398685f7e470dbaeaf65ff919de72bda7489/tests/tr1/tests/string1/test.cpp

//=============================================================================

#define LIT(x)          VX_LIT(T, x)
#define CHECK_STR(a, b) VX_CHECK(::vx::str::compare(a, b) == 0)

//=============================================================================

template <typename T>
static void test_container()
{
    using string = vx::str::basic_static_string<20, T>;
    T carr[] = { 'a', 'b', 'c', '\0' };

    string v0;
    VX_CHECK(v0.empty());
    VX_CHECK(v0.size() == 0);

    string v1(5, '\0');
    string v1a(6, 'x');
    VX_CHECK(v1.size() == 5);
    VX_CHECK(v1.end()[-1] == '\0');
    VX_CHECK(v1a.size() == 6);
    VX_CHECK(v1a.end()[-1] == 'x');

    string v2(v1a);
    VX_CHECK(v2.size() == 6);
    VX_CHECK(*v2.begin() == 'x');

    string v3(v1a.begin(), v1a.end());
    VX_CHECK(v3.size() == 6);
    VX_CHECK(*v3.begin() == 'x');

    const string v4(v1a.begin(), v1a.end());
    VX_CHECK(v4.size() == 6);
    VX_CHECK(*v4.begin() == 'x');
    v0 = v4;
    VX_CHECK(v0.size() == 6);
    VX_CHECK(*v0.begin() == 'x');
    VX_CHECK(v0[0] == 'x');

    v0.reserve(12);
    VX_CHECK(12 <= v0.capacity());
    v0.resize(8);
    VX_CHECK(v0.size() == 8);
    VX_CHECK(v0.end()[-1] == '\0');
    v0.resize(10, 'z');
    VX_CHECK(v0.size() == 10);
    VX_CHECK(v0.end()[-1] == 'z');
    VX_CHECK(v0.size() <= v0.max_size());

    VX_SECTION("iterators")
    {
        // check iterators generators
        typename string::iterator p_it(v0.begin());
        typename string::const_iterator p_cit(v4.begin());
        typename string::reverse_iterator p_rit(v0.rbegin());
        typename string::const_reverse_iterator p_crit(v4.rbegin());
        VX_CHECK(*p_it == 'x');
        VX_CHECK(*--(p_it = v0.end()) == 'z');
        VX_CHECK(*p_cit == 'x');
        VX_CHECK(*--(p_cit = v4.end()) == 'x');
        VX_CHECK(*p_rit == 'z');
        VX_CHECK(*--(p_rit = v0.rend()) == 'x');
        VX_CHECK(*p_crit == 'x');
        VX_CHECK(*--(p_crit = v4.rend()) == 'x');
    }

    VX_SECTION("const iterators")
    {
        // check const iterators generators
        typename string::const_iterator p_it(v0.cbegin());
        typename string::const_iterator p_cit(v4.cbegin());
        typename string::const_reverse_iterator p_rit(v0.crbegin());
        typename string::const_reverse_iterator p_crit(v4.crbegin());
        VX_CHECK(*p_it == 'x');
        VX_CHECK(*--(p_it = v0.cend()) == 'z');
        VX_CHECK(*p_cit == 'x');
        VX_CHECK(*--(p_cit = v4.cend()) == 'x');
        VX_CHECK(*p_rit == 'z');
        VX_CHECK(*--(p_rit = v0.crend()) == 'x');
        VX_CHECK(*p_crit == 'x');
        VX_CHECK(*--(p_crit = v4.crend()) == 'x');
    }

    VX_CHECK(*v0.begin() == 'x');
    VX_CHECK(*v4.begin() == 'x');

    v0.push_back('a');
    VX_CHECK(v0.end()[-1] == 'a');

    v0.pop_back();
    VX_CHECK(v0.front() == 'x');
    VX_CHECK(v0.back() == 'z');

    v0.shrink_to_fit();
    VX_CHECK(v0.front() == 'x');

    VX_SECTION("move")
    {
        string v5(20, 'x');
        string v6(std::move(v5));
        VX_CHECK(v6.size() == 20);

        string v7;
        v7.assign(std::move(v6));
        VX_CHECK(v7.size() == 20);

        string v8;
        v8 = std::move(v7);
        VX_CHECK(v8.size() == 20);

        string v8a(std::move(v8));
        VX_CHECK(v8a.size() == 20);
    }

    {
        v0.assign(v4.begin(), v4.end());
        VX_CHECK(v0.size() == v4.size());
        VX_CHECK(*v0.begin() == *v4.begin());

        v0.assign(4, 'w');
        VX_CHECK(v0.size() == 4);
        VX_CHECK(*v0.begin() == 'w');

        VX_CHECK(*v0.insert(v0.begin(), 'a') == 'a');
        VX_CHECK(v0.size() == 5);
        VX_CHECK(*v0.begin() == 'a');
        VX_CHECK(v0.begin()[1] == 'w');

        VX_CHECK(*v0.insert(v0.begin(), 2, 'b') == 'b');
        VX_CHECK(v0.size() == 7);
        VX_CHECK(*v0.begin() == 'b');
        VX_CHECK(v0.begin()[1] == 'b');
        VX_CHECK(v0.begin()[2] == 'a');

        VX_CHECK(*v0.insert(v0.end(), v4.begin(), v4.end()) == *v4.begin());
        VX_CHECK(v0.end()[-1] == v4.end()[-1]);

        VX_CHECK(*v0.insert(v0.end(), carr, carr + 3) == *carr);
        VX_CHECK(v0.end()[-1] == 'c');

        v0.erase(v0.begin());
        VX_CHECK(*v0.begin() == 'b');
        VX_CHECK(v0.begin()[1] == 'a');

        v0.erase(v0.begin(), v0.begin() + 1);
        VX_CHECK(*v0.begin() == 'a');
    }

    {
        v0.clear();
        VX_CHECK(v0.empty());
        v0.swap(v1);
        VX_CHECK(!v0.empty());
        VX_CHECK(v1.empty());
        std::swap(v0, v1);
        VX_CHECK(v0.empty());
        VX_CHECK(!v1.empty());
        VX_CHECK(v1 == v1);
        VX_CHECK(v0 < v1);
        VX_CHECK(v0 != v1);
        VX_CHECK(v1 > v0);
        VX_CHECK(v0 <= v1);
        VX_CHECK(v1 >= v0);
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
    using string = vx::str::basic_static_string<20, T>;

    string s1, s2(30, '\0');
    string s3(4, '\0');
    string s4(LIT("s4")), s5(LIT("s5xxx"), 2), s6(3, 'a');
    string s7(5, 'b'), s8(1, 'c');
    string s9(s7);
    string s10(LIT("s5xxx"), 1, 4);

    VX_SECTION("constructors")
    {
        CHECK_STR(s1.c_str(), LIT(""));
        CHECK_STR(s2.c_str(), LIT(""));
        VX_CHECK(vx::mem::compare(s3.c_str(), LIT("\0\0\0\0"), sizeof(T) * 4) == 0);
        CHECK_STR(s4.c_str(), LIT("s4"));
        CHECK_STR(s5.c_str(), LIT("s5"));
        CHECK_STR(s6.c_str(), LIT("aaa"));
        CHECK_STR(s7.c_str(), LIT("bbbbb"));
        CHECK_STR(s8.c_str(), LIT("c"));
        VX_CHECK(s7.length() == 5);
        CHECK_STR(s9.c_str(), LIT("bbbbb"));
        CHECK_STR(s10.c_str(), LIT("5xxx"));
    }

    VX_SECTION("assignment")
    {
        s1 = LIT("hello");
        CHECK_STR(s1.c_str(), LIT("hello"));
        s1 = 'x';
        CHECK_STR(s1.c_str(), LIT("x"));
        s1 = s4;
        CHECK_STR(s1.c_str(), LIT("s4"));
        s1.assign(LIT("AB"));
        CHECK_STR(s1.c_str(), LIT("AB"));
        s1.assign(1, 'C');
        CHECK_STR(s1.c_str(), LIT("C"));
        s1.assign(s4);
        CHECK_STR(s1.c_str(), LIT("s4"));
    }

    VX_SECTION("append")
    {
        s1 += LIT("abc");
        CHECK_STR(s1.c_str(), LIT("s4abc"));
        s1 += 'd';
        CHECK_STR(s1.c_str(), LIT("s4abcd"));
        s1 += s4;
        CHECK_STR(s1.c_str(), LIT("s4abcds4"));
        s1 = LIT("A");
        s1.append(LIT("BC"));
        CHECK_STR(s1.c_str(), LIT("ABC"));
        s1.append(1, 'D');
        CHECK_STR(s1.c_str(), LIT("ABCD"));
        s1.append(s4);
        CHECK_STR(s1.c_str(), LIT("ABCDs4"));
        CHECK_STR((s4 + s5).c_str(), LIT("s4s5"));
        CHECK_STR((s4 + LIT("s5")).c_str(), LIT("s4s5"));
        CHECK_STR((LIT("s4") + s5).c_str(), LIT("s4s5"));
        CHECK_STR((s4 + '5').c_str(), LIT("s45"));
        CHECK_STR(('4' + s5).c_str(), LIT("4s5"));

        VX_CHECK(s4 + LIT("more") == string(LIT("s4more")));
        VX_CHECK(LIT("more") + s4 == string(LIT("mores4")));
        VX_CHECK(s4 + '+' == string(LIT("s4+")));
        VX_CHECK('+' + s4 == string(LIT("+s4")));

        {
            string s11(LIT("abc"));
            string s12(LIT("def"));

            CHECK_STR((s11 + string(LIT("def"))).c_str(), LIT("abcdef"));
            CHECK_STR((string(LIT("abc")) + s12).c_str(), LIT("abcdef"));
            CHECK_STR((string(LIT("abc")) + string(LIT("def"))).c_str(), LIT("abcdef"));

            CHECK_STR((string(LIT("abc")) + LIT("def")).c_str(), LIT("abcdef"));
            CHECK_STR((string(LIT("abc")) + 'd').c_str(), LIT("abcd"));
            CHECK_STR((LIT("abc") + string(LIT("def"))).c_str(), LIT("abcdef"));
            CHECK_STR(('a' + string(LIT("def"))).c_str(), LIT("adef"));

            string s13(std::move(s12));
            CHECK_STR(s13.c_str(), LIT("def"));
        }
    }

    VX_SECTION("insert")
    {
        s1 = LIT("abc");
        s1.insert(3, LIT("Dd"));
        s1.insert(1, LIT("BC"), 1);
        s1.insert(0, LIT("A"));
        CHECK_STR(s1.c_str(), LIT("AaBbcDd"));
        s1.insert(7, 2, 'E');
        s1.insert(4, 1, 'C');
        CHECK_STR(s1.c_str(), LIT("AaBbCcDdEE"));
        s1.insert(10, s4);
        s1.insert(0, s4, 0, 1);
        CHECK_STR(s1.c_str(), LIT("sAaBbCcDdEEs4"));
    }

    VX_SECTION("replace")
    {
        s1 = LIT("");
        s1.replace(0, 0, LIT("123ab789"));
        s1.replace(3, 2, LIT("45678"), 3);
        CHECK_STR(s1.c_str(), LIT("123456789"));
        s1.replace(1, 3, LIT("xx"), 2);
        s1.replace(0, 0, LIT("0"));
        CHECK_STR(s1.c_str(), LIT("01xx56789"));
        s1.replace(3, 1, s4, 1, 1);
        s1.replace(2, 1, s4);
        CHECK_STR(s1.c_str(), LIT("01s4456789"));
    }

    VX_SECTION("copy")
    {
        T buf[10]{};
        s1 = LIT("012XX");
        VX_CHECK(s1.copy(buf, vx::mem::array_size(buf)) == 5);
        VX_CHECK(vx::mem::compare(buf, LIT("012XX"), sizeof(T) * 5) == 0);
        VX_CHECK(s1.copy(buf, 3, 1) == 3);
        VX_CHECK(vx::mem::compare(buf, LIT("12X"), sizeof(T) * 3) == 0);
    }

    s1 = LIT("s4s4");

    // find
    {
        VX_SECTION("find")
        {
            VX_CHECK(s1.find(s4) == 0);
            VX_CHECK(s1.find(s4, 1) == 2);
            VX_CHECK(s1.find(s4, 3) == string::npos);
            VX_CHECK(s1.find(LIT("s4")) == 0);
            VX_CHECK(s1.find(LIT("s4"), 3) == string::npos);
            VX_CHECK(s1.find(LIT("s4XX"), 1, 2) == 2);
            VX_CHECK(s1.find('s') == 0);
            VX_CHECK(s1.find('s', 1) == 2);
            VX_CHECK(s1.find('x') == string::npos);
        }

        VX_SECTION("rfind")
        {
            VX_CHECK(s1.rfind(s4) == 2);
            VX_CHECK(s1.rfind(s4, 1) == 0);
            VX_CHECK(s1.rfind(s5, 3) == string::npos);
            VX_CHECK(s1.rfind(LIT("s4")) == 2);
            VX_CHECK(s1.rfind(LIT("s4"), 3) == 2);
            VX_CHECK(s1.rfind(LIT("s4XX"), 1, 3) == string::npos);
            VX_CHECK(s1.rfind('s') == 2);
            VX_CHECK(s1.rfind('s', 2) == 2);
            VX_CHECK(s1.rfind('x') == string::npos);
        }

        VX_SECTION("find_first_of")
        {
            VX_CHECK(s1.find_first_of(s4) == 0);
            VX_CHECK(s1.find_first_of(s4, 1) == 1);
            VX_CHECK(s1.find_first_of(s4, 4) == string::npos);
            VX_CHECK(s1.find_first_of(LIT("s4")) == 0);
            VX_CHECK(s1.find_first_of(LIT("s4"), 3) == 3);
            VX_CHECK(s1.find_first_of(LIT("abs"), 1, 2) == string::npos);
            VX_CHECK(s1.find_first_of('s') == 0);
            VX_CHECK(s1.find_first_of('s', 1) == 2);
            VX_CHECK(s1.find_first_of('x') == string::npos);
        }

        VX_SECTION("find_last_of")
        {
            VX_CHECK(s1.find_last_of(s4) == 3);
            VX_CHECK(s1.find_last_of(s4, 1) == 1);
            VX_CHECK(s1.find_last_of(s6) == string::npos);
            VX_CHECK(s1.find_last_of(LIT("s4")) == 3);
            VX_CHECK(s1.find_last_of(LIT("s4"), 2) == 2);
            VX_CHECK(s1.find_last_of(LIT("abs"), 1, 2) == string::npos);
            VX_CHECK(s1.find_last_of('s') == 2);
            VX_CHECK(s1.find_last_of('s', 1) == 0);
            VX_CHECK(s1.find_last_of('x') == string::npos);
        }

        VX_SECTION("find_first_not_of")
        {
            VX_CHECK(s1.find_first_not_of(s5) == 1);
            VX_CHECK(s1.find_first_not_of(s5, 2) == 3);
            VX_CHECK(s1.find_first_not_of(s4) == string::npos);
            VX_CHECK(s1.find_first_not_of(LIT("s5")) == 1);
            VX_CHECK(s1.find_first_not_of(LIT("s5"), 2) == 3);
            VX_CHECK(s1.find_first_not_of(LIT("s4a"), 1, 2) == string::npos);
            VX_CHECK(s1.find_first_not_of('s') == 1);
            VX_CHECK(s1.find_first_not_of('s', 2) == 3);
            VX_CHECK(s1.find_first_not_of('s', 4) == string::npos);
        }

        VX_SECTION("find_last_not_of")
        {
            VX_CHECK(s1.find_last_not_of(s5) == 3);
            VX_CHECK(s1.find_last_not_of(s5, 2) == 1);
            VX_CHECK(s1.find_last_not_of(s4) == string::npos);
            VX_CHECK(s1.find_last_not_of(LIT("s5")) == 3);
            VX_CHECK(s1.find_last_not_of(LIT("s5"), 2) == 1);
            VX_CHECK(s1.find_last_not_of(LIT("s4a"), 1, 2) == string::npos);
            VX_CHECK(s1.find_last_not_of('s') == 3);
            VX_CHECK(s1.find_last_not_of('s', 2) == 1);
            VX_CHECK(s1.find_last_not_of('s', 0) == string::npos);
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
        size_t hash_val = vx::hash<string>()(string(LIT("abc")));
        VX_UNUSED(hash_val);
    }

    VX_SECTION("initlializer_list")
    {
        std::initializer_list<T> init{ 'a', 'b', 'c' };
        string s11(init);
        VX_CHECK(s11.size() == 3);
        VX_CHECK(s11[2] == 'c');

        s11 += init;
        VX_CHECK(s11.size() == 6);
        VX_CHECK(s11[5] == 'c');

        s11 = init;
        VX_CHECK(s11.size() == 3);
        VX_CHECK(s11[2] == 'c');

        s11.append(init);
        VX_CHECK(s11.size() == 6);
        VX_CHECK(s11[5] == 'c');

        s11.assign(init);
        VX_CHECK(s11.size() == 3);
        VX_CHECK(s11[2] == 'c');

        VX_CHECK(*s11.insert(s11.begin() + 1, init) == *init.begin());
        VX_CHECK(s11.size() == 6);
        VX_CHECK(s11[2] == 'b');

        s11.replace(s11.begin(), s11.begin() + 2, init);
        VX_CHECK(s11.size() == 7);
        VX_CHECK(s11[2] == 'c');
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
