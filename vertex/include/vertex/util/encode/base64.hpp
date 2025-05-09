#pragma once

#include <vector>
#include <string>

#include "vertex/config/language_config.hpp"

namespace vx {
namespace base64 {

/**
 * @brief Encodes binary data into a Base64-encoded string.
 *
 * This function converts the input binary buffer into a Base64 string representation.
 * It processes input in 3-byte blocks and appends padding ('=') as necessary.
 *
 * @param data Pointer to the binary data buffer to encode. Must not be null.
 * @param size The number of bytes in the input buffer.
 * @param encoded Output string that will be populated with the Base64 result. Cleared and overwritten on success.
 * @return True if encoding was successful, false if input is null.
 *
 * @note Sets vx::err::INVALID_ARGUMENT if `data` is null.
 * @note The output string will be cleared and resized as needed.
 */
VX_API bool encode(const uint8_t* data, size_t size, std::string& encoded);

/**
 * @brief Decodes a Base64-encoded string into binary data.
 *
 * This function parses a Base64-encoded input string and converts it into raw binary data.
 * Optionally validates that input contains only valid Base64 characters.
 *
 * @param encoded Base64-encoded input string.
 * @param data Output vector that will hold the decoded binary data. Cleared and overwritten on success.
 * @param validate If true, invalid characters will cause decoding to fail and return false.
 * @return True if decoding was successful and input was valid; false otherwise.
 *
 * @note Returns false and sets vx::err::SIZE_ERROR if input size is not a multiple of 4.
 * @note Returns false and sets vx::err::INVALID_ARGUMENT if `validate` is true and input contains invalid characters.
 */
VX_API bool decode(const std::string& encoded, std::vector<uint8_t>& data, bool validate = false);

}
}