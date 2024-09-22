#pragma once

#include <string>

namespace vx {
namespace crypto {

class MD5
{
private:

    static const uint32_t k[64];
    static const uint32_t r[64];

    static uint32_t left_rotate(uint32_t x, uint32_t c);

public:

    MD5();
    void update(const uint8_t* data, size_t size);
    void finalize();
    std::string to_string() const;

    void clear();

private:

    void process_block(const uint8_t* block);
    void make_result();

    struct state
    {
        uint32_t a, b, c, d;
    };

    state m_state;                          // Hash state

    uint64_t m_bit_count;                   // Total number of bits in the buffer
    uint8_t m_buffer[64];                   // Input buffer
    size_t m_buffer_size;                   // Current buffer size

    bool m_finalized;                       // Is finalized
    uint8_t m_result[16];                   // Final result

};

}
}