#pragma once

#include <string>

#include "vertex/config/language_config.hpp"

namespace vx {
namespace crypto {

class SHA256
{
public:

    VX_API SHA256();

    /**
     * @brief Computes the SHA-256 hash of a data buffer in one call.
     *
     * @param data Pointer to the input data buffer.
     * @param size Size of the buffer in bytes.
     * @return A 64-character hexadecimal string representing the SHA-256 hash.
     */
    static inline std::string hash(const uint8_t* data, size_t size)
    {
        SHA256 sha256;
        sha256.update(data, size);
        sha256.finalize();
        return sha256.to_string();
    }

    /**
     * @brief Adds data to the SHA-256 hash computation.
     *
     * This function can be called multiple times to hash data incrementally.
     *
     * @param data Pointer to the data to hash.
     * @param size Size of the data in bytes.
     */
    VX_API void update(const uint8_t* data, size_t size);

    /**
     * @brief Finalizes the SHA-256 hash computation.
     *
     * Once finalized, the internal state is sealed and cannot be updated unless reset.
     */
    VX_API void finalize();

    /**
     * @brief Returns the final SHA-256 hash as a 64-character hexadecimal string.
     *
     * @return The SHA-256 digest as a `std::string`. Must call `finalize()` before use.
     */
    VX_API std::string to_string() const;

    /**
     * @brief Resets the internal state to begin a new SHA-256 computation.
     */
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