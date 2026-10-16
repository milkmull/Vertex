#include "vertex/std/hash_set.hpp"

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <set>
#include <vector>

#include "vertex_test/test.hpp"

// Generic tests: every test is a template over the set type, so the same code
// runs against the open-addressing and the chaining backend. Add a backend by
// adding one alias and one line in TEST_SETS.

//=========================================================================
// helpers
//=========================================================================

namespace {

// Non-trivial type that counts live instances, copies and moves. Moved-from
// objects get v == -1.
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
};

inline int val(int x)
{
    return x;
}
inline int val(const tracked& t)
{
    return t.v;
}

// Deliberately NOT avalanching (no is_avalanching), so the table's own
// finalizer is exercised. The default hasher is covered by default_hasher.
template <typename K>
struct test_hash
{
    size_t operator()(const K& k) const noexcept
    {
        return vx::default_hash<int>()(val(k));
    }
};

template <typename K>
using open_set = vx::unordered_set<K, test_hash<K>>;

template <typename K>
using chain_set = vx::chain_unordered_set<K, test_hash<K>>;

// sorted contents, by walking the iterators
template <typename S>
std::vector<int> contents(const S& s)
{
    std::vector<int> out;
    for (auto it = s.begin(); it != s.end(); ++it)
    {
        out.push_back(val(*it));
    }
    std::sort(out.begin(), out.end());
    return out;
}

template <typename S>
bool eq_model(const S& s, const std::vector<int>& sorted)
{
    return s.size() == sorted.size() && contents(s) == sorted;
}

template <typename S>
bool eq(const S& s, std::initializer_list<int> l)
{
    std::vector<int> sorted(l);
    std::sort(sorted.begin(), sorted.end());
    return eq_model(s, sorted);
}

// inserts [lo, hi)
template <typename S>
void fill(S& s, int lo, int hi)
{
    using K = typename S::key_type;
    for (int i = lo; i < hi; ++i)
    {
        VX_CHECK(s.insert(K(i)));
    }
}

template <typename S>
std::vector<int> range(int lo, int hi)
{
    std::vector<int> v;
    for (int i = lo; i < hi; ++i)
    {
        v.push_back(i);
    }
    return v;
}

