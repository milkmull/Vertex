#include "vertex/std/string.hpp"
#include "vertex/std/string_utils.hpp"
#include "vertex/std/vector.hpp"

#include <cstddef>
#include <initializer_list>
#include <list>
#include <string>

#include "vertex_test/test.hpp"

#define ONLY_BASIC_TESTS

// https://github.com/microsoft/STL/blob/e2ef398685f7e470dbaeaf65ff919de72bda7489/tests/tr1/tests/string1/test.cpp

//=============================================================================
// helpers
//=============================================================================

#define LIT(x)          VX_LIT(T, x)
#define CHECK_STR(a, b) VX_CHECK(::vx::str::compare(a, b) == 0)

// string built from a literal (needs `string` and `T` in scope)
#define STR(x) string(LIT(x))

// exact comparison: size, contents and null terminator
#define EQ(s, x) VX_CHECK(str_eq((s), LIT(x)))

namespace {

template <typename S>
bool str_eq(const S& s, const typename S::value_type* lit)
{
    using T = typename S::value_type;
    size_t n = 0;
    while (lit[n] != T())
    {
        ++n;
    }
    if (s.size() != n || s.c_str()[n] != T())
    {
        return false;
    }
    for (size_t i = 0; i < n; ++i)
    {
        if (s.c_str()[i] != lit[i])
        {
            return false;
        }
    }
    return true;
}

// class invariants that must hold after every operation
template <typename S>
bool valid(const S& s)
{
    using T = typename S::value_type;
    return s.c_str() != nullptr && s.data() == s.c_str() && s.c_str()[s.size()] == T() && s.capacity() >= s.size();
}

template <typename S, typename M>
bool eq_model(const S& s, const M& m)
{
    if (!valid(s) || s.size() != m.size())
    {
        return false;
    }
    for (size_t i = 0; i < m.size(); ++i)
    {
        if (s.c_str()[i] != m[i])
        {
            return false;
        }
    }
    return true;
}

template <typename T>
bool buf_eq(const T* a, const T* b, size_t n)
{
    for (size_t i = 0; i < n; ++i)
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}

template <typename T>
std::basic_string<T> ascii(const char* a)
{
    std::basic_string<T> r;
    for (; *a; ++a)
    {
        r.push_back(static_cast<T>(*a));
    }
    return r;
}

inline int sgn(int x)
{
    return (x > 0) - (x < 0);
}

} // namespace

#if defined(__cpp_lib_char8_t)
    #define TEST_CHAR8(fn) \
        VX_MESSAGE("  char8_t"); \
        fn<char8_t>();
#else
    #define TEST_CHAR8(fn)
#endif

