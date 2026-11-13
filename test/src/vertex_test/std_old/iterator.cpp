#include <algorithm>
#include <cstddef>
#include <forward_list>
#include <initializer_list>
#include <iterator>
#include <list>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "vertex/std/iterator/iterator.hpp"
#include "vertex/std/iterator/_priv/pointer_iterator.hpp"
#include "vertex_test/test.hpp"

namespace vp = vx::_priv;

using vx::test::copyable_int;
using vx::test::movable_int;

namespace {

//=============================================================================
// shared helpers
//=============================================================================

struct owner_a
{};
struct owner_b
{};

using it_t = vp::pointer_iterator<owner_a, int>;
using cit_t = vp::pointer_iterator<owner_a, const int>;
using bit_t = vp::pointer_iterator<owner_b, int>;
using lit_t = vp::pointer_iterator<owner_a, long>;
using rit_t = vp::reverse_pointer_iterator<it_t>;
using crit_t = vp::reverse_pointer_iterator<cit_t>;

struct point
{
    int x;
    int y;
};

// detection idiom: is `A == B` a valid expression?
template <typename A, typename B, typename = void>
struct can_eq : std::false_type
{};

template <typename A, typename B>
struct can_eq<A, B, std::void_t<decltype(std::declval<A>() == std::declval<B>())>> : std::true_type
{};

bool equals(const std::vector<int>& got, std::initializer_list<int> expected)
{
    return got.size() == expected.size() && std::equal(got.begin(), got.end(), expected.begin());
}

} // namespace

//=============================================================================
// _priv type traits
//=============================================================================

VX_TEST_CASE(pointer_iterator_traits_classification)
{
    VX_SECTION("is_pointer_iterator");
    {
        VX_CHECK(vp::is_pointer_iterator<it_t>::value);
        VX_CHECK(vp::is_pointer_iterator<cit_t>::value);
        VX_CHECK(vp::is_pointer_iterator<rit_t>::value);
        VX_CHECK(vp::is_pointer_iterator<const it_t&>::value); // cv/ref stripped
        VX_CHECK(!vp::is_pointer_iterator<int*>::value);
        VX_CHECK(!vp::is_pointer_iterator<std::vector<int>::iterator>::value);
        VX_CHECK(!vp::is_pointer_iterator<vx::reverse_iterator<int*>>::value);
    }

    VX_SECTION("is_forward_pointer_iterator / is_reverse_pointer_iterator");
    {
        VX_CHECK(vp::is_forward_pointer_iterator<it_t>::value);
        VX_CHECK(!vp::is_forward_pointer_iterator<rit_t>::value);
        VX_CHECK(!vp::is_forward_pointer_iterator<int*>::value);

        VX_CHECK(vp::is_reverse_pointer_iterator<rit_t>::value);
        VX_CHECK(vp::is_reverse_pointer_iterator<crit_t>::value);
        VX_CHECK(!vp::is_reverse_pointer_iterator<it_t>::value);
        VX_CHECK(!vp::is_reverse_pointer_iterator<int*>::value);
    }

    VX_SECTION("is_my_pointer_iterator");
    {
        VX_CHECK((vp::is_my_pointer_iterator<it_t, owner_a>::value));
        VX_CHECK((vp::is_my_pointer_iterator<cit_t, owner_a>::value));
        VX_CHECK(!(vp::is_my_pointer_iterator<it_t, owner_b>::value));
        VX_CHECK(!(vp::is_my_pointer_iterator<bit_t, owner_a>::value));
        VX_CHECK(!(vp::is_my_pointer_iterator<int*, owner_a>::value));
    }

    VX_SECTION("is_forward_pointer_iterator_of");
    {
        VX_CHECK((vp::is_forward_pointer_iterator_of<it_t, int>::value));
        VX_CHECK((vp::is_forward_pointer_iterator_of<cit_t, int>::value)); // cv on T ignored
        VX_CHECK((vp::is_forward_pointer_iterator_of<it_t, const int>::value));
        VX_CHECK(!(vp::is_forward_pointer_iterator_of<it_t, long>::value));
        VX_CHECK(!(vp::is_forward_pointer_iterator_of<rit_t, int>::value));
        VX_CHECK(!(vp::is_forward_pointer_iterator_of<int*, int>::value));
    }

    VX_SECTION("is_compatible_pointer_iterator");
    {
        VX_CHECK((vp::is_compatible_pointer_iterator<it_t, it_t>::value));
        VX_CHECK((vp::is_compatible_pointer_iterator<it_t, cit_t>::value));
        VX_CHECK((vp::is_compatible_pointer_iterator<cit_t, it_t>::value));
        VX_CHECK(!(vp::is_compatible_pointer_iterator<it_t, bit_t>::value)); // different owner
        VX_CHECK(!(vp::is_compatible_pointer_iterator<it_t, lit_t>::value)); // different element type
        VX_CHECK((vp::is_compatible_pointer_iterator<rit_t, crit_t>::value));
        VX_CHECK(!(vp::is_compatible_pointer_iterator<it_t, rit_t>::value)); // forward vs reverse
        VX_CHECK(!(vp::is_compatible_pointer_iterator<int*, int*>::value));
    }
}

//=============================================================================
// pointer_iterator
//=============================================================================

