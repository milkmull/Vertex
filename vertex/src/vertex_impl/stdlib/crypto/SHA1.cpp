#include <sstream>
#include <iomanip>

#include "vertex/stdlib/crypto/SHA1.hpp"

namespace vx {
namespace crypto {

// https://www.boost.org/doc/libs/1_70_0/boost/uuid/detail/sha1.hpp
// https://en.wikipedia.org/wiki/SHA1
// http://www.zedwood.com/article/cpp-sha1-function
// https://datatracker.ietf.org/doc/html/rfc3174

static inline uint32_t left_rotate(uint32_t x, uint32_t n)
{
    return (x << n) | (x >> (32 - n));
}

SHA1::SHA1()
    : m_state{ 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 }
    , m_bit_count(0)
    , m_buffer{}
    , m_buffer_size(0)
    , m_finalized(false)
    , m_result{} {}

void SHA1::update(const uint8_t* data, size_t size)
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

void SHA1::finalize()
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
        m_buffer[56 + i] = static_cast<uint8_t>(m_bit_count >> (56 - i * 8));
    }

    process_block(m_buffer);
    make_result();

    m_finalized = true;
}

void SHA1::process_block(const uint8_t* block)
{
    uint32_t w[80]{};
    for (int i = 0; i < 16; ++i)
    {
        w[i] = (block[i * 4 + 0] << 24) | (block[i * 4 + 1] << 16) |
               (block[i * 4 + 2] <<  8) | (block[i * 4 + 3] <<  0);
    }
    for (int i = 16; i < 80; ++i)
    {
        w[i] = left_rotate(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
    }

    uint32_t a = m_state.a;
    uint32_t b = m_state.b;
    uint32_t c = m_state.c;
    uint32_t d = m_state.d;
    uint32_t e = m_state.e;

    for (int i = 0; i < 80; ++i)
    {
        uint32_t f, k;
        if (i < 20)
        {
            f = (b & c) | ((~b) & d);
            k = 0x5A827999;
        }
        else if (i < 40)
        {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1;
        }
        else if (i < 60)
        {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
        }
        else
        {
            f = b ^ c ^ d;
            k = 0xCA62C1D6;
        }

        uint32_t t = left_rotate(a, 5) + f + e + k + w[i];
        e = d;
        d = c;
        c = left_rotate(b, 30);
        b = a;
        a = t;
    }

    m_state.a += a;
    m_state.b += b;
    m_state.c += c;
    m_state.d += d;
    m_state.e += e;
}

void SHA1::make_result()
{
    for (int i = 0; i < 4; ++i)
    {
        m_result[i + 0 * 4] = static_cast<uint8_t>(m_state.a >> (24 - i * 8));
        m_result[i + 1 * 4] = static_cast<uint8_t>(m_state.b >> (24 - i * 8));
        m_result[i + 2 * 4] = static_cast<uint8_t>(m_state.c >> (24 - i * 8));
        m_result[i + 3 * 4] = static_cast<uint8_t>(m_state.d >> (24 - i * 8));
        m_result[i + 4 * 4] = static_cast<uint8_t>(m_state.e >> (24 - i * 8));
    }
}

std::string SHA1::to_string() const
{
    if (!m_finalized)
    {
        return "";
    }

    std::ostringstream oss;

    for (int i = 0; i < 20; ++i)
    {
        oss << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<int>(m_result[i]);
    }

    return oss.str();
}

void SHA1::clear()
{
    *this = SHA1();
}

}
}