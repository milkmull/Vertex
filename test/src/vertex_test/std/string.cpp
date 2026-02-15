#include "vertex/std/string.hpp"
#include "vertex_test/test.hpp"

//=============================================================================

template <intmax_t N, intmax_t D = 1>
using growth_rate = typename vx::string::growth_rate_type<N, D>;

VX_TEST_CASE(constructors)
{
    VX_SECTION("default")
    {
        vx::string s;
        VX_CHECK(s.is_valid());
        VX_CHECK(s.size() == 0);
        VX_CHECK(s.capacity() == 0);
        VX_CHECK(*s.data() == 0);
    }

    VX_SECTION("copy")
    {
        const char* cstr = "hello";
        const size_t size = vx::str::length(cstr);

        const vx::string s1(cstr);
        vx::string s(s1);

        VX_CHECK(s.is_valid());
        VX_CHECK(s.size() == size);
        VX_CHECK(s.capacity() == size);
        VX_CHECK(s.compare(cstr) == 0);
    }

    VX_SECTION("move")
    {
        const char* cstr = "hello";
        const size_t size = vx::str::length(cstr);

        vx::string s1(cstr);
        vx::string s(std::move(s1));

        VX_CHECK(!s1.is_valid());
        VX_CHECK(s.is_valid());
        VX_CHECK(s.size() == size);
        VX_CHECK(s.capacity() == size);
        VX_CHECK(s.compare(cstr) == 0);
    }

    // substring copy (two-arg + three-arg)
    VX_SECTION("substring copy")
    {
        const char* cstr = "hello";
        const vx::string src(cstr);

        {
            vx::string s(src, 2);
            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == 3);
            VX_CHECK(s.capacity() == 3);
            VX_CHECK(s.compare("llo") == 0);
        }

        {
            vx::string s(src, 1, 2);
            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == 2);
            VX_CHECK(s.capacity() == 2);
            VX_CHECK(s.compare("el") == 0);
        }

        {
            vx::string s(src, 99); // invalid offset
            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == 0);
            VX_CHECK(s.capacity() == 0);
        }
    }

    VX_SECTION("char")
    {
        const char c = 'x';
        constexpr size_t count = 20;
        vx::string s(count, c);

        VX_CHECK(s.is_valid());
        VX_CHECK(s.size() == count);
        VX_CHECK(s.capacity() == count);

        for (const char sc : s)
        {
            VX_CHECK(sc == c);
        }
    }

    VX_SECTION("pointer")
    {
        const char* cstr = "hello";
        const size_t size = vx::str::length(cstr);

        {
            vx::string s(cstr);
            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == size);
            VX_CHECK(s.capacity() == size);
            VX_CHECK(s.compare(cstr) == 0);
        }

        {
            vx::string s(cstr, 3);
            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == 3);
            VX_CHECK(s.capacity() == 3);
            VX_CHECK(s.compare("hel") == 0);
        }
    }

    VX_SECTION("iterator range")
    {
        const char* cstr = "hello";
        const size_t size = vx::str::length(cstr);

        // iterator range (pointer iterator path)
        {
            const vx::string s1(cstr);
            vx::string s(s1.begin(), s1.end());

            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == size);
            VX_CHECK(s.capacity() == size);
            VX_CHECK(s.compare(cstr) == 0);
        }

        // iterator range (non-pointer iterator path)
        {
            const char arr[] = { 'h', 'e', 'l', 'l', 'o' };
            const size_t count = vx::mem::array_size(arr);
            vx::string s(std::begin(arr), std::end(arr));

            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == count);
            VX_CHECK(s.capacity() == count);
            VX_CHECK(s.compare(cstr) == 0);
        }
    }

    VX_SECTION("list")
    {
        vx::string s = { 'h', 'e', 'l', 'l', 'o' };
        VX_CHECK(s.is_valid());
        VX_CHECK(s.size() == 5);
        VX_CHECK(s.capacity() == 5);
        VX_CHECK(s.compare("hello") == 0);
    }

    VX_SECTION("view")
    {
        const char* cs = "hello";
        vx::string_view v(cs);


        {
            vx::string s(v);
            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == v.size());
            VX_CHECK(s.capacity() == v.size());
            VX_CHECK(s.compare(cs) == 0);
        }

        {
            vx::string s(v, 1, 2);
            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == 2);
            VX_CHECK(s.capacity() == 2);
            VX_CHECK(s.compare("el") == 0);
        }

        {
            vx::string s(v, 99); // invalid offset
            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == 0);
            VX_CHECK(s.capacity() == 0);
        }
    }
}

//==============================================================================

VX_TEST_CASE(test_string_conversion_operators)
{
    const vx::string s("hello");

    VX_SECTION("string_view")
    {
        vx::string_view sv = s;
        VX_CHECK(sv.size() == 5);
        VX_CHECK(sv == "hello");
    }

    VX_SECTION("std::string")
    {
        std::string s1 = s;
        VX_CHECK(s1.size() == 5);
        VX_CHECK(s1 == "hello");
    }
}

//==============================================================================

