#pragma once

#include <string>

#include "vertex/config/language_config.hpp"

namespace vx {
namespace crypto {

class MD5
{
public:

    VX_API MD5();

    /**
     * @brief Computes the MD5 hash of a data buffer in one call.
     *
     * @param data Pointer to the input data buffer.
     * @param size Size of the buffer in bytes.
     * @return A 32-character hexadecimal string representing the MD5 hash.
     */
    static inline std::string hash(const uint8_t* data, size_t size)
    {
        MD5 md5;
        md5.update(data, size);
        md5.finalize();
        return md5.to_string();
    }

    /**
     * @brief Adds data to the hash computation.
     *
     * May be called multiple times before `finalize()` to hash data incrementally.
     *
     * @param data Pointer to the input data.
     * @param size Size of the data in bytes.
     */
    VX_API void update(const uint8_t* data, size_t size);

    /**
     * @brief Finalizes the hash computation.
     *
     * After calling this function, no further updates are allowed unless `reset()` is called.
     */
    VX_API void finalize();

    /**
     * @brief Returns the final MD5 hash as a 32-character hexadecimal string.
     *
     * @return The hash as a `std::string`. Undefined if `finalize()` has not been called.
     */
    VX_API std::string to_string() const;

    /**
     * @brief Resets the internal state to begin a new hash computation.
     */
    VX_API void reset();

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

} // namespace crypto
} // namespace vx