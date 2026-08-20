#pragma once

#include <iostream>
#include <string>
#include <utility>

#include "vertex/os/atomic.hpp"
#include "vertex/std/string.hpp"
#include "vertex/util/random.hpp"
#define VX_ENABLE_PROFILING
#include "vertex/system/profiler.hpp"

//=========================================================================

using char_type = char;

using str1 = std::basic_string<char_type>;
using str2 = vx::str::basic_string<char_type>;

//=========================================================================

static constexpr size_t RR = 500; // number of repetitions
// this needs to stay small enough to not trigger std::string alignment optimization
static constexpr size_t NN = 100; // number of elements

#if defined(_MSC_VER)

static constexpr size_t mscv_manual_align_byte_count = 4096;
VX_STATIC_ASSERT_MSG(NN * sizeof(char_type) < mscv_manual_align_byte_count, "Element count too large");

#endif // defined(_MSC_VER)

//=========================================================================

template <typename Str>
std::string function_name(const char* fn)
{
    VX_IF_CONSTEXPR ((std::is_same<Str, str1>::value))
    {
        return std::string(fn) + " (std)";
    }
    else // VX_IF_CONSTEXPR ((std::is_same<Str, str2>::value))
    {
        return std::string(fn) + " (vx)";
    }
}

#define function_str(str_) (function_name<Str>(str_))
#define start_timer(str_)  ::vx::profile::_priv::profile_timer timer(function_str(str_))
#define stop_timer()       timer.stop()

//=========================================================================
// constructors
//=========================================================================

template <typename Str>
VX_NO_INLINE void profile_fill_constructor(size_t N)
{
    start_timer("fill construct");
    Str s(N, char_type{ 'a' });
    stop_timer();
}

template <typename Str>
VX_NO_INLINE void profile_list_constructor(size_t)
{
    Str s1{ char_type{ 'a' }, char_type{ 'a' }, char_type{ 'a' }, char_type{ 'a' } };
    start_timer("list construct");
    Str s2(s1);
    stop_timer();
}

template <typename Str>
VX_NO_INLINE void profile_copy_constructor(size_t N)
{
    Str s1(N, char_type{ 'a' });
    start_timer("copy construct");
    Str s2(s1);
    stop_timer();
}

template <typename Str>
VX_NO_INLINE void profile_range_constructor(size_t N)
{
    Str s1(N, char_type{ 'a' });
    start_timer("range construct");
    Str s2(s1.begin(), s1.end());
    stop_timer();
}

template <typename Str>
VX_NO_INLINE void profile_move_constructor(size_t N)
{

    Str src(N, char_type{ 'a' });
    start_timer("move construct");
    Str dst = std::move(src);
    stop_timer();
}

//=========================================================================
// destructor
//=========================================================================

template <typename Str>
VX_NO_INLINE void profile_destructor(size_t N)
{
    Str* s = vx::mem::default_allocator<Str>::allocate(1);
    vx::mem::construct_in_place(s, N, char_type{ 'a' });

    start_timer("destruct");
    vx::mem::destroy_in_place(s);
    stop_timer();

    vx::mem::default_allocator<Str>::deallocate(s, 1);
}

//=========================================================================
// assignment
//=========================================================================

template <typename Str>
void profile_copy_assignment_reallocate(size_t N)
{
    Str src(N, char_type{ 'a' });
    Str dst(N / 2, char_type{ 'a' });

    vx::os::do_not_optimize(src);
    vx::os::do_not_optimize(dst);

    start_timer("copy assign realloc");
    dst = src;
    stop_timer();
}

template <typename Str>
void profile_copy_assignment_grow(size_t N)
{
    Str src(N, char_type{ 'a' });
    Str dst(N / 2, char_type{ 'a' });
    dst.reserve(N);

    vx::os::do_not_optimize(src);
    vx::os::do_not_optimize(dst);

    {
        start_timer("copy assign grow");
        dst.operator=(src);
        stop_timer();
    }
}

template <typename Str>
void profile_copy_assignment_shrink(size_t N)
{
    Str src(N / 2, char_type{ 'a' });
    Str dst(N, char_type{ 'a' });

    start_timer("copy assign shrink");
    dst = src;
    stop_timer();
}

template <typename Str>
void profile_list_assignment(size_t N)
{
    using char_type = typename Str::value_type;
    Str dst(N, char_type{ 'a' });

    start_timer("list assign");
    dst = { char_type{ 'a' }, char_type{ 'a' }, char_type{ 'a' }, char_type{ 'a' }, char_type{ 'a' } };
    stop_timer();
}

template <typename Str>
void profile_move_assignment(size_t N)
{
    Str src(N, char_type{ 'a' });
    Str dst(N / 2, char_type{ 'a' });

    start_timer("move assign");
    dst = std::move(src);
    stop_timer();
}

