#pragma once

#include <string>

namespace vx {
namespace str {

// https://en.wikipedia.org/wiki/Levenshtein_distance

int32_t levenshtein_distance(const std::string& s1, const std::string& s2);

// https://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance

int32_t damerau_levenshtein_distance(const std::string& s1, const std::string& s2);

} // namespace str
} // namespace vx