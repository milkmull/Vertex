#pragma once

// Lightweight test harness for vx::format, organized as one flat array of
// test cases per argument category (int, float, bool, string, pointer, char).
//
// Rationale: within a single category the argument type is fixed (e.g. every
// integer-formatting test takes integer args), so there's no need for
// polymorphism/type-erasure. Each category is just:
//
//     static constexpr test_entry<int> int_tests[] = { ... };
//
// and a single templated run_tests<T>() drives all of them.

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>

#include "vertex/std/format.hpp"

using namespace vx;

//==============================================================================
// entry: one row per test, N args of type T
//==============================================================================

template <typename T, size_t N = 1, typename C = char>
struct test_entry
{
    const C* fmt;
    T args[N];
    const C* expected;
    fmt::format_error expected_err = fmt::format_error::none;
};

// convenience alias for the common single-arg case
template <typename T, typename C = char>
using single_test_entry = test_entry<T, 1, C>;
