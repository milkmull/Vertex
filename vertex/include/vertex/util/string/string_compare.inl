#pragma once

#include <vector>

#include "vertex/util/string/string_compare.hpp"
#include "vertex/system/validate.hpp"

namespace vx {
namespace str {

namespace __detail {

template <typename IT1, typename IT2>
inline int32_t levenshtein_distance(IT1 first1, IT1 last1, IT2 first2, IT2 last2, const bool damerau)
{
    VX_ITERATOR_VALID_RANGE(first1, last1);
    VX_ITERATOR_VALID_RANGE(first2, last2);

    const size_t size1 = static_cast<size_t>(std::distance(first1, last1));
    const size_t size2 = static_cast<size_t>(std::distance(first2, last2));

    // If one of the words is empty, the distance is equal to the size of the other word
    if (size1 == 0) return static_cast<int32_t>(size2);
    if (size2 == 0) return static_cast<int32_t>(size1);

    std::vector<std::vector<int32_t>> dp(size1 + 1, std::vector<int32_t>(size2 + 1));

    for (size_t i = 0; i <= size1; ++i)
    {
        // Deletion
        dp[i][0] = static_cast<int32_t>(i);
    }
    for (size_t j = 0; j <= size2; ++j)
    {
        // Insertion
        dp[0][j] = static_cast<int32_t>(j);
    }

    for (size_t i = 1; i <= size1; ++i)
    {
        for (size_t j = 1; j <= size2; ++j)
        {
            // Substitution cost
            const int32_t cost = (*std::next(first1, i - 1) == *std::next(first2, j - 1)) ? 0 : 1;

            dp[i][j] = std::min({
                // Deletion
                dp[i - 1][j] + 1,
                // Insertion
                dp[i][j - 1] + 1,
                // Substitution
                dp[i - 1][j - 1] + cost
            });

            if (damerau &&
                (i > 1 && j > 1 && 
                (*std::next(first1, i - 1) == *std::next(first2, j - 2)) &&
                (*std::next(first1, i - 2) == *std::next(first2, j - 1))))
            {
                dp[i][j] = std::min(dp[i][j], dp[i - 2][j - 2] + 1); // Transposition
            }
        }
    }

    // The bottom-right cell contains the distance
    return dp[size1][size2];
}

} // namespace __detail

// https://en.wikipedia.org/wiki/Levenshtein_distance

template <typename IT1, typename IT2>
inline int32_t levenshtein_distance(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    return __detail::levenshtein_distance(first1, last1, first2, last2, false);
}

// https://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance

template <typename IT1, typename IT2>
inline int32_t damerau_levenshtein_distance(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    return __detail::levenshtein_distance(first1, last1, first2, last2, true);
}

} // namespace str
} // namespace vx