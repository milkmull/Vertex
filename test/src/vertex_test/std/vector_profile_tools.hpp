#pragma once

#include<iostream>
#include <vector>
#include <utility>

#include "vertex/std/_priv/dynamic_array_base.hpp"
#include "vertex/util/random.hpp"
#define VX_ENABLE_PROFILING
#include "vertex/system/profiler.hpp"

//=========================================================================

namespace vx {
template <typename T>
using vector = _priv::dynamic_array_base<T>;
} // namespace vx

template <typename T>
using vec1 = std::vector<T>;

template <typename T>
using vec2 = vx::vector<T>;

//=========================================================================

struct trivial_type
{
    uint64_t a, b, c, d;
};

bool operator==(const trivial_type& lhs, const trivial_type& rhs) noexcept
{
    return lhs.a == rhs.a && lhs.b == rhs.b && lhs.c == rhs.c && lhs.d == rhs.d;
}

//=========================================================================

// Non-trivial type to check copy/move/destruct Ns
struct non_trivial_type
{
    non_trivial_type()
        : x(0)
    {}
    non_trivial_type(int val)
        : x(val)
    {}
    non_trivial_type(const non_trivial_type& other)
    {
        x = other.x;
        ++copy_count;
    }
    non_trivial_type(non_trivial_type&& other) noexcept
    {
        x = other.x;
        other.x = 0;
        ++move_count;
    }
    ~non_trivial_type()
    {
        ++destruct_count;
    }

    non_trivial_type& operator=(const non_trivial_type& other)
    {
        x = other.x;
        ++copy_count;
        return *this;
    }
    non_trivial_type& operator=(non_trivial_type&& other) noexcept
    {
        x = other.x;
        other.x = 0;
        ++move_count;
        return *this;
    }

    int x;

    static inline int copy_count = 0;
    static inline int move_count = 0;
    static inline int destruct_count = 0;

    static void reset_counters()
    {
        copy_count = 0;
        move_count = 0;
        destruct_count = 0;
    }

    friend bool operator==(const non_trivial_type& lhs, const non_trivial_type& rhs)
    {
        return lhs.x == rhs.x;
    }

    friend bool operator<(const non_trivial_type& lhs, const non_trivial_type& rhs)
    {
        return lhs.x < rhs.x;
    }

    friend bool operator>(const non_trivial_type& lhs, const non_trivial_type& rhs)
    {
        return rhs < lhs;
    }

    friend bool operator<=(const non_trivial_type& lhs, const non_trivial_type& rhs)
    {
        return !(rhs < lhs);
    }

    friend bool operator>=(const non_trivial_type& lhs, const non_trivial_type& rhs)
    {
        return !(lhs < rhs);
    }
};

//=========================================================================


static constexpr size_t RR = 2000; // number of repetitions
// this needs to stay small enough to not trigger std::vector alignment optimization
static constexpr size_t NN = 100; // number of elements

#if defined(_MSC_VER)

static constexpr size_t mscv_manual_align_byte_count = 4096;
VX_STATIC_ASSERT(NN * sizeof(trivial_type)      < mscv_manual_align_byte_count, "Element count too large");
VX_STATIC_ASSERT(NN * sizeof(non_trivial_type)  < mscv_manual_align_byte_count, "Element count too large");

#endif // defined(_MSC_VER)

//=========================================================================

template <typename Vec>
std::string function_name(const char* fn)
{
    VX_IF_CONSTEXPR((std::is_same<Vec, vec1<trivial_type>>::value))
    {
        return std::string(fn) + " (trivial std)";
    }
    else VX_IF_CONSTEXPR((std::is_same<Vec, vec1<non_trivial_type>>::value))
    {
        return std::string(fn) + " (non trivial std)";
    }
    else VX_IF_CONSTEXPR((std::is_same<Vec, vec2<trivial_type>>::value))
    {
        return std::string(fn) + " (trivial vx)";
    }
    else // VX_IF_CONSTEXPR((std::is_same<Vec, vec2<non_trivial_type>>::value))
    {
        VX_STATIC_ASSERT((std::is_same<Vec, vec2<non_trivial_type>>::value), "invalid type");
        return std::string(fn) + " (non trivial vx)";
    }
}

#define function_str(str)   (function_name<Vec>(str))
#define start_timer(str)    ::vx::profile::_priv::profile_timer timer(function_str(str))
#define stop_timer()        timer.stop()

//=========================================================================
// constructors
//=========================================================================

