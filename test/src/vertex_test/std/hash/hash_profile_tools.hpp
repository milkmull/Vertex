#pragma once

#include <cstdint>
#include <iostream>
#include <iterator>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "vertex/os/compiler.hpp"
#include "vertex/std/hash_map.hpp" // assumed path: vx::node_map / flat_map
#include "vertex/std/hash_set.hpp" // assumed path: vx::node_set / flat_set
#include "vertex/util/random.hpp"
#define VX_ENABLE_PROFILING
#include "vertex/system/profiler.hpp"

//=========================================================================
// types (default options everywhere)
//=========================================================================

using uh_key = uint64_t;

using uset_std = std::unordered_set<uh_key>;
using uset_node = vx::node_set<uh_key>; // == vx::hash_set<uh_key>
using uset_flat = vx::flat_set<uh_key>;

using umap_std = std::unordered_map<uh_key, uh_key>;
using umap_node = vx::node_map<uh_key, uh_key>; // == vx::hash_map<uh_key, uh_key>
using umap_flat = vx::flat_map<uh_key, uh_key>;

static constexpr size_t UR = 2000; // number of repetitions
static constexpr size_t UN = 50;   // number of elements (>= 8)

//=========================================================================
// helpers
//=========================================================================

namespace uprof {

// splitmix64: a bijection, so distinct i always give distinct keys
inline uh_key make_key(size_t i) noexcept
{
    uint64_t x = static_cast<uint64_t>(i) + 0x9e3779b97f4a7c15ull;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ull;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebull;
    return x ^ (x >> 31);
}

// key indices at or above this are never inserted, so lookups on them miss
static constexpr size_t miss_offset = 1000000;

template <typename...>
struct make_void
{
    using type = void;
};

template <typename C, typename = void>
struct is_map : std::false_type
{};

template <typename C>
struct is_map<C, typename make_void<typename C::mapped_type>::type> : std::true_type
{};

// vx containers expose hasher_type (std ones expose hasher), and return
// expected<>/success from fallible operations instead of void/pair
template <typename C, typename = void>
struct is_vx : std::false_type
{};

template <typename C>
struct is_vx<C, typename make_void<typename C::hasher_type>::type> : std::true_type
{};

// element: key for sets, pair<key, key> for maps
template <typename C, bool = is_map<C>::value>
struct elem_of
{
    using type = uh_key;
    static type make(size_t i)
    { return make_key(i); }
};

template <typename C>
struct elem_of<C, true>
{
    using type = std::pair<uh_key, uh_key>;
    static type make(size_t i)
    { return type(make_key(i), make_key(i + 17)); }
};

inline uh_key key_of(uh_key k) noexcept
{
    return k;
}

inline uh_key key_of(const std::pair<uh_key, uh_key>& p) noexcept
{
    return p.first;
}

template <typename T>
inline void sink(T& v)
{
    vx::os::do_not_optimize(v);
}

// vx fallible results (expected / success) are assumed to succeed, the same way
// vx::vector does it internally: VX_VERIFY(ok). std results are just consumed.
template <typename R>
inline void consume(R& r, std::true_type)
{
    VX_VERIFY(r);
    sink(r);
}

template <typename R>
inline void consume(R& r, std::false_type)
{
    sink(r);
}

template <typename C, typename R>
inline void consume(R& r)
{
    consume(r, is_vx<C>{});
}

template <typename C>
std::vector<typename elem_of<C>::type> make_elems(size_t count, size_t offset = 0)
{
    std::vector<typename elem_of<C>::type> out;
    out.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        out.push_back(elem_of<C>::make(offset + i));
    }
    return out;
}

inline std::vector<uh_key> make_keys(size_t count, size_t offset = 0)
{
    std::vector<uh_key> out;
    out.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        out.push_back(make_key(offset + i));
    }
    return out;
}

//-------------------------------------------------------------------------
// uniform adapters over std (void / pair returns) and vx (expected / success)
//-------------------------------------------------------------------------

template <typename C, typename E>
inline void insert_one(C& c, const E& e)
{
    auto r = c.insert(e);
    consume<C>(r);
}

template <typename C, typename E>
inline void emplace_one(C& c, const E& e)
{
    auto r = c.emplace(e);
    consume<C>(r);
}

