#include "vertex/std/string.hpp"
#include "vertex_test/test.hpp"

//=============================================================================

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
        const vx::string s1("hello");
        vx::string s(s1);

        VX_CHECK(s.is_valid());
        VX_CHECK(s.size() == s1.size());
        VX_CHECK(s.capacity() == s1.capacity());
        VX_CHECK(s.compare(s1) == 0);
    }

    VX_SECTION("move")
    {
        vx::string s1("hello");
        vx::string s(std::move(s1));

        VX_CHECK(!s1.is_valid());
        VX_CHECK(s.is_valid());
        VX_CHECK(s.size() == 5);
        VX_CHECK(s.capacity() == 5);
        VX_CHECK(s.compare("hello") == 0);
    }

    // substring copy (two-arg + three-arg)
    VX_SECTION("substring copy")
    {
        const vx::string src("hello");

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

    VX_SECTION("char-fill")
    {
        constexpr size_t count = 20;
        vx::string s(count, 'x');

        VX_CHECK(s.is_valid());
        VX_CHECK(s.size() == count);
        VX_CHECK(s.capacity() == count);

        for (const char c : s)
        {
            VX_CHECK(c == 'x');
        }
    }

    VX_SECTION("pointer")
    {
        const char* cs = "hello";

        {
            vx::string s(cs);
            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == vx::str::length(cs));
            VX_CHECK(s.capacity() == s.size());
            VX_CHECK(s.compare(cs) == 0);
        }

        {
            vx::string s(cs, 3);
            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == 3);
            VX_CHECK(s.capacity() == 3);
            VX_CHECK(s.compare("hel") == 0);
        }
    }

    VX_SECTION("iterator range")
    {
        const char* cs = "hello";

        // iterator range (pointer iterator path)
        {
            const vx::string s1(cs);
            vx::string s(s1.begin(), s1.end());

            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == 5);
            VX_CHECK(s.capacity() == 5);
            VX_CHECK(s.compare(cs) == 0);
        }

        // iterator range (non-pointer iterator path)
        {
            const char arr[] = { 'h', 'e', 'l', 'l', 'o' };
            vx::string s(std::begin(arr), std::end(arr));

            VX_CHECK(s.is_valid());
            VX_CHECK(s.size() == 5);
            VX_CHECK(s.capacity() == 5);
            VX_CHECK(s.compare(cs) == 0);
        }
    }

    VX_SECTION("list")
    {
        const char* cs = "hello";
        vx::string s = { 'h', 'e', 'l', 'l', 'o' };
        VX_CHECK(s.is_valid());
        VX_CHECK(s.size() == 5);
        VX_CHECK(s.capacity() == 5);
        VX_CHECK(s.compare(cs) == 0);
    }

    VX_SECTION("view")
    {
        const char* cs = "hello";

        vx::string_view v(cs);
        vx::string s(v);

        VX_CHECK(s.is_valid());
        VX_CHECK(s.size() == v.size());
        VX_CHECK(s.capacity() == v.size());
        VX_CHECK(s.compare(cs) == 0);
    }
}

//==============================================================================

VX_TEST_CASE(test_string_conversion_operators)
{
    const vx::string s("hello");

    // Test conversion to basic_string_view
    {
        vx::string_view sv = s;
        VX_CHECK(sv.size() == 5);
        VX_CHECK(sv == "hello");
    }

    // Test conversion to std::basic_string
    {
        std::string s1 = s;
        VX_CHECK(s1.size() == 5);
        VX_CHECK(s1 == "hello");
    }
}

//=============================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
