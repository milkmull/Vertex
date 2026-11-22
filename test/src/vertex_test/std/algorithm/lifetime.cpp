// Tests for vx::range::construct / construct_maybe_trivial / destroy / destroy_n
//
// Assumption: the header is "vertex/std/algorithm/construct.hpp" -- adjust to
// wherever these functions actually live.

#include "vertex/std/algorithm/lifetime.hpp"

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <list>
#include <new>
#include <vector>

#include "vertex_test/std/algorithm/algorithm_tools.hpp"
#include "vertex_test/test.hpp"

namespace vx {
namespace test {

namespace {

//=============================================================================
// local helpers (candidates for algorithm_tools.hpp if other sections need them)
//=============================================================================

// Non-trivial type that records every construction and destruction.
struct tracked
{
    static inline int constructed = 0;
    static inline int destroyed = 0;
    static inline int live = 0;

    static void reset()
    {
        constructed = 0;
        destroyed = 0;
        live = 0;
    }

    int val;

    tracked()
        : val(7)
    {
        ++constructed;
        ++live;
    }

    tracked(const tracked&) = delete;
    tracked& operator=(const tracked&) = delete;

    ~tracked()
    {
        ++destroyed;
        --live;
    }
};

// Uninitialized, correctly aligned storage for N objects of T, pre-filled with
// a sentinel byte so tests can tell which bytes a function touched.
VX_DISABLE_MSVC_WARNING_PUSH()
VX_DISABLE_MSVC_WARNING(4324) // padding added from explicit alignment

template <typename T, std::size_t N>
struct raw_buffer
{
    static constexpr unsigned char fill = 0xCD;

    alignas(T) unsigned char bytes[N * sizeof(T)];

    raw_buffer()
    {
        std::memset(bytes, fill, sizeof(bytes));
    }

    T* data()
    {
        return reinterpret_cast<T*>(bytes);
    }

    // True if every byte of elements [first, last) still holds the sentinel.
    bool is_untouched(std::size_t first, std::size_t last) const
    {
        for (std::size_t i = first * sizeof(T); i < last * sizeof(T); ++i)
        {
            if (bytes[i] != fill)
            {
                return false;
            }
        }
        return true;
    }
};

VX_DISABLE_MSVC_WARNING_POP()

// Minimal iterator over raw memory, so the generic (non-pointer) overloads can
// be exercised on storage that holds no live objects. The category is a
// parameter so both the random-access and the step-by-step paths are covered.
template <typename T, typename Category>
class raw_iterator
{
public:

    using iterator_category = Category;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    raw_iterator() = default;

    explicit raw_iterator(T* p)
        : m_ptr(p)
    {}

    reference operator*() const
    {
        return *m_ptr;
    }

    pointer operator->() const
    {
        return m_ptr;
    }

    raw_iterator& operator++()
    {
        ++m_ptr;
        return *this;
    }

    raw_iterator operator++(int)
    {
        raw_iterator tmp = *this;
        ++m_ptr;
        return tmp;
    }

    raw_iterator& operator--()
    {
        --m_ptr;
        return *this;
    }

    raw_iterator operator+(difference_type n) const
    {
        return raw_iterator(m_ptr + n);
    }

    difference_type operator-(const raw_iterator& o) const
    {
        return m_ptr - o.m_ptr;
    }

    bool operator==(const raw_iterator& o) const
    { return m_ptr == o.m_ptr; }
    bool operator!=(const raw_iterator& o) const
    { return m_ptr != o.m_ptr; }
    bool operator<(const raw_iterator& o) const
    { return m_ptr < o.m_ptr; }
    bool operator<=(const raw_iterator& o) const
    { return m_ptr <= o.m_ptr; }
    bool operator>(const raw_iterator& o) const
    { return m_ptr > o.m_ptr; }
    bool operator>=(const raw_iterator& o) const
    { return m_ptr >= o.m_ptr; }

