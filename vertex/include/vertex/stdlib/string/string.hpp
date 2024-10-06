#pragma once

#include "vertex/system/compiler.hpp"

#if (VX_CPP_STANDARD == 17)

#   include <string_view>

#   define str_arg_t std::string_view
#   define wstr_arg_t std::wstring_view
#   define basic_str_arg_t std::basic_string_view 

#else

#   include <string>

#   define str_arg_t std::string
#   define wstr_arg_t std::wstring
#   define basic_str_arg_t std::basic_string

#endif

#include <vector>

namespace vx {
namespace str {

///////////////////////////////////////////////////////////////////////////////
// To/From String
///////////////////////////////////////////////////////////////////////////////

template <typename T>
inline std::string to_string(const T& value);

template <typename T>
inline T from_string(const std::string& s);

///////////////////////////////////////////////////////////////////////////////
// contains
///////////////////////////////////////////////////////////////////////////////

constexpr inline bool contains(const str_arg_t& s, const char val);
constexpr inline bool contains(const str_arg_t& s, const str_arg_t& val);

///////////////////////////////////////////////////////////////////////////////
// count
///////////////////////////////////////////////////////////////////////////////

inline size_t count(const str_arg_t& s, const char val);
inline size_t count(const str_arg_t& s, const str_arg_t& val);

///////////////////////////////////////////////////////////////////////////////
// starts_with
///////////////////////////////////////////////////////////////////////////////

constexpr inline bool starts_with(const str_arg_t& s, const char prefix);
constexpr inline bool starts_with(const str_arg_t& s, const str_arg_t& prefix);

///////////////////////////////////////////////////////////////////////////////
// ends_with
///////////////////////////////////////////////////////////////////////////////

constexpr inline bool ends_with(const str_arg_t& s, const char suffix);
constexpr inline bool ends_with(const str_arg_t& s, const str_arg_t& suffix);

///////////////////////////////////////////////////////////////////////////////
// alpha
///////////////////////////////////////////////////////////////////////////////

inline bool is_alpha(const char c);
inline bool is_alpha(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// numeric
///////////////////////////////////////////////////////////////////////////////

inline bool is_digit(const char c);
inline bool is_numeric(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// alnum
///////////////////////////////////////////////////////////////////////////////

inline bool is_alnum(const char c);
inline bool is_alnum(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// ascii
///////////////////////////////////////////////////////////////////////////////

inline bool is_ascii(const char c);
inline bool is_ascii(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// space
///////////////////////////////////////////////////////////////////////////////

inline bool is_space(const char c);
inline bool is_space(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// hex
///////////////////////////////////////////////////////////////////////////////

inline bool is_hex_digit(const char c);
inline bool is_hex(const str_arg_t& s);

inline std::string to_hex_string(const void* data, size_t size);

///////////////////////////////////////////////////////////////////////////////
// lower
///////////////////////////////////////////////////////////////////////////////

inline bool is_lower(const char c);
inline bool is_lower(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// to_lower
///////////////////////////////////////////////////////////////////////////////

inline char to_lower(const char c);
inline std::string to_lower(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// upper
///////////////////////////////////////////////////////////////////////////////

inline bool is_upper(const char c);
inline bool is_upper(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// to_upper
///////////////////////////////////////////////////////////////////////////////

inline char to_upper(const char c);
inline std::string to_upper(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// title
///////////////////////////////////////////////////////////////////////////////

inline std::string title(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// case_insensitive_compare
///////////////////////////////////////////////////////////////////////////////

inline bool case_insensitive_compare(const str_arg_t& s1, const str_arg_t& s2);

///////////////////////////////////////////////////////////////////////////////
// strip
///////////////////////////////////////////////////////////////////////////////

inline std::string strip(const str_arg_t& s, const str_arg_t& characters = str_arg_t(" \t\n\v\f\r"));

///////////////////////////////////////////////////////////////////////////////
// lstrip
///////////////////////////////////////////////////////////////////////////////

inline std::string lstrip(const str_arg_t& s, const str_arg_t& characters = str_arg_t(" \t\n\v\f\r"));

///////////////////////////////////////////////////////////////////////////////
// rstrip
///////////////////////////////////////////////////////////////////////////////

inline std::string rstrip(const str_arg_t& s, const str_arg_t& characters = str_arg_t(" \t\n\v\f\r"));

///////////////////////////////////////////////////////////////////////////////
// trim_prefix
///////////////////////////////////////////////////////////////////////////////

inline std::string trim_prefix(const str_arg_t& s, const str_arg_t& prefix);

///////////////////////////////////////////////////////////////////////////////
// trim_suffix
///////////////////////////////////////////////////////////////////////////////

inline std::string trim_suffix(const str_arg_t& s, const str_arg_t& suffix);

///////////////////////////////////////////////////////////////////////////////
// remove
///////////////////////////////////////////////////////////////////////////////

inline std::string remove(const str_arg_t& s, const char val);
inline std::string remove(const str_arg_t& s, const str_arg_t& val);

///////////////////////////////////////////////////////////////////////////////
// replace
///////////////////////////////////////////////////////////////////////////////

inline std::string replace(
    const str_arg_t& s,
    const char old_val,
    const char new_val
);

inline std::string replace(
    const str_arg_t& s,
    const str_arg_t& old_val,
    const str_arg_t& new_val
);

///////////////////////////////////////////////////////////////////////////////
// join
///////////////////////////////////////////////////////////////////////////////

template <typename IT>
inline std::string join(IT first, IT last, const str_arg_t& delimiter = str_arg_t());

///////////////////////////////////////////////////////////////////////////////
// join_path
///////////////////////////////////////////////////////////////////////////////

template <typename IT>
inline std::string join_path(IT first, IT last);

inline std::string join_path(const char* s1, const char* s2);
inline std::string join_path(const char* s1, const std::string& s2);
inline std::string join_path(const std::string& s1, const char* s2);
inline std::string join_path(const std::string& s1, const std::string& s2);

#if (VX_CPP_STANDARD == 17)

inline std::string join_path(const str_arg_t& s1, const str_arg_t& s2);

#endif

///////////////////////////////////////////////////////////////////////////////
// split
///////////////////////////////////////////////////////////////////////////////

inline std::vector<std::string> split(const str_arg_t& s, const char delimiter);
inline std::vector<std::string> split(const str_arg_t& s, const str_arg_t& delimiter);

///////////////////////////////////////////////////////////////////////////////
// split_any
///////////////////////////////////////////////////////////////////////////////

inline std::vector<std::string> split_any(const str_arg_t& s, const str_arg_t& characters);

///////////////////////////////////////////////////////////////////////////////
// split_words
///////////////////////////////////////////////////////////////////////////////

inline std::vector<std::string> split_words(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// split_lines
///////////////////////////////////////////////////////////////////////////////

inline std::vector<std::string> split_lines(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// split_path
///////////////////////////////////////////////////////////////////////////////

inline std::vector<std::string> split_path(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// repeat
///////////////////////////////////////////////////////////////////////////////

inline std::string repeat(const char s, size_t n);
inline std::string repeat(const str_arg_t& s, size_t n);

///////////////////////////////////////////////////////////////////////////////
// reverse
///////////////////////////////////////////////////////////////////////////////

inline std::string reverse(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// wstring
///////////////////////////////////////////////////////////////////////////////

inline std::wstring string_to_wstring(const str_arg_t& s);
inline std::string wstring_to_string(const wstr_arg_t& ws);

///////////////////////////////////////////////////////////////////////////////
// Numeric Conversions
///////////////////////////////////////////////////////////////////////////////

template <typename IT, typename T>
inline IT parse_digits(IT first, IT last, T& value, size_t* count = nullptr);

inline int32_t to_int32(const std::string& s, size_t* count = nullptr, int base = 10);
inline int64_t to_int64(const std::string& s, size_t* count = nullptr, int base = 10);

inline uint32_t to_uint32(const std::string& s, size_t* count = nullptr, int base = 10);
inline uint64_t to_uint64(const std::string& s, size_t* count = nullptr, int base = 10);

inline float to_float(const std::string& s, size_t* count = nullptr);
inline double to_double(const std::string& s, size_t* count = nullptr);

}
}

#include "string.inl"

#undef  str_arg_t
#undef wstr_arg_t