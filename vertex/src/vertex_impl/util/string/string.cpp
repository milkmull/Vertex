#include "vertex/util/string/string.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace str {

std::string to_hex_string(const void* data, size_t size)
{
    static constexpr char hex[] = "0123456789ABCDEF";

    std::string res;
    res.reserve(2 * size);

    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);

    for (size_t i = 0; i < size; ++i)
    {
        const char x1 = hex[(bytes[i] >> 4) & 0xF]; // High nibble
        const char x2 = hex[(bytes[i] >> 0) & 0xF]; // Low nibble

        res.push_back(x1);
        res.push_back(x2);
    }

    return res;
}

int32_t to_int32(const std::string& s, size_t* count, int base)
{
    int32_t value = 0;
    size_t i = 0;

    if (base < 2 || base > 36)
    {
        VX_ERR(err::INVALID_ARGUMENT) << "base should be between 2 and 36";
    }

    try
    {
        value = std::stol(s, &i, base);
    }
    catch (const std::invalid_argument&)
    {
        VX_ERR(err::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        err::set(err::OUT_OF_RANGE);
    }

    if (count)
    {
        *count = i;
    }

    return value;
}

int64_t to_int64(const std::string& s, size_t* count, int base)
{
    int64_t value = 0;
    size_t i = 0;

    if (base < 2 || base > 36)
    {
        VX_ERR(err::INVALID_ARGUMENT) << "base should be between 2 and 36";
    }

    try
    {
        value = std::stoll(s, &i, base);
    }
    catch (const std::invalid_argument&)
    {
        VX_ERR(err::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        err::set(err::OUT_OF_RANGE);
    }

    if (count)
    {
        *count = i;
    }

    return value;
}

uint32_t to_uint32(const std::string& s, size_t* count, int base)
{
    uint32_t value = 0;
    size_t i = 0;

    if (base < 2 || base > 36)
    {
        VX_ERR(err::INVALID_ARGUMENT) << "base should be between 2 and 36";
    }

    try
    {
        value = std::stoul(s, &i, base);
    }
    catch (const std::invalid_argument&)
    {
        VX_ERR(err::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        err::set(err::OUT_OF_RANGE);
    }

    if (count)
    {
        *count = i;
    }

    return value;
}

uint64_t to_uint64(const std::string& s, size_t* count, int base)
{
    uint64_t value = 0;
    size_t i = 0;

    if (base < 2 || base > 36)
    {
        VX_ERR(err::INVALID_ARGUMENT) << "base should be between 2 and 36";
    }

    try
    {
        value = std::stoull(s, &i, base);
    }
    catch (const std::invalid_argument&)
    {
        VX_ERR(err::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        err::set(err::OUT_OF_RANGE);
    }

    if (count)
    {
        *count = i;
    }

    return value;
}

float to_float(const std::string& s, size_t* count)
{
    float value = 0.0f;
    size_t i = 0;

    try
    {
        value = std::stof(s, &i);
    }
    catch (const std::invalid_argument&)
    {
        VX_ERR(err::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        err::set(err::OUT_OF_RANGE);
    }

    if (count)
    {
        *count = i;
    }

    return value;
}

double to_double(const std::string& s, size_t* count)
{
    double value = 0.0;
    size_t i = 0;

    try
    {
        value = std::stod(s, &i);
    }
    catch (const std::invalid_argument&)
    {
        VX_ERR(err::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        err::set(err::OUT_OF_RANGE);
    }

    if (count)
    {
        *count = i;
    }

    return value;
}

} // namespace str
} // namespace vx