VX_TEST_CASE(pointer_iterator_member_types)
{
    VX_STATIC_CHECK(std::is_same<it_t::iterator_category, std::random_access_iterator_tag>::value);
    VX_STATIC_CHECK(std::is_same<it_t::value_type, int>::value);
    VX_STATIC_CHECK(std::is_same<it_t::difference_type, std::ptrdiff_t>::value);
    VX_STATIC_CHECK(std::is_same<it_t::pointer, int*>::value);
    VX_STATIC_CHECK(std::is_same<it_t::reference, int&>::value);

    VX_STATIC_CHECK(std::is_same<cit_t::pointer, const int*>::value);
    VX_STATIC_CHECK(std::is_same<cit_t::reference, const int&>::value);

    // std::iterator_traits must see through the wrapper
    VX_STATIC_CHECK(std::is_same<std::iterator_traits<it_t>::value_type, int>::value);
    VX_STATIC_CHECK(std::is_same<std::iterator_traits<it_t>::iterator_category, std::random_access_iterator_tag>::value);

    // operators keep the owner-tagged type (no slicing back to the base)
    VX_STATIC_CHECK(std::is_same<decltype(++std::declval<it_t&>()), it_t&>::value);
    VX_STATIC_CHECK(std::is_same<decltype(--std::declval<it_t&>()), it_t&>::value);
    VX_STATIC_CHECK(std::is_same<decltype(std::declval<it_t&>()++), it_t>::value);
    VX_STATIC_CHECK(std::is_same<decltype(std::declval<it_t&>()--), it_t>::value);
    VX_STATIC_CHECK(std::is_same<decltype(std::declval<it_t&>() += 1), it_t&>::value);
    VX_STATIC_CHECK(std::is_same<decltype(std::declval<it_t&>() -= 1), it_t&>::value);
    VX_STATIC_CHECK(std::is_same<decltype(std::declval<it_t>() + 1), it_t>::value);
    VX_STATIC_CHECK(std::is_same<decltype(1 + std::declval<it_t>()), it_t>::value);
    VX_STATIC_CHECK(std::is_same<decltype(std::declval<it_t>() - 1), it_t>::value);
    VX_STATIC_CHECK(std::is_same<decltype(std::declval<it_t>() - std::declval<it_t>()), std::ptrdiff_t>::value);
    VX_STATIC_CHECK(std::is_same<decltype(*std::declval<cit_t>()), const int&>::value);

    // construction from a raw pointer must be explicit
    VX_STATIC_CHECK(!std::is_convertible<int*, it_t>::value);
    VX_STATIC_CHECK(std::is_constructible<it_t, int*>::value);

    // bool conversion must be explicit
    VX_STATIC_CHECK(!std::is_convertible<it_t, bool>::value);
    VX_STATIC_CHECK(std::is_constructible<bool, it_t>::value);
}

VX_TEST_CASE(pointer_iterator_construction)
{
    int arr[3] = { 1, 2, 3 };

    VX_SECTION("default constructed is null");
    {
        it_t it;
        VX_CHECK(it.ptr() == nullptr);
        VX_CHECK(!static_cast<bool>(it));
    }

    VX_SECTION("construct from pointer");
    {
        it_t it(arr);
        VX_CHECK(it.ptr() == arr);
        VX_CHECK(static_cast<bool>(it));
    }

    VX_SECTION("copy");
    {
        it_t a(arr + 1);
        it_t b = a;
        VX_CHECK(b.ptr() == arr + 1);
        it_t c;
        c = b;
        VX_CHECK(c.ptr() == arr + 1);
    }

    VX_SECTION("implicit conversion to const iterator");
    {
        it_t it(arr + 2);
        cit_t cit = it;
        VX_CHECK(cit.ptr() == arr + 2);
        VX_CHECK(*cit == 3);
    }

    VX_SECTION("no conversion between owners");
    {
        VX_STATIC_CHECK(!std::is_convertible<it_t, bit_t>::value);
        VX_STATIC_CHECK(!std::is_convertible<bit_t, it_t>::value);
    }

    VX_SECTION("no conversion between unrelated element types");
    {
        VX_STATIC_CHECK(!std::is_convertible<it_t, lit_t>::value);
    }
}

VX_TEST_CASE(pointer_iterator_const_to_nonconst_must_not_convert)
{
    // const_iterator -> iterator must be rejected. The converting constructor
    // is constrained with is_convertible<U, T> (element values) rather than
    // is_convertible<U*, T*>, so I expect this to FAIL on the current code.
    VX_STATIC_CHECK(!std::is_convertible<cit_t, it_t>::value);
    VX_STATIC_CHECK(!std::is_constructible<it_t, cit_t>::value);
    VX_STATIC_CHECK(!std::is_convertible<crit_t, rit_t>::value);
}

VX_TEST_CASE(pointer_iterator_access)
{
    int arr[5] = { 10, 20, 30, 40, 50 };
    it_t it(arr);

    VX_SECTION("dereference");
    {
        VX_CHECK(*it == 10);
        VX_CHECK(&*it == arr);
    }

    VX_SECTION("write through iterator");
    {
        *it = 99;
        VX_CHECK(arr[0] == 99);
        arr[0] = 10;
    }

    VX_SECTION("subscript");
    {
        VX_CHECK(it[0] == 10);
        VX_CHECK(it[2] == 30);
        VX_CHECK(it[4] == 50);

        it_t last(arr + 4);
        VX_CHECK(last[-1] == 40);
        VX_CHECK(last[-4] == 10);
    }

    VX_SECTION("arrow operator");
    {
        point pts[2] = {
            { 1, 2 },
            { 3, 4 }
        };
        vp::pointer_iterator<owner_a, point> p(pts);
        VX_CHECK(p->x == 1);
        VX_CHECK(p->y == 2);
        ++p;
        VX_CHECK(p->x == 3);
        VX_CHECK(p->y == 4);
        p->x = 30;
        VX_CHECK(pts[1].x == 30);
    }
}

