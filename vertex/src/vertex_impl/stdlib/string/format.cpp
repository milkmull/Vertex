#include "vertex/stdlib/string/format.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace str {

namespace _priv {

bool parse_format_string(const std::string& fmt, std::vector<format_token>& stack)
{
    const size_t size = fmt.size();
    format_token* token = nullptr;

    bool in_field = false;
    bool index_found = false;
    size_t arg_index = 0;

    for (size_t i = 0; i < size; ++i)
    {
        const char c = fmt[i];

        if (c == '{')
        {
            if (in_field)
            {
                VX_ERROR(error::error_code::INVALID_ARGUMENT) << "invalid { at position " << i;
                return false;
            }

            // Handle escaped '{{'
            if (i + 1 < size && fmt[i + 1] == '{')
            {
                stack.emplace_back(token_type::escaped_open, i++, 2, "{");
                continue;
            }

            stack.emplace_back(token_type::field, i, 0);
            token = &stack.back();
            in_field = true;

            continue;
        }
        if (c == '}')
        {
            if (!in_field)
            {
                // Handle escaped '}}'
                if (i + 1 < size && fmt[i + 1] == '}')
                {
                    stack.emplace_back(token_type::escaped_open, i++, 2, "}");
                    continue;
                }

                VX_ERROR(error::error_code::INVALID_ARGUMENT) << "unmatched } at position " << i;
                return false;
            }

            token->index = arg_index++;
            token->size = (i - token->off) + 1;
            in_field = false;
            index_found = false;

            continue;
        }
        if ('0' <= c && c <= '9')
        {
            if (in_field)
            {
                token->index = (token->index * 10) + static_cast<size_t>(c - '0');
                index_found = true;
            }

            continue;
        }
        
        if (in_field)
        {
            VX_ERROR(error::error_code::INVALID_ARGUMENT) << "invalid character " << c << " in field";
            return false;
        }
    }

    if (in_field)
    {
        VX_ERROR(error::error_code::INVALID_ARGUMENT) << "missing closing bracket";
        return false;
    }

    return true;
}

std::string build_format_string(const std::string& fmt, const std::vector<format_token>& tokens)
{
    std::string out;
    size_t i = 0;

    for (const format_token& token : tokens)
    {
        out.append(fmt.substr(i, token.off - i)).append(token.out);
        i = token.off + token.size;
    }

    out.append(fmt.substr(i));
    return out;
}

} // namespace _priv

}
}