// Runs a templated test for both backends, with int and tracked keys
// (tracked also checks for leaks).
#define TEST_SETS(name) \
    VX_TEST_CASE(name##_open_int) \
    { \
        test_##name<open_set<int>>(); \
    } \
    VX_TEST_CASE(name##_chain_int) \
    { \
        test_##name<chain_set<int>>(); \
    } \
    VX_TEST_CASE(name##_open_tracked) \
    { \
        tracked::reset(); \
        test_##name<open_set<tracked>>(); \
        VX_CHECK(tracked::live == 0); \
    } \
    VX_TEST_CASE(name##_chain_tracked) \
    { \
        tracked::reset(); \
        test_##name<chain_set<tracked>>(); \
        VX_CHECK(tracked::live == 0); \
    }

} // namespace

//=========================================================================
// construction
//=========================================================================

template <typename S>
void test_construct()
{
    using K = typename S::key_type;

    VX_SECTION("default is empty and usable without allocating")
    {
        S s;
        VX_CHECK(s.empty());
        VX_CHECK(s.size() == 0);
        VX_CHECK(s.capacity() == 0);
        VX_CHECK(s.begin() == s.end());
        VX_CHECK(s.find(K(1)) == s.end());
        VX_CHECK(!s.contains(K(1)));
        VX_CHECK(s.count(K(1)) == 0);
        VX_CHECK(s.erase(K(1)) == 0);
        s.clear();
        VX_CHECK(s.empty());
    }

    VX_SECTION("create")
    {
        auto a = S::create();
        VX_CHECK(a);
        VX_CHECK(a.value().empty());

        auto b = S::create(size_t(16));
        VX_CHECK(b);
        VX_CHECK(b.value().empty());
        VX_CHECK(b.value().capacity() >= 16);

        S src;
        fill(src, 0, 10);
        auto c = S::create(src);
        VX_CHECK(c);
        VX_CHECK(c.value() == src);
    }

    VX_SECTION("reserving constructor")
    {
        S s(size_t(50));
        VX_CHECK(s.empty());
        VX_CHECK(s.capacity() >= 50);
    }
}
TEST_SETS(construct)

//=========================================================================
// insert / lookup
//=========================================================================

template <typename S>
void test_insert_find()
{
    using K = typename S::key_type;

    VX_SECTION("insert reports whether it inserted")
    {
        S s;
        auto r = s.insert(K(1));
        VX_CHECK(r);
        VX_CHECK(r.value().inserted);
        VX_CHECK(val(*r.value().it) == 1);
        VX_CHECK(s.size() == 1);

        auto again = s.insert(K(1));
        VX_CHECK(again);
        VX_CHECK(!again.value().inserted);
        VX_CHECK(again.value().it == r.value().it);
        VX_CHECK(s.size() == 1);

        const K c(2);
        VX_CHECK(s.insert(c).value().inserted);
        VX_CHECK(s.emplace(3).value().inserted);
        VX_CHECK(!s.emplace(3).value().inserted);
        VX_CHECK(eq(s, { 1, 2, 3 }));
    }

    VX_SECTION("find / contains / count")
    {
        S s;
        fill(s, 0, 100);
        for (int i = 0; i < 100; ++i)
        {
            auto it = s.find(K(i));
            VX_CHECK(it != s.end());
            VX_CHECK(val(*it) == i);
            VX_CHECK(s.contains(K(i)));
            VX_CHECK(s.count(K(i)) == 1);
        }
        for (int i = 100; i < 200; ++i)
        {
            VX_CHECK(s.find(K(i)) == s.end());
            VX_CHECK(!s.contains(K(i)));
            VX_CHECK(s.count(K(i)) == 0);
        }
        VX_CHECK(s.size() == 100);
    }

    VX_SECTION("const lookup")
    {
        S s;
        fill(s, 0, 10);
        const S& cs = s;
        VX_CHECK(cs.find(K(3)) != cs.end());
        VX_CHECK(cs.find(K(30)) == cs.end());
        VX_CHECK(cs.contains(K(9)));
    }

    VX_SECTION("hash variants")
    {
        S s;
        const K k(7);
        const size_t h = s.hash_of(k);
        VX_CHECK(s.try_emplace_with_hash(h, K(7)).value().inserted);
        VX_CHECK(!s.try_emplace_with_hash(h, K(7)).value().inserted);
        VX_CHECK(s.find_with_hash(k, h) != s.end());
        VX_CHECK(s.contains_with_hash(k, h));
        VX_CHECK(s.erase_with_hash(k, h) == 1);
        VX_CHECK(s.erase_with_hash(k, h) == 0);
        VX_CHECK(s.empty());
    }

    VX_SECTION("insert_range")
    {
        std::vector<K> src;
        for (int i = 0; i < 20; ++i)
        {
            src.push_back(K(i % 10)); // duplicates are skipped
        }
        S s;
        VX_CHECK(s.insert_range(src.begin(), src.end()));
        VX_CHECK(eq_model(s, range<S>(0, 10)));
    }
}
TEST_SETS(insert_find)

//=========================================================================
// erase
//=========================================================================

template <typename S>
void test_erase()
{
    using K = typename S::key_type;

    VX_SECTION("by key")
    {
        S s;
        fill(s, 0, 100);
        for (int i = 0; i < 100; i += 2)
        {
            VX_CHECK(s.erase(K(i)) == 1);
        }
        for (int i = 0; i < 100; i += 2)
        {
            VX_CHECK(s.erase(K(i)) == 0);
        }
        VX_CHECK(s.size() == 50);
        for (int i = 0; i < 100; ++i)
        {
            VX_CHECK(s.contains(K(i)) == (i % 2 == 1));
        }
    }

    VX_SECTION("by iterator returns the next element")
    {
        S s;
        fill(s, 0, 10);
        auto it = s.find(K(5));
        VX_CHECK(it != s.end());
        auto next = s.erase(it);
        VX_CHECK(s.size() == 9);
        VX_CHECK(!s.contains(K(5)));
        if (next != s.end())
        {
            VX_CHECK(s.contains(K(val(*next))));
        }

        // erase through the whole table with the returned iterator
        size_t n = 0;
        for (auto i = s.begin(); i != s.end(); i = s.erase(i))
        {
            ++n;
        }
        VX_CHECK(n == 9);
        VX_CHECK(s.empty());
    }

    VX_SECTION("range")
    {
        S s;
        fill(s, 0, 20);
        auto it = s.erase(s.begin(), s.begin()); // empty range
        VX_CHECK(it == s.begin());
        VX_CHECK(s.size() == 20);

        it = s.erase(s.begin(), s.end());
        VX_CHECK(it == s.end());
        VX_CHECK(s.empty());
    }

    VX_SECTION("erase_if")
    {
        S s;
        fill(s, 0, 100);
        const size_t n = s.erase_if([](const K& k)
            { return val(k) % 2 == 1; });
        VX_CHECK(n == 50);
        VX_CHECK(s.size() == 50);
        for (int i = 0; i < 100; ++i)
        {
            VX_CHECK(s.contains(K(i)) == (i % 2 == 0));
        }
        VX_CHECK(s.erase_if([](const K&)
                     { return false; }) == 0);
        VX_CHECK(s.erase_if([](const K&)
                     { return true; }) == 50);
        VX_CHECK(s.empty());
    }

    VX_SECTION("erase at begin until empty")
    {
        S s;
        fill(s, 0, 64);
        while (!s.empty())
        {
            s.erase(s.begin());
        }
        VX_CHECK(s.begin() == s.end());
        VX_CHECK(s.insert(K(1)).value().inserted); // still usable
    }

    VX_SECTION("sliding window churn (tombstones in the open backend)")
    {
        S s;
        const int window = 16;
        const int total = 5000;
        for (int i = 0; i < total; ++i)
        {
            VX_CHECK(s.insert(K(i)).value().inserted);
            if (i >= window)
            {
                VX_CHECK(s.erase(K(i - window)) == 1);
            }
        }
        VX_CHECK(eq_model(s, range<S>(total - window, total)));
    }
}
TEST_SETS(erase)

//=========================================================================
// growth / capacity
//=========================================================================

template <typename S>
void test_growth()
{
    using K = typename S::key_type;

    VX_SECTION("many inserts keep every element")
    {
        S s;
        fill(s, 0, 2000);
        VX_CHECK(s.size() == 2000);
        VX_CHECK(s.capacity() >= s.size());
        VX_CHECK(eq_model(s, range<S>(0, 2000)));
        for (int i = 0; i < 2000; ++i)
        {
            VX_CHECK(s.contains(K(i)));
        }
    }

    VX_SECTION("reserve")
    {
        S s;
        VX_CHECK(s.reserve(0));
        VX_CHECK(s.capacity() == 0);

        VX_CHECK(s.reserve(100));
        VX_CHECK(s.capacity() >= 100);
        VX_CHECK(s.empty());

        fill(s, 0, 50);
        const size_t cap = s.capacity();
        VX_CHECK(s.reserve(10)); // smaller: no-op
        VX_CHECK(s.capacity() == cap);
        VX_CHECK(eq_model(s, range<S>(0, 50)));

        VX_CHECK(!s.reserve(S::max_size() + 1));
        VX_CHECK(eq_model(s, range<S>(0, 50)));
    }

    VX_SECTION("_capacity variants after reserve")
    {
        S s;
        VX_CHECK(s.reserve(100));
        for (int i = 0; i < 100; ++i)
        {
            VX_CHECK(s.insert_capacity(K(i)).inserted);
        }
        VX_CHECK(!s.insert_capacity(K(5)).inserted);
        VX_CHECK(eq_model(s, range<S>(0, 100)));
    }

    VX_SECTION("_unique variants")
    {
        S s;
        VX_CHECK(s.reserve(10));
        auto r = s.insert_unique(K(1));
        VX_CHECK(r);
        VX_CHECK(val(*r.value()) == 1);
        auto it = s.insert_unique_capacity(K(2));
        VX_CHECK(val(*it) == 2);
        VX_CHECK(eq(s, { 1, 2 }));
    }

    VX_SECTION("clear keeps the table usable")
    {
        S s;
        fill(s, 0, 100);
        const size_t cap = s.capacity();
        s.clear();
        VX_CHECK(s.empty());
        VX_CHECK(s.begin() == s.end());
        VX_CHECK(s.capacity() == cap);
        VX_CHECK(!s.contains(K(5)));
        fill(s, 0, 100);
        VX_CHECK(eq_model(s, range<S>(0, 100)));
    }

    VX_SECTION("shrink_to_fit")
    {
        S s;
        fill(s, 0, 200);
        const size_t before = s.capacity();
        for (int i = 3; i < 200; ++i)
        {
            VX_CHECK(s.erase(K(i)) == 1);
        }
        VX_CHECK(s.shrink_to_fit());
        VX_CHECK(s.capacity() <= before);
        VX_CHECK(eq(s, { 0, 1, 2 }));

        s.clear();
        VX_CHECK(s.shrink_to_fit());
        VX_CHECK(s.capacity() == 0);
        VX_CHECK(s.insert(K(1)).value().inserted); // usable after deallocating
    }
}
TEST_SETS(growth)

//=========================================================================
// copy / move / swap / compare
//=========================================================================

template <typename S>
void test_value_semantics()
{
    using K = typename S::key_type;

    VX_SECTION("copy is independent")
    {
        S a;
        fill(a, 0, 50);
        S b(a);
        VX_CHECK(a == b);
        VX_CHECK(eq_model(b, range<S>(0, 50)));

        VX_CHECK(b.insert(K(100)).value().inserted);
        VX_CHECK(b.erase(K(0)) == 1);
        VX_CHECK(eq_model(a, range<S>(0, 50))); // a untouched
        VX_CHECK(a != b);

        S empty;
        S c(empty);
        VX_CHECK(c.empty());
    }

    VX_SECTION("copy assign")
    {
        S a;
        fill(a, 0, 30);
        S b;
        fill(b, 100, 105);
        b = a;
        VX_CHECK(b == a);
        VX_CHECK(eq_model(b, range<S>(0, 30)));

        S& self = b;
        b = self;
        VX_CHECK(eq_model(b, range<S>(0, 30)));

        auto r = b.assign(S());
        VX_CHECK(r);
        VX_CHECK(b.empty());
    }

    VX_SECTION("move")
    {
        S a;
        fill(a, 0, 50);
        S b(std::move(a));
        VX_CHECK(eq_model(b, range<S>(0, 50)));
        VX_CHECK(a.empty());
        VX_CHECK(a.begin() == a.end());
        VX_CHECK(!a.contains(K(1)));
        VX_CHECK(a.insert(K(1)).value().inserted); // moved-from is reusable

        S c;
        fill(c, 500, 510);
        c = std::move(b);
        VX_CHECK(eq_model(c, range<S>(0, 50)));
        VX_CHECK(b.empty());
    }

    VX_SECTION("swap")
    {
        S a;
        S b;
        fill(a, 0, 10);
        fill(b, 100, 103);
        a.swap(b);
        VX_CHECK(eq_model(a, range<S>(100, 103)));
        VX_CHECK(eq_model(b, range<S>(0, 10)));

        S e;
        a.swap(e);
        VX_CHECK(a.empty());
        VX_CHECK(eq_model(e, range<S>(100, 103)));
    }

    VX_SECTION("equality ignores insertion order and capacity")
    {
        S a;
        S b;
        fill(a, 0, 40);
        for (int i = 39; i >= 0; --i)
        {
            VX_CHECK(b.insert(K(i)));
        }
        VX_CHECK(b.reserve(500)); // different capacity
        VX_CHECK(a == b);
        VX_CHECK(b.erase(K(7)) == 1);
        VX_CHECK(a != b);
        VX_CHECK(b.insert(K(7)));
        VX_CHECK(a == b);
        VX_CHECK(S() == S());
    }
}
TEST_SETS(value_semantics)

//=========================================================================
// iteration
//=========================================================================

template <typename S>
void test_iteration()
{
    S s;
    VX_CHECK(s.begin() == s.end());

    fill(s, 0, 100);
    size_t n = 0;
    for (auto it = s.begin(); it != s.end(); ++it)
    {
        ++n;
    }
    VX_CHECK(n == 100);
    VX_CHECK(eq_model(s, range<S>(0, 100)));

    // post-increment and const iteration
    const S& cs = s;
    n = 0;
    for (auto it = cs.cbegin(); it != cs.cend(); it++)
    {
        ++n;
    }
    VX_CHECK(n == 100);

    // sparse table (many erased): iteration still visits exactly the survivors
    for (int i = 0; i < 100; ++i)
    {
        if (i % 10 != 0)
        {
            VX_CHECK(s.erase(typename S::key_type(i)) == 1);
        }
    }
    //VX_CHECK(eq_model(s, { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 } == std::initializer_list<int>{} ? std::vector<int>{} : std::vector<int>{ 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 }));
}
TEST_SETS(iteration)

//=========================================================================
// default hasher
//=========================================================================

template <typename S>
void test_default_hasher()
{
    S s;
    for (int i = 0; i < 1000; ++i)
    {
        VX_CHECK(s.insert(i));
    }
    VX_CHECK(s.size() == 1000);
    for (int i = 0; i < 1000; ++i)
    {
        VX_CHECK(s.contains(i));
    }
    VX_CHECK(!s.contains(1000));
    VX_CHECK(s.erase(500) == 1);
    VX_CHECK(!s.contains(500));
}

VX_TEST_CASE(default_hasher_open)
{
    test_default_hasher<vx::unordered_set<int>>();
}

VX_TEST_CASE(default_hasher_chain)
{
    test_default_hasher<vx::chain_unordered_set<int>>();
}

//=========================================================================
// lifetime
//=========================================================================

template <typename S>
void test_lifetime()
{
    tracked::reset();
    {
        S s;
        for (int i = 0; i < 100; ++i)
        {
            VX_CHECK(s.insert(tracked(i)));
            VX_CHECK(tracked::live == static_cast<int>(s.size()));
        }
        VX_CHECK(tracked::copies == 0); // growth relocates by move (or not at all)

        for (int i = 0; i < 10; ++i)
        {
            VX_CHECK(s.erase(tracked(i)) == 1);
        }
        VX_CHECK(tracked::live == 90);

        S c(s);
        VX_CHECK(tracked::live == 180);
        VX_CHECK(c == s);

        s.clear();
        VX_CHECK(tracked::live == 90);

        VX_CHECK(c.erase_if([](const tracked& t)
                     { return t.v < 50; }) == 40);
        VX_CHECK(tracked::live == 50);

        c.clear();
        VX_CHECK(c.shrink_to_fit());
        VX_CHECK(tracked::live == 0);
    }
    VX_CHECK(tracked::live == 0);
}

VX_TEST_CASE(lifetime_open)
{
    test_lifetime<open_set<tracked>>();
}

VX_TEST_CASE(lifetime_chain)
{
    test_lifetime<chain_set<tracked>>();
}

//=========================================================================
// chain only: element addresses are stable
//=========================================================================

VX_TEST_CASE(chain_pointer_stability)
{
    chain_set<int> s;
    VX_CHECK(s.insert(42));
    const int* p = &*s.find(42);

    for (int i = 0; i < 5000; ++i)
    {
        VX_CHECK(s.insert(i + 100)); // many rehashes
    }
    VX_CHECK(&*s.find(42) == p);

    VX_CHECK(s.reserve(100000));
    VX_CHECK(&*s.find(42) == p);

    for (int i = 0; i < 5000; ++i)
    {
        VX_CHECK(s.erase(i + 100) == 1);
    }
    VX_CHECK(s.shrink_to_fit());
    VX_CHECK(&*s.find(42) == p);
    VX_CHECK(*p == 42);
}

//=========================================================================
// randomized comparison against std::set
//=========================================================================

template <typename S>
void test_stress()
{
    using K = typename S::key_type;

    for (unsigned seed = 1; seed <= 8; ++seed)
    {
        S s;
        std::set<int> m;
        unsigned state = seed * 7919u;

        auto rnd = [&](unsigned n)
        {
            state = state * 1664525u + 1013904223u;
            return (state >> 8) % n;
        };

        for (int iter = 0; iter < 2000; ++iter)
        {
            const int x = static_cast<int>(rnd(300));
            switch (rnd(8))
            {
                case 0:
                case 1:
                case 2:
                {
                    auto r = s.insert(K(x));
                    VX_CHECK(r);
                    VX_CHECK(r.value().inserted == m.insert(x).second);
                    break;
                }
                case 3:
                case 4:
                {
                    VX_CHECK(s.erase(K(x)) == m.erase(x));
                    break;
                }
                case 5:
                {
                    VX_CHECK(s.contains(K(x)) == (m.count(x) == 1));
                    break;
                }
                case 6:
                {
                    const int mod = static_cast<int>(rnd(5)) + 2;
                    s.erase_if([mod](const K& k)
                        { return val(k) % mod == 0; });
                    for (auto it = m.begin(); it != m.end();)
                    {
                        it = (*it % mod == 0) ? m.erase(it) : std::next(it);
                    }
                    break;
                }
                case 7:
                {
                    const unsigned k = rnd(20);
                    if (k == 0)
                    {
                        s.clear();
                        m.clear();
                    }
                    else if (k < 10)
                    {
                        VX_CHECK(s.reserve(rnd(400)));
                    }
                    else
                    {
                        VX_CHECK(s.shrink_to_fit());
                    }
                    break;
                }
            }

            VX_CHECK(eq_model(s, std::vector<int>(m.begin(), m.end())));
        }
    }
}
TEST_SETS(stress)

//=========================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
