#include <sstream>
#include <iomanip>

#include "vertex/stdlib/crypto/MD5.hpp"

namespace vx {
namespace crypto {

const uint32_t MD5::k[64] = {
    0xD76AA478, 0xE8C7B756, 0x242070DB, 0xC1BDCEEE,
    0xF57C0FAF, 0x4787C62A, 0xA8304613, 0xFD469501,
    0x698098D8, 0x8B44F7AF, 0xFFFF5BB1, 0x895CD7BE,
    0x6B901122, 0xFD987193, 0xA679438E, 0x49B40821,
    0xF61E2562, 0xC040B340, 0x265E5A51, 0xE9B6C7AA,
    0xD62F105D, 0x02441453, 0xD8A1E681, 0xE7D3FBC8,
    0x21E1CDE6, 0xC33707D6, 0xF4D50D87, 0x455A14ED,
    0xA9E3E905, 0xFCEFA3F8, 0x676F02D9, 0x8D2A4C8A,
    0xFFFA3942, 0x8771F681, 0x6D9D6122, 0xFDE5380C,
    0xA4BEEA44, 0x4BDECFA9, 0xF6BB4B60, 0xBEBFBC70,
    0x289B7EC6, 0xEAA127FA, 0xD4EF3085, 0x04881D05,
    0xD9D4D039, 0xE6DB99E5, 0x1FA27CF8, 0xC4AC5665,
    0xF4292244, 0x432AFF97, 0xAB9423A7, 0xFC93A039,
    0x655B59C3, 0x8F0CCC92, 0xFFEFF47D, 0x85845DD1,
    0x6FA87E4F, 0xFE2CE6E0, 0xA3014314, 0x4E0811A1,
    0xF7537E82, 0xBD3AF235, 0x2AD7D2BB, 0xEB86D391
};

const uint32_t MD5::r[64] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

uint32_t MD5::left_rotate(uint32_t x, uint32_t c)
{
    return (x << c) | (x >> (32 - c));
}

MD5::MD5()
    : m_state{ 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476 }
    , m_bit_count(0)
    , m_buffer{}
    , m_buffer_size(0)
    , m_finalized(false)
    , m_result{} {}

void MD5::update(const uint8_t* data, size_t size)
{
    if (m_finalized)
    {
        return;
    }

    m_bit_count += size * 8;

    for (size_t i = 0; i < size; ++i)
    {
        m_buffer[m_buffer_size++] = data[i];
        if (m_buffer_size == 64)
        {
            process_block(m_buffer);
            m_buffer_size = 0;
        }
    }
}

void MD5::finalize()
{
    if (m_finalized)
    {
        return;
    }

    // Append the padding bits and length
    m_buffer[m_buffer_size++] = 0x80;

    if (m_buffer_size > 56)
    {
        while (m_buffer_size < 64)
        {
            m_buffer[m_buffer_size++] = 0x00;
        }
        process_block(m_buffer);
        m_buffer_size = 0;
    }
    while (m_buffer_size < 56)
    {
        m_buffer[m_buffer_size++] = 0x00;
    }

    // Append the length in bits
    for (int i = 0; i < sizeof(m_bit_count); ++i)
    {
        m_buffer[56 + i] = static_cast<uint8_t>(m_bit_count >> (i * 8));
    }

    process_block(m_buffer);
    make_result();

    m_finalized = true;
}

void MD5::process_block(const uint8_t* block)
{
    uint32_t a = m_state.a, b = m_state.b, c = m_state.c, d = m_state.d, f, g;
    uint32_t w[16]{};

    for (int i = 0; i < 16; ++i)
    {
        w[i] = (block[i * 4 + 0] <<  0) | (block[i * 4 + 1] <<  8) |
               (block[i * 4 + 2] << 16) | (block[i * 4 + 3] << 24);
    }

    for (int i = 0; i < 64; ++i)
    {
        if (i < 16)
        {
            f = (b & c) | (~b & d);
            g = i;
        }
        else if (i < 32)
        {
            f = (d & b) | (~d & c);
            g = (5 * i + 1) % 16;
        }
        else if (i < 48)
        {
            f = b ^ c ^ d;
            g = (3 * i + 5) % 16;
        }
        else
        {
            f = c ^ (b | ~d);
            g = (7 * i) % 16;
        }

        uint32_t temp = d;
        d = c;
        c = b;
        b += left_rotate(a + f + k[i] + w[g], r[i]);
        a = temp;
    }

    m_state.a += a;
    m_state.b += b;
    m_state.c += c;
    m_state.d += d;
}

void MD5::make_result()
{
    for (int i = 0; i < 4; ++i)
    {
        m_result[i + 0 * 4] = static_cast<uint8_t>(m_state.a >> (i * 8));
        m_result[i + 1 * 4] = static_cast<uint8_t>(m_state.b >> (i * 8));
        m_result[i + 2 * 4] = static_cast<uint8_t>(m_state.c >> (i * 8));
        m_result[i + 3 * 4] = static_cast<uint8_t>(m_state.d >> (i * 8));
    }
}

std::string MD5::to_string() const
{
    if (!m_finalized)
    {
        return "";
    }

    std::ostringstream oss;

    for (int i = 0; i < 16; ++i)
    {
        oss << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<int>(m_result[i]);
    }

    return oss.str();
}

void MD5::clear()
{
    *this = MD5();
}

}
}