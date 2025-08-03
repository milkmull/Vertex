#pragma once

#include "vertex/config/language_config.hpp"

#include <string>
#if (VX_CPP_STANDARD >= 17)
#   include <string_view>
#endif // (VX_CPP_STANDARD >= 17)

namespace vx {
namespace str {

///////////////////////////////////////////////////////////////////////////////
// arg types
///////////////////////////////////////////////////////////////////////////////

#if defined(__cpp_lib_string_view)

using str_arg_t = std::string_view;
using wstr_arg_t = std::wstring_view;

template <typename char_t>
using basic_str_arg_t = std::basic_string_view<char_t>;

#else

using str_arg_t = std::string;
using wstr_arg_t = std::wstring;

template <typename char_t>
using basic_str_arg_t = std::basic_string<char_t>;

#endif  // __cpp_lib_string_view

} // namespace str
} // namespace vx