#pragma once

#include "vertex/config/language_config.hpp"

#include <string>
#if VX_HAVE_STD_STRING_VIEW
    #include <string_view>
#endif // VX_HAVE_STD_STRING_VIEW

namespace vx {
namespace str {

///////////////////////////////////////////////////////////////////////////////
// arg types
///////////////////////////////////////////////////////////////////////////////

#if VX_HAVE_STD_STRING_VIEW

using str_arg_t = std::string_view;
using wstr_arg_t = std::wstring_view;

template <typename char_t>
using basic_str_arg_t = std::basic_string_view<char_t>;

#else

using str_arg_t = std::string;
using wstr_arg_t = std::wstring;

template <typename char_t>
using basic_str_arg_t = std::basic_string<char_t>;

#endif // VX_HAVE_STD_STRING_VIEW

} // namespace str
} // namespace vx
