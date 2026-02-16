#include "vertex/std/string.hpp"
#include "vertex_test/test.hpp"

// https://github.com/microsoft/STL/blob/e2ef398685f7e470dbaeaf65ff919de72bda7489/tests/tr1/tests/string1/test.cpp

//=============================================================================

#define LIT(x, b)       VX_LIT(T, x, b)
#define LIT1(x)         VX_LIT(T, x, sb1)
#define LIT2(x)         VX_LIT(T, x, sb2)
#define LIT3(x)         VX_LIT(T, x, sb3)
#define CHECK_STR(a, b) VX_CHECK(::vx::str::compare(a, b) == 0)

template <typename T>
static void test_traits()
{
    using string = vx::basic_string<T>;
    using traits = typename string::traits_type;

    T ch[] = { 'x', '1', '2', '3', '4', '5', '6', '7', '8', '9', '\0' };
    T sb1[64]{};
    T sb2[64]{};

    traits::assign(ch[0], '0');
    VX_CHECK(ch[0] == '0');
    VX_CHECK(traits::eq(ch[0], ch[0]));
    VX_CHECK(!traits::eq(ch[0], '4'));
    VX_CHECK(traits::lt(ch[0], '4'));
    VX_CHECK(!traits::lt(ch[0], ch[0]));

    VX_CHECK(traits::compare(LIT("abc", sb1), LIT("abcd", sb2), 3) == 0);
    VX_CHECK(traits::compare(LIT("abc", sb1), LIT("abcd", sb2), 4) < 0);

    VX_CHECK(traits::length(LIT("", sb1)) == 0);
    VX_CHECK(traits::length(ch) == 10);

    VX_CHECK(traits::find(ch, 3, '3') == nullptr);
    VX_CHECK(*traits::find(LIT("abcd", sb1), 4, 'd') == 'd');

    CHECK_STR(traits::move(ch, LIT("abc", sb1), 0), LIT("0123456789", sb2));
    CHECK_STR(traits::move(&ch[2], ch, 4), LIT("01236789", sb2));
    CHECK_STR(traits::move(ch, &ch[2], 4), LIT("0123236789", sb2));

    CHECK_STR(traits::copy(ch, LIT("abc", sb1), 0), LIT("0123236789", sb2));
    CHECK_STR(traits::copy(&ch[2], ch, 2), LIT("01236789", sb2));

    CHECK_STR(traits::assign(ch, 2, '3'), LIT("3301236789", sb2));

    VX_CHECK(traits::not_eof('e') == 'e');
    VX_CHECK(traits::not_eof(EOF) != EOF);

    VX_CHECK(traits::to_char_type('x') == 'x');
    VX_CHECK(traits::to_int_type('x') == 'x');

    VX_CHECK(traits::eq_int_type('x', 'x'));
    VX_CHECK(traits::eq_int_type(EOF, EOF));
    VX_CHECK(!traits::eq_int_type('x', EOF));

    VX_CHECK(traits::eof() == EOF);
}

VX_TEST_CASE(traits)
{
    VX_MESSAGE("  char");
    test_traits<char>();

    VX_MESSAGE("  wchar_t");
    test_traits<wchar_t>();

#if defined(__cpp_lib_char8_t)
    VX_MESSAGE("  char8_t");
    test_traits<char8_t>();
#endif

    VX_MESSAGE("  char16_t");
    test_traits<char16_t>();

    VX_MESSAGE("  char32_t");
    test_traits<char32_t>();
}

//=============================================================================

