#include "vertex/std/hash_map.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <list>
#include <map>
#include <sstream>
#include <type_traits>
#include <utility>
#include <vector>

#include "vertex_test/test.hpp"

// Generic tests: every test is a template over the map type, so the same code
// runs against every key type / allocator combination (TEST_MAPS).
//
// Key types:
//   int           trivial key
//   copyable_int  non-trivial key (copy + move, moved-from value is -1)
//   movable_int   move-only key (copy deleted: an accidental copy is a compile error)
//
// The mapped type is `int` for the generic tests; dedicated tests below cover
// move-only / lifetime-tracked / multi-argument mapped types.
//
// Contents are checked as sorted (key, mapped) pairs. For key k the canonical
// mapped value is vof(k) = k * 10 + 1, which is what fill() inserts.
//
// Code that needs to copy a key or mapped value is guarded with
// `if constexpr (map_copyable<S>)`.
//
// Requirements on test helpers outside this file:
//   * tracking_allocator<T> must have the converting constructor
//       template <typename U> tracking_allocator(const tracking_allocator<U>&)
//     that copies id + stats (needed because the table rebinds the allocator internally).
//
// Not covered: merge (disabled in the set tests as well; add it once it exists).
// Assumed map API (taken from basic_hash_map): get, at, get_or_insert, operator[],
// try_emplace, insert_or_assign, plus everything inherited from the table base
// (insert / emplace / erase / find / contains / count / reserve / assign / ...).

//=========================================================================
// helpers
//=========================================================================

namespace {

using vx::test::allocator_stats;
using vx::test::copyable_int;
using vx::test::movable_int;
using vx::test::tracking_allocator;

template <typename K>
constexpr bool copyable = std::is_copy_constructible_v<K>;

template <typename S>
constexpr bool map_copyable = copyable<typename S::key_type> && copyable<typename S::mapped_type>;

// works for int, copyable_int, movable_int, counted_key (all convert to int)
template <typename T>
int val(const T& t)
{
    return static_cast<int>(t);
}

// canonical mapped value for key k
constexpr int vof(int k)
{
    return k * 10 + 1;
}

using kv = std::pair<int, int>;

// Deliberately NOT avalanching, so the table's own finalizer is exercised.
template <typename K>
struct test_hash
{
    size_t operator()(const K& k) const noexcept
    {
        return vx::default_hash<int>()(val(k));
    }
};

// Everything collides: forces maximal probe chains / deleted-slot handling.
template <typename K>
struct const_hash
{
    size_t operator()(const K&) const noexcept
    {
        return 0;
    }
};

// Claims to be avalanching, so the table should skip its own mixing.
template <typename K>
struct avalanche_hash
{
    using is_avalanching = void;

    size_t operator()(const K& k) const noexcept
    {
        uint64_t x = static_cast<uint64_t>(val(k)) + 0x9E3779B97F4A7C15ull;
        x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ull;
        x = (x ^ (x >> 27)) * 0x94D049BB133111EBull;
        x = x ^ (x >> 31);
        return static_cast<size_t>(x);
    }
};

template <typename K>
using plain_map = vx::unordered_map<K, int, test_hash<K>>;

template <typename K>
using tracked_map = vx::unordered_map<K, int, test_hash<K>, std::equal_to<K>, tracking_allocator<std::pair<const K, int>>>;

template <typename K>
using degenerate_map = vx::unordered_map<K, int, const_hash<K>>;

template <typename K>
using avalanche_map = vx::unordered_map<K, int, avalanche_hash<K>>;

// sorted (key, mapped) pairs, by walking the iterators
template <typename S>
std::vector<kv> contents(const S& s)
{
    std::vector<kv> out;
    for (auto it = s.begin(); it != s.end(); ++it)
    {
        out.push_back(kv(val(it->first), val(it->second)));
    }
    std::sort(out.begin(), out.end());
    return out;
}

template <typename S>
bool eq_model(const S& s, const std::vector<kv>& sorted)
{
    return s.size() == sorted.size() && contents(s) == sorted;
}

template <typename S>
bool eq(const S& s, std::initializer_list<kv> l)
{
    std::vector<kv> sorted(l);
    std::sort(sorted.begin(), sorted.end());
    return eq_model(s, sorted);
}

// [lo, hi) with the canonical mapped values
std::vector<kv> range(int lo, int hi)
{
    std::vector<kv> v;
    for (int i = lo; i < hi; ++i)
    {
        v.push_back(kv(i, vof(i)));
    }
    return v;
}

// [lo, hi) where every key maps to `value`
std::vector<kv> range_with(int lo, int hi, int value)
{
    std::vector<kv> v;
    for (int i = lo; i < hi; ++i)
    {
        v.push_back(kv(i, value));
    }
    return v;
}

// inserts [lo, hi) -> vof(key)
template <typename S>
void fill(S& s, int lo, int hi)
{
    using K = typename S::key_type;
    for (int i = lo; i < hi; ++i)
    {
        auto r = s.try_emplace(K(i), vof(i));
        VX_CHECK(r);
        VX_CHECK(r.value().inserted);
    }
}

// a value_type for (i, vof(i)), for the base insert / emplace / range APIs
template <typename S>
typename S::value_type entry(int i)
{
    using K = typename S::key_type;
    using V = typename S::mapped_type;
    return typename S::value_type(K(i), V(vof(i)));
}

template <typename S>
std::vector<typename S::value_type> entries(int lo, int hi)
{
    std::vector<typename S::value_type> v;
    for (int i = lo; i < hi; ++i)
    {
        v.push_back(entry<S>(i));
    }
    return v;
}

//-------------------------------------------------------------------------
// counted_key: tracks construction / destruction / copies / moves, and
// detects use-after-destroy and double destroy via a magic value.
// Used both as a key and as a mapped type.
//-------------------------------------------------------------------------

struct counted_stats
{
    size_t live = 0;
    size_t constructed = 0;
    size_t destroyed = 0;
    size_t copies = 0;
    size_t moves = 0;
    size_t bad = 0; // operations on a destroyed / garbage object
};

counted_stats g_counted_stats;

constexpr unsigned counted_magic = 0xC0FFEEu;

struct counted_key
{
    using stats_t = counted_stats;

    static stats_t& stats() noexcept
    {
        return g_counted_stats;
    }

    static void reset() noexcept
    {
        g_counted_stats = counted_stats{};
    }

    counted_key() noexcept
        : m_v(0)
    {
        ++stats().live;
        ++stats().constructed;
    }

    explicit counted_key(int v) noexcept
        : m_v(v)
    {
        ++stats().live;
        ++stats().constructed;
    }

    counted_key(const counted_key& o)
        : m_v(o.m_v)
    {
        o.check();
        ++stats().live;
        ++stats().constructed;
        ++stats().copies;
    }

    counted_key(counted_key&& o) noexcept
        : m_v(o.m_v)
    {
        o.check();
        o.m_v = -1;
        ++stats().live;
        ++stats().constructed;
        ++stats().moves;
    }

    counted_key& operator=(const counted_key& o)
    {
        check();
        o.check();
        m_v = o.m_v;
        ++stats().copies;
        return *this;
    }

    counted_key& operator=(counted_key&& o) noexcept
    {
        check();
        o.check();
        m_v = o.m_v;
        o.m_v = -1;
        ++stats().moves;
        return *this;
    }

    ~counted_key()
    {
        check();
        m_magic = 0;
        ++stats().destroyed;
        --stats().live;
    }

    explicit operator int() const noexcept
    {
        return m_v;
    }

    friend bool operator==(const counted_key& a, const counted_key& b) noexcept
    {
        return a.m_v == b.m_v;
    }

    friend bool operator!=(const counted_key& a, const counted_key& b) noexcept
    {
        return a.m_v != b.m_v;
    }

private:

    void check() const noexcept
    {
        if (m_magic != counted_magic)
        {
            ++stats().bad;
        }
    }

    int m_v;
    unsigned m_magic = counted_magic;
};

// both key and mapped are counted, so live == 2 * size
using counted_map = vx::unordered_map<counted_key, counted_key, test_hash<counted_key>>;

//-------------------------------------------------------------------------
// multi-argument key / mapped types
//-------------------------------------------------------------------------

struct pt_key
{
    int x;
    int y;

    pt_key(int x_, int y_) noexcept
        : x(x_)
        , y(y_)
    {}

    friend bool operator==(const pt_key& a, const pt_key& b) noexcept
    {
        return a.x == b.x && a.y == b.y;
    }
};

struct pt_hash
{
    size_t operator()(const pt_key& k) const noexcept
    {
        return vx::default_hash<int>()(k.x * 7919 + k.y);
    }
};

//-------------------------------------------------------------------------
// heterogeneous (transparent) lookup: wide_key can be looked up by int
//-------------------------------------------------------------------------

struct wide_key
{
    int v;

    explicit wide_key(int x) noexcept
        : v(x)
    {}
};

struct wide_hash
{
    using is_transparent = void;

    size_t operator()(const wide_key& k) const noexcept
    {
        return (*this)(k.v);
    }

    size_t operator()(int v) const noexcept
    {
        return vx::default_hash<int>()(v);
    }
};

struct wide_eq
{
    using is_transparent = void;

    bool operator()(const wide_key& a, const wide_key& b) const noexcept
    {
        return a.v == b.v;
    }

    bool operator()(const wide_key& a, int b) const noexcept
    {
        return a.v == b;
    }

    bool operator()(int a, const wide_key& b) const noexcept
    {
        return a == b.v;
    }
};

//-------------------------------------------------------------------------
// stateful hasher / equality, to check they are copied, moved and swapped
//-------------------------------------------------------------------------

struct seeded_hash
{
    int seed = 0;

    seeded_hash() = default;

    explicit seeded_hash(int s) noexcept
        : seed(s)
    {}

    size_t operator()(int k) const noexcept
    {
        return vx::default_hash<int>()(k + seed);
    }
};

struct tagged_eq
{
    int tag = 0;

    tagged_eq() = default;

    explicit tagged_eq(int t) noexcept
        : tag(t)
    {}

    bool operator()(int a, int b) const noexcept
    {
        return a == b;
    }
};

//-------------------------------------------------------------------------
// failing_allocator: malloc based, can be told to fail.
// countdown == -1: never fail. countdown == k: k more allocations succeed,
// then every allocation returns nullptr until countdown is reset.
//-------------------------------------------------------------------------

struct fail_state
{
    long countdown = -1;
    long live = 0;
    size_t allocs = 0;
    size_t deallocs = 0;
    size_t failures = 0;
};

template <typename T>
class failing_allocator
{
public:

    using value_type = T;

    failing_allocator() noexcept = default;

    explicit failing_allocator(fail_state* s) noexcept
        : m_s(s)
    {}

    template <typename U>
    failing_allocator(const failing_allocator<U>& o) noexcept
        : m_s(o.state())
    {}

    fail_state* state() const noexcept
    {
        return m_s;
    }

