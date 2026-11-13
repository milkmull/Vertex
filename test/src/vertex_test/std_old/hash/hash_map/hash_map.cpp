#include "vertex/std/hash_map.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <type_traits>
#include <utility>
#include <vector>

#include "vertex_test/test.hpp"

// Every generic test runs against BOTH backends:
//   flat_family   vx::flat_map   (open addressing, Swiss-table style)
//   node_family  vx::node_map  (nodeed, one allocation per node)
// A "family" is a tiny struct exposing `map<K, V, Hash, Eq, Alloc>`, so a test
// templated on the map type (or on family + key) is written once.
//
// Key / mapped types (all constructible from int, val() reads them back):
//   int           trivial
//   copyable_int  non-trivial (copy + move, moved-from value is -1)
//   movable_int   move-only (copy deleted: an accidental copy is a compile error)
//
// Assumptions about code outside this file:
//   * vertex/std/hash_map.hpp is the header that defines flat_map / node_map
//   * value_type is map_pair<K, V>: public members `key` and `value`, and
//     constructible from (K&&, V&&)
//   * copyable_int / movable_int are constructible from int; tests that need a
//     default-constructible mapped type are guarded with is_default_constructible
//   * tracking_allocator<T> has the converting constructor
//       template <typename U> tracking_allocator(const tracking_allocator<U>&)
//
// The mapped value for key k is always mv(k) unless a test says otherwise, so
// "the right value ended up with the right key" is checked everywhere.
//
// Registration macros (each generates one test case per family / type combo):
//   TEST_MAPS(name)             test_<name><M>()      int/int, copyable, movable, tracked x2
//   TEST_MAPS_COPYABLE(name)    same, copyable combos only
//   TEST_KEYS(name)             test_<name><F, K>()   map<K, K>, every key type
//   TEST_FAMILIES(name)         test_<name><F>()
//   TEST_PLAIN_KEYS(name)       test_<name><K>()      both backends inside
//   TEST_CHAIN_KEYS(name)       test_<name><K>()      node_map only (stability tests)
//
// Layout:
//   1. helpers          families, types, hashers, allocators, map aliases, macros
//   2. construction
//   3. insert / emplace / lookup
//   4. map API          get, at, operator[], get_or_insert, try_emplace, insert_or_assign
//   5. erase
//   6. merge
//   7. capacity
//   8. copy / move / swap / assign / compare
//   9. iteration
//  10. hashing
//  11. element lifetime
//  12. allocators
//  13. error reporting
//  14. randomized stress
//  15. node_map only: reference and iterator stability

//-------------------------------------------------------------------------
// families
//-------------------------------------------------------------------------

struct flat_family
{
    template <typename K,
        typename V,
        typename H = vx::default_hash<K>,
        typename E = std::equal_to<K>,
        typename A = vx::mem::default_allocator<vx::pair<const K, V>>>
    using map = vx::flat_map<K, V, H, E, A>;
};

struct node_family
{
    template <typename K,
        typename V,
        typename H = vx::default_hash<K>,
        typename E = std::equal_to<K>,
        typename A = vx::mem::default_allocator<vx::pair<const K, V>>>
    using map = vx::node_map<K, V, H, E, A>;
};

//=========================================================================
// 0. compile-time policy checks
//
// Concrete numbers for the shipped policies. If this file compiles, they
// hold; the message names the broken line. Generic invariants (valid sizing,
// probe/sizing pow2 agreement) are asserted inside flat_table / node_table
// themselves, so they also fire for user-supplied policies.
//=========================================================================

namespace {

namespace flat = vx::_flat_hash_priv;
namespace node = vx::_node_hash_priv;

using flat_sizing = flat::default_sizing;
using node_sizing = node::default_sizing;
using flat_probe = flat::default_probe;

// ---- flat sizing: 7/8 load, must always leave an EMPTY byte ----

VX_STATIC_ASSERT_MSG(flat::valid_flat_sizing<flat_sizing>(), "flat default sizing is valid");

VX_STATIC_ASSERT_MSG(vx::_hash_priv::valid_sizing<flat_sizing>(), "flat default sizing is valid");
VX_STATIC_ASSERT_MSG(vx::_hash_priv::valid_sizing<node_sizing>(), "node default sizing is valid");
VX_STATIC_ASSERT_MSG(!vx::_hash_priv::valid_sizing<node_sizing>() || !flat::valid_flat_sizing<node_sizing>(),
    "node sizing (load 1.0) must be rejected as a flat sizing");

VX_STATIC_ASSERT_MSG(flat_sizing::max_load(0) == 0, "flat: cap 0 holds nothing");
VX_STATIC_ASSERT_MSG(flat_sizing::max_load(1) == 0, "flat: cap 1 holds nothing");
VX_STATIC_ASSERT_MSG(flat_sizing::max_load(3) == 2, "flat: cap 3 -> 2");
VX_STATIC_ASSERT_MSG(flat_sizing::max_load(7) == 6, "flat: cap 7 -> 6");
VX_STATIC_ASSERT_MSG(flat_sizing::max_load(15) == 14, "flat: cap 15 -> 14");
VX_STATIC_ASSERT_MSG(flat_sizing::max_load(31) == 28, "flat: cap 31 -> 28");
VX_STATIC_ASSERT_MSG(flat_sizing::max_load(63) == 56, "flat: cap 63 -> 56");

VX_STATIC_ASSERT_MSG(flat_sizing::next_capacity(0, 1) == 3, "flat: first allocation");
VX_STATIC_ASSERT_MSG(flat_sizing::next_capacity(0, 2) == 3, "flat: 2 fit in cap 3");
VX_STATIC_ASSERT_MSG(flat_sizing::next_capacity(0, 3) == 7, "flat: 3 need cap 7");
VX_STATIC_ASSERT_MSG(flat_sizing::next_capacity(0, 6) == 7, "flat: 6 fit in cap 7");
VX_STATIC_ASSERT_MSG(flat_sizing::next_capacity(0, 7) == 15, "flat: 7 need cap 15");
VX_STATIC_ASSERT_MSG(flat_sizing::next_capacity(0, 28) == 31, "flat: 28 fit in cap 31");
VX_STATIC_ASSERT_MSG(flat_sizing::next_capacity(0, 29) == 63, "flat: 29 need cap 63");

VX_STATIC_ASSERT_MSG(flat_sizing::next_capacity(3, 3) == 7, "flat: 3 -> 7");
VX_STATIC_ASSERT_MSG(flat_sizing::next_capacity(7, 7) == 15, "flat: 7 -> 15");
VX_STATIC_ASSERT_MSG(flat_sizing::next_capacity(15, 15) == 31, "flat: 15 -> 31");
VX_STATIC_ASSERT_MSG(flat_sizing::next_capacity(31, 1) == 63, "flat: always grows");
VX_STATIC_ASSERT_MSG(flat_sizing::next_capacity(7, 100) == 127, "flat: can skip levels");
VX_STATIC_ASSERT_MSG(flat_sizing::next_capacity(3, 0) == 7, "flat: required 0 is treated as 1");

VX_STATIC_ASSERT_MSG(flat_sizing::next_capacity(flat_sizing::max_capacity, 1) == 0, "flat: no growth past max_capacity");
VX_STATIC_ASSERT_MSG(flat_sizing::next_capacity(0, std::numeric_limits<vx::_hash_priv::hash_size_t>::max()) == 0, "flat: unrepresentable request");

// ---- node sizing: load 1.0, one element per bucket ----

VX_STATIC_ASSERT_MSG(node_sizing::max_load(0) == 0, "node: cap 0 holds nothing");
VX_STATIC_ASSERT_MSG(node_sizing::max_load(3) == 4, "node: cap 3 -> 4 buckets -> 4 elements");
VX_STATIC_ASSERT_MSG(node_sizing::max_load(7) == 8, "node: cap 7 -> 8");

VX_STATIC_ASSERT_MSG(node_sizing::next_capacity(0, 4) == 3, "node: 4 fit in cap 3");
VX_STATIC_ASSERT_MSG(node_sizing::next_capacity(0, 5) == 7, "node: 5 need cap 7");
VX_STATIC_ASSERT_MSG(node_sizing::next_capacity(3, 1) == 7, "node: always grows");

// ---- probe ----

VX_STATIC_ASSERT_MSG(flat_probe::requires_pow2, "triangular probe requires pow2");
VX_STATIC_ASSERT_MSG(flat_probe::start(0x1234, 15) == 4, "probe: start masks the hash");
VX_STATIC_ASSERT_MSG(flat_probe::wrap_lane(17, 15) == 1, "probe: wrap_lane masks");
VX_STATIC_ASSERT_MSG(flat_probe::advance(14, 16, 15) == 14, "probe: advance wraps around the ring");

// ---- cleanup policy: must trigger before the table is full ----

VX_STATIC_ASSERT_MSG(flat::default_cleanup::cleanup_limit(28) < 28, "cleanup limit < max_load");
VX_STATIC_ASSERT_MSG(flat::default_cleanup::cleanup_limit(32) == 25, "cleanup limit is 25/32 of max_load");

// ---- backend type properties ----

using flat_int_set = flat_family::map<int, int>;
using node_int_set = node_family::map<int, int>;

VX_STATIC_ASSERT_MSG(std::is_nothrow_move_constructible_v<flat_int_set>, "flat_set move ctor is noexcept");
VX_STATIC_ASSERT_MSG(std::is_nothrow_move_constructible_v<node_int_set>, "node_set move ctor is noexcept");
VX_STATIC_ASSERT_MSG(std::is_nothrow_move_assignable_v<flat_int_set>, "flat_set move assign is noexcept");
VX_STATIC_ASSERT_MSG(std::is_nothrow_move_assignable_v<node_int_set>, "node_set move assign is noexcept");
VX_STATIC_ASSERT_MSG(std::is_nothrow_swappable_v<flat_int_set>, "flat_set swap is noexcept");
VX_STATIC_ASSERT_MSG(std::is_nothrow_swappable_v<node_int_set>, "node_set swap is noexcept");

using vx::test::movable_int;

VX_STATIC_ASSERT_MSG((!std::is_copy_constructible_v<flat_family::map<movable_int, movable_int>> ||
                         std::is_copy_constructible_v<flat_family::map<movable_int, movable_int>>),
    "placeholder: replace if you SFINAE copy on key copyability");

} // namespace

// Walks every capacity a sizing policy produces and checks the properties the
// tables rely on. Runs against any policy, so a new load factor only needs one
// more TEST registration below.
template <typename Sizing>
void check_sizing_progression(bool needs_free_slot)
{
    using size_type = vx::_hash_priv::hash_size_t;

    size_type cap = Sizing::next_capacity(0, 1);
    VX_CHECK(cap != 0);

    size_type prev_load = 0;
    for (int step = 0; step < 24 && cap != 0; ++step)
    {
        const size_type load = Sizing::max_load(cap);

        VX_CHECK(((cap + 1) & cap) == 0); // 2^k - 1
        VX_CHECK(load >= prev_load);      // monotonic
        if (needs_free_slot)
        {
            VX_CHECK(load < cap); // an EMPTY byte always exists
        }

        // tightest fit really is tight
        VX_CHECK(Sizing::next_capacity(0, load) <= cap);
        VX_CHECK(Sizing::next_capacity(0, load + 1) > cap);

        // growth always makes progress and always makes room
        const size_type next = Sizing::next_capacity(cap, 1);
        if (next != 0)
        {
            VX_CHECK(next > cap);
            VX_CHECK(Sizing::max_load(next) > load);
        }

        prev_load = load;
        cap = next;
    }
}

VX_TEST_CASE(sizing_progression_flat)
{
    check_sizing_progression<vx::_flat_hash_priv::default_sizing>(true);
}

VX_TEST_CASE(sizing_progression_node)
{
    check_sizing_progression<vx::_node_hash_priv::default_sizing>(false);
}

//=========================================================================
// 1. helpers
//=========================================================================

namespace {

using vx::test::allocator_stats;
using vx::test::copyable_int;
using vx::test::movable_int;
using vx::test::tracking_allocator;

template <typename K>
constexpr bool copyable = std::is_copy_constructible_v<K>;

template <typename T>
int val(const T& t)
{
    return static_cast<int>(t);
}

// canonical mapped value for key k
constexpr int mv(int k)
{
    return k * 10 + 1;
}

//-------------------------------------------------------------------------
// hashers
//-------------------------------------------------------------------------

// Deliberately NOT avalanching, so the table's own finalizer is exercised.
template <typename K>
struct test_hash
{
    size_t operator()(const K& k) const noexcept
    {
        return vx::default_hash<int>()(val(k));
    }
};

// Everything collides.
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
// counted_obj: tracks construction / destruction / copies / moves and detects
// use-after-destroy / double destroy via a magic value. One counter per Tag,
// so keys and mapped values are tracked separately.
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

counted_stats g_counted_stats[2];

constexpr unsigned counted_magic = 0xC0FFEEu;

template <int Tag>
struct counted_obj
{
    static counted_stats& stats() noexcept
    {
        return g_counted_stats[Tag];
    }

    counted_obj() noexcept
        : counted_obj(0)
    {}

    explicit counted_obj(int v) noexcept
        : m_v(v)
    {
        ++stats().live;
        ++stats().constructed;
    }

    counted_obj(const counted_obj& o)
        : m_v(o.m_v)
    {
        o.check();
        ++stats().live;
        ++stats().constructed;
        ++stats().copies;
    }

    counted_obj(counted_obj&& o) noexcept
        : m_v(o.m_v)
    {
        o.check();
        o.m_v = -1;
        ++stats().live;
        ++stats().constructed;
        ++stats().moves;
    }

    counted_obj& operator=(const counted_obj& o)
    {
        check();
        o.check();
        m_v = o.m_v;
        ++stats().copies;
        return *this;
    }

    counted_obj& operator=(counted_obj&& o) noexcept
    {
        check();
        o.check();
        m_v = o.m_v;
        o.m_v = -1;
        ++stats().moves;
        return *this;
    }

    ~counted_obj()
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

    friend bool operator==(const counted_obj& a, const counted_obj& b) noexcept
    {
        return a.m_v == b.m_v;
    }