VX_TEST_CASE(test_assignment_operator)
{
    VX_SECTION("copy")
    {
        vx::string a = "hello";
        vx::string b = "world";
        b = a;

        VX_CHECK(b.size() == a.size());
        VX_CHECK(b.compare(a) == 0);
    }

    VX_SECTION("move")
    {
        vx::string a = "world";
        const size_t size = a.size();

        vx::string b = "hello";
        b = std::move(a);

        VX_CHECK(!a.is_valid());
        VX_CHECK(b.size() == size);
    }

    VX_SECTION("char")
    {
        vx::string s(1, 'y');
        s = 'x';

        VX_CHECK(s.size() == 1);
        VX_CHECK(s.compare("x") == 0);
    }

    VX_SECTION("pointer")
    {
        const char* cs = "abc";
        vx::string s = "def";
        s = "abc";

        VX_CHECK(s.size() == 3);
        VX_CHECK(s.compare(cs) == 0);
    }

    VX_SECTION("list")
    {
        vx::string s = "lo";
        s = { 'h', 'i' };

        VX_CHECK(s.size() == 2);
        VX_CHECK(s.compare("hi") == 0);
    }

    VX_SECTION("view")
    {
        vx::string_view v("hello", 5);
        vx::string s = "world";
        s = v;

        VX_CHECK(s.size() == 5);
        VX_CHECK(s.compare(v) == 0);
    }
}

//==============================================================================

VX_TEST_CASE(test_assign)
{
    VX_SECTION("copy")
    {
        const char* cstr = "hello";
        const size_t size = vx::str::length(cstr);

        const vx::string s1(cstr);
        vx::string s = "world";
        s.assign(s1);

        VX_CHECK(s.is_valid());
        VX_CHECK(s.size() == size);
        VX_CHECK(s.compare(cstr) == 0);
    }

    VX_SECTION("move")
    {
        const char* cstr = "hello";
        const size_t size = vx::str::length(cstr);

        vx::string s1(cstr);
        vx::string s = "world";
        s.assign(std::move(s1));

        VX_CHECK(!s1.is_valid());
        VX_CHECK(s.is_valid());
        VX_CHECK(s.size() == size);
        VX_CHECK(s.compare(cstr) == 0);
    }

    // substring copy (two-arg + three-arg)
    VX_SECTION("substring copy")
    {
        const char* cstr = "hello";
        const vx::string src(cstr);

        {
            vx::string s = "world";
            s.assign(src, 2);

            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == 3);
            VX_CHECK(s.compare("llo") == 0);
        }

        {
            vx::string s = "world";
            s.assign(src, 1, 2);

            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == 2);
            VX_CHECK(s.compare("el") == 0);
        }

        {
            vx::string s = "world";
            s.assign(src, 99); // invalid offset

            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == 0);
        }
    }

    VX_SECTION("char")
    {
        // single
        {
            const char c = 'x';

            vx::string s(1, 'y');
            s.assign(c);

            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == 1);
            VX_CHECK(s[0] == c);
        }

        // repeated
        {
            const char c = 'x';
            constexpr size_t count = 20;

            vx::string s(count, 'y');
            s.assign(count, c);

            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == count);

            for (const char sc : s)
            {
                VX_CHECK(sc == c);
            }
        }
    }

    VX_SECTION("pointer")
    {
        const char* cstr = "hello";
        const size_t size = vx::str::length(cstr);

        {
            vx::string s = "world";
            s.assign(cstr);

            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == size);
            VX_CHECK(s.compare(cstr) == 0);
        }

        {
            vx::string s = "world";
            s.assign(cstr, 3);

            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == 3);
            VX_CHECK(s.compare("hel") == 0);
        }
    }

    VX_SECTION("iterator range")
    {
        const char* cstr = "hello";
        const size_t size = vx::str::length(cstr);

        // iterator range (pointer iterator path)
        {
            const vx::string s1(cstr);
            vx::string s = "world";
            s.assign(s1.begin(), s1.end());

            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == size);
            VX_CHECK(s.compare(cstr) == 0);
        }

        // iterator range (non-pointer iterator path)
        {
            const char arr[] = { 'h', 'e', 'l', 'l', 'o' };
            const size_t count = vx::mem::array_size(arr);

            vx::string s = "world";
            s.assign(std::begin(arr), std::end(arr));

            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == count);
            VX_CHECK(s.compare(cstr) == 0);
        }
    }

    VX_SECTION("list")
    {
        vx::string s = "world";
        s.assign({ 'h', 'e', 'l', 'l', 'o' });

        VX_CHECK(s.is_valid());
        VX_CHECK(s.size() == 5);
        VX_CHECK(s.compare("hello") == 0);
    }

    VX_SECTION("view")
    {
        const char* cs = "hello";
        vx::string_view v(cs);

        {
            vx::string s = "world";
            s.assign(v);

            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == v.size());
            VX_CHECK(s.compare(cs) == 0);
        }

        {
            vx::string s = "world";
            s.assign(v, 1, 2);

            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == 2);
            VX_CHECK(s.compare("el") == 0);
        }

        {
            vx::string s = "world";
            s.assign(v, 99); // invalid offset

            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == 0);
        }
    }
}

