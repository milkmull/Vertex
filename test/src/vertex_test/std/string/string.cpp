#include "vertex/std/string.hpp"
#include "vertex/std/string_utils.hpp"
#include "vertex_test/test.hpp"

// https://github.com/microsoft/STL/blob/e2ef398685f7e470dbaeaf65ff919de72bda7489/tests/tr1/tests/string1/test.cpp

//=============================================================================

#define LIT(x)          VX_LIT(T, x)
#define CHECK_STR(a, b) VX_CHECK(::vx::str::compare(a, b) == 0)

template <typename T>
static void test_traits()
{
    using string = vx::str::basic_string<T>;
    using traits = typename string::traits_type;

    T ch[] = { T('x'), T('1'), T('2'), T('3'), T('4'), T('5'), T('6'), T('7'), T('8'), T('9'), T() };

    traits::assign(ch[0], T('0'));
    VX_CHECK(ch[0] == T('0'));
    VX_CHECK(traits::eq(ch[0], ch[0]));
    VX_CHECK(!traits::eq(ch[0], T('4')));
    VX_CHECK(traits::lt(ch[0], T('4')));
    VX_CHECK(!traits::lt(ch[0], ch[0]));

    VX_CHECK(traits::compare(LIT("abc"), LIT("abcd"), 3) == 0);
    VX_CHECK(traits::compare(LIT("abc"), LIT("abcd"), 4) < 0);

    VX_CHECK(traits::length(LIT("")) == 0);
    VX_CHECK(traits::length(ch) == 10);

    VX_CHECK(traits::find(ch, 3, T('3')) == nullptr);
    VX_CHECK(*traits::find(LIT("abcd"), 4, T('d')) == T('d'));

    CHECK_STR(traits::move(ch, LIT("abc"), 0), LIT("0123456789"));
    CHECK_STR(traits::move(&ch[2], ch, 4), LIT("01236789"));
    CHECK_STR(traits::move(ch, &ch[2], 4), LIT("0123236789"));

    CHECK_STR(traits::copy(ch, LIT("abc"), 0), LIT("0123236789"));
    CHECK_STR(traits::copy(&ch[2], ch, 2), LIT("01236789"));

    CHECK_STR(traits::assign(ch, 2, T('3')), LIT("3301236789"));

    VX_CHECK(traits::not_eof(T('e')) == T('e'));
    VX_CHECK(traits::not_eof(EOF) != EOF);

    VX_CHECK(traits::to_char_type(T('x')) == T('x'));
    VX_CHECK(traits::to_int_type(T('x')) == T('x'));

    VX_CHECK(traits::eq_int_type(T('x'), T('x')));
    VX_CHECK(traits::eq_int_type(EOF, EOF));
    VX_CHECK(!traits::eq_int_type(T('x'), EOF));

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
    using string = vx::str::basic_string<T>;
    using alloc = vx::mem::default_allocator<T>;
    T carr[] = { T('a'), T('b'), T('c'), T() };

    string v0;
    alloc al = v0.get_allocator();
    string v0a(al);
    VX_CHECK(v0.empty());
    VX_CHECK(v0.size() == 0);
    VX_CHECK(v0a.size() == 0);
    VX_CHECK(v0a.get_allocator() == al);

    string v1(5, T());
    string v1a(6, T('x'));
    string v1b(7, T('y'), al);
    VX_CHECK(v1.size() == 5);
    VX_CHECK(v1.end()[-1] == T());
    VX_CHECK(v1a.size() == 6);
    VX_CHECK(v1a.end()[-1] == T('x'));
    VX_CHECK(v1b.size() == 7);
    VX_CHECK(v1b.end()[-1] == T('y'));

    string v2(v1a);
    VX_CHECK(v2.size() == 6);
    VX_CHECK(*v2.begin() == T('x'));

    string v2a(v2, al);
    VX_CHECK(v2a.size() == 6);
    VX_CHECK(*v2a.begin() == T('x'));

    string v3(v1a.begin(), v1a.end());
    VX_CHECK(v3.size() == 6);
    VX_CHECK(*v3.begin() == T('x'));

    const string v4(v1a.begin(), v1a.end(), al);
    VX_CHECK(v4.size() == 6);
    VX_CHECK(*v4.begin() == T('x'));
    v0 = v4;
    VX_CHECK(v0.size() == 6);
    VX_CHECK(*v0.begin() == T('x'));
    VX_CHECK(v0[0] == T('x'));

    v0.reserve(12);
    VX_CHECK(12 <= v0.capacity());
    v0.resize(8);
    VX_CHECK(v0.size() == 8);
    VX_CHECK(v0.end()[-1] == T());
    v0.resize(10, T('z'));
    VX_CHECK(v0.size() == 10);
    VX_CHECK(v0.end()[-1] == T('z'));
    VX_CHECK(v0.size() <= v0.max_size());

    VX_SECTION("iterators")
    {
        typename string::iterator p_it(v0.begin());
        typename string::const_iterator p_cit(v4.begin());
        typename string::reverse_iterator p_rit(v0.rbegin());
        typename string::const_reverse_iterator p_crit(v4.rbegin());
        VX_CHECK(*p_it == T('x'));
        VX_CHECK(*--(p_it = v0.end()) == T('z'));
        VX_CHECK(*p_cit == T('x'));
        VX_CHECK(*--(p_cit = v4.end()) == T('x'));
        VX_CHECK(*p_rit == T('z'));
        VX_CHECK(*--(p_rit = v0.rend()) == T('x'));
        VX_CHECK(*p_crit == T('x'));
        VX_CHECK(*--(p_crit = v4.rend()) == T('x'));
    }

    VX_SECTION("const iterators")
    {
        typename string::const_iterator p_it(v0.cbegin());
        typename string::const_iterator p_cit(v4.cbegin());
        typename string::const_reverse_iterator p_rit(v0.crbegin());
        typename string::const_reverse_iterator p_crit(v4.crbegin());
        VX_CHECK(*p_it == T('x'));
        VX_CHECK(*--(p_it = v0.cend()) == T('z'));
        VX_CHECK(*p_cit == T('x'));
        VX_CHECK(*--(p_cit = v4.cend()) == T('x'));
        VX_CHECK(*p_rit == T('z'));
        VX_CHECK(*--(p_rit = v0.crend()) == T('x'));
        VX_CHECK(*p_crit == T('x'));
        VX_CHECK(*--(p_crit = v4.crend()) == T('x'));
    }

    VX_CHECK(*v0.begin() == T('x'));
    VX_CHECK(*v4.begin() == T('x'));

    v0.push_back(T('a'));
    VX_CHECK(v0.end()[-1] == T('a'));

    v0.pop_back();
    VX_CHECK(v0.front().value() == T('x'));
    VX_CHECK(v0.back().value() == T('z'));

    v0.shrink_to_fit();
    VX_CHECK(v0.front().value() == T('x'));

    VX_SECTION("move")
    {
        VX_DISABLE_USE_AFTER_MOVE_WARNING();

        string v5(20, T('x'));
        string v6(std::move(v5));
        VX_CHECK(v5.empty());
        VX_CHECK(v6.size() == 20);

        VX_DISABLE_WARNING_POP();

        string v7;
        v7.assign(std::move(v6));
        VX_CHECK(v5.empty());
        VX_CHECK(v7.size() == 20);

        string v8;
        v8 = std::move(v7);
        VX_CHECK(v5.empty());
        VX_CHECK(v8.size() == 20);

        string v8a(std::move(v8));
        VX_CHECK(v5.empty());
        VX_CHECK(v8a.size() == 20);
    }

    {
        v0.assign(v4.begin(), v4.end());
        VX_CHECK(v0.size() == v4.size());
        VX_CHECK(*v0.begin() == *v4.begin());

        v0.assign(4, T('w'));
        VX_CHECK(v0.size() == 4);
        VX_CHECK(*v0.begin() == T('w'));

        VX_CHECK(*v0.insert(v0.begin(), T('a')) == T('a'));
        VX_CHECK(v0.size() == 5);
        VX_CHECK(*v0.begin() == T('a'));
        VX_CHECK(v0.begin()[1] == T('w'));

        VX_CHECK(*v0.insert(v0.begin(), 2, T('b')) == T('b'));
        VX_CHECK(v0.size() == 7);
        VX_CHECK(*v0.begin() == T('b'));
        VX_CHECK(v0.begin()[1] == T('b'));
        VX_CHECK(v0.begin()[2] == T('a'));

        VX_CHECK(*v0.insert(v0.end(), v4.begin(), v4.end()) == *v4.begin());
        VX_CHECK(v0.end()[-1] == v4.end()[-1]);

        VX_CHECK(*v0.insert(v0.end(), carr, carr + 3) == *carr);
        VX_CHECK(v0.end()[-1] == T('c'));

        v0.erase(v0.begin());
        VX_CHECK(*v0.begin() == T('b'));
        VX_CHECK(v0.begin()[1] == T('a'));

        v0.erase(v0.begin(), v0.begin() + 1);
        VX_CHECK(*v0.begin() == T('a'));
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
    using string = vx::str::basic_string<T>;

    string s1, s2(30, T());
    string s3(4, T());
    string s4(LIT("s4")), s5(LIT("s5xxx"), 2), s6(3, T('a'));
    string s7(5, T('b')), s8(1, T('c'));
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
        s1 = T('x');
        CHECK_STR(s1.c_str(), LIT("x"));
        s1 = s4;
        CHECK_STR(s1.c_str(), LIT("s4"));
        s1.assign(LIT("AB"));
        CHECK_STR(s1.c_str(), LIT("AB"));
        s1.assign(1, T('C'));
        CHECK_STR(s1.c_str(), LIT("C"));
        s1.assign(s4);
        CHECK_STR(s1.c_str(), LIT("s4"));
    }

    VX_SECTION("append")
    {
        s1 += LIT("abc");
        CHECK_STR(s1.c_str(), LIT("s4abc"));
        s1 += T('d');
        CHECK_STR(s1.c_str(), LIT("s4abcd"));
        s1 += s4;
        CHECK_STR(s1.c_str(), LIT("s4abcds4"));
        s1 = LIT("A");
        s1.append(LIT("BC"));
        CHECK_STR(s1.c_str(), LIT("ABC"));
        s1.append(1, T('D'));
        CHECK_STR(s1.c_str(), LIT("ABCD"));
        s1.append(s4);
        CHECK_STR(s1.c_str(), LIT("ABCDs4"));
        CHECK_STR((s4 + s5).c_str(), LIT("s4s5"));
        CHECK_STR((s4 + LIT("s5")).c_str(), LIT("s4s5"));
        CHECK_STR((LIT("s4") + s5).c_str(), LIT("s4s5"));
        CHECK_STR((s4 + T('5')).c_str(), LIT("s45"));
        CHECK_STR((T('4') + s5).c_str(), LIT("4s5"));

        VX_CHECK(s4 + LIT("more") == string(LIT("s4more")));
        VX_CHECK(LIT("more") + s4 == string(LIT("mores4")));
        VX_CHECK(s4 + T('+') == string(LIT("s4+")));
        VX_CHECK(T('+') + s4 == string(LIT("+s4")));

        {
            string s11(LIT("abc"));
            string s12(LIT("def"));

            CHECK_STR((s11 + string(LIT("def"))).c_str(), LIT("abcdef"));
            CHECK_STR((string(LIT("abc")) + s12).c_str(), LIT("abcdef"));
            CHECK_STR((string(LIT("abc")) + string(LIT("def"))).c_str(), LIT("abcdef"));

            CHECK_STR((string(LIT("abc")) + LIT("def")).c_str(), LIT("abcdef"));
            CHECK_STR((string(LIT("abc")) + T('d')).c_str(), LIT("abcd"));
            CHECK_STR((LIT("abc") + string(LIT("def"))).c_str(), LIT("abcdef"));
            CHECK_STR((T('a') + string(LIT("def"))).c_str(), LIT("adef"));

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
        s1.insert(7, 2, T('E'));
        s1.insert(4, 1, T('C'));
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
            VX_CHECK(s1.find(T('s')) == 0);
            VX_CHECK(s1.find(T('s'), 1) == 2);
            VX_CHECK(s1.find(T('x')) == string::npos);
        }

        VX_SECTION("rfind")
        {
            VX_CHECK(s1.rfind(s4) == 2);
            VX_CHECK(s1.rfind(s4, 1) == 0);
            VX_CHECK(s1.rfind(s5, 3) == string::npos);
            VX_CHECK(s1.rfind(LIT("s4")) == 2);
            VX_CHECK(s1.rfind(LIT("s4"), 3) == 2);
            VX_CHECK(s1.rfind(LIT("s4XX"), 1, 3) == string::npos);
            VX_CHECK(s1.rfind(T('s')) == 2);
            VX_CHECK(s1.rfind(T('s'), 2) == 2);
            VX_CHECK(s1.rfind(T('x')) == string::npos);
        }

        VX_SECTION("find_first_of")
        {
            VX_CHECK(s1.find_first_of(s4) == 0);
            VX_CHECK(s1.find_first_of(s4, 1) == 1);
            VX_CHECK(s1.find_first_of(s4, 4) == string::npos);
            VX_CHECK(s1.find_first_of(LIT("s4")) == 0);
            VX_CHECK(s1.find_first_of(LIT("s4"), 3) == 3);
            VX_CHECK(s1.find_first_of(LIT("abs"), 1, 2) == string::npos);
            VX_CHECK(s1.find_first_of(T('s')) == 0);
            VX_CHECK(s1.find_first_of(T('s'), 1) == 2);
            VX_CHECK(s1.find_first_of(T('x')) == string::npos);
        }

        VX_SECTION("find_last_of")
        {
            VX_CHECK(s1.find_last_of(s4) == 3);
            VX_CHECK(s1.find_last_of(s4, 1) == 1);
            VX_CHECK(s1.find_last_of(s6) == string::npos);
            VX_CHECK(s1.find_last_of(LIT("s4")) == 3);
            VX_CHECK(s1.find_last_of(LIT("s4"), 2) == 2);
            VX_CHECK(s1.find_last_of(LIT("abs"), 1, 2) == string::npos);
            VX_CHECK(s1.find_last_of(T('s')) == 2);
            VX_CHECK(s1.find_last_of(T('s'), 1) == 0);
            VX_CHECK(s1.find_last_of(T('x')) == string::npos);
        }

        VX_SECTION("find_first_not_of")
        {
            VX_CHECK(s1.find_first_not_of(s5) == 1);
            VX_CHECK(s1.find_first_not_of(s5, 2) == 3);
            VX_CHECK(s1.find_first_not_of(s4) == string::npos);
            VX_CHECK(s1.find_first_not_of(LIT("s5")) == 1);
            VX_CHECK(s1.find_first_not_of(LIT("s5"), 2) == 3);
            VX_CHECK(s1.find_first_not_of(LIT("s4a"), 1, 2) == string::npos);
            VX_CHECK(s1.find_first_not_of(T('s')) == 1);
            VX_CHECK(s1.find_first_not_of(T('s'), 2) == 3);
            VX_CHECK(s1.find_first_not_of(T('s'), 4) == string::npos);
        }

        VX_SECTION("find_last_not_of")
        {
            VX_CHECK(s1.find_last_not_of(s5) == 3);
            VX_CHECK(s1.find_last_not_of(s5, 2) == 1);
            VX_CHECK(s1.find_last_not_of(s4) == string::npos);
            VX_CHECK(s1.find_last_not_of(LIT("s5")) == 3);
            VX_CHECK(s1.find_last_not_of(LIT("s5"), 2) == 1);
            VX_CHECK(s1.find_last_not_of(LIT("s4a"), 1, 2) == string::npos);
            VX_CHECK(s1.find_last_not_of(T('s')) == 3);
            VX_CHECK(s1.find_last_not_of(T('s'), 2) == 1);
            VX_CHECK(s1.find_last_not_of(T('s'), 0) == string::npos);
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
        std::initializer_list<T> init{ T('a'), T('b'), T('c') };
        string s11(init);
        VX_CHECK(s11.size() == 3);
        VX_CHECK(s11[2] == T('c'));

        s11 += init;
        VX_CHECK(s11.size() == 6);
        VX_CHECK(s11[5] == T('c'));

        s11 = init;
        VX_CHECK(s11.size() == 3);
        VX_CHECK(s11[2] == T('c'));

        s11.append(init);
        VX_CHECK(s11.size() == 6);
        VX_CHECK(s11[5] == T('c'));

        s11.assign(init);
        VX_CHECK(s11.size() == 3);
        VX_CHECK(s11[2] == T('c'));

        VX_CHECK(*s11.insert(s11.begin() + 1, init) == *init.begin());
        VX_CHECK(s11.size() == 6);
        VX_CHECK(s11[2] == T('b'));

        s11.replace(s11.begin(), s11.begin() + 2, init);
        VX_CHECK(s11.size() == 7);
        VX_CHECK(s11[2] == T('c'));
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

//=========================================================================
// element access errors: front / back / at
//=========================================================================

static void test_element_access_errors()
{
    using string = vx::string;

    string empty_s;
    VX_CHECK_EXPECTED_ERROR(empty_s.front(), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(empty_s.back(), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(empty_s.at(0), vx::err::out_of_range);

    const string const_empty_s;
    VX_CHECK_EXPECTED_ERROR(const_empty_s.front(), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(const_empty_s.back(), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(const_empty_s.at(0), vx::err::out_of_range);

    string s("abc");
    VX_CHECK(s.front().value() == 'a');
    VX_CHECK(s.back().value() == 'c');
    VX_CHECK(s.at(1).value() == 'b');
    VX_CHECK_EXPECTED_ERROR(s.at(3), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(s.at(1000), vx::err::out_of_range);

    const string cs("def");
    VX_CHECK(cs.front().value() == 'd');
    VX_CHECK(cs.back().value() == 'f');
    VX_CHECK(cs.at(2).value() == 'f');
    VX_CHECK_EXPECTED_ERROR(cs.at(3), vx::err::out_of_range);

    // clearing empties it back out again
    s.clear();
    VX_CHECK_EXPECTED_ERROR(s.front(), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(s.back(), vx::err::out_of_range);
}

VX_TEST_CASE(element_access_errors)
{
    test_element_access_errors();
}

//=========================================================================
// erase() offset-based error handling
//=========================================================================

static void test_erase_errors()
{
    using string = vx::string;

    string s("abcde");

    // off is the only thing that's ever an error: off == size() and
    // off > size() are both invalid, regardless of overload
    VX_CHECK_EXPECTED_ERROR(s.erase(s.size() + 1), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(s.erase(1000), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(s.erase(s.size() + 1, 1), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(s.erase(1000, 1), vx::err::out_of_range);

    // string must be untouched by all the failed calls above
    VX_CHECK(s.size() == 5);
    VX_CHECK(s[0] == 'a');
    VX_CHECK(s[4] == 'e');

    // count running past the end is defined behavior, not an error:
    // it clamps to "erase through the end of the string" (mirrors npos)
    auto e1 = s.erase(3, 5);
    VX_CHECK(e1);
    VX_CHECK(s.size() == 3);
    VX_CHECK(s == "abc");

    auto e2 = s.erase(0, s.size() + 1);
    VX_CHECK(e2);
    VX_CHECK(s.empty());

    // sanity: valid single erase still succeeds
    s = "abcde";
    auto e3 = s.erase(1, 1);
    VX_CHECK(e3);
    VX_CHECK(s.size() == 4);
    VX_CHECK(s[1] == 'c'); // 'b' was removed

    // sanity: valid range erase still succeeds
    auto e4 = s.erase(0, 2);
    VX_CHECK(e4);
    VX_CHECK(s.size() == 2);

    // erase(off, 0) at a valid offset is a legal no-op, not an error
    auto e5 = s.erase(0, 0);
    VX_CHECK(e5);
    VX_CHECK(s.size() == 2);

    // default erase() (off=0, count=npos) erases everything
    auto e6 = s.erase();
    VX_CHECK(e6);
    VX_CHECK(s.empty());

    // erasing the empty string with default args is still valid
    auto e7 = s.erase();
    VX_CHECK(e7);
    VX_CHECK(s.empty());

    // an explicit count larger than what's left clamps rather than errors,
    // same as the npos default above but with a concrete finite value
    string s2("abcdef");
    auto e8 = s2.erase(4, 100);
    VX_CHECK(e8);
    VX_CHECK(s2 == "abcd");

    // off == size() (one-past-the-end) is valid: erasing "nothing left"
    string s3("abc");
    auto e9 = s3.erase(s3.size(), 5);
    VX_CHECK(e9);
    VX_CHECK(s3 == "abc");
}

VX_TEST_CASE(erase_errors)
{
    test_erase_errors();
}

//=========================================================================
// offset-based insert() error handling
//=========================================================================

static void test_offset_insert_errors()
{
    using string = vx::string;

    string s("abc");
    const string::size_type bad_off = s.size() + 1;

    VX_CHECK_EXPECTED_ERROR(s.insert(bad_off, 'x'), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(s.insert(bad_off, 3, 'x'), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(s.insert(bad_off, "xyz"), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(s.insert(bad_off, "xyz", 2), vx::err::out_of_range);

    std::initializer_list<char> init{ 'x', 'y', 'z' };
    VX_CHECK_EXPECTED_ERROR(s.insert(bad_off, init), vx::err::out_of_range);

    char carr[] = { 'p', 'q', 'r' };
    VX_CHECK_EXPECTED_ERROR(s.insert(bad_off, carr, carr + 3), vx::err::out_of_range);

    string other("QQ");
    VX_CHECK_EXPECTED_ERROR(s.insert(bad_off, other), vx::err::out_of_range);
    VX_CHECK_EXPECTED_ERROR(s.insert(bad_off, other, 0), vx::err::out_of_range);

    // string must be untouched by all the failed calls above
    VX_CHECK(s.size() == 3);
    VX_CHECK(s[0] == 'a');
    VX_CHECK(s[2] == 'c');

    // off == size() is one-past-the-end and IS valid (append)
    auto ok_append = s.insert(s.size(), 'z');
    VX_CHECK(ok_append);
    VX_CHECK(s.back().value() == 'z');

    // an invalid *source* offset on the other-string overload is not an
    // error as long as our own offset is valid: it's a documented no-op
    string s2("abc");
    auto noop = s2.insert(1, other, other.size() + 1);
    VX_CHECK(noop);
    VX_CHECK(s2.size() == 3); // untouched

    // sanity: valid offset insert still succeeds for every overload
    string v2("abc");
    auto i1 = v2.insert(0, 'A');
    VX_CHECK(i1);
    VX_CHECK(v2.front().value() == 'A');

    auto i2 = v2.insert(0, 2, 'B');
    VX_CHECK(i2);
    VX_CHECK(v2[0] == 'B');
    VX_CHECK(v2[1] == 'B');

    auto i3 = v2.insert(0, init);
    VX_CHECK(i3);
    VX_CHECK(v2[0] == 'x');

    auto i4 = v2.insert(0, carr, carr + 3);
    VX_CHECK(i4);
    VX_CHECK(v2[0] == 'p');

    auto i5 = v2.insert(0, other);
    VX_CHECK(i5);
    VX_CHECK(v2[0] == 'Q');
}

VX_TEST_CASE(offset_insert_errors)
{
    test_offset_insert_errors();
}

//=========================================================================
// replace() offset-based error handling
//=========================================================================

static void test_replace_errors()
{
    using string = vx::string;

    string s("abcdef");
    const string::size_type bad_off = s.size() + 1;

    string other("XY");
    VX_CHECK_ERROR(s.replace(bad_off, 2, other), vx::err::out_of_range);
    VX_CHECK_ERROR(s.replace(bad_off, 2, "xy"), vx::err::out_of_range);
    VX_CHECK_ERROR(s.replace(bad_off, 2, "xy", 2), vx::err::out_of_range);
    VX_CHECK_ERROR(s.replace(bad_off, 2, 2, 'x'), vx::err::out_of_range);

    std::initializer_list<char> init{ 'x', 'y' };
    VX_CHECK_ERROR(s.replace(bad_off, 2, init), vx::err::out_of_range);

    // valid target offset, but source offset (on the other-string overload)
    // is out of range
    VX_CHECK_ERROR(s.replace(0, 1, other, other.size() + 1), vx::err::out_of_range);

    // string must be untouched by all the failed calls above
    VX_CHECK(s.size() == 6);
    VX_CHECK(s == "abcdef");

    // sanity: valid replace calls still succeed
    VX_CHECK(s.replace(0, 2, "AB"));
    VX_CHECK(s == "ABcdef");

    VX_CHECK(s.replace(2, 2, 3, 'z'));
    VX_CHECK(s == "ABzzzef");

    VX_CHECK(s.replace(0, 2, other));
    VX_CHECK(s.compare(0, 2, "XY") == 0);
}

VX_TEST_CASE(replace_errors)
{
    test_replace_errors();
}

//=========================================================================
// capacity / size_error handling: reserve, resize, insert, replace overflow
//=========================================================================

static void test_size_error_handling()
{
    using string = vx::string;

    string s("abc");
    const string::size_type too_big = static_cast<string::size_type>(-1);

    VX_CHECK_ERROR(s.reserve(too_big), vx::err::size_error);
    VX_CHECK_ERROR(s.resize(too_big), vx::err::size_error);
    VX_CHECK_ERROR(s.resize(too_big, 'z'), vx::err::size_error);

    // string must be untouched by all the failed calls above
    VX_CHECK(s.size() == 3);
    VX_CHECK(s[0] == 'a');

    // insert_reallocate's own size_error check
    VX_CHECK_EXPECTED_ERROR(s.insert(0, too_big, 'x'), vx::err::size_error);

    // replace_n's own size_error check (huge insert side)
    VX_CHECK_ERROR(s.replace(0, 1, too_big, 'x'), vx::err::size_error);

    // sanity: reasonable reserve/resize calls still succeed
    VX_CHECK(s.reserve(100));
    VX_CHECK(s.capacity() >= 100);

    VX_CHECK(s.resize(10));
    VX_CHECK(s.size() == 10);

    VX_CHECK(s.resize(2));
    VX_CHECK(s.size() == 2);

    VX_CHECK(s.resize(5, 'q'));
    VX_CHECK(s.size() == 5);
    VX_CHECK(s.back().value() == 'q');
}

VX_TEST_CASE(size_error_handling)
{
    test_size_error_handling();
}

//=========================================================================
// assign() error handling
//=========================================================================

static void test_assign_errors()
{
    using string = vx::string;

    string s("abc");
    const string::size_type too_big = static_cast<string::size_type>(-1);

    // count > max_size() is checked before the source is ever touched,
    // so it's safe to pass small/short-lived sources here
    VX_CHECK_ERROR(s.assign(too_big, 'x'), vx::err::size_error);

    char carr[] = { 'p', 'q', 'r' };
    VX_CHECK_ERROR(s.assign(carr, too_big), vx::err::size_error);

    // string must be untouched by all the failed calls above
    VX_CHECK(s.size() == 3);
    VX_CHECK(s[0] == 'a');

    // sanity: valid assigns still succeed, growing, shrinking, and
    // exactly-matching-size cases (each takes a different internal branch)
    VX_CHECK(s.assign(5, 'w')); // grow: count > size
    VX_CHECK(s.size() == 5);
    VX_CHECK(s[0] == 'w');

    VX_CHECK(s.assign(5, 'w')); // exact match: count == size
    VX_CHECK(s.size() == 5);

    VX_CHECK(s.assign(2, 'w')); // shrink: count < size
    VX_CHECK(s.size() == 2);

    VX_CHECK(s.assign(carr, 3)); // pointer overload, grows via realloc
    VX_CHECK(s.size() == 3);
    VX_CHECK(s[0] == 'p');

    // count == 0 is a documented clear(), not an error
    VX_CHECK(s.assign(0, 'z'));
    VX_CHECK(s.empty());

    std::initializer_list<char> init{ 'd', 'e', 'f' };
    VX_CHECK(s.assign(init));
    VX_CHECK(s.size() == 3);

    string other("gh");
    VX_CHECK(s.assign(other));
    VX_CHECK(s.size() == 2);
    VX_CHECK(s[0] == 'g');

    // self-assign is a documented no-op / early-return success, not an error
    VX_CHECK(s.assign(s));
    VX_CHECK(s.size() == 2);
}

VX_TEST_CASE(assign_errors)
{
    test_assign_errors();
}

//=========================================================================
// fallible construction via basic_string::create()
//=========================================================================

static void test_fallible_construction()
{
    using string = vx::string;
    using alloc = vx::mem::default_allocator<char>;

    auto c0 = string::create();
    VX_CHECK(c0);
    VX_CHECK(c0.value().empty());

    alloc al;
    auto c0a = string::create(al);
    VX_CHECK(c0a);
    VX_CHECK(c0a.value().get_allocator() == al);

    auto c1 = string::create(5, 'x');
    VX_CHECK(c1);
    VX_CHECK(c1.value().size() == 5);
    VX_CHECK(c1.value().back().value() == 'x');

    auto c2 = string::create("hello");
    VX_CHECK(c2);
    VX_CHECK(c2.value().size() == 5);
    VX_CHECK(c2.value()[0] == 'h');

    auto c2a = string::create("hello world", 5);
    VX_CHECK(c2a);
    VX_CHECK(c2a.value().size() == 5);
    VX_CHECK(c2a.value() == "hello");

    std::initializer_list<char> init{ 'a', 'b', 'c' };
    auto c3 = string::create(init);
    VX_CHECK(c3);
    VX_CHECK(c3.value().size() == 3);
    VX_CHECK(c3.value()[2] == 'c');

    string src_copy("copy me");
    auto c4 = string::create(src_copy);
    VX_CHECK(c4);
    VX_CHECK(c4.value().size() == src_copy.size());
    VX_CHECK(c4.value()[0] == 'c');
    VX_CHECK(src_copy.size() == 7); // source untouched by a copy-create

    // sub-range creates
    auto c5 = string::create(src_copy, 5);
    VX_CHECK(c5);
    VX_CHECK(c5.value() == "me");

    auto c6 = string::create(src_copy, 0, 4);
    VX_CHECK(c6);
    VX_CHECK(c6.value() == "copy");

    string src_move("move me");
    auto c7 = string::create(std::move(src_move));
    VX_CHECK(c7);
    VX_CHECK(c7.value() == "move me");

    // pointer-forward iterator path (vx::string::iterator)
    string src_iter("abcd");
    auto c8 = string::create(src_iter.begin(), src_iter.end());
    VX_CHECK(c8);
    VX_CHECK(c8.value().size() == 4);
    VX_CHECK(c8.value()[3] == 'd');

    // generic (non-vx-pointer) iterator path, e.g. raw pointers / std types
    char carr[] = { 'e', 'f', 'g' };
    auto c9 = string::create(carr, carr + 3);
    VX_CHECK(c9);
    VX_CHECK(c9.value().size() == 3);
    VX_CHECK(c9.value()[0] == 'e');

    std::string std_src("std source");
    auto c10 = string::create(std_src.begin(), std_src.end());
    VX_CHECK(c10);
    VX_CHECK(c10.value().size() == std_src.size());
    VX_CHECK(c10.value()[0] == 's');

    // error paths: requesting more than max_size()
    const string::size_type too_big = static_cast<string::size_type>(-1);

    auto ce1 = string::create(too_big, 'x');
    VX_CHECK_EXPECTED_ERROR(ce1, vx::err::size_error);

    auto ce2 = string::create(carr, too_big);
    VX_CHECK_EXPECTED_ERROR(ce2, vx::err::size_error);
}

VX_TEST_CASE(fallible_construction)
{
    test_fallible_construction();
}

//=========================================================================
// empty-input edge cases (0-sized ranges shouldn't error or misbehave)
//=========================================================================

static void test_zero_sized_operations()
{
    using string = vx::string;

    // 0-count constructors/creates should succeed as empty strings
    string s0(0, 'x');
    VX_CHECK(s0.empty());

    auto c0 = string::create(0, 'x');
    VX_CHECK(c0);
    VX_CHECK(c0.value().empty());

    std::initializer_list<char> empty_init{};
    string s1(empty_init);
    VX_CHECK(s1.empty());

    // 0-count insert/replace/erase at valid offsets are no-ops, not errors
    string s("abc");
    auto ins0 = s.insert(1, 0, 'x');
    VX_CHECK(ins0);
    VX_CHECK(s.size() == 3);

    auto rep0 = s.replace(1, 0, 0, 'x');
    VX_CHECK(rep0);
    VX_CHECK(s.size() == 3);

    // assign(0, c) clears the string rather than erroring
    VX_CHECK(s.assign(0, 'x'));
    VX_CHECK(s.empty());

    // erasing an already-empty string with a 0-count range is a no-op
    auto e0 = s.erase(0, 0);
    VX_CHECK(e0);
    VX_CHECK(s.empty());
}

VX_TEST_CASE(zero_sized_operations)
{
    test_zero_sized_operations();
}

//=============================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