template <typename Vec>
VX_NO_INLINE void profile_size_constructor(size_t N)
{
    start_timer("size construct");
    Vec v(N);
    stop_timer();
}

template <typename Vec>
VX_NO_INLINE void profile_fill_constructor(size_t N)
{
    using T = typename Vec::value_type;
    start_timer("fill construct");
    Vec v(N, T{});
    stop_timer();
}

template <typename Vec>
VX_NO_INLINE void profile_list_constructor(size_t N)
{
    using T = typename Vec::value_type;
    Vec v1{ T{ 1 }, T{ 2 }, T{ 3 }, T{ 4 } };
    start_timer("list construct");
    Vec v2(v1);
    stop_timer();
}

template <typename Vec>
VX_NO_INLINE void profile_copy_constructor(size_t N)
{
    Vec v1(N);
    start_timer("copy construct");
    Vec v2(v1);
    stop_timer();
}

template <typename Vec>
VX_NO_INLINE void profile_range_constructor(size_t N)
{
    Vec v1(N);
    start_timer("range construct");
    Vec v2(v1.begin(), v1.end());
    stop_timer();
}

template <typename Vec>
VX_NO_INLINE void profile_move_constructor(size_t N)
{
    Vec src(N);
    start_timer("move construct");
    Vec dst = std::move(src);
    stop_timer();
}

//=========================================================================
// destructor
//=========================================================================

template <typename Vec>
VX_NO_INLINE void profile_destructor(size_t N)
{
    Vec* v = vx::default_allocator<Vec>::allocate(1);
    vx::mem::construct_in_place(v, N);

    start_timer("destruct");
    vx::mem::destroy_in_place(v);
    stop_timer();

    vx::default_allocator<Vec>::deallocate(v, 1);
}

//=========================================================================
// assignment
//=========================================================================

template <typename Vec>
void profile_copy_assignment(size_t N)
{
    Vec src(N);
    Vec dst(N / 2);

    start_timer("copy assign");
    dst = src;
    stop_timer();
}

template <typename Vec>
void profile_list_assignment(size_t N)
{
    using T = typename Vec::value_type;
    Vec dst(N);

    start_timer("list assign");
    dst = { T{ 0 }, T{ 1 }, T{ 2 }, T{ 3 }, T{ 4 } };
    stop_timer();
}

template <typename Vec>
void profile_move_assignment(size_t N)
{
    Vec src(N);
    Vec dst(N / 2);

    start_timer("move assign");
    dst = std::move(src);
    stop_timer();
}

//=========================================================================
// reserve
//=========================================================================

template <typename Vec>
void profile_reserve_grow(size_t N)
{
    // If the sizes are too big, we will lose here because std::vector
    // will switch to its big allocation alignment which appeals to
    // vectorization. We will still win head to head in non vectorization
    // and vectorization, but we will lose if we are not vectorized when
    // std::vector is.

    Vec v(N / 2);
    start_timer("reserve grow");
    v.reserve(N);
    stop_timer();
}

template <typename Vec>
void profile_reserve_shrink(size_t N)
{
    Vec v(N);
    start_timer("reserve shrink");
    v.reserve(N / 2);
    stop_timer();
}

//=========================================================================
// clear
//=========================================================================

template <typename Vec>
void profile_clear(size_t N)
{
    Vec v(N);
    start_timer("clear");
    v.clear();
    stop_timer();
}

template <typename Vec>
void profile_shrink_to_fit(size_t N)
{
    Vec v(N / 2);
    v.reserve(N);
    start_timer("shrink to fit");
    v.shrink_to_fit();
    stop_timer();
}

//=========================================================================
// resize
//=========================================================================

template <typename Vec>
void profile_resize_grow(size_t N)
{
    Vec v(N / 2);
    start_timer("resize grow");
    v.resize(N);
    stop_timer();
}

template <typename Vec>
void profile_resize_shrink(size_t N)
{
    Vec v(N);
    start_timer("resize shrink");
    v.resize(N / 2);
    stop_timer();
}

//=========================================================================
// emplace
//=========================================================================

template <typename Vec>
void profile_emplace(size_t N)
{
    Vec v;
    v.reserve(N);

    start_timer("emplace");
    for (size_t i = 0; i < N; ++i)
    {
        v.emplace(v.begin() + v.size() / 2);
    }
    stop_timer();
}

