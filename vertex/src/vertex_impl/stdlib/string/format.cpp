#include "vertex/stdlib/string/format.hpp"
#include "vertex/stdlib/string/regex.hpp"

namespace vx {
namespace str {

VX_API std::string format(const std::string& fmt, const std::string* args, size_t count)
{
    std::string result;
    size_t i = 0;

    static regex re(R"(\{\s*(\d+)?\s*\})");
    regex_smatch m;

    auto it = fmt.begin();
    while (re.search(it, fmt.end(), m))
    {
        result.append(m.prefix());

        size_t index = i;

        if (m[1].matched)
        {
            index = str::to_uint64(m[1].str());
        }
        else
        {
            ++i;
        }

        if (index < count)
        {
            result.append(args[index]);
        }
        else
        {
            result.append(m.str());
        }

        it = m[0].second;
    }

    result.append(m.suffix());

    return result;
}

}
}