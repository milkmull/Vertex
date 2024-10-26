#pragma once

#include "vertex/util/string/string.hpp"

namespace vx {
namespace str {

namespace _priv {

enum class token_type
{
    field,
    escaped_open,
    escaped_close
};

struct format_token
{
    format_token(token_type type, size_t off, size_t size)
        : type(type), off(off), size(size) {}

    format_token(token_type type, size_t off, size_t size, const char* out)
        : type(type), off(off), size(size), out(out) {}

    token_type type;

    size_t off;
    size_t size;

    size_t index = 0;
    std::string out;

    size_t specifier_off = 0;
    size_t specifier_size = 0;
};

bool parse_format_string(const std::string& fmt, std::vector<format_token>& stack);
std::string build_format_string(const std::string& fmt, const std::vector<format_token>& tokens);

template <size_t index>
inline void format_args(const std::string&, std::vector<format_token>&, std::ostringstream&) {}

template <size_t index, typename Arg, typename... Args>
inline void format_args(const std::string& fmt, std::vector<format_token>& tokens, std::ostringstream& oss, Arg&& arg, Args&&... args)
{
    for (format_token& token : tokens)
    {
        if (token.type == token_type::field && token.index == index)
        {
            if (token.specifier_off != 0)
            {
                // Extract the format specifier
                std::string specifier(1 + token.specifier_size, '%'); // Pre-fill with '%'

                std::copy(
                    fmt.data() + token.specifier_off,
                    fmt.data() + token.specifier_off + token.specifier_size,
                    specifier.data() + 1
                );

                // First, calculate the size needed for the formatted output
                int size = std::snprintf(nullptr, 0, specifier.c_str(), token.out.c_str());
                if (size < 0)
                {
                    VX_ERR(err::UNSUPPORTED_FORMAT) << "invalid format specifier: " << specifier;
                    return;
                }

                // Grow the output string to fit the new formatted data
                token.out.resize(size);

                // Write formatted data directly into the allocated space
                std::snprintf(
                    token.out.data(),
                    static_cast<size_t>(size + 1), // +1 for null terminator
                    specifier.c_str(),
                    std::forward<Arg>(arg)
                );
            }
            else
            {
                oss << std::forward<Arg>(arg);
                token.out = oss.str();
                oss.clear();
                oss.str(std::string());
            }
        }
    }

    format_args<index + 1>(fmt, tokens, oss, std::forward<Args>(args)...);
}

} // namespace _priv

template <typename... Args>
std::string format(const std::string& fmt, Args&&... args)
{
    std::vector<_priv::format_token> tokens;
    if (!_priv::parse_format_string(fmt, tokens))
    {
        return fmt;
    }

    std::ostringstream oss;
    _priv::format_args<0>(fmt, tokens, oss, std::forward<Args>(args)...);
    return build_format_string(fmt, tokens);
}

} // namespace str
} // namespace vx