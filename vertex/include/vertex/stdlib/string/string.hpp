#pragma once

#include <string>

namespace vx {
namespace str {

///////////////////////////////////////////////////////////////////////////////
// To/From String
///////////////////////////////////////////////////////////////////////////////

template <typename T>
inline std::string to_string(T value);

template <typename T>
inline T from_string(const std::string& s);

///////////////////////////////////////////////////////////////////////////////
// Contains
///////////////////////////////////////////////////////////////////////////////

inline bool contains(const std::string& s, const char c);
inline bool contains(const std::string& s1, const std::string& s2);

///////////////////////////////////////////////////////////////////////////////
// Count
///////////////////////////////////////////////////////////////////////////////

inline size_t count(const std::string& s, const char c);
inline size_t count(const std::string& s1, const std::string& s2);

///////////////////////////////////////////////////////////////////////////////
// Starts/Ends With
///////////////////////////////////////////////////////////////////////////////

inline bool starts_with(const std::string& s, const char value);
inline bool starts_with(const std::string& s, const std::string& value);

inline bool ends_with(const std::string& s, const char value);
inline bool ends_with(const std::string& s, const std::string& value);

///////////////////////////////////////////////////////////////////////////////
// Boolean Checks
///////////////////////////////////////////////////////////////////////////////

inline bool is_alpha(const char c);
inline bool is_alpha(const std::string& s);

inline bool is_digit(const char c);
inline bool is_numeric(const std::string& s);

inline bool is_alnum(const char c);
inline bool is_alnum(const std::string& s);

inline bool is_ascii(const char c);
inline bool is_ascii(const std::string& s);

inline bool is_space(const char c);
inline bool is_space(const std::string& s);

inline bool is_printable(const char c);
inline bool is_printable(const std::string& s);

///////////////////////////////////////////////////////////////////////////////
// Hex
///////////////////////////////////////////////////////////////////////////////

inline bool is_hex_digit(const char c);
inline bool is_hex(const std::string& s);

inline std::string to_hex_string(const uint8_t* data, size_t size);

///////////////////////////////////////////////////////////////////////////////
// Upper/Lower
///////////////////////////////////////////////////////////////////////////////

inline bool is_lower(const char c);
inline bool is_lower(const std::string& s);

inline bool is_upper(const char c);
inline bool is_upper(const std::string& s);

inline char to_lower(const char c);
inline std::string to_lower(const std::string& s);

inline char to_upper(const char c);
inline std::string to_upper(const std::string& s);

inline std::string title(const std::string& s);

inline bool case_insensitive_compare(const std::string& s1, const std::string& s2);

///////////////////////////////////////////////////////////////////////////////
// Strip
///////////////////////////////////////////////////////////////////////////////

inline std::string strip(const std::string& s);
inline std::string strip(const std::string& s, const std::string& characters);

inline std::string lstrip(const std::string& s);
inline std::string lstrip(const std::string& s, const std::string& characters);

inline std::string rstrip(const std::string& s);
inline std::string rstrip(const std::string& s, const std::string& characters);

///////////////////////////////////////////////////////////////////////////////
// Trim Perfix/Suffix
///////////////////////////////////////////////////////////////////////////////

inline std::string trim_prefix(const std::string& s, const std::string& prefix);
inline std::string trim_suffix(const std::string& s, const std::string& suffix);

///////////////////////////////////////////////////////////////////////////////
// Replace
///////////////////////////////////////////////////////////////////////////////

inline std::string replace(const std::string& s, const std::string old_val, const std::string& new_val);

///////////////////////////////////////////////////////////////////////////////
// Join
///////////////////////////////////////////////////////////////////////////////

template <typename IT>
inline std::string join(IT first, IT last);

template <typename IT>
inline std::string join(IT first, IT last, const std::string& delimiter);

template <typename IT>
inline std::string join_path(IT first, IT last);

inline std::string join_path(const std::string& s1, const std::string& s2);

///////////////////////////////////////////////////////////////////////////////
// Split
///////////////////////////////////////////////////////////////////////////////

inline std::vector<std::string> split(const std::string& s, const std::string& delimiter);
inline std::vector<std::string> split_any(const std::string& s, const std::string& characters);
inline std::vector<std::string> split_words(const std::string& s);
inline std::vector<std::string> split_lines(const std::string& s);
inline std::vector<std::string> split_path(const std::string& s);

///////////////////////////////////////////////////////////////////////////////
// Repeat
///////////////////////////////////////////////////////////////////////////////

inline std::string repeat(const char c, size_t n);
inline std::string repeat(const std::string& s, size_t n);

///////////////////////////////////////////////////////////////////////////////
// Reverse
///////////////////////////////////////////////////////////////////////////////

inline std::string reverse(const std::string& s);

///////////////////////////////////////////////////////////////////////////////
// wstring
///////////////////////////////////////////////////////////////////////////////

inline std::wstring string_to_wstring(const std::string& s);
inline std::string wstring_to_string(const std::wstring& ws);

///////////////////////////////////////////////////////////////////////////////
// Numeric Conversions
///////////////////////////////////////////////////////////////////////////////

inline int32_t to_int32(const std::string& s, size_t* count = nullptr, int base = 10);
inline int64_t to_int64(const std::string& s, size_t* count = nullptr, int base = 10);

inline uint32_t to_uint32(const std::string& s, size_t* count = nullptr, int base = 10);
inline uint64_t to_uint64(const std::string& s, size_t* count = nullptr, int base = 10);

inline float to_float(const std::string& s, size_t* count = nullptr);
inline double to_double(const std::string& s, size_t* count = nullptr);

}
}

#include "string.inl"