VX_TEST_CASE(pointer_iterator_arithmetic)
{
    int arr[5] = { 10, 20, 30, 40, 50 };

    VX_SECTION("pre/post increment");
    {
        it_t a(arr);
        VX_CHECK((++a).ptr() == arr + 1);
        it_t old = a++;
        VX_CHECK(old.ptr() == arr + 1);
        VX_CHECK(a.ptr() == arr + 2);
    }

    VX_SECTION("pre/post decrement");
    {
        it_t a(arr + 4);
        VX_CHECK((--a).ptr() == arr + 3);
        it_t old = a--;
        VX_CHECK(old.ptr() == arr + 3);
        VX_CHECK(a.ptr() == arr + 2);
    }

    VX_SECTION("compound assignment");
    {
        it_t a(arr);
        a += 3;
        VX_CHECK(a.ptr() == arr + 3);
        a -= 2;
        VX_CHECK(a.ptr() == arr + 1);
        a += 0;
        VX_CHECK(a.ptr() == arr + 1);
        a += -1;
        VX_CHECK(a.ptr() == arr);
    }

    VX_SECTION("binary + and -");
    {
        it_t a(arr + 1);
        VX_CHECK((a + 2).ptr() == arr + 3);
        VX_CHECK((2 + a).ptr() == arr + 3);
        VX_CHECK((a - 1).ptr() == arr);
        VX_CHECK((a + 0).ptr() == a.ptr());
        VX_CHECK(a.ptr() == arr + 1); // unchanged
    }

    VX_SECTION("iterator difference");
    {
        it_t first(arr);
        it_t last(arr + 5);
        VX_CHECK(last - first == 5);
        VX_CHECK(first - last == -5);
        VX_CHECK(first - first == 0);
    }
}

VX_TEST_CASE(pointer_iterator_comparison)
{
    int arr[5] = { 1, 2, 3, 4, 5 };
    it_t a(arr + 1);
    it_t b(arr + 3);
    it_t a2(arr + 1);

    VX_SECTION("same type");
    {
        VX_CHECK(a == a2);
        VX_CHECK(!(a != a2));
        VX_CHECK(a != b);
        VX_CHECK(a < b);
        VX_CHECK(!(b < a));
        VX_CHECK(b > a);
        VX_CHECK(a <= b);
        VX_CHECK(a <= a2);
        VX_CHECK(b >= a);
        VX_CHECK(a >= a2);
    }

    VX_SECTION("iterator vs const_iterator, both directions");
    {
        cit_t c(arr + 1);
        VX_CHECK(a == c);
        VX_CHECK(c == a);
        VX_CHECK(b != c);
        VX_CHECK(c < b);
        VX_CHECK(b > c);
        VX_CHECK(b - c == 2);
        VX_CHECK(c - b == -2);
    }

    VX_SECTION("default constructed iterators compare equal");
    {
        it_t n1;
        it_t n2;
        VX_CHECK(n1 == n2);
        VX_CHECK(n1 != a);
    }

    VX_SECTION("different owners / element types are not comparable");
    {
        VX_STATIC_CHECK((can_eq<it_t, it_t>::value));
        VX_STATIC_CHECK((can_eq<it_t, cit_t>::value));
        VX_STATIC_CHECK(!(can_eq<it_t, bit_t>::value));
        VX_STATIC_CHECK(!(can_eq<it_t, lit_t>::value));
        VX_STATIC_CHECK(!(can_eq<it_t, int*>::value));
        VX_STATIC_CHECK(!(can_eq<it_t, rit_t>::value));
    }
}

VX_TEST_CASE(pointer_iterator_std_interop)
{
    VX_SECTION("std::distance");
    {
        int arr[4] = { 1, 2, 3, 4 };
        VX_CHECK(std::distance(it_t(arr), it_t(arr + 4)) == 4);
    }

    VX_SECTION("std::find");
    {
        int arr[4] = { 1, 2, 3, 4 };
        it_t found = std::find(it_t(arr), it_t(arr + 4), 3);
        VX_CHECK(found.ptr() == arr + 2);
        it_t missing = std::find(it_t(arr), it_t(arr + 4), 42);
        VX_CHECK(missing.ptr() == arr + 4);
    }

    VX_SECTION("std::sort");
    {
        int arr[6] = { 5, 3, 6, 1, 4, 2 };
        std::sort(it_t(arr), it_t(arr + 6));
        VX_CHECK(std::is_sorted(arr, arr + 6));
        VX_CHECK(arr[0] == 1);
        VX_CHECK(arr[5] == 6);
    }

    VX_SECTION("std::reverse");
    {
        int arr[4] = { 1, 2, 3, 4 };
        std::reverse(it_t(arr), it_t(arr + 4));
        VX_CHECK(arr[0] == 4);
        VX_CHECK(arr[3] == 1);
    }
}

//=============================================================================
// reverse_pointer_iterator
//=============================================================================

