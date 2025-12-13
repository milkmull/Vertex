#include <iostream>
#include <vector>

#include "vertex/std/_priv/dynamic_array_base.hpp"
#define VX_ENABLE_PROFILING
#include "vertex/system/profiler.hpp"

namespace vx {
template <typename T>
using vector = _priv::dynamic_array_base<T, aligned_allocator<mem::ideal_align>>;
} // namespace vx

template <typename T>
using vec1 = std::vector<T>;

template <typename T>
using vec2 = vx::vector<T>;

using vx::profile::is_enabled;
using vx::profile::start;
using vx::profile::stop;

struct big_data
{
    uint64_t a, b, c, d;
};

// Non-trivial type to check copy/move/destruct counts
struct non_trivial
{
    non_trivial()
        : x(0)
    {}
    non_trivial(int val)
        : x(val)
    {}
    non_trivial(const non_trivial& other)
    {
        x = other.x;
        ++copy_count;
    }
    non_trivial(non_trivial&& other) noexcept
    {
        x = other.x;
        other.x = 0;
        ++move_count;
    }
    ~non_trivial()
    {
        ++destruct_count;
    }

    non_trivial& operator=(const non_trivial& other)
    {
        x = other.x;
        ++copy_count;
        return *this;
    }
    non_trivial& operator=(non_trivial&& other) noexcept
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
};

static constexpr int R = 300;       // number of repetitions
static constexpr size_t N = 200000; // number of elements

// Helper profiling functions

template <typename Vec>
void profile_constructor(const char* name, size_t count)
{
    ::vx::profile::_priv::profile_timer timerVX_LINE(name);
    Vec v(count);
    timerVX_LINE.stop();
}

template <typename Vec>
void profile_push_back(const char* name, size_t count)
{
    VX_PROFILE_SCOPE(name);
    Vec v;
    for (size_t i = 0; i < count; ++i)
        v.push_back(big_data{ i, i + 1, i + 2, i + 3 });
}

template <typename Vec>
void profile_reserve_push_back(const char* name, size_t count)
{
    VX_PROFILE_SCOPE(name);
    Vec v;
    v.reserve(count);
    for (size_t i = 0; i < count; ++i)
        v.push_back(big_data{ i, i + 1, i + 2, i + 3 });
}

template <typename Vec>
void profile_allocate(const char* name, size_t count)
{
    VX_PROFILE_SCOPE(name);
    Vec src(count);
}

template <typename Vec>
void profile_copy(const char* name, size_t count)
{
    Vec src(count);
    ::vx::profile::_priv::profile_timer timerVX_LINE(name);
    Vec dst = src;
    timerVX_LINE.stop();
}

template <typename Vec>
void profile_copy_assignment(const char* name, size_t count)
{
    Vec src(count);
    Vec dst;
    {
        VX_PROFILE_SCOPE(name);
        dst = src;
    }
}

template <typename Vec>
void profile_move(const char* name, size_t count)
{
    Vec src(count);
    VX_PROFILE_SCOPE(name);
    Vec dst = std::move(src);
}

template <typename Vec>
void profile_move_assignment(const char* name, size_t count)
{
    Vec src(count);
    Vec dst;
    VX_PROFILE_SCOPE(name);
    dst = std::move(src);
}

// Non-trivial push_back test with counters
template <typename Vec>
void profile_push_back_non_trivial(const char* name, size_t count)
{
    non_trivial::reset_counters();

    VX_PROFILE_SCOPE(name);
    Vec v;
    for (size_t i = 0; i < count; ++i)
        v.push_back(non_trivial{ int(i) });
}

// Non-trivial push_back test with counters
template <typename Vec>
void profile_reserve_push_back_non_trivial(const char* name, size_t count)
{
    non_trivial::reset_counters();

    VX_PROFILE_SCOPE(name);
    Vec v;
    v.reserve(count);
    for (size_t i = 0; i < count; ++i)
        v.push_back(non_trivial{ int(i) });
}

int main()
{
    VX_PROFILE_START("profile_dynamic_array.csv");

    for (int r = 0; r < R; ++r)
    {
        //profile_constructor<vec1<big_data>>("construction (vec1)", N);
        //profile_constructor<vec2<big_data>>("construction (vec2)", N);

        //profile_push_back<vec1<big_data>>("push_back (vec1)", N);
        //profile_push_back<vec2<big_data>>("push_back (vec2)", N);
        //
        //profile_reserve_push_back<vec1<big_data>>("reserve push_back (vec1)", N);
        //profile_reserve_push_back<vec2<big_data>>("reserve push_back (vec2)", N);

        profile_copy<vec1<big_data>>("copy (vec1)", N);
        profile_copy<vec2<big_data>>("copy (vec2)", N);
        //
        //profile_copy_assignment<vec1<big_data>>("copy assignment (vec1)", N);
        //profile_copy_assignment<vec2<big_data>>("copy assignment (vec2)", N);

        //profile_move<vec2<big_data>>("move (vec2)", N);
        //profile_move<vec1<big_data>>("move (vec1)", N);
        //
        //profile_move_assignment<vec1<big_data>>("move assignment (vec1)", N);
        //profile_move_assignment<vec2<big_data>>("move assignment (vec2)", N);
        //
        //profile_push_back_non_trivial<vec1<non_trivial>>("push_back non_trivial (vec1)", N);
        //profile_push_back_non_trivial<vec2<non_trivial>>("push_back non_trivial (vec2)", N);
        //
        //profile_reserve_push_back_non_trivial<vec1<non_trivial>>("reserve push_back non_trivial (vec1)", N);
        //profile_reserve_push_back_non_trivial<vec2<non_trivial>>("reserve push_back non_trivial (vec2)", N);
        //
        //profile_copy<vec1<non_trivial>>("copy non_trivial (vec1)", N);
        //profile_copy<vec2<non_trivial>>("copy non_trivial (vec2)", N);
    }

    VX_PROFILE_STOP();
    return 0;
}
