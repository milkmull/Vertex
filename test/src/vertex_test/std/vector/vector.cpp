#include "vertex/std/vector.hpp"

#include <cstddef>
#include <cstdlib>
#include <initializer_list>
#include <limits>
#include <list>
#include <new>
#include <vector>

#include "vertex_test/test.hpp"

//#define ONLY_CONTAINER_TESTS

// https://github.com/microsoft/STL/blob/e2ef398685f7e470dbaeaf65ff919de72bda7489/tests/tr1/tests/vector/test.cpp

//=========================================================================
// helpers
//=========================================================================

namespace {

// Non-trivial type that counts live instances, copies and moves. Moved-from
// objects get v == -1. Deliberately default-constructible and assignable,
// because the middle-insert path default-constructs then assigns.
struct tracked
{
    int v;

    static inline int live = 0;
    static inline int copies = 0;
    static inline int moves = 0;

    static void reset()
    {
        live = 0;
        copies = 0;
        moves = 0;
    }

    tracked()
        : v(0)
    {
        ++live;
    }
    tracked(int x)
        : v(x)
    {
        ++live;
    }
    tracked(const tracked& o)
        : v(o.v)
    {
        ++live;
        ++copies;
    }
    tracked(tracked&& o) noexcept
        : v(o.v)
    {
        o.v = -1;
        ++live;
        ++moves;
    }
    tracked& operator=(const tracked& o)
    {
        v = o.v;
        ++copies;
        return *this;
    }
    tracked& operator=(tracked&& o) noexcept
    {
        v = o.v;
        o.v = -1;
        ++moves;
        return *this;
    }
    ~tracked()
    {
        --live;
    }