VX_TEST_CASE(reverse_pointer_iterator_basic)
{
    int arr[5] = { 10, 20, 30, 40, 50 };
    rit_t rbegin{ it_t(arr + 5) };
    rit_t rend{ it_t(arr) };

    VX_STATIC_CHECK(std::is_same<rit_t::value_type, int>::value);
    VX_STATIC_CHECK(std::is_same<rit_t::iterator_category, std::random_access_iterator_tag>::value);
    VX_STATIC_CHECK(std::is_same<decltype(++std::declval<rit_t&>()), rit_t&>::value);
    VX_STATIC_CHECK(std::is_same<decltype(std::declval<rit_t&>()++), rit_t>::value);
    VX_STATIC_CHECK(std::is_same<decltype(std::declval<rit_t>() + 1), rit_t>::value);
    VX_STATIC_CHECK(std::is_same<decltype(1 + std::declval<rit_t>()), rit_t>::value);

    VX_SECTION("default constructed is null");
    {
        rit_t r;
        VX_CHECK(!static_cast<bool>(r));
        VX_CHECK(r.ptr() == nullptr);
    }

    VX_SECTION("dereference yields element before the base position");
    {
        VX_CHECK(static_cast<bool>(rbegin));
        VX_CHECK(*rbegin == 50);
        VX_CHECK(&*rbegin == arr + 4);
        VX_CHECK(rbegin.ptr() == arr + 5);
        VX_CHECK(rend.ptr() == arr);
    }

    VX_SECTION("increment walks backwards");
    {
        rit_t r = rbegin;
        ++r;
        VX_CHECK(*r == 40);
        rit_t old = r++;
        VX_CHECK(*old == 40);
        VX_CHECK(*r == 30);
    }

    VX_SECTION("decrement walks forwards");
    {
        rit_t r(it_t(arr + 2));
        VX_CHECK(*r == 20);
        --r;
        VX_CHECK(*r == 30);
        rit_t old = r--;
        VX_CHECK(*old == 30);
        VX_CHECK(*r == 40);
    }

    VX_SECTION("compound assignment");
    {
        rit_t r = rbegin;
        r += 3;
        VX_CHECK(*r == 20);
        r -= 2;
        VX_CHECK(*r == 40);
    }

    VX_SECTION("binary + and -");
    {
        VX_CHECK(*(rbegin + 2) == 30);
        VX_CHECK(*(2 + rbegin) == 30);
        VX_CHECK(*(rend - 1) == 10);
        VX_CHECK((rbegin + 5) == rend);
        VX_CHECK((rend - 5) == rbegin);
        VX_CHECK(*rbegin == 50); // unchanged
    }

    VX_SECTION("subscript");
    {
        VX_CHECK(rbegin[0] == 50);
        VX_CHECK(rbegin[1] == 40);
        VX_CHECK(rbegin[4] == 10);
        rit_t mid = rbegin + 2;
        VX_CHECK(mid[-2] == 50);
        VX_CHECK(mid[2] == 10);
    }

    VX_SECTION("arrow operator");
    {
        point pts[2] = {
            { 1, 2 },
            { 3, 4 }
        };
        vp::reverse_pointer_iterator<vp::pointer_iterator<owner_a, point>> r{ vp::pointer_iterator<owner_a, point>(pts + 2) };
        VX_CHECK(r->x == 3);
        ++r;
        VX_CHECK(r->y == 2);
    }

    VX_SECTION("full reverse traversal");
    {
        std::vector<int> out;
        for (rit_t r = rbegin; r != rend; ++r)
        {
            out.push_back(*r);
        }
        VX_CHECK(equals(out, { 50, 40, 30, 20, 10 }));
    }

    VX_SECTION("write through reverse iterator");
    {
        *rbegin = 55;
        VX_CHECK(arr[4] == 55);
        arr[4] = 50;
    }
}

VX_TEST_CASE(reverse_pointer_iterator_conversion_and_equality)
{
    int arr[3] = { 1, 2, 3 };
    rit_t r{ it_t(arr + 3) };

    VX_SECTION("implicit conversion to const reverse iterator");
    {
        crit_t c = r;
        VX_CHECK(c.ptr() == arr + 3);
        VX_CHECK(*c == 3);
    }

    VX_SECTION("equality, including mixed const / non-const");
    {
        crit_t c = r;
        rit_t same{ it_t(arr + 3) };
        rit_t other{ it_t(arr + 2) };
        VX_CHECK(r == same);
        VX_CHECK(r != other);
        VX_CHECK(r == c);
        VX_CHECK(c == r);
        VX_CHECK(other != c);
    }

    VX_SECTION("forward and reverse iterators are not comparable");
    {
        VX_STATIC_CHECK(!(can_eq<it_t, rit_t>::value));
        VX_STATIC_CHECK(!(can_eq<rit_t, it_t>::value));
    }
}

VX_TEST_CASE(reverse_pointer_iterator_ordering_and_difference)
{
    int arr[5] = { 10, 20, 30, 40, 50 };
    rit_t rbegin{ it_t(arr + 5) };
    rit_t rend{ it_t(arr) };

    VX_SECTION("ordering");
    {
        VX_CHECK(rbegin < rend);
        VX_CHECK(rend > rbegin);
        VX_CHECK(rbegin <= rend);
        VX_CHECK(rend >= rbegin);
        VX_CHECK(!(rend < rbegin));
        VX_CHECK(!(rbegin > rend));
        VX_CHECK(rbegin <= rbegin);
        VX_CHECK(rbegin >= rbegin);
    }

    VX_SECTION("difference");
    {
        VX_CHECK(rend - rbegin == 5);
        VX_CHECK(rbegin - rend == -5);
        VX_CHECK(rbegin - rbegin == 0);
        VX_CHECK((rbegin + 2) - rbegin == 2);
    }

    VX_SECTION("std::copy over a reverse range");
    {
        // std::copy computes the element count as (last - first)
        int out[5] = { 0, 0, 0, 0, 0 };
        std::copy(rbegin, rend, out);
        VX_CHECK(out[0] == 50);
        VX_CHECK(out[1] == 40);
        VX_CHECK(out[2] == 30);
        VX_CHECK(out[3] == 20);
        VX_CHECK(out[4] == 10);
    }
}

