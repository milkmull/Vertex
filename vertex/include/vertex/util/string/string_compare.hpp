#pragma once

#include <string>
#include <vector>

namespace vx {
namespace str {

// https://en.wikipedia.org/wiki/Levenshtein_distance

inline int32_t levenshtein_distance(const std::string& s1, const std::string& s2)
{
    const size_t size1 = s1.size();
    const size_t size2 = s2.size();

    // If one of the words is empty, the distance is equal to the size of the other word
    if (size1 == 0) return static_cast<int32_t>(size2);
    if (size2 == 0) return static_cast<int32_t>(size1);

    std::vector<std::vector<int32_t>> dp(size1 + 1, std::vector<int32_t>(size2 + 1));

    for (size_t i = 0; i <= size1; ++i)
    {
        dp[i][0] = static_cast<int32_t>(i); // Deletion
    }
    for (size_t j = 0; j <= size2; ++j)
    {
        dp[0][j] = static_cast<int32_t>(j); // Insertion
    }

    for (size_t i = 1; i <= size1; ++i)
    {
        for (size_t j = 1; j <= size2; ++j)
        {
            int32_t cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1; // Substitution cost

            dp[i][j] = std::min({
                dp[i - 1][j] + 1,       // Deletion
                dp[i][j - 1] + 1,       // Insertion
                dp[i - 1][j - 1] + cost // Substitution
            });
        }
    }

    // The bottom-right cell contains the distance
    return dp[size1][size2];
}

// https://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance

inline int32_t damerau_levenshtein_distance(const std::string& s1, const std::string& s2)
{
    const size_t size1 = s1.size();
    const size_t size2 = s2.size();

    // If one of the words is empty, the distance is equal to the size of the other word
    if (size1 == 0) return static_cast<int32_t>(size2);
    if (size2 == 0) return static_cast<int32_t>(size1);

    std::vector<std::vector<int32_t>> dp(size1 + 1, std::vector<int32_t>(size2 + 1));

    for (size_t i = 0; i <= size1; ++i)
    {
        dp[i][0] = static_cast<int32_t>(i); // Deletion
    }
    for (size_t j = 0; j <= size2; ++j)
    {
        dp[0][j] = static_cast<int32_t>(j); // Insertion
    }

    for (size_t i = 1; i <= size1; ++i)
    {
        for (size_t j = 1; j <= size2; ++j)
        {
            int32_t cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1; // Substitution cost

            dp[i][j] = std::min({
                dp[i - 1][j] + 1,       // Deletion
                dp[i][j - 1] + 1,       // Insertion
                dp[i - 1][j - 1] + cost // Substitution
            });

            if (i > 1 && j > 1 && s1[i - 1] == s2[j - 2] && s1[i - 2] == s2[j - 1])
            {
                dp[i][j] = std::min(dp[i][j], dp[i - 2][j - 2] + 1); // Transposition
            }
        }
    }

    // The bottom-right cell contains the distance
    return dp[size1][size2];
}

} // namespace str
} // namespace vx