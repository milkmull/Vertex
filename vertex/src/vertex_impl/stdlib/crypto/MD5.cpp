#include <sstream>
#include <iomanip>

#include "vertex/stdlib/crypto/MD5.hpp"

namespace vx {
namespace crypto {

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define FF(a, b, c, d, x, s, k) (a) = (ROTATE_LEFT((a) + F((b), (c), (d)) + (x) + (k), (s)) + (b))
#define GG(a, b, c, d, x, s, k) (a) = (ROTATE_LEFT((a) + G((b), (c), (d)) + (x) + (k), (s)) + (b))
#define HH(a, b, c, d, x, s, k) (a) = (ROTATE_LEFT((a) + H((b), (c), (d)) + (x) + (k), (s)) + (b))
#define II(a, b, c, d, x, s, k) (a) = (ROTATE_LEFT((a) + I((b), (c), (d)) + (x) + (k), (s)) + (b))

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
    uint32_t a = m_state.a, b = m_state.b, c = m_state.c, d = m_state.d;
    uint32_t x[16]{};

    for (int i = 0; i < 16; ++i)
    {
        x[i] = (block[i * 4 + 0] <<  0) | (block[i * 4 + 1] <<  8) |
               (block[i * 4 + 2] << 16) | (block[i * 4 + 3] << 24);
    }

    /* Round 1 */
    FF(a, b, c, d, x[ 0], S11, 0xD76AA478); /* 1 */
    FF(d, a, b, c, x[ 1], S12, 0xE8C7B756); /* 2 */
    FF(c, d, a, b, x[ 2], S13, 0x242070DB); /* 3 */
    FF(b, c, d, a, x[ 3], S14, 0xC1BDCEEE); /* 4 */
    FF(a, b, c, d, x[ 4], S11, 0xF57C0FAF); /* 5 */
    FF(d, a, b, c, x[ 5], S12, 0x4787C62A); /* 6 */
    FF(c, d, a, b, x[ 6], S13, 0xA8304613); /* 7 */
    FF(b, c, d, a, x[ 7], S14, 0xFD469501); /* 8 */
    FF(a, b, c, d, x[ 8], S11, 0x698098D8); /* 9 */
    FF(d, a, b, c, x[ 9], S12, 0x8B44F7AF); /* 10 */
    FF(c, d, a, b, x[10], S13, 0xFFFF5BB1); /* 11 */
    FF(b, c, d, a, x[11], S14, 0x895CD7BE); /* 12 */
    FF(a, b, c, d, x[12], S11, 0x6B901122); /* 13 */
    FF(d, a, b, c, x[13], S12, 0xFD987193); /* 14 */
    FF(c, d, a, b, x[14], S13, 0xA679438E); /* 15 */
    FF(b, c, d, a, x[15], S14, 0x49B40821); /* 16 */

    /* Round 2 */
    GG(a, b, c, d, x[ 1], S21, 0xF61E2562); /* 17 */
    GG(d, a, b, c, x[ 6], S22, 0xC040B340); /* 18 */
    GG(c, d, a, b, x[11], S23, 0x265E5A51); /* 19 */
    GG(b, c, d, a, x[ 0], S24, 0xE9B6C7AA); /* 20 */
    GG(a, b, c, d, x[ 5], S21, 0xD62F105D); /* 21 */
    GG(d, a, b, c, x[10], S22, 0x2441453); /* 22 */
    GG(c, d, a, b, x[15], S23, 0xD8A1E681); /* 23 */
    GG(b, c, d, a, x[ 4], S24, 0xE7D3FBC8); /* 24 */
    GG(a, b, c, d, x[ 9], S21, 0x21E1CDE6); /* 25 */
    GG(d, a, b, c, x[14], S22, 0xC33707D6); /* 26 */
    GG(c, d, a, b, x[ 3], S23, 0xF4D50D87); /* 27 */
    GG(b, c, d, a, x[ 8], S24, 0x455A14ED); /* 28 */
    GG(a, b, c, d, x[13], S21, 0xA9E3E905); /* 29 */
    GG(d, a, b, c, x[ 2], S22, 0xFCEFA3F8); /* 30 */
    GG(c, d, a, b, x[ 7], S23, 0x676F02D9); /* 31 */
    GG(b, c, d, a, x[12], S24, 0x8D2A4C8A); /* 32 */

    /* Round 3 */
    HH(a, b, c, d, x[ 5], S31, 0xFFFA3942); /* 33 */
    HH(d, a, b, c, x[ 8], S32, 0x8771F681); /* 34 */
    HH(c, d, a, b, x[11], S33, 0x6D9D6122); /* 35 */
    HH(b, c, d, a, x[14], S34, 0xFDE5380C); /* 36 */
    HH(a, b, c, d, x[ 1], S31, 0xA4BEEA44); /* 37 */
    HH(d, a, b, c, x[ 4], S32, 0x4BDECFA9); /* 38 */
    HH(c, d, a, b, x[ 7], S33, 0xF6BB4B60); /* 39 */
    HH(b, c, d, a, x[10], S34, 0xBEBFBC70); /* 40 */
    HH(a, b, c, d, x[13], S31, 0x289B7EC6); /* 41 */
    HH(d, a, b, c, x[ 0], S32, 0xEAA127FA); /* 42 */
    HH(c, d, a, b, x[ 3], S33, 0xD4EF3085); /* 43 */
    HH(b, c, d, a, x[ 6], S34, 0x04881D05); /* 44 */
    HH(a, b, c, d, x[ 9], S31, 0xD9D4D039); /* 45 */
    HH(d, a, b, c, x[12], S32, 0xE6DB99E5); /* 46 */
    HH(c, d, a, b, x[15], S33, 0x1FA27CF8); /* 47 */
    HH(b, c, d, a, x[ 2], S34, 0xC4AC5665); /* 48 */

    /* Round 4 */
    II(a, b, c, d, x[ 0], S41, 0xF4292244); /* 49 */
    II(d, a, b, c, x[ 7], S42, 0x432AFF97); /* 50 */
    II(c, d, a, b, x[14], S43, 0xAB9423A7); /* 51 */
    II(b, c, d, a, x[ 5], S44, 0xFC93A039); /* 52 */
    II(a, b, c, d, x[12], S41, 0x655B59C3); /* 53 */
    II(d, a, b, c, x[ 3], S42, 0x8F0CCC92); /* 54 */
    II(c, d, a, b, x[10], S43, 0xFFEFF47D); /* 55 */
    II(b, c, d, a, x[ 1], S44, 0x85845DD1); /* 56 */
    II(a, b, c, d, x[ 8], S41, 0x6FA87E4F); /* 57 */
    II(d, a, b, c, x[15], S42, 0xFE2CE6E0); /* 58 */
    II(c, d, a, b, x[ 6], S43, 0xA3014314); /* 59 */
    II(b, c, d, a, x[13], S44, 0x4E0811A1); /* 60 */
    II(a, b, c, d, x[ 4], S41, 0xF7537E82); /* 61 */
    II(d, a, b, c, x[11], S42, 0xBD3AF235); /* 62 */
    II(c, d, a, b, x[ 2], S43, 0x2AD7D2BB); /* 63 */
    II(b, c, d, a, x[ 9], S44, 0xEB86D391); /* 64 */

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