template <typename C, typename K>
inline void erase_key(C& c, const K& k)
{
    auto r = c.erase(k);
    sink(r);
}

template <typename C, typename K>
inline bool contains_key(const C& c, const K& k, std::true_type)
{
    return c.contains(k);
}

template <typename C, typename K>
inline bool contains_key(const C& c, const K& k, std::false_type)
{
    return c.find(k) != c.end(); // what C++20 contains() does
}

template <typename C, typename K>
inline bool contains_key(const C& c, const K& k)
{
    return contains_key(c, k, is_vx<C>{});
}

template <typename C>
inline void reserve(C& c, size_t n, std::true_type)
{
    auto r = c.reserve(n);
    consume<C>(r);
}

template <typename C>
inline void reserve(C& c, size_t n, std::false_type)
{
    c.reserve(n);
}

template <typename C>
inline void reserve(C& c, size_t n)
{
    reserve(c, n, is_vx<C>{});
}

template <typename C, typename IT>
inline void insert_range(C& c, IT first, IT last, std::true_type)
{
    auto r = c.insert(first, last);
    consume<C>(r);
}

template <typename C, typename IT>
inline void insert_range(C& c, IT first, IT last, std::false_type)
{
    c.insert(first, last);
}

template <typename C, typename IT>
inline void insert_range(C& c, IT first, IT last)
{
    insert_range(c, first, last, is_vx<C>{});
}

template <typename C>
inline void shrink(C& c, std::true_type)
{
    auto r = c.shrink_to_fit();
    consume<C>(r);
}

template <typename C>
inline void shrink(C& c, std::false_type)
{
    c.rehash(0); // std has no shrink_to_fit; this is the closest equivalent
}

template <typename C>
inline void shrink(C& c)
{
    shrink(c, is_vx<C>{});
}

template <typename C>
inline void merge(C& dst, C& src, std::true_type)
{
    auto r = dst.merge(src);
    consume<C>(r);
}

template <typename C>
inline void merge(C& dst, C& src, std::false_type)
{
    dst.merge(src);
}

template <typename C>
inline void merge(C& dst, C& src)
{
    merge(dst, src, is_vx<C>{});
}

template <typename C>
C make_filled(size_t count, size_t offset = 0)
{
    C c;
    reserve(c, count);
    for (const auto& e : make_elems<C>(count, offset))
    {
        insert_one(c, e);
    }
    return c;
}

// names for the timer output
template <typename C>
struct label;

template <>
struct label<uset_std>
{
    static const char* get()
    { return "uset std "; }
};
template <>
struct label<uset_node>
{
    static const char* get()
    { return "uset node"; }
};
template <>
struct label<uset_flat>
{
    static const char* get()
    { return "uset flat"; }
};
template <>
struct label<umap_std>
{
    static const char* get()
    { return "umap std "; }
};
template <>
struct label<umap_node>
{
    static const char* get()
    { return "umap node"; }
};
template <>
struct label<umap_flat>
{
    static const char* get()
    { return "umap flat"; }
};

} // namespace uprof

#define uh_start_timer(str) \
    ::vx::profile::_priv::profile_timer timer(std::string(str) + " (" + ::uprof::label<C>::get() + ")")
#define uh_stop_timer() timer.stop()

//=========================================================================
// constructors
//=========================================================================

template <typename C>
VX_NO_INLINE void profile_default_constructor(size_t)
{
    uh_start_timer("default construct");
    C c;
    uh_stop_timer();
}

template <typename C>
VX_NO_INLINE void profile_size_constructor(size_t N)
{
    uh_start_timer("size construct");
    C c(N);
    uh_stop_timer();
}

template <typename C>
VX_NO_INLINE void profile_range_constructor(size_t N)
{
    auto elems = uprof::make_elems<C>(N);

    uh_start_timer("range construct");
    C c(elems.begin(), elems.end());
    uh_stop_timer();
}

template <typename C>
VX_NO_INLINE void profile_list_constructor(size_t)
{
    auto e = uprof::make_elems<C>(4);

    uh_start_timer("list construct");
    C c{ e[0], e[1], e[2], e[3] };
    uh_stop_timer();
}

template <typename C>
VX_NO_INLINE void profile_copy_constructor(size_t N)
{
    C c1 = uprof::make_filled<C>(N);

    uh_start_timer("copy construct");
    C c2(c1);
    uh_stop_timer();
}

