#pragma once

#include <string>

#include "vertex/system/compiler.hpp"

namespace vx {
namespace crypto {

class SHA256
{
public:

    VX_API SHA256();

    static inline std::string hash(const uint8_t* data, size_t size)
    {
        SHA256 sha256;
        sha256.update(data, size);
        sha256.finalize();
        return sha256.to_string();
    }

    VX_API void update(const uint8_t* data, size_t size);
    VX_API void finalize();
    VX_API std::string to_string() const;
    VX_API void reset();

private:

    void process_block(const uint8_t* block);
    void make_result();

    struct state
    {
        uint32_t a, b, c, d, e, f, g, h;
    };

    state m_state;                          // Hash state

    uint64_t m_bit_count;                   // Total number of bits in the buffer
    uint8_t m_buffer[64];                   // Input buffer
    size_t m_buffer_size;                   // Current buffer size

    bool m_finalized;                       // Is finalized
    uint8_t m_result[32];                   // Final result

};

} // namespace crypto
} // namespace vx