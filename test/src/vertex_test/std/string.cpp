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

int main()
{
    VX_RUN_TESTS();
    return 0;
}