//=============================================================================

VX_TEST_CASE(test_element_access)
{
    VX_SECTION("front / front const")
    {
        vx::string s = "hello";
        VX_CHECK(s.front() == 'h');

        const vx::string& cs = s;
        VX_CHECK(cs.front() == 'h');
    }

    VX_SECTION("back / back const")
    {
        vx::string s = "hello";
        VX_CHECK(s.back() == 'o');

        const vx::string& cs = s;
        VX_CHECK(cs.back() == 'o');
    }

    VX_SECTION("data / data const")
    {
        const char* cstr = "hello";

        vx::string s = cstr;
        VX_CHECK(s.data() != nullptr);
        VX_CHECK(std::memcmp(s.data(), cstr, s.size()) == 0);

        const vx::string& cs = s;
        VX_CHECK(cs.data() != nullptr);
        VX_CHECK(std::memcmp(cs.data(), cstr, cs.size()) == 0);
    }

    VX_SECTION("operator[] / operator[] const")
    {
        const char* cstr = "hello";

        vx::string s = cstr;
        for (size_t i = 0; i < s.size(); ++i)
        {
            VX_CHECK(s[i] == cstr[i]);
        }

        const vx::string& cs = s;
        for (size_t i = 0; i < cs.size(); ++i)
        {
            VX_CHECK(cs[i] == cstr[i]);
        }
    }

    VX_SECTION("c_str")
    {
        const char* cstr = "hello";
        vx::string s = cstr;
        VX_CHECK(s.c_str() != nullptr);
        VX_CHECK(vx::str::compare(s.c_str(), cstr) == 0);
    }
}

VX_TEST_CASE(test_iterators)
{
    VX_SECTION("begin / end")
    {
        const char* cstr = "hello";
        vx::string s = cstr;

        size_t idx = 0;
        for (auto it = s.begin(); it != s.end(); ++it, ++idx)
        {
            VX_CHECK(*it == cstr[idx]);
        }
        VX_CHECK(idx == s.size());
    }

    VX_SECTION("begin const / end const")
    {
        const char* cstr = "hello";
        vx::string s = cstr;

        const vx::string& cs = s;
        size_t idx = 0;
        for (auto it = cs.begin(); it != cs.end(); ++it, ++idx)
        {
            VX_CHECK(*it == cstr[idx]);
        }
        VX_CHECK(idx == cs.size());
    }

    VX_SECTION("cbegin / cend")
    {
        const char* cstr = "hello";
        vx::string s = cstr;

        size_t idx = 0;
        for (auto it = s.cbegin(); it != s.cend(); ++it, ++idx)
        {
            VX_CHECK(*it == cstr[idx]);
        }
        VX_CHECK(idx == s.size());
    }

    VX_SECTION("rbegin / rend")
    {
        const char* cstr = "hello";
        vx::string s = cstr;

        size_t idx = s.size();
        for (auto it = s.rbegin(); it != s.rend(); ++it)
        {
            --idx;
            VX_CHECK(*it == cstr[idx]);
        }
        VX_CHECK(idx == 0);
    }

    VX_SECTION("rbegin const / rend const")
    {
        const char* cstr = "hello";
        vx::string s = cstr;

        const vx::string& cs = s;

        size_t idx = cs.size();
        for (auto it = cs.rbegin(); it != cs.rend(); ++it)
        {
            --idx;
            VX_CHECK(*it == cstr[idx]);
        }
        VX_CHECK(idx == 0);
    }

    VX_SECTION("crbegin / crend")
    {
        const char* cstr = "hello";
        vx::string s = cstr;

        size_t idx = s.size();
        for (auto it = s.crbegin(); it != s.crend(); ++it)
        {
            --idx;
            VX_CHECK(*it == cstr[idx]);
        }
        VX_CHECK(idx == 0);
    }
}

//=============================================================================