    T* get() const
    {
        return m_ptr;
    }

private:

    T* m_ptr = nullptr;
};

template <typename T>
using random_iter = raw_iterator<T, std::random_access_iterator_tag>;

template <typename T>
using forward_iter = raw_iterator<T, std::forward_iterator_tag>;

void make_live(tracked* p, std::size_t n)
{
    for (std::size_t i = 0; i < n; ++i)
    {
        ::new (static_cast<void*>(p + i)) tracked;
    }
}

// Destroys by hand, independent of the functions under test.
void kill(tracked* p, std::size_t n)
{
    for (std::size_t i = 0; i < n; ++i)
    {
        p[i].~tracked();
    }
}

//-----------------------------------------------------------------------------
// call wrappers so one set of checks covers both construct variants
//-----------------------------------------------------------------------------

struct construct_fn
{
    template <typename A, typename B>
    auto operator()(A a, B b) const
    {
        return vx::range::construct(a, b);
    }
};

struct construct_maybe_trivial_fn
{
    template <typename A, typename B>
    auto operator()(A a, B b) const
    {
        return vx::range::construct_maybe_trivial(a, b);
    }
};

struct destroy_fn
{
    template <typename A, typename B>
    auto operator()(A a, B b) const
    {
        return vx::range::destroy(a, b);
    }
};

//-----------------------------------------------------------------------------
// construct checks
//-----------------------------------------------------------------------------

// fn(first, last) -> iterator, for iterators over raw tracked storage
template <typename IT, typename Fn>
void check_construct_iter(Fn fn)
{
    tracked::reset();
    raw_buffer<tracked, 5> buf;
    tracked* p = buf.data();

    IT first(p + 1);
    IT last(p + 4);

    // empty range
    VX_CHECK(fn(first, first) == first);
    VX_CHECK(tracked::constructed == 0);
    VX_CHECK(buf.is_untouched(0, 5));

    // full range
    VX_CHECK(fn(first, last) == last);
    VX_CHECK(tracked::constructed == 3);
    VX_CHECK(tracked::live == 3);

    for (std::size_t i = 1; i <= 3; ++i)
    {
        VX_CHECK(p[i].val == 7);
    }

    // elements outside [first, last) are not touched
    VX_CHECK(buf.is_untouched(0, 1));
    VX_CHECK(buf.is_untouched(4, 5));

    kill(p + 1, 3);
}

// Value types whose default constructor sets a known value.
template <typename T, typename Fn, typename Get>
void check_constructed_values(Fn fn, Get get, int expected)
{
    // pointer + count
    {
        raw_buffer<T, 5> buf;
        T* p = buf.data();

        VX_CHECK(fn(p + 1, std::size_t(3)) == p + 4);

        for (std::size_t i = 1; i <= 3; ++i)
        {
            VX_CHECK(get(p[i]) == expected);
        }

        VX_CHECK(buf.is_untouched(0, 1));
        VX_CHECK(buf.is_untouched(4, 5));
    }

    // iterator pair
    {
        raw_buffer<T, 5> buf;
        T* p = buf.data();

        VX_CHECK(fn(p + 1, p + 4) == p + 4);

        for (std::size_t i = 1; i <= 3; ++i)
        {
            VX_CHECK(get(p[i]) == expected);
        }

        VX_CHECK(buf.is_untouched(0, 1));
        VX_CHECK(buf.is_untouched(4, 5));
    }
}

// Trivial types: the value is unspecified (or zero), so only the return value
// and the extent of what was touched are checked.
template <typename T, typename Fn>
void check_construct_trivial_extent(Fn fn)
{
    {
        raw_buffer<T, 5> buf;
        T* p = buf.data();

        VX_CHECK(fn(p + 1, std::size_t(3)) == p + 4);
        VX_CHECK(buf.is_untouched(0, 1));
        VX_CHECK(buf.is_untouched(4, 5));

        VX_CHECK(fn(p + 2, std::size_t(0)) == p + 2);
    }

    {
        raw_buffer<T, 5> buf;
        T* p = buf.data();

        VX_CHECK(fn(p + 1, p + 4) == p + 4);
        VX_CHECK(buf.is_untouched(0, 1));
        VX_CHECK(buf.is_untouched(4, 5));

        VX_CHECK(fn(p + 2, p + 2) == p + 2);
    }
}

template <typename Fn>
void check_construct(Fn fn)
{
    VX_SECTION("count: zero");
    {
        tracked::reset();
        raw_buffer<tracked, 3> buf;
        tracked* p = buf.data();

        VX_CHECK(fn(p, std::size_t(0)) == p);
        VX_CHECK(tracked::constructed == 0);
        VX_CHECK(buf.is_untouched(0, 3));
    }

    VX_SECTION("count: constructs every element and returns one past the end");
    {
        tracked::reset();
        raw_buffer<tracked, 5> buf;
        tracked* p = buf.data();

        VX_CHECK(fn(p + 1, std::size_t(3)) == p + 4);
        VX_CHECK(tracked::constructed == 3);
        VX_CHECK(tracked::live == 3);

        for (std::size_t i = 1; i <= 3; ++i)
        {
            VX_CHECK(p[i].val == 7);
        }

        VX_CHECK(buf.is_untouched(0, 1));
        VX_CHECK(buf.is_untouched(4, 5));

        kill(p + 1, 3);
        VX_CHECK(tracked::live == 0);
    }

    VX_SECTION("count: single element");
    {
        tracked::reset();
        raw_buffer<tracked, 1> buf;
        tracked* p = buf.data();

        VX_CHECK(fn(p, std::size_t(1)) == p + 1);
        VX_CHECK(tracked::constructed == 1);
        VX_CHECK(p->val == 7);

        kill(p, 1);
    }

    VX_SECTION("iterator: raw pointers");
    {
        check_construct_iter<tracked*>(fn);
    }

    VX_SECTION("iterator: random access");
    {
        check_construct_iter<random_iter<tracked>>(fn);
    }

    VX_SECTION("iterator: forward");
    {
        check_construct_iter<forward_iter<tracked>>(fn);
    }

    VX_SECTION("movable_int / copyable_int run their default constructor");
    {
        check_constructed_values<movable_int>(fn, [](const movable_int& x)
            { return x.val; }, 0);
        check_constructed_values<copyable_int>(fn, [](const copyable_int& x)
            { return x.val; }, 0);
    }

    VX_SECTION("over_aligned runs its default member initializer");
    {
        check_constructed_values<over_aligned>(fn, [](const over_aligned& x)
            { return x.x; }, 3);
    }

    VX_SECTION("trivial types: return value and extent only");
    {
        check_construct_trivial_extent<int>(fn);
        check_construct_trivial_extent<trivial_int>(fn);
    }
}

//-----------------------------------------------------------------------------
// destroy checks
//-----------------------------------------------------------------------------

template <typename IT, typename Fn>
void check_destroy_iter(Fn fn)
{
    tracked::reset();
    raw_buffer<tracked, 5> buf;
    tracked* p = buf.data();
    make_live(p, 5);

    IT first(p + 1);
    IT last(p + 4);

    // empty range
    VX_CHECK(fn(first, first) == first);
    VX_CHECK(tracked::destroyed == 0);

    // full range
    VX_CHECK(fn(first, last) == last);
    VX_CHECK(tracked::destroyed == 3);
    VX_CHECK(tracked::live == 2); // the two outside the range are still alive

    kill(p, 1);
    kill(p + 4, 1);
    VX_CHECK(tracked::live == 0);
}

// Trivially destructible: nothing to run, but the return value must still be
// the end of the range.
template <typename T, typename Fn>
void check_destroy_trivial(Fn fn)
{
    raw_buffer<T, 4> buf;
    T* p = buf.data();

    VX_CHECK(fn(p, std::size_t(4)) == p + 4);
    VX_CHECK(fn(p, std::size_t(0)) == p);

    VX_CHECK(fn(p, p + 4) == p + 4);
    VX_CHECK(fn(p, p) == p);

    using F = forward_iter<T>;
    VX_CHECK(fn(F(p), F(p + 4)) == F(p + 4));

    using R = random_iter<T>;
    VX_CHECK(fn(R(p), R(p + 4)) == R(p + 4));
}

template <typename Fn>
void check_destroy(Fn fn)
{
    VX_SECTION("count: zero");
    {
        tracked::reset();
        raw_buffer<tracked, 3> buf;
        tracked* p = buf.data();
        make_live(p, 3);

        VX_CHECK(fn(p, std::size_t(0)) == p);
        VX_CHECK(tracked::destroyed == 0);
        VX_CHECK(tracked::live == 3);

        kill(p, 3);
    }

    VX_SECTION("count: destroys exactly the given objects and returns one past the end");
    {
        tracked::reset();
        raw_buffer<tracked, 5> buf;
        tracked* p = buf.data();
        make_live(p, 5);

        VX_CHECK(fn(p + 1, std::size_t(3)) == p + 4);
        VX_CHECK(tracked::destroyed == 3);
        VX_CHECK(tracked::live == 2);

        kill(p, 1);
        kill(p + 4, 1);
        VX_CHECK(tracked::live == 0);
    }

    VX_SECTION("count: single element");
    {
        tracked::reset();
        raw_buffer<tracked, 1> buf;
        tracked* p = buf.data();
        make_live(p, 1);

        VX_CHECK(fn(p, std::size_t(1)) == p + 1);
        VX_CHECK(tracked::destroyed == 1);
        VX_CHECK(tracked::live == 0);
    }

    VX_SECTION("iterator: raw pointers");
    {
        check_destroy_iter<tracked*>(fn);
    }

    VX_SECTION("iterator: random access");
    {
        check_destroy_iter<random_iter<tracked>>(fn);
    }

    VX_SECTION("iterator: forward");
    {
        check_destroy_iter<forward_iter<tracked>>(fn);
    }

    VX_SECTION("trivially destructible types");
    {
        check_destroy_trivial<int>(fn);
        check_destroy_trivial<trivial_int>(fn);
        check_destroy_trivial<movable_int>(fn);
        check_destroy_trivial<copyable_int>(fn);
        check_destroy_trivial<over_aligned>(fn);
    }
}

//-----------------------------------------------------------------------------
// destroy_n checks
//-----------------------------------------------------------------------------

template <typename IT>
void check_destroy_n_tracked()
{
    tracked::reset();
    raw_buffer<tracked, 5> buf;
    tracked* p = buf.data();
    make_live(p, 5);

    // zero
    VX_CHECK(vx::range::destroy_n(IT(p + 1), std::size_t(0)) == IT(p + 1));
    VX_CHECK(tracked::destroyed == 0);

    // n objects, returns first advanced by n
    VX_CHECK(vx::range::destroy_n(IT(p + 1), std::size_t(3)) == IT(p + 4));
    VX_CHECK(tracked::destroyed == 3);
    VX_CHECK(tracked::live == 2);

    kill(p, 1);
    kill(p + 4, 1);
    VX_CHECK(tracked::live == 0);
}

template <typename T, typename IT>
void check_destroy_n_trivial_iter()
{
    raw_buffer<T, 4> buf;
    T* p = buf.data();

    VX_CHECK(vx::range::destroy_n(IT(p), std::size_t(0)) == IT(p));
    VX_CHECK(vx::range::destroy_n(IT(p), std::size_t(4)) == IT(p + 4));
    VX_CHECK(vx::range::destroy_n(IT(p + 1), std::size_t(2)) == IT(p + 3));
}

template <typename T>
void check_destroy_n_trivial()
{
    check_destroy_n_trivial_iter<T, T*>();
    check_destroy_n_trivial_iter<T, random_iter<T>>();
    check_destroy_n_trivial_iter<T, forward_iter<T>>();
}

} // namespace

//=============================================================================
// construct
//=============================================================================

VX_TEST_CASE(construct_range)
{
    check_construct(construct_fn{});
}

VX_TEST_CASE(construct_maybe_trivial_range)
{
    check_construct(construct_maybe_trivial_fn{});
}

//=============================================================================
// destroy
//=============================================================================

VX_TEST_CASE(destroy_range)
{
    check_destroy(destroy_fn{});
}

VX_TEST_CASE(destroy_n_range)
{
    VX_SECTION("tracked: raw pointers");
    {
        check_destroy_n_tracked<tracked*>();
    }

    VX_SECTION("tracked: random access");
    {
        check_destroy_n_tracked<random_iter<tracked>>();
    }

    VX_SECTION("tracked: forward");
    {
        check_destroy_n_tracked<forward_iter<tracked>>();
    }

    VX_SECTION("trivially destructible types, every iterator kind");
    {
        check_destroy_n_trivial<int>();
        check_destroy_n_trivial<trivial_int>();
        check_destroy_n_trivial<movable_int>();
        check_destroy_n_trivial<copyable_int>();
        check_destroy_n_trivial<over_aligned>();
    }
}

//=============================================================================
// construct then destroy round trip
//=============================================================================

VX_TEST_CASE(construct_destroy_round_trip)
{
    VX_SECTION("tracked: live count returns to zero");
    {
        tracked::reset();
        raw_buffer<tracked, 8> buf;
        tracked* p = buf.data();

        tracked* end = vx::range::construct(p, std::size_t(8));
        VX_CHECK(end == p + 8);
        VX_CHECK(tracked::live == 8);

        tracked* end2 = vx::range::destroy(p, std::size_t(8));
        VX_CHECK(end2 == p + 8);
        VX_CHECK(tracked::live == 0);
        VX_CHECK(tracked::constructed == 8);
        VX_CHECK(tracked::destroyed == 8);
    }

    VX_SECTION("tracked: iterator forms and destroy_n");
    {
        tracked::reset();
        raw_buffer<tracked, 6> buf;
        tracked* p = buf.data();

        VX_CHECK(vx::range::construct(p, p + 3) == p + 3);
        VX_CHECK(vx::range::construct_maybe_trivial(p + 3, std::size_t(3)) == p + 6);
        VX_CHECK(tracked::live == 6);

        VX_CHECK(vx::range::destroy_n(p, std::size_t(2)) == p + 2);
        VX_CHECK(vx::range::destroy(p + 2, p + 6) == p + 6);
        VX_CHECK(tracked::live == 0);
    }
}

//=============================================================================
// noexcept
//=============================================================================

VX_TEST_CASE(destroy_noexcept)
{
    tracked* tp = nullptr;
    int* ip = nullptr;

    VX_STATIC_CHECK(noexcept(vx::range::destroy(tp, std::size_t(0))));
    VX_STATIC_CHECK(noexcept(vx::range::destroy(tp, tp)));
    VX_STATIC_CHECK(noexcept(vx::range::destroy_n(tp, std::size_t(0))));

    VX_STATIC_CHECK(noexcept(vx::range::destroy(ip, std::size_t(0))));
    VX_STATIC_CHECK(noexcept(vx::range::destroy(ip, ip)));
    VX_STATIC_CHECK(noexcept(vx::range::destroy_n(ip, std::size_t(0))));
}

} // namespace test
} // namespace vx

#ifndef VX_TEST_NO_MAIN
int main(int argc, char** argv)
{
    return VX_RUN_TESTS(argc, argv);
}
#endif
