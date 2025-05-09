#pragma once

#include <string>
#include <algorithm>

namespace vx {
namespace str {

// https://en.wikipedia.org/wiki/Levenshtein_distance

/**
 * @brief Computes the Levenshtein distance between two sequences.
 *
 * This function computes the minimum number of single-character edits (insertions, deletions, or substitutions)
 * required to change one sequence into the other.
 *
 * @tparam IT1 Iterator type for the first sequence.
 * @tparam IT2 Iterator type for the second sequence.
 * @param first1 Iterator pointing to the beginning of the first sequence.
 * @param last1 Iterator pointing to the end of the first sequence.
 * @param first2 Iterator pointing to the beginning of the second sequence.
 * @param last2 Iterator pointing to the end of the second sequence.
 * @return The Levenshtein distance as an int32_t.
 */
template <typename IT1, typename IT2>
inline int32_t levenshtein_distance(IT1 first1, IT1 last1, IT2 first2, IT2 last2);

// https://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance

/**
 * @brief Computes the Damerau-Levenshtein distance between two sequences.
 *
 * This function extends the Levenshtein distance by also allowing transpositions of adjacent characters.
 *
 * @tparam IT1 Iterator type for the first sequence.
 * @tparam IT2 Iterator type for the second sequence.
 * @param first1 Iterator pointing to the beginning of the first sequence.
 * @param last1 Iterator pointing to the end of the first sequence.
 * @param first2 Iterator pointing to the beginning of the second sequence.
 * @param last2 Iterator pointing to the end of the second sequence.
 * @return The Damerau-Levenshtein distance as an int32_t.
 */
template <typename IT1, typename IT2>
inline int32_t damerau_levenshtein_distance(IT1 first1, IT1 last1, IT2 first2, IT2 last2);

} // namespace str
} // namespace vx

#include "vertex/util/string/string_compare.inl"