VX_TEST_CASE(test_append)
{
    VX_SECTION("append string")
    {
        vx::string a("hello");
        vx::string b(" world");

        a.append(b);

        VX_CHECK(a.compare("hello world") == 0);
        VX_CHECK(a.size() == 11);
    }

    VX_SECTION("append string with offset")
    {
        vx::string a("abc");
        vx::string b("012345");

        a.append(b, 2); // "2345"
        VX_CHECK(a.compare("abc2345") == 0);

        a.assign("abc");
        a.append(b, 1, 3); // "123"
        VX_CHECK(a.compare("abc123") == 0);

        a.assign("abc");
        a.append(b, 99); // invalid offset => no-op
        VX_CHECK(a.compare("abc") == 0);
    }

    VX_SECTION("append char")
    {
        vx::string s("hi");
        s.append('!');
        VX_CHECK(s.compare("hi!") == 0);
        VX_CHECK(s.size() == 3);
    }

    VX_SECTION("append char repeated")
    {
        vx::string s("x");
        s.append(4, 'a'); // "xaaaa"
        VX_CHECK(s.compare("xaaaa") == 0);
        VX_CHECK(s.size() == 5);
    }

    VX_SECTION("append pointer")
    {
        vx::string s("foo");
        const char* p = "bar";
        s.append(p);
        VX_CHECK(s.compare("foobar") == 0);
        VX_CHECK(s.size() == 6);
    }

    VX_SECTION("append pointer with count")
    {
        vx::string s("foo");
        const char* p = "barbaz";
        s.append(p, 3); // "bar"
        VX_CHECK(s.compare("foobar") == 0);
        VX_CHECK(s.size() == 6);
    }

    VX_SECTION("append initializer_list")
    {
        vx::string s("test");
        s.append({ '1', '2', '3' });
        VX_CHECK(s.compare("test123") == 0);
        VX_CHECK(s.size() == 7);
    }

    VX_SECTION("append iterator range (pointer iter path)")
    {
        vx::string s("A");
        const char* p = "xyz";
        s.append(p, p + 3);
        VX_CHECK(s.compare("Axyz") == 0);
    }

    VX_SECTION("append iterator range (non-pointer iter path)")
    {
        vx::string s("A");
        const char arr[] = { 'x', 'y', 'z' };
        s.append(std::begin(arr), std::end(arr));
        VX_CHECK(s.compare("Axyz") == 0);
    }

    VX_SECTION("append view")
    {
        vx::string s("num:");
        vx::string_view v("12345");

        s.append(v);
        VX_CHECK(s.compare("num:12345") == 0);
    }

    VX_SECTION("append view with offset")
    {
        vx::string s("num:");
        vx::string_view v("ABCDE");

        s.append(v, 1, 3); // "BCD"
        VX_CHECK(s.compare("num:BCD") == 0);

        s.assign("num:");
        s.append(v, 99); // invalid offset => no-op
        VX_CHECK(s.compare("num:") == 0);
    }
}

//=============================================================================

VX_TEST_CASE(test_operator_plus_equal)
{
    VX_SECTION("+= string")
    {
        vx::string a("hello");
        vx::string b(" world");

        a += b;

        VX_CHECK(a.compare("hello world") == 0);
        VX_CHECK(a.size() == 11);
    }

    VX_SECTION("+= char")
    {
        vx::string s("abc");
        s += 'd';

        VX_CHECK(s.compare("abcd") == 0);
        VX_CHECK(s.size() == 4);
    }

    VX_SECTION("+= pointer")
    {
        vx::string s("foo");
        const char* p = "bar";

        s += p;

        VX_CHECK(s.compare("foobar") == 0);
        VX_CHECK(s.size() == 6);
    }

    VX_SECTION("+= initializer_list")
    {
        vx::string s("hello");
        s += { ' ', 'X', 'Y' };

        VX_CHECK(s.compare("hello XY") == 0);
        VX_CHECK(s.size() == 8);
    }

    VX_SECTION("+= view")
    {
        vx::string s("start:");
        vx::string_view v("END");

        s += v;

        VX_CHECK(s.compare("start:END") == 0);
        VX_CHECK(s.size() == 9);
    }
}

VX_TEST_CASE(test_operator_plus)
{
    //=========================================================================
    // basic_string + basic_string
    //=========================================================================

    VX_SECTION("string + string")
    {
        const vx::string a("hello");
        const vx::string b(" world");

        vx::string r = a + b;
        VX_CHECK(r.compare("hello world") == 0);
    }

    VX_SECTION("string&& + string&&")
    {
        vx::string a("foo");
        vx::string b("bar");

        vx::string r = std::move(a) + std::move(b);
        VX_CHECK(r.compare("foobar") == 0);
    }

    //=========================================================================
    // basic_string + char
    //=========================================================================

    VX_SECTION("string + char")
    {
        const vx::string a("abc");

        vx::string r = a + 'd';
        VX_CHECK(r.compare("abcd") == 0);
    }

    VX_SECTION("char + string")
    {
        const vx::string s("abc");

        vx::string r = 'x' + s;
        VX_CHECK(r.compare("xabc") == 0);
    }

    //=========================================================================
    // basic_string + pointer
    //=========================================================================

    VX_SECTION("string + pointer")
    {
        const vx::string a("foo");
        const char* p = "bar";

        vx::string r = a + p;
        VX_CHECK(r.compare("foobar") == 0);
    }

    VX_SECTION("pointer + string")
    {
        const char* p = "foo";
        const vx::string s("bar");

        vx::string r = p + s;
        VX_CHECK(r.compare("foobar") == 0);
    }

    //=========================================================================
    // basic_string + view
    //=========================================================================

    VX_SECTION("string + view")
    {
        const vx::string a("pre-");
        vx::string_view v("post");

        vx::string r = a + v;
        VX_CHECK(r.compare("pre-post") == 0);
    }

    VX_SECTION("view + string")
    {
        vx::string_view v("123");
        const vx::string s("abc");

        vx::string r = v + s;
        VX_CHECK(r.compare("123abc") == 0);
    }

    //=========================================================================
    // string&& + string
    //=========================================================================

    VX_SECTION("string&& + string")
    {
        vx::string a("hi ");
        const vx::string b("there");

        vx::string r = std::move(a) + b;
        VX_CHECK(r.compare("hi there") == 0);
    }

    // string + string&&
    VX_SECTION("string + string&&")
    {
        const vx::string a("hi ");
        vx::string b("there");

        vx::string r = a + std::move(b);
        VX_CHECK(r.compare("therehi ") == 0);
    }

    //=========================================================================
    // string&& + char
    //=========================================================================

    VX_SECTION("string&& + char")
    {
        vx::string a("abc");
        vx::string r = std::move(a) + 'Z';
        VX_CHECK(r.compare("abcZ") == 0);
    }

    // char + string&&
    VX_SECTION("char + string&&")
    {
        vx::string b("abc");

        vx::string r = 'Z' + std::move(b);
        VX_CHECK(r.compare("abcZ") == 0);
    }

    //=========================================================================
    // string&& + pointer
    //=========================================================================

    VX_SECTION("string&& + pointer")
    {
        vx::string a("foo");
        const char* p = "BAR";

        vx::string r = std::move(a) + p;
        VX_CHECK(r.compare("fooBAR") == 0);
    }

    // pointer + string&&
    VX_SECTION("pointer + string&&")
    {
        const char* p = "foo";
        vx::string b("BAR");

        vx::string r = p + std::move(b);
        VX_CHECK(r.compare("fooBAR") == 0);
    }

    //=========================================================================
    // string&& + view
    //=========================================================================

    VX_SECTION("string&& + view")
    {
        vx::string a("begin:");
        vx::string_view v("end");

        vx::string r = std::move(a) + v;
        VX_CHECK(r.compare("begin:end") == 0);
    }

    // view + string&&
    VX_SECTION("view + string&&")
    {
        vx::string_view v("123");
        vx::string s("abc");

        vx::string r = v + std::move(s);
        VX_CHECK(r.compare("123abc") == 0);
    }
}