template <typename C>
VX_NO_INLINE void profile_move_constructor(size_t N)
{
    C src = uprof::make_filled<C>(N);

    uh_start_timer("move construct");
    C dst = std::move(src);
    uh_stop_timer();
}

//=========================================================================
// destructor
//=========================================================================

template <typename C>
VX_NO_INLINE void profile_destructor(size_t N)
{
    auto elems = uprof::make_elems<C>(N);

    C* c = vx::mem::default_allocator<C>::allocate(1);
    vx::mem::construct_in_place(c);
    for (const auto& e : elems)
    {
        uprof::insert_one(*c, e);
    }

    uh_start_timer("destruct");
    vx::mem::destroy_in_place(c);
    uh_stop_timer();

    vx::mem::default_allocator<C>::deallocate(c, 1);
}

//=========================================================================
// assignment
//=========================================================================

template <typename C>
void profile_copy_assignment_grow(size_t N)
{
    C src = uprof::make_filled<C>(N);
    C dst = uprof::make_filled<C>(N / 2, N); // disjoint keys

    vx::os::do_not_optimize(src);
    vx::os::do_not_optimize(dst);

    uh_start_timer("copy assign grow");
    dst = src;
    uh_stop_timer();
}

template <typename C>
void profile_copy_assignment_shrink(size_t N)
{
    C src = uprof::make_filled<C>(N / 2);
    C dst = uprof::make_filled<C>(N, N); // disjoint keys

    vx::os::do_not_optimize(src);
    vx::os::do_not_optimize(dst);

    uh_start_timer("copy assign shrink");
    dst = src;
    uh_stop_timer();
}

template <typename C>
void profile_list_assignment(size_t N)
{
    C dst = uprof::make_filled<C>(N);
    auto e = uprof::make_elems<C>(5, N * 2);

    uh_start_timer("list assign");
    dst = { e[0], e[1], e[2], e[3], e[4] };
    uh_stop_timer();
}

template <typename C>
void profile_move_assignment(size_t N)
{
    C src = uprof::make_filled<C>(N);
    C dst = uprof::make_filled<C>(N / 2, N);

    uh_start_timer("move assign");
    dst = std::move(src);
    uh_stop_timer();
}

//=========================================================================
// reserve / shrink / clear / swap
//=========================================================================

template <typename C>
void profile_reserve_grow(size_t N)
{
    C c = uprof::make_filled<C>(N / 2);

    uh_start_timer("reserve grow");
    uprof::reserve(c, N * 4);
    uh_stop_timer();
}

template <typename C>
void profile_reserve_noop(size_t N)
{
    C c = uprof::make_filled<C>(N);

    uh_start_timer("reserve no-op");
    uprof::reserve(c, N / 2);
    uh_stop_timer();
}

template <typename C>
void profile_shrink_to_fit(size_t N)
{
    C c = uprof::make_filled<C>(N / 2);
    uprof::reserve(c, N * 8);

    uh_start_timer("shrink to fit");
    uprof::shrink(c);
    uh_stop_timer();
}

template <typename C>
void profile_clear(size_t N)
{
    C c = uprof::make_filled<C>(N);

    uh_start_timer("clear");
    c.clear();
    uh_stop_timer();
}

template <typename C>
void profile_swap(size_t N)
{
    C c1 = uprof::make_filled<C>(N);
    C c2 = uprof::make_filled<C>(N / 2, N);

    uh_start_timer("swap");
    c1.swap(c2);
    uh_stop_timer();
}

//=========================================================================
// insert / emplace
//=========================================================================

template <typename C>
void profile_insert_grow(size_t N)
{
    auto elems = uprof::make_elems<C>(N);
    C c;

    uh_start_timer("insert grow");
    for (size_t i = 0; i < N; ++i)
    {
        uprof::insert_one(c, elems[i]);
    }
    uh_stop_timer();
}

template <typename C>
void profile_insert_reserved(size_t N)
{
    auto elems = uprof::make_elems<C>(N);
    C c;
    uprof::reserve(c, N);

    uh_start_timer("insert reserved");
    for (size_t i = 0; i < N; ++i)
    {
        uprof::insert_one(c, elems[i]);
    }
    uh_stop_timer();
}