    friend bool operator!=(const counted_obj& a, const counted_obj& b) noexcept
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

using counted_key = counted_obj<0>;
using counted_val = counted_obj<1>;

void reset_counted() noexcept
{
    g_counted_stats[0] = counted_stats{};
    g_counted_stats[1] = counted_stats{};
}

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
// failing_allocator: malloc based, can be told to fail.
// countdown == -1: never fail. countdown == k: k more allocations succeed,
// then every allocation returns nullptr until countdown is reset.
//-------------------------------------------------------------------------

struct fail_state
{
    long countdown = -1;
    long live = 0;
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

//-------------------------------------------------------------------------
// map aliases (F = family, K = key, V = mapped)
//-------------------------------------------------------------------------

template <typename F, typename K, typename V>
using plain_map = typename F::template map<K, V, test_hash<K>>;

template <typename F, typename K, typename V>
using tracked_map = typename F::template map<K, V, test_hash<K>, std::equal_to<K>, tracking_allocator<K>>;

template <typename F, typename K, typename V>
using failing_map = typename F::template map<K, V, test_hash<K>, std::equal_to<K>, failing_allocator<K>>;

template <typename F, typename K, typename V>
using degenerate_map = typename F::template map<K, V, const_hash<K>>;

template <typename F, typename K, typename V>
using avalanche_map = typename F::template map<K, V, avalanche_hash<K>>;

template <typename F>
using counted_map = typename F::template map<counted_key, counted_val, test_hash<counted_key>>;

template <typename M>
using key_of = typename M::key_type;

template <typename M>
using mapped_of = typename M::mapped_type;

//-------------------------------------------------------------------------
// element / content helpers
//-------------------------------------------------------------------------

// builds an element of map type M
template <typename M>
typename M::value_type make(int k, int v)
{
    return typename M::value_type(key_of<M>(k), mapped_of<M>(v));
}

// inserts key k with its canonical value
template <typename M>
auto ins(M& m, int k)
{
    return m.insert(make<M>(k, mv(k)));
}

// inserts key k with an explicit value, checking that it was inserted
template <typename M>
void put(M& m, int k, int v)
{
    auto r = m.insert(make<M>(k, v));
    VX_CHECK(r);
    VX_CHECK(r.value().inserted);
}

using model = std::vector<std::pair<int, int>>;

// sorted (key, value) pairs, by walking the iterators
template <typename M>
model contents(const M& m)
{
    model out;
    for (auto it = m.begin(); it != m.end(); ++it)
    {
        out.emplace_back(val(it->key), val(it->value));
    }
    std::sort(out.begin(), out.end());
    return out;
}

template <typename M>
bool eq_model(const M& m, const model& sorted)
{
    return m.size() == sorted.size() && contents(m) == sorted;
}

template <typename M>
bool eq(const M& m, std::initializer_list<std::pair<int, int>> l)
{
    model sorted(l);
    std::sort(sorted.begin(), sorted.end());
    return eq_model(m, sorted);
}

// [lo, hi) with canonical values
model range(int lo, int hi)
{
    model v;
    for (int i = lo; i < hi; ++i)
    {
        v.emplace_back(i, mv(i));
    }
    return v;
}

// inserts [lo, hi) with canonical values, checking that each insert succeeds
template <typename M>
void fill(M& m, int lo, int hi)
{
    for (int i = lo; i < hi; ++i)
    {
        VX_CHECK(ins(m, i));
    }
}

// single-pass input iterator yielding elements of M for the given keys
template <typename M>
class gen_iter
{
public:

    using iterator_category = std::input_iterator_tag;
    using value_type = typename M::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type*;
    using reference = value_type;

    gen_iter() = default;

    gen_iter(const std::vector<int>* keys, size_t i) noexcept
        : m_keys(keys)
        , m_i(i)
    {}

    value_type operator*() const
    {
        const int k = (*m_keys)[m_i];
        return make<M>(k, mv(k));
    }

    gen_iter& operator++() noexcept
    {
        ++m_i;
        return *this;
    }

    gen_iter operator++(int) noexcept
    {
        gen_iter t = *this;
        ++m_i;
        return t;
    }

    friend bool operator==(const gen_iter& a, const gen_iter& b) noexcept
    {
        return a.m_i == b.m_i;
    }

    friend bool operator!=(const gen_iter& a, const gen_iter& b) noexcept
    {
        return a.m_i != b.m_i;
    }

private:

    const std::vector<int>* m_keys = nullptr;
    size_t m_i = 0;
};

} // namespace

//-------------------------------------------------------------------------
// test registration macros
//-------------------------------------------------------------------------

#define TEST_MAPS_FOR(name, tag, fam) \
    VX_TEST_CASE(name##_##tag##_int) \
    { \
        test_##name<plain_map<fam, int, int>>(); \
    } \
    VX_TEST_CASE(name##_##tag##_copyable) \
    { \
        test_##name<plain_map<fam, copyable_int, copyable_int>>(); \
    } \
    VX_TEST_CASE(name##_##tag##_movable) \
    { \
        test_##name<plain_map<fam, movable_int, movable_int>>(); \
    } \
    VX_TEST_CASE(name##_##tag##_tracked_copyable) \
    { \
        test_##name<tracked_map<fam, copyable_int, copyable_int>>(); \
    } \
    VX_TEST_CASE(name##_##tag##_tracked_movable) \
    { \
        test_##name<tracked_map<fam, movable_int, movable_int>>(); \
    }

#define TEST_MAPS_COPYABLE_FOR(name, tag, fam) \
    VX_TEST_CASE(name##_##tag##_int) \
    { \
        test_##name<plain_map<fam, int, int>>(); \
    } \
    VX_TEST_CASE(name##_##tag##_copyable) \
    { \
        test_##name<plain_map<fam, copyable_int, copyable_int>>(); \
    } \
    VX_TEST_CASE(name##_##tag##_tracked_copyable) \
    { \
        test_##name<tracked_map<fam, copyable_int, copyable_int>>(); \
    }

#define TEST_KEYS_FOR(name, tag, fam) \
    VX_TEST_CASE(name##_##tag##_int) \
    { \
        test_##name<fam, int>(); \
    } \
    VX_TEST_CASE(name##_##tag##_copyable) \
    { \
        test_##name<fam, copyable_int>(); \
    } \
    VX_TEST_CASE(name##_##tag##_movable) \
    { \
        test_##name<fam, movable_int>(); \
    }

#define TEST_MAPS(name) \
    TEST_MAPS_FOR(name, flat, flat_family) \
    TEST_MAPS_FOR(name, node, node_family)

#define TEST_MAPS_COPYABLE(name) \
    TEST_MAPS_COPYABLE_FOR(name, flat, flat_family) \
    TEST_MAPS_COPYABLE_FOR(name, node, node_family)

#define TEST_KEYS(name) \
    TEST_KEYS_FOR(name, flat, flat_family) \
    TEST_KEYS_FOR(name, node, node_family)

// test_<name><F>() for both families
#define TEST_FAMILIES(name) \
    VX_TEST_CASE(name##_flat) \
    { \
        test_##name<flat_family>(); \
    } \
    VX_TEST_CASE(name##_node) \
    { \
        test_##name<node_family>(); \
    }

// test_<name><K>() for every key type, no family (tests that use both backends)
#define TEST_PLAIN_KEYS(name) \
    VX_TEST_CASE(name##_int) \
    { \
        test_##name<int>(); \
    } \
    VX_TEST_CASE(name##_copyable) \
    { \
        test_##name<copyable_int>(); \
    } \
    VX_TEST_CASE(name##_movable) \
    { \
        test_##name<movable_int>(); \
    }

// test_<name><K>() for node_map only, every key type
#define TEST_CHAIN_KEYS(name) \
    VX_TEST_CASE(name##_node_int) \
    { \
        test_##name<int>(); \
    } \
    VX_TEST_CASE(name##_node_copyable) \
    { \
        test_##name<copyable_int>(); \
    } \
    VX_TEST_CASE(name##_node_movable) \
    { \
        test_##name<movable_int>(); \
    }

//=========================================================================
// 2. construction
//=========================================================================

template <typename M>
void test_construct()
{
    using K = key_of<M>;

    VX_SECTION("default is empty and usable without allocating")
    {
        M m;
        VX_CHECK(m.empty());
        VX_CHECK(m.size() == 0);
        VX_CHECK(m.capacity() == 0);
        VX_CHECK(m.begin() == m.end());
        VX_CHECK(m.find(K(1)) == m.end());
        VX_CHECK(!m.contains(K(1)));
        VX_CHECK(m.count(K(1)) == 0);
        VX_CHECK(m.get(K(1)) == nullptr);
        VX_CHECK(m.erase(K(1)) == 0);
        m.clear();
        VX_CHECK(m.empty());
    }

    VX_SECTION("reserving constructor")
    {
        M m(size_t(50));
        VX_CHECK(m.empty());
        VX_CHECK(m.capacity() >= 50);
    }

    VX_SECTION("create")
    {
        auto a = M::create();
        VX_CHECK(a);
        VX_CHECK(a.value().empty());

        auto b = M::create(size_t(16));
        VX_CHECK(b);
        VX_CHECK(b.value().empty());
        VX_CHECK(b.value().capacity() >= 16);

        M src;
        fill(src, 0, 10);

        // create from an rvalue takes the buffer
        auto mv_ = M::create(std::move(src));
        VX_CHECK(mv_);
        VX_CHECK(eq_model(mv_.value(), range(0, 10)));

        if constexpr (copyable<typename M::value_type>)
        {
            M src2;
            fill(src2, 0, 10);
            auto c = M::create(src2);
            VX_CHECK(c);
            VX_CHECK(c.value() == src2);
        }
    }
}
TEST_MAPS(construct)

template <typename M>
void test_construct_from_ranges()
{
    using V = typename M::value_type;

    std::vector<V> src;
    for (int i = 0; i < 20; ++i)
    {
        src.push_back(make<M>(i % 10, mv(i % 10))); // duplicates
    }

    VX_SECTION("range constructor")
    {
        M m(src.begin(), src.end());
        VX_CHECK(eq_model(m, range(0, 10)));
    }

    VX_SECTION("range constructor honours the n argument")
    {
        M m(src.begin(), src.end(), size_t(64));
        VX_CHECK(eq_model(m, range(0, 10)));
        VX_CHECK(m.capacity() >= 64);
    }

    VX_SECTION("range from a non-random-access iterator")
    {
        std::list<V> l(src.begin(), src.end());
        M m(l.begin(), l.end());
        VX_CHECK(eq_model(m, range(0, 10)));
    }

    VX_SECTION("the first value wins for a duplicate key")
    {
        std::vector<V> dup;
        dup.push_back(make<M>(1, 10));
        dup.push_back(make<M>(2, 20));
        dup.push_back(make<M>(1, 99));

        M m(dup.begin(), dup.end());
        VX_CHECK(eq(m, {
                           { 1, 10 },
                           { 2, 20 }
        }));
    }

    VX_SECTION("initializer_list constructor")
    {
        M m{ make<M>(1, 10), make<M>(2, 20), make<M>(2, 99), make<M>(3, 30) };
        VX_CHECK(eq(m, {
                           { 1, 10 },
                           { 2, 20 },
                           { 3, 30 }
        }));

        M t(std::initializer_list<V>{ make<M>(5, 50), make<M>(6, 60) }, size_t(32));
        VX_CHECK(eq(t, {
                           { 5, 50 },
                           { 6, 60 }
        }));
        VX_CHECK(t.capacity() >= 32);
    }

    VX_SECTION("create from range / initializer_list")
    {
        auto a = M::create(src.begin(), src.end());
        VX_CHECK(a);
        VX_CHECK(eq_model(a.value(), range(0, 10)));

        auto b = M::create(src.begin(), src.end(), size_t(100));
        VX_CHECK(b);
        VX_CHECK(b.value().capacity() >= 100);
        VX_CHECK(eq_model(b.value(), range(0, 10)));

        auto c = M::create(std::initializer_list<V>{ make<M>(7, 70), make<M>(8, 80), make<M>(7, 0) });
        VX_CHECK(c);
        VX_CHECK(eq(c.value(), {
                                   { 7, 70 },
                                   { 8, 80 }
        }));
    }

    VX_SECTION("create from an empty range")
    {
        std::vector<V> none;
        auto a = M::create(none.begin(), none.end());
        VX_CHECK(a);
        VX_CHECK(a.value().empty());
    }
}
TEST_MAPS_COPYABLE(construct_from_ranges)

template <typename F>
void test_construct_from_input_iterators()
{
    using M = plain_map<F, int, int>;
    using It = gen_iter<M>;

    // true single-pass input iterators take the non-reserving insert path
    const std::vector<int> keys = { 1, 2, 3, 3, 4, 1, 5 };
    const model expected = {
        { 1, mv(1) },
        { 2, mv(2) },
        { 3, mv(3) },
        { 4, mv(4) },
        { 5, mv(5) }
    };

    It first(&keys, 0);
    It last(&keys, keys.size());
    M m(first, last);
    VX_CHECK(eq_model(m, expected));

    M t;
    VX_CHECK(t.insert(It(&keys, 0), It(&keys, keys.size())));
    VX_CHECK(eq_model(t, expected));
}
TEST_FAMILIES(construct_from_input_iterators)

//=========================================================================
// 3. insert / emplace / lookup
//=========================================================================

template <typename M>
void test_insert_find()
{
    using K = key_of<M>;
    using V = typename M::value_type;

    VX_SECTION("insert reports whether it inserted")
    {
        M m;
        auto r = ins(m, 1);
        VX_CHECK(r);
        VX_CHECK(r.value().inserted);
        VX_CHECK(val(r.value().it->key) == 1);
        VX_CHECK(val(r.value().it->value) == mv(1));
        VX_CHECK(m.size() == 1);

        // a duplicate key does not overwrite the value
        auto again = m.insert(make<M>(1, 999));
        VX_CHECK(again);
        VX_CHECK(!again.value().inserted);
        VX_CHECK(again.value().it == r.value().it);
        VX_CHECK(val(again.value().it->value) == mv(1));
        VX_CHECK(m.size() == 1);

        if constexpr (copyable<V>)
        {
            const V c = make<M>(2, 20);
            VX_CHECK(m.insert(c).value().inserted);
            VX_CHECK(!m.insert(c).value().inserted);
            VX_CHECK(eq(m, {
                               { 1, mv(1) },
                               { 2, 20    }
            }));
        }
    }

    VX_SECTION("find / contains / count")
    {
        M m;
        fill(m, 0, 100);
        for (int i = 0; i < 100; ++i)
        {
            auto it = m.find(K(i));
            VX_CHECK(it != m.end());
            VX_CHECK(val(it->key) == i);
            VX_CHECK(val(it->value) == mv(i));
            VX_CHECK(m.contains(K(i)));
            VX_CHECK(m.count(K(i)) == 1);
        }
        for (int i = 100; i < 200; ++i)
        {
            VX_CHECK(m.find(K(i)) == m.end());
            VX_CHECK(!m.contains(K(i)));
            VX_CHECK(m.count(K(i)) == 0);
        }
        VX_CHECK(m.size() == 100);
    }

    VX_SECTION("const lookup")
    {
        M m;
        fill(m, 0, 10);
        const M& cm = m;
        VX_CHECK(cm.find(K(3)) != cm.end());
        VX_CHECK(val(cm.find(K(3))->value) == mv(3));
        VX_CHECK(cm.find(K(30)) == cm.end());
        VX_CHECK(cm.contains(K(9)));
        VX_CHECK(cm.count(K(9)) == 1);
    }

    if constexpr (copyable<V>)
    {
        VX_SECTION("insert range")
        {
            std::vector<V> src;
            for (int i = 0; i < 20; ++i)
            {
                src.push_back(make<M>(i % 10, mv(i % 10))); // duplicates are skipped
            }
            M m;
            VX_CHECK(m.insert(src.begin(), src.end()));
            VX_CHECK(eq_model(m, range(0, 10)));

            // inserting the same range again changes nothing
            VX_CHECK(m.insert(src.begin(), src.end()));
            VX_CHECK(eq_model(m, range(0, 10)));
        }

        VX_SECTION("insert range does not overwrite existing values")
        {
            M m;
            put(m, 1, 10);

            std::vector<V> src;
            src.push_back(make<M>(1, 99));
            src.push_back(make<M>(2, 20));
            VX_CHECK(m.insert(src.begin(), src.end()));
            VX_CHECK(eq(m, {
                               { 1, 10 },
                               { 2, 20 }
            }));
        }

        VX_SECTION("insert initializer_list")
        {
            M m;
            VX_CHECK(m.insert(std::initializer_list<V>{ make<M>(1, 10), make<M>(2, 20), make<M>(1, 99), make<M>(3, 30) }));
            VX_CHECK(eq(m, {
                               { 1, 10 },
                               { 2, 20 },
                               { 3, 30 }
            }));
        }

        VX_SECTION("insert empty range")
        {
            std::vector<V> none;
            M m;
            VX_CHECK(m.insert(none.begin(), none.end()));
            VX_CHECK(m.empty());
        }
    }
}
TEST_MAPS(insert_find)

template <typename M>
void test_emplace()
{
    using K = key_of<M>;
    using T = mapped_of<M>;

    VX_SECTION("emplace inserts, duplicates return the existing element")
    {
        M m;
        auto first = m.emplace(K(7), T(70));
        VX_CHECK(first);
        VX_CHECK(first.value().inserted);
        VX_CHECK(val(first.value().it->value) == 70);

        auto second = m.emplace(K(7), T(1));
        VX_CHECK(second);
        VX_CHECK(!second.value().inserted);
        VX_CHECK(first.value().it == second.value().it);
        VX_CHECK(val(second.value().it->value) == 70); // not overwritten
        VX_CHECK(m.size() == 1);
    }

    VX_SECTION("emplace_assume_capacity")
    {
        M m;
        VX_CHECK(m.reserve(10));
        VX_CHECK(m.emplace_assume_capacity(K(6), T(60)).inserted);
        VX_CHECK(!m.emplace_assume_capacity(K(6), T(1)).inserted);
        VX_CHECK(eq(m, {
                           { 6, 60 }
        }));
    }
}
TEST_MAPS(emplace)

// the _assume_capacity / _assume_unique variants of insert and emplace
template <typename M>
void test_insert_variants()
{
    using K = key_of<M>;
    using T = mapped_of<M>;

    VX_SECTION("insert_assume_capacity")
    {
        M m;
        VX_CHECK(m.reserve(100));
        for (int i = 0; i < 100; ++i)
        {
            VX_CHECK(m.insert_assume_capacity(make<M>(i, mv(i))).inserted);
        }
        VX_CHECK(!m.insert_assume_capacity(make<M>(5, 0)).inserted);
        VX_CHECK(eq_model(m, range(0, 100)));
    }

    VX_SECTION("insert_assume_unique")
    {
        M m;
        VX_CHECK(m.reserve(10));
        auto r = m.insert_assume_unique(make<M>(1, 10));
        VX_CHECK(r);
        VX_CHECK(val(r.value()->value) == 10);
        auto it = m.insert_assume_unique_and_capacity(make<M>(2, 20));
        VX_CHECK(val(it->value) == 20);
        VX_CHECK(eq(m, {
                           { 1, 10 },
                           { 2, 20 }
        }));
    }

    VX_SECTION("emplace_assume_unique")
    {
        M m;
        VX_CHECK(m.reserve(10));
        auto a = m.emplace_assume_unique(K(1), T(10));
        VX_CHECK(a);
        VX_CHECK(val(a.value()->value) == 10);
        auto b = m.emplace_assume_unique_and_capacity(K(2), T(20));
        VX_CHECK(val(b->value) == 20);
        VX_CHECK(eq(m, {
                           { 1, 10 },
                           { 2, 20 }
        }));
    }
}
TEST_MAPS(insert_variants)

//=========================================================================
// 4. map API
//=========================================================================

template <typename M>
void test_get_at()
{
    using K = key_of<M>;
    using T = mapped_of<M>;

    VX_SECTION("get returns a pointer to the value, or nullptr")
    {
        M m;
        fill(m, 0, 50);
        for (int i = 0; i < 50; ++i)
        {
            T* p = m.get(K(i));
            VX_CHECK(p != nullptr);
            VX_CHECK(val(*p) == mv(i));
        }
        VX_CHECK(m.get(K(50)) == nullptr);
        VX_CHECK(m.get(K(-1)) == nullptr);
    }

    VX_SECTION("get gives mutable access")
    {
        M m;
        fill(m, 0, 10);
        T* p = m.get(K(3));
        VX_CHECK(p != nullptr);
        *p = T(777);
        VX_CHECK(val(*m.get(K(3))) == 777);
        VX_CHECK(val(m.find(K(3))->value) == 777);
        VX_CHECK(m.size() == 10);
    }

    VX_SECTION("const get")
    {
        M m;
        fill(m, 0, 10);
        const M& cm = m;
        const T* p = cm.get(K(4));
        VX_CHECK(p != nullptr);
        VX_CHECK(val(*p) == mv(4));
        VX_CHECK(cm.get(K(40)) == nullptr);
    }

    VX_SECTION("at returns the value or out_of_range")
    {
        M m;
        fill(m, 0, 10);

        auto r = m.at(K(3));
        VX_CHECK(r);
        VX_CHECK(val(r.value()) == mv(3));

        // the reference is live: writing through it changes the map
        r.value() = T(555);
        VX_CHECK(val(*m.get(K(3))) == 555);

        VX_CHECK_EXPECTED_ERROR(m.at(K(999)), vx::err::out_of_range);
        VX_CHECK(m.size() == 10); // at never inserts
    }

    VX_SECTION("at on an empty map")
    {
        M m;
        VX_CHECK_EXPECTED_ERROR(m.at(K(1)), vx::err::out_of_range);
        VX_CHECK(m.empty());
    }

    VX_SECTION("const at")
    {
        M m;
        fill(m, 0, 10);
        const M& cm = m;

        auto r = cm.at(K(4));
        VX_CHECK(r);
        VX_CHECK(val(r.value()) == mv(4));
        VX_CHECK_EXPECTED_ERROR(cm.at(K(40)), vx::err::out_of_range);
    }
}
TEST_MAPS(get_at)

template <typename M>
void test_subscript()
{
    using K = key_of<M>;
    using T = mapped_of<M>;

    if constexpr (std::is_default_constructible_v<T>)
    {
        VX_SECTION("inserts a default-constructed value")
        {
            M m;
            T& r = m[K(1)];
            VX_CHECK(m.size() == 1);
            VX_CHECK(m.contains(K(1)));
            r = T(5);
            VX_CHECK(val(*m.get(K(1))) == 5);
        }

        if constexpr (std::is_same_v<T, int>)
        {
            VX_SECTION("a new int value is zero")
            {
                M m;
                VX_CHECK(m[K(77)] == 0);
            }
        }

        VX_SECTION("an existing key returns the same reference")
        {
            M m;
            T& a = m[K(1)];
            a = T(9);
            T& b = m[K(1)];
            VX_CHECK(&a == &b);
            VX_CHECK(val(b) == 9);
            VX_CHECK(m.size() == 1);
        }

        VX_SECTION("assigning through operator[] with growth")
        {
            M m;
            for (int i = 0; i < 1000; ++i)
            {
                m[K(i)] = T(mv(i));
            }
            VX_CHECK(eq_model(m, range(0, 1000)));

            // overwrite
            for (int i = 0; i < 1000; ++i)
            {
                m[K(i)] = T(i);
            }
            VX_CHECK(m.size() == 1000);
            for (int i = 0; i < 1000; ++i)
            {
                VX_CHECK(val(*m.get(K(i))) == i);
            }
        }

        if constexpr (copyable<K>)
        {
            VX_SECTION("lvalue key overload copies the key")
            {
                M m;
                const K k(3);
                m[k] = T(30);
                VX_CHECK(val(k) == 3);
                VX_CHECK(val(*m.get(K(3))) == 30);
            }
        }
    }
}
TEST_MAPS(subscript)

template <typename M>
void test_get_or_insert()
{
    using K = key_of<M>;
    using T = mapped_of<M>;

    if constexpr (std::is_default_constructible_v<T>)
    {
        VX_SECTION("inserts a default value and returns a pointer to it")
        {
            M m;
            auto r = m.get_or_insert(K(1));
            VX_CHECK(r);
            T* p = r.value();
            VX_CHECK(p != nullptr);
            VX_CHECK(m.size() == 1);

            *p = T(5);
            VX_CHECK(val(*m.get(K(1))) == 5);
        }

        VX_SECTION("an existing key is returned untouched")
        {
            M m;
            put(m, 1, 10);

            auto r = m.get_or_insert(K(1));
            VX_CHECK(r);
            VX_CHECK(r.value() == m.get(K(1)));
            VX_CHECK(val(*r.value()) == 10);
            VX_CHECK(m.size() == 1);
        }

        VX_SECTION("growth")
        {
            M m;
            for (int i = 0; i < 1000; ++i)
            {
                auto r = m.get_or_insert(K(i));
                VX_CHECK(r);
                *r.value() = T(mv(i));
            }
            VX_CHECK(eq_model(m, range(0, 1000)));
        }

        if constexpr (copyable<K>)
        {
            VX_SECTION("lvalue key overload")
            {
                M m;
                const K k(2);
                auto r = m.get_or_insert(k);
                VX_CHECK(r);
                VX_CHECK(val(k) == 2);
                VX_CHECK(m.contains(K(2)));
            }
        }
    }
}
TEST_MAPS(get_or_insert)

template <typename M>
void test_try_emplace()
{
    using K = key_of<M>;
    using T = mapped_of<M>;

    VX_SECTION("inserts when the key is absent")
    {
        M m;
        auto r = m.try_emplace(K(1), T(10));
        VX_CHECK(r);
        VX_CHECK(r.value().inserted);
        VX_CHECK(val(r.value().it->key) == 1);
        VX_CHECK(val(r.value().it->value) == 10);
        VX_CHECK(m.size() == 1);
    }

    VX_SECTION("an existing key is left alone and the arguments are not consumed")
    {
        M m;
        put(m, 1, 10);

        K k(1);
        T v(99);
        auto r = m.try_emplace(std::move(k), std::move(v));
        VX_CHECK(r);
        VX_CHECK(!r.value().inserted);
        VX_CHECK(val(r.value().it->value) == 10); // not overwritten
        VX_CHECK(val(k) == 1);                    // not moved from
        VX_CHECK(val(v) == 99);
        VX_CHECK(m.size() == 1);
    }

    if constexpr (!std::is_same_v<K, int> && !std::is_same_v<T, int>)
    {
        VX_SECTION("the key and arguments are moved in when inserting")
        {
            M m;
            K k(2);
            T v(20);
            auto r = m.try_emplace(std::move(k), std::move(v));
            VX_CHECK(r);
            VX_CHECK(r.value().inserted);
            VX_CHECK(val(r.value().it->key) == 2);
            VX_CHECK(val(r.value().it->value) == 20);
            VX_CHECK(val(k) == -1); // moved-from sentinel
            VX_CHECK(val(v) == -1);
        }
    }

    if constexpr (copyable<K>)
    {
        VX_SECTION("lvalue key overload copies the key")
        {
            M m;
            const K k(3);
            auto r = m.try_emplace(k, T(30));
            VX_CHECK(r);
            VX_CHECK(r.value().inserted);
            VX_CHECK(val(k) == 3);
            VX_CHECK(val(m.get(K(3))[0]) == 30);
        }
    }

    if constexpr (std::is_default_constructible_v<T>)
    {
        VX_SECTION("no mapped arguments default-constructs the value")
        {
            M m;
            auto r = m.try_emplace(K(4));
            VX_CHECK(r);
            VX_CHECK(r.value().inserted);
            VX_CHECK(m.contains(K(4)));
        }
    }

    VX_SECTION("growth")
    {
        M m;
        for (int i = 0; i < 1000; ++i)
        {
            auto r = m.try_emplace(K(i), T(mv(i)));
            VX_CHECK(r);
            VX_CHECK(r.value().inserted);
        }
        VX_CHECK(eq_model(m, range(0, 1000)));

        for (int i = 0; i < 1000; ++i)
        {
            VX_CHECK(!m.try_emplace(K(i), T(0)).value().inserted);
        }
        VX_CHECK(eq_model(m, range(0, 1000)));
    }
}
TEST_MAPS(try_emplace)

template <typename M>
void test_insert_or_assign()
{
    using K = key_of<M>;
    using T = mapped_of<M>;

    VX_SECTION("inserts when the key is absent")
    {
        M m;
        auto r = m.insert_or_assign(K(1), T(10));
        VX_CHECK(r);
        VX_CHECK(r.value().inserted);
        VX_CHECK(val(r.value().it->key) == 1);
        VX_CHECK(val(r.value().it->value) == 10);
        VX_CHECK(m.size() == 1);
    }

    VX_SECTION("assigns when the key is present")
    {
        M m;
        auto first = m.insert_or_assign(K(1), T(10));
        VX_CHECK(first);

        auto second = m.insert_or_assign(K(1), T(20));
        VX_CHECK(second);
        VX_CHECK(!second.value().inserted);
        VX_CHECK(second.value().it == first.value().it);
        VX_CHECK(val(second.value().it->value) == 20);
        VX_CHECK(val(*m.get(K(1))) == 20);
        VX_CHECK(m.size() == 1);
    }

    if constexpr (copyable<T>)
    {
        VX_SECTION("an lvalue value is copied, not consumed, when inserting")
        {
            M m;
            T v(30);
            VX_CHECK(m.insert_or_assign(K(2), v).value().inserted);
            VX_CHECK(val(v) == 30);
            VX_CHECK(val(*m.get(K(2))) == 30);
        }

        VX_SECTION("an lvalue value is copied, not consumed, when assigning")
        {
            M m;
            put(m, 2, 1);
            T v(31);
            VX_CHECK(!m.insert_or_assign(K(2), v).value().inserted);
            VX_CHECK(val(v) == 31);
            VX_CHECK(val(*m.get(K(2))) == 31);
        }
    }

    if constexpr (!std::is_same_v<T, int>)
    {
        VX_SECTION("an rvalue value is moved in")
        {
            M m;
            T v(40);
            VX_CHECK(m.insert_or_assign(K(3), std::move(v)).value().inserted);
            VX_CHECK(val(v) == -1);
            VX_CHECK(val(*m.get(K(3))) == 40);

            T w(41);
            VX_CHECK(!m.insert_or_assign(K(3), std::move(w)).value().inserted);
            VX_CHECK(val(w) == -1);
            VX_CHECK(val(*m.get(K(3))) == 41);
        }
    }

    if constexpr (copyable<K>)
    {
        VX_SECTION("lvalue key overload copies the key")
        {
            M m;
            const K k(5);
            VX_CHECK(m.insert_or_assign(k, T(50)).value().inserted);
            VX_CHECK(!m.insert_or_assign(k, T(51)).value().inserted);
            VX_CHECK(val(k) == 5);
            VX_CHECK(val(*m.get(K(5))) == 51);
        }
    }

    VX_SECTION("growth, then overwrite everything")
    {
        M m;
        for (int i = 0; i < 1000; ++i)
        {
            VX_CHECK(m.insert_or_assign(K(i), T(mv(i))).value().inserted);
        }
        VX_CHECK(eq_model(m, range(0, 1000)));

        for (int i = 0; i < 1000; ++i)
        {
            VX_CHECK(!m.insert_or_assign(K(i), T(i)).value().inserted);
        }
        VX_CHECK(m.size() == 1000);
        for (int i = 0; i < 1000; ++i)
        {
            VX_CHECK(val(*m.get(K(i))) == i);
        }
    }
}
TEST_MAPS(insert_or_assign)

template <typename M>
void test_value_mutation()
{
    using K = key_of<M>;
    using T = mapped_of<M>;

    VX_SECTION("assign through a found iterator")
    {
        M m;
        fill(m, 0, 10);
        auto it = m.find(K(3));
        VX_CHECK(it != m.end());
        it->value = T(777);
        VX_CHECK(val(*m.get(K(3))) == 777);
        VX_CHECK(m.size() == 10);
    }

    VX_SECTION("assign through range-for")
    {
        M m;
        fill(m, 0, 100);
        for (auto& e : m)
        {
            e.value = T(val(e.key) + 1000);
        }
        for (int i = 0; i < 100; ++i)
        {
            VX_CHECK(val(*m.get(K(i))) == i + 1000);
        }
    }
}
TEST_MAPS(value_mutation)

//=========================================================================
// 5. erase
//=========================================================================

template <typename M>
void test_erase()
{
    using K = key_of<M>;

    VX_SECTION("by key")
    {
        M m;
        fill(m, 0, 100);
        for (int i = 0; i < 100; i += 2)
        {
            VX_CHECK(m.erase(K(i)) == 1);
        }
        for (int i = 0; i < 100; i += 2)
        {
            VX_CHECK(m.erase(K(i)) == 0);
        }
        VX_CHECK(m.size() == 50);
        for (int i = 0; i < 100; ++i)
        {
            VX_CHECK(m.contains(K(i)) == (i % 2 == 1));
            VX_CHECK((m.get(K(i)) != nullptr) == (i % 2 == 1));
        }
    }

    VX_SECTION("missing key on a non-empty table")
    {
        M m;
        fill(m, 0, 10);
        VX_CHECK(m.erase(K(999)) == 0);
        VX_CHECK(m.size() == 10);
    }

    VX_SECTION("erase leaves the other values intact")
    {
        M m;
        fill(m, 0, 100);
        for (int i = 0; i < 100; i += 3)
        {
            VX_CHECK(m.erase(K(i)) == 1);
        }
        for (int i = 0; i < 100; ++i)
        {
            if (i % 3 != 0)
            {
                VX_CHECK(val(*m.get(K(i))) == mv(i));
            }
        }
    }

    VX_SECTION("by iterator returns the next element")
    {
        M m;
        fill(m, 0, 10);
        auto it = m.find(K(5));
        VX_CHECK(it != m.end());
        auto next = m.erase(it);
        VX_CHECK(m.size() == 9);
        VX_CHECK(!m.contains(K(5)));
        if (next != m.end())
        {
            VX_CHECK(m.contains(K(val(next->key))));
        }

        // erase through the whole table using the returned iterator
        size_t n = 0;
        for (auto i = m.begin(); i != m.end(); i = m.erase(i))
        {
            ++n;
        }
        VX_CHECK(n == 9);
        VX_CHECK(m.empty());
        VX_CHECK(m.begin() == m.end());
        VX_CHECK(ins(m, 1).value().inserted); // still usable
    }

    VX_SECTION("by const_iterator")
    {
        M m;
        fill(m, 0, 10);
        typename M::const_iterator cit = m.find(K(4));
        m.erase(cit);
        VX_CHECK(!m.contains(K(4)));
        VX_CHECK(m.size() == 9);
    }

    VX_SECTION("erase at begin until empty")
    {
        M m;
        fill(m, 0, 64);
        while (!m.empty())
        {
            m.erase(m.begin());
        }
        VX_CHECK(m.begin() == m.end());
    }

    VX_SECTION("conditional erase while iterating")
    {
        M m;
        fill(m, 0, 200);
        for (auto it = m.begin(); it != m.end();)
        {
            if (val(it->key) % 3 == 0)
            {
                it = m.erase(it);
            }
            else
            {
                ++it;
            }
        }

        model expected;
        for (int i = 0; i < 200; ++i)
        {
            if (i % 3 != 0)
            {
                expected.emplace_back(i, mv(i));
            }
        }
        VX_CHECK(eq_model(m, expected));
    }

    VX_SECTION("erase then re-insert gives the new value")
    {
        M m;
        put(m, 1, 10);
        VX_CHECK(m.erase(K(1)) == 1);
        put(m, 1, 20);
        VX_CHECK(val(*m.get(K(1))) == 20);
    }
}
TEST_MAPS(erase)

//=========================================================================
// 6. merge
//=========================================================================

// merge moves elements out of the source and leaves behind the keys the
// target already has. The target's values always win. Both maps are the same type.
template <typename M>
void test_merge()
{
    VX_SECTION("disjoint maps")
    {
        M a;
        M b;
        fill(a, 0, 10);
        fill(b, 10, 20);
        VX_CHECK(a.merge(b));
        VX_CHECK(eq_model(a, range(0, 20)));
        VX_CHECK(b.empty());
        VX_CHECK(b.begin() == b.end());
    }

    VX_SECTION("the target's value wins and the duplicate stays in the source")
    {
        M a;
        M b;
        put(a, 1, 10);
        put(a, 2, 20);
        put(b, 2, 99);
        put(b, 3, 30);

        VX_CHECK(a.merge(b));
        VX_CHECK(eq(a, {
                           { 1, 10 },
                           { 2, 20 },
                           { 3, 30 }
        }));
        VX_CHECK(eq(b, {
                           { 2, 99 }
        }));
    }

    VX_SECTION("duplicates stay in the source")
    {
        M a;
        M b;
        fill(a, 0, 10);
        fill(b, 5, 15);
        VX_CHECK(a.merge(b));
        VX_CHECK(eq_model(a, range(0, 15)));
        VX_CHECK(eq_model(b, range(5, 10)));
    }

    VX_SECTION("empty source / empty target")
    {
        M a;
        M e;
        fill(a, 0, 10);
        VX_CHECK(a.merge(e));
        VX_CHECK(eq_model(a, range(0, 10)));

        M t;
        VX_CHECK(t.merge(a));
        VX_CHECK(eq_model(t, range(0, 10)));
        VX_CHECK(a.empty());
    }

    VX_SECTION("self merge is a no-op")
    {
        M a;
        fill(a, 0, 10);
        VX_CHECK(a.merge(a));
        VX_CHECK(eq_model(a, range(0, 10)));
    }

    VX_SECTION("rvalue source")
    {
        M a;
        M b;
        fill(a, 0, 5);
        fill(b, 5, 10);
        VX_CHECK(a.merge(std::move(b)));
        VX_CHECK(eq_model(a, range(0, 10)));
    }

    VX_SECTION("large merge with growth")
    {
        M a;
        M b;
        fill(a, 0, 1000);
        fill(b, 500, 1500);
        VX_CHECK(a.merge(b));
        VX_CHECK(eq_model(a, range(0, 1500)));
        VX_CHECK(eq_model(b, range(500, 1000)));
    }

    VX_SECTION("both maps stay usable afterwards")
    {
        M a;
        M b;
        fill(a, 0, 20);
        fill(b, 10, 30);
        VX_CHECK(a.merge(b));
        fill(b, 100, 110);
        VX_CHECK(ins(a, 999));
        VX_CHECK(a.size() == 31);
        VX_CHECK(b.size() == 20);
    }
}
TEST_MAPS(merge)

// merge only needs the same value_type, so flat and node maps can feed each other
template <typename K>
void test_merge_across_backends()
{
    using FM = plain_map<flat_family, K, K>;
    using CM = plain_map<node_family, K, K>;

    VX_SECTION("node into flat")
    {
        FM f;
        CM c;
        fill(f, 0, 10);
        fill(c, 5, 20);
        VX_CHECK(f.merge(c));
        VX_CHECK(eq_model(f, range(0, 20)));
        VX_CHECK(eq_model(c, range(5, 10)));
    }

    VX_SECTION("flat into node")
    {
        CM c;
        FM f;
        fill(c, 0, 10);
        fill(f, 5, 20);
        VX_CHECK(c.merge(f));
        VX_CHECK(eq_model(c, range(0, 20)));
        VX_CHECK(eq_model(f, range(5, 10)));
    }

    VX_SECTION("the target's value wins across backends")
    {
        FM f;
        CM c;
        put(f, 1, 10);
        put(c, 1, 99);
        put(c, 2, 20);
        VX_CHECK(f.merge(c));
        VX_CHECK(eq(f, {
                           { 1, 10 },
                           { 2, 20 }
        }));
        VX_CHECK(eq(c, {
                           { 1, 99 }
        }));
    }

    VX_SECTION("large, with growth in the target")
    {
        FM f;
        CM c;
        fill(f, 0, 100);
        fill(c, 50, 3000);
        VX_CHECK(f.merge(std::move(c)));
        VX_CHECK(eq_model(f, range(0, 3000)));
        VX_CHECK(eq_model(c, range(50, 100)));
    }
}
TEST_PLAIN_KEYS(merge_across_backends)

//=========================================================================
// 7. capacity
//=========================================================================

template <typename M>
void test_growth()
{
    using K = key_of<M>;

    VX_SECTION("many inserts keep every element")
    {
        M m;
        fill(m, 0, 2000);
        VX_CHECK(m.size() == 2000);
        VX_CHECK(m.capacity() >= m.size());
        VX_CHECK(eq_model(m, range(0, 2000)));
        for (int i = 0; i < 2000; ++i)
        {
            VX_CHECK(m.contains(K(i)));
        }
    }

    VX_SECTION("reserve")
    {
        M m;
        VX_CHECK(m.reserve(0));
        VX_CHECK(m.capacity() == 0);

        VX_CHECK(m.reserve(100));
        VX_CHECK(m.capacity() >= 100);
        VX_CHECK(m.empty());

        fill(m, 0, 50);
        const size_t cap = m.capacity();
        VX_CHECK(m.reserve(10)); // smaller: no-op
        VX_CHECK(m.capacity() == cap);
        VX_CHECK(eq_model(m, range(0, 50)));
    }

    VX_SECTION("reserve on a populated table keeps elements and values")
    {
        M m;
        fill(m, 0, 40);
        VX_CHECK(m.reserve(5000));
        VX_CHECK(m.capacity() >= 5000);
        VX_CHECK(eq_model(m, range(0, 40)));
    }

    VX_SECTION("clear keeps the table usable")
    {
        M m;
        fill(m, 0, 100);
        const size_t cap = m.capacity();
        m.clear();
        VX_CHECK(m.empty());
        VX_CHECK(m.begin() == m.end());
        VX_CHECK(m.capacity() == cap);
        VX_CHECK(!m.contains(K(5)));
        VX_CHECK(m.get(K(5)) == nullptr);
        fill(m, 0, 100);
        VX_CHECK(eq_model(m, range(0, 100)));
    }

    VX_SECTION("clear_and_deallocate")
    {
        M m;
        fill(m, 0, 100);
        m.clear_and_deallocate();
        VX_CHECK(m.empty());
        VX_CHECK(m.capacity() == 0);
        VX_CHECK(m.begin() == m.end());
        fill(m, 0, 10);
        VX_CHECK(eq_model(m, range(0, 10)));
    }

    VX_SECTION("shrink_to_fit")
    {
        M m;
        fill(m, 0, 200);
        const size_t before = m.capacity();
        for (int i = 3; i < 200; ++i)
        {
            VX_CHECK(m.erase(K(i)) == 1);
        }
        VX_CHECK(m.shrink_to_fit());
        VX_CHECK(m.capacity() <= before);
        VX_CHECK(eq_model(m, range(0, 3)));

        m.clear();
        VX_CHECK(m.shrink_to_fit());
        VX_CHECK(m.capacity() == 0);
        VX_CHECK(ins(m, 1).value().inserted); // usable after deallocating
    }
}
TEST_MAPS(growth)

// reserve(n) must guarantee that n elements fit without another rehash,
// right at the growth boundaries.
template <typename M>
void test_load_boundary()
{
    const int sizes[] = { 1, 2, 3, 7, 8, 9, 14, 15, 16, 17, 31, 32, 33, 63, 64, 65, 127, 128, 129, 1000 };
    for (int n : sizes)
    {
        M m;
        VX_CHECK(m.reserve(static_cast<size_t>(n)));
        const size_t cap = m.capacity();
        VX_CHECK(cap >= static_cast<size_t>(n));

        for (int i = 0; i < n; ++i)
        {
            VX_CHECK(m.insert_assume_capacity(make<M>(i, mv(i))).inserted);
        }
        VX_CHECK(m.capacity() == cap); // no rehash happened
        VX_CHECK(m.size() == static_cast<size_t>(n));
        VX_CHECK(eq_model(m, range(0, n)));
    }

    // size never exceeds capacity at any step of normal growth
    M m;
    for (int i = 0; i < 3000; ++i)
    {
        VX_CHECK(ins(m, i));
        VX_CHECK(m.size() <= m.capacity());
    }
}
TEST_MAPS(load_boundary)

// capacity must stay bounded when elements are constantly inserted and erased
// (flat: tombstone cleanup; node: nothing accumulates)
template <typename M>
void test_tombstone_churn()
{
    using K = key_of<M>;

    VX_SECTION("sliding window does not grow without bound")
    {
        M m;
        const int window = 16;
        const int total = 20000;
        for (int i = 0; i < total; ++i)
        {
            VX_CHECK(ins(m, i).value().inserted);
            if (i >= window)
            {
                VX_CHECK(m.erase(K(i - window)) == 1);
            }
        }
        VX_CHECK(eq_model(m, range(total - window, total)));
        VX_CHECK(m.capacity() < 1024);
    }

    VX_SECTION("fill and drain cycles keep capacity stable")
    {
        M m;
        size_t cap_after_first = 0;
        for (int round = 0; round < 50; ++round)
        {
            fill(m, 0, 100);
            for (int i = 0; i < 100; ++i)
            {
                VX_CHECK(m.erase(K(i)) == 1);
            }
            VX_CHECK(m.empty());
            if (round == 0)
            {
                cap_after_first = m.capacity();
            }
        }
        VX_CHECK(m.capacity() <= cap_after_first * 2);
    }
}
TEST_MAPS(tombstone_churn)

//=========================================================================
// 8. copy / move / swap / assign / compare
//=========================================================================

template <typename M>
void test_copy()
{
    using K = key_of<M>;

    VX_SECTION("copy is independent")
    {
        M a;
        fill(a, 0, 50);
        M b(a);
        VX_CHECK(a == b);
        VX_CHECK(eq_model(b, range(0, 50)));

        VX_CHECK(ins(b, 100).value().inserted);
        VX_CHECK(b.erase(K(0)) == 1);
        VX_CHECK(eq_model(a, range(0, 50))); // a untouched
        VX_CHECK(a != b);

        // changing a value in the copy does not change the original
        VX_CHECK(b.insert_or_assign(K(7), mapped_of<M>(0)));
        VX_CHECK(val(*a.get(K(7))) == mv(7));

        M empty;
        M c(empty);
        VX_CHECK(c.empty());
    }

    VX_SECTION("copy assign")
    {
        M a;
        fill(a, 0, 30);
        M b;
        fill(b, 100, 105);
        b = a;
        VX_CHECK(b == a);
        VX_CHECK(eq_model(b, range(0, 30)));

        M& self = b;
        b = self;
        VX_CHECK(eq_model(b, range(0, 30)));

        // assigning an empty map empties the target
        M empty;
        b = empty;
        VX_CHECK(b.empty());
    }

    VX_SECTION("copy of a table with erased elements")
    {
        M a;
        fill(a, 0, 100);
        for (int i = 0; i < 100; i += 3)
        {
            VX_CHECK(a.erase(K(i)) == 1);
        }
        M b(a);
        VX_CHECK(a == b);
        VX_CHECK(b.size() == a.size());
        for (int i = 0; i < 100; ++i)
        {
            VX_CHECK(b.contains(K(i)) == (i % 3 != 0));
            if (i % 3 != 0)
            {
                VX_CHECK(val(*b.get(K(i))) == mv(i));
            }
        }
    }
}
TEST_MAPS_COPYABLE(copy)

template <typename M>
void test_move_swap()
{
    using K = key_of<M>;

    VX_SECTION("move construct")
    {
        M a;
        fill(a, 0, 50);
        M b(std::move(a));
        VX_CHECK(eq_model(b, range(0, 50)));
        VX_CHECK(a.empty());
        VX_CHECK(a.begin() == a.end());
        VX_CHECK(!a.contains(K(1)));
        VX_CHECK(ins(a, 1).value().inserted); // moved-from is reusable
    }

    VX_SECTION("move assign")
    {
        M b;
        fill(b, 0, 50);
        M c;
        fill(c, 500, 510);
        c = std::move(b);
        VX_CHECK(eq_model(c, range(0, 50)));
        VX_CHECK(b.empty());
        VX_CHECK(ins(b, 1).value().inserted);
    }

    VX_SECTION("move of an empty table")
    {
        M a;
        M b(std::move(a));
        VX_CHECK(b.empty());
        VX_CHECK(a.empty());
        VX_CHECK(ins(b, 1).value().inserted);
    }

    VX_SECTION("swap")
    {
        M a;
        M b;
        fill(a, 0, 10);
        fill(b, 100, 103);
        a.swap(b);
        VX_CHECK(eq_model(a, range(100, 103)));
        VX_CHECK(eq_model(b, range(0, 10)));

        M e;
        a.swap(e);
        VX_CHECK(a.empty());
        VX_CHECK(eq_model(e, range(100, 103)));

        // both empty
        M x;
        M y;
        x.swap(y);
        VX_CHECK(x.empty() && y.empty());
    }

    VX_SECTION("self swap / self move assign leave a usable table")
    {
        M a;
        fill(a, 0, 20);
        a.swap(a);
        VX_CHECK(eq_model(a, range(0, 20)));

        M& r = a;
        a = std::move(r);
        // valid but unspecified: just require it is still a working map
        VX_CHECK(ins(a, 1000));
        VX_CHECK(a.contains(K(1000)));
        VX_CHECK(a.size() == static_cast<size_t>(std::distance(a.begin(), a.end())));
    }

    VX_SECTION("equality ignores insertion order and capacity")
    {
        M a;
        M b;
        fill(a, 0, 40);
        for (int i = 39; i >= 0; --i)
        {
            VX_CHECK(ins(b, i));
        }
        VX_CHECK(b.reserve(500)); // different capacity
        VX_CHECK(a == b);
        VX_CHECK(b.erase(K(7)) == 1);
        VX_CHECK(a != b);
        VX_CHECK(ins(b, 7));
        VX_CHECK(a == b);
        VX_CHECK(M() == M());

        // same size, different keys
        M c;
        M d;
        fill(c, 0, 5);
        fill(d, 1, 6);
        VX_CHECK(c != d);
    }

    VX_SECTION("equality compares the values too")
    {
        M a;
        M b;
        fill(a, 0, 20);
        fill(b, 0, 20);
        VX_CHECK(a == b);

        // same keys, one value differs
        VX_CHECK(b.insert_or_assign(K(5), mapped_of<M>(0)));
        VX_CHECK(a != b);
        VX_CHECK(b.insert_or_assign(K(5), mapped_of<M>(mv(5))));
        VX_CHECK(a == b);
    }
}
TEST_MAPS(move_swap)

template <typename M>
void test_assign()
{
    using V = typename M::value_type;

    VX_SECTION("assign(const&) / assign(&&)")
    {
        M a;
        fill(a, 0, 20);
        M b;
        fill(b, 100, 110);

        VX_CHECK(b.assign(a));
        VX_CHECK(b == a);

        M c;
        fill(c, 200, 205);
        VX_CHECK(b.assign(std::move(c)));
        VX_CHECK(eq_model(b, range(200, 205)));

        VX_CHECK(b.assign(M()));
        VX_CHECK(b.empty());
    }

    VX_SECTION("assign(range)")
    {
        std::vector<V> src;
        for (int i = 0; i < 30; ++i)
        {
            src.push_back(make<M>(i % 15, mv(i % 15)));
        }
        M m;
        fill(m, 100, 120);
        VX_CHECK(m.assign(src.begin(), src.end()));
        VX_CHECK(eq_model(m, range(0, 15)));

        std::vector<V> none;
        VX_CHECK(m.assign(none.begin(), none.end()));
        VX_CHECK(m.empty());
    }

    VX_SECTION("assign(initializer_list) / operator=(initializer_list)")
    {
        M m;
        fill(m, 100, 120);
        VX_CHECK(m.assign(std::initializer_list<V>{ make<M>(1, 10), make<M>(2, 20), make<M>(2, 99), make<M>(3, 30) }));
        VX_CHECK(eq(m, {
                           { 1, 10 },
                           { 2, 20 },
                           { 3, 30 }
        }));

        m = std::initializer_list<V>{ make<M>(7, 70), make<M>(8, 80) };
        VX_CHECK(eq(m, {
                           { 7, 70 },
                           { 8, 80 }
        }));
    }
}
TEST_MAPS_COPYABLE(assign)

//=========================================================================
// 9. iteration
//=========================================================================

template <typename M>
void test_iteration()
{
    using K = key_of<M>;
    using It = typename M::iterator;
    using CIt = typename M::const_iterator;

    VX_STATIC_CHECK((std::is_convertible_v<It, CIt>));
    VX_STATIC_CHECK((std::is_convertible_v<typename std::iterator_traits<It>::iterator_category, std::forward_iterator_tag>));
    VX_STATIC_CHECK((std::is_convertible_v<typename std::iterator_traits<CIt>::iterator_category, std::forward_iterator_tag>));

    M m;
    VX_CHECK(m.begin() == m.end());
    VX_CHECK(m.cbegin() == m.cend());

    fill(m, 0, 100);
    size_t n = 0;
    for (auto it = m.begin(); it != m.end(); ++it)
    {
        ++n;
    }
    VX_CHECK(n == 100);
    VX_CHECK(eq_model(m, range(0, 100)));

    // range-for, mutable and const
    n = 0;
    for (const auto& e : m)
    {
        (void)e;
        ++n;
    }
    VX_CHECK(n == 100);

    const M& cm = m;
    n = 0;
    for (const auto& e : cm)
    {
        VX_CHECK(val(e.value) == mv(val(e.key)));
        ++n;
    }
    VX_CHECK(n == 100);

    // post-increment returns the old position; const iteration
    n = 0;
    for (auto it = cm.cbegin(); it != cm.cend(); it++)
    {
        ++n;
    }
    VX_CHECK(n == 100);

    {
        auto it = m.begin();
        auto old = it++;
        VX_CHECK(old == m.begin());
        VX_CHECK(it != old);
    }

    // iterator converts to const_iterator and compares equal
    {
        It it = m.begin();
        CIt cit = it;
        VX_CHECK(cit == cm.begin());
        VX_CHECK(cit == it);
    }

    // each key visited exactly once
    {
        const model seen = contents(m);
        VX_CHECK(std::adjacent_find(seen.begin(), seen.end(), [](const auto& a, const auto& b)
                     { return a.first == b.first; }) == seen.end());
    }

    // sparse table (many erased): iteration still visits exactly the survivors
    for (int i = 0; i < 100; ++i)
    {
        if (i % 10 != 0)
        {
            VX_CHECK(m.erase(K(i)) == 1);
        }
    }
    VX_CHECK(eq(m, {
                       { 0,  mv(0)  },
                       { 10, mv(10) },
                       { 20, mv(20) },
                       { 30, mv(30) },
                       { 40, mv(40) },
                       { 50, mv(50) },
                       { 60, mv(60) },
                       { 70, mv(70) },
                       { 80, mv(80) },
                       { 90, mv(90) }
    }));

    // single element in a large table
    M big;
    VX_CHECK(big.reserve(1000));
    VX_CHECK(ins(big, 42));
    n = 0;
    for (const auto& e : big)
    {
        VX_CHECK(val(e.key) == 42);
        VX_CHECK(val(e.value) == mv(42));
        ++n;
    }
    VX_CHECK(n == 1);
}
TEST_MAPS(iteration)

//=========================================================================
// 10. hashing
//=========================================================================

template <typename F>
void test_default_hasher()
{
    using M = typename F::template map<int, int>;

    M m;
    for (int i = 0; i < 1000; ++i)
    {
        VX_CHECK(ins(m, i));
    }
    VX_CHECK(m.size() == 1000);
    for (int i = 0; i < 1000; ++i)
    {
        VX_CHECK(m.contains(i));
        VX_CHECK(*m.get(i) == mv(i));
    }
    VX_CHECK(!m.contains(1000));
    VX_CHECK(m.erase(500) == 1);
    VX_CHECK(!m.contains(500));

    // negative and extreme keys
    VX_CHECK(m.insert(make<M>(-1, 1)));
    VX_CHECK(m.insert(make<M>(INT32_MIN, 2)));
    VX_CHECK(m.insert(make<M>(INT32_MAX, 3)));
    VX_CHECK(m.contains(-1) && m.contains(INT32_MIN) && m.contains(INT32_MAX));
    VX_CHECK(*m.get(INT32_MIN) == 2);
    VX_CHECK(*m.get(INT32_MAX) == 3);
}
TEST_FAMILIES(default_hasher)

template <typename F, typename K>
void test_degenerate_hash()
{
    using M = degenerate_map<F, K, K>;
    using T = K;

    M m;
    for (int i = 0; i < 300; ++i)
    {
        VX_CHECK(ins(m, i).value().inserted);
    }
    VX_CHECK(!ins(m, 150).value().inserted);
    VX_CHECK(eq_model(m, range(0, 300)));
    for (int i = 0; i < 300; ++i)
    {
        VX_CHECK(m.contains(K(i)));
    }
    VX_CHECK(!m.contains(K(300)));

    // erase in the middle of long nodes, then look everything up again
    for (int i = 0; i < 300; i += 2)
    {
        VX_CHECK(m.erase(K(i)) == 1);
    }
    for (int i = 0; i < 300; ++i)
    {
        VX_CHECK(m.contains(K(i)) == (i % 2 == 1));
    }

    // reinsertion after erase (flat: tombstone reuse), with a new value
    for (int i = 0; i < 300; i += 2)
    {
        VX_CHECK(m.insert(make<M>(i, mv(i) + 1)).value().inserted);
    }
    for (int i = 0; i < 300; ++i)
    {
        VX_CHECK(val(*m.get(K(i))) == (i % 2 == 0 ? mv(i) + 1 : mv(i)));
    }

    // the other API calls on a single node
    VX_CHECK(m.insert_or_assign(K(10), T(0)).value().inserted == false);
    VX_CHECK(val(*m.get(K(10))) == 0);
    VX_CHECK(m.try_emplace(K(1000), T(5)).value().inserted);
    VX_CHECK(m.erase(K(1000)) == 1);

    // churn through a single node
    for (int i = 300; i < 1500; ++i)
    {
        VX_CHECK(ins(m, i).value().inserted);
        VX_CHECK(m.erase(K(i - 300)) == 1);
    }
    VX_CHECK(eq_model(m, range(1200, 1500)));
}
TEST_KEYS(degenerate_hash)

template <typename F, typename K>
void test_avalanching_hash()
{
    using M = avalanche_map<F, K, K>;

    M m;
    fill(m, 0, 2000);
    VX_CHECK(eq_model(m, range(0, 2000)));
    for (int i = 0; i < 2000; ++i)
    {
        VX_CHECK(m.contains(K(i)));
    }
    for (int i = 0; i < 2000; i += 2)
    {
        VX_CHECK(m.erase(K(i)) == 1);
    }
    VX_CHECK(m.size() == 1000);
    VX_CHECK(m.shrink_to_fit());
    for (int i = 0; i < 2000; ++i)
    {
        VX_CHECK(m.contains(K(i)) == (i % 2 == 1));
        if (i % 2 == 1)
        {
            VX_CHECK(val(*m.get(K(i))) == mv(i));
        }
    }
}
TEST_KEYS(avalanching_hash)

template <typename F>
void test_transparent_lookup()
{
    using M = typename F::template map<wide_key, int, wide_hash, wide_eq>;

    M m;
    for (int i = 0; i < 50; ++i)
    {
        VX_CHECK(m.emplace(wide_key(i), mv(i)));
    }

    // lookups by int: no wide_key temporary needed
    VX_CHECK(m.find(7) != m.end());
    VX_CHECK(m.find(7)->key.v == 7);
    VX_CHECK(m.find(70) == m.end());
    VX_CHECK(m.contains(7));
    VX_CHECK(!m.contains(70));
    VX_CHECK(m.count(7) == 1);
    VX_CHECK(m.count(70) == 0);

    // get / at by int
    VX_CHECK(m.get(7) != nullptr);
    VX_CHECK(*m.get(7) == mv(7));
    VX_CHECK(m.get(70) == nullptr);
    {
        auto r = m.at(8);
        VX_CHECK(r);
        VX_CHECK(r.value() == mv(8));
        VX_CHECK_EXPECTED_ERROR(m.at(80), vx::err::out_of_range);
    }

    const M& cm = m;
    VX_CHECK(cm.find(8) != cm.end());
    VX_CHECK(cm.contains(8));
    VX_CHECK(cm.get(8) != nullptr);
    VX_CHECK(cm.at(8));

    // lookups by key type still work
    VX_CHECK(m.contains(wide_key(9)));

    // erase by heterogeneous key
    VX_CHECK(m.erase(7));
    VX_CHECK(!m.erase(7));
    VX_CHECK(!m.contains(7));

    // erase(iterator) must still pick the iterator overload, not erase<Q>
    auto next = m.erase(m.find(9));
    (void)next;
    VX_CHECK(!m.contains(9));

    VX_CHECK(m.erase(wide_key(10)));
    VX_CHECK(m.size() == 47);
}
TEST_FAMILIES(transparent_lookup)

template <typename F>
void test_stateful_functors()
{
    using M = typename F::template map<int, int, seeded_hash, tagged_eq>;

    M a(seeded_hash(7), tagged_eq(9));
    VX_CHECK(a.hash_function().seed == 7);
    VX_CHECK(a.key_eq().tag == 9);
    fill(a, 0, 50);

    M copy(a);
    VX_CHECK(copy.hash_function().seed == 7);
    VX_CHECK(copy.key_eq().tag == 9);
    VX_CHECK(copy == a);

    M moved(std::move(copy));
    VX_CHECK(moved.hash_function().seed == 7);
    VX_CHECK(moved.key_eq().tag == 9);
    VX_CHECK(eq_model(moved, range(0, 50)));

    auto made = M::create(a);
    VX_CHECK(made);
    VX_CHECK(made.value().hash_function().seed == 7);
    VX_CHECK(made.value().key_eq().tag == 9);

    // swap exchanges the functors together with the contents
    M b(seeded_hash(1), tagged_eq(2));
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
        VX_CHECK(*a.get(i) == mv(i));
    }
}
TEST_FAMILIES(stateful_functors)

//=========================================================================
// 11. element lifetime
//=========================================================================

// movable_int has a deleted copy constructor, so the generic tests above
// already prove that growth / relocation / move never copy a key or a value.
// This checks that the consuming calls really consume their arguments.
template <typename F>
void test_move_only_consumption()
{
    using K = movable_int;
    using T = movable_int;
    using M = plain_map<F, K, T>;

    VX_STATIC_CHECK(!std::is_copy_constructible_v<K>);
    VX_STATIC_CHECK(!std::is_copy_constructible_v<T>);

    M m;

    typename M::value_type e = make<M>(5, 50);
    auto r = m.insert(std::move(e));
    VX_CHECK(r);
    VX_CHECK(r.value().inserted);
    VX_CHECK(val(r.value().it->key) == 5);
    VX_CHECK(val(r.value().it->value) == 50);

    // a duplicate is not inserted, and the argument is left as it was
    auto dup = m.insert(make<M>(5, 1));
    VX_CHECK(!dup.value().inserted);
    VX_CHECK(val(dup.value().it->value) == 50);
}
TEST_FAMILIES(move_only_consumption)

// constructs vs destroys, copies vs moves, for keys AND mapped values
template <typename F>
void test_lifetime_counted()
{
    using CM = counted_map<F>;
    using V = typename CM::value_type;

    auto& ks = counted_key::stats();
    auto& vs = counted_val::stats();
    reset_counted();

    {
        CM s;
        CM m;
        CM o;

        auto live_matches = [&]()
        {
            const size_t n = s.size() + m.size() + o.size();
            return ks.live == n && vs.live == n && ks.bad == 0 && vs.bad == 0;
        };

        // Sections share s / m / o and run one after another, so every section
        // starts by emptying them and zeroing the counters.
        auto fresh = [&]()
        {
            s.clear();
            m.clear();
            o.clear();
            reset_counted();
        };

        VX_SECTION("insert + growth: rvalue inserts never copy")
        {
            fresh();
            fill(s, 0, 1000);
            VX_CHECK(s.size() == 1000);
            VX_CHECK(live_matches());
            VX_CHECK(ks.copies == 0 && vs.copies == 0); // relocation on growth moves (or does nothing)

            for (int i = 1000; i < 1100; ++i)
            {
                VX_CHECK(s.emplace(counted_key(i), counted_val(mv(i))));
            }
            VX_CHECK(live_matches());
            VX_CHECK(ks.copies == 0 && vs.copies == 0);
        }

        VX_SECTION("duplicate insert does not copy")
        {
            fresh();
            {
                const V dup = make<CM>(5, 50); // lives outside the map, so scope it
                VX_CHECK(s.insert(dup).value().inserted);

                const size_t kc = ks.copies;
                const size_t vc = vs.copies;
                VX_CHECK(!s.insert(dup).value().inserted);
                VX_CHECK(!s.emplace(counted_key(5), counted_val(1)).value().inserted);
                VX_CHECK(ks.copies == kc);
                VX_CHECK(vs.copies == vc);
            }
            VX_CHECK(live_matches());
        }

        VX_SECTION("try_emplace on an existing key moves nothing")
        {
            fresh();
            VX_CHECK(s.try_emplace(counted_key(7), counted_val(70)).value().inserted);

            const size_t km = ks.moves;
            const size_t vm = vs.moves;
            const size_t kc = ks.copies;
            const size_t vc = vs.copies;
            VX_CHECK(!s.try_emplace(counted_key(7), counted_val(71)).value().inserted);
            VX_CHECK(ks.moves == km && vs.moves == vm);
            VX_CHECK(ks.copies == kc && vs.copies == vc);
            VX_CHECK(val(*s.get(counted_key(7))) == 70);
            VX_CHECK(live_matches());
        }

        VX_SECTION("insert_or_assign on an existing key assigns in place")
        {
            fresh();
            VX_CHECK(s.insert_or_assign(counted_key(3), counted_val(30)).value().inserted);
            VX_CHECK(!s.insert_or_assign(counted_key(3), counted_val(31)).value().inserted);
            VX_CHECK(s.size() == 1);
            VX_CHECK(val(*s.get(counted_key(3))) == 31);
            VX_CHECK(ks.copies == 0 && vs.copies == 0);
            VX_CHECK(live_matches());
        }

        VX_SECTION("erase destroys key and value")
        {
            fresh();
            fill(s, 1000, 1200);
            for (int i = 1000; i < 1200; i += 2)
            {
                VX_CHECK(s.erase(counted_key(i)) == 1);
            }
            VX_CHECK(s.size() == 100);
            VX_CHECK(live_matches());

            for (auto it = s.begin(); it != s.end();)
            {
                it = s.erase(it);
            }
            VX_CHECK(s.empty());
            VX_CHECK(live_matches());
        }

        VX_SECTION("clear / shrink / reserve")
        {
            fresh();
            fill(s, 0, 200);
            for (int i = 0; i < 200; i += 2)
            {
                VX_CHECK(s.erase(counted_key(i)) == 1);
            }
            const size_t kc = ks.copies;
            const size_t vc = vs.copies;
            VX_CHECK(s.shrink_to_fit());
            VX_CHECK(s.reserve(5000));
            VX_CHECK(ks.copies == kc && vs.copies == vc); // rehash never copies
            VX_CHECK(s.size() == 100);

            s.clear();
            VX_CHECK(s.empty());
            VX_CHECK(live_matches());
        }

        VX_SECTION("copy / move / swap / assign")
        {
            fresh();
            fill(s, 0, 100);
            const size_t kc = ks.copies;
            const size_t vc = vs.copies;

            CM c(s);
            VX_CHECK(ks.copies - kc == 100); // one copy per key
            VX_CHECK(vs.copies - vc == 100); // and per value
            VX_CHECK(ks.live == s.size() + c.size());
            VX_CHECK(vs.live == s.size() + c.size());

            const size_t kn = ks.constructed;
            const size_t vn = vs.constructed;
            m = std::move(c);
            VX_CHECK(ks.constructed == kn && vs.constructed == vn); // buffer transfer, no per-element work
            VX_CHECK(c.empty());
            VX_CHECK(live_matches());

            o.swap(m);
            VX_CHECK(m.empty());
            VX_CHECK(o.size() == 100);
            VX_CHECK(live_matches());

            // overwriting a populated table destroys its old elements
            fill(m, 500, 520);
            m = s;
            VX_CHECK(m == s);
            VX_CHECK(live_matches());

            VX_CHECK(m.assign(CM()));
            VX_CHECK(m.empty());
            VX_CHECK(live_matches());
        }

        VX_SECTION("merge never copies and keeps every element alive exactly once")
        {
            fresh();
            fill(s, 0, 50);
            fill(m, 25, 75);
            const size_t kc = ks.copies;
            const size_t vc = vs.copies;

            VX_CHECK(s.merge(m));
            VX_CHECK(s.size() == 75);
            VX_CHECK(m.size() == 25); // the duplicates stay behind
            VX_CHECK(ks.copies == kc && vs.copies == vc);
            VX_CHECK(live_matches());
        }

        VX_SECTION("self swap")
        {
            fresh();
            fill(s, 0, 50);
            s.swap(s);
            VX_CHECK(s.size() == 50);
            VX_CHECK(live_matches());
        }
    }

    // everything destroyed exactly once, nothing touched after destruction
    VX_CHECK(ks.live == 0 && vs.live == 0);
    VX_CHECK(ks.constructed == ks.destroyed);
    VX_CHECK(vs.constructed == vs.destroyed);
    VX_CHECK(ks.bad == 0 && vs.bad == 0);
}
TEST_FAMILIES(lifetime_counted)

//=========================================================================
// 12. allocators
//=========================================================================

template <typename F>
void test_allocator_no_leaks()
{
    using K = copyable_int;
    using A = tracking_allocator<K>;
    using M = tracked_map<F, K, K>;

    allocator_stats stats;
    {
        M m(test_hash<K>(), std::equal_to<K>(), A(1, &stats));
        VX_CHECK(m.get_allocator().id() == 1);
        VX_CHECK(stats.allocate_calls == 0); // empty table does not allocate

        fill(m, 0, 500);
        VX_CHECK(stats.allocate_calls > 0);
        VX_CHECK(stats.live_allocations > 0);

        m.clear();
        VX_CHECK(stats.live_allocations > 0); // clear keeps the storage
        m.clear_and_deallocate();
        VX_CHECK(stats.live_allocations == 0);
        VX_CHECK(m.capacity() == 0);

        fill(m, 0, 100);

        // copy uses the same allocator (and stats)
        M c(m);
        VX_CHECK(c == m);
        VX_CHECK(c.get_allocator().id() == 1);

        // moving transfers the buffer: no new allocation
        const size_t before = stats.allocate_calls;
        M mv_(std::move(c));
        VX_CHECK(stats.allocate_calls == before);
        VX_CHECK(mv_ == m);
        VX_CHECK(mv_.get_allocator().id() == 1);

        m.clear();
        mv_.clear();
        VX_CHECK(m.shrink_to_fit());
        VX_CHECK(mv_.shrink_to_fit());
        VX_CHECK(stats.live_allocations == 0);
    }
    VX_CHECK(stats.live_allocations == 0);
    VX_CHECK(stats.allocate_calls == stats.deallocate_calls);
}
TEST_FAMILIES(allocator_no_leaks)

template <typename F>
void test_allocator_get_allocator_round_trips()
{
    // the table rebinds the allocator internally: get_allocator() must still
    // return the original allocator type with the original state
    using K = copyable_int;
    using A = tracking_allocator<K>;
    using M = tracked_map<F, K, K>;

    allocator_stats stats;
    M m(test_hash<K>(), std::equal_to<K>(), A(42, &stats));
    fill(m, 0, 100);

    A a = m.get_allocator();
    VX_CHECK(a.id() == 42);
    VX_CHECK(a.stats() == &stats);
    VX_STATIC_CHECK((std::is_same_v<typename M::allocator_type, A>));
}
TEST_FAMILIES(allocator_get_allocator_round_trips)

template <typename F>
void test_allocator_propagation()
{
    using K = copyable_int;
    using A = tracking_allocator<K>;
    using M = tracked_map<F, K, K>;

    allocator_stats s1;
    allocator_stats s2;
    {
        M a(test_hash<K>(), std::equal_to<K>(), A(1, &s1));
        fill(a, 0, 20);

        auto same = M::create(a); // keeps a's allocator
        VX_CHECK(same);
        VX_CHECK(same.value().get_allocator().id() == 1);
        VX_CHECK(same.value() == a);

        auto other = M::create(a, A(2, &s2)); // re-inserts into a different allocator
        VX_CHECK(other);
        VX_CHECK(other.value().get_allocator().id() == 2);
        VX_CHECK(other.value() == a);
        VX_CHECK(s2.live_allocations > 0);
    }
    VX_CHECK(s1.live_allocations == 0);
    VX_CHECK(s2.live_allocations == 0);
}
TEST_FAMILIES(allocator_propagation)

template <typename F>
void test_allocator_assign_and_swap()
{
    using K = copyable_int;
    using A = tracking_allocator<K>;
    using M = tracked_map<F, K, K>;

    allocator_stats s1;
    allocator_stats s2;
    {
        M a(test_hash<K>(), std::equal_to<K>(), A(1, &s1));
        M b(test_hash<K>(), std::equal_to<K>(), A(2, &s2));
        fill(a, 0, 30);
        fill(b, 100, 140);

        // copy assign between unequal allocators: whichever allocator the
        // implementation picks, contents must be right and nothing may leak
        a = b;
        VX_CHECK(eq_model(a, range(100, 140)));
        VX_CHECK(eq_model(b, range(100, 140)));

        // move assign between unequal allocators
        M c(test_hash<K>(), std::equal_to<K>(), A(3, &s1));
        fill(c, 500, 520);
        c = std::move(b);
        VX_CHECK(eq_model(c, range(100, 140)));

        // swap with EQUAL allocators
        M d(test_hash<K>(), std::equal_to<K>(), A(1, &s1));
        fill(d, 900, 910);
        M e(test_hash<K>(), std::equal_to<K>(), A(1, &s1));
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
TEST_FAMILIES(allocator_assign_and_swap)

//=========================================================================
// 13. error reporting
//
// "Not found" and "already present" are results, not errors (covered by the
// insert / erase / at tests above). Failures are one of:
//   size_error      the request cannot be represented, nothing is attempted
//   out_of_memory   allocation failed; the table must be left as it was
//=========================================================================

template <typename M>
void test_size_errors()
{
    const size_t huge = std::numeric_limits<size_t>::max();

    VX_SECTION("reserve beyond max_size leaves the table untouched")
    {
        M m;
        fill(m, 0, 20);
        const size_t cap = m.capacity();

        VX_CHECK_ERROR(m.reserve(m.max_size() + 1), vx::err::size_error);
        VX_CHECK_ERROR(m.reserve(huge), vx::err::size_error);

        VX_CHECK(m.capacity() == cap);
        VX_CHECK(eq_model(m, range(0, 20)));
        VX_CHECK(ins(m, 100)); // still usable
    }

    VX_SECTION("reserve beyond max_size on an empty table")
    {
        M m;
        VX_CHECK_ERROR(m.reserve(huge), vx::err::size_error);
        VX_CHECK(m.capacity() == 0);
        VX_CHECK(m.empty());
        VX_CHECK(ins(m, 1));
    }

    VX_SECTION("create with an impossible size")
    {
        VX_CHECK_EXPECTED_ERROR(M::create(huge), vx::err::size_error);
    }
}
TEST_MAPS(size_errors)

template <typename F>
void test_error_out_of_memory_first_allocation()
{
    using A = failing_allocator<int>;
    using M = failing_map<F, int, int>;

    fail_state st;
    {
        M m(test_hash<int>(), std::equal_to<int>(), A{ &st });
        st.countdown = 0;

        VX_CHECK_EXPECTED_ERROR(ins(m, 1), vx::err::out_of_memory);
        VX_CHECK_EXPECTED_ERROR(m.emplace(2, 20), vx::err::out_of_memory);
        VX_CHECK_ERROR(m.reserve(10), vx::err::out_of_memory);

        VX_CHECK(m.empty());
        VX_CHECK(m.capacity() == 0);
        VX_CHECK(m.begin() == m.end());
        VX_CHECK(st.failures > 0);

        // recovers once allocation works again
        st.countdown = -1;
        VX_CHECK(ins(m, 1));
        VX_CHECK(eq(m, {
                           { 1, mv(1) }
        }));
    }
    VX_CHECK(st.live == 0);
}
TEST_FAMILIES(error_out_of_memory_first_allocation)

// the map-specific inserting calls report out_of_memory and change nothing
template <typename F>
void test_error_out_of_memory_map_api()
{
    using A = failing_allocator<int>;
    using M = failing_map<F, int, int>;

    fail_state st;
    auto make_map = [&]()
    {
        return M(test_hash<int>(), std::equal_to<int>(), A{ &st });
    };

    VX_SECTION("on an empty map")
    {
        M m = make_map();
        st.countdown = 0;
        VX_CHECK_EXPECTED_ERROR(m.try_emplace(1, 10), vx::err::out_of_memory);
        VX_CHECK_EXPECTED_ERROR(m.insert_or_assign(2, 20), vx::err::out_of_memory);
        VX_CHECK_EXPECTED_ERROR(m.get_or_insert(3), vx::err::out_of_memory);

        VX_CHECK(m.empty());
        VX_CHECK(m.capacity() == 0);

        st.countdown = -1;
        VX_CHECK(m.try_emplace(1, 10));
        VX_CHECK(m.insert_or_assign(2, 20));
        VX_CHECK(m.get_or_insert(3));
        VX_CHECK(m.size() == 3);
    }

    st.countdown = -1; // a failed check must not leak a countdown into the next section

    VX_SECTION("when the table is full")
    {
        M m = make_map();
        VX_CHECK(m.reserve(10));
        const int n = static_cast<int>(m.capacity());
        fill(m, 0, n);
        VX_CHECK(m.size() == m.capacity());

        st.countdown = 0;
        VX_CHECK_EXPECTED_ERROR(m.try_emplace(1000, 1), vx::err::out_of_memory);
        VX_CHECK_EXPECTED_ERROR(m.insert_or_assign(1001, 1), vx::err::out_of_memory);
        VX_CHECK_EXPECTED_ERROR(m.get_or_insert(1002), vx::err::out_of_memory);

        VX_CHECK(eq_model(m, range(0, n)));

        VX_CHECK(!m.try_emplace(0, 5).value().inserted);
        VX_CHECK(!m.insert_or_assign(0, 5).value().inserted);
        VX_CHECK(*m.get(0) == 5);
        VX_CHECK(m.get_or_insert(1));

        st.countdown = -1;
        VX_CHECK(m.try_emplace(1000, 1));
        VX_CHECK(m.contains(1000));
    }

    VX_CHECK(st.live == 0);
}
TEST_FAMILIES(error_out_of_memory_map_api)

template <typename F>
void test_error_out_of_memory_growth()
{
    using A = failing_allocator<int>;
    using M = failing_map<F, int, int>;

    fail_state st;
    {
        M m(test_hash<int>(), std::equal_to<int>(), A{ &st });

        // fill to exactly the current capacity, so the next insert must grow
        VX_CHECK(m.reserve(10));
        const int n = static_cast<int>(m.capacity());
        fill(m, 0, n);
        VX_CHECK(m.size() == m.capacity());

        st.countdown = 0;
        VX_CHECK_EXPECTED_ERROR(ins(m, 1000), vx::err::out_of_memory);
        VX_CHECK_EXPECTED_ERROR(m.emplace(1001, 1), vx::err::out_of_memory);

        // nothing was lost or added
        VX_CHECK(eq_model(m, range(0, n)));
        VX_CHECK(!m.contains(1000));

        st.countdown = -1;
        VX_CHECK(ins(m, 1000));
        VX_CHECK(m.contains(1000));
    }
    VX_CHECK(st.live == 0);
}
TEST_FAMILIES(error_out_of_memory_growth)

// fail the Nth allocation for every N: the table must report the error,
// keep exactly what was inserted so far, and stay usable.
// (node_map also allocates once per element, so this hits node allocation too.)
template <typename F>
void test_error_out_of_memory_insert_sweep()
{
    using A = failing_allocator<int>;
    using M = failing_map<F, int, int>;

    for (long budget = 0; budget < 10; ++budget)
    {
        fail_state st;
        st.countdown = budget;
        {
            M m(test_hash<int>(), std::equal_to<int>(), A{ &st });
            model inserted;
            bool failed = false;

            for (int i = 0; i < 500; ++i)
            {
                auto r = ins(m, i);
                if (!r)
                {
                    failed = true;
                    break;
                }
                inserted.emplace_back(i, mv(i));
                if (i % 16 == 0)
                {
                    VX_CHECK(eq_model(m, inserted));
                }
            }

            if (budget == 0)
            {
                VX_CHECK(failed);
            }
            VX_CHECK(eq_model(m, inserted));
            for (const auto& kv : inserted)
            {
                VX_CHECK(m.contains(kv.first));
            }

            // usable again once allocation works
            st.countdown = -1;
            VX_CHECK(ins(m, 100000));
            VX_CHECK(m.contains(100000));
        }
        VX_CHECK(st.live == 0);
    }
}
TEST_FAMILIES(error_out_of_memory_insert_sweep)

// same sweep with lifetime-counting keys AND values, through every inserting
// call: a failed call must not leak or double destroy what it was inserting
template <typename F>
void test_error_out_of_memory_leaks_no_elements()
{
    using A = failing_allocator<counted_key>;
    using M = typename F::template map<counted_key, counted_val, test_hash<counted_key>, std::equal_to<counted_key>, A>;

    auto& ks = counted_key::stats();
    auto& vs = counted_val::stats();

    for (long budget = 0; budget < 8; ++budget)
    {
        reset_counted();
        fail_state st;
        st.countdown = budget;
        {
            M m(test_hash<counted_key>(), std::equal_to<counted_key>(), A{ &st });
            for (int i = 0; i < 300; ++i)
            {
                bool ok = false;
                switch (i % 3)
                {
                    case 0:
                        ok = static_cast<bool>(m.insert(make<M>(i, mv(i))));
                        break;
                    case 1:
                        ok = static_cast<bool>(m.try_emplace(counted_key(i), counted_val(mv(i))));
                        break;
                    default:
                        ok = static_cast<bool>(m.insert_or_assign(counted_key(i), counted_val(mv(i))));
                        break;
                }
                if (!ok)
                {
                    break;
                }
            }
            VX_CHECK(ks.live == m.size());
            VX_CHECK(vs.live == m.size());
            VX_CHECK(ks.bad == 0 && vs.bad == 0);
        }
        VX_CHECK(ks.live == 0 && vs.live == 0);
        VX_CHECK(ks.constructed == ks.destroyed);
        VX_CHECK(vs.constructed == vs.destroyed);
        VX_CHECK(ks.bad == 0 && vs.bad == 0);
        VX_CHECK(st.live == 0);
    }
}
TEST_FAMILIES(error_out_of_memory_leaks_no_elements)

template <typename F>
void test_error_out_of_memory_reserve_and_shrink()
{
    using A = failing_allocator<int>;
    using M = failing_map<F, int, int>;

    fail_state st;
    {
        M m(test_hash<int>(), std::equal_to<int>(), A{ &st });
        fill(m, 0, 50);

        st.countdown = 0;
        VX_CHECK_ERROR(m.reserve(100000), vx::err::out_of_memory);
        VX_CHECK(eq_model(m, range(0, 50)));

        st.countdown = -1;
        for (int i = 3; i < 50; ++i)
        {
            VX_CHECK(m.erase(i) == 1);
        }

        st.countdown = 0;
        VX_CHECK_ERROR(m.shrink_to_fit(), vx::err::out_of_memory); // needs a new, smaller buffer
        VX_CHECK(eq_model(m, range(0, 3)));

        st.countdown = -1;
        VX_CHECK(m.shrink_to_fit());
        VX_CHECK(eq_model(m, range(0, 3)));
    }
    VX_CHECK(st.live == 0);
}
TEST_FAMILIES(error_out_of_memory_reserve_and_shrink)

template <typename F>
void test_error_out_of_memory_create()
{
    using A = failing_allocator<int>;
    using M = failing_map<F, int, int>;

    fail_state st;
    {
        st.countdown = 0;

        auto make_n = [&]()
        {
            return M::create(size_t(100), test_hash<int>(), std::equal_to<int>(), A{ &st });
        };
        VX_CHECK_EXPECTED_ERROR(make_n(), vx::err::out_of_memory);

        std::vector<typename M::value_type> src;
        for (int i = 0; i < 20; ++i)
        {
            src.push_back(make<M>(i, mv(i)));
        }
        auto make_range = [&]()
        {
            return M::create(src.begin(), src.end(), size_t(0), test_hash<int>(), std::equal_to<int>(), A{ &st });
        };
        VX_CHECK_EXPECTED_ERROR(make_range(), vx::err::out_of_memory);

        // copying fails without touching the original
        st.countdown = -1;
        M a(test_hash<int>(), std::equal_to<int>(), A{ &st });
        fill(a, 0, 40);

        st.countdown = 0;
        auto make_copy = [&]()
        {
            return M::create(a);
        };
        VX_CHECK_EXPECTED_ERROR(make_copy(), vx::err::out_of_memory);
        VX_CHECK(eq_model(a, range(0, 40)));
    }
    VX_CHECK(st.live == 0);
}
TEST_FAMILIES(error_out_of_memory_create)

// Fail the Nth allocation of a copy, for every N. node_map copies allocate one
// block per element, so a failure midway must free the partially built copy.
// flat_map needs a single allocation, so only budget 0 fails.
template <typename F>
void test_error_out_of_memory_copy_sweep()
{
    using A = failing_allocator<int>;
    using M = failing_map<F, int, int>;

    fail_state st;
    {
        M a(test_hash<int>(), std::equal_to<int>(), A{ &st });
        fill(a, 0, 40);
        const long baseline = st.live;

        bool saw_failure = false;
        bool saw_success = false;

        for (long budget = 0; budget < 64; ++budget)
        {
            st.countdown = budget;
            {
                auto c = M::create(a);
                if (c)
                {
                    saw_success = true;
                    VX_CHECK(c.value() == a);
                }
                else
                {
                    saw_failure = true;
                }
            }
            st.countdown = -1;

            // the copy (partial or complete) is gone, the original is untouched
            VX_CHECK(st.live == baseline);
            VX_CHECK(eq_model(a, range(0, 40)));
        }

        VX_CHECK(saw_failure);
        VX_CHECK(saw_success);

        // assign() has the same guarantee
        M b(test_hash<int>(), std::equal_to<int>(), A{ &st });
        fill(b, 100, 110);
        const long baseline2 = st.live;
        for (long budget = 0; budget < 64; ++budget)
        {
            st.countdown = budget;
            const auto r = b.assign(a);
            st.countdown = -1;

            if (!r)
            {
                VX_CHECK(eq_model(b, range(100, 110)));
                VX_CHECK(st.live == baseline2);
            }
            else
            {
                VX_CHECK(b == a);
                break;
            }
        }
    }
    VX_CHECK(st.live == 0);
}
TEST_FAMILIES(error_out_of_memory_copy_sweep)

template <typename F>
void test_error_out_of_memory_assign()
{
    using A = failing_allocator<int>;
    using M = failing_map<F, int, int>;

    fail_state st;
    {
        M a(test_hash<int>(), std::equal_to<int>(), A{ &st });
        M b(test_hash<int>(), std::equal_to<int>(), A{ &st });
        fill(a, 0, 40);
        fill(b, 100, 200);

        std::vector<typename M::value_type> src;
        for (int i = 500; i < 520; ++i)
        {
            src.push_back(make<M>(i, mv(i)));
        }

        st.countdown = 0;
        VX_CHECK_ERROR(a.assign(b), vx::err::out_of_memory);
        VX_CHECK_ERROR(a.assign(src.begin(), src.end()), vx::err::out_of_memory);

        // a failed assign changes neither side
        VX_CHECK(eq_model(a, range(0, 40)));
        VX_CHECK(eq_model(b, range(100, 200)));

        st.countdown = -1;
        VX_CHECK(a.assign(b));
        VX_CHECK(a == b);
    }
    VX_CHECK(st.live == 0);
}
TEST_FAMILIES(error_out_of_memory_assign)

template <typename F>
void test_error_out_of_memory_insert_range()
{
    using A = failing_allocator<int>;
    using M = failing_map<F, int, int>;

    fail_state st;
    {
        M m(test_hash<int>(), std::equal_to<int>(), A{ &st });

        std::vector<typename M::value_type> src;
        for (int i = 0; i < 200; ++i)
        {
            src.push_back(make<M>(i, mv(i)));
        }

        st.countdown = 0;
        VX_CHECK_ERROR(m.insert(src.begin(), src.end()), vx::err::out_of_memory);

        // Whatever was inserted before the failure must be a valid subset of the input
        // (right key, right value), and the table must still be internally consistent.
        for (const auto& kv : contents(m))
        {
            VX_CHECK(kv.first >= 0 && kv.first < 200);
            VX_CHECK(kv.second == mv(kv.first));
        }
        VX_CHECK(m.size() == static_cast<size_t>(std::distance(m.begin(), m.end())));

        st.countdown = -1;
        VX_CHECK(m.insert(src.begin(), src.end()));
        VX_CHECK(eq_model(m, range(0, 200)));
    }
    VX_CHECK(st.live == 0);
}
TEST_FAMILIES(error_out_of_memory_insert_range)

// merge reserves up front, so a failure moves nothing
template <typename F>
void test_error_out_of_memory_merge()
{
    using A = failing_allocator<int>;
    using M = failing_map<F, int, int>;

    fail_state st;
    {
        M a(test_hash<int>(), std::equal_to<int>(), A{ &st });
        M b(test_hash<int>(), std::equal_to<int>(), A{ &st });
        fill(a, 0, 40);
        fill(b, 100, 300);

        st.countdown = 0;
        VX_CHECK_ERROR(a.merge(b), vx::err::out_of_memory);

        VX_CHECK(eq_model(a, range(0, 40)));
        VX_CHECK(eq_model(b, range(100, 300)));

        st.countdown = -1;
        VX_CHECK(a.merge(b));
        VX_CHECK(b.empty());
        VX_CHECK(a.size() == 240);
    }
    VX_CHECK(st.live == 0);
}
TEST_FAMILIES(error_out_of_memory_merge)

//=========================================================================
// 14. randomized stress (compared against std::map)
//=========================================================================

template <typename M>
void test_stress()
{
    using K = key_of<M>;
    using T = mapped_of<M>;

    auto as_model = [](const std::map<int, int>& m)
    {
        return model(m.begin(), m.end());
    };

    for (unsigned seed = 1; seed <= 8; ++seed)
    {
        M s;
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
            const int v = static_cast<int>(rnd(1000));

            switch (rnd(10))
            {
                case 0:
                case 1:
                {
                    // insert: an existing key keeps its value
                    auto r = s.insert(make<M>(x, v));
                    VX_CHECK(r);
                    VX_CHECK(r.value().inserted == m.insert({ x, v }).second);
                    break;
                }
                case 2:
                {
                    // insert_or_assign: always ends up with the new value
                    auto r = s.insert_or_assign(K(x), T(v));
                    VX_CHECK(r);
                    const bool existed = m.count(x) == 1;
                    m[x] = v;
                    VX_CHECK(r.value().inserted == !existed);
                    break;
                }
                case 3:
                {
                    // try_emplace: an existing key keeps its value
                    auto r = s.try_emplace(K(x), T(v));
                    VX_CHECK(r);
                    VX_CHECK(r.value().inserted == m.emplace(x, v).second);
                    break;
                }
                case 4:
                case 5:
                {
                    VX_CHECK((s.erase(K(x)) == 1) == (m.erase(x) == 1));
                    break;
                }
                case 6:
                {
                    const T* p = s.get(K(x));
                    const auto it = m.find(x);
                    VX_CHECK((p != nullptr) == (it != m.end()));
                    if (p && it != m.end())
                    {
                        VX_CHECK(val(*p) == it->second);
                    }
                    VX_CHECK(s.contains(K(x)) == (it != m.end()));
                    break;
                }
                case 7:
                {
                    if constexpr (std::is_default_constructible_v<T>)
                    {
                        s[K(x)] = T(v);
                    }
                    else
                    {
                        VX_CHECK(s.insert_or_assign(K(x), T(v)));
                    }
                    m[x] = v;
                    break;
                }
                case 8:
                {
                    auto r = s.at(K(x));
                    VX_CHECK(static_cast<bool>(r) == (m.count(x) == 1));
                    break;
                }
                case 9:
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

            VX_CHECK(eq_model(s, as_model(m)));
        }
    }
}
TEST_MAPS(stress)

template <typename F>
void test_stress_degenerate_hash()
{
    test_stress<degenerate_map<F, int, int>>();
}
TEST_FAMILIES(stress_degenerate_hash)

template <typename F>
void test_stress_avalanching_hash()
{
    test_stress<avalanche_map<F, int, int>>();
}
TEST_FAMILIES(stress_avalanching_hash)

//=========================================================================
// 15. node_map only: reference and iterator stability
//
// node_map promises:
//   * references / pointers to elements AND to mapped values (the pointers
//     get() hands out) survive everything except erasing that element:
//     growth, reserve, shrink_to_fit, erase of others, move, swap
//   * iterators survive inserts that do not rehash, and erase of other elements
//   * rehash invalidates iterators but not references
//   * elements are never constructed, moved or copied by a rehash
// flat_map promises none of this, so these tests are registered for node only.
//=========================================================================

template <typename K>
void test_node_reference_stability_rehash()
{
    using M = plain_map<node_family, K, K>;
    using P = typename M::value_type;

    // Sections run one after another in the same call, so each one builds its
    // own table instead of sharing state with the others.

    // 100 elements, recording the address of each as it goes in
    auto seed = [](M& m, std::vector<const P*>& ptrs)
    {
        for (int i = 0; i < 100; ++i)
        {
            auto r = ins(m, i);
            VX_CHECK(r);
            ptrs.push_back(&*r.value().it);
        }
    };

    // both the element and the pointer get() returns must be unchanged
    auto check_ptrs = [](const M& m, const std::vector<const P*>& ptrs, int lo, int hi, int step)
    {
        for (int i = lo; i < hi; i += step)
        {
            const P* expected = ptrs[static_cast<size_t>(i)];

            auto it = m.find(K(i));
            VX_CHECK(it != m.end());
            VX_CHECK(&*it == expected);
            VX_CHECK(val(expected->key) == i);
            VX_CHECK(val(expected->value) == mv(i));
            VX_CHECK(m.get(K(i)) == &expected->value);
        }
    };

    VX_SECTION("growth")
    {
        M m;
        std::vector<const P*> ptrs;
        seed(m, ptrs);

        fill(m, 100, 5000);
        check_ptrs(m, ptrs, 0, 100, 1);
    }

    VX_SECTION("reserve")
    {
        M m;
        std::vector<const P*> ptrs;
        seed(m, ptrs);

        VX_CHECK(m.reserve(50000));
        check_ptrs(m, ptrs, 0, 100, 1);
    }

    VX_SECTION("shrink_to_fit after erasing most elements")
    {
        M m;
        std::vector<const P*> ptrs;
        seed(m, ptrs);

        fill(m, 100, 2000);
        for (int i = 100; i < 2000; ++i)
        {
            VX_CHECK(m.erase(K(i)) == 1);
        }
        VX_CHECK(m.shrink_to_fit());
        VX_CHECK(m.size() == 100);
        check_ptrs(m, ptrs, 0, 100, 1);
    }

    VX_SECTION("erasing other elements")
    {
        M m;
        std::vector<const P*> ptrs;
        seed(m, ptrs);

        // keep the odd ones, erase the even ones
        for (int i = 0; i < 100; i += 2)
        {
            VX_CHECK(m.erase(K(i)) == 1);
        }
        check_ptrs(m, ptrs, 1, 100, 2);
    }

    VX_SECTION("assigning an existing value does not move the element")
    {
        M m;
        std::vector<const P*> ptrs;
        seed(m, ptrs);

        for (int i = 0; i < 100; ++i)
        {
            VX_CHECK(!m.insert_or_assign(K(i), K(i + 5000)).value().inserted);
        }
        for (int i = 0; i < 100; ++i)
        {
            const P* expected = ptrs[static_cast<size_t>(i)];
            VX_CHECK(&*m.find(K(i)) == expected);
            VX_CHECK(val(expected->value) == i + 5000);
        }
    }

    VX_SECTION("insert / erase churn around a long-lived element")
    {
        M m;
        std::vector<const P*> ptrs;
        seed(m, ptrs);

        const P* anchor = ptrs[7];
        const int window = 8;
        for (int i = 1000; i < 21000; ++i)
        {
            VX_CHECK(ins(m, i).value().inserted);
            if (i >= 1000 + window)
            {
                VX_CHECK(m.erase(K(i - window)) == 1);
            }
        }
        VX_CHECK(&*m.find(K(7)) == anchor);
        VX_CHECK(m.get(K(7)) == &anchor->value);
        VX_CHECK(val(anchor->value) == mv(7));
    }
}
TEST_CHAIN_KEYS(node_reference_stability_rehash)

// the classic pattern: hold a reference from operator[] and keep inserting
template <typename K>
void test_node_subscript_reference_stability()
{
    using M = plain_map<node_family, K, K>;

    if constexpr (std::is_default_constructible_v<K>)
    {
        VX_SECTION("operator[] reference survives growth")
        {
            M m;
            K& r = m[K(-1)];
            r = K(123);

            fill(m, 0, 5000);
            VX_CHECK(&r == m.get(K(-1)));
            VX_CHECK(val(r) == 123);
        }

        VX_SECTION("get_or_insert pointer survives growth")
        {
            M m;
            auto g = m.get_or_insert(K(-1));
            VX_CHECK(g);
            K* p = g.value();
            *p = K(321);

            fill(m, 0, 5000);
            VX_CHECK(p == m.get(K(-1)));
            VX_CHECK(val(*p) == 321);
        }
    }
}
TEST_CHAIN_KEYS(node_subscript_reference_stability)

template <typename K>
void test_node_reference_stability_move_swap()
{
    using M = plain_map<node_family, K, K>;
    using P = typename M::value_type;
    using ptr_list = std::vector<const P*>;

    auto addresses = [](const M& m, int lo, int hi)
    {
        ptr_list out;
        for (int i = lo; i < hi; ++i)
        {
            auto it = m.find(K(i));
            out.push_back(it == m.end() ? nullptr : &*it);
        }
        return out;
    };

    // Each section builds its own source table: a section that moves from `a`
    // would otherwise leave a moved-from table for the next one.
    auto build = [&](M& a)
    {
        fill(a, 0, 50);
        ptr_list before = addresses(a, 0, 50);
        VX_CHECK(std::find(before.begin(), before.end(), nullptr) == before.end());
        return before;
    };

    VX_SECTION("move construct hands over the very same nodes")
    {
        M a;
        const ptr_list before = build(a);

        M b(std::move(a));
        VX_CHECK(addresses(b, 0, 50) == before);
    }

    VX_SECTION("move assign hands over the very same nodes")
    {
        M a;
        const ptr_list before = build(a);

        M b;
        fill(b, 500, 510);
        b = std::move(a);
        VX_CHECK(addresses(b, 0, 50) == before);
    }

    VX_SECTION("swap exchanges the nodes without moving them")
    {
        M a;
        const ptr_list before = build(a);

        M b;
        fill(b, 100, 110);
        const ptr_list b_before = addresses(b, 100, 110);

        a.swap(b);
        VX_CHECK(addresses(b, 0, 50) == before);
        VX_CHECK(addresses(a, 100, 110) == b_before);
    }

    VX_SECTION("moved-into table keeps stable addresses while it grows")
    {
        M a;
        const ptr_list before = build(a);

        M b(std::move(a));
        fill(b, 1000, 6000);
        VX_CHECK(addresses(b, 0, 50) == before);
    }
}
TEST_CHAIN_KEYS(node_reference_stability_move_swap)

template <typename K>
void test_node_iterator_stability()
{
    using M = plain_map<node_family, K, K>;
    using It = typename M::iterator;

    VX_SECTION("iterators survive inserts that do not rehash")
    {
        M m;
        VX_CHECK(m.reserve(1000));
        const size_t cap = m.capacity();
        fill(m, 0, 10);

        std::vector<It> its;
        for (int i = 0; i < 10; ++i)
        {
            its.push_back(m.find(K(i)));
        }

        // stay within capacity: no rehash
        fill(m, 10, static_cast<int>(cap));
        VX_CHECK(m.capacity() == cap);

        for (int i = 0; i < 10; ++i)
        {
            VX_CHECK(its[static_cast<size_t>(i)] == m.find(K(i)));
            VX_CHECK(val(its[static_cast<size_t>(i)]->key) == i);
            VX_CHECK(val(its[static_cast<size_t>(i)]->value) == mv(i));
        }
    }

    VX_SECTION("iterators survive erase of other elements")
    {
        M m;
        fill(m, 0, 100);

        auto it = m.find(K(50));
        VX_CHECK(it != m.end());

        // erase every odd key; 50 is even so it stays
        for (int i = 1; i < 100; i += 2)
        {
            VX_CHECK(m.erase(K(i)) == 1);
        }

        VX_CHECK(it == m.find(K(50)));
        VX_CHECK(val(it->key) == 50);
        VX_CHECK(val(it->value) == mv(50));

        // walking on from the held iterator only visits survivors, and ends
        size_t visited = 0;
        for (; it != m.end(); ++it)
        {
            VX_CHECK(val(it->key) % 2 == 0);
            ++visited;
            VX_CHECK(visited <= 50);
        }
        VX_CHECK(visited >= 1);
    }

    VX_SECTION("a value assigned through a held iterator is seen by the map")
    {
        M m;
        fill(m, 0, 100);

        auto it = m.find(K(10));
        for (int i = 20; i < 80; ++i)
        {
            VX_CHECK(m.erase(K(i)) == 1);
        }
        it->value = K(4242);
        VX_CHECK(val(*m.get(K(10))) == 4242);
    }

    VX_SECTION("erase through a held iterator after other erases")
    {
        M m;
        fill(m, 0, 100);

        auto keep = m.find(K(10));
        auto victim = m.find(K(90));

        for (int i = 20; i < 80; ++i)
        {
            VX_CHECK(m.erase(K(i)) == 1);
        }

        VX_CHECK(val(keep->key) == 10);
        (void)m.erase(victim); // erase_at has to relink using the bucket it remembered
        VX_CHECK(!m.contains(K(90)));
        VX_CHECK(val(keep->key) == 10);
        VX_CHECK(m.size() == 100 - 60 - 1);
    }

    VX_SECTION("erase of an element in a shared node keeps neighbours valid")
    {
        // every key collides, so all elements share one bucket node
        using D = degenerate_map<node_family, K, K>;
        using DIt = typename D::iterator;

        D m;
        fill(m, 0, 50);

        std::vector<DIt> its;
        for (int i = 0; i < 50; ++i)
        {
            its.push_back(m.find(K(i)));
        }

        for (int i = 0; i < 50; i += 2)
        {
            (void)m.erase(its[static_cast<size_t>(i)]);
        }

        VX_CHECK(m.size() == 25);
        for (int i = 1; i < 50; i += 2)
        {
            VX_CHECK(its[static_cast<size_t>(i)] == m.find(K(i)));
            VX_CHECK(val(its[static_cast<size_t>(i)]->key) == i);
            VX_CHECK(val(its[static_cast<size_t>(i)]->value) == mv(i));
        }
        for (int i = 0; i < 50; i += 2)
        {
            VX_CHECK(!m.contains(K(i)));
        }
    }

    VX_SECTION("erase returns an iterator that stays valid for the rest of the walk")
    {
        M m;
        fill(m, 0, 500);

        // erase every third element, using the returned iterator to continue
        size_t erased = 0;
        for (auto it = m.begin(); it != m.end();)
        {
            if (val(it->key) % 3 == 0)
            {
                it = m.erase(it);
                ++erased;
            }
            else
            {
                ++it;
            }
        }

        VX_CHECK(erased == 167);
        VX_CHECK(m.size() == 500 - erased);
        for (int i = 0; i < 500; ++i)
        {
            VX_CHECK(m.contains(K(i)) == (i % 3 != 0));
        }
    }
}
TEST_CHAIN_KEYS(node_iterator_stability)

// A rehash only relinks nodes: no key or value is constructed, moved, copied
// or destroyed by reserve / growth / shrink_to_fit.
VX_TEST_CASE(node_rehash_never_touches_elements)
{
    using CM = counted_map<node_family>;

    auto& ks = counted_key::stats();
    auto& vs = counted_val::stats();
    reset_counted();

    // Sections run one after another in the same call, so each builds its own map.
    auto same = [&](const counted_stats& kb, const counted_stats& vb)
    {
        return ks.constructed == kb.constructed && ks.destroyed == kb.destroyed && ks.moves == kb.moves && ks.copies == kb.copies && vs.constructed == vb.constructed && vs.destroyed == vb.destroyed && vs.moves == vb.moves && vs.copies == vb.copies;
    };

    VX_SECTION("reserve")
    {
        CM m;
        fill(m, 0, 500);

        const counted_stats kb = ks;
        const counted_stats vb = vs;
        VX_CHECK(m.reserve(20000));
        VX_CHECK(same(kb, vb));
    }

    VX_SECTION("growth through insert")
    {
        CM m;
        fill(m, 0, 500);

        // only the new elements are constructed; the existing 500 are left alone
        const counted_stats kb = ks;
        const counted_stats vb = vs;
        fill(m, 500, 3000);
        VX_CHECK(ks.copies == kb.copies && vs.copies == vb.copies);
        VX_CHECK(m.size() == 3000);
        VX_CHECK(ks.live == m.size() && vs.live == m.size());
    }

    VX_SECTION("shrink_to_fit")
    {
        CM m;
        fill(m, 0, 500);

        for (auto it = m.begin(); it != m.end();)
        {
            if (val(it->key) >= 3)
            {
                it = m.erase(it);
            }
            else
            {
                ++it;
            }
        }
        VX_CHECK(m.size() == 3);

        const counted_stats kb = ks;
        const counted_stats vb = vs;
        VX_CHECK(m.shrink_to_fit());
        VX_CHECK(same(kb, vb));
    }

    VX_CHECK(ks.live == 0 && vs.live == 0);
    VX_CHECK(ks.constructed == ks.destroyed);
    VX_CHECK(vs.constructed == vs.destroyed);
    VX_CHECK(ks.bad == 0 && vs.bad == 0);
}

//=========================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