//=============================================================================

VX_TEST_CASE(test_insert)
{
    //=========================================================================
    // insert(off, string)
    //=========================================================================

    VX_SECTION("insert index: string")
    {
        vx::string s("hello");
        vx::string other("XX");

        s.insert(2, other);

        VX_CHECK(s.compare("heXXllo") == 0);
    }

    VX_SECTION("insert index: string with offset")
    {
        vx::string s("hello");
        vx::string other("ABCDE");

        s.insert(1, other, 2, 2); // insert "CD"

        VX_CHECK(s.compare("hCDello") == 0);

        // invalid other offset => no-op
        s.assign("hello");
        s.insert(1, other, 99);
        VX_CHECK(s.compare("hello") == 0);
    }

    //=========================================================================
    // insert(off, char)
    //=========================================================================

    VX_SECTION("insert index: char")
    {
        vx::string s("abcd");
        s.insert(2, 'X'); // a b X c d

        VX_CHECK(s.compare("abXcd") == 0);
    }

    VX_SECTION("insert index: repeated char")
    {
        vx::string s("abcd");
        s.insert(1, 3, 'Z'); // a ZZZ b c d

        VX_CHECK(s.compare("aZZZbcd") == 0);
    }

    //=========================================================================
    // insert(off, pointer)
    //=========================================================================

    VX_SECTION("insert index: pointer")
    {
        vx::string s("hello");
        const char* p = "XYZ";

        s.insert(3, p);

        VX_CHECK(s.compare("helXYZlo") == 0);
    }

    VX_SECTION("insert index: pointer with count")
    {
        vx::string s("hello");
        const char* p = "ABCDE";

        s.insert(3, p, 2); // "AB"

        VX_CHECK(s.compare("helABlo") == 0);
    }

    //=========================================================================
    // insert(off, initializer_list)
    //=========================================================================

    VX_SECTION("insert index: initializer_list")
    {
        vx::string s("hello");
        s.insert(1, { 'X', 'Y' });

        VX_CHECK(s.compare("hXYello") == 0);
    }

    //=========================================================================
    // insert(off, iterators)
    //=========================================================================

    VX_SECTION("insert index: iterator range (pointer iter path)")
    {
        vx::string s("hello");
        const char* p = "xyz";

        s.insert(2, p, p + 3);

        VX_CHECK(s.compare("hexyzllo") == 0);
    }

    VX_SECTION("insert index: iterator range (non-pointer iter path)")
    {
        vx::string s("hello");
        const char arr[] = { '1', '2', '3' };

        s.insert(4, std::begin(arr), std::end(arr));

        VX_CHECK(s.compare("hell123o") == 0);
    }

    //=========================================================================
    // insert(off, view)
    //=========================================================================

    VX_SECTION("insert index: view")
    {
        vx::string s("hello");
        vx::string_view v("XYZ");

        s.insert(1, v);

        VX_CHECK(s.compare("hXYZello") == 0);
    }

    VX_SECTION("insert index: view with offset")
    {
        vx::string s("hello");
        vx::string_view v("ABCDE");

        s.insert(2, v, 1, 3); // "BCD"

        VX_CHECK(s.compare("heBCDllo") == 0);

        // invalid view offset => no-op
        s.assign("hello");
        s.insert(2, v, 99);
        VX_CHECK(s.compare("hello") == 0);
    }

    //=========================================================================
    // ITERATOR-BASED INSERT VERSIONS
    // Same functionality, but off is replaced with an iterator
    //=========================================================================

    VX_SECTION("insert iterator: string")
    {
        vx::string s("hello");
        vx::string other("XX");

        s.insert(s.begin() + 2, other.begin(), other.end());

        VX_CHECK(s.compare("heXXllo") == 0);
    }

    VX_SECTION("insert iterator: char")
    {
        vx::string s("abcd");

        // single char insertion via iterator+char form
        s.insert(s.begin() + 2, 'X');

        VX_CHECK(s.compare("abXcd") == 0);
    }

    VX_SECTION("insert iterator: repeated char")
    {
        vx::string s("abcd");
        s.insert(s.begin() + 1, 3, 'Z'); // a ZZZ b c d

        VX_CHECK(s.compare("aZZZbcd") == 0);
    }

    VX_SECTION("insert iterator: pointer")
    {
        vx::string s("hello");
        const char* p = "XYZ";

        s.insert(s.begin() + 3, p, p + 3);

        VX_CHECK(s.compare("helXYZlo") == 0);
    }

    VX_SECTION("insert iterator: initializer_list")
    {
        vx::string s("hello");

        s.insert(s.begin() + 1, { 'X', 'Y' });

        VX_CHECK(s.compare("hXYello") == 0);
    }

    VX_SECTION("insert iterator: view")
    {
        vx::string s("hello");
        vx::string_view v("XYZ");

        // iterator version handles same logic through the template path
        s.insert(s.begin() + 1, v.begin(), v.end());

        VX_CHECK(s.compare("hXYZello") == 0);
    }
}