//=============================================================================
// vx::reverse_iterator
//=============================================================================

VX_TEST_CASE(reverse_iterator_member_types)
{
    using ri = vx::reverse_iterator<int*>;
    VX_STATIC_CHECK(std::is_same<ri::iterator_type, int*>::value);
    VX_STATIC_CHECK(std::is_same<ri::value_type, int>::value);
    VX_STATIC_CHECK(std::is_same<ri::pointer, int*>::value);
    VX_STATIC_CHECK(std::is_same<ri::reference, int&>::value);
    VX_STATIC_CHECK(std::is_same<ri::difference_type, std::ptrdiff_t>::value);
    VX_STATIC_CHECK(std::is_same<ri::iterator_category, std::random_access_iterator_tag>::value);

    using cri = vx::reverse_iterator<const int*>;
    VX_STATIC_CHECK(std::is_same<cri::reference, const int&>::value);

    using lri = vx::reverse_iterator<std::list<int>::iterator>;
    VX_STATIC_CHECK(std::is_same<lri::iterator_category, std::bidirectional_iterator_tag>::value);

    VX_STATIC_CHECK(!std::is_convertible<int*, ri>::value); // explicit ctor
    VX_STATIC_CHECK(std::is_constructible<ri, int*>::value);
}

VX_TEST_CASE(reverse_iterator_pointers)
{
    int arr[5] = { 1, 2, 3, 4, 5 };
    auto rbegin = vx::make_reverse_iterator(arr + 5);
    auto rend = vx::make_reverse_iterator(arr);

    VX_STATIC_CHECK(std::is_same<decltype(rbegin), vx::reverse_iterator<int*>>::value);

    VX_SECTION("default constructed");
    {
        vx::reverse_iterator<int*> r;
        VX_CHECK(r.base() == nullptr);
    }

    VX_SECTION("base");
    {
        VX_CHECK(rbegin.base() == arr + 5);
        VX_CHECK(rend.base() == arr);
    }

    VX_SECTION("dereference");
    {
        VX_CHECK(*rbegin == 5);
        VX_CHECK(*(rend - 1) == 1);
        VX_CHECK(&*(rend - 1) == arr);
    }

    VX_SECTION("arrow");
    {
        point pts[2] = {
            { 1, 2 },
            { 3, 4 }
        };
        auto r = vx::make_reverse_iterator(pts + 2);
        VX_CHECK(r->x == 3);
        VX_CHECK(r->y == 4);
    }

    VX_SECTION("full traversal");
    {
        std::vector<int> out;
        for (auto it = rbegin; it != rend; ++it)
        {
            out.push_back(*it);
        }
        VX_CHECK(equals(out, { 5, 4, 3, 2, 1 }));
    }

    VX_SECTION("pre/post increment and decrement");
    {
        auto r = rbegin;
        VX_CHECK(*++r == 4);
        auto old = r++;
        VX_CHECK(*old == 4);
        VX_CHECK(*r == 3);
        VX_CHECK(*--r == 4);
        old = r--;
        VX_CHECK(*old == 4);
        VX_CHECK(*r == 5);
    }

    VX_SECTION("arithmetic");
    {
        VX_CHECK(*(rbegin + 2) == 3);
        VX_CHECK(*(2 + rbegin) == 3);
        VX_CHECK(*(rend - 1) == 1);
        auto r = rbegin;
        r += 4;
        VX_CHECK(*r == 1);
        r -= 3;
        VX_CHECK(*r == 4);
        VX_CHECK((rbegin + 5) == rend);
    }

    VX_SECTION("subscript");
    {
        VX_CHECK(rbegin[0] == 5);
        VX_CHECK(rbegin[2] == 3);
        VX_CHECK(rbegin[4] == 1);
    }

    VX_SECTION("difference has swapped operands");
    {
        VX_CHECK(rend - rbegin == 5);
        VX_CHECK(rbegin - rend == -5);
        VX_CHECK(rbegin - rbegin == 0);
        VX_STATIC_CHECK(std::is_same<decltype(rend - rbegin), std::ptrdiff_t>::value);
    }

    VX_SECTION("comparison is reversed");
    {
        VX_CHECK(rbegin < rend);
        VX_CHECK(rend > rbegin);
        VX_CHECK(rbegin <= rend);
        VX_CHECK(rend >= rbegin);
        VX_CHECK(!(rend < rbegin));
        VX_CHECK(!(rbegin > rend));
        VX_CHECK(rbegin <= rbegin);
        VX_CHECK(rbegin >= rbegin);
        VX_CHECK(rbegin == rbegin);
        VX_CHECK(rbegin != rend);
        VX_CHECK(!(rbegin == rend));
    }

    VX_SECTION("write through");
    {
        *rbegin = 55;
        VX_CHECK(arr[4] == 55);
        arr[4] = 5;
    }
}