// Runs test_<name><T>() for every character type
#define TEST_CHARS(name) \
    VX_TEST_CASE(name) \
    { \
        VX_MESSAGE("  char"); \
        test_##name<char>(); \
        VX_MESSAGE("  wchar_t"); \
        test_##name<wchar_t>(); \
        TEST_CHAR8(test_##name) \
        VX_MESSAGE("  char16_t"); \
        test_##name<char16_t>(); \
        VX_MESSAGE("  char32_t"); \
        test_##name<char32_t>(); \
    }

//=============================================================================
// original tests (unchanged)
//=============================================================================

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

TEST_CHARS(traits)

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
    VX_CHECK(v0.front() == T('x'));
    VX_CHECK(v0.back() == T('z'));

    v0.shrink_to_fit();
    VX_CHECK(v0.front() == T('x'));

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

TEST_CHARS(container)

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

TEST_CHARS(basics)

#if !defined(ONLY_BASIC_TESTS)

//=============================================================================
// construction
//=============================================================================

template <typename T>
static void test_ctor()
{
    using string = vx::str::basic_string<T>;
    using allocator = typename string::allocator_type;

    VX_SECTION("default / allocator")
    {
        string s;
        VX_CHECK(s.empty());
        VX_CHECK(s.size() == 0);
        VX_CHECK(s.length() == 0);
        VX_CHECK(s.capacity() == 0);
        VX_CHECK(valid(s));
        VX_CHECK(s.begin() == s.end());
        EQ(s, "");

        allocator al;
        string t(al);
        VX_CHECK(t.empty());
        VX_CHECK(valid(t));
        VX_CHECK(t.get_allocator() == al);
    }

    VX_SECTION("count + char")
    {
        string s(size_t(4), T('x'));
        EQ(s, "xxxx");
        VX_CHECK(s.capacity() == 4);

        string z(size_t(0), T('x'));
        VX_CHECK(z.empty());
        VX_CHECK(z.capacity() == 0);
        VX_CHECK(valid(z));

        string n(size_t(3), T());
        VX_CHECK(n.size() == 3);
        VX_CHECK(n[0] == T() && n[2] == T());
        VX_CHECK(valid(n));

        string a(size_t(2), T('y'), allocator());
        EQ(a, "yy");
    }

    VX_SECTION("pointer")
    {
        string s(LIT("hello"));
        EQ(s, "hello");
        VX_CHECK(s.capacity() == 5);

        string e(LIT(""));
        VX_CHECK(e.empty());
        VX_CHECK(valid(e));

        string p(LIT("hello"), 3);
        EQ(p, "hel");

        string p0(LIT("hello"), 0);
        VX_CHECK(p0.empty());
        VX_CHECK(valid(p0));
    }

    VX_SECTION("substring")
    {
        const string base(LIT("abcdef"));

        string a(base, size_t(2));
        EQ(a, "cdef");
        string b(base, size_t(2), size_t(3));
        EQ(b, "cde");
        string c(base, size_t(2), size_t(100));
        EQ(c, "cdef");
        string d(base, size_t(6)); // off == size
        VX_CHECK(d.empty());
        VX_CHECK(valid(d));
        string e(base, size_t(7)); // off > size
        VX_CHECK(e.empty());
        VX_CHECK(valid(e));
        string f(base, size_t(0), size_t(0));
        VX_CHECK(f.empty());
        VX_CHECK(valid(f));
    }

    VX_SECTION("iterator ranges")
    {
        T src[] = { T('a'), T('b'), T('c'), T() };

        string a(src, src + 3);
        EQ(a, "abc");

        string b(a.begin() + 1, a.end());
        EQ(b, "bc");

        string c(src, src);
        VX_CHECK(c.empty());
        VX_CHECK(valid(c));

        const string d(a.cbegin(), a.cend());
        EQ(d, "abc");
    }

    VX_SECTION("initializer_list")
    {
        string s{ T('a'), T('b'), T('c') };
        EQ(s, "abc");

        std::initializer_list<T> none{};
        string e(none);
        VX_CHECK(e.empty());
        VX_CHECK(valid(e));
    }

    VX_SECTION("copy")
    {
        string a(LIT("abc"));
        string b(a);
        EQ(b, "abc");
        EQ(a, "abc");
        VX_CHECK(a.data() != b.data());

        allocator al;
        string c(a, al);
        EQ(c, "abc");

        string e;
        string f(e);
        VX_CHECK(f.empty());
        VX_CHECK(valid(f));
    }

    VX_SECTION("move")
    {
        VX_DISABLE_USE_AFTER_MOVE_WARNING();

        string a(LIT("abcdefghijklmnopqrstuvwxyz"));
        const T* p = a.data();
        string b(std::move(a));
        EQ(b, "abcdefghijklmnopqrstuvwxyz");
        VX_CHECK(b.data() == p);
        VX_CHECK(a.empty());
        VX_CHECK(a.capacity() == 0);
        VX_CHECK(valid(a));

        allocator al;
        string c(std::move(b), al);
        EQ(c, "abcdefghijklmnopqrstuvwxyz");
        VX_CHECK(c.data() == p);
        VX_CHECK(b.empty());
        VX_CHECK(valid(b));

        string e;
        string f(std::move(e));
        VX_CHECK(f.empty());
        VX_CHECK(valid(f));

        VX_DISABLE_WARNING_POP();
    }
}
TEST_CHARS(ctor)

template <typename T>
static void test_factory()
{
    using string = vx::str::basic_string<T>;

    auto a = string::create();
    VX_CHECK(a);
    VX_CHECK(a.value().empty());
    VX_CHECK(valid(a.value()));

    auto b = string::create(size_t(3), T('x'));
    VX_CHECK(b);
    EQ(b.value(), "xxx");

    auto c = string::create(LIT("hello"));
    VX_CHECK(c);
    EQ(c.value(), "hello");

    auto c2 = string::create(LIT("hello"), size_t(3));
    VX_CHECK(c2);
    EQ(c2.value(), "hel");

    std::initializer_list<T> il{ T('a'), T('b') };
    auto d = string::create(il);
    VX_CHECK(d);
    EQ(d.value(), "ab");

    string src(LIT("abcdef"));

    auto e = string::create(src);
    VX_CHECK(e);
    EQ(e.value(), "abcdef");
    EQ(src, "abcdef");

    auto e1 = string::create(src, size_t(2));
    VX_CHECK(e1);
    EQ(e1.value(), "cdef");

    auto e2 = string::create(src, size_t(2), size_t(3));
    VX_CHECK(e2);
    EQ(e2.value(), "cde");

    auto e3 = string::create(src, size_t(9)); // out of range offset yields empty
    VX_CHECK(e3);
    VX_CHECK(e3.value().empty());

    auto v = string::create(src.view());
    VX_CHECK(v);
    EQ(v.value(), "abcdef");

    T arr[] = { T('x'), T('y'), T() };
    auto g = string::create(arr, arr + 2);
    VX_CHECK(g);
    EQ(g.value(), "xy");

    std::list<T> l{ T('p'), T('q'), T('r') };
    auto h = string::create(l.begin(), l.end());
    VX_CHECK(h);
    EQ(h.value(), "pqr");

    {
        VX_DISABLE_USE_AFTER_MOVE_WARNING();
        auto f = string::create(std::move(src));
        VX_CHECK(f);
        EQ(f.value(), "abcdef");
        VX_CHECK(src.empty());
        VX_DISABLE_WARNING_POP();
    }

    auto too_big = string::create(string().max_size() + 1, T('x'));
    VX_CHECK(!too_big);
    VX_CHECK_EXPECTED_ERROR(too_big, vx::err::size_error);
}
TEST_CHARS(factory)

//=============================================================================
// assignment
//=============================================================================

template <typename T>
static void test_assign_ops()
{
    using string = vx::str::basic_string<T>;
    const string a(LIT("hello"));

    VX_SECTION("copy assign: grow, shrink, equal")
    {
        string b;
        VX_CHECK(b.assign(a));
        EQ(b, "hello");

        string c(LIT("x"));
        c = a;
        EQ(c, "hello");

        string d(LIT("hello world, longer"));
        const size_t cap = d.capacity();
        d = a;
        EQ(d, "hello");
        VX_CHECK(d.capacity() == cap); // shrinking assign must not reallocate
        VX_CHECK(valid(d));

        string e = a;
        e = a;
        EQ(e, "hello");
    }

    VX_SECTION("self assign")
    {
        string e(LIT("abc"));
        string& er = e;
        e = er;
        EQ(e, "abc");
        VX_CHECK(e.assign(er));
        EQ(e, "abc");
    }

    VX_SECTION("char / pointer / count")
    {
        string s(LIT("hello"));
        s = T('x');
        EQ(s, "x");
        VX_CHECK(s.assign(T('y')));
        EQ(s, "y");

        string e;
        e = T('z');
        EQ(e, "z");

        s = LIT("hello world");
        EQ(s, "hello world");
        VX_CHECK(s.assign(LIT("ab")));
        EQ(s, "ab");
        VX_CHECK(s.assign(LIT("abcdef"), 3));
        EQ(s, "abc");
        VX_CHECK(s.assign(size_t(5), T('q')));
        EQ(s, "qqqqq");
        VX_CHECK(s.assign(size_t(0), T('q')));
        VX_CHECK(s.empty());
        VX_CHECK(valid(s));

        string z;
        VX_CHECK(z.assign(LIT("")));
        VX_CHECK(z.assign(size_t(0), T('x')));
        VX_CHECK(valid(z));
    }

    VX_SECTION("initializer_list")
    {
        std::initializer_list<T> il{ T('1'), T('2') };
        string s(LIT("hello"));
        s = il;
        EQ(s, "12");
        s.clear();
        VX_CHECK(s.assign(il));
        EQ(s, "12");
    }

    VX_SECTION("substring")
    {
        const string src(LIT("abcdef"));
        string s;
        VX_CHECK(s.assign(src, 2, 3));
        EQ(s, "cde");
        VX_CHECK(s.assign(src, size_t(2)));
        EQ(s, "cdef");
        auto bad = s.assign(src, size_t(7));
        VX_CHECK(!bad);
        EQ(s, "cdef"); // untouched
        VX_CHECK(s.assign(src, size_t(6)));
        VX_CHECK(s.empty());
    }

    VX_SECTION("iterator ranges")
    {
        T arr[] = { T('r'), T('s'), T('t'), T() };
        string s(LIT("zz"));
        VX_CHECK(s.assign(arr, arr + 3));
        EQ(s, "rst");
        VX_CHECK(s.assign(a.begin() + 1, a.begin() + 4));
        EQ(s, "ell");
        VX_CHECK(s.assign(arr, arr));
        VX_CHECK(s.empty());
    }

    VX_SECTION("reuses capacity")
    {
        string d(LIT("abcdefghij"));
        VX_CHECK(d.reserve(32));
        const T* p = d.data();
        d = LIT("xyz");
        EQ(d, "xyz");
        VX_CHECK(d.data() == p);
    }

    VX_SECTION("move assign")
    {
        VX_DISABLE_USE_AFTER_MOVE_WARNING();

        string x(LIT("abc"));
        const T* p = x.data();
        string y(LIT("a much longer string"));
        y = std::move(x);
        EQ(y, "abc");
        VX_CHECK(y.data() == p);
        VX_CHECK(x.empty());
        VX_CHECK(valid(x));

        string z;
        VX_CHECK(z.assign(std::move(y)));
        EQ(z, "abc");

        VX_DISABLE_WARNING_POP();
    }
}
TEST_CHARS(assign_ops)

//=============================================================================
// element access / iterators
//=============================================================================

template <typename T>
static void test_element_access()
{
    using string = vx::str::basic_string<T>;
    string s(LIT("abc"));
    const string& cs = s;

    VX_CHECK(s.front() == T('a'));
    VX_CHECK(s.back() == T('c'));
    VX_CHECK(cs.front() == T('a'));
    VX_CHECK(cs.back() == T('c'));
    VX_CHECK(s[1] == T('b'));
    VX_CHECK(cs[2] == T('c'));
    VX_CHECK(s.data() == &s[0]);
    VX_CHECK(cs.data() == s.c_str());

    VX_CHECK(s.at(0));
    VX_CHECK(s.at(2).value() == T('c'));
    VX_CHECK(cs.at(1).value() == T('b'));

    auto bad = s.at(3);
    VX_CHECK(!bad);
    VX_CHECK_EXPECTED_ERROR(bad, vx::err::out_of_range);
    auto cbad = cs.at(100);
    VX_CHECK(!cbad);
    VX_CHECK_EXPECTED_ERROR(cbad, vx::err::out_of_range);

    string e;
    VX_CHECK(!e.at(0));

    s[1] = T('X');
    EQ(s, "aXc");
    s.front() = T('A');
    s.back() = T('C');
    EQ(s, "AXC");
}
TEST_CHARS(element_access)

template <typename T>
static void test_iterator_ops()
{
    using string = vx::str::basic_string<T>;

    string s(LIT("abcd"));
    const string& cs = s;
    const T* expect = LIT("abcd");

    size_t i = 0;
    for (auto it = s.begin(); it != s.end(); ++it)
    {
        VX_CHECK(*it == expect[i++]);
    }
    VX_CHECK(i == 4);

    i = 0;
    for (auto it = cs.cbegin(); it != cs.cend(); ++it)
    {
        VX_CHECK(*it == expect[i++]);
    }

    i = 4;
    for (auto it = s.rbegin(); it != s.rend(); ++it)
    {
        VX_CHECK(*it == expect[--i]);
    }
    VX_CHECK(i == 0);

    i = 4;
    for (auto it = cs.crbegin(); it != cs.crend(); ++it)
    {
        VX_CHECK(*it == expect[--i]);
    }

    VX_CHECK(*s.begin() == T('a'));
    VX_CHECK(*(s.end() - 1) == T('d'));
    VX_CHECK(static_cast<size_t>(s.end() - s.begin()) == s.size());

    for (auto it = s.begin(); it != s.end(); ++it)
    {
        *it = static_cast<T>(*it - T('a') + T('A'));
    }
    EQ(s, "ABCD");

    string e;
    VX_CHECK(e.begin() == e.end());
    VX_CHECK(e.rbegin() == e.rend());
    VX_CHECK(e.cbegin() == e.cend());
}
TEST_CHARS(iterator_ops)

//=============================================================================
// capacity / size
//=============================================================================

template <typename T>
static void test_capacity_ops()
{
    using string = vx::str::basic_string<T>;

    VX_SECTION("reserve")
    {
        string s(LIT("abc"));
        VX_CHECK(s.reserve(50));
        VX_CHECK(s.capacity() >= 50);
        EQ(s, "abc");

        const T* p = s.data();
        const size_t cap = s.capacity();
        VX_CHECK(s.reserve(10)); // smaller: no-op
        VX_CHECK(s.capacity() == cap);
        VX_CHECK(s.data() == p);

        string e;
        VX_CHECK(e.reserve(0));
        VX_CHECK(e.capacity() == 0);
        VX_CHECK(valid(e));
        VX_CHECK(e.reserve(8));
        VX_CHECK(e.capacity() >= 8);
        VX_CHECK(e.empty());
        VX_CHECK(valid(e));

        VX_CHECK(!s.reserve(s.max_size() + 1));
        EQ(s, "abc");
    }

    VX_SECTION("size queries")
    {
        string s(LIT("abc"));
        VX_CHECK(!s.empty());
        VX_CHECK(s.size() == 3);
        VX_CHECK(s.length() == 3);
        VX_CHECK(s.size_bytes() == 3 * sizeof(T));
        VX_CHECK(!s.full());
        VX_CHECK(s.size() <= s.max_size());
        VX_CHECK(s.max_size() > 0);
    }

    VX_SECTION("clear keeps capacity, clear_and_deallocate frees")
    {
        string s(LIT("abcd"));
        const size_t cap = s.capacity();
        s.clear();
        VX_CHECK(s.empty());
        VX_CHECK(s.capacity() == cap);
        VX_CHECK(valid(s));
        s.clear(); // twice is fine
        VX_CHECK(valid(s));

        VX_CHECK(s.append(LIT("xy")));
        EQ(s, "xy");

        s.clear_and_deallocate();
        VX_CHECK(s.empty());
        VX_CHECK(s.capacity() == 0);
        VX_CHECK(valid(s));
        s.clear_and_deallocate(); // idempotent
        VX_CHECK(s.push_back(T('z')));
        EQ(s, "z");

        string e;
        e.clear();
        VX_CHECK(valid(e));
    }

    VX_SECTION("shrink_to_fit")
    {
        string s(LIT("abc"));
        VX_CHECK(s.reserve(64));
        VX_CHECK(s.shrink_to_fit());
        VX_CHECK(s.capacity() == s.size());
        EQ(s, "abc");

        VX_CHECK(s.shrink_to_fit()); // already tight
        VX_CHECK(s.capacity() == 3);

        string e;
        VX_CHECK(e.shrink_to_fit()); // never allocated
        VX_CHECK(e.capacity() == 0);
        VX_CHECK(valid(e));
    }

    VX_SECTION("shrink_to_fit on emptied string must stay valid")
    {
        string s(LIT("abcd"));
        s.clear();
        (void)s.shrink_to_fit();
        VX_CHECK(s.empty());
        VX_CHECK(valid(s));
        VX_CHECK(s.push_back(T('9')));
        EQ(s, "9");
    }
}
TEST_CHARS(capacity_ops)

template <typename T>
static void test_resize_ops()
{
    using string = vx::str::basic_string<T>;

    string s(LIT("abc"));

    VX_CHECK(s.resize(6, T('x')));
    EQ(s, "abcxxx");

    VX_CHECK(s.resize(2, T('q')));
    EQ(s, "ab");

    VX_CHECK(s.resize(2, T('q'))); // same size
    EQ(s, "ab");

    VX_CHECK(s.reserve(32));
    const T* p = s.data();
    VX_CHECK(s.resize(10, T('z'))); // within capacity: no realloc
    VX_CHECK(s.data() == p);
    EQ(s, "abzzzzzzzz");

    VX_CHECK(s.resize(0));
    VX_CHECK(s.empty());
    VX_CHECK(valid(s));

    VX_CHECK(s.resize(size_t(4))); // default fills with T()
    VX_CHECK(s.size() == 4);
    for (size_t i = 0; i < 4; ++i)
    {
        VX_CHECK(s[i] == T());
    }
    VX_CHECK(valid(s));

    string e;
    VX_CHECK(e.resize(0));
    VX_CHECK(e.empty());
    VX_CHECK(valid(e));
    VX_CHECK(e.resize(3, T('1')));
    EQ(e, "111");

    // growth beyond capacity keeps existing elements
    string g(LIT("abc"));
    VX_CHECK(g.resize(100, T('8')));
    VX_CHECK(g.size() == 100);
    VX_CHECK(g[0] == T('a') && g[2] == T('c') && g[3] == T('8') && g[99] == T('8'));
    VX_CHECK(valid(g));
}
TEST_CHARS(resize_ops)

//=============================================================================
// push_back / pop_back
//=============================================================================

template <typename T>
static void test_push_pop()
{
    using string = vx::str::basic_string<T>;

    VX_SECTION("push_back grows and keeps values")
    {
        string s;
        int reallocs = 0;
        const T* last = nullptr;
        for (int i = 0; i < 1000; ++i)
        {
            VX_CHECK(s.push_back(static_cast<T>('a' + i % 26)));
            if (s.data() != last)
            {
                ++reallocs;
                last = s.data();
            }
            VX_CHECK(s.capacity() >= s.size());
        }
        VX_CHECK(s.size() == 1000);
        VX_CHECK(valid(s));
        for (size_t i = 0; i < 1000; ++i)
        {
            VX_CHECK(s[i] == static_cast<T>('a' + static_cast<int>(i % 26)));
        }
        VX_CHECK(reallocs < 40); // amortized growth
    }

    VX_SECTION("pop_back")
    {
        string s(LIT("abc"));
        s.pop_back();
        EQ(s, "ab");
        s.pop_back();
        s.pop_back();
        VX_CHECK(s.empty());
        VX_CHECK(valid(s));
        s.pop_back(); // no-op on empty
        VX_CHECK(valid(s));

        string e;
        e.pop_back();
        VX_CHECK(valid(e));
    }

    VX_SECTION("pop_back then push_back reuses the buffer")
    {
        string s(LIT("abc"));
        const T* p = s.data();
        s.pop_back();
        VX_CHECK(s.push_back(T('z')));
        EQ(s, "abz");
        VX_CHECK(s.data() == p);
    }
}
TEST_CHARS(push_pop)

//=============================================================================
// append
//=============================================================================

template <typename T>
static void test_append_ops()
{
    using string = vx::str::basic_string<T>;
    const string o(LIT("12345"));

    VX_SECTION("literals / chars / counts")
    {
        string s(LIT("ab"));
        VX_CHECK(s.append(LIT("cd")));
        EQ(s, "abcd");
        VX_CHECK(s.append(LIT("efgh"), 2));
        EQ(s, "abcdef");
        VX_CHECK(s.append(size_t(2), T('x')));
        EQ(s, "abcdefxx");
        VX_CHECK(s.append(T('y')));
        EQ(s, "abcdefxxy");
        VX_CHECK(valid(s));
    }

    VX_SECTION("strings")
    {
        string s(LIT("ab"));
        VX_CHECK(s.append(o));
        EQ(s, "ab12345");
        VX_CHECK(s.append(o, 1, 2));
        EQ(s, "ab1234523");
        VX_CHECK(s.append(o, size_t(3)));
        EQ(s, "ab123452345");

        auto bad = s.append(o, size_t(6));
        VX_CHECK(!bad);
        VX_CHECK(s.append(o, size_t(5))); // off == size appends nothing
        EQ(s, "ab123452345");
    }

    VX_SECTION("initializer_list / ranges")
    {
        std::initializer_list<T> il{ T('1'), T('2') };
        string s(LIT("ab"));
        VX_CHECK(s.append(il));
        EQ(s, "ab12");

        T arr[] = { T('x'), T('y'), T('z'), T() };
        string t(LIT("ab"));
        VX_CHECK(t.append(arr, arr + 3));
        EQ(t, "abxyz");

        string u(LIT("ab"));
        VX_CHECK(u.append(o.begin() + 1, o.begin() + 4));
        EQ(u, "ab234");
    }

    VX_SECTION("operator+=")
    {
        std::initializer_list<T> il{ T('f') };
        string s;
        s += LIT("ab");
        s += T('c');
        s += string(LIT("de"));
        s += il;
        EQ(s, "abcdef");
    }

    VX_SECTION("capacity behaviour")
    {
        string s(LIT("ab"));
        VX_CHECK(s.reserve(32));
        const T* p = s.data();
        VX_CHECK(s.append(LIT("cd")));
        VX_CHECK(s.data() == p); // within capacity: no realloc

        VX_CHECK(s.append(size_t(40), T('x')));
        VX_CHECK(s.size() == 44);
        VX_CHECK(s[0] == T('a') && s[3] == T('d') && s[43] == T('x'));
        VX_CHECK(valid(s));
    }

    VX_SECTION("to an empty string")
    {
        string e;
        VX_CHECK(e.append(LIT("abc")));
        EQ(e, "abc");

        string e2;
        VX_CHECK(e2.append(LIT("")));
        VX_CHECK(e2.append(size_t(0), T('x')));
        VX_CHECK(e2.empty());
        VX_CHECK(valid(e2));
    }

    VX_SECTION("self append")
    {
        string s(LIT("abc"));
        VX_CHECK(s.append(s)); // reallocates
        EQ(s, "abcabc");
        VX_CHECK(s.reserve(64));
        VX_CHECK(s.append(s)); // stays in capacity
        EQ(s, "abcabcabcabc");
        VX_CHECK(s.append(s, 1, 3));
        EQ(s, "abcabcabcabcbca");
    }
}
TEST_CHARS(append_ops)

//=============================================================================
// insert (offset based, returns expected)
//=============================================================================

template <typename T>
static void test_insert_offset()
{
    using string = vx::str::basic_string<T>;
    const string o(LIT("12345"));

    VX_SECTION("single char")
    {
        string s(LIT("ac"));
        auto r = s.insert(size_t(1), T('b'));
        VX_CHECK(r);
        VX_CHECK(r.value() == s.begin() + 1);
        EQ(s, "abc");

        r = s.insert(size_t(0), T('<'));
        VX_CHECK(r.value() == s.begin());
        EQ(s, "<abc");
        
        r = s.insert(s.size(), T('>'));
        VX_CHECK(r.value() == s.end() - 1);
        EQ(s, "<abc>");
        
        string e;
        VX_CHECK(e.insert(size_t(0), T('x')));
        EQ(e, "x");
    }

    VX_SECTION("count + char: spill / no spill / realloc")
    {
        string a(LIT("ab"));
        VX_CHECK(a.reserve(20));
        auto r = a.insert(size_t(1), size_t(5), T('x')); // count > chars after pos
        VX_CHECK(r);
        EQ(a, "axxxxxb");
        VX_CHECK(r.value() == a.begin() + 1);

        string b(LIT("abcdef"));
        VX_CHECK(b.reserve(20));
        VX_CHECK(b.insert(size_t(2), size_t(2), T('x'))); // count <= chars after pos
        EQ(b, "abxxcdef");

        string c(LIT("abc"));
        VX_CHECK(c.reserve(20));
        VX_CHECK(c.insert(size_t(1), size_t(2), T('x'))); // exactly equal
        EQ(c, "axxbc");

        string d(LIT("abc"));
        VX_CHECK(d.insert(size_t(1), size_t(10), T('y'))); // reallocation
        VX_CHECK(d.size() == 13);
        VX_CHECK(d[0] == T('a') && d[1] == T('y') && d[10] == T('y') && d[11] == T('b') && d[12] == T('c'));
        VX_CHECK(valid(d));

        string e(LIT("ab"));
        VX_CHECK(e.insert(e.size(), size_t(3), T('z')));
        EQ(e, "abzzz");

        string f;
        VX_CHECK(f.insert(size_t(0), size_t(3), T('w')));
        EQ(f, "www");
    }

    VX_SECTION("pointers")
    {
        string a(LIT("ab"));
        VX_CHECK(a.insert(size_t(1), LIT("XY")));
        EQ(a, "aXYb");

        string b(LIT("ab"));
        VX_CHECK(b.insert(size_t(1), LIT("XYZ"), 2));
        EQ(b, "aXYb");
    }

    VX_SECTION("strings")
    {
        string a(LIT("ab"));
        VX_CHECK(a.insert(size_t(1), o));
        EQ(a, "a12345b");

        string b(LIT("ab"));
        VX_CHECK(b.insert(size_t(1), o, 1, 2));
        EQ(b, "a23b");

        string c(LIT("ab"));
        VX_CHECK(c.insert(size_t(2), o, size_t(3)));
        EQ(c, "ab45");
    }

    VX_SECTION("initializer_list / ranges")
    {
        std::initializer_list<T> il{ T('1'), T('2') };
        string a(LIT("ab"));
        VX_CHECK(a.insert(size_t(1), il));
        EQ(a, "a12b");

        T arr[] = { T('7'), T('8'), T('9'), T() };
        string b(LIT("ab"));
        VX_CHECK(b.insert(size_t(0), arr, arr + 2));
        EQ(b, "78ab");
        VX_CHECK(b.insert(b.size(), arr, arr + 3));
        EQ(b, "78ab789");

        string c(LIT("ab"));
        VX_CHECK(c.insert(size_t(1), o.begin() + 1, o.begin() + 3));
        EQ(c, "a23b");
    }

    VX_SECTION("zero-count inserts are no-ops")
    {
        string s(LIT("abc")); // capacity == size
        VX_CHECK(s.insert(size_t(1), size_t(0), T('x')));
        VX_CHECK(s.insert(size_t(1), LIT("")));
        VX_CHECK(s.insert(s.size(), size_t(0), T('x')));
        std::initializer_list<T> none{};
        VX_CHECK(s.insert(size_t(1), none));
        EQ(s, "abc");

        string e;
        VX_CHECK(e.insert(size_t(0), size_t(0), T('x')));
        VX_CHECK(e.insert(size_t(0), LIT("")));
        VX_CHECK(e.empty());
        VX_CHECK(valid(e));
    }

    VX_SECTION("out of range offsets")
    {
        string s(LIT("abc"));

        auto r1 = s.insert(size_t(4), T('x'));
        VX_CHECK(!r1);
        VX_CHECK_EXPECTED_ERROR(r1, vx::err::out_of_range);

        auto r2 = s.insert(size_t(4), size_t(0), T('x')); // still an error with count 0
        VX_CHECK(!r2);
        VX_CHECK_EXPECTED_ERROR(r2, vx::err::out_of_range);

        auto r3 = s.insert(size_t(99), LIT("x"));
        VX_CHECK(!r3);
        VX_CHECK_EXPECTED_ERROR(r3, vx::err::out_of_range);

        auto r4 = s.insert(size_t(0), o, size_t(6)); // bad offset into the other string
        VX_CHECK(!r4);
        VX_CHECK_EXPECTED_ERROR(r4, vx::err::out_of_range);

        EQ(s, "abc");
    }
}
TEST_CHARS(insert_offset)

//=============================================================================
// insert (iterator based)
//=============================================================================

template <typename T>
static void test_insert_iterator()
{
    using string = vx::str::basic_string<T>;
    const string o(LIT("12345"));

    VX_SECTION("single")
    {
        string s(LIT("ac"));
        auto it = s.insert(s.begin() + 1, T('b'));
        VX_CHECK(*it == T('b'));
        it = s.insert(s.begin(), T('<'));
        VX_CHECK(it == s.begin());
        it = s.insert(s.end(), T('>'));
        VX_CHECK(*it == T('>'));
        EQ(s, "<abc>");
    }

    VX_SECTION("count")
    {
        string v(LIT("abc"));
        VX_CHECK(v.reserve(30));
        auto it = v.insert(v.begin() + 1, size_t(6), T('x'));
        VX_CHECK(it == v.begin() + 1);
        EQ(v, "axxxxxxbc");

        string w(LIT("ab")); // realloc path
        it = w.insert(w.begin() + 1, size_t(4), T('y'));
        VX_CHECK(*it == T('y'));
        EQ(w, "ayyyyb");

        string z;
        it = z.insert(z.begin(), size_t(0), T('x'));
        VX_UNUSED(it);
        VX_CHECK(z.empty());
        VX_CHECK(valid(z));
    }

    VX_SECTION("pointers and strings")
    {
        string s(LIT("ab"));
        auto it = s.insert(s.begin() + 1, o);
        VX_CHECK(*it == T('1'));
        EQ(s, "a12345b");

        it = s.insert(s.begin(), LIT("XY"));
        VX_CHECK(*it == T('X'));
        EQ(s, "XYa12345b");

        it = s.insert(s.end(), LIT("QRS"), 2);
        VX_CHECK(*it == T('Q'));
        EQ(s, "XYa12345bQR");

        string t(LIT("ab"));
        it = t.insert(t.begin() + 1, o, size_t(3));
        VX_CHECK(*it == T('4'));
        EQ(t, "a45b");

        string u(LIT("ab"));
        it = u.insert(u.begin() + 1, o, 1, 2);
        VX_CHECK(*it == T('2'));
        EQ(u, "a23b");
    }

    VX_SECTION("bad offset into the other string leaves the string alone")
    {
        string t(LIT("ab"));
        auto it = t.insert(t.begin() + 1, o, size_t(99));
        VX_CHECK(it == t.begin() + 1);
        EQ(t, "ab");
    }

    VX_SECTION("initializer_list and ranges")
    {
        std::initializer_list<T> il{ T('2'), T('3'), T('4') };
        string v(LIT("15"));
        v.insert(v.begin() + 1, il);
        EQ(v, "12345");

        T arr[] = { T('8'), T('9'), T() };
        v.insert(v.end(), arr, arr + 2);
        EQ(v, "1234589");

        v.insert(v.begin() + 5, o.begin() + 2, o.begin() + 4);
        EQ(v, "123453489");

        std::initializer_list<T> none{};
        v.insert(v.begin() + 1, none);
        VX_CHECK(v.size() == 9);
    }
}
TEST_CHARS(insert_iterator)

//=============================================================================
// replace (offset based)
//=============================================================================

template <typename T>
static void test_replace_offset()
{
    using string = vx::str::basic_string<T>;
    const string o(LIT("12345"));

    VX_SECTION("pointer: same size / shrink / grow")
    {
        string a(LIT("hello world"));
        VX_CHECK(a.replace(size_t(0), size_t(5), LIT("HELLO")));
        EQ(a, "HELLO world");

        string b(LIT("hello world"));
        VX_CHECK(b.replace(size_t(5), size_t(6), LIT("!")));
        EQ(b, "hello!");

        string c(LIT("ab")); // realloc
        VX_CHECK(c.replace(size_t(1), size_t(1), LIT("XYZ")));
        EQ(c, "aXYZ");

        string d(LIT("ab")); // in capacity
        VX_CHECK(d.reserve(32));
        const T* p = d.data();
        VX_CHECK(d.replace(size_t(1), size_t(1), LIT("XYZ")));
        EQ(d, "aXYZ");
        VX_CHECK(d.data() == p);
    }

    VX_SECTION("insert / erase / clamp via replace")
    {
        string s(LIT("abc"));
        VX_CHECK(s.replace(size_t(1), size_t(0), LIT("XY")));
        EQ(s, "aXYbc");
        VX_CHECK(s.replace(size_t(1), size_t(2), LIT("")));
        EQ(s, "abc");

        string t(LIT("abcdef"));
        VX_CHECK(t.replace(size_t(4), size_t(100), LIT("X")));
        EQ(t, "abcdX");

        VX_CHECK(t.replace(size_t(0), t.size(), LIT("z")));
        EQ(t, "z");
        VX_CHECK(t.replace(size_t(0), t.size(), LIT("")));
        VX_CHECK(t.empty());
        VX_CHECK(valid(t));
    }

    VX_SECTION("strings / counts / chars")
    {
        string a(LIT("abcdef"));
        VX_CHECK(a.replace(size_t(0), size_t(1), o));
        EQ(a, "12345bcdef");

        string b(LIT("abcdef"));
        VX_CHECK(b.replace(size_t(0), size_t(1), o, size_t(1), size_t(2)));
        EQ(b, "23bcdef");

        string c(LIT("abcdef"));
        VX_CHECK(c.replace(size_t(1), size_t(2), LIT("12345"), 3));
        EQ(c, "a123def");

        string d(LIT("abcdef"));
        VX_CHECK(d.replace(size_t(1), size_t(2), size_t(3), T('x')));
        EQ(d, "axxxdef");
    }

    VX_SECTION("initializer_list / ranges")
    {
        std::initializer_list<T> il{ T('1'), T('2') };
        string a(LIT("abcdef"));
        VX_CHECK(a.replace(size_t(1), size_t(2), il));
        EQ(a, "a12def");

        T arr[] = { T('7'), T('8'), T('9'), T() };
        string b(LIT("abcdef"));
        VX_CHECK(b.replace(size_t(1), size_t(2), arr, arr + 3));
        EQ(b, "a789def");

        string c(LIT("abcdef"));
        VX_CHECK(c.replace(size_t(1), size_t(2), o.begin() + 1, o.begin() + 3));
        EQ(c, "a23def");
    }

    VX_SECTION("empty string")
    {
        string e;
        VX_CHECK(e.replace(size_t(0), size_t(0), LIT("abc")));
        EQ(e, "abc");

        string e2;
        VX_CHECK(e2.replace(size_t(0), size_t(0), LIT("")));
        VX_CHECK(e2.empty());
        VX_CHECK(valid(e2));
    }

    VX_SECTION("out of range")
    {
        string s(LIT("abc"));
        VX_CHECK(!s.replace(size_t(4), size_t(0), LIT("x")));
        VX_CHECK(!s.replace(size_t(0), size_t(1), o, size_t(6)));
        EQ(s, "abc");
    }
}
TEST_CHARS(replace_offset)

//=============================================================================
// replace (iterator based)
//=============================================================================

template <typename T>
static void test_replace_iterator()
{
    using string = vx::str::basic_string<T>;
    const string o(LIT("12345"));

    VX_SECTION("pointer: same size / shrink / grow")
    {
        string a(LIT("hello world"));
        VX_CHECK(a.replace(a.begin(), a.begin() + 5, LIT("HELLO")));
        EQ(a, "HELLO world");

        string b(LIT("hello world"));
        VX_CHECK(b.replace(b.begin() + 5, b.end(), LIT("!")));
        EQ(b, "hello!");

        string c(LIT("ab"));
        VX_CHECK(c.replace(c.begin() + 1, c.begin() + 2, LIT("XYZ")));
        EQ(c, "aXYZ");

        string d(LIT("abc"));
        VX_CHECK(d.replace(d.begin() + 1, d.begin() + 1, LIT("XY")));
        EQ(d, "aXYbc");
        VX_CHECK(d.replace(d.begin() + 1, d.begin() + 3, LIT("")));
        EQ(d, "abc");
    }

    VX_SECTION("strings / counts / chars")
    {
        string a(LIT("abcdef"));
        VX_CHECK(a.replace(a.begin(), a.begin() + 1, o));
        EQ(a, "12345bcdef");

        string b(LIT("abcdef"));
        VX_CHECK(b.replace(b.begin(), b.begin() + 1, o, 1, 2));
        EQ(b, "23bcdef");

        string c(LIT("abcdef"));
        VX_CHECK(c.replace(c.begin() + 1, c.begin() + 3, LIT("12345"), 3));
        EQ(c, "a123def");

        string d(LIT("abcdef"));
        VX_CHECK(d.replace(d.begin() + 1, d.begin() + 3, size_t(3), T('x')));
        EQ(d, "axxxdef");
    }

    VX_SECTION("initializer_list / ranges")
    {
        std::initializer_list<T> il{ T('1'), T('2') };
        string a(LIT("abcdef"));
        VX_CHECK(a.replace(a.begin() + 1, a.begin() + 3, il));
        EQ(a, "a12def");

        T arr[] = { T('7'), T('8'), T('9'), T() };
        string b(LIT("abcdef"));
        VX_CHECK(b.replace(b.begin() + 1, b.begin() + 3, arr, arr + 3));
        EQ(b, "a789def");

        string c(LIT("abcdef"));
        VX_CHECK(c.replace(c.begin() + 1, c.begin() + 3, o.begin() + 1, o.begin() + 3));
        EQ(c, "a23def");
    }

    VX_SECTION("bad offset into the other string")
    {
        string s(LIT("abc"));
        VX_CHECK(!s.replace(s.begin(), s.begin() + 1, o, size_t(6)));
        EQ(s, "abc");
    }
}
TEST_CHARS(replace_iterator)

//=============================================================================
// erase
//=============================================================================

template <typename T>
static void test_erase_ops()
{
    using string = vx::str::basic_string<T>;

    VX_SECTION("offset + count")
    {
        string s(LIT("abcdef"));
        auto r = s.erase(size_t(1), size_t(2));
        VX_CHECK(r);
        VX_CHECK(*r.value() == T('d'));
        EQ(s, "adef");

        VX_CHECK(s.erase(size_t(0), size_t(0)));
        VX_CHECK(s.erase(s.size(), size_t(0)));
        EQ(s, "adef");

        VX_CHECK(s.erase(size_t(2), size_t(100))); // count is clamped
        EQ(s, "ad");

        VX_CHECK(s.erase(size_t(2))); // off == size
        EQ(s, "ad");

        auto bad = s.erase(size_t(3));
        VX_CHECK(!bad);
        VX_CHECK_EXPECTED_ERROR(bad, vx::err::out_of_range);
        EQ(s, "ad");

        VX_CHECK(s.erase()); // erase everything
        VX_CHECK(s.empty());
        VX_CHECK(valid(s));

        string e;
        VX_CHECK(e.erase());
        VX_CHECK(e.erase(size_t(0), size_t(0)));
        VX_CHECK(valid(e));
    }

    VX_SECTION("erase(off) removes the tail")
    {
        string s(LIT("abcdef"));
        VX_CHECK(s.erase(size_t(2)));
        EQ(s, "ab");
    }

    VX_SECTION("iterator based")
    {
        string s(LIT("abcdef"));
        auto it = s.erase(s.begin());
        VX_CHECK(*it == T('b'));
        EQ(s, "bcdef");

        it = s.erase(s.begin() + 2);
        VX_CHECK(*it == T('e'));
        EQ(s, "bcef");

        it = s.erase(s.end() - 1);
        VX_CHECK(it == s.end());
        EQ(s, "bce");

        it = s.erase(s.begin() + 1, s.end());
        VX_CHECK(it == s.end());
        EQ(s, "b");

        it = s.erase(s.begin(), s.begin()); // empty range
        EQ(s, "b");
        it = s.erase(s.end(), s.end()); // empty range at end
        EQ(s, "b");

        it = s.erase(s.begin(), s.end());
        VX_CHECK(s.empty());
        VX_CHECK(valid(s));
    }
}
TEST_CHARS(erase_ops)

//=============================================================================
// substr / copy / view
//=============================================================================

template <typename T>
static void test_substr_copy_view()
{
    using string = vx::str::basic_string<T>;
    const string s(LIT("abcdef"));

    VX_SECTION("substr")
    {
        EQ(s.substr(), "abcdef");
        EQ(s.substr(2), "cdef");
        EQ(s.substr(2, 3), "cde");
        EQ(s.substr(2, 100), "cdef");
        EQ(s.substr(6), "");
        EQ(s.substr(7), "");
        EQ(s.substr(0, 0), "");
        VX_CHECK(s.substr().data() != s.data());
        VX_CHECK(valid(s.substr(3)));
    }

    VX_SECTION("copy")
    {
        T buf[10]{};
        VX_CHECK(s.copy(buf, 10) == 6);
        VX_CHECK(buf_eq(buf, LIT("abcdef"), 6));
        VX_CHECK(s.copy(buf, 3, 1) == 3);
        VX_CHECK(buf_eq(buf, LIT("bcd"), 3));
        VX_CHECK(s.copy(buf, 100, 4) == 2);
        VX_CHECK(buf_eq(buf, LIT("ef"), 2));
        VX_CHECK(s.copy(buf, 5, 6) == 0); // off == size
        VX_CHECK(s.copy(buf, 5, 7) == 0); // off > size
    }

    VX_SECTION("view")
    {
        auto whole = s.view();
        VX_CHECK(whole.size() == 6);
        VX_CHECK(whole.data() == s.data());

        auto mid = s.view(1, 3);
        VX_CHECK(mid.size() == 3);
        VX_CHECK(mid.data() == s.data() + 1);

        VX_CHECK(s.view(2, 100).size() == 4);
        VX_CHECK(s.view(6).size() == 0);
        VX_CHECK(s.view(7).size() == 0);

        decltype(s.view()) conv = s; // implicit conversion
        VX_CHECK(conv.size() == 6);
        VX_CHECK(conv.data() == s.data());
    }
}
TEST_CHARS(substr_copy_view)

//=============================================================================
// compatible string types (string views)
//=============================================================================

template <typename T>
static void test_compatible_strings()
{
    using string = vx::str::basic_string<T>;
    const string base(LIT("hello"));
    auto sv = base.view();

    VX_SECTION("construct / assign")
    {
        string a(sv);
        EQ(a, "hello");
        VX_CHECK(a.data() != base.data());

        string b(sv, size_t(1), size_t(3));
        EQ(b, "ell");

        string c;
        c = sv;
        EQ(c, "hello");
        VX_CHECK(c.assign(sv, size_t(1), size_t(2)));
        EQ(c, "el");
        VX_CHECK(!c.assign(sv, size_t(6)));
        EQ(c, "el");
        VX_CHECK(c.assign(sv));
        EQ(c, "hello");
    }

    VX_SECTION("insert / append / replace")
    {
        string d(LIT("<>"));
        VX_CHECK(d.insert(size_t(1), sv));
        EQ(d, "<hello>");
        VX_CHECK(d.insert(size_t(0), sv, size_t(4)));
        EQ(d, "o<hello>");
        VX_CHECK(d.insert(d.size(), sv, size_t(1), size_t(2)));
        EQ(d, "o<hello>el");

        string e(LIT("ab"));
        VX_CHECK(e.append(sv));
        EQ(e, "abhello");
        e += sv;
        EQ(e, "abhellohello");
        VX_CHECK(e.append(sv, size_t(3)));
        EQ(e, "abhellohellolo");

        string r(LIT("xyz"));
        VX_CHECK(r.replace(size_t(1), size_t(1), sv));
        EQ(r, "xhelloz");
        VX_CHECK(r.replace(size_t(0), size_t(1), sv, size_t(3)));
        EQ(r, "lohelloz");
    }

    VX_SECTION("search / compare")
    {
        const string hay(LIT("say hello there"));
        VX_CHECK(hay.find(sv) == 4);
        VX_CHECK(hay.rfind(sv) == 4);
        VX_CHECK(hay.contains(sv));
        VX_CHECK(hay.find_first_of(sv) == 4);
        VX_CHECK(hay.find_last_of(sv) == 14);
        VX_CHECK(hay.find_first_not_of(sv) == 0);
        VX_CHECK(hay.find_last_not_of(sv) == 13);

        VX_CHECK(base.compare(sv) == 0);
        VX_CHECK(hay.compare(sv) != 0);
    }
}
TEST_CHARS(compatible_strings)

//=============================================================================
// searching
//=============================================================================

template <typename T>
static void test_search_ops()
{
    using string = vx::str::basic_string<T>;
    const string s(LIT("hello world")); // h0 e1 l2 l3 o4 _5 w6 o7 r8 l9 d10

    VX_SECTION("find")
    {
        VX_CHECK(s.find(STR("o")) == 4);
        VX_CHECK(s.find(STR("o"), 5) == 7);
        VX_CHECK(s.find(LIT("world")) == 6);
        VX_CHECK(s.find(LIT("o"), 5) == 7);
        VX_CHECK(s.find(LIT("worldxx"), 0, 5) == 6);
        VX_CHECK(s.find(LIT("xyz")) == string::npos);
        VX_CHECK(s.find(T('o')) == 4);
        VX_CHECK(s.find(T('o'), 5) == 7);
        VX_CHECK(s.find(T('o'), 8) == string::npos);
        VX_CHECK(s.find(LIT("hello world!")) == string::npos); // needle longer than haystack
        VX_CHECK(s.find(LIT("d"), 10) == 10);
        VX_CHECK(s.find(LIT("d"), 11) == string::npos);
        VX_CHECK(s.find(LIT("")) == 0);
        VX_CHECK(s.find(LIT(""), 11) == 11);
        VX_CHECK(s.find(LIT(""), 12) == string::npos);

        string e;
        VX_CHECK(e.find(T('a')) == string::npos);
        VX_CHECK(e.find(LIT("a")) == string::npos);
        VX_CHECK(e.find(LIT("")) == 0);

        const string a(LIT("aaaa"));
        VX_CHECK(a.find(LIT("aa"), 1) == 1);
        VX_CHECK(a.rfind(LIT("aa")) == 2);
    }

    VX_SECTION("rfind")
    {
        VX_CHECK(s.rfind(STR("o")) == 7);
        VX_CHECK(s.rfind(STR("o"), 6) == 4);
        VX_CHECK(s.rfind(LIT("l")) == 9);
        VX_CHECK(s.rfind(LIT("hello")) == 0);
        VX_CHECK(s.rfind(LIT("hello"), 0) == 0);
        VX_CHECK(s.rfind(LIT("world"), 5) == string::npos);
        VX_CHECK(s.rfind(LIT("worldxx"), string::npos, 5) == 6);
        VX_CHECK(s.rfind(T('o')) == 7);
        VX_CHECK(s.rfind(T('o'), 6) == 4);
        VX_CHECK(s.rfind(T('h'), 0) == 0);
        VX_CHECK(s.rfind(T('x')) == string::npos);
        VX_CHECK(s.rfind(LIT("")) == 11);
    }

    VX_SECTION("find_first_of")
    {
        VX_CHECK(s.find_first_of(STR("lo")) == 2);
        VX_CHECK(s.find_first_of(STR("lo"), 4) == 4);
        VX_CHECK(s.find_first_of(LIT("lo")) == 2);
        VX_CHECK(s.find_first_of(LIT("ld"), 8) == 9);
        VX_CHECK(s.find_first_of(LIT("xlo"), 0, 1) == string::npos);
        VX_CHECK(s.find_first_of(LIT("loxyz"), 0, 2) == 2);
        VX_CHECK(s.find_first_of(LIT("xyz")) == string::npos);
        VX_CHECK(s.find_first_of(LIT("lo"), 11) == string::npos);
        VX_CHECK(s.find_first_of(T('o')) == 4);
        VX_CHECK(s.find_first_of(T('o'), 5) == 7);
        VX_CHECK(s.find_first_of(T('x')) == string::npos);
    }

    VX_SECTION("find_last_of")
    {
        VX_CHECK(s.find_last_of(STR("lo")) == 9);
        VX_CHECK(s.find_last_of(STR("lo"), 8) == 7);
        VX_CHECK(s.find_last_of(LIT("h")) == 0);
        VX_CHECK(s.find_last_of(LIT("lo"), 3) == 3);
        VX_CHECK(s.find_last_of(LIT("lo"), 0) == string::npos);
        VX_CHECK(s.find_last_of(T('o')) == 7);
        VX_CHECK(s.find_last_of(T('o'), 6) == 4);
        VX_CHECK(s.find_last_of(T('x')) == string::npos);
    }

    VX_SECTION("find_first_not_of")
    {
        VX_CHECK(s.find_first_not_of(STR("hel")) == 4);
        VX_CHECK(s.find_first_not_of(LIT("helo ")) == 6);
        VX_CHECK(s.find_first_not_of(LIT("helo wrd")) == string::npos);
        VX_CHECK(s.find_first_not_of(LIT("hel"), 5) == 5);
        VX_CHECK(s.find_first_not_of(LIT("hlxx"), 0, 2) == 1);
        VX_CHECK(s.find_first_not_of(T('h')) == 1);
    }

    VX_SECTION("find_last_not_of")
    {
        VX_CHECK(s.find_last_not_of(STR("dl")) == 8);
        VX_CHECK(s.find_last_not_of(LIT("dlr")) == 7);
        VX_CHECK(s.find_last_not_of(LIT("helo wrd")) == string::npos);
        VX_CHECK(s.find_last_not_of(LIT("dlr"), 6) == 6);
        VX_CHECK(s.find_last_not_of(LIT("dlxx"), string::npos, 2) == 8);
        VX_CHECK(s.find_last_not_of(T('d')) == 9);
    }

    VX_SECTION("contains")
    {
        VX_CHECK(s.contains(STR("lo w")));
        VX_CHECK(s.contains(LIT("world")));
        VX_CHECK(!s.contains(LIT("xyz")));
        VX_CHECK(s.contains(T('w')));
        VX_CHECK(!s.contains(T('x')));
    }
}
TEST_CHARS(search_ops)

//=============================================================================
// comparison
//=============================================================================

template <typename T>
static void test_compare_ops()
{
    using string = vx::str::basic_string<T>;
    const string s(LIT("abcdef"));

    VX_SECTION("compare")
    {
        VX_CHECK(s.compare(s) == 0);
        VX_CHECK(s.compare(STR("abcdef")) == 0);
        VX_CHECK(s.compare(STR("abcdeg")) < 0);
        VX_CHECK(s.compare(STR("abcdee")) > 0);
        VX_CHECK(s.compare(STR("abcdefg")) < 0); // prefix sorts first
        VX_CHECK(s.compare(STR("abcde")) > 0);
        VX_CHECK(s.compare(STR("")) > 0);
        VX_CHECK(string().compare(STR("")) == 0);
        VX_CHECK(string().compare(s) < 0);

        VX_CHECK(s.compare(LIT("abcdef")) == 0);
        VX_CHECK(s.compare(LIT("abcdeg")) < 0);
        VX_CHECK(s.compare(LIT("abcdee")) > 0);
    }

    VX_SECTION("compare with offsets")
    {
        VX_CHECK(s.compare(1, 3, STR("bcd")) == 0);
        VX_CHECK(s.compare(1, 3, STR("bce")) < 0);
        VX_CHECK(s.compare(1, 100, STR("bcdef")) == 0); // count is clamped
        VX_CHECK(s.compare(1, 3, LIT("bcd")) == 0);
        VX_CHECK(s.compare(1, 3, LIT("bcdxxx"), 3) == 0);
        VX_CHECK(s.compare(1, 3, LIT("bcdxxx"), 4) < 0);
        VX_CHECK(s.compare(1, 3, STR("xbcdx"), 1, 3) == 0);
        VX_CHECK(s.compare(1, 3, STR("xbcdx"), 1, 2) > 0);
    }

    VX_SECTION("operators")
    {
        const string a(LIT("abc"));
        const string b(LIT("abd"));
        const string c(LIT("ab"));
        string a2(LIT("abc"));
        VX_CHECK(a2.reserve(100));

        VX_CHECK(a == a);
        VX_CHECK(a == a2); // capacity doesn't matter
        VX_CHECK(!(a == b));
        VX_CHECK(a != b);
        VX_CHECK(a < b);
        VX_CHECK(b > a);
        VX_CHECK(a <= b);
        VX_CHECK(a <= a);
        VX_CHECK(b >= a);
        VX_CHECK(a >= a);
        VX_CHECK(c < a);
        VX_CHECK(a > c);

        VX_CHECK(a == LIT("abc"));
        VX_CHECK(LIT("abc") == a);
        VX_CHECK(a != LIT("abd"));
        VX_CHECK(LIT("abd") != a);
        VX_CHECK(a < LIT("abd"));
        VX_CHECK(LIT("abc") < b);
        VX_CHECK(b > LIT("abc"));
        VX_CHECK(LIT("abd") > a);
        VX_CHECK(a <= LIT("abc"));
        VX_CHECK(LIT("abc") <= a);
        VX_CHECK(a >= LIT("abc"));
        VX_CHECK(LIT("abc") >= a);
    }
}
TEST_CHARS(compare_ops)

//=============================================================================
// operator+
//=============================================================================

template <typename T>
static void test_concat_ops()
{
    using string = vx::str::basic_string<T>;
    const string a(LIT("abc"));
    const string b(LIT("def"));
    string e;

    VX_SECTION("lvalues")
    {
        EQ(a + b, "abcdef");
        EQ(a + LIT("xyz"), "abcxyz");
        EQ(LIT("xyz") + a, "xyzabc");
        EQ(a + T('!'), "abc!");
        EQ(T('!') + a, "!abc");
        EQ(a + b + a, "abcdefabc");
        EQ(a, "abc"); // inputs untouched
        EQ(b, "def");
    }

    VX_SECTION("rvalues")
    {
        EQ(STR("abc") + STR("def"), "abcdef");
        EQ(STR("abc") + b, "abcdef");
        EQ(a + STR("def"), "abcdef");
        EQ(STR("abc") + LIT("def"), "abcdef");
        EQ(STR("abc") + T('d'), "abcd");
        EQ(LIT("abc") + STR("def"), "abcdef");
        EQ(T('a') + STR("def"), "adef");
    }

    VX_SECTION("empty operands")
    {
        EQ(e + e, "");
        EQ(e + a, "abc");
        EQ(a + e, "abc");
        EQ(e + LIT(""), "");
        EQ(LIT("") + e, "");
        EQ(e + T('x'), "x");
        EQ(T('x') + e, "x");
        EQ(STR("abc") + e, "abc");
        EQ(e + STR("abc"), "abc");
    }

    VX_SECTION("repeated concatenation")
    {
        string acc;
        for (int i = 0; i < 50; ++i)
        {
            acc = acc + T('x');
        }
        VX_CHECK(acc.size() == 50);
        VX_CHECK(valid(acc));
    }
}
TEST_CHARS(concat_ops)

//=============================================================================
// swap / move
//=============================================================================

template <typename T>
static void test_swap_move()
{
    using string = vx::str::basic_string<T>;

    VX_SECTION("swap")
    {
        string a(LIT("abc"));
        string b(LIT("longer text"));
        const T* pa = a.data();
        const T* pb = b.data();

        a.swap(b);
        EQ(a, "longer text");
        EQ(b, "abc");
        VX_CHECK(a.data() == pb && b.data() == pa);

        vx::swap(a, b);
        EQ(a, "abc");

        string e;
        a.swap(e);
        VX_CHECK(a.empty());
        VX_CHECK(valid(a));
        EQ(e, "abc");

        string x, y;
        x.swap(y);
        VX_CHECK(valid(x) && valid(y));
    }

    VX_SECTION("moved-from objects stay usable")
    {
        VX_DISABLE_USE_AFTER_MOVE_WARNING();

        string a(LIT("abc"));
        string b(std::move(a));
        VX_CHECK(a.empty());
        VX_CHECK(a.capacity() == 0);
        VX_CHECK(valid(a));

        VX_CHECK(a.push_back(T('x'))); // must not write into shared static storage
        EQ(a, "x");
        EQ(b, "abc");

        string d;
        d = std::move(b);
        EQ(d, "abc");
        VX_CHECK(valid(b));
        b += LIT("z");
        EQ(b, "z");

        a = LIT("again");
        EQ(a, "again");

        string fresh;
        VX_CHECK(valid(fresh));
        EQ(fresh, "");

        VX_DISABLE_WARNING_POP();
    }
}
TEST_CHARS(swap_move)

//=============================================================================
// non-pointer (generic) iterators
//=============================================================================

template <typename T>
static void test_generic_iterators()
{
    using string = vx::str::basic_string<T>;
    const std::list<T> l{ T('x'), T('y'), T('z') };
    const std::list<T> none;

    VX_SECTION("construct")
    {
        string s(l.begin(), l.end());
        EQ(s, "xyz");
        string e(none.begin(), none.end());
        VX_CHECK(e.empty());
        VX_CHECK(valid(e));
    }

    VX_SECTION("assign / append")
    {
        string s(LIT("12345"));
        VX_CHECK(s.assign(l.begin(), l.end()));
        EQ(s, "xyz");
        VX_CHECK(s.assign(none.begin(), none.end()));
        VX_CHECK(s.empty());

        string t(LIT("ab"));
        VX_CHECK(t.append(l.begin(), l.end()));
        EQ(t, "abxyz");
        VX_CHECK(t.append(none.begin(), none.end()));
        EQ(t, "abxyz");
    }

    VX_SECTION("insert by offset")
    {
        string s(LIT("ab"));
        auto r = s.insert(size_t(1), l.begin(), l.end());
        VX_CHECK(r);
        VX_CHECK(r.value() == s.begin() + 1);
        EQ(s, "axyzb");
        VX_CHECK(s.insert(s.size(), l.begin(), l.end()));
        EQ(s, "axyzbxyz");
        VX_CHECK(s.insert(size_t(0), l.begin(), l.end()));
        EQ(s, "xyzaxyzbxyz");
        VX_CHECK(s.insert(size_t(2), none.begin(), none.end()));
        VX_CHECK(s.size() == 11);

        auto bad = s.insert(size_t(99), l.begin(), l.end());
        VX_CHECK(!bad);
        VX_CHECK_EXPECTED_ERROR(bad, vx::err::out_of_range);
        VX_CHECK(s.size() == 11);
    }

    VX_SECTION("insert by iterator")
    {
        string s(LIT("ab"));
        auto it = s.insert(s.begin() + 1, l.begin(), l.end());
        VX_CHECK(*it == T('x'));
        EQ(s, "axyzb");
    }

    VX_SECTION("replace")
    {
        string a(LIT("abcdef"));
        VX_CHECK(a.replace(size_t(1), size_t(2), l.begin(), l.end()));
        EQ(a, "axyzdef");

        string b(LIT("abcdef"));
        VX_CHECK(b.replace(b.begin() + 1, b.begin() + 3, l.begin(), l.end()));
        EQ(b, "axyzdef");
    }
}
TEST_CHARS(generic_iterators)

//=============================================================================
// aliasing: the source points into the string's own buffer
//=============================================================================

// mode 0: capacity == size, so every growing operation reallocates
// mode 1: plenty of capacity, so every operation takes the in-place path
template <typename T>
static void prepare(vx::str::basic_string<T>& s, int mode)
{
    if (mode == 0)
    {
        (void)s.shrink_to_fit();
    }
    else
    {
        (void)s.reserve(64);
    }
}

template <typename T>
static void test_aliasing()
{
    using string = vx::str::basic_string<T>;
    using model = std::basic_string<T>;

    const model base = ascii<T>("abcdefgh");
    const size_t n = base.size();

    VX_SECTION("insert from own buffer")
    {
        for (int mode = 0; mode < 2; ++mode)
        {
            for (size_t pos = 0; pos <= n; ++pos)
            {
                // whole string
                {
                    string s(base.data(), base.size());
                    prepare(s, mode);
                    model m = base;
                    const model copy = m;
                    VX_CHECK(s.insert(pos, s));
                    m.insert(pos, copy);
                    VX_CHECK(eq_model(s, m));
                }

                for (size_t a = 0; a <= n; ++a)
                {
                    for (size_t len = 0; a + len <= n; ++len)
                    {
                        const model src = base.substr(a, len);

                        string s1(base.data(), base.size());
                        prepare(s1, mode);
                        model m1 = base;
                        VX_CHECK(s1.insert(pos, s1.data() + a, len));
                        m1.insert(pos, src);
                        VX_CHECK(eq_model(s1, m1));

                        string s2(base.data(), base.size());
                        prepare(s2, mode);
                        VX_CHECK(s2.insert(s2.begin() + static_cast<std::ptrdiff_t>(pos), s2.data() + a, len));
                        VX_CHECK(eq_model(s2, m1));

                        string s3(base.data(), base.size());
                        prepare(s3, mode);
                        VX_CHECK(s3.insert(pos, s3, a, len));
                        VX_CHECK(eq_model(s3, m1));
                    }
                }
            }
        }
    }

    VX_SECTION("replace from own buffer")
    {
        for (int mode = 0; mode < 2; ++mode)
        {
            for (size_t off = 0; off <= n; ++off)
            {
                for (size_t count = 0; off + count <= n; ++count)
                {
                    // whole string
                    {
                        string s(base.data(), base.size());
                        prepare(s, mode);
                        model m = base;
                        const model copy = m;
                        VX_CHECK(s.replace(off, count, s));
                        m.replace(off, count, copy);
                        VX_CHECK(eq_model(s, m));
                    }

                    for (size_t a = 0; a <= n; ++a)
                    {
                        for (size_t len = 0; a + len <= n; ++len)
                        {
                            const model src = base.substr(a, len);
                            model m1 = base;
                            m1.replace(off, count, src);

                            string s1(base.data(), base.size());
                            prepare(s1, mode);
                            VX_CHECK(s1.replace(off, count, s1.data() + a, len));
                            VX_CHECK(eq_model(s1, m1));

                            string s2(base.data(), base.size());
                            prepare(s2, mode);
                            VX_CHECK(s2.replace(off, count, s2, a, len));
                            VX_CHECK(eq_model(s2, m1));

                            string s3(base.data(), base.size());
                            prepare(s3, mode);
                            VX_CHECK(s3.replace(
                                s3.begin() + static_cast<std::ptrdiff_t>(off),
                                s3.begin() + static_cast<std::ptrdiff_t>(off + count),
                                s3.data() + a,
                                len));
                            VX_CHECK(eq_model(s3, m1));
                        }
                    }
                }
            }
        }
    }

    VX_SECTION("append / assign from own buffer")
    {
        for (int mode = 0; mode < 2; ++mode)
        {
            {
                string s(base.data(), base.size());
                prepare(s, mode);
                model m = base;
                const model copy = m;
                VX_CHECK(s.append(s));
                m.append(copy);
                VX_CHECK(eq_model(s, m));
            }

            for (size_t a = 0; a <= n; ++a)
            {
                // operator=(const T*) and assign(const T*) from a suffix of itself
                {
                    string s(base.data(), base.size());
                    prepare(s, mode);
                    s = s.c_str() + a;
                    VX_CHECK(eq_model(s, base.substr(a)));
                }
                {
                    string s(base.data(), base.size());
                    prepare(s, mode);
                    VX_CHECK(s.assign(s.c_str() + a));
                    VX_CHECK(eq_model(s, base.substr(a)));
                }

                for (size_t len = 0; a + len <= n; ++len)
                {
                    const model src = base.substr(a, len);

                    string s1(base.data(), base.size());
                    prepare(s1, mode);
                    VX_CHECK(s1.assign(s1.data() + a, len));
                    VX_CHECK(eq_model(s1, src));

                    string s2(base.data(), base.size());
                    prepare(s2, mode);
                    VX_CHECK(s2.assign(s2, a, len));
                    VX_CHECK(eq_model(s2, src));

                    model m3 = base;
                    m3.append(src);
                    string s3(base.data(), base.size());
                    prepare(s3, mode);
                    VX_CHECK(s3.append(s3.data() + a, len));
                    VX_CHECK(eq_model(s3, m3));

                    string s4(base.data(), base.size());
                    prepare(s4, mode);
                    VX_CHECK(s4.append(s4, a, len));
                    VX_CHECK(eq_model(s4, m3));
                }
            }
        }
    }
}
TEST_CHARS(aliasing)

//=============================================================================
// embedded null characters
//=============================================================================

template <typename T>
static void test_embedded_nulls()
{
    using string = vx::str::basic_string<T>;
    const T nul = T();

    const string s(LIT("a\0b\0c"), 5);
    VX_CHECK(s.size() == 5);
    VX_CHECK(s[0] == T('a') && s[1] == nul && s[2] == T('b') && s[3] == nul && s[4] == T('c'));
    VX_CHECK(valid(s));

    VX_CHECK(s.find(T('c')) == 4);
    VX_CHECK(s.find(nul) == 1);
    VX_CHECK(s.find(nul, 2) == 3);
    VX_CHECK(s.find(LIT("b\0c"), 0, 3) == 2);
    VX_CHECK(s.rfind(nul) == 3);
    VX_CHECK(s.find_first_of(LIT("c\0"), 0, 2) == 1);

    string copy(s);
    VX_CHECK(copy == s);
    VX_CHECK(copy.size() == 5);

    const string prefix(LIT("a\0b"), 3);
    VX_CHECK(s.compare(prefix) > 0);
    VX_CHECK(s.compare(0, 3, prefix) == 0);
    VX_CHECK(s != prefix);

    string t(s);
    VX_CHECK(t.erase(size_t(1), size_t(1)));
    VX_CHECK(t.size() == 4);
    VX_CHECK(t[0] == T('a') && t[1] == T('b') && t[2] == nul && t[3] == T('c'));
    VX_CHECK(valid(t));

    string u;
    VX_CHECK(u.append(LIT("x\0y"), 3));
    VX_CHECK(u.append(prefix));
    VX_CHECK(u.size() == 6);
    VX_CHECK(u[1] == nul && u[3] == T('a') && u[4] == nul && u[5] == T('b'));
    VX_CHECK(valid(u));

    string v(s);
    VX_CHECK(v.insert(size_t(2), LIT("\0\0"), 2));
    VX_CHECK(v.size() == 7);
    VX_CHECK(v[2] == nul && v[3] == nul && v[4] == T('b'));
    VX_CHECK(valid(v));
}
TEST_CHARS(embedded_nulls)

//=============================================================================
// empty strings share static storage, which must never be written to
//=============================================================================

template <typename T>
static void test_static_buffer()
{
    using string = vx::str::basic_string<T>;

    {
        string a;
        a.clear();
        a.pop_back();
        VX_CHECK(a.erase());
        VX_CHECK(a.erase(size_t(0), size_t(0)));
        VX_CHECK(a.resize(0));
        VX_CHECK(a.shrink_to_fit());
        VX_CHECK(a.reserve(0));
        VX_CHECK(a.assign(LIT("")));
        VX_CHECK(a.assign(size_t(0), T('x')));
        VX_CHECK(a.append(LIT("")));
        VX_CHECK(a.append(size_t(0), T('x')));
        VX_CHECK(a.insert(size_t(0), LIT("")));
        VX_CHECK(a.replace(size_t(0), size_t(0), LIT("")));
        a.clear_and_deallocate();
        VX_CHECK(a.empty());
        VX_CHECK(valid(a));
    }

    string fresh;
    VX_CHECK(fresh.c_str()[0] == T());
    VX_CHECK(valid(fresh));

    // the unsafe release path leaves the string pointing at the static buffer too
    {
        string b(LIT("abc"));
        T* p = b.unsafe().release();
        VX_CHECK(p != nullptr);
        typename string::allocator_type al;
        al.deallocate(p, 4);
        VX_CHECK(valid(b));
        VX_CHECK(b.c_str()[0] == T());
    }

    string fresh2;
    VX_CHECK(fresh2.c_str()[0] == T());
}
TEST_CHARS(static_buffer)

//=============================================================================
// error paths
//=============================================================================

template <typename T>
static void test_errors()
{
    using string = vx::str::basic_string<T>;
    string s(LIT("abc"));
    const string o(LIT("xyz"));
    const size_t huge = s.max_size();

    VX_SECTION("out of range")
    {
        auto r1 = s.insert(size_t(4), T('x'));
        VX_CHECK(!r1);
        VX_CHECK_EXPECTED_ERROR(r1, vx::err::out_of_range);

        auto r2 = s.insert(size_t(99), LIT("x"));
        VX_CHECK(!r2);
        VX_CHECK_EXPECTED_ERROR(r2, vx::err::out_of_range);

        auto r3 = s.erase(size_t(4));
        VX_CHECK(!r3);
        VX_CHECK_EXPECTED_ERROR(r3, vx::err::out_of_range);

        auto r4 = s.at(3);
        VX_CHECK(!r4);
        VX_CHECK_EXPECTED_ERROR(r4, vx::err::out_of_range);

        VX_CHECK(!s.assign(o, size_t(4)));
        VX_CHECK(!s.append(o, size_t(4)));
        VX_CHECK(!s.replace(size_t(4), size_t(0), LIT("x")));
        VX_CHECK(!s.replace(size_t(0), size_t(1), o, size_t(4)));
        EQ(s, "abc");
    }

    VX_SECTION("size limits (checked before the source is read, nothing is modified)")
    {
        VX_CHECK(!s.reserve(huge + 1));
        VX_CHECK(!s.resize(huge + 1));
        VX_CHECK(!s.append(LIT("x"), huge));
        VX_CHECK(!s.append(huge, T('x')));
        VX_CHECK(!s.assign(LIT("x"), huge + 1));
        VX_CHECK(!s.assign(huge + 1, T('x')));
        VX_CHECK(!s.replace(size_t(0), size_t(0), LIT("x"), huge));
        VX_CHECK(!s.replace(size_t(0), size_t(0), LIT("x"), static_cast<size_t>(-1))); // would wrap size_t

        auto r = s.insert(size_t(1), LIT("x"), huge);
        VX_CHECK(!r);
        VX_CHECK_EXPECTED_ERROR(r, vx::err::size_error);

        EQ(s, "abc");
        VX_CHECK(valid(s));
    }
}
TEST_CHARS(errors)

//=============================================================================
// misc
//=============================================================================

template <typename T>
static void test_misc()
{
    using string = vx::str::basic_string<T>;

    VX_CHECK(string::npos == static_cast<size_t>(-1));

    VX_SECTION("hash")
    {
        const string a(LIT("abc"));
        string b(LIT("abc"));
        VX_CHECK(b.reserve(100));
        vx::hash<string> h;
        VX_CHECK(h(a) == h(b)); // equal contents hash equal regardless of capacity
        VX_CHECK(h(string()) == h(string()));
        VX_CHECK(h(a) != h(STR("abd")));
    }
}
TEST_CHARS(misc)

//=============================================================================
// unsafe_ops
//=============================================================================

template <typename T>
static void test_unsafe_api()
{
    using string = vx::str::basic_string<T>;

    VX_SECTION("push_back / append assume_capacity")
    {
        string s;
        VX_CHECK(s.reserve(16));
        const T* base = s.data();
        auto ops = s.unsafe();
        const string o(LIT("12345"));
        T arr[] = { T('p'), T('q'), T() };

        ops.push_back_assume_capacity(T('a'));
        EQ(s, "a");
        ops.push_back_assume_capacity(T('b'));
        ops.append_assume_capacity(LIT("cd"));
        EQ(s, "abcd");
        ops.append_assume_capacity(LIT("efg"), 2);
        EQ(s, "abcdef");
        ops.append_assume_capacity(size_t(2), T('x'));
        ops.append_assume_capacity(T('y'));
        EQ(s, "abcdefxxy");
        ops.append_assume_capacity(o);
        ops.append_assume_capacity(o, 1, 2);
        EQ(s, "abcdefxxy1234523");
        VX_CHECK(s.size() == 16);
        VX_CHECK(s.data() == base); // never reallocated
        VX_CHECK(valid(s));

        string t;
        VX_CHECK(t.reserve(8));
        t.unsafe().append_assume_capacity(arr, arr + 2);
        EQ(t, "pq");
    }

    VX_SECTION("assign_assume_capacity")
    {
        string s(LIT("abcdefgh"));
        const T* base = s.data();
        auto ops = s.unsafe();
        const string o(LIT("xyz"));
        const string src(LIT("0123456789"));
        T arr[] = { T('r'), T('s'), T('t'), T() };
        std::initializer_list<T> il{ T('h'), T('i') };

        ops.assign_assume_capacity(o);
        EQ(s, "xyz");
        ops.assign_assume_capacity(LIT("12"));
        EQ(s, "12");
        ops.assign_assume_capacity(LIT("abcdef"), 4);
        EQ(s, "abcd");
        ops.assign_assume_capacity(T('q'));
        EQ(s, "q");
        ops.assign_assume_capacity(size_t(3), T('z'));
        EQ(s, "zzz");
        ops.assign_assume_capacity(il);
        EQ(s, "hi");
        ops.assign_assume_capacity(src, 2, 3);
        EQ(s, "234");
        ops.assign_assume_capacity(arr, arr + 3);
        EQ(s, "rst");
        VX_CHECK(s.data() == base);
        VX_CHECK(valid(s));
    }

    VX_SECTION("assign_assume_no_alias")
    {
        string s(LIT("ab"));
        auto ops = s.unsafe();
        const string o(LIT("hello world"));
        T arr[] = { T('r'), T('s'), T('t'), T() };

        VX_CHECK(ops.assign_assume_no_alias(o)); // grows
        EQ(s, "hello world");
        VX_CHECK(ops.assign_assume_no_alias(LIT("hi")));
        EQ(s, "hi");
        VX_CHECK(ops.assign_assume_no_alias(LIT("abcdef"), 3));
        EQ(s, "abc");
        VX_CHECK(ops.assign_assume_no_alias(o, 6));
        EQ(s, "world");
        VX_CHECK(ops.assign_assume_no_alias(o, 6, 3));
        EQ(s, "wor");

        auto bad = ops.assign_assume_no_alias(o, size_t(99));
        VX_CHECK(!bad);
        EQ(s, "wor");

        VX_CHECK(ops.assign_assume_no_alias(arr, arr + 3));
        EQ(s, "rst");
        VX_CHECK(valid(s));
    }

    VX_SECTION("assign_assume_no_alias_and_capacity")
    {
        string s(LIT("abcdefgh"));
        const T* base = s.data();
        auto ops = s.unsafe();
        const string o(LIT("xyz"));

        ops.assign_assume_no_alias_and_capacity(o);
        EQ(s, "xyz");
        ops.assign_assume_no_alias_and_capacity(LIT("12"));
        EQ(s, "12");
        ops.assign_assume_no_alias_and_capacity(LIT("abcdef"), 4);
        EQ(s, "abcd");
        VX_CHECK(s.data() == base);
        VX_CHECK(valid(s));
    }

    VX_SECTION("resize_assume_capacity")
    {
        string s(LIT("abc"));
        VX_CHECK(s.reserve(10));
        const T* base = s.data();
        auto ops = s.unsafe();

        ops.resize_assume_capacity(6, T('z'));
        EQ(s, "abczzz");
        ops.resize_assume_capacity(2);
        EQ(s, "ab");
        ops.resize_assume_capacity(2);
        EQ(s, "ab");
        ops.resize_assume_capacity(4); // fills with T()
        VX_CHECK(s.size() == 4 && s[2] == T() && s[3] == T());
        ops.resize_assume_capacity(0);
        VX_CHECK(s.empty());
        VX_CHECK(s.data() == base);
        VX_CHECK(valid(s));
    }

    VX_SECTION("erase / pop_back")
    {
        string s(LIT("abcdef"));
        auto ops = s.unsafe();

        (void)ops.erase_assume_in_range(1, 2);
        EQ(s, "adef");
        (void)ops.erase_assume_in_range(0, 0);
        (void)ops.erase_assume_in_range(4, 0);
        EQ(s, "adef");
        (void)ops.erase_assume_in_range(2, 2);
        EQ(s, "ad");

        ops.pop_back_assume_nonempty();
        EQ(s, "a");
        ops.pop_back_assume_nonempty();
        VX_CHECK(s.empty());
        VX_CHECK(valid(s));
    }

    VX_SECTION("copy / substr / view assume_in_range")
    {
        string s(LIT("abcdef"));
        auto ops = s.unsafe();

        T buf[8]{};
        VX_CHECK(ops.copy_assume_in_range(buf, 3, 1) == 3);
        VX_CHECK(buf_eq(buf, LIT("bcd"), 3));

        EQ(ops.substr_assume_in_range(1, 3), "bcd");

        auto v = ops.view_assume_in_range(1, 3);
        VX_CHECK(v.size() == 3);
        VX_CHECK(v.data() == s.data() + 1);
    }

    VX_SECTION("acquire / release")
    {
        typename string::allocator_type al;

        T* p = al.allocate(4); // 3 chars + terminator
        p[0] = T('1');
        p[1] = T('2');
        p[2] = T('3');
        p[3] = T();

        string s(LIT("existing buffer is freed"));
        VX_CHECK(s.unsafe().acquire(p));
        VX_CHECK(s.data() == p);
        VX_CHECK(s.capacity() == 3);
        EQ(s, "123");

        T* q = s.unsafe().release();
        VX_CHECK(q == p);
        VX_CHECK(s.empty());
        VX_CHECK(s.capacity() == 0);
        VX_CHECK(valid(s));
        VX_CHECK(q[0] == T('1')); // buffer is still alive and now owned by the caller
        al.deallocate(q, 4);

        VX_CHECK(s.append(LIT("ok")));
        EQ(s, "ok");

        string e;
        VX_CHECK(e.unsafe().release() == nullptr); // nothing to release
        VX_CHECK(valid(e));
    }
}
TEST_CHARS(unsafe_api)

//=============================================================================
// randomized comparison against std::basic_string
//=============================================================================

template <typename T>
static void test_stress()
{
    using string = vx::str::basic_string<T>;
    using model = std::basic_string<T>;

    for (unsigned seed = 1; seed <= 6; ++seed)
    {
        string s;
        model m;
        unsigned state = seed * 7919u;

        auto rnd = [&](unsigned n) -> unsigned
        {
            state = state * 1664525u + 1013904223u;
            return (state >> 8) % n;
        };
        auto ch = [&]() -> T
        {
            return static_cast<T>('a' + rnd(5));
        };
        auto chunk = [&]() -> model
        {
            model c;
            const unsigned len = rnd(6);
            for (unsigned i = 0; i < len; ++i)
            {
                c.push_back(ch());
            }
            return c;
        };
        auto size_u = [&]() -> unsigned
        {
            return static_cast<unsigned>(m.size());
        };

        for (int iter = 0; iter < 1500; ++iter)
        {
            switch (rnd(18))
            {
                case 0:
                {
                    const T c = ch();
                    VX_CHECK(s.push_back(c));
                    m.push_back(c);
                    break;
                }
                case 1:
                {
                    const size_t off = rnd(size_u() + 1);
                    const T c = ch();
                    VX_CHECK(s.insert(off, c));
                    m.insert(off, size_t(1), c);
                    break;
                }
                case 2:
                {
                    const size_t off = rnd(size_u() + 1);
                    const size_t cnt = rnd(6);
                    const T c = ch();
                    VX_CHECK(s.insert(off, cnt, c));
                    m.insert(off, cnt, c);
                    break;
                }
                case 3:
                {
                    const size_t off = rnd(size_u() + 1);
                    const model c = chunk();
                    VX_CHECK(s.insert(off, c.data(), c.size()));
                    m.insert(off, c);
                    break;
                }
                case 4:
                {
                    const size_t off = rnd(size_u() + 1);
                    const size_t cnt = rnd(2) ? string::npos : rnd(size_u() - static_cast<unsigned>(off) + 1);
                    VX_CHECK(s.erase(off, cnt));
                    m.erase(off, cnt);
                    break;
                }
                case 5:
                    if (!m.empty())
                    {
                        const size_t off = rnd(size_u());
                        VX_CHECK(s.erase(off, size_t(1)));
                        m.erase(off, size_t(1));
                    }
                    break;
                case 6:
                    s.pop_back();
                    if (!m.empty())
                    {
                        m.pop_back();
                    }
                    break;
                case 7:
                {
                    const size_t n = rnd(40);
                    const T c = ch();
                    VX_CHECK(s.resize(n, c));
                    m.resize(n, c);
                    break;
                }
                case 8:
                    VX_CHECK(s.reserve(rnd(64)));
                    break;
                case 9:
                    (void)s.shrink_to_fit();
                    break;
                case 10:
                {
                    const size_t off = rnd(size_u() + 1);
                    const size_t cnt = rnd(size_u() - static_cast<unsigned>(off) + 1);
                    const model c = chunk();
                    VX_CHECK(s.replace(off, cnt, c.data(), c.size()));
                    m.replace(off, cnt, c);
                    break;
                }
                case 11:
                {
                    const model c = chunk();
                    VX_CHECK(s.append(c.data(), c.size()));
                    m.append(c);
                    break;
                }
                case 12:
                {
                    const size_t cnt = rnd(5);
                    const T c = ch();
                    VX_CHECK(s.append(cnt, c));
                    m.append(cnt, c);
                    break;
                }
                case 13: // insert from own buffer
                {
                    const unsigned a = rnd(size_u() + 1);
                    const unsigned len = rnd(size_u() - a + 1);
                    const size_t off = rnd(size_u() + 1);
                    const model src = m.substr(a, len);
                    VX_CHECK(s.insert(off, s.data() + a, len));
                    m.insert(off, src);
                    break;
                }
                case 14: // replace from own buffer
                {
                    const unsigned a = rnd(size_u() + 1);
                    const unsigned len = rnd(size_u() - a + 1);
                    const size_t off = rnd(size_u() + 1);
                    const size_t cnt = rnd(size_u() - static_cast<unsigned>(off) + 1);
                    const model src = m.substr(a, len);
                    VX_CHECK(s.replace(off, cnt, s.data() + a, len));
                    m.replace(off, cnt, src);
                    break;
                }
                case 15: // generic (non-pointer) iterator path
                {
                    const model add = chunk();
                    std::list<T> l(add.begin(), add.end());
                    const size_t off = rnd(size_u() + 1);
                    VX_CHECK(s.insert(off, l.begin(), l.end()));
                    m.insert(off, add);
                    break;
                }
                case 16: // assign (occasionally resets the string)
                {
                    const model c = chunk();
                    VX_CHECK(s.assign(c.data(), c.size()));
                    m.assign(c);
                    break;
                }
                case 17: // read-only operations
                {
                    const model pat = chunk();
                    const size_t off = rnd(size_u() + 1);
                    const size_t n = pat.size();
                    const T* p = pat.data();

                    VX_CHECK(eq_model(s.substr(off, 3), m.substr(off, 3)));
                    VX_CHECK(static_cast<size_t>(s.find(p, off, n)) == m.find(p, off, n));
                    VX_CHECK(static_cast<size_t>(s.rfind(p, off, n)) == m.rfind(p, off, n));
                    VX_CHECK(static_cast<size_t>(s.find_first_of(p, off, n)) == m.find_first_of(p, off, n));
                    VX_CHECK(static_cast<size_t>(s.find_last_of(p, off, n)) == m.find_last_of(p, off, n));
                    VX_CHECK(static_cast<size_t>(s.find_first_not_of(p, off, n)) == m.find_first_not_of(p, off, n));
                    VX_CHECK(static_cast<size_t>(s.find_last_not_of(p, off, n)) == m.find_last_not_of(p, off, n));
                    VX_CHECK(sgn(s.compare(p)) == sgn(m.compare(p)));
                    break;
                }
            }

            VX_CHECK(eq_model(s, m));
        }
    }
}
TEST_CHARS(stress)

//=============================================================================
// allocators
//
// Stateful allocators are only used through copy / swap here: moves are
// supported for stateless allocators only.
//=============================================================================

using talloc = vx::test::tracking_allocator<char>;
using tstr = vx::str::basic_string<char, talloc>;
using vx::test::allocator_stats;

VX_TEST_CASE(tracking_allocator_balance)
{
    allocator_stats st;
    {
        tstr s(talloc(1, &st));
        VX_CHECK(s.empty());
        VX_CHECK(st.allocate_calls == 0); // the empty string never allocates
        VX_CHECK(s.get_allocator().id() == 1);

        VX_CHECK(s.reserve(100));
        VX_CHECK(st.allocate_calls == 1);

        for (int i = 0; i < 100; ++i)
        {
            VX_CHECK(s.push_back('a'));
        }
        VX_CHECK(st.allocate_calls == 1); // no growth inside reserved capacity

        for (int i = 0; i < 1000; ++i)
        {
            VX_CHECK(s.push_back('b'));
        }
        VX_CHECK(s.insert(size_t(5), size_t(50), 'c'));
        VX_CHECK(s.erase(size_t(0), size_t(20)));
        VX_CHECK(s.resize(5000, 'd'));
        VX_CHECK(s.append("tail"));
        VX_CHECK(s.replace(size_t(3), size_t(10), "a replacement that is longer than what it replaces"));
        VX_CHECK(s.shrink_to_fit());

        tstr c(s);
        c = s;
        VX_CHECK(c == s);
        c.swap(s);
        VX_CHECK(c == s);

        VX_CHECK(s.assign(size_t(10), 'e'));
        s.clear();
        s.clear_and_deallocate();
    }
    VX_CHECK(st.live_allocations == 0);
    VX_CHECK(st.allocate_calls == st.deallocate_calls);
    VX_CHECK(st.reallocate_calls == 0); // the string never calls reallocate()
}

VX_TEST_CASE(stateful_allocator_propagation)
{
    allocator_stats s1, s2;
    {
        tstr a(size_t(4), 'x', talloc(1, &s1));

        tstr b(a); // copy ctor keeps the source's allocator
        VX_CHECK(b.get_allocator().id() == 1);

        tstr c(a, talloc(2, &s2)); // explicit allocator wins
        VX_CHECK(c.get_allocator().id() == 2);
        VX_CHECK(c == a);

        tstr e(size_t(3), 'y', talloc(2, &s2));
        e = a; // copy assign does NOT propagate: the buffer stays with allocator 2
        VX_CHECK(e.get_allocator().id() == 2);
        VX_CHECK(e == a);
        VX_CHECK(s2.live_allocations == 2); // c and e

        tstr g(size_t(1), 'a', talloc(1, &s1));
        tstr h(size_t(1), 'b', talloc(2, &s2));
        g.swap(h); // swap exchanges allocators together with buffers
        VX_CHECK(g.get_allocator().id() == 2 && g[0] == 'b');
        VX_CHECK(h.get_allocator().id() == 1 && h[0] == 'a');
    }
    VX_CHECK(s1.live_allocations == 0);
    VX_CHECK(s2.live_allocations == 0);
}

#endif // !defined(ONLY_BASIC_TESTS)

//=========================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
