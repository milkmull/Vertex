#pragma once

#include <string>

#include "vertex/config/language_config.hpp"

namespace vx {
namespace crypto {

class SHA1
{
public:

    VX_API SHA1();

    static inline std::string hash(const uint8_t* data, size_t size)
    {
        SHA1 sha1;
        sha1.update(data, size);
        sha1.finalize();
        return sha1.to_string();
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
        uint32_t a, b, c, d, e;
    };

    state m_state;                          // Hash state

    uint64_t m_bit_count;                   // Total number of bits in the buffer
    uint8_t m_buffer[64];                   // Input buffer
    size_t m_buffer_size;                   // Current buffer size

    bool m_finalized;                       // Is finalized
    uint8_t m_result[20];                   // Final result

};

} // namespace crypto
} // namespace vx