template <typename C>
void profile_insert_duplicate(size_t N)
{
    auto elems = uprof::make_elems<C>(N);
    C c = uprof::make_filled<C>(N);

    uh_start_timer("insert duplicate");
    for (size_t i = 0; i < N; ++i)
    {
        uprof::insert_one(c, elems[i]);
    }
    uh_stop_timer();
}

template <typename C>
void profile_insert_range(size_t N)
{
    auto elems = uprof::make_elems<C>(N);
    C c = uprof::make_filled<C>(N / 2); // half of the range already present

    uh_start_timer("insert range");
    uprof::insert_range(c, elems.begin(), elems.end());
    uh_stop_timer();
}

template <typename C>
void profile_emplace_grow(size_t N)
{
    auto elems = uprof::make_elems<C>(N);
    C c;

    uh_start_timer("emplace grow");
    for (size_t i = 0; i < N; ++i)
    {
        uprof::emplace_one(c, elems[i]);
    }
    uh_stop_timer();
}

template <typename C>
void profile_emplace_reserved(size_t N)
{
    auto elems = uprof::make_elems<C>(N);
    C c;
    uprof::reserve(c, N);

    uh_start_timer("emplace reserved");
    for (size_t i = 0; i < N; ++i)
    {
        uprof::emplace_one(c, elems[i]);
    }
    uh_stop_timer();
}

template <typename C>
void profile_merge(size_t N)
{
    C dst = uprof::make_filled<C>(N / 2);
    C src = uprof::make_filled<C>(N, N / 4); // partial overlap with dst

    uh_start_timer("merge");
    uprof::merge(dst, src);
    uh_stop_timer();
}

//=========================================================================
// lookup
//=========================================================================

template <typename C>
void profile_find_hit(size_t N)
{
    C c = uprof::make_filled<C>(N);
    auto keys = uprof::make_keys(N);

    uh_start_timer("find hit");
    for (size_t i = 0; i < N; ++i)
    {
        auto it = c.find(keys[i]);
        uprof::sink(it);
    }
    uh_stop_timer();
}

template <typename C>
void profile_find_miss(size_t N)
{
    C c = uprof::make_filled<C>(N);
    auto keys = uprof::make_keys(N, uprof::miss_offset);

    uh_start_timer("find miss");
    for (size_t i = 0; i < N; ++i)
    {
        auto it = c.find(keys[i]);
        uprof::sink(it);
    }
    uh_stop_timer();
}

template <typename C>
void profile_contains_hit(size_t N)
{
    C c = uprof::make_filled<C>(N);
    auto keys = uprof::make_keys(N);

    constexpr size_t INNER = 20000; // 20000 * 50 = 1M lookups per section
    size_t hits = 0;

    // warm-up
    for (size_t i = 0; i < N; ++i)
        hits += uprof::contains_key(c, keys[i]);

    uh_start_timer("contains hit");
    for (size_t r = 0; r < INNER; ++r)
        for (size_t i = 0; i < N; ++i)
            hits += uprof::contains_key(c, keys[i]);
    uh_stop_timer();

    VX_ASSERT(hits == N * (INNER + 1));
    static volatile size_t g_sink;
    g_sink = hits;
    uprof::sink(hits);
}

template <typename C>
void profile_contains_miss(size_t N)
{
    C c = uprof::make_filled<C>(N);         // elements 0..N-1
    auto keys = uprof::make_elems<C>(N, N); // elements N..2N-1, all absent

    vx::random::gen rng;
    vx::random::shuffle(keys.begin(), keys.end(), rng);

    constexpr size_t INNER = 20000;
    size_t hits = 0;

    for (size_t i = 0; i < N; ++i) // warm-up
        hits += uprof::contains_key(c, keys[i]);

    uh_start_timer("contains miss");
    for (size_t r = 0; r < INNER; ++r)
        for (size_t i = 0; i < N; ++i)
            hits += uprof::contains_key(c, keys[i]);
    uh_stop_timer();

    VX_ASSERT(hits == 0);
    static volatile size_t g_sink;
    g_sink = hits;
    uprof::sink(hits);
}

template <typename C>
void profile_count_hit(size_t N)
{
    C c = uprof::make_filled<C>(N);
    auto keys = uprof::make_keys(N);

    uh_start_timer("count hit");
    for (size_t i = 0; i < N; ++i)
    {
        auto n = c.count(keys[i]);
        uprof::sink(n);
    }
    uh_stop_timer();
}