    bool operator==(const tracked& o) const
    {
        return v == o.v;
    }
    bool operator!=(const tracked& o) const
    {
        return v != o.v;
    }
    bool operator<(const tracked& o) const
    {
        return v < o.v;
    }
    bool operator>(const tracked& o) const
    {
        return v > o.v;
    }
    bool operator<=(const tracked& o) const
    {
        return v <= o.v;
    }
    bool operator>=(const tracked& o) const
    {
        return v >= o.v;
    }
};

inline int val(int x)
{
    return x;
}
inline int val(const tracked& t)
{
    return t.v;
}

template <typename T>
vx::vector<T> make(std::initializer_list<int> l)
{
    vx::vector<T> v;
    for (int x : l)
    {
        VX_CHECK(v.emplace_back(x));
    }
    return v;
}

template <typename V>
bool eq(const V& v, std::initializer_list<int> l)
{
    if (v.size() != l.size())
    {
        return false;
    }
    size_t i = 0;
    for (int x : l)
    {
        if (val(v[i++]) != x)
        {
            return false;
        }
    }
    return true;
}

template <typename V>
bool eq_model(const V& v, const std::vector<int>& m)
{
    if (v.size() != m.size())
    {
        return false;
    }
    for (size_t i = 0; i < m.size(); ++i)
    {
        if (val(v[i]) != m[i])
        {
            return false;
        }
    }
    return true;
}

// Runs a templated test for int, copyable_int and tracked (and checks tracked leaks).
#define TEST_BOTH(name) \
    VX_TEST_CASE(name##_int) \
    { \
        test_##name<int>(); \
    } \
    VX_TEST_CASE(name##_copyable) \
    { \
        test_##name<vx::test::copyable_int>(); \
    } \
    VX_TEST_CASE(name##_tracked) \
    { \
        tracked::reset(); \
        test_##name<tracked>(); \
        VX_CHECK(tracked::live == 0); \
    }

} // namespace

//=========================================================================
// original test
//=========================================================================

static void test_container()
{
    using vec = vx::vector<char>;
    using alloc = vx::mem::default_allocator<char>;

    vec v0;
    alloc al = v0.get_allocator();
    vec v0a(al);
    VX_CHECK(v0.empty());
    VX_CHECK(v0.size() == 0);
    VX_CHECK(v0a.size() == 0);
    VX_CHECK(v0a.get_allocator() == al);

    vec v1(5);
    vec v1a(6, 'x');
    vec v1b(7, 'y', al);
    VX_CHECK(v1.size() == 5);
    VX_CHECK(v1a.size() == 6);
    VX_CHECK(v1a.back() == 'x');
    VX_CHECK(v1b.size() == 7);
    VX_CHECK(v1b.back() == 'y');

    vec v2(v1a);
    VX_CHECK(v2.size() == 6);
    VX_CHECK(v2.front() == 'x');

    vec v2a(v2, al);
    VX_CHECK(v2a.size() == 6);
    VX_CHECK(v2a.front() == 'x');

    vec v3(v1a.begin(), v1a.end());
    VX_CHECK(v3.size() == 6);
    VX_CHECK(v3.front() == 'x');

    const vec v4(v1a.begin(), v1a.end(), al);
    v0 = v4;
    VX_CHECK(v0.size() == 6);
    VX_CHECK(v0.front() == 'x');
    VX_CHECK(v0[0] == 'x');
    VX_CHECK(v0.at(5).value() == 'x');

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
        VX_CHECK(v6.size() == 0);
        VX_CHECK(v7.size() == 20);

        vec v8;
        v8 = std::move(v7);
        VX_CHECK(v7.size() == 0);
        VX_CHECK(v8.size() == 20);

        // matching allocator -> takes over the buffer instead of moving elements
        vec v8a(std::move(v8), alloc());
        VX_CHECK(v8.size() == 0);
        VX_CHECK(v8a.size() == 20);

        VX_DISABLE_WARNING_POP();

        vec v9;
        v9.resize(10, '\0');
        VX_CHECK(v9.size() == 10);
        VX_CHECK(v9[9] == 0);

        using movable_int = vx::test::movable_int;
        vx::vector<movable_int> v10;
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
        vx::vector<copyable_int> v11;
        copyable_int ci1(1);
        v11.push_back(ci1);
        VX_CHECK(ci1.val == 1);
        VX_CHECK(v11[0].val == 1);

        copyable_int ci3(3);
        v11.insert(v11.begin(), ci3);
        VX_CHECK(ci3.val == 3);
        VX_CHECK(v11[0].val == 3);
        VX_CHECK(v11[1].val == 1);

        vx::vector<copyable_int> v12(v11);
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

        char* pd = v0x.data();
        VX_CHECK(pd == nullptr);
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

#if !defined(ONLY_CONTAINER_TESTS)

//=========================================================================
// construction
//=========================================================================

template <typename T>
void test_construct()
{
    using vec = vx::vector<T>;

    VX_SECTION("default / allocator");
    {
        vec v;
        VX_CHECK(v.empty());
        VX_CHECK(v.size() == 0);
        VX_CHECK(v.capacity() == 0);
        VX_CHECK(v.data() == nullptr);
        VX_CHECK(v.begin() == v.end());

        typename vec::allocator_type al;
        vec w(al);
        VX_CHECK(w.empty());
        VX_CHECK(w.get_allocator() == al);
    }

    VX_SECTION("count");
    {
        vec v(size_t(5));
        VX_CHECK(v.size() == 5);
        VX_CHECK(v.capacity() == 5);

        vec z(size_t(0));
        VX_CHECK(z.empty());
        VX_CHECK(z.data() == nullptr);
    }

    VX_SECTION("fill");
    {
        vec v(size_t(4), T(7));
        VX_CHECK(eq(v, { 7, 7, 7, 7 }));
        vec z(size_t(0), T(7));
        VX_CHECK(z.empty());
    }

    VX_SECTION("initializer_list");
    {
        vec v{ T(1), T(2), T(3) };
        VX_CHECK(eq(v, { 1, 2, 3 }));
        std::initializer_list<T> none{};
        vec e(none);
        VX_CHECK(e.empty());
    }

    VX_SECTION("copy");
    {
        vec a = make<T>({ 1, 2, 3 });
        vec b(a);
        VX_CHECK(eq(b, { 1, 2, 3 }));
        VX_CHECK(eq(a, { 1, 2, 3 }));
        VX_CHECK(a.data() != b.data());

        typename vec::allocator_type al;
        vec c(a, al);
        VX_CHECK(eq(c, { 1, 2, 3 }));

        vec empty;
        vec d(empty);
        VX_CHECK(d.empty());
        VX_CHECK(d.data() == nullptr);
    }

    VX_SECTION("move");
    {
        VX_DISABLE_USE_AFTER_MOVE_WARNING();

        vec a = make<T>({ 1, 2, 3 });
        T* p = a.data();
        vec b(std::move(a));
        VX_CHECK(eq(b, { 1, 2, 3 }));
        VX_CHECK(b.data() == p);
        VX_CHECK(a.empty());
        VX_CHECK(a.data() == nullptr);
        VX_CHECK(a.capacity() == 0);

        typename vec::allocator_type al;
        vec c(std::move(b), al);
        VX_CHECK(eq(c, { 1, 2, 3 }));
        VX_CHECK(b.empty());

        VX_DISABLE_WARNING_POP();
    }

    VX_SECTION("pointer / vx iterator ranges");
    {
        T src[] = { T(1), T(2), T(3), T(4) };
        vec a(src, src + 4);
        VX_CHECK(eq(a, { 1, 2, 3, 4 }));

        vec b(a.begin() + 1, a.end());
        VX_CHECK(eq(b, { 2, 3, 4 }));

        vec c(src, src);
        VX_CHECK(c.empty());
        VX_CHECK(c.data() == nullptr);
    }
}
TEST_BOTH(construct)

template <typename T>
void test_create()
{
    using vec = vx::vector<T>;

    auto a = vec::create();
    VX_CHECK(a);
    VX_CHECK(a.value().empty());

    auto b = vec::create(size_t(3));
    VX_CHECK(b);
    VX_CHECK(b.value().size() == 3);

    auto c = vec::create(size_t(3), T(4));
    VX_CHECK(c);
    VX_CHECK(eq(c.value(), { 4, 4, 4 }));

    std::initializer_list<T> il{ T(1), T(2) };
    auto d = vec::create(il);
    VX_CHECK(d);
    VX_CHECK(eq(d.value(), { 1, 2 }));

    vec src = make<T>({ 5, 6, 7 });
    auto e = vec::create(src);
    VX_CHECK(e);
    VX_CHECK(eq(e.value(), { 5, 6, 7 }));
    VX_CHECK(eq(src, { 5, 6, 7 }));

    {
        VX_DISABLE_USE_AFTER_MOVE_WARNING();
        auto f = vec::create(std::move(src));
        VX_CHECK(f);
        VX_CHECK(eq(f.value(), { 5, 6, 7 }));
        VX_CHECK(src.empty());
        VX_DISABLE_WARNING_POP();
    }

    T arr[] = { T(8), T(9) };
    auto g = vec::create(arr, arr + 2);
    VX_CHECK(g);
    VX_CHECK(eq(g.value(), { 8, 9 }));

    std::list<int> l{ 1, 2, 3 };
    auto h = vec::create(l.begin(), l.end());
    VX_CHECK(h);
    VX_CHECK(eq(h.value(), { 1, 2, 3 }));
}
TEST_BOTH(create)

//=========================================================================
// assignment
//=========================================================================

template <typename T>
void test_assign()
{
    using vec = vx::vector<T>;
    const vec a = make<T>({ 1, 2, 3, 4, 5 });

    VX_SECTION("copy assign: grow, shrink, equal");
    {
        vec b;
        VX_CHECK(b.assign(a));
        VX_CHECK(eq(b, { 1, 2, 3, 4, 5 }));

        vec c = make<T>({ 9 });
        c = a;
        VX_CHECK(eq(c, { 1, 2, 3, 4, 5 }));

        vec d = make<T>({ 1, 2, 3, 4, 5, 6, 7, 8 });
        const size_t cap = d.capacity();
        d = a;
        VX_CHECK(eq(d, { 1, 2, 3, 4, 5 }));
        VX_CHECK(d.capacity() == cap); // shrinking assign must not reallocate

        vec e = a;
        e = a;
        VX_CHECK(eq(e, { 1, 2, 3, 4, 5 }));
    }

    VX_SECTION("self assign");
    {
        vec e = a;
        vec& er = e;
        e = er;
        VX_CHECK(eq(e, { 1, 2, 3, 4, 5 }));
    }

    VX_SECTION("initializer_list");
    {
        std::initializer_list<T> il{ T(7), T(8) };
        vec d = a;
        d = il;
        VX_CHECK(eq(d, { 7, 8 }));
        d.clear();
        VX_CHECK(d.assign(il));
        VX_CHECK(eq(d, { 7, 8 }));
    }

    VX_SECTION("count / value");
    {
        vec d = make<T>({ 1, 2, 3 });
        VX_CHECK(d.assign(size_t(2), T(4)));
        VX_CHECK(eq(d, { 4, 4 }));
        VX_CHECK(d.assign(size_t(5), T(5)));
        VX_CHECK(eq(d, { 5, 5, 5, 5, 5 }));
        VX_CHECK(d.assign(size_t(12), T(6)));
        VX_CHECK(d.size() == 12);
        VX_CHECK(val(d.front()) == 6 && val(d.back()) == 6);
        VX_CHECK(d.assign(size_t(0), T(1)));
        VX_CHECK(d.empty());
    }

    VX_SECTION("pointer + count / pointer range");
    {
        T src[] = { T(1), T(2), T(3), T(4) };
        vec d = make<T>({ 9, 9 });
        VX_CHECK(d.assign(static_cast<const T*>(src), size_t(3)));
        VX_CHECK(eq(d, { 1, 2, 3 }));
        VX_CHECK(d.assign(src, src + 4));
        VX_CHECK(eq(d, { 1, 2, 3, 4 }));
        VX_CHECK(d.assign(src, src));
        VX_CHECK(d.empty());
    }

    VX_SECTION("vx iterator range from another vector");
    {
        vec d = make<T>({ 9 });
        VX_CHECK(d.assign(a.begin() + 1, a.begin() + 4));
        VX_CHECK(eq(d, { 2, 3, 4 }));
    }

    VX_SECTION("move assign");
    {
        VX_DISABLE_USE_AFTER_MOVE_WARNING();

        vec x = make<T>({ 1, 2, 3 });
        T* p = x.data();
        vec y = make<T>({ 7, 7, 7, 7 });
        y = std::move(x);
        VX_CHECK(eq(y, { 1, 2, 3 }));
        VX_CHECK(y.data() == p);
        VX_CHECK(x.empty());
        VX_CHECK(x.data() == nullptr);

        vec z;
        VX_CHECK(z.assign(std::move(y)));
        VX_CHECK(eq(z, { 1, 2, 3 }));

        VX_DISABLE_WARNING_POP();
    }
}
TEST_BOTH(assign)

//=========================================================================
// element access / iterators
//=========================================================================

template <typename T>
void test_access()
{
    using vec = vx::vector<T>;
    vec v = make<T>({ 10, 20, 30 });
    const vec& cv = v;

    VX_CHECK(val(v.front()) == 10);
    VX_CHECK(val(v.back()) == 30);
    VX_CHECK(val(cv.front()) == 10);
    VX_CHECK(val(cv.back()) == 30);
    VX_CHECK(val(v[1]) == 20);
    VX_CHECK(val(cv[2]) == 30);
    VX_CHECK(v.data() == &v[0]);
    VX_CHECK(cv.data() == &cv[0]);

    VX_CHECK(v.at(0));
    VX_CHECK(val(v.at(2).value()) == 30);
    VX_CHECK(val(cv.at(1).value()) == 20);

    auto bad = v.at(3);
    VX_CHECK(!bad);
    VX_CHECK_EXPECTED_ERROR(bad, vx::err::out_of_range);
    auto cbad = cv.at(100);
    VX_CHECK(!cbad);
    VX_CHECK_EXPECTED_ERROR(cbad, vx::err::out_of_range);

    vec e;
    auto ebad = e.at(0);
    VX_CHECK(!ebad);

    v[1] = T(21);
    VX_CHECK(eq(v, { 10, 21, 30 }));
}
TEST_BOTH(access)

template <typename T>
void test_iterators()
{
    using vec = vx::vector<T>;
    vec v = make<T>({ 1, 2, 3, 4 });
    const vec& cv = v;

    int sum = 0;
    for (auto it = v.begin(); it != v.end(); ++it)
    {
        sum += val(*it);
    }
    VX_CHECK(sum == 10);

    sum = 0;
    for (auto it = cv.cbegin(); it != cv.cend(); ++it)
    {
        sum += val(*it);
    }
    VX_CHECK(sum == 10);

    int expect = 4;
    for (auto it = v.rbegin(); it != v.rend(); ++it)
    {
        VX_CHECK(val(*it) == expect--);
    }
    VX_CHECK(expect == 0);

    expect = 4;
    for (auto it = cv.crbegin(); it != cv.crend(); ++it)
    {
        VX_CHECK(val(*it) == expect--);
    }

    VX_CHECK(val(*v.begin()) == 1);
    VX_CHECK(val(*(v.end() - 1)) == 4);
    VX_CHECK(static_cast<size_t>(v.end() - v.begin()) == v.size());

    // mutate through iterator
    for (auto it = v.begin(); it != v.end(); ++it)
    {
        *it = T(val(*it) * 2);
    }
    VX_CHECK(eq(v, { 2, 4, 6, 8 }));

    vec e;
    VX_CHECK(e.begin() == e.end());
    VX_CHECK(e.rbegin() == e.rend());
    VX_CHECK(e.cbegin() == e.cend());
}
TEST_BOTH(iterators)

//=========================================================================
// capacity / size
//=========================================================================

template <typename T>
void test_capacity()
{
    using vec = vx::vector<T>;

    VX_SECTION("reserve");
    {
        vec v = make<T>({ 1, 2, 3 });
        VX_CHECK(v.reserve(50));
        VX_CHECK(v.capacity() >= 50);
        VX_CHECK(v.size() == 3);
        VX_CHECK(eq(v, { 1, 2, 3 }));

        T* p = v.data();
        const size_t cap = v.capacity();
        VX_CHECK(v.reserve(10)); // smaller: no-op
        VX_CHECK(v.capacity() == cap);
        VX_CHECK(v.data() == p);

        vec e;
        VX_CHECK(e.reserve(0));
        VX_CHECK(e.data() == nullptr);
        VX_CHECK(e.reserve(8));
        VX_CHECK(e.capacity() >= 8);
        VX_CHECK(e.empty());

        if (v.max_size() < static_cast<size_t>(-1))
        {
            VX_CHECK(!v.reserve(v.max_size() + 1));
            VX_CHECK(eq(v, { 1, 2, 3 }));
        }
    }

    VX_SECTION("size queries");
    {
        vec v = make<T>({ 1, 2, 3 });
        VX_CHECK(!v.empty());
        VX_CHECK(v.size() == 3);
        VX_CHECK(v.size_bytes() == 3 * sizeof(T));
        VX_CHECK(v.size() <= v.max_size());
    }

    VX_SECTION("clear keeps capacity, clear_and_deallocate frees");
    {
        vec v = make<T>({ 1, 2, 3, 4 });
        const size_t cap = v.capacity();
        v.clear();
        VX_CHECK(v.empty());
        VX_CHECK(v.capacity() == cap);
        VX_CHECK(v.data() != nullptr);

        v.clear(); // clear twice is fine
        VX_CHECK(v.empty());

        VX_CHECK(v.emplace_back(5));
        VX_CHECK(eq(v, { 5 }));

        v.clear_and_deallocate();
        VX_CHECK(v.empty());
        VX_CHECK(v.capacity() == 0);
        VX_CHECK(v.data() == nullptr);
        v.clear_and_deallocate(); // idempotent
        VX_CHECK(v.emplace_back(6));
        VX_CHECK(eq(v, { 6 }));
    }

    VX_SECTION("shrink_to_fit");
    {
        vec v = make<T>({ 1, 2, 3 });
        VX_CHECK(v.reserve(64));
        VX_CHECK(v.shrink_to_fit());
        VX_CHECK(v.capacity() == v.size());
        VX_CHECK(eq(v, { 1, 2, 3 }));

        VX_CHECK(v.shrink_to_fit()); // already tight
        VX_CHECK(v.capacity() == 3);

        vec e;
        VX_CHECK(e.shrink_to_fit()); // never allocated
        VX_CHECK(e.capacity() == 0);
    }

    VX_SECTION("shrink_to_fit on emptied vector must stay valid");
    {
        // What happens to the buffer is implementation-defined here, but the
        // vector has to stay usable and leak nothing either way.
        vec v = make<T>({ 1, 2, 3, 4 });
        v.clear();
        (void)v.shrink_to_fit();
        VX_CHECK(v.empty());
        VX_CHECK(v.emplace_back(9));
        VX_CHECK(eq(v, { 9 }));
    }
}
TEST_BOTH(capacity)

template <typename T>
void test_resize()
{
    using vec = vx::vector<T>;

    vec v = make<T>({ 1, 2, 3 });

    VX_CHECK(v.resize(6, T(9)));
    VX_CHECK(eq(v, { 1, 2, 3, 9, 9, 9 }));

    VX_CHECK(v.resize(2, T(7)));
    VX_CHECK(eq(v, { 1, 2 }));

    VX_CHECK(v.resize(2, T(7))); // same size
    VX_CHECK(eq(v, { 1, 2 }));

    VX_CHECK(v.reserve(32));
    T* p = v.data();
    VX_CHECK(v.resize(10, T(5))); // within capacity: no realloc
    VX_CHECK(v.data() == p);
    VX_CHECK(eq(v, { 1, 2, 5, 5, 5, 5, 5, 5, 5, 5 }));

    VX_CHECK(v.resize(0));
    VX_CHECK(v.empty());

    VX_CHECK(v.resize(size_t(4)));
    VX_CHECK(v.size() == 4);

    vec e;
    VX_CHECK(e.resize(0));
    VX_CHECK(e.empty());
    VX_CHECK(e.resize(3, T(1)));
    VX_CHECK(eq(e, { 1, 1, 1 }));

    // growth beyond capacity keeps existing elements
    vec g = make<T>({ 1, 2, 3 });
    VX_CHECK(g.resize(100, T(8)));
    VX_CHECK(g.size() == 100);
    VX_CHECK(val(g[0]) == 1 && val(g[2]) == 3 && val(g[3]) == 8 && val(g[99]) == 8);
}
TEST_BOTH(resize)

//=========================================================================
// push_back / emplace_back / pop_back
//=========================================================================

template <typename T>
void test_push_pop()
{
    using vec = vx::vector<T>;

    VX_SECTION("push_back grows and keeps values");
    {
        vec v;
        int reallocs = 0;
        T* last = nullptr;
        for (int i = 0; i < 1000; ++i)
        {
            VX_CHECK(v.push_back(T(i)));
            if (v.data() != last)
            {
                ++reallocs;
                last = v.data();
            }
            VX_CHECK(v.capacity() >= v.size());
        }
        VX_CHECK(v.size() == 1000);
        for (int i = 0; i < 1000; ++i)
        {
            VX_CHECK(val(v[static_cast<size_t>(i)]) == i);
        }
        VX_CHECK(reallocs < 40); // amortized growth, not one realloc per push
    }

    VX_SECTION("push_back const T& and T&&");
    {
        vec v;
        const T c(1);
        auto r = v.push_back(c);
        VX_CHECK(r);
        VX_CHECK(val(*r.value()) == 1);
        r = v.push_back(T(2));
        VX_CHECK(r);
        VX_CHECK(r.value() == v.end() - 1);
        VX_CHECK(eq(v, { 1, 2 }));
    }

    VX_SECTION("pop_back");
    {
        vec v = make<T>({ 1, 2, 3 });
        v.pop_back();
        VX_CHECK(eq(v, { 1, 2 }));
        v.pop_back();
        v.pop_back();
        VX_CHECK(v.empty());
        v.pop_back(); // no-op on empty
        VX_CHECK(v.empty());

        vec e;
        e.pop_back();
        VX_CHECK(e.empty());
    }
}
TEST_BOTH(push_pop)

VX_TEST_CASE(emplace_back_multiarg)
{
    using movable_int = vx::test::movable_int;
    vx::vector<movable_int> v;
    movable_int a(1);
    VX_CHECK(v.push_back(std::move(a)));
    VX_CHECK(a.val == -1);
    VX_CHECK(v[0].val == 1);

    VX_CHECK(v.emplace_back());
    VX_CHECK(v.back().val == 0);
    VX_CHECK(v.emplace_back(2));
    VX_CHECK(v.back().val == 2);
    VX_CHECK(v.emplace_back(3, 2));
    VX_CHECK(v.back().val == 0x32);
    VX_CHECK(v.emplace_back(4, 3, 2));
    VX_CHECK(v.back().val == 0x432);
    VX_CHECK(v.size() == 5);
    VX_CHECK(v[0].val == 1); // survived reallocations
}

//=========================================================================
// insert (offset based, returns expected)
//=========================================================================

template <typename T>
void test_insert_offset()
{
    using vec = vx::vector<T>;

    VX_SECTION("single: begin / middle / end, with and without realloc");
    {
        vec v = make<T>({ 2, 4 });
        auto r = v.insert(size_t(0), T(1));
        VX_CHECK(r);
        VX_CHECK(r.value() == v.begin());
        VX_CHECK(eq(v, { 1, 2, 4 }));

        r = v.insert(size_t(2), T(3));
        VX_CHECK(r);
        VX_CHECK(r.value() == v.begin() + 2);
        VX_CHECK(eq(v, { 1, 2, 3, 4 }));

        const T c(5);
        r = v.insert(v.size(), c);
        VX_CHECK(r);
        VX_CHECK(eq(v, { 1, 2, 3, 4, 5 }));

        VX_CHECK(v.reserve(64)); // within capacity
        r = v.insert(size_t(1), T(9));
        VX_CHECK(r);
        VX_CHECK(eq(v, { 1, 9, 2, 3, 4, 5 }));
    }

    VX_SECTION("insert into empty");
    {
        vec e;
        VX_CHECK(e.insert(size_t(0), T(1)));
        VX_CHECK(eq(e, { 1 }));
    }

    VX_SECTION("count + value: spill / no spill / realloc");
    {
        // count > elements after pos
        vec a = make<T>({ 1, 2 });
        VX_CHECK(a.reserve(20));
        auto r = a.insert(size_t(1), size_t(5), T(9));
        VX_CHECK(r);
        VX_CHECK(eq(a, { 1, 9, 9, 9, 9, 9, 2 }));
        VX_CHECK(r.value() == a.begin() + 1);

        // count <= elements after pos
        vec b = make<T>({ 1, 2, 3, 4, 5, 6 });
        VX_CHECK(b.reserve(20));
        VX_CHECK(b.insert(size_t(2), size_t(2), T(9)));
        VX_CHECK(eq(b, { 1, 2, 9, 9, 3, 4, 5, 6 }));

        // exactly equal
        vec c = make<T>({ 1, 2, 3 });
        VX_CHECK(c.reserve(20));
        VX_CHECK(c.insert(size_t(1), size_t(2), T(9)));
        VX_CHECK(eq(c, { 1, 9, 9, 2, 3 }));

        // reallocation
        vec d = make<T>({ 1, 2, 3 });
        VX_CHECK(d.insert(size_t(1), size_t(10), T(7)));
        VX_CHECK(d.size() == 13);
        VX_CHECK(val(d[0]) == 1 && val(d[1]) == 7 && val(d[10]) == 7 && val(d[11]) == 2 && val(d[12]) == 3);

        // at end
        vec e = make<T>({ 1, 2 });
        VX_CHECK(e.insert(e.size(), size_t(3), T(8)));
        VX_CHECK(eq(e, { 1, 2, 8, 8, 8 }));
    }

    VX_SECTION("initializer_list / pointer range");
    {
        vec v = make<T>({ 1, 5 });
        std::initializer_list<T> il{ T(2), T(3), T(4) };
        VX_CHECK(v.insert(size_t(1), il));
        VX_CHECK(eq(v, { 1, 2, 3, 4, 5 }));

        T src[] = { T(7), T(8) };
        VX_CHECK(v.insert(size_t(0), src, src + 2));
        VX_CHECK(eq(v, { 7, 8, 1, 2, 3, 4, 5 }));

        VX_CHECK(v.insert(v.size(), src, src + 2));
        VX_CHECK(eq(v, { 7, 8, 1, 2, 3, 4, 5, 7, 8 }));
    }

    VX_SECTION("zero-count inserts are no-ops");
    {
        vec v = make<T>({ 1, 2, 3 });
        VX_CHECK(v.reserve(16));

        auto r = v.insert(size_t(1), size_t(0), T(9));
        VX_CHECK(r);
        VX_CHECK(eq(v, { 1, 2, 3 }));

        std::initializer_list<T> none{};
        VX_CHECK(v.insert(size_t(1), none));
        VX_CHECK(eq(v, { 1, 2, 3 }));

        T src[] = { T(1) };
        VX_CHECK(v.insert(size_t(2), src, src));
        VX_CHECK(eq(v, { 1, 2, 3 }));

        VX_CHECK(v.insert(v.size(), size_t(0), T(9)));
        VX_CHECK(eq(v, { 1, 2, 3 }));

        vec full = make<T>({ 1, 2, 3 }); // capacity == size
        VX_CHECK(full.insert(size_t(1), size_t(0), T(9)));
        VX_CHECK(eq(full, { 1, 2, 3 }));

        vec e;
        VX_CHECK(e.insert(size_t(0), size_t(0), T(1)));
        VX_CHECK(e.empty());
    }

    VX_SECTION("out of range offsets");
    {
        vec v = make<T>({ 1, 2, 3 });
        const T c(1);

        auto r1 = v.insert(size_t(4), c);
        VX_CHECK(!r1);
        VX_CHECK_EXPECTED_ERROR(r1, vx::err::out_of_range);

        auto r2 = v.insert(size_t(99), T(1));
        VX_CHECK(!r2);
        VX_CHECK_EXPECTED_ERROR(r2, vx::err::out_of_range);

        auto r3 = v.insert(size_t(4), size_t(2), c);
        VX_CHECK(!r3);
        VX_CHECK_EXPECTED_ERROR(r3, vx::err::out_of_range);

        auto r3b = v.insert(size_t(4), size_t(0), c); // still out of range with count 0
        VX_CHECK(!r3b);

        std::initializer_list<T> il{ T(1) };
        auto r4 = v.insert(size_t(4), il);
        VX_CHECK(!r4);
        VX_CHECK_EXPECTED_ERROR(r4, vx::err::out_of_range);

        T src[] = { T(1), T(2) };
        auto r5 = v.insert(size_t(4), src, src + 2);
        VX_CHECK(!r5);
        VX_CHECK_EXPECTED_ERROR(r5, vx::err::out_of_range);

        VX_CHECK(eq(v, { 1, 2, 3 })); // untouched
    }
}
TEST_BOTH(insert_offset)

//=========================================================================
// insert / emplace (iterator based)
//=========================================================================

template <typename T>
void test_insert_iterator()
{
    using vec = vx::vector<T>;

    VX_SECTION("single");
    {
        vec v = make<T>({ 2, 4 });
        auto it = v.insert(v.begin(), T(1));
        VX_CHECK(it == v.begin());
        it = v.insert(v.begin() + 2, T(3));
        VX_CHECK(val(*it) == 3);
        const T c(5);
        it = v.insert(v.end(), c);
        VX_CHECK(val(*it) == 5);
        VX_CHECK(eq(v, { 1, 2, 3, 4, 5 }));
    }

    VX_SECTION("count");
    {
        vec v = make<T>({ 1, 2, 3 });
        VX_CHECK(v.reserve(30));
        auto it = v.insert(v.begin() + 1, size_t(6), T(9));
        VX_CHECK(it == v.begin() + 1);
        VX_CHECK(eq(v, { 1, 9, 9, 9, 9, 9, 9, 2, 3 }));

        vec w = make<T>({ 1, 2 }); // realloc path
        it = w.insert(w.begin() + 1, size_t(4), T(7));
        VX_CHECK(val(*it) == 7);
        VX_CHECK(eq(w, { 1, 7, 7, 7, 7, 2 }));

        vec z;
        it = z.insert(z.begin(), size_t(0), T(1));
        VX_CHECK(z.empty());
    }

    VX_SECTION("initializer_list and ranges");
    {
        vec v = make<T>({ 1, 5 });
        std::initializer_list<T> il{ T(2), T(3), T(4) };
        v.insert(v.begin() + 1, il);
        VX_CHECK(eq(v, { 1, 2, 3, 4, 5 }));

        T src[] = { T(8), T(9) };
        v.insert(v.end(), src, src + 2);
        VX_CHECK(eq(v, { 1, 2, 3, 4, 5, 8, 9 }));

        vec other = make<T>({ 6, 7 });
        v.insert(v.begin() + 5, other.begin(), other.end());
        VX_CHECK(eq(v, { 1, 2, 3, 4, 5, 6, 7, 8, 9 }));

        std::initializer_list<T> none{};
        v.insert(v.begin() + 1, none);
        VX_CHECK(v.size() == 9);
    }

    VX_SECTION("emplace");
    {
        vec v = make<T>({ 1, 3 });
        auto r = v.emplace(size_t(1), 2);
        VX_CHECK(r);
        VX_CHECK(eq(v, { 1, 2, 3 }));
        r = v.emplace(v.size(), 4);
        VX_CHECK(r);
        VX_CHECK(eq(v, { 1, 2, 3, 4 }));

        auto bad = v.emplace(size_t(99), 1);
        VX_CHECK(!bad);
        VX_CHECK_EXPECTED_ERROR(bad, vx::err::out_of_range);

        auto it = v.emplace(v.begin(), 0);
        VX_CHECK(val(*it) == 0);
        VX_CHECK(eq(v, { 0, 1, 2, 3, 4 }));
    }
}
TEST_BOTH(insert_iterator)

VX_TEST_CASE(emplace_multiarg_insert)
{
    using movable_int = vx::test::movable_int;
    vx::vector<movable_int> v;
    movable_int a(1);
    v.push_back(std::move(a));

    v.emplace(v.begin() + 1);
    VX_CHECK(v[1].val == 0);
    v.emplace(v.begin() + 1, 2);
    VX_CHECK(v[1].val == 2);
    v.emplace(v.begin() + 1, 3, 2);
    VX_CHECK(v[1].val == 0x32);
    v.emplace(v.begin() + 1, 4, 3, 2);
    VX_CHECK(v[1].val == 0x432);
    VX_CHECK(v[0].val == 1);
    VX_CHECK(v.size() == 5);

    movable_int m(7);
    v.insert(v.begin(), std::move(m));
    VX_CHECK(m.val == -1);
    VX_CHECK(v[0].val == 7);
}

//=========================================================================
// erase
//=========================================================================

template <typename T>
void test_erase()
{
    using vec = vx::vector<T>;

    VX_SECTION("offset: single");
    {
        vec v = make<T>({ 1, 2, 3, 4, 5 });
        auto r = v.erase(size_t(0));
        VX_CHECK(r);
        VX_CHECK(r.value() == v.begin());
        VX_CHECK(eq(v, { 2, 3, 4, 5 }));
        r = v.erase(size_t(1));
        VX_CHECK(val(*r.value()) == 4);
        VX_CHECK(eq(v, { 2, 4, 5 }));
        r = v.erase(size_t(2));
        VX_CHECK(r);
        VX_CHECK(r.value() == v.end());
        VX_CHECK(eq(v, { 2, 4 }));

        auto bad = v.erase(size_t(2));
        VX_CHECK(!bad);
        VX_CHECK_EXPECTED_ERROR(bad, vx::err::out_of_range);

        vec e;
        VX_CHECK(!e.erase(size_t(0)));
    }

    VX_SECTION("offset: range");
    {
        vec v = make<T>({ 1, 2, 3, 4, 5, 6 });
        auto r = v.erase(size_t(1), size_t(2));
        VX_CHECK(r);
        VX_CHECK(val(*r.value()) == 4);
        VX_CHECK(eq(v, { 1, 4, 5, 6 }));

        VX_CHECK(v.erase(size_t(0), size_t(0))); // empty range
        VX_CHECK(eq(v, { 1, 4, 5, 6 }));

        VX_CHECK(v.erase(v.size(), size_t(0))); // empty range at end
        VX_CHECK(eq(v, { 1, 4, 5, 6 }));

        VX_CHECK(v.erase(size_t(2), size_t(2))); // tail
        VX_CHECK(eq(v, { 1, 4 }));

        auto bad1 = v.erase(size_t(1), size_t(2));
        VX_CHECK(!bad1);
        VX_CHECK_EXPECTED_ERROR(bad1, vx::err::out_of_range);
        auto bad2 = v.erase(size_t(3), size_t(0));
        VX_CHECK(!bad2);
        auto bad3 = v.erase(v.size(), size_t(1));
        VX_CHECK(!bad3);
        VX_CHECK(eq(v, { 1, 4 }));

        VX_CHECK(v.erase(size_t(0), v.size())); // everything
        VX_CHECK(v.empty());

        vec e;
        VX_CHECK(e.erase(size_t(0), size_t(0)));
    }

    VX_SECTION("iterator based");
    {
        vec v = make<T>({ 1, 2, 3, 4, 5, 6 });
        auto it = v.erase(v.begin());
        VX_CHECK(val(*it) == 2);
        it = v.erase(v.begin() + 2);
        VX_CHECK(val(*it) == 5);
        VX_CHECK(eq(v, { 2, 3, 5, 6 }));
        it = v.erase(v.end() - 1);
        VX_CHECK(it == v.end());
        VX_CHECK(eq(v, { 2, 3, 5 }));

        it = v.erase(v.begin() + 1, v.end());
        VX_CHECK(it == v.end());
        VX_CHECK(eq(v, { 2 }));

        it = v.erase(v.begin(), v.begin()); // empty range
        VX_CHECK(eq(v, { 2 }));
        it = v.erase(v.end(), v.end()); // empty range at end
        VX_CHECK(eq(v, { 2 }));

        v.erase(v.begin(), v.end());
        VX_CHECK(v.empty());
    }
}
TEST_BOTH(erase)

//=========================================================================
// swap / comparison
//=========================================================================

template <typename T>
void test_swap_compare()
{
    using vec = vx::vector<T>;

    VX_SECTION("swap");
    {
        vec a = make<T>({ 1, 2, 3 });
        vec b = make<T>({ 9 });
        T* pa = a.data();
        T* pb = b.data();

        a.swap(b);
        VX_CHECK(eq(a, { 9 }));
        VX_CHECK(eq(b, { 1, 2, 3 }));
        VX_CHECK(a.data() == pb && b.data() == pa);

        vx::swap(a, b);
        VX_CHECK(eq(a, { 1, 2, 3 }));

        vec e;
        a.swap(e);
        VX_CHECK(a.empty());
        VX_CHECK(a.data() == nullptr);
        VX_CHECK(eq(e, { 1, 2, 3 }));
    }

    VX_SECTION("comparison");
    {
        vec a = make<T>({ 1, 2, 3 });
        vec b = make<T>({ 1, 2, 3 });
        vec c = make<T>({ 1, 2, 4 });
        vec d = make<T>({ 1, 2 });
        vec e;

        VX_CHECK(a == b);
        VX_CHECK(!(a != b));
        VX_CHECK(a != c);
        VX_CHECK(a < c);
        VX_CHECK(c > a);
        VX_CHECK(a <= b && a >= b);
        VX_CHECK(a <= c && c >= a);
        VX_CHECK(!(c < a));
        VX_CHECK(d < a); // prefix is smaller
        VX_CHECK(a > d);
        VX_CHECK(e < a);
        VX_CHECK(e == e);
        VX_CHECK(!(e < e));
        VX_CHECK(a != d);
    }

    VX_SECTION("multi-byte ordering (catches memcmp on little endian)");
    {
        vec big = make<T>({ 256 });
        vec small = make<T>({ 1 });
        VX_CHECK(big > small);
        VX_CHECK(small < big);
        vec x = make<T>({ 1, 256 });
        vec y = make<T>({ 1, 2 });
        VX_CHECK(x > y);
        VX_CHECK(make<T>({ -1 }) < make<T>({ 1 }));
    }
}
TEST_BOTH(swap_compare)

VX_TEST_CASE(compare_floating_point)
{
    // std::vector semantics: element-wise ==, so NaN != NaN and -0.0 == +0.0.
    // Fails if range::compare uses memcmp for floating point types.
    const double nan = std::numeric_limits<double>::quiet_NaN();
    vx::vector<double> a{ nan };
    VX_CHECK(!(a == a));
    vx::vector<double> z1{ 0.0 };
    vx::vector<double> z2{ -0.0 };
    VX_CHECK(z1 == z2);
    vx::vector<double> s{ 1.0, 2.0 };
    vx::vector<double> t{ 1.0, 3.0 };
    VX_CHECK(s < t);
}

//=========================================================================
// non-pointer (generic) iterators
//=========================================================================

// Requires type_traits::is_iterator to accept std::list iterators.
template <typename T>
void test_generic_iterators()
{
    using vec = vx::vector<T>;
    std::list<int> l{ 1, 2, 3 };
    std::list<int> none;

    VX_SECTION("construct");
    {
        vec v(l.begin(), l.end());
        VX_CHECK(eq(v, { 1, 2, 3 }));
        vec e(none.begin(), none.end());
        VX_CHECK(e.empty());
    }

    VX_SECTION("assign shorter / longer / empty");
    {
        vec v = make<T>({ 9, 9, 9, 9, 9 });
        VX_CHECK(v.assign(l.begin(), l.end()));
        VX_CHECK(eq(v, { 1, 2, 3 }));

        vec w = make<T>({ 9 });
        VX_CHECK(w.assign(l.begin(), l.end()));
        VX_CHECK(eq(w, { 1, 2, 3 }));

        VX_CHECK(w.assign(none.begin(), none.end()));
        VX_CHECK(w.empty());
    }

    VX_SECTION("insert by offset");
    {
        vec v = make<T>({ 10, 20 });
        VX_CHECK(v.insert(size_t(0), l.begin(), l.end()));
        VX_CHECK(eq(v, { 1, 2, 3, 10, 20 }));
        auto r = v.insert(size_t(4), l.begin(), l.end());
        VX_CHECK(r);
        VX_CHECK(r.value() == v.begin() + 4);
        VX_CHECK(eq(v, { 1, 2, 3, 10, 1, 2, 3, 20 }));
        VX_CHECK(v.insert(v.size(), l.begin(), l.end()));
        VX_CHECK(eq(v, { 1, 2, 3, 10, 1, 2, 3, 20, 1, 2, 3 }));

        VX_CHECK(v.insert(size_t(2), none.begin(), none.end()));
        VX_CHECK(v.size() == 11);
    }

    VX_SECTION("insert by offset: out of range must not modify");
    {
        vec v = make<T>({ 10, 20 });
        auto bad = v.insert(size_t(5), l.begin(), l.end());
        VX_CHECK(!bad);
        VX_CHECK_EXPECTED_ERROR(bad, vx::err::out_of_range);
        VX_CHECK(eq(v, { 10, 20 }));
    }

    VX_SECTION("insert by iterator");
    {
        vec v = make<T>({ 10, 20 });
        auto it = v.insert(v.begin() + 1, l.begin(), l.end());
        VX_CHECK(it == v.begin() + 1);
        VX_CHECK(eq(v, { 10, 1, 2, 3, 20 }));
        v.insert(v.end(), l.begin(), l.end());
        VX_CHECK(eq(v, { 10, 1, 2, 3, 20, 1, 2, 3 }));
    }
}
TEST_BOTH(generic_iterators)

//=========================================================================
// unsafe_ops
//=========================================================================

template <typename T>
void test_unsafe()
{
    using vec = vx::vector<T>;

    VX_SECTION("push_back / emplace_back assume_capacity");
    {
        vec v;
        VX_CHECK(v.reserve(8));
        T* base = v.data();

        for (int i = 1; i <= 3; ++i)
        {
            v.unsafe().push_back_assume_capacity(T(i));
        }
        const T c(4);
        v.unsafe().push_back_assume_capacity(c);
        auto it = v.unsafe().emplace_back_assume_capacity(5);
        VX_CHECK(it == v.end() - 1);
        VX_CHECK(v.data() == base); // never reallocated
        VX_CHECK(eq(v, { 1, 2, 3, 4, 5 }));
    }

    VX_SECTION("insert_assume_capacity: end, begin, middle");
    {
        vec v = make<T>({ 1, 2, 3, 4, 5 });
        VX_CHECK(v.reserve(8));
        T* base = v.data();

        auto it = v.unsafe().insert_assume_capacity(v.size(), T(6));
        VX_CHECK(val(*it) == 6);
        it = v.unsafe().insert_assume_capacity(size_t(0), T(0));
        VX_CHECK(val(*it) == 0);
        it = v.unsafe().insert_assume_capacity(size_t(3), T(99));
        VX_CHECK(val(*it) == 99);
        VX_CHECK(v.data() == base);
        VX_CHECK(v.size() == v.capacity());
        VX_CHECK(eq(v, { 0, 1, 2, 99, 3, 4, 5, 6 }));
    }

    VX_SECTION("append_assume_capacity");
    {
        vec v = make<T>({ 1, 2 });
        VX_CHECK(v.reserve(10));
        T* base = v.data();
        T src[] = { T(3), T(4), T(5) };

        auto it = v.unsafe().append_assume_capacity(src, 3);
        VX_CHECK(it == v.begin() + 2);
        VX_CHECK(eq(v, { 1, 2, 3, 4, 5 }));

        v.unsafe().append_assume_capacity(src, 0);
        VX_CHECK(v.size() == 5);
        VX_CHECK(v.data() == base);

        vec e;
        VX_CHECK(e.reserve(4));
        e.unsafe().append_assume_capacity(src, 3);
        VX_CHECK(eq(e, { 3, 4, 5 }));
    }

    VX_SECTION("erase_unordered_assume_in_range");
    {
        vec v = make<T>({ 1, 2, 3, 4, 5 });
        v.unsafe().erase_unordered_assume_in_range(1);
        VX_CHECK(eq(v, { 1, 5, 3, 4 }));
        v.unsafe().erase_unordered_assume_in_range(3); // last element
        VX_CHECK(eq(v, { 1, 5, 3 }));
        v.unsafe().erase_unordered_assume_in_range(0);
        VX_CHECK(eq(v, { 3, 5 }));
        v.unsafe().erase_unordered_assume_in_range(1);
        VX_CHECK(eq(v, { 3 }));
        v.unsafe().erase_unordered_assume_in_range(0); // only element
        VX_CHECK(v.empty());
    }

    VX_SECTION("pop_back_assume_nonempty");
    {
        vec v = make<T>({ 1, 2, 3 });
        v.unsafe().pop_back_assume_nonempty();
        VX_CHECK(eq(v, { 1, 2 }));
        v.unsafe().pop_back_assume_nonempty();
        v.unsafe().pop_back_assume_nonempty();
        VX_CHECK(v.empty());
    }

    VX_SECTION("set_size_assume_constructed");
    {
        vec v = make<T>({ 1, 2 });
        VX_CHECK(v.reserve(6));

        for (int i = 3; i <= 5; ++i)
        {
            new (v.data() + v.size()) T(i);
            v.unsafe().set_size_assume_constructed(v.size() + 1);
        }
        VX_CHECK(eq(v, { 1, 2, 3, 4, 5 }));

        (v.data() + v.size() - 1)->~T(); // manual destroy, then publish
        v.unsafe().set_size_assume_constructed(v.size() - 1);
        VX_CHECK(eq(v, { 1, 2, 3, 4 }));
    }

    VX_SECTION("acquire / release");
    {
        typename vec::allocator_type al;

        T* p = al.allocate(3);
        for (int i = 0; i < 3; ++i)
        {
            new (p + i) T(i + 1);
        }

        vec v = make<T>({ 9, 9 }); // existing buffer must be freed
        VX_CHECK(v.unsafe().acquire(p, 3));
        VX_CHECK(v.data() == p);
        VX_CHECK(v.capacity() == 3);
        VX_CHECK(eq(v, { 1, 2, 3 }));

        T* q = v.unsafe().release();
        VX_CHECK(q == p);
        VX_CHECK(v.empty());
        VX_CHECK(v.data() == nullptr);
        VX_CHECK(v.capacity() == 0);

        for (int i = 0; i < 3; ++i)
        {
            q[i].~T();
        }
        al.deallocate(q, 3);

        VX_CHECK(v.emplace_back(1)); // still usable after release
        VX_CHECK(eq(v, { 1 }));
    }
}
TEST_BOTH(unsafe)

//=========================================================================
// move-only type
//=========================================================================

VX_TEST_CASE(move_only_type)
{
    using mi = vx::test::movable_int;
    vx::vector<mi> v;

    for (int i = 0; i < 50; ++i)
    {
        VX_CHECK(v.emplace_back(i)); // growth must move, not copy
    }
    VX_CHECK(v.insert(size_t(10), mi(100))); // middle insert: move-assign path
    VX_CHECK(v.insert(size_t(0), mi(200)));
    VX_CHECK(v.insert(v.size(), mi(300)));
    VX_CHECK(v.size() == 53);
    VX_CHECK(v.erase(size_t(5), size_t(3)));
    VX_CHECK(v.erase(size_t(0)));
    VX_CHECK(v.size() == 49);
    VX_CHECK(v[0].val == 0);

    v.pop_back();
    VX_CHECK(v.size() == 48);
    VX_CHECK(v.resize(size_t(60))); // default-constructs
    VX_CHECK(v.size() == 60);
    VX_CHECK(v.back().val == 0);

    VX_CHECK(v.shrink_to_fit()); // relocation by move

    {
        VX_DISABLE_USE_AFTER_MOVE_WARNING();

        vx::vector<mi> w(std::move(v));
        vx::vector<mi> x;
        x = std::move(w);
        VX_CHECK(x.size() == 60);
        x.swap(v);
        VX_CHECK(v.size() == 60 && x.empty());

        VX_DISABLE_WARNING_POP();
    }

    VX_CHECK(v.reserve(200));
    v.unsafe().push_back_assume_capacity(mi(7));
    VX_CHECK(v.size() == 61);
    v.unsafe().erase_unordered_assume_in_range(0);
    VX_CHECK(v.size() == 60);
    VX_CHECK(v[0].val == 7); // last element moved into the hole
}

//=========================================================================
// lifetime
//=========================================================================

VX_TEST_CASE(lifetime_tracking)
{
    tracked::reset();
    {
        vx::vector<tracked> v;
        for (int i = 0; i < 100; ++i)
        {
            VX_CHECK(v.emplace_back(i));
            VX_CHECK(tracked::live == static_cast<int>(v.size()));
        }
        // growth should relocate by moving, not copying
        VX_CHECK(tracked::copies == 0);

        v.erase(size_t(10), size_t(20));
        VX_CHECK(tracked::live == 80);
        v.insert(size_t(5), size_t(7), tracked(1));
        VX_CHECK(tracked::live == 87);
        v.pop_back();
        VX_CHECK(tracked::live == 86);
        VX_CHECK(v.resize(10, tracked(0)));
        VX_CHECK(tracked::live == 10);
        v.clear();
        VX_CHECK(tracked::live == 0);

        VX_CHECK(v.resize(size_t(5)));
        VX_CHECK(tracked::live == 5);
        for (size_t i = 0; i < 5; ++i)
        {
            VX_CHECK(v[i].v == 0); // default constructed
        }

        vx::vector<tracked> w(v);
        VX_CHECK(tracked::live == 10);
        VX_CHECK(w == v);
        w = vx::vector<tracked>(size_t(2), tracked(3));
        VX_CHECK(tracked::live == 7);
        v.clear_and_deallocate();
        VX_CHECK(tracked::live == 2);
    }
    VX_CHECK(tracked::live == 0);
}

//=========================================================================
// allocator
//=========================================================================

using talloc = vx::test::tracking_allocator<int>;
using tvec = vx::vector<int, talloc>;
using vx::test::allocator_stats;

VX_TEST_CASE(tracking_allocator_balance)
{
    // NOTE: if this fails with allocate_calls != deallocate_calls (off by one),
    // the vector is calling deallocate(nullptr, 0) on the first growth of an
    // empty vector. Guard the deallocate in reallocate / resize_reallocate /
    // insert_reallocate / assign_from with `if (ptr)`.
    allocator_stats st;
    {
        tvec v(talloc(1, &st));
        VX_CHECK(v.reserve(100));
        VX_CHECK(st.allocate_calls == 1);

        for (int i = 0; i < 100; ++i)
        {
            VX_CHECK(v.push_back(i));
        }
        VX_CHECK(st.allocate_calls == 1); // no growth inside reserved capacity

        for (int i = 0; i < 1000; ++i)
        {
            VX_CHECK(v.push_back(i));
        }
        VX_CHECK(v.insert(size_t(5), size_t(50), 1));
        VX_CHECK(v.erase(size_t(0), size_t(20)));
        VX_CHECK(v.resize(5000, 2));
        VX_CHECK(v.shrink_to_fit());

        tvec c(v);
        c = v;
        tvec m(std::move(c));
        VX_CHECK(v.assign(size_t(10), 3));
        v.clear_and_deallocate();
    }
    VX_CHECK(st.live_allocations == 0);
    VX_CHECK(st.allocate_calls == st.deallocate_calls);
    VX_CHECK(st.reallocate_calls == 0); // vector never calls reallocate()
}

VX_TEST_CASE(stateful_allocator_propagation)
{
    allocator_stats s1, s2;
    {
        tvec a(size_t(4), 7, talloc(1, &s1));

        tvec b(a); // copy ctor keeps the source's allocator
        VX_CHECK(b.get_allocator().id() == 1);

        tvec c(a, talloc(2, &s2)); // explicit allocator wins
        VX_CHECK(c.get_allocator().id() == 2);
        VX_CHECK(c == a);

        tvec d(std::move(b)); // move ctor takes the allocator
        VX_CHECK(d.get_allocator().id() == 1);

        tvec e(size_t(3), 9, talloc(2, &s2));
        e = a; // copy assign: does NOT propagate, buffer stays with allocator 2
        VX_CHECK(e.get_allocator().id() == 2);
        VX_CHECK(e == a);

        tvec f(size_t(2), 5, talloc(2, &s2));
        f = std::move(d); // move assign: takes allocator 1, frees old buffer via allocator 2
        VX_CHECK(f.get_allocator().id() == 1);
        VX_CHECK(s2.live_allocations == 2); // c and e still alive

        tvec g(size_t(1), 1, talloc(1, &s1));
        tvec h(size_t(1), 2, talloc(2, &s2));
        g.swap(h); // swap exchanges allocators together with buffers
        VX_CHECK(g.get_allocator().id() == 2 && g[0] == 2);
        VX_CHECK(h.get_allocator().id() == 1 && h[0] == 1);
    }
    VX_CHECK(s1.live_allocations == 0);
    VX_CHECK(s2.live_allocations == 0);
}

//=========================================================================
// randomized comparison against std::vector
//=========================================================================

template <typename T>
void test_stress()
{
    for (unsigned seed = 1; seed <= 8; ++seed)
    {
        vx::vector<T> v;
        std::vector<int> m;
        unsigned s = seed * 7919u;

        auto rnd = [&](unsigned n)
        {
            s = s * 1664525u + 1013904223u;
            return (s >> 8) % n;
        };

        for (int iter = 0; iter < 2000; ++iter)
        {
            const int x = static_cast<int>(rnd(1000));
            switch (rnd(10))
            {
                case 0:
                {
                    VX_CHECK(v.push_back(T(x)));
                    m.push_back(x);
                    break;
                }
                case 1:
                {
                    const size_t off = rnd(static_cast<unsigned>(m.size()) + 1);
                    VX_CHECK(v.insert(off, T(x)));
                    m.insert(m.begin() + static_cast<std::ptrdiff_t>(off), x);
                    break;
                }
                case 2:
                {
                    const size_t off = rnd(static_cast<unsigned>(m.size()) + 1);
                    const size_t cnt = rnd(6); // includes 0
                    VX_CHECK(v.insert(off, cnt, T(x)));
                    m.insert(m.begin() + static_cast<std::ptrdiff_t>(off), cnt, x);
                    break;
                }
                case 3:
                {
                    if (!m.empty())
                    {
                        const size_t off = rnd(static_cast<unsigned>(m.size()));
                        VX_CHECK(v.erase(off));
                        m.erase(m.begin() + static_cast<std::ptrdiff_t>(off));
                    }
                    break;
                }
                case 4:
                {
                    const size_t off = rnd(static_cast<unsigned>(m.size()) + 1);
                    const size_t cnt = rnd(static_cast<unsigned>(m.size() - off) + 1);
                    VX_CHECK(v.erase(off, cnt));
                    m.erase(
                        m.begin() + static_cast<std::ptrdiff_t>(off),
                        m.begin() + static_cast<std::ptrdiff_t>(off + cnt));
                    break;
                }
                case 5:
                {
                    v.pop_back();
                    if (!m.empty())
                    {
                        m.pop_back();
                    }
                    break;
                }
                case 6:
                {
                    const size_t n = rnd(40);
                    VX_CHECK(v.resize(n, T(x)));
                    m.resize(n, x);
                    break;
                }
                case 7:
                {
                    VX_CHECK(v.reserve(rnd(64)));
                    break;
                }
                case 8:
                {
                    (void)v.shrink_to_fit();
                    break;
                }
                case 9:
                {
                    const size_t cnt = rnd(5);
                    std::vector<int> add;
                    for (size_t i = 0; i < cnt; ++i)
                    {
                        add.push_back(static_cast<int>(rnd(1000)));
                    }
                    std::list<int> l(add.begin(), add.end()); // generic iterator path
                    const size_t off = rnd(static_cast<unsigned>(m.size()) + 1);
                    VX_CHECK(v.insert(off, l.begin(), l.end()));
                    m.insert(m.begin() + static_cast<std::ptrdiff_t>(off), add.begin(), add.end());
                    break;
                }
            }

            VX_CHECK(eq_model(v, m));
            VX_CHECK(v.capacity() >= v.size());
        }
    }
}
TEST_BOTH(stress)

#endif // !defined(ONLY_CONTAINER_TESTS)

//=========================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