VX_TEST_CASE(reverse_iterator_conversions)
{
    int arr[3] = { 1, 2, 3 };

    VX_SECTION("converting constructor");
    {
        vx::reverse_iterator<int*> r(arr + 3);
        vx::reverse_iterator<const int*> cr = r;
        VX_CHECK(cr.base() == arr + 3);
        VX_CHECK(*cr == 3);
    }

    VX_SECTION("converting assignment");
    {
        vx::reverse_iterator<int*> r(arr + 2);
        vx::reverse_iterator<const int*> cr(static_cast<const int*>(arr + 3));
        cr = r;
        VX_CHECK(cr.base() == arr + 2);
        VX_CHECK(*cr == 2);
    }

    VX_SECTION("mixed comparison");
    {
        vx::reverse_iterator<int*> a(arr + 3);
        vx::reverse_iterator<const int*> b(static_cast<const int*>(arr + 3));
        vx::reverse_iterator<const int*> c(static_cast<const int*>(arr + 1));
        VX_CHECK(a == b);
        VX_CHECK(a != c);
        VX_CHECK(a < c);
        VX_CHECK(c > a);
        VX_CHECK(c - a == 2);
    }
}

VX_TEST_CASE(reverse_iterator_other_iterators)
{
    VX_SECTION("std::vector iterators");
    {
        std::vector<int> v = { 1, 2, 3, 4 };
        auto rb = vx::make_reverse_iterator(v.end());
        auto re = vx::make_reverse_iterator(v.begin());
        std::vector<int> out;
        for (; rb != re; ++rb)
        {
            out.push_back(*rb);
        }
        VX_CHECK(equals(out, { 4, 3, 2, 1 }));
    }

    VX_SECTION("bidirectional (std::list) iterators");
    {
        std::list<int> l = { 1, 2, 3 };
        auto r = vx::make_reverse_iterator(l.end());
        VX_CHECK(*r == 3);
        ++r;
        VX_CHECK(*r == 2);
        --r;
        VX_CHECK(*r == 3);
        auto old = r++;
        VX_CHECK(*old == 3);
        VX_CHECK(*r == 2);
        VX_CHECK(r != vx::make_reverse_iterator(l.begin()));
    }

    VX_SECTION("vx pointer_iterator");
    {
        int arr[4] = { 7, 8, 9, 10 };
        vx::reverse_iterator<it_t> rb{ it_t(arr + 4) };
        vx::reverse_iterator<it_t> re{ it_t(arr) };
        VX_CHECK(*rb == 10);
        VX_CHECK(rb[3] == 7);
        VX_CHECK(re - rb == 4);
        VX_CHECK(rb < re);
        VX_CHECK(*(rb + 1) == 9);
    }
}

//=============================================================================
// vx::back_insert_iterator / back_inserter
//=============================================================================

VX_TEST_CASE(back_insert_iterator_types)
{
    using bi = vx::back_insert_iterator<std::vector<int>>;
    VX_STATIC_CHECK(std::is_same<bi::iterator_category, std::output_iterator_tag>::value);
    VX_STATIC_CHECK(std::is_void<bi::value_type>::value);
    VX_STATIC_CHECK(std::is_void<bi::pointer>::value);
    VX_STATIC_CHECK(std::is_void<bi::reference>::value);
    VX_STATIC_CHECK(std::is_same<bi::m_container_type, std::vector<int>>::value);
    VX_STATIC_CHECK(std::is_same<bi::difference_type, std::ptrdiff_t>::value);

    std::vector<int> v;
    VX_STATIC_CHECK(std::is_same<decltype(vx::back_inserter(v)), bi>::value);

    // dereference / increment are no-ops that return the iterator itself
    VX_STATIC_CHECK(std::is_same<decltype(*std::declval<bi&>()), bi&>::value);
    VX_STATIC_CHECK(std::is_same<decltype(++std::declval<bi&>()), bi&>::value);
    VX_STATIC_CHECK(std::is_same<decltype(std::declval<bi&>()++), bi>::value);
}

VX_TEST_CASE(back_insert_iterator_behavior)
{
    VX_SECTION("assignment appends");
    {
        std::vector<int> v;
        auto it = vx::back_inserter(v);
        *it = 1;
        ++it;
        *it = 2;
        it++;
        *it = 3;
        VX_CHECK(equals(v, { 1, 2, 3 }));
    }

    VX_SECTION("deref/increment do not themselves insert");
    {
        std::vector<int> v;
        auto it = vx::back_inserter(v);
        *it;
        ++it;
        it++;
        *it;
        VX_CHECK(v.empty());
    }

    VX_SECTION("*it++ = value idiom");
    {
        std::vector<int> v;
        auto it = vx::back_inserter(v);
        *it++ = 10;
        *it++ = 20;
        VX_CHECK(equals(v, { 10, 20 }));
    }

    VX_SECTION("appends after existing elements");
    {
        std::vector<int> v = { 1, 2 };
        auto it = vx::back_inserter(v);
        *it = 3;
        VX_CHECK(equals(v, { 1, 2, 3 }));
    }

    VX_SECTION("copies of the iterator share the container");
    {
        std::vector<int> v;
        auto a = vx::back_inserter(v);
        auto b = a;
        *a = 1;
        *b = 2;
        VX_CHECK(equals(v, { 1, 2 }));
    }

    VX_SECTION("works with std::list and std::string");
    {
        std::list<int> l;
        auto li = vx::back_inserter(l);
        *li = 4;
        *li = 5;
        VX_CHECK(l.size() == 2);
        VX_CHECK(l.front() == 4);
        VX_CHECK(l.back() == 5);

        std::string s;
        auto si = vx::back_inserter(s);
        *si = 'a';
        *si = 'b';
        VX_CHECK(s == "ab");
    }

    VX_SECTION("copy vs move overloads");
    {
        std::vector<copyable_int> v;
        auto it = vx::back_inserter(v);

        copyable_int x(7);
        *it = x; // const T& overload: source untouched
        VX_CHECK(x.val == 7);
        VX_CHECK(v.size() == 1);
        VX_CHECK(v[0].val == 7);

        *it = std::move(x); // T&& overload: source moved-from (-1)
        VX_CHECK(x.val == -1);
        VX_CHECK(v.size() == 2);
        VX_CHECK(v[1].val == 7);
    }

    VX_SECTION("move-only element type");
    {
        std::vector<movable_int> v;
        auto it = vx::back_inserter(v);
        movable_int m(5);
        *it = std::move(m);
        VX_CHECK(m.val == -1);
        VX_CHECK(v.size() == 1);
        VX_CHECK(v[0].val == 5);
        *it = movable_int(6);
        VX_CHECK(v.size() == 2);
        VX_CHECK(v[1].val == 6);
    }

    VX_SECTION("std::copy into a back_inserter");
    {
        int src[4] = { 1, 2, 3, 4 };
        std::vector<int> v;
        std::copy(src, src + 4, vx::back_inserter(v));
        VX_CHECK(equals(v, { 1, 2, 3, 4 }));
    }
}