template <typename C>
void profile_count_miss(size_t N)
{
    C c = uprof::make_filled<C>(N);
    auto keys = uprof::make_keys(N, uprof::miss_offset);

    uh_start_timer("count miss");
    for (size_t i = 0; i < N; ++i)
    {
        auto n = c.count(keys[i]);
        uprof::sink(n);
    }
    uh_stop_timer();
}

//=========================================================================
// erase
//=========================================================================

template <typename C>
void profile_erase_key_hit(size_t N)
{
    C c = uprof::make_filled<C>(N);
    auto keys = uprof::make_keys(N);

    uh_start_timer("erase key hit");
    for (size_t i = 0; i < N; ++i)
    {
        uprof::erase_key(c, keys[i]);
    }
    uh_stop_timer();
}

template <typename C>
void profile_erase_key_miss(size_t N)
{
    C c = uprof::make_filled<C>(N);
    auto keys = uprof::make_keys(N, uprof::miss_offset);

    uh_start_timer("erase key miss");
    for (size_t i = 0; i < N; ++i)
    {
        uprof::erase_key(c, keys[i]);
    }
    uh_stop_timer();
}

template <typename C>
void profile_erase_iterator(size_t N)
{
    C c = uprof::make_filled<C>(N);

    uh_start_timer("erase iterator");
    for (size_t i = 0; i < N; ++i)
    {
        auto it = c.erase(c.begin());
        uprof::sink(it);
    }
    uh_stop_timer();
}

//=========================================================================
// iterate / compare
//=========================================================================

template <typename C>
void profile_iterate(size_t N)
{
    C c = uprof::make_filled<C>(N);

    uh_start_timer("iterate");
    uint64_t sum = 0;
    for (const auto& e : c)
    {
        sum += uprof::key_of(e);
    }
    uprof::sink(sum);
    uh_stop_timer();
}

template <typename C>
void profile_compare_equal(size_t N)
{
    C c1 = uprof::make_filled<C>(N);
    C c2 = c1;
    bool equal = false;

    uh_start_timer("compare equal");
    equal = (c1 == c2);
    uprof::sink(equal);
    uh_stop_timer();
}

template <typename C>
void profile_compare_not_equal(size_t N)
{
    C c1 = uprof::make_filled<C>(N);
    C c2 = uprof::make_filled<C>(N, 1); // same size, one key differs at each end
    bool equal = false;

    uh_start_timer("compare not equal");
    equal = (c1 == c2);
    uprof::sink(equal);
    uh_stop_timer();
}

//=========================================================================
// vx only: fallible construction, assume_* fast paths, deallocating clear
//=========================================================================

template <typename C>
VX_NO_INLINE void profile_vx_create_copy(size_t N)
{
    C c1 = uprof::make_filled<C>(N);

    uh_start_timer("create copy");
    auto r = C::create(c1);
    uprof::consume<C>(r);
    uh_stop_timer();
}

template <typename C>
VX_NO_INLINE void profile_vx_create_range(size_t N)
{
    auto elems = uprof::make_elems<C>(N);

    uh_start_timer("create range");
    auto r = C::create(elems.begin(), elems.end());
    uprof::consume<C>(r);
    uh_stop_timer();
}

template <typename C>
void profile_vx_insert_assume_unique(size_t N)
{
    auto elems = uprof::make_elems<C>(N);
    C c;

    uh_start_timer("insert assume unique");
    for (size_t i = 0; i < N; ++i)
    {
        auto r = c.insert_assume_unique(elems[i]);
        uprof::consume<C>(r);
    }
    uh_stop_timer();
}

template <typename C>
void profile_vx_insert_assume_capacity(size_t N)
{
    auto elems = uprof::make_elems<C>(N);
    C c;
    uprof::reserve(c, N);

    uh_start_timer("insert assume capacity");
    for (size_t i = 0; i < N; ++i)
    {
        auto r = c.insert_assume_capacity(elems[i]);
        uprof::sink(r);
    }
    uh_stop_timer();
}

template <typename C>
void profile_vx_insert_assume_unique_and_capacity(size_t N)
{
    auto elems = uprof::make_elems<C>(N);
    C c;
    uprof::reserve(c, N);

    uh_start_timer("insert assume unique+cap");
    for (size_t i = 0; i < N; ++i)
    {
        auto it = c.insert_assume_unique_and_capacity(elems[i]);
        uprof::sink(it);
    }
    uh_stop_timer();
}