//=========================================================================
// reserve
//=========================================================================

template <typename Str>
void profile_reserve_grow(size_t N)
{
    // If the sizes are too big, we will lose here because std::string
    // will switch to its big allocation alignment which appeals to
    // stringization. We will still win head to head in non stringization
    // and stringization, but we will lose if we are not stringized when
    // std::string is.

    Str s(N / 2, char_type{ 'a' });
    start_timer("reserve grow");
    s.reserve(N);
    stop_timer();
}

template <typename Str>
void profile_reserve_shrink(size_t N)
{
    Str s(N, char_type{ 'a' });
    start_timer("reserve shrink");
    s.reserve(N);
    stop_timer();
}

//=========================================================================
// clear
//=========================================================================

template <typename Str>
void profile_clear(size_t N)
{
    Str s(N, char_type{ 'a' });
    start_timer("clear");
    s.clear();
    stop_timer();
}

template <typename Str>
void profile_shrink_to_fit(size_t N)
{
    Str s(N / 2, char_type{ 'a' });
    s.reserve(N);
    start_timer("shrink to fit");
    s.shrink_to_fit();
    stop_timer();
}

//=========================================================================
// resize
//=========================================================================

template <typename Str>
void profile_resize_grow(size_t N)
{
    Str s(N / 2, char_type{ 'a' });
    start_timer("resize grow");
    s.resize(N, char_type{ 'a' });
    stop_timer();
}

template <typename Str>
void profile_resize_shrink(size_t N)
{
    Str s(N, char_type{ 'a' });
    start_timer("resize shrink");
    s.resize(N / 2, char_type{ 'a' });
    stop_timer();
}

//=========================================================================
// insert
//=========================================================================

template <typename Str>
void profile_insert_n(size_t N)
{
    using char_type = typename Str::value_type;
    Str s(N, char_type{ 'a' });
    start_timer("insert n");
    s.insert(s.begin() + static_cast<typename Str::difference_type>(N / 2), N / 2, char_type{ 'a' });
    stop_timer();
}

template <typename Str>
void profile_insert_n_unused(size_t N)
{
    using char_type = typename Str::value_type;
    Str s(N, char_type{ 'a' });
    s.reserve(N + N / 2);
    start_timer("insert n unused");
    s.insert(s.begin() + static_cast<typename Str::difference_type>(N / 2), N / 2, char_type{ 'a' });
    stop_timer();
}

template <typename Str>
void profile_insert_n_back(size_t N)
{
    using char_type = typename Str::value_type;
    Str s(N, char_type{ 'a' });
    start_timer("insert n back");
    s.insert(s.end(), N / 2, char_type{ 'a' });
    stop_timer();
}

template <typename Str>
void profile_insert_range(size_t N)
{
    Str s1(N / 2, char_type{ 'a' });
    Str s2(N, char_type{ 'a' });
    start_timer("insert range");
    s2.insert(s2.begin() + static_cast<typename Str::difference_type>(N / 2), s1.begin(), s1.end());
    stop_timer();
}

//=========================================================================
// erase
//=========================================================================

template <typename Str>
void profile_erase(size_t N)
{
    Str s(N, char_type{ 'a' });
    start_timer("erase");
    s.erase(s.begin() + static_cast<typename Str::difference_type>(N / 2));
    stop_timer();
}

template <typename Str>
void profile_erase_range(size_t N)
{
    Str s(N, char_type{ 'a' });
    start_timer("erase range");
    s.erase(s.begin(), s.begin() + static_cast<typename Str::difference_type>(N / 2));
    stop_timer();
}

//=========================================================================
// compare
//=========================================================================

template <typename Str>
void profile_compare(size_t N)
{
    using char_type = typename Str::value_type;
    Str s1;

    for (size_t i = 0; i < N; ++i)
    {
        s1.push_back(char_type{ 'a' });
    }

    Str s2 = s1;
    volatile bool equal = false;

    {
        start_timer("compare");
        equal = s1 == s2;
        stop_timer();
    }

    if (s1.empty() != equal)
    {
    }
}

//=========================================================================
// push back
//=========================================================================

template <typename Str>
void profile_push_back(size_t N)
{
    using char_type = typename Str::value_type;
    Str s;

    start_timer("push back");
    for (size_t i = 0; i < N; ++i)
    {
        s.push_back(char_type{ 'a' });
    }
    stop_timer();
}

template <typename Str>
void profile_reserve_push_back(size_t N)
{
    using char_type = typename Str::value_type;
    Str s;
    s.reserve(N);

    start_timer("reserve push back");
    for (size_t i = 0; i < N; ++i)
    {
        s.push_back(char_type{ 'a' });
    }
    stop_timer();
}

//=========================================================================
// append
//=========================================================================

