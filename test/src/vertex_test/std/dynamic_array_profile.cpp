#include <iostream>
#include <vector>

#include "vertex/std/_priv/dynamic_array_base.hpp"
#define VX_ENABLE_PROFILING
#include "vertex/system/profiler.hpp"
#include <utility>

namespace vx {
template <typename T>
using vector = _priv::dynamic_array_base<T>;
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

bool operator==(const big_data& lhs, const big_data& rhs) noexcept
{
    return lhs.a == rhs.a && lhs.b == rhs.b && lhs.c == rhs.c && lhs.d == rhs.d;
}

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

static constexpr int R = 3000;       // number of repetitions
// this needs to stay small enough to not trigger std::vector alignment optimization
static constexpr size_t N = 300; // number of elements

// Helper profiling functions

template <typename Vec>
VX_NO_INLINE void profile_size_constructor(const char* name, size_t count)
{
    ::vx::profile::_priv::profile_timer timer(name);
    Vec v(count);
    timer.stop();
}

template <typename Vec, typename T>
VX_NO_INLINE void profile_fill_constructor(const char* name, size_t count, const T& value)
{
    ::vx::profile::_priv::profile_timer timer(name);
    Vec v(count, value);
    timer.stop();
}

template <typename Vec>
VX_NO_INLINE void profile_copy_constructor(const char* name, size_t count)
{
    Vec v1(count);
    ::vx::profile::_priv::profile_timer timer(name);
    Vec v2(v1);
    timer.stop();
}

template <typename Vec>
VX_NO_INLINE void profile_range_constructor(const char* name, size_t count)
{
    Vec v1(count);
    ::vx::profile::_priv::profile_timer timer(name);
    Vec v2(v1.begin(), v1.end());
    timer.stop();
}

template <typename Vec>
VX_NO_INLINE void profile_destructor(const char* name, size_t count)
{
    Vec* v = vx::default_allocator<Vec>::allocate(1);
    vx::mem::construct_in_place(v, count);

    ::vx::profile::_priv::profile_timer timer(name);
    vx::mem::destroy_in_place(v);
    timer.stop();

    vx::default_allocator<Vec>::deallocate(v, 1);
}

template <typename Vec>
void profile_push_back(const char* name, size_t count)
{
    Vec v;

    ::vx::profile::_priv::profile_timer timer(name);
    for (size_t i = 0; i < count; ++i)
    {
        v.push_back(big_data{ i, i + 1, i + 2, i + 3 });
    }
    timer.stop();

}

template <typename Vec>
void profile_reserve_push_back(const char* name, size_t count)
{
    Vec v;
    v.reserve(count);

    ::vx::profile::_priv::profile_timer timer(name);
    for (size_t i = 0; i < count; ++i)
    {
        v.push_back(big_data{ i, i + 1, i + 2, i + 3 });
    }
    timer.stop();
}

template <typename Vec>
void profile_allocate(const char* name, size_t count)
{
    VX_PROFILE_SCOPE(name);
    Vec src(count);
}

template <typename Vec>
void profile_copy_assignment(const char* name, size_t count)
{
    Vec src(count);
    Vec dst(count / 2);
    {
        VX_PROFILE_SCOPE(name);
        dst = src;
    }
}

template <typename Vec>
void profile_move(const char* name, size_t count)
{
    Vec src(count);
    {
        VX_PROFILE_SCOPE(name);
        Vec dst = std::move(src);
    }
}

template <typename Vec>
void profile_move_assignment(const char* name, size_t count)
{
    Vec src(count);
    Vec dst(count / 2);
    {
        VX_PROFILE_SCOPE(name);
        dst = std::move(src);
    }
}

template <typename Vec>
void profile_reserve_grow(const char* name, size_t count)
{
    // If the sizes are too big, we will lose here because std::vector
    // will switch to its big allocation alignment which appeals to
    // vectorization. We will still win head to head in non vectorization
    // and vectorization, but we will lose if we are not vectorized when
    // std::vector is.

    Vec v(count / 2);
    ::vx::profile::_priv::profile_timer timer(name);
    v.reserve(count);
    timer.stop();
}

template <typename Vec>
void profile_reserve_shrink(const char* name, size_t count)
{
    Vec v(count);
    ::vx::profile::_priv::profile_timer timer(name);
    v.reserve(count / 2);
    timer.stop();
}

template <typename Vec>
void profile_clear(const char* name, size_t count)
{
    Vec v(count);
    ::vx::profile::_priv::profile_timer timer(name);
    v.clear();
    timer.stop();
}

template <typename Vec>
void profile_shrink_to_fit(const char* name, size_t count)
{
    Vec v(count / 2);
    v.reserve(count);
    ::vx::profile::_priv::profile_timer timer(name);
    v.shrink_to_fit();
    timer.stop();
}

template <typename Vec>
void profile_resize_grow(const char* name, size_t count)
{
    Vec v(count / 2);
    ::vx::profile::_priv::profile_timer timer(name);
    v.resize(count);
    timer.stop();
}

template <typename Vec>
void profile_resize_shrink(const char* name, size_t count)
{
    Vec v(count);
    ::vx::profile::_priv::profile_timer timer(name);
    v.resize(count / 2);
    timer.stop();
}

template <typename Vec>
void profile_emplace(const char* name, size_t count)
{
    Vec v(count);
    v.reserve(count + 1);
    ::vx::profile::_priv::profile_timer timer(name);
    v.emplace(v.begin() + count / 2);
    timer.stop();
}

template <typename Vec>
void profile_emplace_grow(const char* name, size_t count)
{
    Vec v(count, big_data{ 1, 1, 1, 1 });
    ::vx::profile::_priv::profile_timer timer(name);
    v.emplace(v.begin() + count / 2);
    timer.stop();

    auto x = v[count / 2];
}

template <typename Vec>
void profile_insert_n(const char* name, size_t count)
{
    Vec v(count);
    ::vx::profile::_priv::profile_timer timer(name);
    v.insert(v.begin() + count / 2, count / 2, big_data{ 0, 1, 2, 3 });
    timer.stop();
}

template <typename Vec>
void profile_insert_n_back(const char* name, size_t count)
{
    Vec v(count);
    ::vx::profile::_priv::profile_timer timer(name);
    v.insert(v.end(), count / 2, big_data{ 0, 1, 2, 3 });
    timer.stop();
}

template <typename Vec>
void profile_insert_range(const char* name, size_t count)
{
    Vec v1(count / 2);
    Vec v2(count);
    ::vx::profile::_priv::profile_timer timer(name);
    v2.insert(v2.begin() + count / 2, v1.begin(), v1.end());
    timer.stop();
}

template <typename Vec>
void profile_erase(const char* name, size_t count)
{
    Vec v(count);
    ::vx::profile::_priv::profile_timer timer(name);
    v.erase(v.begin() + count / 2);
    timer.stop();
}

template <typename Vec>
void profile_erase_range(const char* name, size_t count)
{
    Vec v(count);
    ::vx::profile::_priv::profile_timer timer(name);
    v.erase(v.begin(), v.begin() + count / 2);
    timer.stop();
}

template <typename Vec>
bool profile_compare(const char* name, size_t count)
{
    Vec v1;

    for (size_t i = 0; i < count; ++i)
    {
        v1.push_back(big_data{ i, i + 1, i + 2, i + 3 });
    }

    Vec v2 = v1;
    bool equal = false;

    {
        VX_PROFILE_SCOPE(name);
        equal = v1 == v2;
    }

    return equal;
}

int main()
{
    VX_PROFILE_START("profile_dynamic_array.csv");

    for (int r = 0; r < R; ++r)
    {
        //profile_size_constructor<vec1<big_data>>("size construction (vec1)", N);
        //profile_size_constructor<vec2<big_data>>("size construction (vec2)", N);
        
        //profile_fill_constructor<vec1<big_data>>("fill construction (vec1)", N, big_data{ 1, 2, 3, 4 });
        //profile_fill_constructor<vec2<big_data>>("fill construction (vec2)", N, big_data{ 1, 2, 3, 4 });
        //
        //profile_copy_constructor<vec1<big_data>>("copy construction (vec1)", N);
        //profile_copy_constructor<vec2<big_data>>("copy construction (vec2)", N);
        //
        //profile_range_constructor<vec1<big_data>>("range construction (vec1)", N);
        //profile_range_constructor<vec2<big_data>>("range construction (vec2)", N);
        //
        //profile_destructor<vec1<big_data>>("destruction (vec1)", N);
        //profile_destructor<vec2<big_data>>("destruction (vec2)", N);
        // 
        //profile_copy_assignment<vec1<big_data>>("copy assignment (vec1)", N);
        //profile_copy_assignment<vec2<big_data>>("copy assignment (vec2)", N);
        //
        //profile_copy_assignment<vec1<non_trivial>>("copy assignment non trivial (vec1)", N);
        //profile_copy_assignment<vec2<non_trivial>>("copy assignment non trivial (vec2)", N);
        //
        //profile_move<vec1<big_data>>("move (vec1)", N);
        //profile_move<vec2<big_data>>("move (vec2)", N);
        //
        //profile_move_assignment<vec1<big_data>>("move assignment (vec1)", N);
        //profile_move_assignment<vec2<big_data>>("move assignment (vec2)", N);
        //
        profile_reserve_grow<vec1<big_data>>("reserve grow (vec1)", N);
        profile_reserve_grow<vec2<big_data>>("reserve grow (vec2)", N);
        profile_reserve_shrink<vec1<big_data>>("reserve shrink (vec1)", N);
        profile_reserve_shrink<vec2<big_data>>("reserve shrink (vec2)", N);
        //
        profile_push_back<vec1<big_data>>("push_back (vec1)", N);
        profile_push_back<vec2<big_data>>("push_back (vec2)", N);
        profile_reserve_push_back<vec1<big_data>>("reserve push_back (vec1)", N);
        profile_reserve_push_back<vec2<big_data>>("reserve push_back (vec2)", N);
        // 
        profile_resize_grow<vec1<big_data>>("resize grow (vec1)", N);
        profile_resize_grow<vec2<big_data>>("resize grow (vec2)", N);
        profile_resize_shrink<vec1<big_data>>("resize shrink (vec1)", N);
        profile_resize_shrink<vec2<big_data>>("resize shrink (vec2)", N);

        //profile_clear<vec1<big_data>>("clear (vec1)", N);
        //profile_clear<vec2<big_data>>("clear (vec2)", N);

        //profile_shrink_to_fit<vec1<big_data>>("shrink to fit (vec1)", N);
        //profile_shrink_to_fit<vec2<big_data>>("shrink to fit (vec2)", N);
        // 
        profile_emplace<vec1<big_data>>("emplace (vec1)", N);
        profile_emplace<vec2<big_data>>("emplace (vec2)", N);
        profile_emplace_grow<vec1<big_data>>("emplace grow (vec1)", N);
        profile_emplace_grow<vec2<big_data>>("emplace grow (vec2)", N);
        //
        profile_insert_n<vec1<big_data>>("insert n (vec1)", N);
        profile_insert_n<vec2<big_data>>("insert n (vec2)", N);
        profile_insert_n_back<vec1<big_data>>("insert n back (vec1)", N);
        profile_insert_n_back<vec2<big_data>>("insert n back (vec2)", N);
        profile_insert_range<vec1<big_data>>("insert range (vec1)", N);
        profile_insert_range<vec2<big_data>>("insert range (vec2)", N);
        //
        //profile_erase<vec1<big_data>>("erase (vec1)", N);
        //profile_erase<vec2<big_data>>("erase (vec2)", N);
        //profile_erase_range<vec1<big_data>>("erase range (vec1)", N);
        //profile_erase_range<vec2<big_data>>("erase range (vec2)", N);
        
        //profile_compare<vec1<big_data>>("compare (vec1)", N);
        //profile_compare<vec2<big_data>>("compare (vec2)", N);
    }

    VX_PROFILE_STOP();
    return 0;
}