    T* allocate(size_t n) noexcept
    {
        if (m_s)
        {
            if (m_s->countdown == 0)
            {
                ++m_s->failures;
                return nullptr;
            }
            if (m_s->countdown > 0)
            {
                --m_s->countdown;
            }
        }

        T* p = static_cast<T*>(std::malloc(n * sizeof(T)));
        if (p && m_s)
        {
            ++m_s->allocs;
            ++m_s->live;
        }
        return p;
    }

    void deallocate(T* p, size_t) noexcept
    {
        if (!p)
        {
            return;
        }
        if (m_s)
        {
            ++m_s->deallocs;
            --m_s->live;
        }
        std::free(p);
    }

private:

    fail_state* m_s = nullptr;
};

template <typename T, typename U>
bool operator==(const failing_allocator<T>& a, const failing_allocator<U>& b) noexcept
{
    return a.state() == b.state();
}

template <typename T, typename U>
bool operator!=(const failing_allocator<T>& a, const failing_allocator<U>& b) noexcept
{
    return !(a == b);
}

template <typename K, typename V = int>
using failing_map = vx::unordered_map<K, V, test_hash<K>, std::equal_to<K>, failing_allocator<std::pair<const K, V>>>;

// Runs a templated test for every key type / allocator combination.
#define TEST_MAPS(name) \
    VX_TEST_CASE(name##_int) \
    { \
        test_##name<plain_map<int>>(); \
    } \
    VX_TEST_CASE(name##_copyable) \
    { \
        test_##name<plain_map<copyable_int>>(); \
    } \
    VX_TEST_CASE(name##_tracked_copyable) \
    { \
        test_##name<tracked_map<copyable_int>>(); \
    }

// Same, but for tests that only make sense with the key types that can be copied.
#define TEST_MAPS_COPYABLE(name) \
    VX_TEST_CASE(name##_int) \
    { \
        test_##name<plain_map<int>>(); \
    } \
    VX_TEST_CASE(name##_copyable) \
    { \
        test_##name<plain_map<copyable_int>>(); \
    } \
    VX_TEST_CASE(name##_tracked_copyable) \
    { \
        test_##name<tracked_map<copyable_int>>(); \
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
        VX_CHECK(s.get(K(1)) == nullptr);
        VX_CHECK(!s.at(K(1)));
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

        // create from an rvalue takes the buffer
        auto m = S::create(std::move(src));
        VX_CHECK(m);
        VX_CHECK(eq_model(m.value(), range(0, 10)));

        if constexpr (map_copyable<S>)
        {
            S src2;
            fill(src2, 0, 10);
            auto c = S::create(src2);
            VX_CHECK(c);
            VX_CHECK(c.value() == src2);
        }
    }

    VX_SECTION("reserving constructor")
    {
        S s(size_t(50));
        VX_CHECK(s.empty());
        VX_CHECK(s.capacity() >= 50);
    }
}
TEST_MAPS(construct)

template <typename S>
void test_construct_from_ranges()
{
    using K = typename S::key_type;
    using V = typename S::value_type;

    std::vector<V> src;
    for (int i = 0; i < 20; ++i)
    {
        src.push_back(entry<S>(i % 10)); // duplicate keys
    }

    VX_SECTION("range constructor")
    {
        S s(src.begin(), src.end());
        VX_CHECK(eq_model(s, range(0, 10)));
    }

    VX_SECTION("range constructor honours the n argument")
    {
        S s(src.begin(), src.end(), size_t(64));
        VX_CHECK(eq_model(s, range(0, 10)));
        VX_CHECK(s.capacity() >= 64);
    }

    VX_SECTION("range from a non-random-access iterator")
    {
        std::list<V> l(src.begin(), src.end());
        S s(l.begin(), l.end());
        VX_CHECK(eq_model(s, range(0, 10)));
    }

    VX_SECTION("range with duplicate keys keeps the first")
    {
        std::vector<V> dup;
        dup.push_back(V(K(1), 100));
        dup.push_back(V(K(1), 200));
        dup.push_back(V(K(2), 300));
        S s(dup.begin(), dup.end());
        VX_CHECK(eq(s, { kv(1, 100), kv(2, 300) }));
    }

    VX_SECTION("initializer_list constructor")
    {
        S s{ V(K(1), 10), V(K(2), 20), V(K(2), 99), V(K(3), 30) };
        VX_CHECK(eq(s, { kv(1, 10), kv(2, 20), kv(3, 30) }));

        S t(std::initializer_list<V>{ V(K(5), 50), V(K(6), 60) }, size_t(32));
        VX_CHECK(eq(t, { kv(5, 50), kv(6, 60) }));
        VX_CHECK(t.capacity() >= 32);
    }

    VX_SECTION("create from range / initializer_list")
    {
        auto a = S::create(src.begin(), src.end());
        VX_CHECK(a);
        VX_CHECK(eq_model(a.value(), range(0, 10)));

        auto b = S::create(src.begin(), src.end(), size_t(100));
        VX_CHECK(b);
        VX_CHECK(b.value().capacity() >= 100);
        VX_CHECK(eq_model(b.value(), range(0, 10)));

        auto c = S::create(std::initializer_list<V>{ V(K(7), 70), V(K(8), 80), V(K(7), 71) });
        VX_CHECK(c);
        VX_CHECK(eq(c.value(), { kv(7, 70), kv(8, 80) }));
    }

    VX_SECTION("create from an empty range")
    {
        std::vector<V> none;
        auto a = S::create(none.begin(), none.end());
        VX_CHECK(a);
        VX_CHECK(a.value().empty());
    }
}
TEST_MAPS_COPYABLE(construct_from_ranges)

//=========================================================================
// insert / lookup (value_type based API from the table base)
//=========================================================================

template <typename S>
void test_insert_find()
{
    using K = typename S::key_type;
    using V = typename S::mapped_type;

    VX_SECTION("insert reports whether it inserted and never overwrites")
    {
        S s;
        auto r = s.insert(typename S::value_type(K(1), V(10)));
        VX_CHECK(r);
        VX_CHECK(r.value().inserted);
        VX_CHECK(val(r.value().it->first) == 1);
        VX_CHECK(val(r.value().it->second) == 10);
        VX_CHECK(s.size() == 1);

        auto again = s.insert(typename S::value_type(K(1), V(20)));
        VX_CHECK(again);
        VX_CHECK(!again.value().inserted);
        VX_CHECK(again.value().it == r.value().it);
        VX_CHECK(val(again.value().it->second) == 10); // untouched
        VX_CHECK(s.size() == 1);

        VX_CHECK(s.emplace(K(3), V(30)).value().inserted);
        VX_CHECK(!s.emplace(K(3), V(31)).value().inserted);
        VX_CHECK(eq(s, { kv(1, 10), kv(3, 30) }));

        if constexpr (map_copyable<S>)
        {
            const typename S::value_type c(K(2), V(20));
            VX_CHECK(s.insert(c).value().inserted);
            VX_CHECK(!s.insert(c).value().inserted);
            VX_CHECK(eq(s, { kv(1, 10), kv(2, 20), kv(3, 30) }));
        }
    }

    VX_SECTION("find / contains / count")
    {
        S s;
        fill(s, 0, 100);
        for (int i = 0; i < 100; ++i)
        {
            auto it = s.find(K(i));
            VX_CHECK(it != s.end());
            VX_CHECK(val(it->first) == i);
            VX_CHECK(val(it->second) == vof(i));
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
        VX_CHECK(val(cs.find(K(3))->second) == vof(3));
        VX_CHECK(cs.find(K(30)) == cs.end());
        VX_CHECK(cs.contains(K(9)));
        VX_CHECK(cs.count(K(9)) == 1);
    }

    VX_SECTION("mutating the mapped value through an iterator")
    {
        S s;
        fill(s, 0, 20);
        for (auto it = s.begin(); it != s.end(); ++it)
        {
            it->second = V(val(it->first) + 1000);
        }
        for (int i = 0; i < 20; ++i)
        {
            VX_CHECK(s.contains(K(i)));
            VX_CHECK(val(s.find(K(i))->second) == i + 1000);
        }
    }

    if constexpr (map_copyable<S>)
    {
        VX_SECTION("insert range")
        {
            std::vector<typename S::value_type> src;
            for (int i = 0; i < 20; ++i)
            {
                src.push_back(entry<S>(i % 10)); // duplicates are skipped
            }
            S s;
            VX_CHECK(s.insert(src.begin(), src.end()));
            VX_CHECK(eq_model(s, range(0, 10)));

            // inserting the same range again changes nothing
            VX_CHECK(s.insert(src.begin(), src.end()));
            VX_CHECK(eq_model(s, range(0, 10)));
        }

        VX_SECTION("insert range does not overwrite existing keys")
        {
            S s;
            VX_CHECK(s.try_emplace(K(1), 111));
            std::vector<typename S::value_type> src;
            src.push_back(typename S::value_type(K(1), V(222)));
            src.push_back(typename S::value_type(K(2), V(333)));
            VX_CHECK(s.insert(src.begin(), src.end()));
            VX_CHECK(eq(s, { kv(1, 111), kv(2, 333) }));
        }

        VX_SECTION("insert initializer_list")
        {
            S s;
            VX_CHECK(s.insert(std::initializer_list<typename S::value_type>{
                typename S::value_type(K(1), V(10)),
                typename S::value_type(K(2), V(20)),
                typename S::value_type(K(1), V(11)),
                typename S::value_type(K(3), V(30)) }));
            VX_CHECK(eq(s, { kv(1, 10), kv(2, 20), kv(3, 30) }));
        }

        VX_SECTION("insert empty range")
        {
            std::vector<typename S::value_type> none;
            S s;
            VX_CHECK(s.insert(none.begin(), none.end()));
            VX_CHECK(s.empty());
        }
    }
}
TEST_MAPS(insert_find)

//VX_TEST_CASE(construct_from_input_iterators)
//{
//    using S = plain_map<int>;
//
//    // single-pass input iterator over pairs: the iterator adapter yields pairs built from the stream
//    struct pair_iter
//    {
//        using iterator_category = std::input_iterator_tag;
//        using value_type = std::pair<int, int>;
//        using difference_type = std::ptrdiff_t;
//        using pointer = const value_type*;
//        using reference = value_type;
//
//        std::istream_iterator<int> it;
//
//        pair_iter() = default;
//
//        explicit pair_iter(std::istream_iterator<int> i)
//            : it(i)
//        {}
//
//        value_type operator*() const
//        {
//            return value_type(*it, vof(*it));
//        }
//
//        pair_iter& operator++()
//        {
//            ++it;
//            return *this;
//        }
//
//        pair_iter operator++(int)
//        {
//            pair_iter old = *this;
//            ++it;
//            return old;
//        }
//
//        friend bool operator==(const pair_iter& a, const pair_iter& b)
//        {
//            return a.it == b.it;
//        }
//
//        friend bool operator!=(const pair_iter& a, const pair_iter& b)
//        {
//            return !(a == b);
//        }
//    };
//
//    std::istringstream in("1 2 3 3 4 1 5");
//    S s(pair_iter{ std::istream_iterator<int>(in) }, pair_iter{ std::istream_iterator<int>() });
//    VX_CHECK(eq_model(s, range(1, 6)));
//
//    std::istringstream in2("10 20 20 30");
//    S t;
//    VX_CHECK(t.insert(pair_iter{ std::istream_iterator<int>(in2) }, pair_iter{ std::istream_iterator<int>() }));
//    VX_CHECK(eq(t, { kv(10, vof(10)), kv(20, vof(20)), kv(30, vof(30)) }));
//}

//=========================================================================
// emplace (table base)
//=========================================================================

template <typename S>
void test_emplace()
{
    using K = typename S::key_type;
    using V = typename S::mapped_type;

    VX_SECTION("emplace / emplace_assume_capacity")
    {
        S s;
        VX_CHECK(s.emplace(K(5), V(50)).value().inserted);
        VX_CHECK(!s.emplace(K(5), V(51)).value().inserted);
        VX_CHECK(s.reserve(10));
        VX_CHECK(s.emplace_assume_capacity(K(6), V(60)).inserted);
        VX_CHECK(!s.emplace_assume_capacity(K(6), V(61)).inserted);
        VX_CHECK(eq(s, { kv(5, 50), kv(6, 60) }));
    }

    VX_SECTION("emplace_assume_unique variants")
    {
        S s;
        VX_CHECK(s.reserve(10));
        auto a = s.emplace_assume_unique(K(1), V(10));
        VX_CHECK(a);
        VX_CHECK(val(a.value()->first) == 1);
        VX_CHECK(val(a.value()->second) == 10);
        auto b = s.emplace_assume_unique_and_capacity(K(2), V(20));
        VX_CHECK(val(b->first) == 2);
        VX_CHECK(eq(s, { kv(1, 10), kv(2, 20) }));
    }

    VX_SECTION("emplace returns an iterator to the existing element")
    {
        S s;
        auto first = s.emplace(K(7), V(70));
        auto second = s.emplace(K(7), V(71));
        VX_CHECK(first.value().it == second.value().it);
        VX_CHECK(val(second.value().it->second) == 70);
        VX_CHECK(s.size() == 1);
    }
}
TEST_MAPS(emplace)

VX_TEST_CASE(emplace_multiple_arguments)
{
    using S = vx::unordered_map<pt_key, int, pt_hash>;

    S s;
    VX_CHECK(s.try_emplace(pt_key(1, 2), 12).value().inserted);
    VX_CHECK(!s.try_emplace(pt_key(1, 2), 99).value().inserted);
    VX_CHECK(s.try_emplace(pt_key(2, 1), 21).value().inserted);
    VX_CHECK(s.size() == 2);
    VX_CHECK(s.contains(pt_key(1, 2)));
    VX_CHECK(s.contains(pt_key(2, 1)));
    VX_CHECK(!s.contains(pt_key(2, 2)));
    VX_CHECK(*s.get(pt_key(1, 2)) == 12);
    VX_CHECK(*s.get(pt_key(2, 1)) == 21);

    VX_CHECK(s.reserve(10));
    VX_CHECK(s.emplace_assume_capacity(pt_key(3, 3), 33).inserted);
    VX_CHECK(s.size() == 3);
}

VX_TEST_CASE(try_emplace_multiple_mapped_arguments)
{
    // mapped type without a default constructor, built in place from several args
    using S = vx::unordered_map<int, pt_key, test_hash<int>>;

    S s;
    auto r = s.try_emplace(1, 3, 4);
    VX_CHECK(r);
    VX_CHECK(r.value().inserted);
    VX_CHECK(r.value().it->second.x == 3);
    VX_CHECK(r.value().it->second.y == 4);

    auto again = s.try_emplace(1, 9, 9);
    VX_CHECK(again);
    VX_CHECK(!again.value().inserted);
    VX_CHECK(s.get(1)->x == 3); // existing mapped value untouched
    VX_CHECK(s.get(1)->y == 4);

    VX_CHECK(s.try_emplace(2, 5, 6).value().inserted);
    VX_CHECK(s.size() == 2);
    VX_CHECK(s.at(2).value().x == 5);

    // insert_or_assign needs only a pt_key to assign from
    VX_CHECK(!s.insert_or_assign(2, pt_key(7, 8)).value().inserted);
    VX_CHECK(s.get(2)->x == 7);
    VX_CHECK(s.get(2)->y == 8);
    VX_CHECK(s.insert_or_assign(3, pt_key(1, 1)).value().inserted);
    VX_CHECK(s.size() == 3);
}

//=========================================================================
// get / at
//=========================================================================

template <typename S>
void test_get_at()
{
    using K = typename S::key_type;
    using V = typename S::mapped_type;

    VX_SECTION("get returns a pointer or nullptr")
    {
        S s;
        fill(s, 0, 50);
        for (int i = 0; i < 50; ++i)
        {
            auto* p = s.get(K(i));
            VX_CHECK(p != nullptr);
            VX_CHECK(val(*p) == vof(i));
        }
        VX_CHECK(s.get(K(50)) == nullptr);
        VX_CHECK(s.get(K(-1)) == nullptr);
        VX_CHECK(s.size() == 50); // get never inserts
    }

    VX_SECTION("get allows modification")
    {
        S s;
        fill(s, 0, 10);
        *s.get(K(3)) = V(999);
        VX_CHECK(val(*s.get(K(3))) == 999);
        VX_CHECK(val(s.find(K(3))->second) == 999);
        VX_CHECK(val(*s.get(K(4))) == vof(4));
    }

    VX_SECTION("const get")
    {
        S s;
        fill(s, 0, 10);
        const S& cs = s;
        const V* p = cs.get(K(4));
        VX_CHECK(p != nullptr);
        VX_CHECK(val(*p) == vof(4));
        VX_CHECK(cs.get(K(40)) == nullptr);
    }

    VX_SECTION("at reports out_of_range for a missing key")
    {
        S s;
        fill(s, 0, 20);

        auto r = s.at(K(5));
        VX_CHECK(r);
        VX_CHECK(val(r.value()) == vof(5));

        r.value() = V(77); // reference into the table
        VX_CHECK(val(s.find(K(5))->second) == 77);

        auto miss = s.at(K(500));
        VX_CHECK(!miss);
        VX_CHECK(s.size() == 20); // at never inserts
        VX_CHECK(!s.contains(K(500)));
    }

    VX_SECTION("const at")
    {
        S s;
        fill(s, 0, 20);
        const S& cs = s;
        auto r = cs.at(K(6));
        VX_CHECK(r);
        VX_CHECK(val(r.value()) == vof(6));
        VX_CHECK(!cs.at(K(600)));
    }

    VX_SECTION("get / at after erase")
    {
        S s;
        fill(s, 0, 20);
        VX_CHECK(s.erase(K(5)) == 1);
        VX_CHECK(s.get(K(5)) == nullptr);
        VX_CHECK(!s.at(K(5)));
        VX_CHECK(s.get(K(6)) != nullptr);
    }
}
TEST_MAPS(get_at)

//=========================================================================
// get_or_insert / operator[]
//=========================================================================

template <typename S>
void test_get_or_insert()
{
    using K = typename S::key_type;
    using V = typename S::mapped_type;

    VX_SECTION("inserts a default-constructed value when absent")
    {
        S s;
        auto r = s.get_or_insert(K(1));
        VX_CHECK(r);
        VX_CHECK(r.value() != nullptr);
        VX_CHECK(val(*r.value()) == 0);
        VX_CHECK(s.size() == 1);
        VX_CHECK(s.contains(K(1)));
    }

    VX_SECTION("returns the existing value when present")
    {
        S s;
        fill(s, 0, 10);
        auto r = s.get_or_insert(K(4));
        VX_CHECK(r);
        VX_CHECK(val(*r.value()) == vof(4));
        VX_CHECK(s.size() == 10);

        *r.value() = V(1234);
        VX_CHECK(val(*s.get(K(4))) == 1234);
    }

    VX_SECTION("many keys, with growth")
    {
        S s;
        for (int i = 0; i < 1000; ++i)
        {
            auto r = s.get_or_insert(K(i));
            VX_CHECK(r);
            *r.value() = V(vof(i)); // pointer used before the next insert
        }
        VX_CHECK(eq_model(s, range(0, 1000)));
    }

    VX_SECTION("operator[] with rvalue keys")
    {
        S s;
        s[K(1)] = V(10);
        s[K(2)] = V(20);
        s[K(1)] = V(11);
        VX_CHECK(eq(s, { kv(1, 11), kv(2, 20) }));
        VX_CHECK(val(s[K(3)]) == 0); // default inserted
        VX_CHECK(s.size() == 3);
    }

    VX_SECTION("operator[] with an lvalue key")
    {
        if constexpr (copyable<K>)
        {
            S s;
            const K k(5);
            s[k] = V(50);
            VX_CHECK(val(s[k]) == 50);
            K k2(6);
            s[k2] = V(60);
            VX_CHECK(eq(s, { kv(5, 50), kv(6, 60) }));
        }
    }

    VX_SECTION("operator[] growth keeps every element")
    {
        S s;
        for (int i = 0; i < 2000; ++i)
        {
            s[K(i)] = V(vof(i));
        }
        VX_CHECK(eq_model(s, range(0, 2000)));
    }
}
TEST_MAPS(get_or_insert)

//=========================================================================
// try_emplace
//=========================================================================

template <typename S>
void test_try_emplace()
{
    using K = typename S::key_type;
    using V = typename S::mapped_type;

    VX_SECTION("inserts, then reports the existing element")
    {
        S s;
        auto a = s.try_emplace(K(1), 10);
        VX_CHECK(a);
        VX_CHECK(a.value().inserted);
        VX_CHECK(val(a.value().it->first) == 1);
        VX_CHECK(val(a.value().it->second) == 10);

        auto b = s.try_emplace(K(1), 20);
        VX_CHECK(b);
        VX_CHECK(!b.value().inserted);
        VX_CHECK(b.value().it == a.value().it);
        VX_CHECK(val(b.value().it->second) == 10); // not overwritten
        VX_CHECK(s.size() == 1);
    }

    VX_SECTION("no mapped arguments value-initializes the mapped value")
    {
        S s;
        auto r = s.try_emplace(K(2));
        VX_CHECK(r);
        VX_CHECK(r.value().inserted);
        VX_CHECK(val(r.value().it->second) == 0);
    }

    VX_SECTION("lvalue key")
    {
        if constexpr (copyable<K>)
        {
            S s;
            const K k(3);
            VX_CHECK(s.try_emplace(k, 30).value().inserted);
            VX_CHECK(!s.try_emplace(k, 31).value().inserted);
            VX_CHECK(val(*s.get(k)) == 30);
        }
    }

    VX_SECTION("rvalue key is consumed only when inserted")
    {
        S s;
        K a(4);
        VX_CHECK(s.try_emplace(std::move(a), 40).value().inserted);
        if constexpr (!std::is_same_v<K, int>)
        {
            VX_CHECK(val(a) == -1); // moved from
        }

        K b(4); // duplicate key: try_emplace must not move from it
        VX_CHECK(!s.try_emplace(std::move(b), 41).value().inserted);
        if constexpr (!std::is_same_v<K, int>)
        {
            VX_CHECK(val(b) == 4);
        }
        VX_CHECK(val(*s.get(K(4))) == 40);
    }

    VX_SECTION("many inserts with growth")
    {
        S s;
        for (int i = 0; i < 2000; ++i)
        {
            auto r = s.try_emplace(K(i), vof(i));
            VX_CHECK(r);
            VX_CHECK(r.value().inserted);
        }
        VX_CHECK(eq_model(s, range(0, 2000)));
        for (int i = 0; i < 2000; ++i)
        {
            VX_CHECK(!s.try_emplace(K(i), -1).value().inserted);
        }
        VX_CHECK(eq_model(s, range(0, 2000)));
    }
}
TEST_MAPS(try_emplace)

//=========================================================================
// insert_or_assign
//=========================================================================

template <typename S>
void test_insert_or_assign()
{
    using K = typename S::key_type;

    VX_SECTION("inserts when absent, assigns when present")
    {
        S s;
        auto a = s.insert_or_assign(K(1), 10);
        VX_CHECK(a);
        VX_CHECK(a.value().inserted);
        VX_CHECK(val(a.value().it->second) == 10);

        auto b = s.insert_or_assign(K(1), 20);
        VX_CHECK(b);
        VX_CHECK(!b.value().inserted);
        VX_CHECK(b.value().it == a.value().it);
        VX_CHECK(val(b.value().it->second) == 20); // overwritten
        VX_CHECK(s.size() == 1);

        VX_CHECK(s.insert_or_assign(K(2), 30).value().inserted);
        VX_CHECK(eq(s, { kv(1, 20), kv(2, 30) }));
    }

    VX_SECTION("lvalue key")
    {
        if constexpr (copyable<K>)
        {
            S s;
            const K k(7);
            VX_CHECK(s.insert_or_assign(k, 70).value().inserted);
            VX_CHECK(!s.insert_or_assign(k, 71).value().inserted);
            VX_CHECK(val(*s.get(k)) == 71);
        }
    }

    VX_SECTION("rvalue key is consumed only when inserted")
    {
        S s;
        K a(8);
        VX_CHECK(s.insert_or_assign(std::move(a), 80).value().inserted);

        K b(8);
        VX_CHECK(!s.insert_or_assign(std::move(b), 81).value().inserted);
        if constexpr (!std::is_same_v<K, int>)
        {
            VX_CHECK(val(b) == 8); // existing key: the argument key is untouched
        }
        VX_CHECK(val(*s.get(K(8))) == 81);
    }

    VX_SECTION("repeated overwrite and growth")
    {
        S s;
        for (int round = 0; round < 3; ++round)
        {
            for (int i = 0; i < 500; ++i)
            {
                auto r = s.insert_or_assign(K(i), round * 1000 + i);
                VX_CHECK(r);
                VX_CHECK(r.value().inserted == (round == 0));
            }
        }
        VX_CHECK(s.size() == 500);
        for (int i = 0; i < 500; ++i)
        {
            VX_CHECK(val(*s.get(K(i))) == 2000 + i);
        }
    }
}
TEST_MAPS(insert_or_assign)

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
            if (i % 2 == 1)
            {
                VX_CHECK(val(*s.get(K(i))) == vof(i)); // survivors keep their values
            }
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
            VX_CHECK(s.contains(K(val(next->first))));
        }

        size_t n = 0;
        for (auto i = s.begin(); i != s.end(); i = s.erase(i))
        {
            ++n;
        }
        VX_CHECK(n == 9);
        VX_CHECK(s.empty());
    }

    VX_SECTION("by const_iterator")
    {
        S s;
        fill(s, 0, 10);
        typename S::const_iterator cit = s.find(K(4));
        s.erase(cit);
        VX_CHECK(!s.contains(K(4)));
        VX_CHECK(s.size() == 9);
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
        VX_CHECK(s.try_emplace(K(1), 1).value().inserted); // still usable
    }

    VX_SECTION("erase every element in insertion order, then reinsert")
    {
        S s;
        fill(s, 0, 300);
        for (int i = 0; i < 300; ++i)
        {
            VX_CHECK(s.erase(K(i)) == 1);
        }
        VX_CHECK(s.empty());
        fill(s, 0, 300);
        VX_CHECK(eq_model(s, range(0, 300)));
    }

    VX_SECTION("reinserting an erased key gets the new value")
    {
        S s;
        fill(s, 0, 10);
        VX_CHECK(s.erase(K(3)) == 1);
        VX_CHECK(s.get(K(3)) == nullptr);
        VX_CHECK(s.try_emplace(K(3), 555).value().inserted);
        VX_CHECK(val(*s.get(K(3))) == 555);
    }

    VX_SECTION("sliding window churn")
    {
        S s;
        const int window = 16;
        const int total = 5000;
        for (int i = 0; i < total; ++i)
        {
            VX_CHECK(s.try_emplace(K(i), vof(i)).value().inserted);
            if (i >= window)
            {
                VX_CHECK(s.erase(K(i - window)) == 1);
            }
        }
        VX_CHECK(eq_model(s, range(total - window, total)));
    }

    VX_SECTION("erase of a missing key on a non-empty table")
    {
        S s;
        fill(s, 0, 10);
        VX_CHECK(s.erase(K(999)) == 0);
        VX_CHECK(s.size() == 10);
    }
}
TEST_MAPS(erase)

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
        VX_CHECK(eq_model(s, range(0, 2000)));
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
        VX_CHECK(eq_model(s, range(0, 50)));

        VX_CHECK(!s.reserve(s.max_size() + 1));
        VX_CHECK(eq_model(s, range(0, 50)));
    }

    VX_SECTION("reserve on a populated table keeps elements and values")
    {
        S s;
        fill(s, 0, 40);
        VX_CHECK(s.reserve(5000));
        VX_CHECK(s.capacity() >= 5000);
        VX_CHECK(eq_model(s, range(0, 40)));
        for (int i = 0; i < 40; ++i)
        {
            VX_CHECK(val(*s.get(K(i))) == vof(i));
        }
    }

    VX_SECTION("_assume_capacity variants after reserve")
    {
        S s;
        VX_CHECK(s.reserve(100));
        for (int i = 0; i < 100; ++i)
        {
            VX_CHECK(s.insert_assume_capacity(entry<S>(i)).inserted);
        }
        VX_CHECK(!s.insert_assume_capacity(entry<S>(5)).inserted);
        VX_CHECK(eq_model(s, range(0, 100)));
    }

    VX_SECTION("_unique variants")
    {
        S s;
        VX_CHECK(s.reserve(10));
        auto r = s.insert_assume_unique(entry<S>(1));
        VX_CHECK(r);
        VX_CHECK(val(r.value()->first) == 1);
        auto it = s.insert_assume_unique_and_capacity(entry<S>(2));
        VX_CHECK(val(it->first) == 2);
        VX_CHECK(eq(s, { kv(1, vof(1)), kv(2, vof(2)) }));
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
        VX_CHECK(s.get(K(5)) == nullptr);
        fill(s, 0, 100);
        VX_CHECK(eq_model(s, range(0, 100)));
    }

    VX_SECTION("clear_and_deallocate")
    {
        S s;
        fill(s, 0, 100);
        s.clear_and_deallocate();
        VX_CHECK(s.empty());
        VX_CHECK(s.capacity() == 0);
        VX_CHECK(s.begin() == s.end());
        fill(s, 0, 10);
        VX_CHECK(eq_model(s, range(0, 10)));
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
        VX_CHECK(eq(s, { kv(0, vof(0)), kv(1, vof(1)), kv(2, vof(2)) }));

        s.clear();
        VX_CHECK(s.shrink_to_fit());
        VX_CHECK(s.capacity() == 0);
        VX_CHECK(s.try_emplace(K(1), 1).value().inserted); // usable after deallocating
    }
}
TEST_MAPS(growth)

// reserve(n) must guarantee that n elements fit without another rehash,
// right at the growth boundaries.
template <typename S>
void test_load_boundary()
{
    using K = typename S::key_type;

    const int sizes[] = { 1, 2, 3, 7, 8, 9, 14, 15, 16, 17, 31, 32, 33, 63, 64, 65, 127, 128, 129, 1000 };
    for (int n : sizes)
    {
        S s;
        VX_CHECK(s.reserve(static_cast<size_t>(n)));
        const size_t cap = s.capacity();
        VX_CHECK(cap >= static_cast<size_t>(n));

        for (int i = 0; i < n; ++i)
        {
            VX_CHECK(s.insert_assume_capacity(entry<S>(i)).inserted);
        }
        VX_CHECK(s.capacity() == cap); // no rehash happened
        VX_CHECK(s.size() == static_cast<size_t>(n));
        VX_CHECK(eq_model(s, range(0, n)));
    }

    // size never exceeds capacity at any step of normal growth
    S s;
    for (int i = 0; i < 3000; ++i)
    {
        VX_CHECK(s.try_emplace(K(i), vof(i)));
        VX_CHECK(s.size() <= s.capacity());
    }
}
TEST_MAPS(load_boundary)

//=========================================================================
// copy / move / swap / compare
//=========================================================================

template <typename S>
void test_copy()
{
    using K = typename S::key_type;

    VX_SECTION("copy is independent (keys and mapped values)")
    {
        S a;
        fill(a, 0, 50);
        S b(a);
        VX_CHECK(a == b);
        VX_CHECK(eq_model(b, range(0, 50)));

        VX_CHECK(b.try_emplace(K(100), 1000).value().inserted);
        VX_CHECK(b.erase(K(0)) == 1);
        *b.get(K(1)) = 4242;
        VX_CHECK(eq_model(a, range(0, 50))); // a untouched, including mapped values
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
        VX_CHECK(eq_model(b, range(0, 30)));

        S& self = b;
        b = self;
        VX_CHECK(eq_model(b, range(0, 30)));

        // assigning an empty map empties the target
        S empty;
        b = empty;
        VX_CHECK(b.empty());
    }

    VX_SECTION("copy of a table with tombstones")
    {
        S a;
        fill(a, 0, 100);
        for (int i = 0; i < 100; i += 3)
        {
            VX_CHECK(a.erase(K(i)) == 1);
        }
        S b(a);
        VX_CHECK(a == b);
        VX_CHECK(b.size() == a.size());
        for (int i = 0; i < 100; ++i)
        {
            VX_CHECK(b.contains(K(i)) == (i % 3 != 0));
            if (i % 3 != 0)
            {
                VX_CHECK(val(*b.get(K(i))) == vof(i));
            }
        }
    }
}
TEST_MAPS_COPYABLE(copy)

template <typename S>
void test_move_swap()
{
    using K = typename S::key_type;

    VX_SECTION("move construct")
    {
        S a;
        fill(a, 0, 50);
        S b(std::move(a));
        VX_CHECK(eq_model(b, range(0, 50)));
        VX_CHECK(a.empty());
        VX_CHECK(a.begin() == a.end());
        VX_CHECK(!a.contains(K(1)));
        VX_CHECK(a.get(K(1)) == nullptr);
        VX_CHECK(a.try_emplace(K(1), 1).value().inserted); // moved-from is reusable
    }

    VX_SECTION("move assign")
    {
        S b;
        fill(b, 0, 50);
        S c;
        fill(c, 500, 510);
        c = std::move(b);
        VX_CHECK(eq_model(c, range(0, 50)));
        VX_CHECK(b.empty());
        VX_CHECK(b.try_emplace(K(1), 1).value().inserted);
    }

    VX_SECTION("move of an empty table")
    {
        S a;
        S b(std::move(a));
        VX_CHECK(b.empty());
        VX_CHECK(a.empty());
        VX_CHECK(b.try_emplace(K(1), 1).value().inserted);
    }

    VX_SECTION("swap")
    {
        S a;
        S b;
        fill(a, 0, 10);
        fill(b, 100, 103);
        a.swap(b);
        VX_CHECK(eq_model(a, range(100, 103)));
        VX_CHECK(eq_model(b, range(0, 10)));

        S e;
        a.swap(e);
        VX_CHECK(a.empty());
        VX_CHECK(eq_model(e, range(100, 103)));

        // both empty
        S x;
        S y;
        x.swap(y);
        VX_CHECK(x.empty() && y.empty());
    }

    VX_SECTION("self swap / self move assign leave a usable table")
    {
        S a;
        fill(a, 0, 20);
        a.swap(a);
        VX_CHECK(eq_model(a, range(0, 20)));

        S& r = a;
        a = std::move(r);
        // valid but unspecified: just require it is still a working map
        VX_CHECK(a.try_emplace(K(1000), 1));
        VX_CHECK(a.contains(K(1000)));
        VX_CHECK(a.size() == static_cast<size_t>(std::distance(a.begin(), a.end())));
    }

    VX_SECTION("equality ignores insertion order and capacity")
    {
        S a;
        S b;
        fill(a, 0, 40);
        for (int i = 39; i >= 0; --i)
        {
            VX_CHECK(b.try_emplace(K(i), vof(i)));
        }
        VX_CHECK(b.reserve(500)); // different capacity
        VX_CHECK(a == b);
        VX_CHECK(b.erase(K(7)) == 1);
        VX_CHECK(a != b);
        VX_CHECK(b.try_emplace(K(7), vof(7)));
        VX_CHECK(a == b);
        VX_CHECK(S() == S());

        // same size, different keys
        S c;
        S d;
        fill(c, 0, 5);
        fill(d, 1, 6);
        VX_CHECK(c != d);
    }

    VX_SECTION("equality compares mapped values, not just keys")
    {
        S a;
        S b;
        fill(a, 0, 10);
        fill(b, 0, 10);
        VX_CHECK(a == b);
        *b.get(K(4)) = -5;
        VX_CHECK(a != b); // same keys, same size, one different value
        *b.get(K(4)) = vof(4);
        VX_CHECK(a == b);
    }
}
TEST_MAPS(move_swap)

//=========================================================================
// assign / observers
//=========================================================================

template <typename S>
void test_assign()
{
    using K = typename S::key_type;
    using V = typename S::value_type;

    VX_SECTION("assign(const&) / assign(&&)")
    {
        S a;
        fill(a, 0, 20);
        S b;
        fill(b, 100, 110);

        VX_CHECK(b.assign(a));
        VX_CHECK(b == a);

        S c;
        fill(c, 200, 205);
        VX_CHECK(b.assign(std::move(c)));
        VX_CHECK(eq_model(b, range(200, 205)));

        VX_CHECK(b.assign(S()));
        VX_CHECK(b.empty());
    }

    VX_SECTION("assign(range)")
    {
        std::vector<V> src;
        for (int i = 0; i < 30; ++i)
        {
            src.push_back(entry<S>(i % 15));
        }
        S s;
        fill(s, 100, 120);
        VX_CHECK(s.assign(src.begin(), src.end()));
        VX_CHECK(eq_model(s, range(0, 15)));

        std::vector<V> none;
        VX_CHECK(s.assign(none.begin(), none.end()));
        VX_CHECK(s.empty());
    }

    VX_SECTION("assign(initializer_list) / operator=(initializer_list)")
    {
        S s;
        fill(s, 100, 120);
        VX_CHECK(s.assign(std::initializer_list<V>{ V(K(1), 10), V(K(2), 20), V(K(2), 21), V(K(3), 30) }));
        VX_CHECK(eq(s, { kv(1, 10), kv(2, 20), kv(3, 30) }));

        s = std::initializer_list<V>{ V(K(7), 70), V(K(8), 80) };
        VX_CHECK(eq(s, { kv(7, 70), kv(8, 80) }));
    }
}
TEST_MAPS_COPYABLE(assign)

VX_TEST_CASE(observers_stateful_functors)
{
    using S = vx::unordered_map<int, int, seeded_hash, tagged_eq>;

    S a(seeded_hash(7), tagged_eq(9));
    VX_CHECK(a.hash_function().seed == 7);
    VX_CHECK(a.key_eq().tag == 9);
    fill(a, 0, 50);

    S copy(a);
    VX_CHECK(copy.hash_function().seed == 7);
    VX_CHECK(copy.key_eq().tag == 9);
    VX_CHECK(copy == a);

    S moved(std::move(copy));
    VX_CHECK(moved.hash_function().seed == 7);
    VX_CHECK(moved.key_eq().tag == 9);
    VX_CHECK(eq_model(moved, range(0, 50)));

    auto made = S::create(a);
    VX_CHECK(made);
    VX_CHECK(made.value().hash_function().seed == 7);
    VX_CHECK(made.value().key_eq().tag == 9);

    // swap exchanges the functors together with the contents
    S b(seeded_hash(1), tagged_eq(2));
    fill(b, 100, 110);
    a.swap(b);
    VX_CHECK(a.hash_function().seed == 1);
    VX_CHECK(a.key_eq().tag == 2);
    VX_CHECK(b.hash_function().seed == 7);
    VX_CHECK(b.key_eq().tag == 9);
    VX_CHECK(eq_model(a, range(100, 110)));
    VX_CHECK(eq_model(b, range(0, 50)));
    for (int i = 100; i < 110; ++i)
    {
        VX_CHECK(a.contains(i));
        VX_CHECK(*a.get(i) == vof(i));
    }
}

//=========================================================================
// iteration
//=========================================================================

template <typename S>
void test_iteration()
{
    using K = typename S::key_type;
    using It = typename S::iterator;
    using CIt = typename S::const_iterator;

    VX_STATIC_CHECK((std::is_convertible_v<It, CIt>));
    VX_STATIC_CHECK((std::is_convertible_v<typename std::iterator_traits<It>::iterator_category, std::forward_iterator_tag>));
    VX_STATIC_CHECK((std::is_convertible_v<typename std::iterator_traits<CIt>::iterator_category, std::forward_iterator_tag>));

    S s;
    VX_CHECK(s.begin() == s.end());
    VX_CHECK(s.cbegin() == s.cend());

    fill(s, 0, 100);
    size_t n = 0;
    for (auto it = s.begin(); it != s.end(); ++it)
    {
        ++n;
    }
    VX_CHECK(n == 100);
    VX_CHECK(eq_model(s, range(0, 100)));

    // range-for, mutable and const
    n = 0;
    for (const auto& e : s)
    {
        VX_CHECK(val(e.second) == vof(val(e.first)));
        ++n;
    }
    VX_CHECK(n == 100);

    const S& cs = s;
    n = 0;
    for (const auto& e : cs)
    {
        VX_CHECK(val(e.second) == vof(val(e.first)));
        ++n;
    }
    VX_CHECK(n == 100);

    // post-increment returns the old position; const iteration
    n = 0;
    for (auto it = cs.cbegin(); it != cs.cend(); it++)
    {
        ++n;
    }
    VX_CHECK(n == 100);

    {
        auto it = s.begin();
        auto old = it++;
        VX_CHECK(old == s.begin());
        VX_CHECK(it != old);
    }

    // iterator converts to const_iterator and compares equal
    {
        It it = s.begin();
        CIt cit = it;
        VX_CHECK(cit == cs.begin());
        VX_CHECK(cit == it);
    }

    // each key visited exactly once
    {
        std::vector<kv> seen = contents(s);
        VX_CHECK(std::adjacent_find(seen.begin(), seen.end(),
                     [](const kv& a, const kv& b)
                     {
                         return a.first == b.first;
                     }) == seen.end());
    }

    // sparse table (many erased): iteration still visits exactly the survivors
    for (int i = 0; i < 100; ++i)
    {
        if (i % 10 != 0)
        {
            VX_CHECK(s.erase(K(i)) == 1);
        }
    }
    {
        std::vector<kv> expected;
        for (int i = 0; i < 100; i += 10)
        {
            expected.push_back(kv(i, vof(i)));
        }
        VX_CHECK(eq_model(s, expected));
    }

    // single element at the very end of a large table
    S big;
    VX_CHECK(big.reserve(1000));
    VX_CHECK(big.try_emplace(K(42), 7));
    n = 0;
    for (const auto& e : big)
    {
        VX_CHECK(val(e.first) == 42);
        VX_CHECK(val(e.second) == 7);
        ++n;
    }
    VX_CHECK(n == 1);
}
TEST_MAPS(iteration)

//=========================================================================
// default hasher
//=========================================================================

VX_TEST_CASE(default_hasher)
{
    using S = vx::unordered_map<int, int>;

    S s;
    for (int i = 0; i < 1000; ++i)
    {
        VX_CHECK(s.try_emplace(i, vof(i)));
    }
    VX_CHECK(s.size() == 1000);
    for (int i = 0; i < 1000; ++i)
    {
        VX_CHECK(s.contains(i));
        VX_CHECK(*s.get(i) == vof(i));
    }
    VX_CHECK(!s.contains(1000));
    VX_CHECK(s.erase(500) == 1);
    VX_CHECK(!s.contains(500));

    // negative and extreme values
    VX_CHECK(s.try_emplace(-1, 1));
    VX_CHECK(s.try_emplace(INT32_MIN, 2));
    VX_CHECK(s.try_emplace(INT32_MAX, 3));
    VX_CHECK(s.contains(-1) && s.contains(INT32_MIN) && s.contains(INT32_MAX));
    VX_CHECK(*s.get(-1) == 1);
    VX_CHECK(*s.get(INT32_MIN) == 2);
    VX_CHECK(*s.get(INT32_MAX) == 3);
}

//=========================================================================
// hash quality edge cases
//=========================================================================

template <typename K>
void test_degenerate_hash()
{
    using S = degenerate_map<K>;

    S s;
    for (int i = 0; i < 300; ++i)
    {
        VX_CHECK(s.try_emplace(K(i), vof(i)).value().inserted);
    }
    VX_CHECK(!s.try_emplace(K(150), -1).value().inserted);
    VX_CHECK(eq_model(s, range(0, 300)));
    for (int i = 0; i < 300; ++i)
    {
        VX_CHECK(s.contains(K(i)));
        VX_CHECK(val(*s.get(K(i))) == vof(i));
    }
    VX_CHECK(!s.contains(K(300)));
    VX_CHECK(s.get(K(300)) == nullptr);

    // erase in the middle of long probe chains, then look everything up again
    for (int i = 0; i < 300; i += 2)
    {
        VX_CHECK(s.erase(K(i)) == 1);
    }
    for (int i = 0; i < 300; ++i)
    {
        VX_CHECK(s.contains(K(i)) == (i % 2 == 1));
        if (i % 2 == 1)
        {
            VX_CHECK(val(*s.get(K(i))) == vof(i));
        }
    }

    // tombstone-heavy reinsertion
    for (int i = 0; i < 300; i += 2)
    {
        VX_CHECK(s.try_emplace(K(i), vof(i)).value().inserted);
    }
    VX_CHECK(eq_model(s, range(0, 300)));

    // insert_or_assign / operator[] through a single long chain
    for (int i = 0; i < 300; ++i)
    {
        VX_CHECK(!s.insert_or_assign(K(i), 7).value().inserted);
    }
    VX_CHECK(eq_model(s, range_with(0, 300, 7)));
    for (int i = 0; i < 300; ++i)
    {
        s[K(i)] = vof(i);
    }
    VX_CHECK(eq_model(s, range(0, 300)));

    // churn through a single probe chain
    for (int i = 300; i < 1500; ++i)
    {
        VX_CHECK(s.try_emplace(K(i), vof(i)).value().inserted);
        VX_CHECK(s.erase(K(i - 300)) == 1);
    }
    VX_CHECK(eq_model(s, range(1200, 1500)));
}
VX_TEST_CASE(degenerate_hash_int)
{
    test_degenerate_hash<int>();
}
VX_TEST_CASE(degenerate_hash_copyable)
{
    test_degenerate_hash<copyable_int>();
}
VX_TEST_CASE(degenerate_hash_movable)
{
    test_degenerate_hash<movable_int>();
}

template <typename K>
void test_avalanching_hash()
{
    using S = avalanche_map<K>;

    S s;
    fill(s, 0, 2000);
    VX_CHECK(eq_model(s, range(0, 2000)));
    for (int i = 0; i < 2000; ++i)
    {
        VX_CHECK(s.contains(K(i)));
        VX_CHECK(val(*s.get(K(i))) == vof(i));
    }
    for (int i = 0; i < 2000; i += 2)
    {
        VX_CHECK(s.erase(K(i)) == 1);
    }
    VX_CHECK(s.size() == 1000);
    VX_CHECK(s.shrink_to_fit());
    for (int i = 0; i < 2000; ++i)
    {
        VX_CHECK(s.contains(K(i)) == (i % 2 == 1));
        if (i % 2 == 1)
        {
            VX_CHECK(val(*s.get(K(i))) == vof(i));
        }
    }
}
VX_TEST_CASE(avalanching_hash_int)
{
    test_avalanching_hash<int>();
}
VX_TEST_CASE(avalanching_hash_movable)
{
    test_avalanching_hash<movable_int>();
}

// capacity must stay bounded when elements are constantly inserted and erased
template <typename S>
void test_tombstone_churn()
{
    using K = typename S::key_type;

    VX_SECTION("sliding window does not grow without bound")
    {
        S s;
        const int window = 16;
        const int total = 20000;
        for (int i = 0; i < total; ++i)
        {
            VX_CHECK(s.try_emplace(K(i), vof(i)));
            if (i >= window)
            {
                VX_CHECK(s.erase(K(i - window)) == 1);
            }
        }
        VX_CHECK(eq_model(s, range(total - window, total)));
        VX_CHECK(s.capacity() < 1024);
    }

    VX_SECTION("fill and drain cycles keep capacity stable")
    {
        S s;
        size_t cap_after_first = 0;
        for (int round = 0; round < 50; ++round)
        {
            fill(s, 0, 100);
            for (int i = 0; i < 100; ++i)
            {
                VX_CHECK(s.erase(K(i)) == 1);
            }
            VX_CHECK(s.empty());
            if (round == 0)
            {
                cap_after_first = s.capacity();
            }
        }
        VX_CHECK(s.capacity() <= cap_after_first * 2);
    }

    VX_SECTION("operator[] / get_or_insert churn does not grow without bound")
    {
        S s;
        const int window = 16;
        for (int i = 0; i < 20000; ++i)
        {
            s[K(i)] = vof(i);
            if (i >= window)
            {
                VX_CHECK(s.erase(K(i - window)) == 1);
            }
        }
        VX_CHECK(s.size() == static_cast<size_t>(window));
        VX_CHECK(s.capacity() < 1024);
    }
}
TEST_MAPS(tombstone_churn)

//=========================================================================
// transparent (heterogeneous) lookup
//=========================================================================

VX_TEST_CASE(transparent_lookup)
{
    using S = vx::unordered_map<wide_key, int, wide_hash, wide_eq>;

    S s;
    for (int i = 0; i < 50; ++i)
    {
        VX_CHECK(s.try_emplace(wide_key(i), vof(i)));
    }

    // lookups by int: no wide_key temporary needed
    VX_CHECK(s.find(7) != s.end());
    VX_CHECK(s.find(7)->first.v == 7);
    VX_CHECK(s.find(7)->second == vof(7));
    VX_CHECK(s.find(70) == s.end());
    VX_CHECK(s.contains(7));
    VX_CHECK(!s.contains(70));
    VX_CHECK(s.count(7) == 1);
    VX_CHECK(s.count(70) == 0);

    // get / at by int
    VX_CHECK(s.get(7) != nullptr);
    VX_CHECK(*s.get(7) == vof(7));
    VX_CHECK(s.get(70) == nullptr);
    VX_CHECK(s.at(7));
    VX_CHECK(s.at(7).value() == vof(7));
    VX_CHECK(!s.at(70));

    const S& cs = s;
    VX_CHECK(cs.find(8) != cs.end());
    VX_CHECK(cs.contains(8));
    VX_CHECK(cs.get(8) != nullptr);
    VX_CHECK(*cs.get(8) == vof(8));
    VX_CHECK(cs.at(8));
    VX_CHECK(!cs.at(80));

    // lookups by key type still work
    VX_CHECK(s.contains(wide_key(9)));
    VX_CHECK(*s.get(wide_key(9)) == vof(9));

    // erase by heterogeneous key
    VX_CHECK(s.erase(7));
    VX_CHECK(!s.erase(7));
    VX_CHECK(!s.contains(7));
    VX_CHECK(s.get(7) == nullptr);

    // erase(iterator) must still pick the iterator overload, not erase<Q>
    auto next = s.erase(s.find(9));
    (void)next;
    VX_CHECK(!s.contains(9));

    VX_CHECK(s.erase(wide_key(10)));
    VX_CHECK(s.size() == 47);

    // insertion APIs still take the real key type
    VX_CHECK(s.try_emplace(wide_key(1000), 5).value().inserted);
    VX_CHECK(!s.insert_or_assign(wide_key(1000), 6).value().inserted);
    VX_CHECK(*s.get(1000) == 6);
    VX_CHECK(*s.get_or_insert(wide_key(1001)).value() == 0);
    VX_CHECK(s.contains(1001));
}

//=========================================================================
// move-only mapped values
//=========================================================================

VX_TEST_CASE(move_only_mapped_values)
{
    using K = int;
    using V = movable_int;
    using S = vx::unordered_map<K, V, test_hash<K>>;

    VX_STATIC_CHECK(!std::is_copy_constructible_v<V>);

    VX_SECTION("try_emplace constructs in place")
    {
        S s;
        VX_CHECK(s.try_emplace(1, 10).value().inserted);
        VX_CHECK(s.try_emplace(2, V(20)).value().inserted);
        VX_CHECK(val(*s.get(1)) == 10);
        VX_CHECK(val(*s.get(2)) == 20);
    }

    VX_SECTION("try_emplace does not consume the argument when the key exists")
    {
        S s;
        VX_CHECK(s.try_emplace(1, 10));
        V v(99);
        auto r = s.try_emplace(1, std::move(v));
        VX_CHECK(r);
        VX_CHECK(!r.value().inserted);
        VX_CHECK(val(v) == 99); // not moved-from
        VX_CHECK(val(*s.get(1)) == 10);

        V w(55);
        VX_CHECK(s.try_emplace(2, std::move(w)).value().inserted);
        VX_CHECK(val(w) == -1); // moved-from sentinel
        VX_CHECK(val(*s.get(2)) == 55);
    }

    VX_SECTION("insert_or_assign move-assigns")
    {
        S s;
        V a(1);
        VX_CHECK(s.insert_or_assign(5, std::move(a)).value().inserted);
        VX_CHECK(val(a) == -1);

        V b(2);
        VX_CHECK(!s.insert_or_assign(5, std::move(b)).value().inserted);
        VX_CHECK(val(b) == -1);
        VX_CHECK(val(*s.get(5)) == 2);
    }

    VX_SECTION("operator[] and get_or_insert")
    {
        S s;
        s[3] = V(30);
        VX_CHECK(val(s[3]) == 30);
        auto r = s.get_or_insert(4);
        VX_CHECK(r);
        *r.value() = V(40);
        VX_CHECK(val(*s.get(4)) == 40);
        VX_CHECK(s.at(4));
    }

    VX_SECTION("growth, reserve and shrink relocate by move")
    {
        S s;
        for (int i = 0; i < 2000; ++i)
        {
            VX_CHECK(s.try_emplace(i, vof(i)));
        }
        VX_CHECK(s.reserve(10000));
        for (int i = 3; i < 2000; ++i)
        {
            VX_CHECK(s.erase(i) == 1);
        }
        VX_CHECK(s.shrink_to_fit());
        VX_CHECK(s.size() == 3);
        for (int i = 0; i < 3; ++i)
        {
            VX_CHECK(val(*s.get(i)) == vof(i));
        }
    }

    VX_SECTION("move construct / assign / swap")
    {
        S a;
        for (int i = 0; i < 20; ++i)
        {
            VX_CHECK(a.try_emplace(i, vof(i)));
        }
        S b(std::move(a));
        VX_CHECK(a.empty());
        VX_CHECK(b.size() == 20);

        S c;
        VX_CHECK(c.try_emplace(1000, 1));
        c = std::move(b);
        VX_CHECK(c.size() == 20);
        VX_CHECK(val(*c.get(7)) == vof(7));

        S d;
        d.swap(c);
        VX_CHECK(c.empty());
        VX_CHECK(d.size() == 20);
    }
}

VX_TEST_CASE(move_only_keys_and_mapped_values)
{
    using K = movable_int;
    using V = movable_int;
    using S = vx::unordered_map<K, V, test_hash<K>>;

    S s;
    for (int i = 0; i < 1000; ++i)
    {
        VX_CHECK(s.try_emplace(K(i), V(vof(i))).value().inserted);
    }
    VX_CHECK(s.size() == 1000);
    for (int i = 0; i < 1000; ++i)
    {
        VX_CHECK(val(*s.get(K(i))) == vof(i));
    }

    // consuming insert_or_assign
    K k(5);
    V v(-5);
    VX_CHECK(!s.insert_or_assign(std::move(k), std::move(v)).value().inserted);
    VX_CHECK(val(*s.get(K(5))) == -5);

    // operator[] / get_or_insert with move-only key and mapped
    s[K(2000)] = V(1);
    VX_CHECK(val(*s.get(K(2000))) == 1);
    VX_CHECK(*s.get_or_insert(K(2001)).value() == V(0) || val(*s.get(K(2001))) == 0);

    // erase and iterate
    for (int i = 0; i < 1000; i += 2)
    {
        VX_CHECK(s.erase(K(i)) == 1);
    }
    size_t n = 0;
    for (const auto& e : s)
    {
        VX_CHECK(val(e.first) >= 0);
        ++n;
    }
    VX_CHECK(n == s.size());

    // create from rvalue
    auto r = S::create(std::move(s));
    VX_CHECK(r);
    VX_CHECK(r.value().size() == n);
}

//=========================================================================
// element lifetime (constructs vs destroys, copies vs moves)
// Both the key and the mapped value are counted_key, so live == 2 * size.
//=========================================================================

VX_TEST_CASE(lifetime_counted_elements)
{
    auto& st = counted_key::stats();
    counted_key::reset();

    {
        counted_map s;
        counted_map m;
        counted_map o;

        auto live_matches = [&]()
        {
            return st.live == 2 * (s.size() + m.size() + o.size()) && st.bad == 0;
        };

        VX_SECTION("try_emplace + growth: rvalue inserts never copy")
        {
            s.clear();
            m.clear();
            o.clear();

            for (int i = 0; i < 1000; ++i)
            {
                VX_CHECK(s.try_emplace(counted_key(i), i));
            }
            VX_CHECK(s.size() == 1000);
            VX_CHECK(live_matches());
            VX_CHECK(st.copies == 0); // relocation on growth moves

            for (int i = 1000; i < 1100; ++i)
            {
                VX_CHECK(s.insert_or_assign(counted_key(i), counted_key(i)));
            }
            VX_CHECK(live_matches());
            VX_CHECK(st.copies == 0);
        }

        VX_SECTION("operator[] / get_or_insert")
        {
            s.clear();
            m.clear();
            o.clear();

            for (int i = 0; i < 300; ++i)
            {
                s[counted_key(i)] = counted_key(i);
            }

            VX_CHECK(s.size() == 300);
            VX_CHECK(live_matches());
        }

        VX_SECTION("duplicate try_emplace constructs nothing that survives")
        {
            s.clear();
            m.clear();
            o.clear();

            VX_CHECK(s.try_emplace(counted_key(5), 5));
            const size_t constructed_before = st.constructed;
            const size_t destroyed_before = st.destroyed;
            for (int i = 0; i < 10; ++i)
            {
                VX_CHECK(!s.try_emplace(counted_key(5), 6).value().inserted);
            }
            VX_CHECK(s.size() == 1);
            VX_CHECK(live_matches());
            // only the temporary keys came and went: anything built is also destroyed
            VX_CHECK(st.constructed - constructed_before == st.destroyed - destroyed_before);
        }

        VX_SECTION("insert_or_assign overwrite destroys nothing extra")
        {
            s.clear();
            m.clear();
            o.clear();

            VX_CHECK(s.try_emplace(counted_key(1), 1));
            for (int i = 0; i < 20; ++i)
            {
                VX_CHECK(!s.insert_or_assign(counted_key(1), counted_key(i)).value().inserted);
            }
            VX_CHECK(s.size() == 1);
            VX_CHECK(live_matches());
        }

        VX_SECTION("erase destroys key and mapped value")
        {
            s.clear();
            m.clear();
            o.clear();

            for (int i = 1000; i < 1200; ++i)
            {
                VX_CHECK(s.try_emplace(counted_key(i), i));
            }
            for (int i = 1000; i < 1200; i += 2)
            {
                VX_CHECK(s.erase(counted_key(i)) == 1);
                VX_CHECK(live_matches());
            }
            for (auto it = s.begin(); it != s.end();)
            {
                it = s.erase(it);
            }
            VX_CHECK(s.empty());
            VX_CHECK(st.live == 0);
        }

        VX_SECTION("clear / shrink / reserve")
        {
            s.clear();
            m.clear();
            o.clear();

            for (int i = 0; i < 200; ++i)
            {
                VX_CHECK(s.try_emplace(counted_key(i), i));
            }
            for (int i = 0; i < 200; i += 2)
            {
                VX_CHECK(s.erase(counted_key(i)) == 1);
            }
            const size_t copies_before = st.copies;
            VX_CHECK(s.shrink_to_fit());
            VX_CHECK(s.reserve(5000));
            VX_CHECK(st.copies == copies_before); // rehash moves
            VX_CHECK(s.size() == 100);
            VX_CHECK(live_matches());

            s.clear();
            VX_CHECK(s.empty());
            VX_CHECK(st.live == 2 * (m.size() + o.size()));
            VX_CHECK(st.bad == 0);
        }

        VX_SECTION("copy / move / swap / assign")
        {
            s.clear();
            m.clear();
            o.clear();

            for (int i = 0; i < 100; ++i)
            {
                VX_CHECK(s.try_emplace(counted_key(i), i));
            }
            const size_t copies_before = st.copies;

            counted_map c(s);
            VX_CHECK(st.copies - copies_before == 200); // one copy per key and per mapped value
            VX_CHECK(st.live == 2 * (s.size() + c.size()));

            const size_t constructed_before = st.constructed;
            m = std::move(c);
            VX_CHECK(st.constructed == constructed_before); // buffer transfer, no per-element work
            VX_CHECK(c.empty());
            VX_CHECK(live_matches());

            o.swap(m);
            VX_CHECK(m.empty());
            VX_CHECK(o.size() == 100);
            VX_CHECK(live_matches());

            // overwriting a populated table destroys its old elements
            for (int i = 500; i < 520; ++i)
            {
                VX_CHECK(m.try_emplace(counted_key(i), i));
            }
            m = s;
            VX_CHECK(m == s);
            VX_CHECK(live_matches());

            VX_CHECK(m.assign(counted_map()));
            VX_CHECK(m.empty());
            VX_CHECK(live_matches());
        }

        VX_SECTION("self operations")
        {
            s.clear();
            m.clear();
            o.clear();

            for (int i = 0; i < 50; ++i)
            {
                VX_CHECK(s.try_emplace(counted_key(i), i));
            }
            s.swap(s);
            VX_CHECK(live_matches());
            VX_CHECK(s.size() == 50);
        }
    }

    // everything destroyed exactly once, nothing touched after destruction
    VX_CHECK(st.live == 0);
    VX_CHECK(st.constructed == st.destroyed);
    VX_CHECK(st.bad == 0);
}

//=========================================================================
// allocator tracking
//=========================================================================

VX_TEST_CASE(allocator_no_leaks)
{
    using K = copyable_int;
    using A = tracking_allocator<std::pair<const K, int>>;
    using S = tracked_map<K>;

    allocator_stats stats;
    {
        S s(test_hash<K>(), std::equal_to<K>(), A(1, &stats));
        VX_CHECK(s.get_allocator().id() == 1);
        VX_CHECK(stats.allocate_calls == 0); // empty table does not allocate

        fill(s, 0, 500);
        VX_CHECK(stats.allocate_calls > 0);
        VX_CHECK(stats.live_allocations > 0);

        s.clear();
        VX_CHECK(stats.live_allocations > 0); // clear keeps the storage
        s.clear_and_deallocate();
        VX_CHECK(stats.live_allocations == 0);
        VX_CHECK(s.capacity() == 0);

        fill(s, 0, 100);

        // copy uses the same allocator (and stats)
        S c(s);
        VX_CHECK(c == s);
        VX_CHECK(c.get_allocator().id() == 1);

        // moving transfers the buffer: no new allocation
        const size_t before = stats.allocate_calls;
        S m(std::move(c));
        VX_CHECK(stats.allocate_calls == before);
        VX_CHECK(m == s);
        VX_CHECK(m.get_allocator().id() == 1);

        // overwriting / looking up existing keys never allocates
        const size_t before_lookup = stats.allocate_calls;
        for (int i = 0; i < 100; ++i)
        {
            VX_CHECK(!s.try_emplace(K(i), -1).value().inserted);
            VX_CHECK(!s.insert_or_assign(K(i), vof(i)).value().inserted);
            VX_CHECK(s.get_or_insert(K(i)));
            VX_CHECK(s.get(K(i)) != nullptr);
        }
        VX_CHECK(stats.allocate_calls == before_lookup);

        s.clear();
        m.clear();
        VX_CHECK(s.shrink_to_fit());
        VX_CHECK(m.shrink_to_fit());
        VX_CHECK(stats.live_allocations == 0);
    }
    VX_CHECK(stats.live_allocations == 0);
    VX_CHECK(stats.allocate_calls == stats.deallocate_calls);
}

VX_TEST_CASE(allocator_get_allocator_round_trips)
{
    // the table rebinds the allocator internally: get_allocator() must still
    // return the original allocator type with the original state
    using K = copyable_int;
    using A = tracking_allocator<std::pair<const K, int>>;
    using S = tracked_map<K>;

    allocator_stats stats;
    S s(test_hash<K>(), std::equal_to<K>(), A(42, &stats));
    fill(s, 0, 100);

    A a = s.get_allocator();
    VX_CHECK(a.id() == 42);
    VX_CHECK(a.stats() == &stats);
    VX_STATIC_CHECK((std::is_same_v<typename S::allocator_type, A>));
}

VX_TEST_CASE(allocator_propagation)
{
    using K = copyable_int;
    using A = tracking_allocator<std::pair<const K, int>>;
    using S = tracked_map<K>;

    allocator_stats s1;
    allocator_stats s2;
    {
        S a(test_hash<K>(), std::equal_to<K>(), A(1, &s1));
        fill(a, 0, 20);

        auto same = S::create(a); // keeps a's allocator
        VX_CHECK(same);
        VX_CHECK(same.value().get_allocator().id() == 1);
        VX_CHECK(same.value() == a);

        auto other = S::create(a, A(2, &s2)); // re-inserts into a different allocator
        VX_CHECK(other);
        VX_CHECK(other.value().get_allocator().id() == 2);
        VX_CHECK(other.value() == a);
        VX_CHECK(s2.live_allocations > 0);
    }
    VX_CHECK(s1.live_allocations == 0);
    VX_CHECK(s2.live_allocations == 0);
}

VX_TEST_CASE(allocator_assign_and_swap)
{
    using K = copyable_int;
    using A = tracking_allocator<std::pair<const K, int>>;
    using S = tracked_map<K>;

    allocator_stats s1;
    allocator_stats s2;
    {
        S a(test_hash<K>(), std::equal_to<K>(), A(1, &s1));
        S b(test_hash<K>(), std::equal_to<K>(), A(2, &s2));
        fill(a, 0, 30);
        fill(b, 100, 140);

        // copy assign between unequal allocators: whichever allocator the
        // implementation picks, contents must be right and nothing may leak
        a = b;
        VX_CHECK(eq_model(a, range(100, 140)));
        VX_CHECK(eq_model(b, range(100, 140)));

        // move assign between unequal allocators
        S c(test_hash<K>(), std::equal_to<K>(), A(3, &s1));
        fill(c, 500, 520);
        c = std::move(b);
        VX_CHECK(eq_model(c, range(100, 140)));

        // swap with EQUAL allocators (swap with unequal ones is undefined for std containers)
        S d(test_hash<K>(), std::equal_to<K>(), A(1, &s1));
        fill(d, 900, 910);
        S e(test_hash<K>(), std::equal_to<K>(), A(1, &s1));
        fill(e, 800, 805);
        d.swap(e);
        VX_CHECK(eq_model(d, range(800, 805)));
        VX_CHECK(eq_model(e, range(900, 910)));
    }
    VX_CHECK(s1.live_allocations == 0);
    VX_CHECK(s2.live_allocations == 0);
    VX_CHECK(s1.allocate_calls == s1.deallocate_calls);
    VX_CHECK(s2.allocate_calls == s2.deallocate_calls);
}

//=========================================================================
// allocation failure injection
//=========================================================================

VX_TEST_CASE(failure_insert_sweep)
{
    // fail the Nth allocation for every N: the table must report the error,
    // keep exactly what was inserted so far, and stay usable
    using A = failing_allocator<std::pair<const int, int>>;
    using S = failing_map<int>;

    for (long budget = 0; budget < 10; ++budget)
    {
        fail_state st;
        st.countdown = budget;
        {
            S s(test_hash<int>(), std::equal_to<int>(), A{ &st });
            std::vector<kv> inserted;
            bool failed = false;

            for (int i = 0; i < 500; ++i)
            {
                auto r = s.try_emplace(i, vof(i));
                if (!r)
                {
                    failed = true;
                    break;
                }
                inserted.push_back(kv(i, vof(i)));
                if (i % 16 == 0)
                {
                    VX_CHECK(eq_model(s, inserted));
                }
            }

            if (budget == 0)
            {
                VX_CHECK(failed);
            }
            VX_CHECK(eq_model(s, inserted));
            for (const kv& e : inserted)
            {
                VX_CHECK(s.contains(e.first));
                VX_CHECK(*s.get(e.first) == e.second);
            }

            // usable again once allocation works
            st.countdown = -1;
            VX_CHECK(s.try_emplace(100000, 1));
            VX_CHECK(s.contains(100000));
        }
        VX_CHECK(st.live == 0);
    }
}

VX_TEST_CASE(failure_map_insert_apis)
{
    // every fallible map insert API reports failure and changes nothing
    using A = failing_allocator<std::pair<const int, int>>;
    using S = failing_map<int>;

    fail_state st;
    {
        S s(test_hash<int>(), std::equal_to<int>(), A{ &st });

        // empty table: the very first insert needs an allocation
        st.countdown = 0;
        VX_CHECK(!s.try_emplace(1, 10));
        VX_CHECK(!s.insert_or_assign(1, 10));
        VX_CHECK(!s.get_or_insert(1));
        VX_CHECK(!s.emplace(1, 10));
        VX_CHECK(s.empty());
        VX_CHECK(st.failures >= 4);

        st.countdown = -1;
        fill(s, 0, 8);
        VX_CHECK(s.reserve(8));
        const size_t cap = s.capacity();

        // hits do not allocate, so they succeed even with a dead allocator
        st.countdown = 0;
        VX_CHECK(!s.try_emplace(3, 999).value().inserted);
        VX_CHECK(!s.insert_or_assign(3, 33).value().inserted);
        VX_CHECK(*s.get(3) == 33);
        auto hit = s.get_or_insert(4);
        VX_CHECK(hit);
        VX_CHECK(*hit.value() == vof(4));
        VX_CHECK(s.capacity() == cap);

        // misses that would need to grow fail cleanly
        for (int i = 8; i < 1000; ++i)
        {
            if (!s.try_emplace(i, vof(i)))
            {
                break;
            }
        }
        const size_t size_before = s.size();
        VX_CHECK(!s.try_emplace(100000, 1));
        VX_CHECK(!s.insert_or_assign(100000, 1));
        VX_CHECK(!s.get_or_insert(100000));
        VX_CHECK(s.size() == size_before);
        VX_CHECK(!s.contains(100000));
        VX_CHECK(s.get(100000) == nullptr);

        // recovers once allocation works again
        st.countdown = -1;
        VX_CHECK(s.try_emplace(100000, 7).value().inserted);
        VX_CHECK(*s.get(100000) == 7);
        VX_CHECK(*s.get(5) == vof(5));
    }
    VX_CHECK(st.live == 0);
}

VX_TEST_CASE(failure_reserve_and_shrink)
{
    using A = failing_allocator<std::pair<const int, int>>;
    using S = failing_map<int>;

    fail_state st;
    {
        S s(test_hash<int>(), std::equal_to<int>(), A{ &st });
        fill(s, 0, 50);

        st.countdown = 0;
        VX_CHECK(!s.reserve(100000));
        VX_CHECK(eq_model(s, range(0, 50)));
        VX_CHECK(st.failures > 0);

        st.countdown = -1;
        for (int i = 3; i < 50; ++i)
        {
            VX_CHECK(s.erase(i) == 1);
        }

        st.countdown = 0;
        VX_CHECK(!s.shrink_to_fit()); // needs a new, smaller buffer
        VX_CHECK(eq(s, { kv(0, vof(0)), kv(1, vof(1)), kv(2, vof(2)) }));

        st.countdown = -1;
        VX_CHECK(s.shrink_to_fit());
        VX_CHECK(eq(s, { kv(0, vof(0)), kv(1, vof(1)), kv(2, vof(2)) }));
    }
    VX_CHECK(st.live == 0);
}

VX_TEST_CASE(failure_create_and_assign)
{
    using A = failing_allocator<std::pair<const int, int>>;
    using S = failing_map<int>;

    fail_state st;
    {
        // create(n) with a failing allocator
        st.countdown = 0;
        auto r = S::create(size_t(100), test_hash<int>(), std::equal_to<int>(), A{ &st });
        VX_CHECK(!r);

        // create(range) with a failing allocator
        std::vector<S::value_type> src = entries<S>(0, 20);
        auto r2 = S::create(src.begin(), src.end(), size_t(0), test_hash<int>(), std::equal_to<int>(), A{ &st });
        VX_CHECK(!r2);

        // create(copy) fails without touching the original
        st.countdown = -1;
        S a(test_hash<int>(), std::equal_to<int>(), A{ &st });
        fill(a, 0, 40);
        st.countdown = 0;
        auto r3 = S::create(a);
        VX_CHECK(!r3);
        VX_CHECK(eq_model(a, range(0, 40)));

        // assign(range) fails without touching the target
        VX_CHECK(!a.assign(src.begin(), src.end()));
        VX_CHECK(eq_model(a, range(0, 40)));

        // assign(const&) fails without touching the target
        S b(test_hash<int>(), std::equal_to<int>(), A{ &st });
        st.countdown = -1;
        fill(b, 100, 200);
        st.countdown = 0;
        VX_CHECK(!a.assign(b));
        VX_CHECK(eq_model(a, range(0, 40)));
        VX_CHECK(eq_model(b, range(100, 200)));
    }
    VX_CHECK(st.live == 0);
}

VX_TEST_CASE(failure_leaves_no_leaked_elements)
{
    // same sweep with lifetime-counting key AND mapped value: a failed insert
    // must not leak or double destroy the key / value that was being inserted
    using A = failing_allocator<std::pair<const counted_key, counted_key>>;
    using S = vx::unordered_map<counted_key, counted_key, test_hash<counted_key>, std::equal_to<counted_key>, A>;

    auto& ck = counted_key::stats();

    for (long budget = 0; budget < 8; ++budget)
    {
        counted_key::reset();
        fail_state st;
        st.countdown = budget;
        {
            S s(test_hash<counted_key>(), std::equal_to<counted_key>(), A{ &st });
            for (int i = 0; i < 300; ++i)
            {
                if (!s.try_emplace(counted_key(i), i))
                {
                    break;
                }
            }
            VX_CHECK(ck.live == 2 * s.size());
            VX_CHECK(ck.bad == 0);

            // the other fallible paths, on the exhausted allocator
            const size_t before = s.size();
            (void)s.insert_or_assign(counted_key(100000), counted_key(1));
            (void)s.get_or_insert(counted_key(100001));
            VX_CHECK(ck.live == 2 * s.size());
            VX_CHECK(s.size() >= before);
            VX_CHECK(ck.bad == 0);
        }
        VX_CHECK(ck.live == 0);
        VX_CHECK(ck.constructed == ck.destroyed);
        VX_CHECK(ck.bad == 0);
        VX_CHECK(st.live == 0);
    }
}

//=========================================================================
// randomized comparison against std::map
//=========================================================================

template <typename S>
void test_stress()
{
    using K = typename S::key_type;
    using V = typename S::mapped_type;

    for (unsigned seed = 1; seed <= 8; ++seed)
    {
        S s;
        std::map<int, int> m;
        unsigned state = seed * 7919u;

        auto rnd = [&](unsigned n)
        {
            state = state * 1664525u + 1013904223u;
            return (state >> 8) % n;
        };

        for (int iter = 0; iter < 2000; ++iter)
        {
            const int x = static_cast<int>(rnd(300));
            switch (rnd(9))
            {
                case 0:
                case 1:
                {
                    auto r = s.try_emplace(K(x), iter);
                    VX_CHECK(r);
                    VX_CHECK(r.value().inserted == m.emplace(x, iter).second);
                    break;
                }
                case 2:
                {
                    auto r = s.insert_or_assign(K(x), iter);
                    VX_CHECK(r);
                    VX_CHECK(r.value().inserted == (m.count(x) == 0));
                    m[x] = iter;
                    break;
                }
                case 3:
                case 4:
                {
                    VX_CHECK((s.erase(K(x)) == 1) == (m.erase(x) == 1));
                    break;
                }
                case 5:
                {
                    VX_CHECK(s.contains(K(x)) == (m.count(x) == 1));
                    auto* p = s.get(K(x));
                    VX_CHECK((p != nullptr) == (m.count(x) == 1));
                    if (p)
                    {
                        VX_CHECK(val(*p) == m[x]);
                    }
                    auto a = s.at(K(x));
                    VX_CHECK(static_cast<bool>(a) == (m.count(x) == 1));
                    break;
                }
                case 6:
                {
                    s[K(x)] = V(iter);
                    m[x] = iter;
                    break;
                }
                case 7:
                {
                    auto r = s.get_or_insert(K(x));
                    VX_CHECK(r);
                    if (m.count(x) == 0)
                    {
                        m[x] = 0;
                    }
                    VX_CHECK(val(*r.value()) == m[x]);
                    *r.value() = V(iter);
                    m[x] = iter;
                    break;
                }
                case 8:
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

            VX_CHECK(eq_model(s, std::vector<kv>(m.begin(), m.end())));
        }
    }
}
TEST_MAPS(stress)

VX_TEST_CASE(stress_degenerate_hash)
{
    test_stress<degenerate_map<int>>();
}

VX_TEST_CASE(stress_avalanching_hash)
{
    test_stress<avalanche_map<int>>();
}

//=========================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