template <typename Str>
void profile_append(size_t N)
{
    using char_type = typename Str::value_type;
    Str s1(N / 2, char_type{ 'a' });
    Str s2(N / 2, char_type{ 'a' });

    start_timer("append");
    s1.append(s2);
    stop_timer();
}

template <typename Str>
void profile_append_n(size_t N)
{
    using char_type = typename Str::value_type;
    Str s(N / 2, char_type{ 'a' });

    start_timer("append n");
    s.append(N / 2, char_type{ 'a' });
    stop_timer();
}

//=========================================================================

template <typename Str>
void run(size_t N, size_t R)
{
    using test_fn = void (*)(size_t);

    test_fn tests[] = {

        profile_fill_constructor<Str>,
        profile_list_constructor<Str>,
        profile_copy_constructor<Str>,
        profile_range_constructor<Str>,
        profile_move_constructor<Str>,

        profile_destructor<Str>,

        profile_copy_assignment_reallocate<Str>,
        profile_copy_assignment_grow<Str>,
        profile_copy_assignment_shrink<Str>,
        profile_list_assignment<Str>,
        profile_move_assignment<Str>,

        profile_reserve_grow<Str>,
        profile_reserve_shrink<Str>,

        profile_clear<Str>,
        profile_shrink_to_fit<Str>,

        profile_resize_grow<Str>,
        profile_resize_shrink<Str>,

        profile_insert_n<Str>,
        profile_insert_n_unused<Str>,
        profile_insert_n_back<Str>,
        profile_insert_range<Str>,

        profile_erase<Str>,
        profile_erase_range<Str>,

        profile_push_back<Str>,
        profile_reserve_push_back<Str>,

        profile_append<Str>,
        profile_append_n<Str>,

        profile_compare<Str>
    };

    vx::random::gen rng;

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

//=========================================================================

template <typename str1, typename str2>
void run_combined(size_t N, size_t R)
{
    using test_fn = void (*)(size_t);

    test_fn tests_1[] = {
        profile_fill_constructor<str1>,
        profile_list_constructor<str1>,
        profile_copy_constructor<str1>,
        profile_range_constructor<str1>,
        profile_move_constructor<str1>,
        profile_destructor<str1>,
        profile_copy_assignment_reallocate<str1>,
        profile_copy_assignment_grow<str1>,
        profile_copy_assignment_shrink<str1>,
        profile_list_assignment<str1>,
        profile_move_assignment<str1>,
        profile_reserve_grow<str1>,
        profile_reserve_shrink<str1>,
        profile_clear<str1>,
        profile_shrink_to_fit<str1>,
        profile_resize_grow<str1>,
        profile_resize_shrink<str1>,
        profile_insert_n<str1>,
        profile_insert_n_unused<str1>,
        profile_insert_n_back<str1>,
        profile_insert_range<str1>,
        profile_erase<str1>,
        profile_erase_range<str1>,
        profile_push_back<str1>,
        profile_append<str1>,
        profile_append_n<str1>,
        profile_reserve_push_back<str1>,
        profile_compare<str1>
    };

    test_fn tests_2[] = {
        profile_fill_constructor<str2>,
        profile_list_constructor<str2>,
        profile_copy_constructor<str2>,
        profile_range_constructor<str2>,
        profile_move_constructor<str2>,
        profile_destructor<str2>,
        profile_copy_assignment_reallocate<str2>,
        profile_copy_assignment_grow<str2>,
        profile_copy_assignment_shrink<str2>,
        profile_list_assignment<str2>,
        profile_move_assignment<str2>,
        profile_reserve_grow<str2>,
        profile_reserve_shrink<str2>,
        profile_clear<str2>,
        profile_shrink_to_fit<str2>,
        profile_resize_grow<str2>,
        profile_resize_shrink<str2>,
        profile_insert_n<str2>,
        profile_insert_n_unused<str2>,
        profile_insert_n_back<str2>,
        profile_insert_range<str2>,
        profile_erase<str2>,
        profile_erase_range<str2>,
        profile_push_back<str2>,
        profile_append<str2>,
        profile_append_n<str2>,
        profile_reserve_push_back<str2>,
        profile_compare<str2>
    };

    constexpr size_t count = vx::mem::array_size(tests_1);
    VX_STATIC_ASSERT_MSG(count == vx::mem::array_size(tests_2), "test list mismatch");

    test_fn combined[2 * count];
    std::copy(std::begin(tests_1), std::end(tests_1), combined);
    std::copy(std::begin(tests_2), std::end(tests_2), combined + count);

    vx::random::gen rng;

    for (size_t r = 0; r < R; ++r)
    {
        test_fn selected[2 * count] = {};
        vx::random::sample(std::begin(combined), std::end(combined), selected, 2 * count, rng);

        for (auto test : selected)
        {
            test(N);
        }
    }
}