//=============================================================================

VX_TEST_CASE(test_clear)
{
    vx::string s("abcdef");
    VX_CHECK(s.size() == 6);

    s.clear();

    VX_CHECK(s.size() == 0);
    VX_CHECK(s.capacity() >= 1);
    VX_CHECK(s.data()[0] == '\0');
}

//=============================================================================

VX_TEST_CASE(test_clear_and_deallocate)
{
    vx::string s("abcdef");
    s.clear_and_deallocate();

    VX_CHECK(s.size() == 0);
    VX_CHECK(s.capacity() == 0);
    VX_CHECK(s.data()[0] == '\0');
}

//=============================================================================

VX_TEST_CASE(test_shrink_to_fit)
{
    VX_SECTION("already exact")
    {
        vx::string s("abcdef");
        s.reserve(s.size());

        const auto old_cap = s.capacity();
        const bool result = s.shrink_to_fit();

        VX_CHECK(result);
        VX_CHECK(s.capacity() == old_cap);
        VX_CHECK(s.size() == 6);
    }

    VX_SECTION("shrink")
    {
        vx::string s("abcdef");
        s.reserve(100);

        VX_CHECK(s.capacity() >= 100);

        const bool result = s.shrink_to_fit();

        VX_CHECK(result);
        VX_CHECK(s.capacity() == s.size()); // exact fit
    }
}

//=============================================================================

VX_TEST_CASE(test_release)
{
    vx::string s("hello");

    const size_t size = s.size();
    char* raw = s.release();

    VX_CHECK(raw != nullptr);
    VX_CHECK(vx::str::compare(raw, "hello") == 0);
    vx::mem::deallocate(raw, size);
}

//=============================================================================

VX_TEST_CASE(test_acquire)
{
    using allocator = typename vx::string::allocator_type;

    // manually allocated C-string
    char* raw = allocator{}.allocate(6);
    vx::mem::copy(raw, "abcde", 6);

    vx::string s;

    const bool ok = s.acquire(raw);

    VX_CHECK(ok);
    VX_CHECK(s.size() == 5);
    VX_CHECK(s.capacity() == 5);
    VX_CHECK(vx::str::compare(s.data(), "abcde") == 0);
}

//=============================================================================

VX_TEST_CASE(test_swap)
{
    vx::string a("abc");
    vx::string b("xyz");

    a.swap(b);

    VX_CHECK(a == "xyz");
    VX_CHECK(b == "abc");
}

//=============================================================================

VX_TEST_CASE(test_empty)
{
    vx::string s;
    VX_CHECK(s.empty());

    s = "x";
    VX_CHECK(!s.empty());

    s.clear();
    VX_CHECK(s.empty());
}

//=============================================================================

VX_TEST_CASE(test_size)
{
    vx::string s("abcd");
    VX_CHECK(s.size() == 4);

    s.push_back('e');
    VX_CHECK(s.size() == 5);

    s.clear();
    VX_CHECK(s.size() == 0);
}

//=============================================================================

VX_TEST_CASE(test_length)
{
    vx::string s("hello");
    VX_CHECK(s.length() == 5);

    s.pop_back();
    VX_CHECK(s.length() == 4);
}

//=============================================================================

VX_TEST_CASE(test_data_size)
{
    vx::string s("abcd");

    VX_CHECK(s.data_size() == 4 * sizeof(char));

    s.push_back('x');
    VX_CHECK(s.data_size() == 5 * sizeof(char));

    s.clear();
    VX_CHECK(s.data_size() == 0);
}

//=============================================================================

