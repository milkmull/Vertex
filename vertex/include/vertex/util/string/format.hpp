#pragma once

#include "vertex/util/string/string.hpp"

namespace vx {
namespace str {

template <typename... Args>
inline std::string format(const std::string& fmt, Args&&... args);

} // namespace str
} // namespace vx

#include "vertex/util/string/format.inl"