template <typename T>
static void test_container()
{
    using string = vx::basic_string<T>;
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
        VX_CHECK(!v5.is_valid());
        VX_CHECK(v6.size() == 20);

        string v7;
        v7.assign(std::move(v6));
        VX_CHECK(!v5.is_valid());
        VX_CHECK(v7.size() == 20);

        string v8;
        v8 = std::move(v7);
        VX_CHECK(!v5.is_valid());
        VX_CHECK(v8.size() == 20);

        string v8a(std::move(v8));
        VX_CHECK(!v5.is_valid());
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
    using string = vx::basic_string<T>;

    T sb1[64]{};
    T sb2[64]{};
    T sb3[64]{};

    string s1, s2(30, '\0');
    string s3(4, '\0');
    string s4(LIT1("s4")), s5(LIT1("s5xxx"), 2), s6(3, 'a');
    string s7(5, 'b'), s8(1, 'c');
    string s9(s7);
    string s10(LIT1("s5xxx"), 1, 4);

    VX_SECTION("constructors")
    {
        CHECK_STR(s1.c_str(), LIT1(""));
        CHECK_STR(s2.c_str(), LIT1(""));
        VX_CHECK(vx::mem::compare(s3.c_str(), LIT1("\0\0\0\0"), sizeof(T) * 5));
        CHECK_STR(s4.c_str(), LIT1("s4"));
        CHECK_STR(s5.c_str(), LIT1("s5"));
        CHECK_STR(s6.c_str(), LIT1("aaa"));
        CHECK_STR(s7.c_str(), LIT1("bbbbb"));
        CHECK_STR(s8.c_str(), LIT1("c"));
        VX_CHECK(s7.length() == 5);
        CHECK_STR(s9.c_str(), LIT1("bbbbb"));
        CHECK_STR(s10.c_str(), LIT1("5xxx"));
    }

    VX_SECTION("assignment")
    {
        s1 = LIT1("hello");
        CHECK_STR(s1.c_str(), LIT1("hello"));
        s1 = 'x';
        CHECK_STR(s1.c_str(), LIT1("x"));
        s1 = s4;
        CHECK_STR(s1.c_str(), LIT1("s4"));
        s1.assign(LIT1("AB"));
        CHECK_STR(s1.c_str(), LIT1("AB"));
        s1.assign(1, 'C');
        CHECK_STR(s1.c_str(), LIT1("C"));
        s1.assign(s4);
        CHECK_STR(s1.c_str(), LIT1("s4"));
    }

    VX_SECTION("append")
    {
        s1 += LIT1("abc");
        CHECK_STR(s1.c_str(), LIT1("s4abc"));
        s1 += 'd';
        CHECK_STR(s1.c_str(), LIT1("s4abcd"));
        s1 += s4;
        CHECK_STR(s1.c_str(), LIT1("s4abcds4"));
        s1 = LIT1("A");
        s1.append(LIT1("BC"));
        CHECK_STR(s1.c_str(), LIT1("ABC"));
        s1.append(1, 'D');
        CHECK_STR(s1.c_str(), LIT1("ABCD"));
        s1.append(s4);
        CHECK_STR(s1.c_str(), LIT1("ABCDs4"));
        CHECK_STR((s4 + s5).c_str(), LIT1("s4s5"));
        CHECK_STR((s4 + LIT1("s5")).c_str(), LIT2("s4s5"));
        CHECK_STR((LIT1("s4") + s5).c_str(), LIT2("s4s5"));
        CHECK_STR((s4 + '5').c_str(), LIT1("s45"));
        CHECK_STR(('4' + s5).c_str(), LIT1("4s5"));

        VX_CHECK(s4 + LIT1("more") == string(LIT2("s4more")));
        VX_CHECK(LIT1("more") + s4 == string(LIT2("mores4")));
        VX_CHECK(s4 + '+' == string(LIT1("s4+")));
        VX_CHECK('+' + s4 == string(LIT1("+s4")));

        {
            string s11(LIT1("abc"));
            string s12(LIT1("def"));

            CHECK_STR((s11 + string(LIT1("def"))).c_str(), LIT2("abcdef"));
            CHECK_STR((string(LIT1("abc")) + s12).c_str(), LIT2("abcdef"));
            CHECK_STR((string(LIT1("abc")) + string(LIT2("def"))).c_str(), LIT3("abcdef"));

            CHECK_STR((string(LIT1("abc")) + LIT2("def")).c_str(), LIT3("abcdef"));
            CHECK_STR((string(LIT1("abc")) + 'd').c_str(), LIT2("abcd"));
            CHECK_STR((LIT1("abc") + string(LIT2("def"))).c_str(), LIT3("abcdef"));
            CHECK_STR(('a' + string(LIT2("def"))).c_str(), LIT3("adef"));

            string s13(std::move(s12));
            CHECK_STR(s13.c_str(), LIT1("def"));
        }
    }

    VX_SECTION("insert")
    {
        s1 = LIT1("abc");
        s1.insert(3, LIT1("Dd"));
        s1.insert(1, LIT1("BC"), 1);
        s1.insert(0, LIT1("A"));
        CHECK_STR(s1.c_str(), LIT1("AaBbcDd"));
        s1.insert(7, 2, 'E');
        s1.insert(4, 1, 'C');
        CHECK_STR(s1.c_str(), LIT1("AaBbCcDdEE"));
        s1.insert(10, s4);
        s1.insert(0, s4, 0, 1);
        CHECK_STR(s1.c_str(), LIT1("sAaBbCcDdEEs4"));
    }

    VX_SECTION("replace")
    {
        s1 = LIT1("");
        s1.replace(0, 0, LIT1("123ab789"));
        s1.replace(3, 2, LIT1("45678"), 3);
        CHECK_STR(s1.c_str(), LIT1("123456789"));
        s1.replace(1, 3, LIT1("xx"), 2);
        s1.replace(0, 0, LIT1("0"));
        CHECK_STR(s1.c_str(), LIT1("01xx56789"));
        s1.replace(3, 1, s4, 1, 1);
        s1.replace(2, 1, s4);
        CHECK_STR(s1.c_str(), LIT1("01s4456789"));
    }

    VX_SECTION("copy")
    {
        T buf[10]{};
        s1 = LIT1("012XX");
        VX_CHECK(s1.copy(buf, vx::mem::array_size(buf)) == 5);
        VX_CHECK(vx::mem::compare(buf, LIT1("012XX"), sizeof(T) * 5) == 0);
        VX_CHECK(s1.copy(buf, 3, 1) == 3);
        VX_CHECK(vx::mem::compare(buf, LIT1("12X"), sizeof(T) * 3) == 0);
    }

    s1 = LIT1("s4s4");

    // find
    {
        VX_SECTION("find")
        {
            VX_CHECK(s1.find(s4) == 0);
            VX_CHECK(s1.find(s4, 1) == 2);
            VX_CHECK(s1.find(s4, 3) == string::npos);
            VX_CHECK(s1.find(LIT1("s4")) == 0);
            VX_CHECK(s1.find(LIT1("s4"), 3) == string::npos);
            VX_CHECK(s1.find(LIT1("s4XX"), 1, 2) == 2);
            VX_CHECK(s1.find('s') == 0);
            VX_CHECK(s1.find('s', 1) == 2);
            VX_CHECK(s1.find('x') == string::npos);
        }

        VX_SECTION("rfind")
        {
            VX_CHECK(s1.rfind(s4) == 2);
            VX_CHECK(s1.rfind(s4, 1) == 0);
            VX_CHECK(s1.rfind(s5, 3) == string::npos);
            VX_CHECK(s1.rfind(LIT1("s4")) == 2);
            VX_CHECK(s1.rfind(LIT1("s4"), 3) == 2);
            VX_CHECK(s1.rfind(LIT1("s4XX"), 1, 3) == string::npos);
            VX_CHECK(s1.rfind('s') == 2);
            VX_CHECK(s1.rfind('s', 2) == 2);
            VX_CHECK(s1.rfind('x') == string::npos);
        }

        VX_SECTION("find_first_of")
        {
            VX_CHECK(s1.find_first_of(s4) == 0);
            VX_CHECK(s1.find_first_of(s4, 1) == 1);
            VX_CHECK(s1.find_first_of(s4, 4) == string::npos);
            VX_CHECK(s1.find_first_of(LIT1("s4")) == 0);
            VX_CHECK(s1.find_first_of(LIT1("s4"), 3) == 3);
            VX_CHECK(s1.find_first_of(LIT1("abs"), 1, 2) == string::npos);
            VX_CHECK(s1.find_first_of('s') == 0);
            VX_CHECK(s1.find_first_of('s', 1) == 2);
            VX_CHECK(s1.find_first_of('x') == string::npos);
        }

        VX_SECTION("find_last_of")
        {
            VX_CHECK(s1.find_last_of(s4) == 3);
            VX_CHECK(s1.find_last_of(s4, 1) == 1);
            VX_CHECK(s1.find_last_of(s6) == string::npos);
            VX_CHECK(s1.find_last_of(LIT1("s4")) == 3);
            VX_CHECK(s1.find_last_of(LIT1("s4"), 2) == 2);
            VX_CHECK(s1.find_last_of(LIT1("abs"), 1, 2) == string::npos);
            VX_CHECK(s1.find_last_of('s') == 2);
            VX_CHECK(s1.find_last_of('s', 1) == 0);
            VX_CHECK(s1.find_last_of('x') == string::npos);
        }

        VX_SECTION("find_first_not_of")
        {
            VX_CHECK(s1.find_first_not_of(s5) == 1);
            VX_CHECK(s1.find_first_not_of(s5, 2) == 3);
            VX_CHECK(s1.find_first_not_of(s4) == string::npos);
            VX_CHECK(s1.find_first_not_of(LIT1("s5")) == 1);
            VX_CHECK(s1.find_first_not_of(LIT1("s5"), 2) == 3);
            VX_CHECK(s1.find_first_not_of(LIT1("s4a"), 1, 2) == string::npos);
            VX_CHECK(s1.find_first_not_of('s') == 1);
            VX_CHECK(s1.find_first_not_of('s', 2) == 3);
            VX_CHECK(s1.find_first_not_of('s', 4) == string::npos);
        }

        VX_SECTION("find_last_not_of")
        {
            VX_CHECK(s1.find_last_not_of(s5) == 3);
            VX_CHECK(s1.find_last_not_of(s5, 2) == 1);
            VX_CHECK(s1.find_last_not_of(s4) == string::npos);
            VX_CHECK(s1.find_last_not_of(LIT1("s5")) == 3);
            VX_CHECK(s1.find_last_not_of(LIT1("s5"), 2) == 1);
            VX_CHECK(s1.find_last_not_of(LIT1("s4a"), 1, 2) == string::npos);
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
        VX_CHECK(s1.compare(LIT1("s4s4")) == 0);
        VX_CHECK(s1.compare(0, 2, LIT1("s4")) == 0);
        VX_CHECK(s1.compare(0, 2, LIT1("s4"), 2) == 0);
        VX_CHECK(s1.compare(2, 2, LIT1("s4xx"), 0, 2) == 0);
        VX_CHECK(s1.compare(s4) != 0);
        VX_CHECK(s1.compare(1, 4, s1) != 0);
        VX_CHECK(s1.compare(0, 4, s4, 0, 2) != 0);
        VX_CHECK(s1.compare(LIT1("s4s5")) != 0);
        VX_CHECK(s1.compare(0, 4, LIT1("s44"), 1) != 0);
        VX_CHECK(s1.compare(1, 4, LIT1("s4xx"), 0, 2) != 0);

        VX_CHECK(s1 == s1);
        VX_CHECK(s1 == LIT1("s4s4"));
        VX_CHECK(LIT1("s4") == s4);
        VX_CHECK(s1 != s4);
        VX_CHECK(s1 != LIT1("xx"));
        VX_CHECK(LIT1("s4") != s1);

        VX_CHECK(s4 < s1);
        VX_CHECK(s4 < LIT1("s5"));
        VX_CHECK(LIT1("s3") < s4);
        VX_CHECK(s4 <= s1);
        VX_CHECK(s4 <= LIT1("s5"));
        VX_CHECK(LIT1("s3") <= s4);

        VX_CHECK(s1 > s4);
        VX_CHECK(LIT1("s5") > s4);
        VX_CHECK(s4 > LIT1("s3"));
        VX_CHECK(s1 >= s4);
        VX_CHECK(LIT1("s5") >= s4);
        VX_CHECK(s4 >= LIT1("s3"));
    }

    VX_SECTION("hash")
    {
        size_t hash_val = vx::hash<string>()(string(LIT1("abc")));
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