VX_TEST_CASE(test_max_size)
{
    vx::string s;

    const auto max = s.max_size();
    VX_CHECK(max > 0);

    VX_CHECK(max == vx::mem::max_array_size<char>() - 1);
}

//=============================================================================

VX_TEST_CASE(test_capacity)
{
    vx::string s("abc");
    const auto initial_cap = s.capacity();

    s.reserve(100);
    VX_CHECK(s.capacity() >= 100);
    VX_CHECK(s.capacity() >= initial_cap);

    s.clear();
    VX_CHECK(s.capacity() >= 1);
}

//=============================================================================

VX_TEST_CASE(test_reserve)
{
    VX_SECTION("shrink")
    {
        vx::string s("abcd");
        const auto cap_before = s.capacity();

        const bool ok1 = s.reserve(cap_before - 1);
        VX_CHECK(ok1);
        VX_CHECK(s.capacity() == cap_before);

        const bool ok2 = s.reserve(cap_before);
        VX_CHECK(ok2);
        VX_CHECK(s.capacity() == cap_before);
    }

    VX_SECTION("grow")
    {
        vx::string s("hello");
        const auto old_ptr = s.data();
        const auto old_cap = s.capacity();
        const auto old_size = s.size();

        const bool ok = s.reserve(old_cap + 50);
        VX_CHECK(ok);

        VX_CHECK(s.capacity() >= old_cap + 50);
        VX_CHECK(s.size() == old_size);
        VX_CHECK(s == "hello");

        VX_CHECK(s.data() != old_ptr);
    }

    VX_SECTION("max_size")
    {
        vx::wstring s(L"abc");
        VX_CHECK_AND_EXPECT_ERROR_CODE(
            !s.reserve(s.max_size() + 1),
            vx::err::size_error
        );
    }
}

//=============================================================================

VX_TEST_CASE(test_resize)
{
    VX_SECTION("shrink")
    {
        vx::string s("hello");

        const bool ok = s.resize(2);
        VX_CHECK(ok);

        VX_CHECK(s.size() == 2);
        VX_CHECK(s == "he");
    }

    VX_SECTION("grow")
    {
        vx::string s("a");

        const bool ok = s.resize(4, 'x');
        VX_CHECK(ok);

        VX_CHECK(s.size() == 4);
        VX_CHECK(s == "axxx");
    }
}

//=============================================================================

VX_TEST_CASE(test_push_back)
{
    VX_SECTION("within capacity")
    {
        vx::string s("ab");
        VX_CHECK(s.reserve(3));
        const auto old_cap = s.capacity();
        const auto old_size = s.size();

        const bool ok = s.push_back('X');
        VX_CHECK(ok);
        VX_CHECK(s.size() == old_size + 1);
        VX_CHECK(s == "abX");
        VX_CHECK(s.capacity() == old_cap);
    }

    VX_SECTION("growth")
    {
        vx::string s;
        s.reserve(1); // small capacity to force growth quickly

        s.push_back('a');
        s.push_back('b'); // should trigger growth

        VX_CHECK(s.size() == 2);
        VX_CHECK(s == "ab");
        VX_CHECK(s.capacity() >= s.size());
    }

    VX_SECTION("push_back multiple chars")
    {
        vx::string s("x");

        s.push_back('y');
        s.push_back('z');

        VX_CHECK(s.size() == 3);
        VX_CHECK(s == "xyz");
    }
}

//=============================================================================

VX_TEST_CASE(test_erase)
{
    VX_SECTION("offset and count")
    {
        vx::string s("abcdef");

        s.erase(2, 2); // remove 'c','d'
        VX_CHECK(s.size() == 4);
        VX_CHECK(s.compare("abef") == 0);
    }

    VX_SECTION("offset only")
    {
        vx::string s("abcdef");

        s.erase(3); // remove 'd','e','f'
        VX_CHECK(s.size() == 3);
        VX_CHECK(s.compare("abc") == 0);
    }

    VX_SECTION("invalid offset")
    {
        vx::string s("abc");

        s.erase(99, 5); // invalid offset
        VX_CHECK(s.size() == 3);
        VX_CHECK(s.compare("abc") == 0);
    }

    VX_SECTION("full string")
    {
        vx::string s("xyz");

        s.erase();
        VX_CHECK(s.size() == 0);
        VX_CHECK(s.empty());
    }

    VX_SECTION("iterator")
    {
        vx::string s("abc");

        auto it = s.erase(s.begin() + 1); // remove 'b'
        VX_CHECK(s.size() == 2);
        VX_CHECK(s.compare("ac") == 0);
        VX_CHECK(*it == 'c'); // iterator points to next element
    }

    VX_SECTION("iterator range")
    {
        vx::string s("abcdef");

        auto it = s.erase(s.begin() + 1, s.begin() + 4); // remove 'b','c','d'
        VX_CHECK(s.size() == 3);
        VX_CHECK(s.compare("aef") == 0);
        VX_CHECK(*it == 'e'); // iterator points to next element
    }
}

//=============================================================================

