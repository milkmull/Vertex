#pragma once

#include <vector>

#include "vertex/util/string/string_cast.hpp"

namespace vx {
namespace str {

///////////////////////////////////////////////////////////////////////////////
// contains
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Checks if a character is present in the string.
 *
 * This function checks whether the specified character `val` exists
 * within the given string `s`.
 *
 * @param s The string to search within.
 * @param val The character to search for.
 * @return True if `val` is found in `s`, false otherwise.
 */
constexpr bool contains(const str_arg_t& s, const char val) noexcept;

/**
 * @brief Checks if a substring is present in the string.
 *
 * This function checks whether the specified substring `val` exists
 * within the given string `s`.
 *
 * @param s The string to search within.
 * @param val The substring to search for.
 * @return True if `val` is found in `s`, false otherwise.
 */
constexpr bool contains(const str_arg_t& s, const str_arg_t& val) noexcept;

///////////////////////////////////////////////////////////////////////////////
// count
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Counts occurrences of a character in the string.
 *
 * This function counts how many times the specified character `val`
 * appears in the string `s`.
 *
 * @param s The string to search within.
 * @param val The character to count.
 * @return The number of occurrences of `val` in `s`.
 */
inline size_t count(const str_arg_t& s, const char val);

/**
 * @brief Counts occurrences of a substring in the string.
 *
 * This function counts how many times the specified substring `val`
 * appears in the string `s`.
 *
 * @param s The string to search within.
 * @param val The substring to count.
 * @return The number of occurrences of `val` in `s`.
 */
inline size_t count(const str_arg_t& s, const str_arg_t& val);

///////////////////////////////////////////////////////////////////////////////
// starts_with
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Checks if the string starts with a specific character.
 *
 * This function checks if the given string `s` starts with the character `prefix`.
 *
 * @param s The string to check.
 * @param prefix The character to check for at the start of the string.
 * @return True if `s` starts with `prefix`, false otherwise.
 */
constexpr bool starts_with(const str_arg_t& s, const char prefix) noexcept;

/**
 * @brief Checks if the string starts with a specific substring.
 *
 * This function checks if the given string `s` starts with the substring `prefix`.
 *
 * @param s The string to check.
 * @param prefix The substring to check for at the start of the string.
 * @return True if `s` starts with `prefix`, false otherwise.
 */
constexpr bool starts_with(const str_arg_t& s, const str_arg_t& prefix) noexcept;

///////////////////////////////////////////////////////////////////////////////
// ends_with
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Checks if the string ends with a specific character.
 *
 * This function checks if the given string `s` ends with the character `suffix`.
 *
 * @param s The string to check.
 * @param suffix The character to check for at the end of the string.
 * @return True if `s` ends with `suffix`, false otherwise.
 */
constexpr bool ends_with(const str_arg_t& s, const char suffix) noexcept;

/**
 * @brief Checks if the string ends with a specific substring.
 *
 * This function checks if the given string `s` ends with the substring `suffix`.
 *
 * @param s The string to check.
 * @param suffix The substring to check for at the end of the string.
 * @return True if `s` ends with `suffix`, false otherwise.
 */
constexpr bool ends_with(const str_arg_t& s, const str_arg_t& suffix) noexcept;

///////////////////////////////////////////////////////////////////////////////
// alpha
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Checks if a character is alphabetic.
 *
 * This function checks if the specified character `c` is an alphabetic letter.
 *
 * @param c The character to check.
 * @return True if `c` is alphabetic, false otherwise.
 */
inline bool is_alpha(const char c);

/**
 * @brief Checks if a string consists only of alphabetic characters.
 *
 * This function checks if every character in the given string `s` is an
 * alphabetic letter.
 *
 * @param s The string to check.
 * @return True if all characters in `s` are alphabetic, false otherwise.
 */
inline bool is_alpha(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// numeric
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Checks if a character is a digit.
 *
 * This function checks if the specified character `c` is a numeric digit.
 *
 * @param c The character to check.
 * @return True if `c` is a digit, false otherwise.
 */
inline bool is_digit(const char c);

/**
 * @brief Checks if a string consists only of numeric characters.
 *
 * This function checks if every character in the given string `s` is a digit.
 *
 * @param s The string to check.
 * @return True if all characters in `s` are digits, false otherwise.
 */
inline bool is_numeric(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// alnum
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Checks if a character is alphanumeric.
 *
 * This function checks if the specified character `c` is alphanumeric (either a letter or a digit).
 *
 * @param c The character to check.
 * @return True if `c` is alphanumeric, false otherwise.
 */
inline bool is_alnum(const char c);

/**
 * @brief Checks if a string consists only of alphanumeric characters.
 *
 * This function checks if every character in the given string `s` is alphanumeric.
 *
 * @param s The string to check.
 * @return True if all characters in `s` are alphanumeric, false otherwise.
 */
inline bool is_alnum(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// ascii
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Checks if a character is an ASCII character.
 *
 * This function checks if the specified character `c` is an ASCII character (value < 128).
 *
 * @param c The character to check.
 * @return True if `c` is an ASCII character, false otherwise.
 */
inline bool is_ascii(const char c);

/**
 * @brief Checks if a string consists only of ASCII characters.
 *
 * This function checks if every character in the given string `s` is an ASCII character.
 *
 * @param s The string to check.
 * @return True if all characters in `s` are ASCII, false otherwise.
 */
inline bool is_ascii(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// space
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Checks if a character is a whitespace character.
 *
 * This function checks if the specified character `c` is a whitespace character.
 *
 * @param c The character to check.
 * @return True if `c` is a whitespace character, false otherwise.
 */
inline bool is_space(const char c);


/**
 * @brief Checks if a string consists only of whitespace characters.
 *
 * This function checks if every character in the given string `s` is a whitespace character.
 *
 * @param s The string to check.
 * @return True if all characters in `s` are whitespace, false otherwise.
 */
inline bool is_space(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// hex
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Checks if a character is a valid hexadecimal digit.
 *
 * This function checks if the specified character `c` is a valid hexadecimal
 * digit (0-9, A-F, a-f).
 *
 * @param c The character to check.
 * @return True if `c` is a valid hexadecimal digit, false otherwise.
 */
inline bool is_hex_digit(const char c);

/**
 * @brief Checks if a string represents a valid hexadecimal number.
 *
 * This function checks if the given string `s` is a valid hexadecimal number.
 * If `allow_prefix` is true, the string may optionally start with a '0x' or '0X' prefix.
 *
 * @param s The string to check.
 * @param allow_prefix If true, allows a '0x' or '0X' prefix in the string.
 * @return True if `s` is a valid hexadecimal number, false otherwise.
 */
inline bool is_hex(const str_arg_t& s, const bool allow_prefix = false);

///////////////////////////////////////////////////////////////////////////////
// lower
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Checks if a character is lowercase.
 *
 * This function checks if the specified character `c` is a lowercase letter.
 *
 * @param c The character to check.
 * @return True if `c` is lowercase, false otherwise.
 */
inline bool is_lower(const char c);

/**
 * @brief Checks if a string consists only of lowercase characters.
 *
 * This function checks if every character in the given string `s` is a lowercase letter.
 *
 * @param s The string to check.
 * @return True if all characters in `s` are lowercase, false otherwise.
 */
inline bool is_lower(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// to_lower
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Converts a character to lowercase.
 *
 * This function converts the specified character `c` to its lowercase equivalent.
 *
 * @param c The character to convert.
 * @return The lowercase version of `c`.
 */
inline char to_lower(const char c);

/**
 * @brief Converts a string to lowercase.
 *
 * This function converts every character in the string `s` to its lowercase equivalent.
 *
 * @param s The string to convert.
 * @return A new string with all characters in lowercase.
 */
inline std::string to_lower(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// upper
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Checks if a character is uppercase.
 *
 * This function checks if the specified character `c` is an uppercase letter.
 *
 * @param c The character to check.
 * @return True if `c` is uppercase, false otherwise.
 */
inline bool is_upper(const char c);

/**
 * @brief Checks if a string consists only of uppercase characters.
 *
 * This function checks if every character in the given string `s` is an uppercase letter.
 *
 * @param s The string to check.
 * @return True if all characters in `s` are uppercase, false otherwise.
 */
inline bool is_upper(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// to_upper
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Converts a character to uppercase.
 *
 * This function converts the specified character `c` to its uppercase equivalent.
 *
 * @param c The character to convert.
 * @return The uppercase version of `c`.
 */
inline char to_upper(const char c);

/**
 * @brief Converts a string to uppercase.
 *
 * This function converts every character in the string `s` to its uppercase equivalent.
 *
 * @param s The string to convert.
 * @return A new string with all characters in uppercase.
 */
inline std::string to_upper(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// title
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Converts the first character of each word in a string to uppercase and the rest to lowercase.
 *
 * This function transforms the input string `s` such that the first character of each word is uppercase
 * and all other characters in each word are lowercase. Words are separated by non-alphabetical characters.
 *
 * @param s The string to transform.
 * @return A new string where each word's first letter is uppercase and the rest are lowercase.
 */
inline std::string title(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// case_insensitive_compare
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Compares two characters for equality in a case-insensitive manner.
 *
 * This function compares two characters `c1` and `c2` for equality, ignoring case differences.
 *
 * @param c1 The first character to compare.
 * @param c2 The second character to compare.
 * @return True if `c1` and `c2` are equal ignoring case, false otherwise.
 */
inline bool case_insensitive_compare(const char c1, const char c2);

/**
 * @brief Compares two strings for equality in a case-insensitive manner.
 *
 * This function compares two strings `s1` and `s2` for equality, ignoring case differences.
 *
 * @param s1 The first string to compare.
 * @param s2 The second string to compare.
 * @return True if `s1` and `s2` are equal ignoring case, false otherwise.
 */
inline bool case_insensitive_compare(const str_arg_t& s1, const str_arg_t& s2);

///////////////////////////////////////////////////////////////////////////////
// strip
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Removes leading and trailing characters from a string.
 *
 * This function removes all characters found in `characters` from both the beginning and end of the input string `s`.
 * By default, it trims standard ASCII whitespace characters. The `off` parameter sets the offset to begin searching
 * from the start.
 *
 * @param s The input string to trim.
 * @param characters A string containing characters to be removed from both ends. Defaults to whitespace characters.
 * @param off Offset position to begin checking for leading characters. Defaults to 0.
 * @return A copy of the input string with specified characters removed from both ends.
 */
inline std::string strip(const str_arg_t& s, const str_arg_t& characters = str_arg_t(" \t\n\v\f\r"), size_t off = 0);

///////////////////////////////////////////////////////////////////////////////
// lstrip
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Removes leading characters from a string.
 *
 * This function removes all characters found in `characters` from the beginning of the input string `s`.
 * By default, it trims standard ASCII whitespace characters. The `off` parameter sets the offset to begin searching
 * from the start.
 *
 * @param s The input string to trim.
 * @param characters A string containing characters to be removed from the beginning. Defaults to whitespace characters.
 * @param off Offset position to begin checking for leading characters. Defaults to 0.
 * @return A copy of the input string with specified characters removed from the beginning.
 */
inline std::string lstrip(const str_arg_t& s, const str_arg_t& characters = str_arg_t(" \t\n\v\f\r"), size_t off = 0);

///////////////////////////////////////////////////////////////////////////////
// rstrip
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Removes trailing characters from a string.
 *
 * This function removes all characters found in `characters` from the end of the input string `s`.
 * By default, it trims standard ASCII whitespace characters. The `off` parameter specifies the last character
 * index to consider for trimming. Defaults to `str_arg_t::npos` (entire string).
 *
 * @param s The input string to trim.
 * @param characters A string containing characters to be removed from the end. Defaults to whitespace characters.
 * @param off Offset position from the end to begin checking for trailing characters. Defaults to `str_arg_t::npos`.
 * @return A copy of the input string with specified characters removed from the end.
 */
inline std::string rstrip(const str_arg_t& s, const str_arg_t& characters = str_arg_t(" \t\n\v\f\r"), size_t off = str_arg_t::npos);


///////////////////////////////////////////////////////////////////////////////
// trim_prefix
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Removes a specified prefix from a string, if present.
 *
 * If the input string `s` starts with `prefix`, the prefix is removed. Otherwise, the original string is returned.
 *
 * @param s The input string.
 * @param prefix The prefix to remove.
 * @return A new string with the prefix removed, if it was present.
 */
inline std::string trim_prefix(const str_arg_t& s, const str_arg_t& prefix);

///////////////////////////////////////////////////////////////////////////////
// trim_suffix
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Removes a specified suffix from a string, if present.
 *
 * If the input string `s` ends with `suffix`, the suffix is removed. Otherwise, the original string is returned.
 *
 * @param s The input string.
 * @param suffix The suffix to remove.
 * @return A new string with the suffix removed, if it was present.
 */
inline std::string trim_suffix(const str_arg_t& s, const str_arg_t& suffix);

///////////////////////////////////////////////////////////////////////////////
// remove
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Removes all occurrences of a character from a string.
 *
 * @param s The input string.
 * @param val The character to remove.
 * @return A copy of the string with all instances of `val` removed.
 */
inline std::string remove(const str_arg_t& s, const char val);

/**
 * @brief Removes all occurrences of a substring from a string.
 *
 * If the substring `val` is empty, the input string is returned unchanged.
 *
 * @param s The input string.
 * @param val The substring to remove.
 * @return A copy of the string with all instances of `val` removed.
 */
inline std::string remove(const str_arg_t& s, const str_arg_t& val);

///////////////////////////////////////////////////////////////////////////////
// replace
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Replaces all occurrences of one character with another.
 *
 * @param s The input string.
 * @param old_val The character to be replaced.
 * @param new_val The character to replace with.
 * @return A copy of the string with all `old_val` characters replaced by `new_val`.
 */
inline std::string replace(
    const str_arg_t& s,
    const char old_val,
    const char new_val
);

/**
 * @brief Replaces all occurrences of a substring with another substring.
 *
 * If `old_val` is empty, `new_val` is inserted between every character of the input string and at both ends.
 *
 * @param s The input string.
 * @param old_val The substring to be replaced.
 * @param new_val The substring to replace with.
 * @return A copy of the string with all instances of `old_val` replaced by `new_val`.
 */
inline std::string replace(
    const str_arg_t& s,
    const str_arg_t& old_val,
    const str_arg_t& new_val
);

///////////////////////////////////////////////////////////////////////////////
// concat
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Concatenates a range of elements into a single string.
 *
 * This function joins the elements in the range [`first`, `last`) without any delimiter.
 * Each element is inserted into an `std::ostringstream`, so the element type must support
 * `operator<<`.
 *
 * @tparam IT Input iterator type.
 * @param first Iterator to the beginning of the range.
 * @param last Iterator to the end of the range.
 * @return A string formed by concatenating the string representations of the elements.
 */
template <typename IT>
inline std::string concat(IT first, IT last);

///////////////////////////////////////////////////////////////////////////////
// join
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Joins a range of elements into a string, separated by a delimiter.
 *
 * This function joins the elements in the range [`first`, `last`) into a single string,
 * inserting `delimiter` between each pair of elements. Each element and the delimiter
 * must be streamable to `std::ostringstream` (i.e., support `operator<<`).
 *
 * @tparam IT Input iterator type.
 * @tparam Delim Type of the delimiter (string, char, etc.).
 * @param first Iterator to the beginning of the range.
 * @param last Iterator to the end of the range.
 * @param delimiter The value inserted between elements.
 * @return A string formed by joining the elements with the given delimiter.
 */
template <typename IT, typename Delim>
inline std::string join(IT first, IT last, const Delim& delimiter);

///////////////////////////////////////////////////////////////////////////////
// split
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Splits a string into substrings using a single character delimiter.
 *
 * @param s The input string to split.
 * @param delimiter The character used to delimit substrings.
 * @return A vector of substrings. If the input is empty, returns an empty vector.
 */
inline std::vector<str_arg_t> split(const str_arg_t& s, const char delimiter);

/**
 * @brief Splits a string into substrings using a string delimiter.
 *
 * @param s The input string to split.
 * @param delimiter The substring used to delimit substrings. If empty, splits `s` into individual characters.
 * @return A vector of substrings. If the input is empty, returns an empty vector.
 */
inline std::vector<str_arg_t> split(const str_arg_t& s, const str_arg_t& delimiter);

///////////////////////////////////////////////////////////////////////////////
// split_words
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Splits a string into words separated by whitespace characters.
 *
 * Whitespace characters include spaces, tabs, newlines, vertical tabs, form feeds, and carriage returns.
 * Consecutive whitespace is treated as a single separator.
 *
 * @param s The input string.
 * @return A vector of word substrings. If no words are found, returns an empty vector.
 */
inline std::vector<str_arg_t> split_words(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// split_lines
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Splits a string into lines based on newline (`'\n'`) characters.
 *
 * @param s The input string.
 * @return A vector of lines. If the input is empty, returns an empty vector.
 */
inline std::vector<str_arg_t> split_lines(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// repeat
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Repeats a character `n` times to form a string.
 *
 * @param s The character to repeat.
 * @param n The number of times to repeat the character.
 * @return A string consisting of `n` copies of `s`. If `n` is 0, returns an empty string.
 */
inline std::string repeat(const char s, size_t n);

/**
 * @brief Repeats a string `n` times to form a new string.
 *
 * @param s The string to repeat.
 * @param n The number of times to repeat the string.
 * @return A string consisting of `n` concatenated copies of `s`. If `n` is 0, returns an empty string.
 */
inline std::string repeat(const str_arg_t& s, size_t n);

///////////////////////////////////////////////////////////////////////////////
// reverse
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Returns a reversed copy of the input string.
 *
 * @param s The input string.
 * @return A new string with the characters of `s` in reverse order.
 */
inline std::string reverse(const str_arg_t& s);

///////////////////////////////////////////////////////////////////////////////
// Numeric Conversions
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Parses digit characters in a range into a numeric value.
 *
 * @tparam IT Iterator type.
 * @tparam T Numeric type to parse into.
 * @param first Iterator pointing to the start of the input range.
 * @param last Iterator pointing to the end of the input range.
 * @param value Output variable that receives the parsed value.
 * @param count Optional output pointer to receive the number of characters processed.
 * @return Iterator pointing to the first character after the parsed digits.
 */
template <typename IT, typename T>
inline IT parse_digits(IT first, IT last, T& value, size_t* count = nullptr);

/**
 * @brief Converts binary data to a hexadecimal string representation.
 *
 * This function takes a pointer to binary data (`data`) and its size (`size`), and
 * converts it into a string where each byte of the data is represented by two hexadecimal
 * characters. The resulting string is uppercase and has no separators between the bytes.
 *
 * @param data A pointer to the binary data to be converted.
 * @param size The size of the data in bytes.
 * @return A string containing the hexadecimal representation of the data.
 */
inline std::string to_hex_string(const void* data, size_t size);

/**
 * @brief Converts a string to a 64-bit signed integer.
 *
 * @param s The input string.
 * @param count Optional pointer to receive the number of characters processed.
 * @param base The numeric base (between 2 and 36).
 * @return The converted int64_t value. Returns 0 and sets an error on failure.
 */
inline int64_t to_int64(const std::string& s, size_t* count = nullptr, int base = 10);

/**
 * @brief Converts a string to a 32-bit signed integer.
 *
 * @param s The input string.
 * @param count Optional pointer to receive the number of characters processed.
 * @param base The numeric base (between 2 and 36).
 * @return The converted int32_t value. Returns 0 and sets an error on failure.
 */
inline int32_t to_int32(const std::string& s, size_t* count = nullptr, int base = 10);

/**
 * @brief Converts a string to a 64-bit unsigned integer.
 *
 * @param s The input string.
 * @param count Optional pointer to receive the number of characters processed.
 * @param base The numeric base (between 2 and 36).
 * @return The converted uint64_t value. Returns 0 and sets an error on failure.
 */
inline uint64_t to_uint64(const std::string& s, size_t* count = nullptr, int base = 10);

/**
 * @brief Converts a string to a 32-bit unsigned integer.
 *
 * @param s The input string.
 * @param count Optional pointer to receive the number of characters processed.
 * @param base The numeric base (between 2 and 36).
 * @return The converted uint32_t value. Returns 0 and sets an error on failure.
 */
inline uint32_t to_uint32(const std::string& s, size_t* count = nullptr, int base = 10);

/**
 * @brief Converts a string to a double-precision floating point number.
 *
 * @param s The input string.
 * @param count Optional pointer to receive the number of characters processed.
 * @return The converted double value. Returns 0.0 and sets an error on failure.
 */
inline double to_double(const std::string& s, size_t* count = nullptr);

/**
 * @brief Converts a string to a single-precision floating point number.
 *
 * @param s The input string.
 * @param count Optional pointer to receive the number of characters processed.
 * @return The converted float value. Returns 0.0f and sets an error on failure.
 */
inline float to_float(const std::string& s, size_t* count = nullptr);

} // namespace str
} // namespace vx

#include "string.inl"

#undef STD_STRING_VIEW_AVAILABLE