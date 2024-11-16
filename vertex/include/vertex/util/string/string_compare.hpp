#pragma once

#include <string>
#include <algorithm>

namespace vx {
namespace str {

// https://en.wikipedia.org/wiki/Levenshtein_distance

template <typename IT1, typename IT2>
inline int32_t levenshtein_distance(IT1 first1, IT1 last1, IT2 first2, IT2 last2);

// https://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance

template <typename IT1, typename IT2>
inline int32_t damerau_levenshtein_distance(IT1 first1, IT1 last1, IT2 first2, IT2 last2);

} // namespace str
} // namespace vx

#include "vertex/util/string/string_compare.inl"