VX_TEST_CASE(test_pop_back)
{
    vx::string s("abc");

    s.pop_back();
    VX_CHECK(s.size() == 2);
    VX_CHECK(s.compare("ab") == 0);

    s.pop_back();
    VX_CHECK(s.size() == 1);
    VX_CHECK(s.compare("a") == 0);

    s.pop_back();
    VX_CHECK(s.size() == 0);
    VX_CHECK(s.empty());

    s.pop_back(); // should be no-op
    VX_CHECK(s.size() == 0);
    VX_CHECK(s.empty());
}

//=============================================================================

VX_TEST_CASE(test_copy)
{
    VX_SECTION("full")
    {
        vx::string s("hello");
        char buffer[10] = {};

        const auto n = s.copy(buffer, 5);
        VX_CHECK(n == 5);
        VX_CHECK(std::strncmp(buffer, "hello", 5) == 0);
    }

    VX_SECTION("partial")
    {
        vx::string s("abcdef");
        char buffer[10] = {};

        const auto n = s.copy(buffer, 3, 2); // copy 'c','d','e'
        VX_CHECK(n == 3);
        VX_CHECK(std::strncmp(buffer, "cde", 3) == 0);
    }

    VX_SECTION("offset beyond size")
    {
        vx::string s("abc");
        char buffer[10] = {};

        const auto n = s.copy(buffer, 5, 99);
        VX_CHECK(n == 0);
        VX_CHECK(buffer[0] == '\0');
    }
}

VX_TEST_CASE(test_substr)
{
    VX_SECTION("full")
    {
        vx::string s("hello");
        vx::string sub = s.substr();

        VX_CHECK(sub == "hello");
        VX_CHECK(sub.size() == s.size());
    }

    VX_SECTION("partial")
    {
        vx::string s("abcdef");
        vx::string sub = s.substr(2, 3); // "cde"

        VX_CHECK(sub == "cde");
        VX_CHECK(sub.size() == 3);
    }

    VX_SECTION("offset beyond size")
    {
        vx::string s("abc");
        vx::string sub = s.substr(99, 5);

        VX_CHECK(sub.empty());
        VX_CHECK(sub.size() == 0);
    }

    VX_SECTION("count beyond end")
    {
        vx::string s("abcde");
        vx::string sub = s.substr(3, 99); // from 'd' to end

        VX_CHECK(sub == "de");
        VX_CHECK(sub.size() == 2);
    }
}

//=============================================================================

VX_TEST_CASE(test_replace)
{
    VX_SECTION("string")
    {
        vx::string s("abcdef");
        vx::string r("XYZ");

        s.replace(2, 3, r); // replace 'c','d','e' with 'XYZ'
        VX_CHECK(s.compare("abXYZf") == 0);
    }

    VX_SECTION("string and offsets")
    {
        vx::string s("abcdef");
        vx::string r("12345");

        s.replace(1, 3, r, 1, 2); // replace 'b','c','d' with '23'
        VX_CHECK(s.compare("a23ef") == 0);
    }

    VX_SECTION("single char count")
    {
        vx::string s("abcdef");

        s.replace(1, 3, 2, 'X'); // replace 'b','c','d' with 'XX'
        VX_CHECK(s.compare("aXXef") == 0);
    }

    VX_SECTION("pointer C-string")
    {
        vx::string s("abcdef");
        const char* p = "123";

        s.replace(2, 3, p); // replace 'c','d','e' with '123'
        VX_CHECK(s.compare("ab123f") == 0);
    }

    VX_SECTION("pointer + count")
    {
        vx::string s("abcdef");
        const char* p = "98765";

        s.replace(1, 3, p, 2); // replace 'b','c','d' with '98'
        VX_CHECK(s.compare("a98ef") == 0);
    }

    VX_SECTION("initializer_list")
    {
        vx::string s("abcdef");

        s.replace(1, 3, { 'X', 'Y', 'Z' });
        VX_CHECK(s.compare("aXYZef") == 0);
    }

    VX_SECTION("iterator range (pointer path)")
    {
        vx::string s1("abcdef");
        vx::string s2("123");

        s1.replace(2, 3, s2.begin(), s2.end());
        VX_CHECK(s1.compare("ab123f") == 0);
    }

    VX_SECTION("iterator range (non-pointer path)")
    {
        vx::string s("abcdef");
        char arr[] { 'x', 'y' };

        s.replace(1, 3, std::begin(arr), std::end(arr));
        VX_CHECK(s.compare("axyef") == 0);
    }

    VX_SECTION("string_view")
    {
        vx::string s("abcdef");
        vx::string_view v("XYZ");

        s.replace(2, 3, v);
        VX_CHECK(s.compare("abXYZf") == 0);
    }

    VX_SECTION("string_view and offsets")
    {
        vx::string s("abcdef");
        vx::string_view v("12345");

        s.replace(1, 3, v, 1, 2); // replace 'b','c','d' with '23'
        VX_CHECK(s.compare("a23ef") == 0);
    }

    VX_SECTION("offset beyond size")
    {
        vx::string s("abc");
        vx::string r("XYZ");

        s.replace(99, 2, r);
        VX_CHECK(s.compare("abc") == 0);
    }
}

//=============================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
