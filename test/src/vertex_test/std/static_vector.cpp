#include "vertex/std/static_vector.hpp"
#include "vertex_test/test.hpp"

// https://github.com/microsoft/STL/blob/e2ef398685f7e470dbaeaf65ff919de72bda7489/tests/tr1/tests/vector/test.cpp

//=========================================================================
// constructors
//=========================================================================

static void test_container()
{
    using vec = vx::static_vector<20, char>;

    vec v0;
    VX_CHECK(v0.empty());
    VX_CHECK(v0.size() == 0);

    vec v1(5);
    vec v1a(6, 'x');
    VX_CHECK(v1.size() == 5);
    VX_CHECK(v1a.size() == 6);
    VX_CHECK(v1a.back() == 'x');

    vec v2(v1a);
    VX_CHECK(v2.size() == 6);
    VX_CHECK(v2.front() == 'x');

    vec v3(v1a.begin(), v1a.end());
    VX_CHECK(v3.size() == 6);
    VX_CHECK(v3.front() == 'x');

    const vec v4(v1a.begin(), v1a.end());
    v0 = v4;
    VX_CHECK(v0.size() == 6);
    VX_CHECK(v0.front() == 'x');
    VX_CHECK(v0[0] == 'x');

    v0.reserve(12);
    VX_CHECK(12 <= v0.capacity());
    v0.resize(8);
    VX_CHECK(v0.size() == 8);
    v0.resize(10, 'z');
    VX_CHECK(v0.size() == 10);
    VX_CHECK(v0.back() == 'z');
    VX_CHECK(v0.size() <= v0.max_size());

    VX_SECTION("iterators")
    {
        typename vec::iterator p_it(v0.begin());
        typename vec::const_iterator p_cit(v4.begin());
        typename vec::reverse_iterator p_rit(v0.rbegin());
        typename vec::const_reverse_iterator p_crit(v4.rbegin());
        VX_CHECK(*p_it == 'x');
        VX_CHECK(*--(p_it = v0.end()) == 'z');
        VX_CHECK(*p_cit == 'x');
        VX_CHECK(*--(p_cit = v4.end()) == 'x');
        VX_CHECK(*p_rit == 'z');
        VX_CHECK(*--(p_rit = v0.rend()) == 'x');
        VX_CHECK(*p_crit == 'x');
        VX_CHECK(*--(p_crit = v4.rend()) == 'x');

        typename vec::const_iterator p_it1 = typename vec::const_iterator();
        typename vec::const_iterator p_it2 = typename vec::const_iterator();
        VX_CHECK(p_it1 == p_it2); // VX_CHECK null forward iterator comparisons
    }

    VX_SECTION("const iterators")
    {
        typename vec::const_iterator p_it(v0.cbegin());
        typename vec::const_iterator p_cit(v4.cbegin());
        typename vec::const_reverse_iterator p_rit(v0.crbegin());
        typename vec::const_reverse_iterator p_crit(v4.crbegin());
        VX_CHECK(*p_it == 'x');
        VX_CHECK(*--(p_it = v0.cend()) == 'z');
        VX_CHECK(*p_cit == 'x');
        VX_CHECK(*--(p_cit = v4.cend()) == 'x');
        VX_CHECK(*p_rit == 'z');
        VX_CHECK(*--(p_rit = v0.crend()) == 'x');
        VX_CHECK(*p_crit == 'x');
        VX_CHECK(*--(p_crit = v4.crend()) == 'x');

        typename vec::const_iterator p_it1 = typename vec::const_iterator();
        typename vec::const_iterator p_it2 = typename vec::const_iterator();
        VX_CHECK(p_it1 == p_it2); // VX_CHECK null forward iterator comparisons
    }

    VX_CHECK(v0.front() == 'x');
    VX_CHECK(v4.front() == 'x');

    v0.push_back('a');
    VX_CHECK(v0.back() == 'a');
    v0.pop_back();
    VX_CHECK(v0.back() == 'z');
    VX_CHECK(v4.back() == 'x');

    {
        vec v5;
        v5.resize(10, '\0');
        VX_CHECK(v5.size() == 10);
        VX_CHECK(v5[9] == 0);

        VX_DISABLE_USE_AFTER_MOVE_WARNING();

        vec v6(20, 'x');
        vec v7(std::move(v6));
        VX_CHECK(v7.size() == 20);

        vec v8;
        v8 = std::move(v7);
        VX_CHECK(v8.size() == 20);

        VX_DISABLE_WARNING_POP();

        vec v9;
        v9.resize(10, '\0');
        VX_CHECK(v9.size() == 10);
        VX_CHECK(v9[9] == 0);

        using movable_int = vx::test::movable_int;
        vx::static_vector<20, movable_int> v10;
        movable_int mi1(1);
        v10.push_back(std::move(mi1));
        VX_CHECK(mi1.val == -1);
        VX_CHECK(v10[0].val == 1);

        movable_int mi3(3);
        v10.insert(v10.begin(), std::move(mi3));
        VX_CHECK(mi3.val == -1);
        VX_CHECK(v10[0].val == 3);
        VX_CHECK(v10[1].val == 1);

        v10.emplace_back();
        VX_CHECK(v10.back().val == 0);
        v10.emplace_back(2);
        VX_CHECK(v10.back().val == 2);
        v10.emplace_back(3, 2);
        VX_CHECK(v10.back().val == 0x32);
        v10.emplace_back(4, 3, 2);
        VX_CHECK(v10.back().val == 0x432);
        v10.emplace_back(5, 4, 3, 2);
        VX_CHECK(v10.back().val == 0x5432);
        v10.emplace_back(6, 5, 4, 3, 2);
        VX_CHECK(v10.back().val == 0x65432);

        v10.emplace(v10.begin() + 1);
        VX_CHECK(v10[1].val == 0);
        v10.emplace(v10.begin() + 1, 2);
        VX_CHECK(v10[1].val == 2);
        v10.emplace(v10.begin() + 1, 3, 2);
        VX_CHECK(v10[1].val == 0x32);
        v10.emplace(v10.begin() + 1, 4, 3, 2);
        VX_CHECK(v10[1].val == 0x432);
        v10.emplace(v10.begin() + 1, 5, 4, 3, 2);
        VX_CHECK(v10[1].val == 0x5432);
        v10.emplace(v10.begin() + 1, 6, 5, 4, 3, 2);
        VX_CHECK(v10[1].val == 0x65432);
    }

    VX_SECTION("lvalue stealing")
    {
        using copyable_int = vx::test::copyable_int;
        vx::static_vector<20, copyable_int> v11;
        copyable_int ci1(1);
        v11.push_back(ci1);
        VX_CHECK(ci1.val == 1);
        VX_CHECK(v11[0].val == 1);

        copyable_int ci3(3);
        v11.insert(v11.begin(), ci3);
        VX_CHECK(ci3.val == 3);
        VX_CHECK(v11[0].val == 3);
        VX_CHECK(v11[1].val == 1);

        vx::static_vector<20, copyable_int> v12(v11);
        VX_CHECK(v11 == v12);
        v11 = v12;
        VX_CHECK(v11 == v12);
    }

    VX_SECTION("front/back")
    {
        char carr[] = "abc";

        typename vec::iterator p_it;
        v0.assign(v4.begin(), v4.end());
        VX_CHECK(v0.size() == v4.size());
        VX_CHECK(v0.front() == v4.front());
        v0.assign(4, 'w');
        VX_CHECK(v0.size() == 4);
        VX_CHECK(v0.front() == 'w');
        VX_CHECK(*v0.insert(v0.begin(), 'a') == 'a');
        VX_CHECK(v0.front() == 'a');
        VX_CHECK(*++(p_it = v0.begin()) == 'w');
        VX_CHECK(*v0.insert(v0.begin(), 2, 'b') == 'b');
        VX_CHECK(v0.front() == 'b');
        VX_CHECK(*++(p_it = v0.begin()) == 'b');
        VX_CHECK(*++ ++(p_it = v0.begin()) == 'a');
        VX_CHECK(*v0.insert(v0.end(), v4.begin(), v4.end()) == *v4.begin());
        VX_CHECK(v0.back() == v4.back());
        VX_CHECK(*v0.insert(v0.end(), carr, carr + 3) == *carr);
        VX_CHECK(v0.back() == 'c');
        v0.erase(v0.begin());
        VX_CHECK(v0.front() == 'b');
        VX_CHECK(*++(p_it = v0.begin()) == 'a');
        v0.erase(v0.begin(), ++(p_it = v0.begin()));
        VX_CHECK(v0.front() == 'a');
    }

    {
        vec v0x;

        v0x.push_back('a');
        VX_CHECK(*v0x.data() == 'a');

        v0x.shrink_to_fit();
        VX_CHECK(*v0x.data() == 'a');
    }

    {
        std::initializer_list<char> init{ 'a', 'b', 'c' };
        vec v11(init);
        VX_CHECK(v11.size() == 3);
        VX_CHECK(v11[2] == 'c');

        v11.clear();
        v11 = init;
        VX_CHECK(v11.size() == 3);
        VX_CHECK(v11[2] == 'c');

        v11.insert(v11.begin() + 1, init);
        VX_CHECK(v11.size() == 6);
        VX_CHECK(v11[2] == 'b');

        v11.assign(init);
        VX_CHECK(v11.size() == 3);
        VX_CHECK(v11[2] == 'c');
    }

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

VX_TEST_CASE(container)
{
    test_container();
}

//=========================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
