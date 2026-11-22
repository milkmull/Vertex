// Tests for vertex/std/algorithm/minmax.hpp
//
// Assumptions (adjust the includes to match your tree):
//   - the algorithm header is "vertex/std/algorithm/minmax.hpp"
//   - the runner header is   "vertex_test/test_runner.hpp"
//   - define VX_TEST_NO_MAIN if a shared runner provides main()

#include "vertex/std/algorithm/minmax.hpp"

#include <algorithm>
#include <cstddef>
#include <list>
#include <vector>

#include "vertex_test/test.hpp"
#include "vertex_test/std/algorithm/algorithm_tools.hpp"

namespace vx {
namespace test {

namespace {

// constexpr data
constexpr int k_values[] = { 3, 1, 4, 1, 5, 9, 2, 6, 5 }; // min first @1, max @5
constexpr int k_ties[] = { 1, 5, 3, 5 };                  // max first @1, last @3

} // namespace

//=============================================================================
// scalar: min
//=============================================================================

VX_TEST_CASE(minmax_scalar_min)
{
    VX_SECTION("basic");
    {
        VX_CHECK(vx::min(1, 2) == 1);
        VX_CHECK(vx::min(2, 1) == 1);
        VX_CHECK(vx::min(-5, 5) == -5);
        VX_CHECK(vx::min(7, 7) == 7);
    }

    VX_SECTION("returns a reference to the argument");
    {
        int a = 1;
        int b = 2;
        VX_CHECK(&vx::min(a, b) == &a);
        VX_CHECK(&vx::min(b, a) == &a);
    }

    VX_SECTION("equivalent values return the first argument");
    {
        int a = 5;
        int b = 5;
        VX_CHECK(&vx::min(a, b) == &a);
        VX_CHECK(&vx::min(b, a) == &b);
    }

    VX_SECTION("custom comparator");
    {
        // with greater, the "smallest" is the numerically largest
        VX_CHECK(vx::min(1, 2, greater_int{}) == 2);
        VX_CHECK(vx::min(2, 1, greater_int{}) == 2);

        keyed_item a{ 1, 0 };
        keyed_item b{ 1, 1 };
        VX_CHECK(&vx::min(a, b, by_key{}) == &a); // tie -> first
        VX_CHECK(&vx::min(b, a, by_key{}) == &b);
    }

    VX_SECTION("movable_int / copyable_int / trivial_int");
    {
        movable_int m1(1);
        movable_int m2(2);
        VX_CHECK(&vx::min(m1, m2) == &m1);
        VX_CHECK(m1.val == 1 && m2.val == 2); // nothing moved from

        copyable_int c1(4);
        copyable_int c2(3);
        VX_CHECK(&vx::min(c1, c2) == &c2);
        VX_CHECK(c1.val == 4 && c2.val == 3);

        trivial_int t1{ 8 };
        trivial_int t2{ 9 };
        VX_CHECK(&vx::min(t1, t2) == &t1);
    }
}

//=============================================================================
// scalar: max
//=============================================================================

VX_TEST_CASE(minmax_scalar_max)
{
    VX_SECTION("basic");
    {
        VX_CHECK(vx::max(1, 2) == 2);
        VX_CHECK(vx::max(2, 1) == 2);
        VX_CHECK(vx::max(-5, 5) == 5);
        VX_CHECK(vx::max(7, 7) == 7);
    }

    VX_SECTION("returns a reference to the argument");
    {
        int a = 1;
        int b = 2;
        VX_CHECK(&vx::max(a, b) == &b);
        VX_CHECK(&vx::max(b, a) == &b);
    }

    VX_SECTION("equivalent values return the first argument");
    {
        int a = 5;
        int b = 5;
        VX_CHECK(&vx::max(a, b) == &a);
        VX_CHECK(&vx::max(b, a) == &b);
    }

    VX_SECTION("custom comparator");
    {
        VX_CHECK(vx::max(1, 2, greater_int{}) == 1);
        VX_CHECK(vx::max(2, 1, greater_int{}) == 1);

        keyed_item a{ 1, 0 };
        keyed_item b{ 1, 1 };
        VX_CHECK(&vx::max(a, b, by_key{}) == &a); // tie -> first
        VX_CHECK(&vx::max(b, a, by_key{}) == &b);
    }

    VX_SECTION("movable_int / copyable_int / trivial_int");
    {
        movable_int m1(1);
        movable_int m2(2);
        VX_CHECK(&vx::max(m1, m2) == &m2);
        VX_CHECK(m1.val == 1 && m2.val == 2);

        copyable_int c1(4);
        copyable_int c2(3);
        VX_CHECK(&vx::max(c1, c2) == &c1);
        VX_CHECK(c1.val == 4 && c2.val == 3);

        trivial_int t1{ 8 };
        trivial_int t2{ 9 };
        VX_CHECK(&vx::max(t1, t2) == &t2);
    }
}

//=============================================================================
// scalar: minmax
//=============================================================================

VX_TEST_CASE(minmax_scalar_minmax)
{
    VX_SECTION("basic");
    {
        auto r1 = vx::minmax(1, 2);
        VX_CHECK(r1.min == 1 && r1.max == 2);

        auto r2 = vx::minmax(2, 1);
        VX_CHECK(r2.min == 1 && r2.max == 2);
    }

    VX_SECTION("returns references to the arguments");
    {
        int a = 1;
        int b = 2;

        auto r1 = vx::minmax(a, b);
        VX_CHECK(&r1.min == &a && &r1.max == &b);

        auto r2 = vx::minmax(b, a);
        VX_CHECK(&r2.min == &a && &r2.max == &b);
    }

    VX_SECTION("equivalent values keep argument order");
    {
        int a = 5;
        int b = 5;

        auto r1 = vx::minmax(a, b);
        VX_CHECK(&r1.min == &a && &r1.max == &b);

        auto r2 = vx::minmax(b, a);
        VX_CHECK(&r2.min == &b && &r2.max == &a);
    }

    VX_SECTION("custom comparator");
    {
        auto r = vx::minmax(1, 2, greater_int{});
        VX_CHECK(r.min == 2 && r.max == 1);

        keyed_item a{ 1, 0 };
        keyed_item b{ 1, 1 };
        auto t = vx::minmax(a, b, by_key{});
        VX_CHECK(&t.min == &a && &t.max == &b);
    }

    VX_SECTION("movable_int");
    {
        movable_int m1(3);
        movable_int m2(1);
        auto r = vx::minmax(m1, m2);
        VX_CHECK(&r.min == &m2 && &r.max == &m1);
        VX_CHECK(m1.val == 3 && m2.val == 1);
    }
}

//=============================================================================
// scalar: clamp
//=============================================================================

VX_TEST_CASE(minmax_scalar_clamp)
{
    VX_SECTION("basic");
    {
        VX_CHECK(vx::clamp(5, 0, 10) == 5);
        VX_CHECK(vx::clamp(-1, 0, 10) == 0);
        VX_CHECK(vx::clamp(11, 0, 10) == 10);
    }

    VX_SECTION("boundaries");
    {
        VX_CHECK(vx::clamp(0, 0, 10) == 0);
        VX_CHECK(vx::clamp(10, 0, 10) == 10);
        VX_CHECK(vx::clamp(3, 3, 3) == 3);
        VX_CHECK(vx::clamp(0, 3, 3) == 3);
        VX_CHECK(vx::clamp(9, 3, 3) == 3);
    }

    VX_SECTION("returns a reference to the selected argument");
    {
        int low = 0;
        int high = 10;

        int below = -4;
        int inside = 5;
        int above = 20;

        VX_CHECK(&vx::clamp(below, low, high) == &low);
        VX_CHECK(&vx::clamp(inside, low, high) == &inside);
        VX_CHECK(&vx::clamp(above, low, high) == &high);
    }

    VX_SECTION("equivalent value returns the value, not the bound");
    {
        keyed_item low{ 0, 0 };
        keyed_item high{ 10, 1 };
        keyed_item at_low{ 0, 2 };
        keyed_item at_high{ 10, 3 };

        VX_CHECK(&vx::clamp(at_low, low, high, by_key{}) == &at_low);
        VX_CHECK(&vx::clamp(at_high, low, high, by_key{}) == &at_high);
    }

    VX_SECTION("custom comparator");
    {
        // with greater, [low, high] = [10, 0] describes the same interval
        VX_CHECK(vx::clamp(5, 10, 0, greater_int{}) == 5);
        VX_CHECK(vx::clamp(15, 10, 0, greater_int{}) == 10);
        VX_CHECK(vx::clamp(-5, 10, 0, greater_int{}) == 0);
    }

    VX_SECTION("movable_int");
    {
        movable_int low(0);
        movable_int high(10);
        movable_int v(20);

        VX_CHECK(&vx::clamp(v, low, high) == &high);
        VX_CHECK(low.val == 0 && high.val == 10 && v.val == 20);
    }
}

//=============================================================================
// scalar: constexpr / noexcept
//=============================================================================

VX_TEST_CASE(minmax_scalar_constexpr)
{
    VX_STATIC_CHECK(vx::min(1, 2) == 1);
    VX_STATIC_CHECK(vx::max(1, 2) == 2);
    VX_STATIC_CHECK(vx::minmax(3, 1).min == 1);
    VX_STATIC_CHECK(vx::minmax(3, 1).max == 3);
    VX_STATIC_CHECK(vx::clamp(15, 0, 10) == 10);
    VX_STATIC_CHECK(vx::clamp(-1, 0, 10) == 0);
    VX_STATIC_CHECK(vx::clamp(4, 0, 10) == 4);

    VX_STATIC_CHECK(vx::min(1, 2, greater_int{}) == 2);
    VX_STATIC_CHECK(vx::max(1, 2, greater_int{}) == 1);

    VX_STATIC_CHECK(noexcept(vx::min(1, 2)));
    VX_STATIC_CHECK(noexcept(vx::max(1, 2)));
    VX_STATIC_CHECK(noexcept(vx::minmax(1, 2)));
    VX_STATIC_CHECK(noexcept(vx::clamp(1, 0, 2)));
}

//=============================================================================
// range: min
//=============================================================================

VX_TEST_CASE(minmax_range_min)
{
    VX_SECTION("empty range returns last");
    {
        std::vector<int> v;
        VX_CHECK(vx::range::min(v.begin(), v.end()) == v.end());
        VX_CHECK(vx::range::min(v.begin(), v.end(), greater_int{}) == v.end());

        int arr[1] = { 0 };
        VX_CHECK(vx::range::min(arr, arr) == arr);
    }

    VX_SECTION("single element");
    {
        std::vector<int> v = { 42 };
        VX_CHECK(vx::range::min(v.begin(), v.end()) == v.begin());
    }

    VX_SECTION("typical");
    {
        std::vector<int> v(std::begin(k_values), std::end(k_values));
        auto it = vx::range::min(v.begin(), v.end());
        VX_CHECK(it == v.begin() + 1);
        VX_CHECK(*it == 1);
    }

    VX_SECTION("returns the first smallest on ties");
    {
        std::vector<int> v = { 2, 1, 3, 1 };
        VX_CHECK(vx::range::min(v.begin(), v.end()) == v.begin() + 1);

        std::vector<keyed_item> items = {
            { 2, 0 },
            { 1, 1 },
            { 3, 2 },
            { 1, 3 }
        };
        auto it = vx::range::min(items.begin(), items.end(), by_key{});
        VX_CHECK(it == items.begin() + 1);
        VX_CHECK(it->id == 1);
    }

    VX_SECTION("sorted / reversed / all equal");
    {
        std::vector<int> asc = { 1, 2, 3, 4, 5 };
        std::vector<int> desc = { 5, 4, 3, 2, 1 };
        std::vector<int> same = { 7, 7, 7, 7 };

        VX_CHECK(vx::range::min(asc.begin(), asc.end()) == asc.begin());
        VX_CHECK(vx::range::min(desc.begin(), desc.end()) == desc.end() - 1);
        VX_CHECK(vx::range::min(same.begin(), same.end()) == same.begin());
    }

    VX_SECTION("custom comparator");
    {
        std::vector<int> v = { 3, 1, 4, 1, 5, 9, 2, 6 };
        auto it = vx::range::min(v.begin(), v.end(), greater_int{});
        VX_CHECK(it == v.begin() + 5);
        VX_CHECK(*it == 9);
    }

    VX_SECTION("non-random-access iterators");
    {
        std::list<int> l = { 4, 2, 8, 2 };
        auto it = vx::range::min(l.begin(), l.end());
        VX_CHECK(std::distance(l.begin(), it) == 1);

        std::list<int> empty;
        VX_CHECK(vx::range::min(empty.begin(), empty.end()) == empty.end());
    }

    VX_SECTION("sub-range");
    {
        std::vector<int> v = { 0, 5, 3, 4, 0 };
        auto it = vx::range::min(v.begin() + 1, v.end() - 1);
        VX_CHECK(it == v.begin() + 2);
    }

    VX_SECTION("comparison count");
    {
        for (std::size_t n = 1; n <= 20; ++n)
        {
            std::vector<int> v(n);
            for (std::size_t i = 0; i < n; ++i)
            {
                v[i] = static_cast<int>((i * 7) % 5);
            }

            std::size_t count = 0;
            vx::range::min(v.begin(), v.end(), counting_less{ &count });
            VX_CHECK(count <= n);
        }
    }

    VX_SECTION("movable_int / copyable_int / trivial_int");
    {
        std::vector<movable_int> m;
        m.reserve(4);
        for (int x : { 3, 1, 4, 1 })
        {
            m.emplace_back(x);
        }
        VX_CHECK(vx::range::min(m.begin(), m.end()) == m.begin() + 1);
        VX_CHECK(m[0].val == 3 && m[1].val == 1 && m[2].val == 4 && m[3].val == 1);

        std::vector<copyable_int> c = { 3, 1, 4, 1 };
        VX_CHECK(vx::range::min(c.begin(), c.end()) == c.begin() + 1);
        VX_CHECK(c[0].val == 3 && c[3].val == 1);

        trivial_int t[] = { { 3 }, { 1 }, { 4 }, { 1 } };
        VX_CHECK(vx::range::min(t, t + 4) == t + 1);
    }
}

//=============================================================================
// range: max
//=============================================================================

VX_TEST_CASE(minmax_range_max)
{
    VX_SECTION("empty range returns last");
    {
        std::vector<int> v;
        VX_CHECK(vx::range::max(v.begin(), v.end()) == v.end());
        VX_CHECK(vx::range::max(v.begin(), v.end(), greater_int{}) == v.end());

        int arr[1] = { 0 };
        VX_CHECK(vx::range::max(arr, arr) == arr);
    }

    VX_SECTION("single element");
    {
        std::vector<int> v = { 42 };
        VX_CHECK(vx::range::max(v.begin(), v.end()) == v.begin());
    }

    VX_SECTION("typical");
    {
        std::vector<int> v(std::begin(k_values), std::end(k_values));
        auto it = vx::range::max(v.begin(), v.end());
        VX_CHECK(it == v.begin() + 5);
        VX_CHECK(*it == 9);
    }

    VX_SECTION("returns the first largest on ties");
    {
        std::vector<int> v(std::begin(k_ties), std::end(k_ties));
        VX_CHECK(vx::range::max(v.begin(), v.end()) == v.begin() + 1);

        std::vector<keyed_item> items = {
            { 1, 0 },
            { 5, 1 },
            { 3, 2 },
            { 5, 3 }
        };
        auto it = vx::range::max(items.begin(), items.end(), by_key{});
        VX_CHECK(it == items.begin() + 1);
        VX_CHECK(it->id == 1);
    }

    VX_SECTION("sorted / reversed / all equal");
    {
        std::vector<int> asc = { 1, 2, 3, 4, 5 };
        std::vector<int> desc = { 5, 4, 3, 2, 1 };
        std::vector<int> same = { 7, 7, 7, 7 };

        VX_CHECK(vx::range::max(asc.begin(), asc.end()) == asc.end() - 1);
        VX_CHECK(vx::range::max(desc.begin(), desc.end()) == desc.begin());
        VX_CHECK(vx::range::max(same.begin(), same.end()) == same.begin());
    }

    VX_SECTION("custom comparator");
    {
        std::vector<int> v = { 3, 1, 4, 1, 5, 9, 2, 6 };
        auto it = vx::range::max(v.begin(), v.end(), greater_int{});
        VX_CHECK(it == v.begin() + 1);
        VX_CHECK(*it == 1);
    }

    VX_SECTION("non-random-access iterators");
    {
        std::list<int> l = { 4, 8, 2, 8 };
        auto it = vx::range::max(l.begin(), l.end());
        VX_CHECK(std::distance(l.begin(), it) == 1);

        std::list<int> empty;
        VX_CHECK(vx::range::max(empty.begin(), empty.end()) == empty.end());
    }

    VX_SECTION("sub-range");
    {
        std::vector<int> v = { 99, 5, 3, 4, 99 };
        auto it = vx::range::max(v.begin() + 1, v.end() - 1);
        VX_CHECK(it == v.begin() + 1);
    }

    VX_SECTION("movable_int / copyable_int / trivial_int");
    {
        std::vector<movable_int> m;
        m.reserve(4);
        for (int x : { 3, 9, 4, 9 })
        {
            m.emplace_back(x);
        }
        VX_CHECK(vx::range::max(m.begin(), m.end()) == m.begin() + 1);
        VX_CHECK(m[0].val == 3 && m[1].val == 9 && m[2].val == 4 && m[3].val == 9);

        std::vector<copyable_int> c = { 3, 9, 4, 9 };
        VX_CHECK(vx::range::max(c.begin(), c.end()) == c.begin() + 1);
        VX_CHECK(c[0].val == 3 && c[3].val == 9);

        trivial_int t[] = { { 3 }, { 9 }, { 4 }, { 9 } };
        VX_CHECK(vx::range::max(t, t + 4) == t + 1);
    }
}

//=============================================================================
// range: minmax
//=============================================================================

VX_TEST_CASE(minmax_range_minmax)
{
    VX_SECTION("empty range returns {last, last}");
    {
        std::vector<int> v;
        auto r = vx::range::minmax(v.begin(), v.end());
        VX_CHECK(r.min == v.end() && r.max == v.end());

        auto rc = vx::range::minmax(v.begin(), v.end(), greater_int{});
        VX_CHECK(rc.min == v.end() && rc.max == v.end());
    }

    VX_SECTION("single element");
    {
        std::vector<int> v = { 42 };
        auto r = vx::range::minmax(v.begin(), v.end());
        VX_CHECK(r.min == v.begin() && r.max == v.begin());
    }

    VX_SECTION("two elements");
    {
        std::vector<int> asc = { 1, 2 };
        auto r1 = vx::range::minmax(asc.begin(), asc.end());
        VX_CHECK(r1.min == asc.begin() && r1.max == asc.begin() + 1);

        std::vector<int> desc = { 2, 1 };
        auto r2 = vx::range::minmax(desc.begin(), desc.end());
        VX_CHECK(r2.min == desc.begin() + 1 && r2.max == desc.begin());

        // tie: first is min, last is max
        std::vector<int> same = { 3, 3 };
        auto r3 = vx::range::minmax(same.begin(), same.end());
        VX_CHECK(r3.min == same.begin() && r3.max == same.begin() + 1);
    }

    VX_SECTION("typical, odd and even lengths");
    {
        std::vector<int> odd(std::begin(k_values), std::end(k_values)); // 9 elements
        auto r1 = vx::range::minmax(odd.begin(), odd.end());
        VX_CHECK(r1.min == odd.begin() + 1);
        VX_CHECK(r1.max == odd.begin() + 5);

        std::vector<int> even = { 3, 1, 4, 1, 5, 9, 2, 6 }; // 8 elements
        auto r2 = vx::range::minmax(even.begin(), even.end());
        VX_CHECK(r2.min == even.begin() + 1);
        VX_CHECK(r2.max == even.begin() + 5);
    }

    VX_SECTION("first smallest, last largest");
    {
        std::vector<keyed_item> items = {
            { 2, 0 },
            { 1, 1 },
            { 5, 2 },
            { 1, 3 },
            { 5, 4 },
            { 3, 5 }
        };
        auto r = vx::range::minmax(items.begin(), items.end(), by_key{});
        VX_CHECK(r.min->id == 1); // first of the smallest
        VX_CHECK(r.max->id == 4); // last of the largest

        std::vector<int> v(std::begin(k_ties), std::end(k_ties));
        auto r2 = vx::range::minmax(v.begin(), v.end());
        VX_CHECK(r2.max == v.begin() + 3);
    }

    VX_SECTION("all equal: min is first, max is last");
    {
        std::vector<int> same = { 7, 7, 7, 7, 7 };
        auto r = vx::range::minmax(same.begin(), same.end());
        VX_CHECK(r.min == same.begin());
        VX_CHECK(r.max == same.end() - 1);

        std::vector<int> even = { 7, 7, 7, 7 };
        auto r2 = vx::range::minmax(even.begin(), even.end());
        VX_CHECK(r2.min == even.begin());
        VX_CHECK(r2.max == even.end() - 1);
    }

    VX_SECTION("sorted / reversed");
    {
        std::vector<int> asc = { 1, 2, 3, 4, 5, 6 };
        auto r1 = vx::range::minmax(asc.begin(), asc.end());
        VX_CHECK(r1.min == asc.begin() && r1.max == asc.end() - 1);

        std::vector<int> desc = { 6, 5, 4, 3, 2, 1 };
        auto r2 = vx::range::minmax(desc.begin(), desc.end());
        VX_CHECK(r2.min == desc.end() - 1 && r2.max == desc.begin());
    }

    VX_SECTION("custom comparator");
    {
        std::vector<int> v = { 3, 1, 4, 1, 5, 9, 2, 6 };
        auto r = vx::range::minmax(v.begin(), v.end(), greater_int{});
        VX_CHECK(*r.min == 9);
        VX_CHECK(*r.max == 1);
        VX_CHECK(r.max == v.begin() + 3); // last "largest" under greater == last 1
    }

    VX_SECTION("non-random-access iterators");
    {
        std::list<int> l = { 4, 8, 2, 8, 2 };
        auto r = vx::range::minmax(l.begin(), l.end());
        VX_CHECK(std::distance(l.begin(), r.min) == 2);
        VX_CHECK(std::distance(l.begin(), r.max) == 3);
    }

    VX_SECTION("agrees with std::minmax_element (exhaustive small inputs)");
    {
        bool all_match = true;

        for_each_sequence(7, 3, [&](const std::vector<int>& v)
            {
            auto expected = std::minmax_element(v.begin(), v.end());
            auto actual = vx::range::minmax(v.begin(), v.end());
            if (actual.min != expected.first || actual.max != expected.second)
            {
                all_match = false;
            }

            auto expected_g = std::minmax_element(v.begin(), v.end(), greater_int{});
            auto actual_g = vx::range::minmax(v.begin(), v.end(), greater_int{});
            if (actual_g.min != expected_g.first || actual_g.max != expected_g.second)
            {
                all_match = false;
            }

            // consistency with the single-result versions
            if (vx::range::min(v.begin(), v.end()) != actual.min)
            {
                all_match = false;
            }
            if (!v.empty() && *vx::range::max(v.begin(), v.end()) != *actual.max)
            {
                all_match = false;
            } });

        VX_CHECK(all_match);
    }

    VX_SECTION("comparison count is about 1.5 per element");
    {
        for (std::size_t n = 2; n <= 33; ++n)
        {
            std::vector<int> v(n);
            for (std::size_t i = 0; i < n; ++i)
            {
                v[i] = static_cast<int>((i * 37) % 11);
            }

            std::size_t count = 0;
            vx::range::minmax(v.begin(), v.end(), counting_less{ &count });
            VX_CHECK(count <= 3 * (n - 1) / 2);
        }
    }

    VX_SECTION("movable_int / copyable_int / trivial_int");
    {
        std::vector<movable_int> m;
        m.reserve(5);
        for (int x : { 3, 1, 4, 1, 4 })
        {
            m.emplace_back(x);
        }
        auto r = vx::range::minmax(m.begin(), m.end());
        VX_CHECK(r.min == m.begin() + 1);
        VX_CHECK(r.max == m.begin() + 4);
        VX_CHECK(m[0].val == 3 && m[1].val == 1 && m[2].val == 4 && m[3].val == 1 && m[4].val == 4);

        std::vector<copyable_int> c = { 3, 1, 4, 1, 4 };
        auto rc = vx::range::minmax(c.begin(), c.end());
        VX_CHECK(rc.min == c.begin() + 1 && rc.max == c.begin() + 4);

        trivial_int t[] = { { 3 }, { 1 }, { 4 }, { 1 }, { 4 } };
        auto rt = vx::range::minmax(t, t + 5);
        VX_CHECK(rt.min == t + 1 && rt.max == t + 4);
    }
}

//=============================================================================
// range: pointer + count
//=============================================================================

VX_TEST_CASE(minmax_range_pointer_count)
{
    const std::size_t n = std::size(k_values);

    VX_SECTION("count of zero returns ptr");
    {
        int arr[1] = { 0 };
        VX_CHECK(vx::range::min(arr, std::size_t(0)) == arr);
        VX_CHECK(vx::range::max(arr, std::size_t(0)) == arr);

        auto r = vx::range::minmax(arr, std::size_t(0));
        VX_CHECK(r.min == arr && r.max == arr);

        VX_CHECK(vx::range::min(arr, std::size_t(0), greater_int{}) == arr);
        VX_CHECK(vx::range::max(arr, std::size_t(0), greater_int{}) == arr);
    }

    VX_SECTION("single element");
    {
        int arr[1] = { 5 };
        VX_CHECK(vx::range::min(arr, std::size_t(1)) == arr);
        VX_CHECK(vx::range::max(arr, std::size_t(1)) == arr);

        auto r = vx::range::minmax(arr, std::size_t(1));
        VX_CHECK(r.min == arr && r.max == arr);
    }

    VX_SECTION("typical");
    {
        VX_CHECK(vx::range::min(k_values, n) == k_values + 1);
        VX_CHECK(vx::range::max(k_values, n) == k_values + 5);

        auto r = vx::range::minmax(k_values, n);
        VX_CHECK(r.min == k_values + 1);
        VX_CHECK(r.max == k_values + 5);
    }

    VX_SECTION("ties");
    {
        VX_CHECK(vx::range::max(k_ties, std::size_t(4)) == k_ties + 1);

        auto r = vx::range::minmax(k_ties, std::size_t(4));
        VX_CHECK(r.min == k_ties);
        VX_CHECK(r.max == k_ties + 3);
    }

    VX_SECTION("custom comparator");
    {
        VX_CHECK(vx::range::min(k_values, n, greater_int{}) == k_values + 5);
        VX_CHECK(vx::range::max(k_values, n, greater_int{}) == k_values + 1);

        auto r = vx::range::minmax(k_values, n, greater_int{});
        VX_CHECK(r.min == k_values + 5);
        VX_CHECK(r.max == k_values + 3); // last 1
    }

    VX_SECTION("matches the iterator overloads");
    {
        for_each_sequence(6, 3, [&](const std::vector<int>& v)
            {
            const int* p = v.data();
            const std::size_t count = v.size();

            VX_SOFT_CHECK(vx::range::min(p, count) == vx::range::min(p, p + count));
            VX_SOFT_CHECK(vx::range::max(p, count) == vx::range::max(p, p + count));

            auto a = vx::range::minmax(p, count);
            auto b = vx::range::minmax(p, p + count);
            VX_SOFT_CHECK(a.min == b.min && a.max == b.max); });
    }

    VX_SECTION("trivial_int");
    {
        const trivial_int t[] = { { 3 }, { 1 }, { 4 }, { 1 }, { 4 } };
        VX_CHECK(vx::range::min(t, std::size_t(5)) == t + 1);
        VX_CHECK(vx::range::max(t, std::size_t(5)) == t + 2);

        auto r = vx::range::minmax(t, std::size_t(5));
        VX_CHECK(r.min == t + 1 && r.max == t + 4);
    }
}

//=============================================================================
// range: constexpr
//=============================================================================

VX_TEST_CASE(minmax_range_constexpr)
{
    constexpr std::size_t n = std::size(k_values);

    VX_STATIC_CHECK(vx::range::min(k_values, k_values + n) == k_values + 1);
    VX_STATIC_CHECK(vx::range::max(k_values, k_values + n) == k_values + 5);
    VX_STATIC_CHECK(vx::range::minmax(k_values, k_values + n).min == k_values + 1);
    VX_STATIC_CHECK(vx::range::minmax(k_values, k_values + n).max == k_values + 5);
    VX_STATIC_CHECK(vx::range::minmax(k_ties, k_ties + 4).max == k_ties + 3);

    VX_STATIC_CHECK(vx::range::min(k_values, n) == k_values + 1);
    VX_STATIC_CHECK(vx::range::max(k_values, n) == k_values + 5);
    VX_STATIC_CHECK(vx::range::minmax(k_values, n).min == k_values + 1);
    VX_STATIC_CHECK(vx::range::minmax(k_values, n).max == k_values + 5);

    VX_STATIC_CHECK(vx::range::min(k_values, k_values + n, greater_int{}) == k_values + 5);
    VX_STATIC_CHECK(vx::range::max(k_values, k_values + n, greater_int{}) == k_values + 1);

    // empty
    VX_STATIC_CHECK(vx::range::min(k_values, k_values) == k_values);
    VX_STATIC_CHECK(vx::range::minmax(k_values, k_values).min == k_values);
    VX_STATIC_CHECK(vx::range::minmax(k_values, k_values).max == k_values);
}

} // namespace test
} // namespace vx

#ifndef VX_TEST_NO_MAIN
int main(int argc, char** argv)
{
    return VX_RUN_TESTS(argc, argv);
}
#endif