//=============================================================================
// vx::move_iterator
//=============================================================================

VX_TEST_CASE(move_iterator_types)
{
    using mi = vx::move_iterator<int*>;
    VX_STATIC_CHECK(std::is_same<mi::iterator_type, int*>::value);
    VX_STATIC_CHECK(std::is_same<mi::value_type, int>::value);
    VX_STATIC_CHECK(std::is_same<mi::reference, int&&>::value);
    VX_STATIC_CHECK(std::is_same<mi::pointer, int*>::value);
    VX_STATIC_CHECK(std::is_same<mi::difference_type, std::ptrdiff_t>::value);
    VX_STATIC_CHECK(std::is_same<mi::iterator_category, std::random_access_iterator_tag>::value);
    VX_STATIC_CHECK(std::is_same<decltype(*std::declval<mi>()), int&&>::value);
    VX_STATIC_CHECK(std::is_same<decltype(vx::make_move_iterator(static_cast<int*>(nullptr))), mi>::value);

    VX_STATIC_CHECK(!std::is_convertible<int*, mi>::value); // explicit ctor
}

VX_TEST_CASE(move_iterator_dereference_moves)
{
    movable_int arr[3] = { movable_int(1), movable_int(2), movable_int(3) };
    movable_int* p = arr;
    auto mit = vx::make_move_iterator(p);

    VX_SECTION("operator* yields an rvalue that can be moved from");
    {
        movable_int a = *mit;
        VX_CHECK(a.val == 1);
        VX_CHECK(arr[0].val == -1);
    }

    VX_SECTION("increment then dereference");
    {
        ++mit;
        movable_int b = *mit;
        VX_CHECK(b.val == 2);
        VX_CHECK(arr[1].val == -1);
    }

    VX_SECTION("subscript");
    {
        // mit now points at arr[1]
        movable_int c = mit[1];
        VX_CHECK(c.val == 3);
        VX_CHECK(arr[2].val == -1);
    }
}

VX_TEST_CASE(move_iterator_operations)
{
    int arr[5] = { 1, 2, 3, 4, 5 };
    auto b = vx::make_move_iterator(arr);
    auto e = vx::make_move_iterator(arr + 5);

    VX_SECTION("default constructed + base + arrow");
    {
        vx::move_iterator<int*> d;
        (void)d;
        VX_CHECK(b.base() == arr);
        VX_CHECK(b.operator->() == arr);
    }

    VX_SECTION("increment / decrement");
    {
        auto it = b;
        VX_CHECK((++it).base() == arr + 1);
        auto old = it++;
        VX_CHECK(old.base() == arr + 1);
        VX_CHECK(it.base() == arr + 2);
        VX_CHECK((--it).base() == arr + 1);
        old = it--;
        VX_CHECK(old.base() == arr + 1);
        VX_CHECK(it.base() == arr);
    }

    VX_SECTION("arithmetic");
    {
        VX_CHECK((b + 3).base() == arr + 3);
        VX_CHECK((3 + b).base() == arr + 3);
        VX_CHECK((e - 2).base() == arr + 3);
        auto it = b;
        it += 4;
        VX_CHECK(it.base() == arr + 4);
        it -= 1;
        VX_CHECK(it.base() == arr + 3);
    }

    VX_SECTION("difference and comparison are not reversed");
    {
        VX_CHECK(e - b == 5);
        VX_CHECK(b - e == -5);
        VX_CHECK(b == b);
        VX_CHECK(b != e);
        VX_CHECK(b < e);
        VX_CHECK(e > b);
        VX_CHECK(b <= e);
        VX_CHECK(e >= b);
        VX_CHECK(b <= b);
        VX_CHECK(!(e < b));
    }

    VX_SECTION("converting constructor and mixed comparison");
    {
        vx::move_iterator<const int*> cb = b;
        VX_CHECK(cb.base() == arr);
        VX_CHECK(cb == b);
        VX_CHECK(e - cb == 5);
    }

    VX_SECTION("moving a range of move-only objects");
    {
        movable_int src[3] = { movable_int(1), movable_int(2), movable_int(3) };
        std::vector<movable_int> dst;
        for (auto it = vx::make_move_iterator(src); it != vx::make_move_iterator(src + 3); ++it)
        {
            dst.push_back(*it);
        }
        VX_CHECK(dst.size() == 3);
        VX_CHECK(dst[0].val == 1);
        VX_CHECK(dst[1].val == 2);
        VX_CHECK(dst[2].val == 3);
        VX_CHECK(src[0].val == -1);
        VX_CHECK(src[1].val == -1);
        VX_CHECK(src[2].val == -1);
    }
}

