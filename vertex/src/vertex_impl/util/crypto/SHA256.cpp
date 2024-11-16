#include <sstream>
#include <iomanip>

#include "vertex/util/crypto/SHA256.hpp"

namespace vx {
namespace crypto {

// https://en.wikipedia.org/wiki/SHA256
// http://www.zedwood.com/article/cpp-sha256-function
// https://datatracker.ietf.org/doc/html/rfc3174

#define SHFR(x, n)    (x >> n)
#define ROTR(x, n)   ((x >> n) | (x << (32 - n)))
#define ROTL(x, n)   ((x << n) | (x >> (32 - n)))
#define CH(x, y, z)  ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#define F1(x) (ROTR(x,  2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define F2(x) (ROTR(x,  6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define F3(x) (ROTR(x,  7) ^ ROTR(x, 18) ^ SHFR(x,  3))
#define F4(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))

static const uint32_t k[64] = {
    0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5,
    0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
    0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
    0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
    0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC,
    0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
    0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
    0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
    0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
    0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
    0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3,
    0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
    0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5,
    0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
    0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
    0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
};

VX_API SHA256::SHA256()
    : m_state{ 0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A, 
               0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19 }
    , m_bit_count(0)
    , m_buffer{}
    , m_buffer_size(0)
    , m_finalized(false)
    , m_result{} {}

VX_API void SHA256::update(const uint8_t* data, size_t size)
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

VX_API void SHA256::finalize()
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

void SHA256::process_block(const uint8_t* block)
{
    uint32_t w[64]{};

    for (int i = 0; i < 16; ++i)
    {
        w[i] = (block[i * 4 + 0] << 24) | (block[i * 4 + 1] << 16) |
               (block[i * 4 + 2] <<  8) | (block[i * 4 + 3] <<  0);
    }
    for (int i = 16; i < 64; ++i)
    {
        w[i] = F4(w[i - 2]) + w[i - 7] + F3(w[i - 15]) + w[i - 16];
    }

    uint32_t a = m_state.a;
    uint32_t b = m_state.b;
    uint32_t c = m_state.c;
    uint32_t d = m_state.d;
    uint32_t e = m_state.e;
    uint32_t f = m_state.f;
    uint32_t g = m_state.g;
    uint32_t h = m_state.h;

    for (int i = 0; i < 64; ++i)
    {
        uint32_t t1 = h + F2(e) + CH(e, f, g) + k[i] + w[i];
        uint32_t t2 = F1(a) + MAJ(a, b, c);

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    m_state.a += a;
    m_state.b += b;
    m_state.c += c;
    m_state.d += d;
    m_state.e += e;
    m_state.f += f;
    m_state.g += g;
    m_state.h += h;
}

void SHA256::make_result()
{
    for (int i = 0; i < 4; ++i)
    {
        m_result[i + 0 * 4] = static_cast<uint8_t>(m_state.a >> (24 - i * 8));
        m_result[i + 1 * 4] = static_cast<uint8_t>(m_state.b >> (24 - i * 8));
        m_result[i + 2 * 4] = static_cast<uint8_t>(m_state.c >> (24 - i * 8));
        m_result[i + 3 * 4] = static_cast<uint8_t>(m_state.d >> (24 - i * 8));
        m_result[i + 4 * 4] = static_cast<uint8_t>(m_state.e >> (24 - i * 8));
        m_result[i + 5 * 4] = static_cast<uint8_t>(m_state.f >> (24 - i * 8));
        m_result[i + 6 * 4] = static_cast<uint8_t>(m_state.g >> (24 - i * 8));
        m_result[i + 7 * 4] = static_cast<uint8_t>(m_state.h >> (24 - i * 8));
    }
}

VX_API std::string SHA256::to_string() const
{
    if (!m_finalized)
    {
        return std::string();
    }

    std::ostringstream oss;

    for (int i = 0; i < 32; ++i)
    {
        oss << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<int>(m_result[i]);
    }

    return oss.str();
}

VX_API void SHA256::reset()
{
    *this = SHA256();
}

} // namespace crypto
} // namespace vx