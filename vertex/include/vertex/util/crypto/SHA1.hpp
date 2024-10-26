#pragma once

#include <string>

#include "vertex/core/compiler.hpp"

namespace vx {
namespace crypto {

class VX_API SHA1
{
public:

    SHA1();
    void update(const uint8_t* data, size_t size);
    void finalize();
    std::string to_string() const;

    void clear();

private:

    void process_block(const uint8_t* block);
    void make_result();

    struct state
    {
        uint32_t a, b, c, d, e;
    };

    state m_state;                          // Hash state

    uint64_t m_bit_count;                   // Total number of bits in the buffer
    uint8_t m_buffer[64];                   // Input buffer
    size_t m_buffer_size;                   // Current buffer size

    bool m_finalized;                       // Is finalized
    uint8_t m_result[20];                   // Final result

};

}
}