template <typename Vec>
void profile_emplace_grow(size_t N)
{
    Vec v;

    start_timer("emplace grow");
    for (size_t i = 0; i < N; ++i)
    {
        v.emplace(v.begin() + v.size() / 2);
    }
    stop_timer();
}

//=========================================================================
// insert
//=========================================================================

template <typename Vec>
void profile_insert_n(size_t N)
{
    using T = typename Vec::value_type;
    Vec v(N);
    start_timer("insert n");
    v.insert(v.begin() + N / 2, N / 2, T{});
    stop_timer();
}

template <typename Vec>
void profile_insert_n_unused(size_t N)
{
    using T = typename Vec::value_type;
    Vec v(N);
    v.reserve(N + N / 2);
    start_timer("insert n unused");
    v.insert(v.begin() + N / 2, N / 2, T{});
    stop_timer();
}

template <typename Vec>
void profile_insert_n_back(size_t N)
{
    using T = typename Vec::value_type;
    Vec v(N);
    start_timer("insert n back");
    v.insert(v.end(), N / 2, T{});
    stop_timer();
}

template <typename Vec>
void profile_insert_range(size_t N)
{
    Vec v1(N / 2);
    Vec v2(N);
    start_timer("insert range");
    v2.insert(v2.begin() + N / 2, v1.begin(), v1.end());
    stop_timer();
}

//=========================================================================
// erase
//=========================================================================

template <typename Vec>
void profile_erase(size_t N)
{
    Vec v(N);
    start_timer("erase");
    v.erase(v.begin() + N / 2);
    stop_timer();
}

template <typename Vec>
void profile_erase_range(size_t N)
{
    Vec v(N);
    start_timer("erase range");
    v.erase(v.begin(), v.begin() + N / 2);
    stop_timer();
}

//=========================================================================
// compare
//=========================================================================

template <typename Vec>
void profile_compare(size_t N)
{
    using T = typename Vec::value_type;
    Vec v1;

    for (size_t i = 0; i < N; ++i)
    {
        v1.push_back(T{});
    }

    Vec v2 = v1;
    bool equal = false;

    {
        start_timer("compare");
        equal = v1 == v2;
        stop_timer();
    }
}

//=========================================================================
// push back
//=========================================================================

template <typename Vec>
void profile_push_back(size_t N)
{
    using T = typename Vec::value_type;
    Vec v;

    start_timer("push back");
    for (size_t i = 0; i < N; ++i)
    {
        v.emplace_back(T{});
    }
    stop_timer();
}

template <typename Vec>
void profile_reserve_push_back(size_t N)
{
    using T = typename Vec::value_type;
    Vec v;
    v.reserve(N);

    start_timer("reserve push back");
    for (size_t i = 0; i < N; ++i)
    {
        v.push_back(T{});
    }
    stop_timer();
}

//=========================================================================

template <typename Vec>
void run(size_t N, size_t R)
{
    using test_fn = void(*)(size_t);

    test_fn tests[] = {

        //profile_size_constructor<Vec>,
        //profile_fill_constructor<Vec>,
        //profile_list_constructor<Vec>,
        //profile_copy_constructor<Vec>,
        //profile_range_constructor<Vec>,
        //profile_move_constructor<Vec>,
        //
        //profile_destructor<Vec>,
        //
        //profile_copy_assignment<Vec>,
        //profile_list_assignment<Vec>,
        //profile_move_assignment<Vec>,
        //
        //profile_reserve_grow<Vec>,
        //profile_reserve_shrink<Vec>,
        //
        //profile_clear<Vec>,
        //profile_shrink_to_fit<Vec>,
        //
        //profile_resize_grow<Vec>,
        //profile_resize_shrink<Vec>,
        //
        //profile_emplace<Vec>,
        //profile_emplace_grow<Vec>,
        //
        //profile_insert_n<Vec>,
        //profile_insert_n_unused<Vec>,
        //profile_insert_n_back<Vec>,
        //profile_insert_range<Vec>,
        //
        //profile_erase<Vec>,
        //profile_erase_range<Vec>,
        
        profile_push_back<Vec>,
        profile_reserve_push_back<Vec>,
        
        //profile_compare<Vec>
    };

    vx::random::rng rng;

    for (size_t r = 0; r < R; ++r)
    {
        constexpr size_t count = vx::mem::array_size(tests);
        test_fn selected_tests[count] = {};

        vx::random::sample(std::begin(tests), std::end(tests), selected_tests, count, rng);

        for (auto test : selected_tests)
        {
            test(N);
        }
    }
}
