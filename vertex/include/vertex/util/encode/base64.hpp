#pragma once

#include <vector>
#include <string>

#include "vertex/config/language_config.hpp"

namespace vx {
namespace base64 {

 ///////////////////////////////////////////////////////////////////////////////
 /// @brief Encodes binary data to a Base64-encoded string.
 ///
 /// This function takes binary data and encodes it into a Base64 string.
 /// The resulting encoded string is stored in the provided 'encoded' parameter.
 ///
 /// @param data Pointer to the binary data to be encoded.
 /// @param size The size of the binary data in bytes.
 /// @param encoded Reference to a string where the encoded Base64 data will be
 /// stored.
 ///
 /// @return 'true' if the encoding is successful, 'false' if there is an
 /// invalid argument.
 ///////////////////////////////////////////////////////////////////////////////
VX_API bool encode(const uint8_t* data, size_t size, std::string& encoded);

///////////////////////////////////////////////////////////////////////////////
/// @brief Decodes a Base64-encoded string into binary data.
///
/// This function decodes a Base64-encoded string back into its original binary
/// form. The resulting binary data is stored in the provided 'decoded'
/// parameter. An optional validation step can be performed to ensure the input
/// is valid Base64.
///
/// @param data The Base64-encoded string to decode.
/// @param data A reference to a vector where the decoded bytes will be stored.
/// @param validate Optional parameter to enable validation of the Base64
/// string.
///
/// @return 'true' if the decoding is successful, 'false' if there is an error
/// in size or invalid argument.
///////////////////////////////////////////////////////////////////////////////
VX_API bool decode(const std::string& encoded, std::vector<uint8_t>& data, bool validate = false);

}
}