template <typename C>
void profile_vx_clear_and_deallocate(size_t N)
{
    C c = uprof::make_filled<C>(N);

    uh_start_timer("clear and deallocate");
    c.clear_and_deallocate();
    uh_stop_timer();
}

//=========================================================================
// test collection
//=========================================================================

using uh_test_fn = void (*)(size_t);

template <typename C>
void collect_vx_tests(std::vector<uh_test_fn>& out, std::true_type)
{
    out.push_back(profile_vx_create_copy<C>);
    out.push_back(profile_vx_create_range<C>);
    out.push_back(profile_vx_insert_assume_unique<C>);
    out.push_back(profile_vx_insert_assume_capacity<C>);
    out.push_back(profile_vx_insert_assume_unique_and_capacity<C>);
    out.push_back(profile_vx_clear_and_deallocate<C>);
}

template <typename C>
void collect_vx_tests(std::vector<uh_test_fn>&, std::false_type)
{}

template <typename C>
void collect_tests(std::vector<uh_test_fn>& out)
{
    // constructors / destructor
    //out.push_back(profile_default_constructor<C>);
    //out.push_back(profile_size_constructor<C>);
    //out.push_back(profile_range_constructor<C>);
    //out.push_back(profile_list_constructor<C>);
    //out.push_back(profile_copy_constructor<C>);
    //out.push_back(profile_move_constructor<C>);
    //out.push_back(profile_destructor<C>);
    //
    //// assignment
    //out.push_back(profile_copy_assignment_grow<C>);
    //out.push_back(profile_copy_assignment_shrink<C>);
    //out.push_back(profile_list_assignment<C>);
    //out.push_back(profile_move_assignment<C>);
    //
    //// reserve / shrink / clear / swap
    //out.push_back(profile_reserve_grow<C>);
    //out.push_back(profile_reserve_noop<C>);
    //out.push_back(profile_shrink_to_fit<C>);
    //out.push_back(profile_clear<C>);
    //out.push_back(profile_swap<C>);
    //
    //// insert / emplace / merge
    //out.push_back(profile_insert_grow<C>);
    //out.push_back(profile_insert_reserved<C>);
    //out.push_back(profile_insert_duplicate<C>);
    //out.push_back(profile_insert_range<C>);
    //out.push_back(profile_emplace_grow<C>);
    //out.push_back(profile_emplace_reserved<C>);
    //out.push_back(profile_merge<C>);
    //
    //// lookup
    ////out.push_back(profile_find_hit<C>);
    ////out.push_back(profile_find_miss<C>);
    out.push_back(profile_contains_hit<C>);
    out.push_back(profile_contains_miss<C>);
    //out.push_back(profile_count_hit<C>);
    //out.push_back(profile_count_miss<C>);

    // erase
    //out.push_back(profile_erase_key_hit<C>);
    //out.push_back(profile_erase_key_miss<C>);
    //out.push_back(profile_erase_iterator<C>);
    //
    //// iterate / compare
    //out.push_back(profile_iterate<C>);
    //out.push_back(profile_compare_equal<C>);
    //out.push_back(profile_compare_not_equal<C>);

    // vx-only extras (no-op for std containers)
    //collect_vx_tests<C>(out, typename uprof::is_vx<C>::type{});
}

//=========================================================================
// driver
//=========================================================================

// Runs the tests for one or more container types, interleaved in random order
// each repetition so every container sees the same cache/allocator conditions.
//
//   run<uset_std, uset_node, uset_flat>(UN, UR);
//   run<umap_std, umap_node, umap_flat>(UN, UR);
template <typename... Cs>
void run(size_t N, size_t R)
{
    std::vector<uh_test_fn> tests;
    int expand[] = { 0, (collect_tests<Cs>(tests), 0)... };
    (void)expand;

    std::vector<uh_test_fn> selected(tests.size());
    vx::random::gen rng;

    for (size_t r = 0; r < R; ++r)
    {
        vx::random::sample(tests.begin(), tests.end(), selected.data(), tests.size(), rng);

        for (auto test : selected)
        {
            test(N);
        }
    }
}