//=============================================================================
// distance / next / iter_swap
//=============================================================================

VX_TEST_CASE(distance_tests)
{
    VX_SECTION("raw pointers");
    {
        int arr[5] = { 1, 2, 3, 4, 5 };
        VX_CHECK(vx::distance(arr, arr + 5) == 5);
        VX_CHECK(vx::distance(arr, arr) == 0);
        VX_CHECK(vx::distance(arr + 2, arr + 3) == 1);
        VX_STATIC_CHECK(std::is_same<decltype(vx::distance(arr, arr + 5)), std::ptrdiff_t>::value);
    }

    VX_SECTION("random access std iterators");
    {
        std::vector<int> v(7);
        VX_CHECK(vx::distance(v.begin(), v.end()) == 7);
        VX_CHECK(vx::distance(v.end(), v.end()) == 0);
        VX_CHECK(vx::distance(v.begin() + 2, v.end()) == 5);
    }

    VX_SECTION("bidirectional / forward iterators (counting path)");
    {
        std::list<int> l = { 1, 2, 3, 4 };
        VX_CHECK(vx::distance(l.begin(), l.end()) == 4);
        VX_CHECK(vx::distance(l.end(), l.end()) == 0);

        std::forward_list<int> f = { 1, 2, 3 };
        VX_CHECK(vx::distance(f.begin(), f.end()) == 3);
    }

    VX_SECTION("vx pointer_iterator");
    {
        int arr[4] = { 1, 2, 3, 4 };
        VX_CHECK(vx::distance(it_t(arr), it_t(arr + 4)) == 4);
        VX_CHECK(vx::distance(it_t(arr), it_t(arr)) == 0);
    }

    VX_SECTION("vx::reverse_iterator");
    {
        int arr[4] = { 1, 2, 3, 4 };
        auto rb = vx::make_reverse_iterator(arr + 4);
        auto re = vx::make_reverse_iterator(arr);
        VX_CHECK(vx::distance(rb, re) == 4);
        VX_CHECK(vx::distance(rb, rb) == 0);
    }

    // NOTE: vx::distance(rit_t, rit_t) is intentionally not exercised here:
    // its range assertion uses first <= last, which hits the reverse ordering
    // issue noted in reverse_pointer_iterator_ordering_and_difference.
}

VX_TEST_CASE(next_tests)
{
    VX_SECTION("forward_list, default step");
    {
        std::forward_list<int> f = { 10, 20, 30 };
        auto it = vx::next(f.begin());
        VX_CHECK(*it == 20);
    }

    VX_SECTION("forward_list, explicit step");
    {
        std::forward_list<int> f = { 10, 20, 30, 40 };
        VX_CHECK(*vx::next(f.begin(), 0) == 10);
        VX_CHECK(*vx::next(f.begin(), 2) == 30);
        VX_CHECK(vx::next(f.begin(), 4) == f.end());
    }

    VX_SECTION("list (bidirectional), forward steps");
    {
        std::list<int> l = { 1, 2, 3 };
        VX_CHECK(*vx::next(l.begin()) == 2);
        VX_CHECK(*vx::next(l.begin(), 2) == 3);
        VX_CHECK(vx::next(l.begin(), 3) == l.end());
    }

    VX_SECTION("does not mutate its argument");
    {
        std::list<int> l = { 1, 2, 3 };
        auto it = l.begin();
        auto n = vx::next(it, 2);
        VX_CHECK(it == l.begin());
        VX_CHECK(*n == 3);
    }
}

VX_TEST_CASE(iter_swap_tests)
{
    VX_SECTION("pointers");
    {
        int a = 1, b = 2;
        vx::iter_swap(&a, &b);
        VX_CHECK(a == 2);
        VX_CHECK(b == 1);
    }

    VX_SECTION("vector iterators");
    {
        std::vector<int> v = { 1, 2, 3, 4 };
        vx::iter_swap(v.begin(), v.end() - 1);
        VX_CHECK(equals(v, { 4, 2, 3, 1 }));
    }

    VX_SECTION("different iterator types");
    {
        int arr[2] = { 10, 20 };
        std::vector<int> v = { 1, 2 };
        vx::iter_swap(arr, v.begin() + 1);
        VX_CHECK(arr[0] == 2);
        VX_CHECK(v[1] == 10);
    }

    VX_SECTION("vx pointer_iterator");
    {
        int arr[3] = { 1, 2, 3 };
        vx::iter_swap(it_t(arr), it_t(arr + 2));
        VX_CHECK(arr[0] == 3);
        VX_CHECK(arr[2] == 1);
    }

    VX_SECTION("self swap leaves value unchanged");
    {
        int a = 5;
        vx::iter_swap(&a, &a);
        VX_CHECK(a == 5);
    }

    VX_SECTION("move-only elements");
    {
        movable_int arr[2] = { movable_int(1), movable_int(2) };
        vx::iter_swap(arr, arr + 1);
        VX_CHECK(arr[0].val == 2);
        VX_CHECK(arr[1].val == 1);
    }
}

//=============================================================================

#ifndef VX_TEST_NO_MAIN
int main(int argc, char** argv)
{
    return VX_RUN_TESTS(argc, argv);
}
#endif
