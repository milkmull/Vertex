#pragma once

#include "vertex/stdlib/string/string.hpp"

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
            oss << std::forward<Arg>(arg);
            token.out = oss.str();
            oss.clear();
            oss.str(std::string());
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

}
}