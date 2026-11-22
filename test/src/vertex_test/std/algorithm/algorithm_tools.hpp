#pragma once

// Shared helpers for the algorithm tests.
//
// Deliberately independent of the code under test (own comparators instead of
// vx::less / vx::greater) so a bug in vx::functional can't hide a bug in an
// algorithm, or vice versa.

#include <cstddef>
#include <initializer_list>
#include <vector>

#include "vertex_test/test_util.hpp"

namespace vx {
namespace test {

//=============================================================================
// comparators
//=============================================================================

struct less_int
{
    constexpr bool operator()(int a, int b) const
    {
        return a < b;
    }
};

struct greater_int
{
    constexpr bool operator()(int a, int b) const
    {
        return a > b;
    }
};

// Counts comparisons. Usage: counting_less{ &count }
struct counting_less
{
    std::size_t* count;

    bool operator()(int a, int b) const
    {
        ++(*count);
        return a < b;
    }
};

//=============================================================================
// keyed element (for stability / tie-breaking checks)
//=============================================================================

// Ordered by `key` only; `id` tells equivalent elements apart.
struct keyed_item
{
    int key;
    int id;
};

struct by_key
{
    bool operator()(const keyed_item& a, const keyed_item& b) const
    {
        return a.key < b.key;
    }
};

//=============================================================================
// input generation
//=============================================================================

// Calls fn(const std::vector<int>&) for every sequence of length 0..max_len
// whose values are drawn from [0, alphabet). A small alphabet produces many
// ties. Total calls: sum over len of alphabet^len, so keep both small
// (e.g. max_len 7, alphabet 3 -> 3280 sequences).
template <typename F>
void for_each_sequence(std::size_t max_len, int alphabet, F&& fn)
{
    for (std::size_t len = 0; len <= max_len; ++len)
    {
        std::vector<int> v(len, 0);

        while (true)
        {
            fn(v);

            std::size_t i = 0;
            while (i < len && ++v[i] == alphabet)
            {
                v[i] = 0;
                ++i;
            }

            if (i == len)
            {
                break;
            }
        }
    }
}

// Builds a vector<movable_int> without triggering a reallocation (which would
// move-construct and poison the source elements mid-build).
inline std::vector<movable_int> make_movable_vector(std::initializer_list<int> values)
{
    std::vector<movable_int> v;
    v.reserve(values.size());

    for (int x : values)
    {
        v.emplace_back(x);
    }

    return v;
}

// True if no element has been moved from (movable_int / copyable_int set
// their value to -1 when moved from, so this assumes -1 never appears in
// valid test data).
template <typename Container>
bool none_moved_from(const Container& c)
{
    for (const auto& x : c)
    {
        if (static_cast<int>(x) == -1)
        {
            return false;
        }
    }

    return true;
}

} // namespace test
} // namespace vx
